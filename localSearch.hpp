#pragma once

#include "hypergraph.hpp"
#include "solution.hpp"
#include "cxxopts.hpp"
#include "localSearchStrategy.hpp"
#include "algorithmState.hpp"
#include <map>
#include <optional>

struct LocalSearch {
	Hypergraph hypergraph;
	Solution bestSolution;
	Solution currentSolution;
	std::unique_ptr<LocalSearchStrategy> strategy;
	cxxopts::ParseResult optionsResult;

	LocalSearch(
		std::unique_ptr<AlgorithmState> state,
		std::unique_ptr<LocalSearchStrategy> strategy, 
		const cxxopts::ParseResult& optionsResult
	) : hypergraph(state->hypergraph), 
		strategy(std::move(strategy)), 
		bestSolution(state->getSolution()), 
		currentSolution(state->getSolution()),
		optionsResult(optionsResult) {}

	Solution run(uint32_t, uint32_t);
	bool isAcceptable(Solution&);

	// TODO
	double solutionPotential;
	double currentSolutionPotential;
	void initializePotential();
	double potential(Node);
	void updatePotential(const std::vector<Node>&, const std::vector<Node>&);
	double harmonicApproximation(uint32_t);
};