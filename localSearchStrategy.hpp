#pragma once

#include "solution.hpp"
#include "hypergraph.hpp"
#include "greedyState.hpp"

struct LocalSearchStrategy {
	std::unordered_map<Node, uint32_t> nodeAges; // higher is newer
	uint32_t highestAge = 0;

	virtual Solution removeNodes(Hypergraph&, Solution&, uint32_t) = 0;
	virtual Solution repairPartialSolution(Hypergraph&, Solution&) = 0;
};