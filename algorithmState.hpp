#pragma once

#include "aliases.hpp"
#include "hypergraph.hpp"
#include "cxxopts.hpp"
#include "solution.hpp"
#include <iostream>

struct AlgorithmState {
	Hypergraph hypergraph;
	Solution solution;
	std::unordered_set<Node> bannedNodes;
	cxxopts::ParseResult optionsResult;

	AlgorithmState(Hypergraph& hypergraph, const cxxopts::ParseResult& optionsResult) : hypergraph(hypergraph), optionsResult(optionsResult) {
		this->hypergraph.reset();
	}

	AlgorithmState(NumNodes n, NumEdges m, std::vector<Edge>&& setSystem, const cxxopts::ParseResult& optionsResult) : hypergraph(n, m, std::move(setSystem)), optionsResult(optionsResult) {}

	Solution& getSolution() {
		return solution;
	}

	virtual void setSolution(Solution solution) {
		this->solution = solution;
	}

	bool isSolved() {
		return hypergraph.isSolved();
	}

	virtual void addEdge(Edge edge) {
		hypergraph.addEdge(std::move(edge));
	}

	virtual Solution calculateSolution(bool=true) = 0;
	virtual void addToSolution(Node) = 0;
	virtual void banFromSolution(Node) {
		throw std::runtime_error("banFromSolution not implemented");
	}
	virtual void deleteNodes(const std::vector<Node>&) = 0;
	virtual void deleteEdges(std::vector<EdgeIndex>&) = 0;

	virtual ~AlgorithmState() = default;
};