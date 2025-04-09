#include "lplocalsearch.hpp"
#include "solution.hpp"
#include <random>
#include <optional>

Solution LPLocalSearch::removeNodes(Hypergraph& hypergraph, Solution& newPartialSolution, uint32_t numNodesToRemove = 2) {
	// Build sampling distribution
	std::vector<double> inverseLPValues;
	std::vector<Node> nodes;
	inverseLPValues.reserve(fractionalLPSolution.size());
	nodes.reserve(fractionalLPSolution.size());
	for (const auto& [node, value] : fractionalLPSolution) {
		if (newPartialSolution.contains(node)) {
			double inverseValue = 1 - fractionalLPSolution[node];
			if (inverseValue > 0.0) {
				inverseLPValues.push_back(inverseValue);
				nodes.push_back(node);
			}
		}
	}

	// Sample nodes to remove
	std::random_device rd;
	std::mt19937 gen(rd());
	std::discrete_distribution samplingDistribution = std::discrete_distribution(inverseLPValues.begin(), inverseLPValues.end());
	std::vector<Node> nodesToRemove;
	for (uint32_t i = 0; i < numNodesToRemove; i++) {
		uint32_t nodeToRemove = nodes[samplingDistribution(gen)];
		if (std::find(nodesToRemove.begin(), nodesToRemove.end(), nodeToRemove) == nodesToRemove.end()) {
			nodesToRemove.push_back(nodeToRemove);
		}
	}

	// Remove nodes
	for (Node node : nodesToRemove) {
		newPartialSolution.erase(node);
	}

	return newPartialSolution;
}

Solution LPLocalSearch::repairPartialSolution(Hypergraph& hypergraph, Solution& partialSolution) {
	if (!hypergraph.isSolvedBy(partialSolution)) {
		GreedyState state = GreedyState(hypergraph, {});
		state.setSolution(partialSolution);
		partialSolution = state.calculateSolution();
	}
	return partialSolution;
}