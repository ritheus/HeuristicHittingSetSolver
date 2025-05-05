#pragma once

#include "localsearchstrategy.hpp"
#include "greedyState.hpp"
#include "solution.hpp"
#include "hypergraph.hpp"
#include "aliases.hpp"
#include "LPSolver.hpp"
#include <map>

struct LPLocalSearch : LocalSearchStrategy {
	std::map<Node, double> fractionalLPSolution;
	GreedyState greedyState;

	LPLocalSearch(GreedyState greedyState, std::map<Node, double> fractionalLPSolution) : greedyState(greedyState), fractionalLPSolution(fractionalLPSolution) {
	}
	LPLocalSearch(GreedyState greedyState) : greedyState(greedyState) {
		fractionalLPSolution = getOrderedFractionalSolution(greedyState.hypergraph);
	}

	void removeNodes(uint32_t) override;
	void repairPartialSolution() override;
	void initializeAlgorithmState(std::unique_ptr<AlgorithmState>) override;
};