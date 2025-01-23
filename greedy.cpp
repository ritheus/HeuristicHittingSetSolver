#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "aliases.hpp"
#include "greedy.hpp"
#include "sigtermHandler.hpp"

namespace Greedy {
    std::unordered_set<Node> calculateSolution(NumNodes& n, NumEdges& m, std::vector<Edge>& setSystem) {
#if _DEBUG
        std::cout << "(using greedy algorithm...)" << std::endl;
#endif
        Hypergraph hypergraph(n, m, setSystem); // O(n * log n + m * deg_edge)
        std::unordered_set<Node> solutionSet;

        while (!hypergraph.isEmpty() && keep_running()) { // O(1)
            auto [highestPriorityNodeGain, highestPriorityNode] = hypergraph.getHighestImpactNode(); // O(1)
            solutionSet.insert(highestPriorityNode); // O(1)
            hypergraph.deleteEdgesContainingNode(highestPriorityNode); // O(deg_node * deg_edge * log n)
        } // O(n log n * deg_node * deg_edge)

        return solutionSet;
    }
}