#pragma once

#include "hypergraph.hpp"
#include "solution.hpp"
#include "cxxopts.hpp"
#include "localSearchStrategy.hpp"
#include "algorithmState.hpp"
#include <string>
#include <map>
#include <optional>

struct LocalSearch {
	Solution bestSolution;
	std::unique_ptr<LocalSearchStrategy> strategy;
	std::unique_ptr<AlgorithmState> state;
	cxxopts::ParseResult optionsResult;

	LocalSearch(
		std::unique_ptr<AlgorithmState> state,
		std::unique_ptr<LocalSearchStrategy> strategy, 
		const cxxopts::ParseResult& optionsResult
	) : strategy(std::move(strategy)), 
		bestSolution(state->getSolution()), 
		optionsResult(optionsResult),
		state(std::move(state)) {}

	Solution run(uint32_t, uint32_t, uint32_t, std::string);
	bool isAcceptable(Solution&);

	// TODO
	double solutionPotential;
	double currentSolutionPotential;
	void initializePotential();
	double potential(Node);
	void updatePotential(const std::vector<Node>&, const std::vector<Node>&);
	double harmonicApproximation(uint32_t);
};