#pragma once

#include "solution.hpp"
#include "hypergraph.hpp"
#include "algorithms/greedyState.hpp"

struct LocalSearchStrategy {
	std::unordered_map<Node, uint32_t> nodeAges; // higher is newer
	uint32_t highestAge = 0;
	std::unique_ptr<AlgorithmState> algorithmState;

	virtual std::vector<Node> removeNodes(uint32_t) = 0;
	virtual std::vector<Node> repairPartialSolution() = 0;
	virtual void revertSolution(FastSet& removedNodes, FastSet& addedNodes) {
		for (Node node : removedNodes) {
			algorithmState->addToSolution(node);
		}
		for (Node node : addedNodes) {
			algorithmState->removeFromSolution(node);
		}
		removedNodes.clear();
		addedNodes.clear();
	}

	virtual void setSolution(Solution& solution) {
		algorithmState->setSolution(solution);
		throw std::runtime_error("setSolution not implemented");
	}

	virtual Solution& getSolution() {
		throw std::runtime_error("getSolution not implemented");
	}
	virtual bool isSolved() {
		throw std::runtime_error("isSolved not implemented");
	}

	virtual bool isSolvedBy(Solution&) {
		throw std::runtime_error("isSolvedBy not implemented");
	}

	virtual void initializeAlgorithmState(std::unique_ptr<AlgorithmState>) = 0;
};