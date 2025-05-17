#include "localsearchstrategy.hpp"

#include "solution.hpp"
#include "hypergraph.hpp"
#include "aliases.hpp"
#include "updatable_priority_queue.hpp"
#include <vector>

struct RandomTabuLocalSearch : LocalSearchStrategy {
	std::vector<Node> currentSolutionVector;
	std::vector<Node> tabuList;
	uint32_t tabuLength;
	RandomTabuLocalSearch(Hypergraph&, Solution&, uint32_t);

	bool nodeHasSingleResponsibility(Hypergraph&, Solution&, Node, EdgeIndex);

	std::vector<Node> removeNodes(uint32_t) override;
	std::vector<Node> repairPartialSolution() override;
	void initializeAlgorithmState(std::unique_ptr<AlgorithmState>) override;
	// tabu list
	void addToTabuList(Node, uint32_t);
	bool canBeBanned(Node);
	bool isTabu(Node);
};