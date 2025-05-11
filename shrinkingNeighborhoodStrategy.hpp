#pragma once

#include "neighborhoodStrategy.hpp"

struct ShrinkingNeighborhoodStrategy : NeighborhoodStrategy {
	uint32_t minNumNodesToDelete;
	uint32_t maxNumNodesToDelete;
	uint32_t stepInterval;

	ShrinkingNeighborhoodStrategy(uint32_t numIterations, uint32_t minNumNodesToDelete, uint32_t maxNumNodesToDelete, uint32_t stepInterval) : minNumNodesToDelete(minNumNodesToDelete), maxNumNodesToDelete(maxNumNodesToDelete), stepInterval(stepInterval), NeighborhoodStrategy(numIterations, maxNumNodesToDelete) {}

	void update() override {
		i++;
		if (i % stepInterval == 0) {
			if (numNodesToDelete > minNumNodesToDelete) {
				numNodesToDelete = std::max(numNodesToDelete /= 2, minNumNodesToDelete);
			}
		}
	}
};