#pragma once

#include "algorithms/algorithmState.hpp"
#include "algorithms/greedyState.hpp"

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

	std::optional<Solution> branchAndReduce(GreedyState, FastSet, FastSet);
	uint32_t calculateSolutionSizeLowerBound(GreedyState&);
	void reduce(GreedyState&);
	Node chooseNextNode(GreedyState&);

	int calculateMaxDegreeBound(GreedyState&);
	int calculateEfficiencyBound(GreedyState&);
};