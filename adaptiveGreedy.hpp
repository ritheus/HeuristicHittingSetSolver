#pragma once

#include "aliases.hpp"
#include "updatable_priority_queue.hpp"
#include "hypergraph.hpp"
#include <vector>
#include <unordered_set>
#include <atomic>


namespace AdaptiveGreedy {
	extern std::atomic<bool> sigterm_received;

	std::unordered_set<Node> calculateSolution(NumNodes&, NumEdges&, std::vector<std::vector<Node>>&);
	void addToSolution(Node node);
	void shrinkSolutionIfApplicable(uint32_t);
}

namespace {
	void addToEdgesOnlyHitByNode(Node, EdgeIndex);
	void clearEdgesHitByNode(Node node);
}