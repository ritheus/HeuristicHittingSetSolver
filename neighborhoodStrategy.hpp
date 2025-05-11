#pragma once

#include "hypergraph.hpp"
#include "solution.hpp"
#include "cxxopts.hpp"

struct NeighborhoodStrategy {
	uint32_t i = 0;
	uint32_t numNodesToDelete = 1;
	uint32_t numIterations = 0;

	NeighborhoodStrategy(uint32_t numIterations, uint32_t numNodesToDelete) : numNodesToDelete(numNodesToDelete), numIterations(numIterations) {}

	virtual void update() = 0;
	virtual bool isDone() {
		return i == numIterations;
	}

	virtual ~NeighborhoodStrategy() = default;
};