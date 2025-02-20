#pragma once

#include "aliases.hpp"
#include "updatable_priority_queue.hpp"
#include "hypergraph.hpp"
#include "adaptiveGreedyState.hpp"
#include <vector>
#include <unordered_set>
#include <atomic>

namespace AdaptiveGreedy {
	std::unordered_set<Node> calculateSolution(AdaptiveGreedyState&);
}