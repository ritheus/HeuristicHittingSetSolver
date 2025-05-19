#pragma once

#include "aliases.hpp"
#include "hypergraph.hpp"
#include "cxxopts.hpp"
#include "solution.hpp"
#include <iostream>

struct AlgorithmState {
	Hypergraph hypergraph;
	Solution solution;
	FastSet bannedNodes;
	std::vector<Node> unbannedNodes;
	cxxopts::ParseResult optionsResult;
	std::unordered_map<Node, uint32_t> nodeAges; // higher is newer
	uint32_t highestAge = 0;

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

	void updateNodeAge(Node node) {
		nodeAges[node] = highestAge++;
	}

	uint32_t getNodeAge(Node node) {
		return nodeAges[node];
	}

	uint32_t getHighestAge() {
		return highestAge;
	}

	void setHighestAge(uint32_t age) {
		highestAge = age;
	}

	void setNodeAges(std::unordered_map<Node, uint32_t> nodeAges) {
		this->nodeAges = nodeAges;
	}

	std::unordered_map<Node, uint32_t> getNodeAges() {
		return nodeAges;
	}

	virtual Solution calculateSolution(bool = true) = 0;
	virtual std::vector<Node> repairSolution() {
		throw std::runtime_error("repairSolution not implemented");
	}
	virtual bool addToSolution(Node) = 0;
	virtual bool removeFromSolution(Node) {
		throw std::runtime_error("removeFromSolution not implemented");
	}
	virtual void banFromSolution(Node) {
		throw std::runtime_error("banFromSolution not implemented");
	}
	virtual void deleteNodes(const std::vector<Node>&) = 0;
	virtual void deleteEdges(std::vector<EdgeIndex>&) = 0;

	virtual ~AlgorithmState() = default;
};