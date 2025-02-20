#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "aliases.hpp"
#include "adaptiveGreedy.hpp"
#include "hypergraph.hpp"
#include "sigtermHandler.hpp"
#include "adaptiveGreedyState.hpp"

namespace AdaptiveGreedy {
    std::unordered_set<Node> calculateSolution(AdaptiveGreedyState& state) {
#if _DEBUG
        std::cout << "(using adaptive greedy algorithm...)" << std::endl;
#endif
        while (!state.hypergraph.isEmpty() && keep_running()) { // O(1)
            auto [highestImpact, highestImpactNode] = state.getHighestImpactNode(); // O(1)
            state.addToSolution(highestImpactNode); // O(deg_node * (deg_node + deg_edge * log n))
            state.shrinkSolutionIfApplicable(highestImpact); // O(n_sol + deg_node * deg_edge * log n)
        } // O(n? more? * ( deg_node^2 + deg_node * deg_edge * log n))
        
        return state.solutionSet;
    }
}