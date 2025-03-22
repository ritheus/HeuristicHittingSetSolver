#pragma once

#include "hypergraph.hpp"
#include "solution.hpp"
#include "algorithmState.hpp"
#include "updatable_priority_queue.hpp"
#include "kernelization.hpp"
#include "cxxopts.hpp"
#include "sigtermHandler.hpp"

struct GreedyState : AlgorithmState {
public:
	pq::updatable_priority_queue<Node, uint32_t> potentialNodeImpact;

	GreedyState(NumNodes, NumEdges, std::vector<Edge>&&); // O(n * log n + m * deg_edge)

	std::unordered_set<Node> calculateSolution(GreedyState&, cxxopts::ParseResult&);
	void deleteNodes(const std::vector<Node>&);
	void deleteEdges(std::vector<EdgeIndex>&) override;
	void addToSolution(Node) override;

	std::pair<uint32_t, Node> getHighestImpactNode();
	uint32_t getImpact(Node);
	bool updateImpact(const Node&, const uint32_t, bool=false);
};