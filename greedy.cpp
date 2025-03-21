#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "aliases.hpp"
#include "greedy.hpp"
#include "sigtermHandler.hpp"
#include "greedyState.hpp"
#include "kernelization.hpp"

namespace Greedy {
    std::unordered_set<Node> calculateSolution(GreedyState& state, cxxopts::ParseResult& optionsResult) {
#if _DEBUG
        std::cout << "(using greedy algorithm...)" << std::endl;
#endif
        kernelization::applyKernelization(state, optionsResult);

        while (!state.hypergraph.isEmpty() && keep_running()) { // O(1)
            auto [highestPriorityNodeGain, highestPriorityNode] = state.getHighestImpactNode(); // O(1)
            state.addToSolution(highestPriorityNode); // O(deg_node * deg_edge * log n)
        } // O(n log n * deg_node * deg_edge)

        return state.getSolution();
    }
}