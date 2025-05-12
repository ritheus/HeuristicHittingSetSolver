#pragma once

#include "neighborhoodStrategy.hpp"

struct OscillatingNeighborhoodStrategy : NeighborhoodStrategy {
	uint32_t minNumNodesToDelete;
	uint32_t maxNumNodesToDelete;
	uint32_t period;
	double angle = 0.0;
	double y = 0.0;
	const double pi = 3.141592653589793;

	OscillatingNeighborhoodStrategy(uint32_t numIterations, uint32_t minNumNodesToDelete, uint32_t maxNumNodesToDelete, uint32_t period) : minNumNodesToDelete(minNumNodesToDelete), maxNumNodesToDelete(maxNumNodesToDelete), period(period), NeighborhoodStrategy(numIterations, maxNumNodesToDelete) {}

	void update() override {
		angle = 2*pi*i / period;
		y = std::cos(angle);
		numNodesToDelete = minNumNodesToDelete + (maxNumNodesToDelete - minNumNodesToDelete) * (y + 1) / 2;

		i++;
	}
};