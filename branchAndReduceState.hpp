#pragma once

#include "algorithmState.hpp"
#include "greedyState.hpp"

struct BranchAndReduceState : AlgorithmState {
private:
	uint32_t globalUpperBound;

public:
	BranchAndReduceState(Hypergraph&, const cxxopts::ParseResult&);
	BranchAndReduceState(NumNodes, NumEdges, std::vector<Edge>&&, const cxxopts::ParseResult&);

	Solution calculateSolution(bool=false) override;
	bool addToSolution(Node) override;
	void deleteNodes(const std::vector<Node>&) override;
	void deleteEdges(std::vector<EdgeIndex>&) override;

	std::optional<Solution> branchAndReduce(GreedyState, std::unordered_set<Node>, std::unordered_set<Node>);
	uint32_t calculateSolutionSizeLowerBound(GreedyState&);
	void reduce(GreedyState&);
	Node chooseNextNode(GreedyState&);

	int calculateMaxDegreeBound(GreedyState&);
	int calculateEfficiencyBound(GreedyState&);
};