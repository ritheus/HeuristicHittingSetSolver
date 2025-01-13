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
        std::vector<std::vector<Node>> removedEdges;
        std::vector<std::vector<Node>> recentlyRemovedEdges;
        pq::updatable_priority_queue<Node, uint32_t> count; // Speicher reservieren?
        setCount(count, setSystem); // O(n * m * log n)
        std::vector<Node> solutionSet;

        while (!setSystem.empty() && !sigterm_received) { // O(1)
            updateCount(count, recentlyRemovedEdges); // O(??? * log n)
            auto [majorityNodeGain, majorityNode] = count.top(); // O(1)
            solutionSet.push_back(majorityNode); // O(1)
            if (sigterm_received) break;
            recentlyRemovedEdges = removeEdgesContainingNode(majorityNode, setSystem, removedEdges); // O(n * m), wenn man edges als sets implementiert, wird das O(log n * m)...
        } // und das alles noch mal O(n)

        return solutionSet;
    }

    void setCount(pq::updatable_priority_queue<Node, uint32_t>& count, const std::vector<std::vector<Node>>& setSystem) {
        for (auto&& edge : setSystem) {
            for (Node node : edge) {
                count.set(node, count.get_priority(node).second + 1); // increment priority
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

    std::vector<std::vector<Node>> removeEdgesContainingNode(Node node, std::vector<std::vector<Node>>& setSystem, std::vector<std::vector<Node>>& removedEdges) {
        std::vector<std::vector<Node>> recentlyRemovedEdges;

        setSystem.erase(
            std::remove_if(setSystem.begin(), setSystem.end(),
                [&](auto& edge) {
                    if (std::find(edge.begin(), edge.end(), node) != edge.end()) {
                        recentlyRemovedEdges.push_back(edge);
                        removedEdges.push_back(std::move(edge));
                        return true; // Mark for removal
                    }
                    return false;
                }),
            setSystem.end());

        return recentlyRemovedEdges;
    }
}