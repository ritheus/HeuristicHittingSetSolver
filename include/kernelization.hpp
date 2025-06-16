#pragma once

#include "hypergraph.hpp"
#include "solution.hpp"
#include "algorithms/algorithmState.hpp"
#include "peripheral/cxxopts.hpp"

namespace kernelization {
	bool applyKernelization(AlgorithmState& state, const cxxopts::ParseResult& optionsResult);

	bool applyUnitEdgeRule(AlgorithmState&);
	bool applyEdgeDominationRule(AlgorithmState&);
	std::vector<EdgeIndex> findDominatedEdges(AlgorithmState&);
	bool edgeDominatesEdge(const Edge&, const Edge&, EdgeIndex, EdgeIndex);
	bool applyVertexDominationRule(AlgorithmState&);
	FastSet findDominatedNodes(AlgorithmState&);
	bool nodeDominatesNode(Node, Node, AlgorithmState&);
	void applyCrownDecompositionRule(AlgorithmState&);
	bool applyCriticalCoreRule(AlgorithmState&);

	bool subedgeOccursInSuperedge(const Edge&, const Edge&, const AlgorithmState&);
	std::vector<EdgeIndex> isCriticalCore(const Edge&, const AlgorithmState&);
	std::vector<EdgeIndex> calculateLink(const Edge&, const AlgorithmState&);
};