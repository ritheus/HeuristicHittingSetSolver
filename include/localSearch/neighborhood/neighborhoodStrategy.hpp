#pragma once

#include "hypergraph.hpp"
#include "solution.hpp"
#include "peripheral/cxxopts.hpp"

struct NeighborhoodStrategy {
	uint32_t i = 0;
	uint32_t numNodesToDelete = 1;
	uint32_t numIterations = 0;
	uint32_t revertSolutionThreshold = 0;

	NeighborhoodStrategy(uint32_t numIterations, uint32_t numNodesToDelete) : numNodesToDelete(numNodesToDelete), numIterations(numIterations) {}

	virtual void update() = 0;
	virtual void adapt() {}
	virtual void setIndex(uint32_t i) { this->i = i; }
	virtual void setNumIterations(uint32_t numIterations) { 
		this->numIterations = numIterations; 
	}
	virtual bool isDone() {
		return i == numIterations;
	}

	virtual ~NeighborhoodStrategy() = default;
};