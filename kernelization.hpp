#pragma once

#include "hypergraph.hpp"
#include "solution.hpp"
#include "algorithmState.hpp"
#include "cxxopts.hpp"

namespace kernelization {
	void applyKernelization(AlgorithmState& state, cxxopts::ParseResult& optionsResult);

	void applyUnitEdgeRule(AlgorithmState&);
	void applyEdgeDominationRule(AlgorithmState&);
	std::vector<EdgeIndex> findDominatedEdges(AlgorithmState&);
	bool edgeDominatesEdge(const Edge&, const Edge&, EdgeIndex, EdgeIndex);
	void applyVertexDominationRule(AlgorithmState&);
	std::unordered_set<Node> findDominatedNodes(AlgorithmState&);
	bool nodeDominatesNode(Node, Node, AlgorithmState&);
	void applyCrownDecompositionRule(AlgorithmState&);
	void applyCriticalCoreRule(AlgorithmState&);

	bool subedgeOccursInSuperedge(const Edge&, const Edge&, const AlgorithmState&);
	std::vector<EdgeIndex> isCriticalCore(const Edge&, const AlgorithmState&);
	std::vector<EdgeIndex> calculateLink(const Edge&, const AlgorithmState&);
};