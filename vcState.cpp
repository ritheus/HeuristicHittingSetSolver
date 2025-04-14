#include "vcState.hpp"
#include "kernelization.hpp"
#include "greedyState.hpp"
#include <glpk.h>
#include <random>

VCState::VCState(Hypergraph& hypergraph, const cxxopts::ParseResult& optionsResult) : AlgorithmState(hypergraph, optionsResult) {}
VCState::VCState(NumNodes n, NumEdges m, std::vector<Edge>&& setSystem, const cxxopts::ParseResult& optionsResult) : AlgorithmState(n, m, std::move(setSystem), optionsResult) {}

Solution VCState::calculateSolution(bool applyKernelization) {
	if (applyKernelization) {
		kernelization::applyKernelization(*this, optionsResult);
	}
	double lambda = 2.0;
	numIterations = optionsResult["vc_numIterations"].as<uint32_t>();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0.0, lambda);
	std::unordered_map<Node, double> fractionalSolution = buildAndSolveLP();
	std::vector<Node> nodesToDelete;
	std::vector<Node> currentSolutionNodes;
	std::vector<Node> bestSolutionNodes;
	nodesToDelete.reserve(fractionalSolution.size());

	for (const auto& [node, value] : fractionalSolution) {
		if (isApproximately(value, 0.0)) {
			nodesToDelete.push_back(node);
		}
	}
	for (Node node : nodesToDelete) {
		fractionalSolution.erase(node);
	}
	deleteNodes(nodesToDelete);

	for (uint32_t i = 0; i < numIterations; i++) {
		for (const auto& [node, value] : fractionalSolution) {
			if (isApproximately(value, 1.0)) {
				currentSolutionNodes.push_back(node);
			}
			else if (value >= 1 / lambda) {
				currentSolutionNodes.push_back(node);
			}
			else if (value < 1 / lambda) {
				if (dist(gen) < value * lambda) {
					currentSolutionNodes.push_back(node);
				}
			}
			if (hypergraph.isSolved()) {
				break;
			}
		}
		if (currentSolutionNodes.size() < bestSolutionNodes.size() || bestSolutionNodes.empty()) {
			bestSolutionNodes = currentSolutionNodes;
		}
		currentSolutionNodes.clear();
	}

	for (Node node : bestSolutionNodes) {
		addToSolution(node);
	}

	for (EdgeIndex edgeIndex = 0; edgeIndex < hypergraph.getEdges().size(); edgeIndex++) {
		if (!hypergraph.isEdgeHit(edgeIndex)) {
			std::uniform_int_distribution<> dist(0, hypergraph.getEdge(edgeIndex).size() - 1);
			Node node = hypergraph.getEdge(edgeIndex)[dist(gen)];
			addToSolution(node);
		}
	}

	orderedFractionalSolution = std::map<Node, double>(fractionalSolution.begin(), fractionalSolution.end());

	return solution;
}

bool VCState::isApproximately(double a, double b, double epsilon) {
	return std::abs(a - b) < epsilon;
}

std::unordered_map<Node, double> VCState::buildAndSolveLP() {
	std::vector<Node> nodes;
	for (uint32_t i = 1; i < hypergraph.getNodes().size(); i++) {
		nodes.push_back(hypergraph.getNodes()[i]);
	}

	std::unordered_map<Node, uint32_t> nodeToColIndex;
	for (uint32_t i = 0; i < nodes.size(); i++) {
		nodeToColIndex[nodes[i]] = i+1;
	}

	glp_prob* lp = glp_create_prob();
	glp_set_obj_dir(lp, GLP_MIN);

	glp_add_cols(lp, nodes.size());
	for (Node node : nodes) { // glpk is 1-indexed, as are the nodes
		glp_set_col_bnds(lp, nodeToColIndex[node], GLP_DB, 0.0, 1.0); // 0 <= x_i <= 1
		glp_set_obj_coef(lp, nodeToColIndex[node], 1.0); // min sum x_i
	}

	glp_add_rows(lp, hypergraph.getEdges().size());

	std::vector<int> ia, ja; // sparse matrix representation
	std::vector<double> ar;

	ia.push_back(0); ja.push_back(0); ar.push_back(0);  // 1-indexed

	for (EdgeIndex edgeIndex = 0; edgeIndex < hypergraph.getEdges().size(); edgeIndex++) {
		const Edge& edge = hypergraph.getEdge(edgeIndex);
		glp_set_row_bnds(lp, edgeIndex + 1, GLP_LO, 1.0, 0.0); // sum >= 1
		for (Node node : edge) {
			ia.push_back(edgeIndex + 1); // 1-indexed
			ja.push_back(nodeToColIndex[node]);
			ar.push_back(1);
		}
	}

	glp_load_matrix(lp, ia.size() - 1, ia.data(), ja.data(), ar.data());

	glp_term_out(GLP_OFF);
	glp_simplex(lp, NULL);
	std::unordered_map<Node, double> fractionalSolution(hypergraph.getNodes().size());
	for (const auto& [node, index] : nodeToColIndex) {
		fractionalSolution[node] = glp_get_col_prim(lp, index);
	}
	glp_delete_prob(lp);

	return fractionalSolution;
}

void VCState::addToSolution(Node node) {
	if (!solution.contains(node)) {
		updateNodeAge(node);
		solution.insert(node);
		for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
			hypergraph.setEdgeHit(edgeIndex);
		}
	}
}

std::map<Node, double> VCState::getOrderedFractionalSolution() {
	return orderedFractionalSolution;
}

void VCState::deleteNodes(const std::vector<Node>& nodesToDelete) {
	hypergraph.deleteNodes(nodesToDelete);
}

void VCState::deleteEdges(std::vector<EdgeIndex>& edgesToDelete) {
	hypergraph.deleteEdges(edgesToDelete);
}