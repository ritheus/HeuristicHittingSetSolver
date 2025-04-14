#include "randomLocalSearch.hpp"
#include "greedyState.hpp"
#include <cmath>
#include <random>

Solution RandomLocalSearch::removeNodes(Hypergraph& hypergraph, Solution& newPartialSolution, uint32_t numNodesToRemove = 2) {
	// Build currentSolutionVector -> Worth if numNodesToRemove is large
	currentSolutionVector.clear();
	for (Node node : newPartialSolution.getSolution()) {
		currentSolutionVector.push_back(node);
	}

	// Remove nodes
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, currentSolutionVector.size() - 1);
	uint32_t sampleIndex = 0;
	for (uint32_t i = 0; i < numNodesToRemove; i++) {
		do {
			sampleIndex = dist(gen);
		} while (sampleIndex >= currentSolutionVector.size());
		currentSolutionVector[sampleIndex] = currentSolutionVector[currentSolutionVector.size() - 1];
		currentSolutionVector.erase(currentSolutionVector.end() - 1);
	}

	// Rebuild solution from currentSolutionVector
	newPartialSolution = Solution();
	for (Node node : currentSolutionVector) {
		newPartialSolution.insert(node);
	}

	return newPartialSolution;
}

Solution RandomLocalSearch::repairPartialSolution(Hypergraph& hypergraph, Solution& partialSolution) {
	if (!hypergraph.isSolvedBy(partialSolution)) {
		GreedyState state = GreedyState(hypergraph, {});
		state.setNodeAges(nodeAges);
		state.setHighestAge(highestAge);
		state.setSolution(partialSolution);
		partialSolution = state.calculateSolution();
		nodeAges = state.nodeAges;
		highestAge = state.highestAge;
	}
	return partialSolution;
}