#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "aliases.hpp"
#include "adaptiveGreedy.hpp"
#include "hypergraph.hpp"
#include "sigtermHandler.hpp"
#include "adaptiveGreedyState.hpp"
#include "kernelization.hpp"
#include "logger.hpp"

namespace AdaptiveGreedy {
    std::unordered_set<Node> calculateSolution(AdaptiveGreedyState& state, cxxopts::ParseResult& optionsResult) {
#if _DEBUG
        std::cout << "(using adaptive greedy algorithm...)" << std::endl;
#endif // _DEBUG
        uint32_t switches = 0;

        //kernelization::applyKernelization(state, optionsResult);

        while (!state.hypergraph.isEmpty() && keep_running()) { // O(1)
            auto [highestImpact, highestImpactNode] = state.getHighestImpactNode(); // O(1)
            state.addToSolution(highestImpactNode); // O(deg_node * (deg_node + deg_edge * log n))
            switches += state.shrinkSolutionIfApplicable(highestImpact); // O(deg_node * deg_edge * log n)
        } // O(n? more? * ( deg_node^2 + deg_node * deg_edge * log n))
        
		LOG("Switches: " << switches);

        return state.getSolution();
    }
}