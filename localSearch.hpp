#pragma once

#include "hypergraph.hpp"
#include "solution.hpp"
#include "cxxopts.hpp"
#include "localSearchStrategy.hpp"
#include "algorithmState.hpp"
#include "neighborhoodStrategy.hpp"
#include <string>
#include <map>
#include <optional>

struct LocalSearch {
	std::unique_ptr<LocalSearchStrategy> strategy;
	Solution bestSolution;
	cxxopts::ParseResult optionsResult;
	std::unique_ptr<AlgorithmState> state;

	LocalSearch(
		std::unique_ptr<AlgorithmState> state,
		std::unique_ptr<LocalSearchStrategy> strategy, 
		const cxxopts::ParseResult& optionsResult
	) : strategy(std::move(strategy)), 
		state(std::move(state)),
		bestSolution(state->getSolution()), 
		optionsResult(optionsResult) {}

	Solution run(std::unique_ptr<NeighborhoodStrategy>);
	bool isAcceptable(Solution&);
	bool updateDelta(std::vector<Node>&, std::vector<Node>&, std::unordered_set<Node>&, std::unordered_set<Node>&);
	void transformSolution(std::unordered_set<Node>&, std::unordered_set<Node>&);

	// TODO
	double solutionPotential;
	double currentSolutionPotential;
	void initializePotential();
	double potential(Node);
	void updatePotential(const std::vector<Node>&, const std::vector<Node>&);
	double harmonicApproximation(uint32_t);
	void log_localsearch(uint32_t, uint32_t, Solution&);
};