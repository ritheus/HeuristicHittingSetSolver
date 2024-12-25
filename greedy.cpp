#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "aliases.hpp"
#include "greedy.hpp"

std::atomic<bool> sigterm_received(false);

std::vector<Node> calculateSolution(NumNodes& n, NumEdges& m, std::vector<std::vector<Node>>& setSystem) {
#if _DEBUG
    std::cout << "(using greedy algorithm...)" << std::endl;
#endif

    std::vector<std::vector<Node>> removedEdges;
    std::vector<std::vector<Node>> recentlyRemovedEdges;
    std::vector<std::vector<Node>> recentlyAddedEdges;
    PriorityQueue<Node, uint32_t> count(n);
    updateCount(count, setSystem, recentlyRemovedEdges); // O(n * m * log n)
    std::vector<Node> solutionSet;

    while (!setSystem.empty() && !sigterm_received) { // O(1)
        updateCount(count, recentlyAddedEdges, recentlyRemovedEdges); // O(??? * log n)
        auto [majorityNode, majorityNodeGain] = count.topPair(); // O(1)
        solutionSet.push_back(majorityNode); // O(1)
        if (sigterm_received) break;
        recentlyRemovedEdges = removeEdgesContainingNode(majorityNode, setSystem, removedEdges); // O(n * m), wenn man edges als sets implementiert, wird das O(log n * m)...
        if (sigterm_received) break;
        recentlyAddedEdges = shrinkSolutionIfApplicable(n, solutionSet, removedEdges, majorityNodeGain, setSystem);
    } // und das alles noch mal O(n)

    return solutionSet;
}

void updateCount(PriorityQueue<Node, uint32_t>& count, const std::vector<std::vector<Node>>& recentlyAddedEdges, const std::vector<std::vector<Node>>& recentlyRemovedEdges) {
    for (auto&& edge : recentlyAddedEdges) {
        for (Node node : edge) {
            count.increment(node);
        }
    }
    for (auto&& edge : recentlyRemovedEdges) {
        for (Node node : edge) {
            count.decrement(node);
        }
    }
}

std::vector<std::vector<Node>> shrinkSolutionIfApplicable(const NumNodes n, std::vector<Node>& solutionSet, std::vector<std::vector<Node>>& removedEdges, uint32_t majorityNodeGain, std::vector<std::vector<Node>>& setSystem) {
    Node minorityNode;
    size_t minorityNodeLoss = removedEdges.size();
    std::vector<std::vector<Node>> recentlyAddedEdges;
    std::vector<std::vector<Node>> removedEdgesForNode;
    std::vector<std::vector<Node>> removedEdgesForMinorityNode;

    for (Node node : solutionSet) { // For every node in solutionSet check what happens when deleted. 
        removedEdgesForNode = std::vector<std::vector<Node>>();
        bool containsOtherNodesFromSolution = false;
        // Take all previously removed edges that contain the node ...
        for (auto&& edge : removedEdges) {
            if (std::find(edge.begin(), edge.end(), node) != edge.end()) {
                // ... but dont contain another node from solutionSet.
                for (Node otherNode : solutionSet) {
                    if (otherNode == node) {
                        continue;
                    }
                    if (std::find(edge.begin(), edge.end(), otherNode) != edge.end()) {
                        containsOtherNodesFromSolution = true;
                    }
                }
                if (!containsOtherNodesFromSolution) {
                    removedEdgesForNode.push_back(edge);
                }
                containsOtherNodesFromSolution = false;
            }
        }

        // We only care for the smallest loss.
        if (removedEdgesForNode.size() < minorityNodeLoss) {
            minorityNode = node;
            minorityNodeLoss = removedEdgesForNode.size();
            removedEdgesForMinorityNode = removedEdgesForNode;
        }
    }
    // If exchanging minorityNode for majorityNode would be a net gain ...
    if (minorityNodeLoss < majorityNodeGain) {
        // ... erase minorityNode from the solution.
        auto it = std::find(solutionSet.begin(), solutionSet.end(), minorityNode);
        if (it != solutionSet.end()) {
            solutionSet.erase(it);
        }
        for (auto& edge : removedEdgesForMinorityNode) {
            auto it = std::find(removedEdges.begin(), removedEdges.end(), edge);
            if (it != removedEdges.end()) {
                removedEdges.erase(it);
                recentlyAddedEdges.push_back(edge);
            }
            setSystem.push_back(edge);
        }
    }

    return recentlyAddedEdges;
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