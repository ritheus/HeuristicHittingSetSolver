#include "vcState.hpp"
#include "kernelization.hpp"
#include "greedyState.hpp"
#include "LPSolver.hpp"
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
	std::unordered_map<Node, double> fractionalSolution = buildAndSolveLP(hypergraph);
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

void VCState::addToSolution(Node node) {
	if (!solution.contains(node)) {
		updateNodeAge(node);
		solution.insert(node);
		for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
			hypergraph.incrementHitCount(edgeIndex);
		}
	}
}

void VCState::removeFromSolution(Node node) {
	if (solution.contains(node)) {
		solution.erase(node);
		for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
			hypergraph.decrementHitCount(edgeIndex);
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