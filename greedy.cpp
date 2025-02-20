#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "aliases.hpp"
#include "greedy.hpp"
#include "sigtermHandler.hpp"
#include "greedyState.hpp"

namespace Greedy {
    std::unordered_set<Node> calculateSolution(GreedyState& state) {
#if _DEBUG
        std::cout << "(using greedy algorithm...)" << std::endl;
#endif
        while (!state.hypergraph.isEmpty() && keep_running()) { // O(1)
            auto [highestPriorityNodeGain, highestPriorityNode] = state.getHighestImpactNode(); // O(1)
            state.solutionSet.insert(highestPriorityNode); // O(1)
            state.deleteEdgesContainingNode(highestPriorityNode); // O(deg_node * deg_edge * log n)
        } // O(n log n * deg_node * deg_edge)

        return state.solutionSet;
    }
}