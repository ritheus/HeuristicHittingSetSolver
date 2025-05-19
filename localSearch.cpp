#include "localSearch.hpp"
#include "neighborhoodStrategy.hpp"
#include "greedyState.hpp"
#include "logger.hpp"
#include <string>
#include <map>
#include <cmath>
#include <random>

Solution LocalSearch::run(std::unique_ptr<NeighborhoodStrategy> neighborhoodStrategy) {
	uint32_t loggingInterval = 10;
	uint32_t noChangeCounter = 0;
	uint32_t adaptThreshold = 1500000;
	FastSet removedNodes;
	FastSet addedNodes;
	std::vector<Node> removedNodesVector;
	std::vector<Node> addedNodesVector;

	strategy->initializeAlgorithmState(std::move(state));
	while (!neighborhoodStrategy->isDone()) {
		removedNodesVector = strategy->removeNodes(neighborhoodStrategy->numNodesToDelete);
		addedNodesVector = strategy->repairPartialSolution();
		Solution& solutionCandidate = strategy->algorithmState->getSolution();
		updateDelta(removedNodesVector, addedNodesVector, removedNodes, addedNodes);
		if (isAcceptable(solutionCandidate)) {
			transformSolution(removedNodes, addedNodes);
			//bestSolution = solutionCandidate;
		}
		log_localsearch(neighborhoodStrategy->i, loggingInterval, bestSolution);
		neighborhoodStrategy->update();
		if (neighborhoodStrategy->i == adaptThreshold) {
			neighborhoodStrategy->adapt();
		}
	}

	if (!strategy->algorithmState->hypergraph.isSolvedBy(bestSolution)) {
		throw std::runtime_error("Solution not found");
	}

	return std::move(bestSolution);
}

/*
* We have to always keep removedNodes and addedNodes up to date, while we only have to update the best solution when we find a better one
* So this method makes it possible to keep track of the difference from the old best solution to the upcoming best solution
*/
bool LocalSearch::updateDelta(std::vector<Node>& removedNodesVector, std::vector<Node>& addedNodesVector, FastSet& removedNodes, FastSet& addedNodes) {
	for (Node node : removedNodesVector) {
		if (addedNodes.find(node) != addedNodes.end()) {
			removedNodes.insert(node);
		}
		else {
			addedNodes.erase(node);
		}
	}
	for (Node node : addedNodesVector) {
		if (removedNodes.find(node) != removedNodes.end()) {
			addedNodes.insert(node);
		}
		else {
			removedNodes.erase(node);
		}
	}
	return true;
}

bool LocalSearch::isAcceptable(Solution& solutionCandidate) {
	return solutionCandidate.size() < bestSolution.size() && strategy->algorithmState->hypergraph.isSolved();
}

void LocalSearch::transformSolution(FastSet& removedNodes, FastSet& addedNodes) {
	for (Node node : removedNodes) {
		bestSolution.erase(node);
	}
	for (Node node : addedNodes) {
		bestSolution.insert(node);
	}
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

void LocalSearch::log_localsearch(uint32_t i, uint32_t loggingInterval, Solution& bestSolution) {
	if (((i + 1) % loggingInterval) == 0 || i == 0) {
		uint32_t log_i = i + 1;
		LOG_LOCALSEARCH("Iteration " << log_i << " " << bestSolution.size());
	}
}
