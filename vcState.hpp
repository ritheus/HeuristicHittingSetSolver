#include "algorithmState.hpp"
#include "aliases.hpp"
#include <map>

struct VCState : AlgorithmState {
	std::map<Node, double> orderedFractionalSolution;
	uint32_t numIterations;

	VCState(Hypergraph&, const cxxopts::ParseResult&);
	VCState(NumNodes, NumEdges, std::vector<Edge>&&, const cxxopts::ParseResult&);

	Solution calculateSolution(bool = true) override;
	void addToSolution(Node) override;
	void removeFromSolution(Node) override;
	void deleteNodes(const std::vector<Node>&) override;
	void deleteEdges(std::vector<EdgeIndex>&) override;

	std::unordered_map<Node, double> buildAndSolveLP();
	std::map<Node, double> getOrderedFractionalSolution();
	bool isApproximately(double a, double b, double epsilon = 0.0001);
};