#include "localSearch.hpp"
#include "neighborhoodStrategy.hpp"
#include "greedyState.hpp"
#include "logger.hpp"
#include <string>
#include <map>
#include <cmath>
#include <random>
#include <chrono>

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

Solution LocalSearch::run(bool revertSolutionFlag, uint32_t revertSolutionThreshold) {
	uint32_t loggingInterval = 100;
	uint32_t noChangeCounter = 0;
	uint32_t revertSolutionCounter = 0;
	uint32_t adaptThreshold = 150000000;
	neighborhoodStrategy->revertSolutionThreshold = revertSolutionThreshold;
	std::vector<Node> removedNodesVector;
	std::vector<Node> addedNodesVector;

	const int maxSeconds = 3600;
	TimePoint start = Clock::now();

	while (true) {
		removedNodesVector = strategy->removeNodes(neighborhoodStrategy->numNodesToDelete);
		addedNodesVector = strategy->repairPartialSolution();
		Solution& solutionCandidate = strategy->algorithmState->getSolution();
		updateDelta(removedNodesVector, addedNodesVector, removedNodes, addedNodes);
		if (isAcceptable(solutionCandidate)) {
			transformSolution(removedNodes, addedNodes);
			revertSolutionCounter = 0;
		}
		else {
			if (revertSolutionFlag) {
				revertSolutionCounter++;
				if (revertSolutionCounter > neighborhoodStrategy->revertSolutionThreshold) {
					revertSolution(removedNodes, addedNodes);
					revertSolutionCounter = 0;
				}
			}
		}
		log_localsearch(neighborhoodStrategy->i, loggingInterval, bestSolution);
		neighborhoodStrategy->update();
		if (neighborhoodStrategy->i == adaptThreshold) {
			neighborhoodStrategy->adapt();
		}

		TimePoint now = Clock::now();
		double elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
		if (neighborhoodStrategy->isDone() || elapsedSeconds >= maxSeconds) {
			if (!strategy->isSolvedBy(bestSolution)) {
				throw std::runtime_error("Solution not found");
			}

			return std::move(bestSolution);
		}
	}

	return std::move(bestSolution);
}

/*
* We have to always keep removedNodes and addedNodes up to date, while we only have to update the best solution when we find a better one
* So this method makes it possible to keep track of the difference from the old best solution to the upcoming best solution
*/
bool LocalSearch::updateDelta(std::vector<Node>& removedNodesVector, std::vector<Node>& addedNodesVector, FastSet& removedNodes, FastSet& addedNodes) {
	for (Node node : removedNodesVector) {
		if (addedNodes.find(node) == addedNodes.end()) {
			removedNodes.insert(node);
		}
		else {
			addedNodes.erase(node);
		}
	}
	for (Node node : addedNodesVector) {
		if (removedNodes.find(node) == removedNodes.end()) {
			addedNodes.insert(node);
		}
		else {
			removedNodes.erase(node);
		}
	}
	return true;
}

bool LocalSearch::isAcceptable(Solution& solutionCandidate) {
	if (solutionCandidate.size() < bestSolution.size()) {
		if (strategy->isSolved()) {
			return true;
		}
		else {
			throw std::runtime_error("hypergraph is unsolved");
		}
	}
	return false;
}

void LocalSearch::transformSolution(FastSet& removedNodes, FastSet& addedNodes) {
	for (Node node : removedNodes) {
		bestSolution.erase(node);
	}
	for (Node node : addedNodes) {
		bestSolution.insert(node);
	}
	removedNodes.clear();
	addedNodes.clear();
}

void LocalSearch::revertSolution(FastSet& removedNodes, FastSet& addedNodes) {
	strategy->revertSolution(removedNodes, addedNodes);
	for (Node node : removedNodes) {
		strategy->algorithmState->addToSolution(node);
	}
	for (Node node : addedNodes) {
		strategy->algorithmState->removeFromSolution(node);
	}
	removedNodes.clear();
	addedNodes.clear();
}

void LocalSearch::replaceStrategy(std::unique_ptr<LocalSearchStrategy> newStrategy) {
	this->state = std::move(this->strategy->algorithmState);
	this->strategy = std::move(newStrategy);
}

void LocalSearch::setSolution(Solution& newSolution, Solution& bestSolution) {
	this->bestSolution = bestSolution;
	this->strategy->setSolution(newSolution);
}

void LocalSearch::resetDelta() {
	this->removedNodes.clear();
	this->addedNodes.clear();
}

void LocalSearch::copyDelta(LocalSearch& source) {
	for (Node node : source.removedNodes) {
		if (this->addedNodes.find(node) == this->addedNodes.end()) {
			this->removedNodes.insert(node);
		}
	}
	for (Node node : source.addedNodes) {
		if (this->removedNodes.find(node) == this->removedNodes.end()) {
			this->addedNodes.insert(node);
		}
	}
}

void LocalSearch::log_localsearch(uint32_t i, uint32_t loggingInterval, Solution& bestSolution) {
	if (((i + 1) % loggingInterval) == 0 || i == 0) {
		uint32_t log_i = i + 1;
		LOG_LOCALSEARCH("Iteration " << log_i << " " << bestSolution.size());
	}
}
