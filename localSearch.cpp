#include "localSearch.hpp"
#include "greedyState.hpp"
#include <map>
#include <cmath>
#include <random>

Solution LocalSearch::run(uint32_t numIterations, uint32_t numNodesToDelete) {
	currentSolution = bestSolution;

	Solution solutionCandidate = Solution(currentSolution);
	for (uint32_t i = 0; i < numIterations; i++) {
		solutionCandidate = strategy->removeNodes(hypergraph, solutionCandidate, numNodesToDelete);
		solutionCandidate = strategy->repairPartialSolution(hypergraph, solutionCandidate);

		if (isAcceptable(solutionCandidate)) {
			bestSolution = solutionCandidate;
			currentSolution = solutionCandidate;
		}
	}

	return bestSolution;
}

bool LocalSearch::isAcceptable(Solution& solutionCandidate) {
	if (solutionCandidate.size() <= currentSolution.size()) {
		return true;
	}
	return false;
}


// TODO

void LocalSearch::initializePotential() {
	for (Node node : bestSolution.solutionSet) {
		solutionPotential += potential(node);
	}
}

void LocalSearch::updatePotential(const std::vector<Node>& removedNodes, const std::vector<Node>& newNodes) {
	for (Node node : removedNodes) {
		currentSolutionPotential -= potential(node);
	}
	for (Node node : newNodes) {
		currentSolutionPotential += potential(node);
	}
}

double LocalSearch::potential(Node node) {
	return harmonicApproximation(hypergraph.getIncidentEdgeIndizes(node).size()); // weight of each node is 1, so that gets ommitted
}

double LocalSearch::harmonicApproximation(uint32_t k) {
	const double gamma = 0.57721566490153286060;
	return std::log(k) + gamma + 1.0 / (2 * k) - 1.0 / (12.0 * k * k);
}
