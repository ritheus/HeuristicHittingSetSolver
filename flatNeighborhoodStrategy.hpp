#pragma once

#include "neighborhoodStrategy.hpp"

struct FlatNeighborhoodStrategy : NeighborhoodStrategy {
	uint32_t numIterations;

	FlatNeighborhoodStrategy(uint32_t numIterations, uint32_t numNodesToDelete) : numIterations(numIterations), NeighborhoodStrategy(numNodesToDelete) {}

	void update() override {
		i++;
	}

	bool isDone() override {
		return numIterations == i;
	}
};