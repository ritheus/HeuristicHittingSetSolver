#pragma once

#include "localsearchstrategy.hpp"
#include "algorithms/greedyState.hpp"
#include "solution.hpp"
#include "hypergraph.hpp"
#include "peripheral/aliases.hpp"
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

	std::vector<Node> removeNodes(uint32_t) override;
	std::vector<Node> repairPartialSolution() override;
	void revertSolution(FastSet&, FastSet&) override;
	void setSolution(Solution&) override;
	Solution& getSolution() override;
	bool isSolved() override;
	bool isSolvedBy(Solution&) override;
	void initializeAlgorithmState(std::unique_ptr<AlgorithmState>) override;
};