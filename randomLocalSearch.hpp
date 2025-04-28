#pragma once

#include "localsearchstrategy.hpp"
#include "aliases.hpp"
#include <vector>

struct RandomLocalSearch : LocalSearchStrategy {
	void removeNodes(uint32_t) override;
	void repairPartialSolution() override;
	void initializeAlgorithmState(std::unique_ptr<AlgorithmState>) override;
};