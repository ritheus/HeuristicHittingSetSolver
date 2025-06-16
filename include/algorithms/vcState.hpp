#include "algorithms/algorithmState.hpp"
#include "peripheral/aliases.hpp"
#include <map>

struct VCState : AlgorithmState {
	std::map<Node, double> orderedFractionalSolution;
	uint32_t numIterations;

	VCState(Hypergraph&, const cxxopts::ParseResult&);
	VCState(NumNodes, NumEdges, std::vector<Edge>&&, const cxxopts::ParseResult&);

	Solution calculateSolution(bool = true) override;
	bool addToSolution(Node) override;
	bool removeFromSolution(Node) override;
	void deleteNodes(const std::vector<Node>&) override;
	void deleteEdges(std::vector<EdgeIndex>&) override;
	std::map<Node, double> getOrderedFractionalSolution();
	bool isApproximately(double a, double b, double epsilon = 0.0001);
};