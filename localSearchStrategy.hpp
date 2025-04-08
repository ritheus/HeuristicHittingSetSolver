#pragma once

#include "solution.hpp"
#include "hypergraph.hpp"
#include "greedyState.hpp"

struct LocalSearchStrategy {
	virtual Solution removeNodes(Hypergraph&, Solution&, uint32_t) = 0;
	virtual Solution repairPartialSolution(Hypergraph&, Solution&) = 0;
};