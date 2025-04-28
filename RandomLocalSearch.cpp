#include "randomLocalSearch.hpp"
#include "greedyState.hpp"
#include <cmath>
#include <random>

void RandomLocalSearch::removeNodes(uint32_t numNodesToRemove = 2) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, algorithmState->solution.solutionVector.size() - 1);
	uint32_t sampleIndex = 0;
	Node nodeToRemove;
	for (uint32_t i = 0; i < numNodesToRemove; i++) {
		do {
			sampleIndex = dist(gen);
		} while (sampleIndex >= algorithmState->solution.solutionVector.size());
		nodeToRemove = algorithmState->solution.solutionVector[sampleIndex];
		algorithmState->removeFromSolution(nodeToRemove);
	}
}

void RandomLocalSearch::repairPartialSolution() {
	algorithmState->calculateSolution();
	//if (!hypergraph.isSolvedBy(partialSolution)) {
		//state.setNodeAges(nodeAges);
		//state.setHighestAge(highestAge);
		//partialSolution = algorithmState->calculateSolution();
		//nodeAges = state.nodeAges;
		//highestAge = state.highestAge;
	//}
}

void RandomLocalSearch::initializeAlgorithmState(std::unique_ptr<AlgorithmState> state) {
	algorithmState = std::move(state);
}