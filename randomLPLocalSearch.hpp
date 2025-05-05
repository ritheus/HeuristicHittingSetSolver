#pragma once

#include "localsearchstrategy.hpp"
#include "greedyState.hpp"
#include "solution.hpp"
#include "hypergraph.hpp"
#include "aliases.hpp"
#include "LPSolver.hpp"
#include <map>
#include <unordered_set>

struct RandomLPLocalSearch : LocalSearchStrategy {
	std::map<Node, double> fractionalLPSolution;
	std::map<Node, double> inverseFractionalLPSolution;
	std::unordered_set<EdgeIndex> unhitEdges;

	RandomLPLocalSearch(std::map<Node, double> fractionalLPSolution) : fractionalLPSolution(fractionalLPSolution) {
	}
	RandomLPLocalSearch(Hypergraph& hypergraph) {
		fractionalLPSolution = getOrderedFractionalSolution(hypergraph);
	}

	void removeNodes(uint32_t) override;
	void repairPartialSolution() override;
	void initializeAlgorithmState(std::unique_ptr<AlgorithmState>) override;
	void buildInverseFractionalLPSolution();
	void addToSolution(Node);
	void removeFromSolution(Node);
};