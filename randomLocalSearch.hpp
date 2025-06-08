#pragma once

#include "localsearchstrategy.hpp"
#include "aliases.hpp"
#include <vector>

struct RandomLocalSearch : LocalSearchStrategy {
	std::vector<Node> removeNodes(uint32_t) override;
	std::vector<Node> repairPartialSolution() override;
	void initializeAlgorithmState(std::unique_ptr<AlgorithmState>) override;
	void setSolution(Solution&) override;
	Solution& getSolution() override;
	bool isSolved() override;
	bool isSolvedBy(Solution&) override;
};