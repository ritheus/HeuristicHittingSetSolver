#pragma once

#include "localSearch/neighborhood/neighborhoodStrategy.hpp"

struct FlatNeighborhoodStrategy : NeighborhoodStrategy {	
	FlatNeighborhoodStrategy(uint32_t numIterations, uint32_t numNodesToDelete) : NeighborhoodStrategy(numIterations, numNodesToDelete) {}

	void update() override {
		i++;
	}
};