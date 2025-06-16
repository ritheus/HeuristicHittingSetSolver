#pragma once

#include "localsearchstrategy.hpp"
#include "algorithms/greedyState.hpp"
#include "solution.hpp"
#include "hypergraph.hpp"
#include "peripheral/aliases.hpp"
#include "LPSolver.hpp"
#include <map>
#include <unordered_set>

struct RandomLPLocalSearch : LocalSearchStrategy {
	std::map<Node, double> fractionalLPSolution;
	std::vector<double> inverseFractionalLPValues;
	std::vector<Node> inverseFractionalLPNodes;
	std::unordered_map<Node, uint32_t> inverseFractionalLPNodesToIndex;
	std::unordered_set<EdgeIndex> unhitEdges;

	RandomLPLocalSearch(std::map<Node, double> fractionalLPSolution, Solution& solution) : fractionalLPSolution(fractionalLPSolution) {
		buildInverseFractionalLPSolution(solution);
	}
	RandomLPLocalSearch(Hypergraph& hypergraph, Solution& solution) {
		fractionalLPSolution = getOrderedFractionalSolution(hypergraph);
		buildInverseFractionalLPSolution(solution);
	}

	std::vector<Node> removeNodes(uint32_t) override;
	std::vector<Node> repairPartialSolution() override;
	void initializeAlgorithmState(std::unique_ptr<AlgorithmState>) override;
	void buildInverseFractionalLPSolution(Solution& solution);
	void addToSolution(Node);
	void removeFromSolution(Node);
};