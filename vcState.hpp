#include "algorithmState.hpp"

struct VCState : AlgorithmState {
	VCState(Hypergraph&, const cxxopts::ParseResult&);
	VCState(NumNodes, NumEdges, std::vector<Edge>&&, const cxxopts::ParseResult&);

	Solution calculateSolution(bool = true) override;
	void addToSolution(Node) override;
	void deleteNodes(const std::vector<Node>&) override;
	void deleteEdges(std::vector<EdgeIndex>&) override;

	std::unordered_map<Node, double> buildAndSolveLP();
	bool isApproximately(double a, double b, double epsilon = 0.0001);
};