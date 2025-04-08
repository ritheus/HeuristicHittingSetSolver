#pragma once

#include "localsearchstrategy.hpp"
#include "solution.hpp"
#include "hypergraph.hpp"
#include "aliases.hpp"
#include <map>

struct LPLocalSearch : LocalSearchStrategy {
	std::map<Node, double> fractionalLPSolution;

	LPLocalSearch(std::map<Node, double> fractionalLPSolution) : fractionalLPSolution(fractionalLPSolution) {}

	Solution removeNodes(Hypergraph&, Solution&, uint32_t) override;
	Solution repairPartialSolution(Hypergraph&, Solution&) override;
};