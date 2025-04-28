#pragma once

#include "localsearchstrategy.hpp"
#include "solution.hpp"
#include "hypergraph.hpp"
#include "aliases.hpp"
#include <map>

struct LPLocalSearch : LocalSearchStrategy {
	std::map<Node, double> fractionalLPSolution;

	LPLocalSearch(std::map<Node, double> fractionalLPSolution) : fractionalLPSolution(fractionalLPSolution) {}

	void removeNodes(uint32_t) override;
	void repairPartialSolution() override;
	void initializeAlgorithmState(std::unique_ptr<AlgorithmState>) override;
};