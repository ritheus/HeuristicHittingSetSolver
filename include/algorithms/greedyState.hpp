#pragma once

#include "hypergraph.hpp"
#include "solution.hpp"
#include "algorithms/algorithmState.hpp"
#include "updatable_priority_queue.hpp"
#include "kernelization.hpp"
#include "peripheral/cxxopts.hpp"
#include "peripheral/sigtermHandler.hpp"

struct GreedyState : AlgorithmState {
public:
	pq::updatable_priority_queue<Node, uint32_t> potentialNodeImpact;

	GreedyState(NumNodes, NumEdges, std::vector<Edge>&&, const cxxopts::ParseResult&); // O(n * log n + m * deg_edge)
	GreedyState(Hypergraph&, const cxxopts::ParseResult&);

	Solution calculateSolution(bool=true);
	virtual std::vector<Node> repairSolution() override;
	void deleteNodes(const std::vector<Node>&);
	void deleteEdges(std::vector<EdgeIndex>&) override;
	bool addToSolution(Node) override;
	bool removeFromSolution(Node) override;
	void banFromSolution(Node) override;
	void setSolution(Solution& solution) override;

	std::pair<uint32_t, Node> getHighestImpactNode();
	uint32_t getImpact(Node);
	bool updateImpact(const Node&, const uint32_t, bool=false, bool=true);
	void removeFromPQ(const Node);
};