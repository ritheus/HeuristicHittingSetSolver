#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "aliases.hpp"
#include "greedy.hpp"

namespace Greedy {
    std::atomic<bool> sigterm_received(false);

    std::vector<Node> calculateSolution(NumNodes& n, NumEdges& m, std::vector<std::vector<Node>>& setSystem) {
#if _DEBUG
        std::cout << "(using greedy algorithm...)" << std::endl;
#endif
        std::vector<std::vector<Node>> recentlyRemovedEdges;
        std::unordered_set<uint32_t> removedEdgesIndizes;
        pq::updatable_priority_queue<Node, uint32_t> count; // Speicher reservieren?
        std::vector<std::vector<uint32_t>> invertedSetSystem(n+1); // n+1 because the given format enumerates nodes beginning with 1 and i dont want to deal with conversion
        setCount(count, setSystem, invertedSetSystem); // O(n * m * log n)
        std::vector<Node> solutionSet;

        while (removedEdgesIndizes.size() < setSystem.size() && !sigterm_received) { // O(1)
            auto [majorityNodeGain, majorityNode] = count.top(); // O(1)
            solutionSet.push_back(majorityNode); // O(1)
            if (sigterm_received) break;
            recentlyRemovedEdges = removeEdgesContainingNode(majorityNode, setSystem, invertedSetSystem, removedEdgesIndizes); // O(n * m), wenn man edges als sets implementiert, wird das O(log n * m)...
            updateCount(count, recentlyRemovedEdges); // O(??? * log n)
        } // und das alles noch mal O(n)

        return solutionSet;
    }

    void setCount(pq::updatable_priority_queue<Node, uint32_t>& count, const std::vector<std::vector<Node>>& setSystem, std::vector<std::vector<uint32_t>>& invertedSetSystem) {
        for (uint32_t i = 0; i < setSystem.size(); i++) {
            for (Node node : setSystem[i]) {
                count.set(node, count.get_priority(node).second + 1); // increment priority
                invertedSetSystem[node].push_back(i);
            }
        }
    }

    void updateCount(pq::updatable_priority_queue<Node, uint32_t>& count, const std::vector<std::vector<Node>>& recentlyRemovedEdges) {
        for (auto&& edge : recentlyRemovedEdges) {
            for (Node node : edge) {
                count.update(node, count.get_priority(node).second - 1); // decrement priority
            }
        }
    }

    std::vector<std::vector<Node>> removeEdgesContainingNode(Node node, std::vector<std::vector<Node>>& setSystem, std::vector<std::vector<uint32_t>>& invertedSetSystem, std::unordered_set<uint32_t>& removedEdgesIndizes) {
        std::vector<std::vector<Node>> recentlyRemovedEdges;

        for (uint32_t edgeIndex : invertedSetSystem[node]) {
            if (removedEdgesIndizes.insert(edgeIndex).second) {
                recentlyRemovedEdges.push_back(setSystem[edgeIndex]);
            }
        }

        return recentlyRemovedEdges;
    }
}