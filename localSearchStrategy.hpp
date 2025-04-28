#pragma once

#include "solution.hpp"
#include "hypergraph.hpp"
#include "greedyState.hpp"

struct LocalSearchStrategy {
	std::unordered_map<Node, uint32_t> nodeAges; // higher is newer
	uint32_t highestAge = 0;
	std::unique_ptr<AlgorithmState> algorithmState;

	virtual void removeNodes(uint32_t) = 0;
	virtual void repairPartialSolution() = 0;

	virtual void initializeAlgorithmState(std::unique_ptr<AlgorithmState>) = 0;
};