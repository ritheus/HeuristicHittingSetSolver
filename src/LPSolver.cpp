#include  "LPSolver.hpp"

#include "hypergraph.hpp"
#include <glpk.h>

std::unordered_map<Node, double> buildAndSolveLP(Hypergraph& hypergraph) {
	std::vector<Node> nodes;
	for (uint32_t i = 1; i < hypergraph.getNodes().size(); i++) {
		nodes.push_back(hypergraph.getNodes()[i]);
	}

	std::unordered_map<Node, uint32_t> nodeToColIndex;
	for (uint32_t i = 0; i < nodes.size(); i++) {
		nodeToColIndex[nodes[i]] = i + 1;
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

std::map<Node, double> getOrderedFractionalSolution(Hypergraph& hypergraph) {
	std::unordered_map<Node, double> fractionalSolution = buildAndSolveLP(hypergraph);
	std::map<Node, double> orderedFractionalSolution(fractionalSolution.begin(), fractionalSolution.end());
	return orderedFractionalSolution;
}