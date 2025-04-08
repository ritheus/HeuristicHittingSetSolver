#pragma once

#include "localsearchstrategy.hpp"
#include "aliases.hpp"
#include <vector>

struct RandomLocalSearch : LocalSearchStrategy {
	std::vector<Node> currentSolutionVector;

	Solution removeNodes(Hypergraph&, Solution&, uint32_t) override;
	Solution repairPartialSolution(Hypergraph&, Solution&) override;
};