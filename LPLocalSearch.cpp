#include "lplocalsearch.hpp"
#include "solution.hpp"
#include <random>
#include <optional>

Solution LPLocalSearch::removeNodes(Hypergraph& hypergraph, Solution& newPartialSolution, uint32_t numNodesToRemove = 2) {
	// Build sampling distribution
	std::map<Node, double> samplingDistribution;
	double sum = 0.0;
	for (const auto& [node, value] : fractionalLPSolution) {
		if (newPartialSolution.contains(node)) {
			double inverseValue = 1 - fractionalLPSolution[node];
			if (inverseValue > 0.0) {
				sum += inverseValue;
				samplingDistribution[node] = sum;
			}
		}
	}

	// Remove node
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0.0, sum);
	for (uint32_t i = 0; i < numNodesToRemove; i++) { // this can be done much faster for larger numNodesToRemove (binary search, multiple samples at once, ...)
		double sampleValue = dist(gen);
		std::optional<std::pair<Node, double>> prev;
		double prevValue = 0.0;
		Node prevNode = 0;
		for (const auto& [node, value] : samplingDistribution) {
			if (sampleValue <= value) {
				newPartialSolution.erase(prevNode);
				samplingDistribution.erase(prevNode);
				break;
			}
			prevValue = value;
			prevNode = node;
		}
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