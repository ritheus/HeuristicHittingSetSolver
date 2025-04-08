#include "localsearchstrategy.hpp"

#include "solution.hpp"
#include "hypergraph.hpp"
#include "aliases.hpp"
#include "updatable_priority_queue.hpp"
#include <vector>

struct AdaptiveGreedyTabuLocalSearch : LocalSearchStrategy {
	std::vector<Node> currentSolutionVector;
	std::vector<Node> tabuList;
	uint32_t tabuLength;
	pq::updatable_priority_queue<Node, uint32_t> solutionNodeSingleResponsibilities;

	AdaptiveGreedyTabuLocalSearch(Hypergraph&, Solution&, uint32_t);

	bool nodeHasSingleResponsibility(Hypergraph&, Solution&, Node, EdgeIndex);

	Solution removeNodes(Hypergraph&, Solution&, uint32_t) override;
	Solution repairPartialSolution(Hypergraph&, Solution&) override;
	// tabu list
	void addToTabuList(Node, uint32_t);
	bool canBeBanned(Node, Hypergraph&);
	bool isTabu(Node);
};