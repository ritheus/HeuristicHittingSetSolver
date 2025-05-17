#include "randomLocalSearch.hpp"
#include "greedyState.hpp"
#include <cmath>
#include <random>

std::vector<Node> RandomLocalSearch::removeNodes(uint32_t numNodesToRemove = 2) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, algorithmState->solution.solutionVector.size() - 1);
	uint32_t sampleIndex = 0;
	Node nodeToRemove;
	std::vector<Node> removedNodes;
	for (uint32_t i = 0; i < numNodesToRemove; i++) {
		do {
			sampleIndex = dist(gen);
		} while (sampleIndex >= algorithmState->solution.solutionVector.size());
		nodeToRemove = algorithmState->solution.solutionVector[sampleIndex];
		algorithmState->removeFromSolution(nodeToRemove);
		removedNodes.push_back(nodeToRemove);
	}

	return removedNodes;
}

std::vector<Node> RandomLocalSearch::repairPartialSolution() {
	return algorithmState->repairSolution();
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