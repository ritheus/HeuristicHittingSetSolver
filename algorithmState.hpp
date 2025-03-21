#pragma once

#include "aliases.hpp"
#include "hypergraph.hpp"
#include <iostream>

struct AlgorithmState {
	Hypergraph hypergraph;
	Solution solution;
	uint32_t lowerSolutionSizeBound = 0;
	uint32_t upperSolutionSizeBound;

	AlgorithmState(NumNodes n, NumEdges m, std::vector<Edge>&& setSystem) : hypergraph(n, m, std::move(setSystem)) { 
		upperSolutionSizeBound = hypergraph.n;
	}

	virtual void addToSolution(Node node) {
		solution.insert(node);
	}

	std::unordered_set<Node>& getSolution() {
		return solution.getSolution();
	}

	virtual void deleteNodes(const std::vector<Node>& nodesToRemove) = 0;

	virtual void addEdge(Edge edge) {
		hypergraph.addEdge(std::move(edge));
	}

	virtual void deleteEdges(std::vector<EdgeIndex>& edgeIndizesToRemove) {
		hypergraph.deleteEdges(edgeIndizesToRemove);
	}

	virtual ~AlgorithmState() = default;
};