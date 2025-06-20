#pragma once

#include "localSearch/neighborhood/neighborhoodStrategy.hpp"
#include <cmath>

struct ShrinkAndOscillateNeighborhoodStrategy : NeighborhoodStrategy {
	uint32_t minNumNodesToDelete;
	uint32_t maxNumNodesToDelete;
	uint32_t period;
	uint32_t stepInterval;
	uint32_t revertSolutionThresholdAnchor = 12800;
	bool shrinking = true;

	ShrinkAndOscillateNeighborhoodStrategy(uint32_t numIterations, uint32_t minNumNodesToDelete, uint32_t maxNumNodesToDelete, uint32_t stepInterval, uint32_t period) : minNumNodesToDelete(minNumNodesToDelete), maxNumNodesToDelete(maxNumNodesToDelete), stepInterval(stepInterval), period(period), NeighborhoodStrategy(numIterations, maxNumNodesToDelete) {}

	void update() override {
		i++;
		if (shrinking) {
			updateShrinking();
		}
		else {
			updateOscillating();
		}
	}

	void adapt() override {
		shrinking = false;
	}

	void updateShrinking() {
		if (i % stepInterval == 0) {
			if (numNodesToDelete > minNumNodesToDelete) {
				numNodesToDelete = std::max(numNodesToDelete /= 2, minNumNodesToDelete);
			}
			revertSolutionThreshold = std::min<uint32_t>(revertSolutionThreshold * 2, 12800);
		}
	}

	void updateOscillating() {
		uint32_t modulo_i = (i + period / 2) % period;
		double x = static_cast<double>(modulo_i) / static_cast<double>(period);
		double tri = 1.0 - std::abs(2.0 * (x - std::floor(x + 0.5)));
		numNodesToDelete = tri * (maxNumNodesToDelete - minNumNodesToDelete) + minNumNodesToDelete;
		revertSolutionThreshold = revertSolutionThresholdAnchor - tri * revertSolutionThresholdAnchor;
	}
};