#pragma once

#include "localSearch/neighborhood/neighborhoodStrategy.hpp"
#include <cmath>

struct OscillatingNeighborhoodStrategy : NeighborhoodStrategy {
	uint32_t minNumNodesToDelete;
	uint32_t maxNumNodesToDelete;
	uint32_t period;

	OscillatingNeighborhoodStrategy(uint32_t numIterations, uint32_t minNumNodesToDelete, uint32_t maxNumNodesToDelete, uint32_t period) : minNumNodesToDelete(minNumNodesToDelete), maxNumNodesToDelete(maxNumNodesToDelete), period(period), NeighborhoodStrategy(numIterations, maxNumNodesToDelete) {}

	void update() override {
		uint32_t modulo_i = (i + period / 2) % period;
		double x = static_cast<double>(modulo_i) / static_cast<double>(period);
		double tri = 1.0 - std::abs(2.0 * (x - std::floor(x + 0.5)));
		double numNodesToDelete = tri * (maxNumNodesToDelete - minNumNodesToDelete) + minNumNodesToDelete;

		i++;
	}
};