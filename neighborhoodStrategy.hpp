#pragma once

#include "hypergraph.hpp"
#include "solution.hpp"
#include "cxxopts.hpp"

struct NeighborhoodStrategy {
	uint32_t i = 0;
	uint32_t numNodesToDelete;

	NeighborhoodStrategy(uint32_t numNodesToDelete) : numNodesToDelete(numNodesToDelete) {}

	virtual void update() = 0;
	virtual bool isDone() = 0;

	virtual ~NeighborhoodStrategy() = default;
};