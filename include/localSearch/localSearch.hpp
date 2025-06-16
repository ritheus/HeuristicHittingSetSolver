#pragma once

#include "hypergraph.hpp"
#include "solution.hpp"
#include "peripheral/cxxopts.hpp"
#include "localSearchStrategy.hpp"
#include "algorithms/algorithmState.hpp"
#include "localSearch/neighborhood/neighborhoodStrategy.hpp"
#include <string>
#include <map>
#include <optional>

struct LocalSearch {
	std::unique_ptr<LocalSearchStrategy> strategy;
	std::unique_ptr<NeighborhoodStrategy> neighborhoodStrategy;
	Solution bestSolution;
	cxxopts::ParseResult optionsResult;
	std::unique_ptr<AlgorithmState> state;
	FastSet removedNodes;
	FastSet addedNodes;

	LocalSearch(
		std::unique_ptr<AlgorithmState> state,
		std::unique_ptr<LocalSearchStrategy> strategy,
		std::unique_ptr<NeighborhoodStrategy> neighborhoodStrategy,
		const cxxopts::ParseResult& optionsResult
	) : strategy(std::move(strategy)), 
		neighborhoodStrategy(std::move(neighborhoodStrategy)),
		state(std::move(state)),
		bestSolution(state->getSolution()), 
		optionsResult(optionsResult) {}

	Solution run(bool = false, uint32_t = 0);
	bool isAcceptable(Solution&);
	bool updateDelta(std::vector<Node>&, std::vector<Node>&, FastSet&, FastSet&);
	void transformSolution(FastSet&, FastSet&);
	void revertSolution(FastSet&, FastSet&);

	void replaceStrategy(std::unique_ptr<LocalSearchStrategy>);
	void setSolution(Solution&, Solution&);
	void resetDelta();
	void copyDelta(LocalSearch&);
	void log_localsearch(uint32_t, uint32_t, Solution&);
};