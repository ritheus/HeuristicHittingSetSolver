#include "localSearch.hpp"
#include "greedyState.hpp"
#include "logger.hpp"
#include <string>
#include <map>
#include <cmath>
#include <random>

Solution LocalSearch::run(uint32_t numIterations, uint32_t minNumNodesToDelete, uint32_t maxNumNodesToDelete = 0, std::string neighborhoodStyle = "flat") {
	uint32_t loggingInterval = 10;

	strategy->initializeAlgorithmState(std::move(state));
	for (uint32_t i = 0; i < numIterations; i++) {
		strategy->removeNodes(minNumNodesToDelete);
		strategy->repairPartialSolution();
		Solution& solutionCandidate = strategy->algorithmState->getSolution();
		if (isAcceptable(solutionCandidate)) {
			bestSolution = solutionCandidate;
		}
		if (((i + 1) % loggingInterval) == 0 || i == 0) {
			uint32_t log_i = i + 1;
			LOG("Iteration " << log_i << " " << bestSolution.size());
		}
	}

	if (!strategy->algorithmState->hypergraph.isSolvedBy(bestSolution)) {
		throw std::runtime_error("Solution not found");
	}

	return std::move(bestSolution);
}

bool LocalSearch::isAcceptable(Solution& solutionCandidate) {
	return solutionCandidate.size() <= bestSolution.size();
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
	return harmonicApproximation(strategy->algorithmState->hypergraph.getIncidentEdgeIndizes(node).size()); // weight of each node is 1, so that gets ommitted
}

double LocalSearch::harmonicApproximation(uint32_t k) {
	const double gamma = 0.57721566490153286060;
	return std::log(k) + gamma + 1.0 / (2 * k) - 1.0 / (12.0 * k * k);
}
