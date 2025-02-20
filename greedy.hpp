#pragma once

#include "aliases.hpp"
#include "updatable_priority_queue.hpp"
#include "hypergraph.hpp"
#include "greedyState.hpp"
#include <vector>
#include <unordered_set>
#include <atomic>

namespace Greedy {
	std::unordered_set<Node> calculateSolution(GreedyState&);
}