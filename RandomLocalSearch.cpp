#include "randomLocalSearch.hpp"
#include "greedyState.hpp"
#include <cmath>
#include <random>

std::vector<Node> RandomLocalSearch::removeNodes(uint32_t numNodesToRemove = 2) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<std::size_t> dist(0, algorithmState->solution.solutionVector.size() - 1);
	uint32_t sampleIndex = 0;
	Node nodeToRemove;
	bool removed;
	std::vector<Node> removedNodes;
	for (uint32_t i = 0; i < numNodesToRemove; i++) {
		do {
			dist = std::uniform_int_distribution<std::size_t>(0, algorithmState->solution.solutionVector.size() - 1);
			sampleIndex = dist(gen);
		} while (sampleIndex >= algorithmState->solution.solutionVector.size() && algorithmState->solution.solutionVector.size() > 0);
		if (algorithmState->solution.solutionVector.size() == 0) {
			break;
		}
		nodeToRemove = algorithmState->solution.solutionVector[sampleIndex];
		removed = algorithmState->removeFromSolution(nodeToRemove);
		if (removed) {
			removedNodes.push_back(nodeToRemove);
		}
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

void RandomLocalSearch::setSolution(Solution& solution) {
	algorithmState->setSolution(solution);
}

Solution& RandomLocalSearch::getSolution() {
	return algorithmState->getSolution();
}

bool RandomLocalSearch::isSolved() {
	return algorithmState->isSolved();
}

bool RandomLocalSearch::isSolvedBy(Solution& solution) {
	return algorithmState->hypergraph.isSolvedBy(solution);
}