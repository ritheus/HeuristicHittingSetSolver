#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "aliases.hpp"
#include "greedy.hpp"

std::vector<Node> calculateSolution(NumNodes n, NumEdges m, std::vector<std::vector<Node>>& setSystem) {
#if _DEBUG
    std::cout << "(using greedy algorithm...)" << std::endl;
#endif

    std::vector<std::vector<Node>> removedEdges;
    std::vector<uint32_t> count(n);
    std::vector<Node> solutionSet;

    while (!setSystem.empty()) { // O(1)
        count = updateCount(n, m, setSystem); // O(n * m)
        auto [majorityNode, majorityNodeGain] = findMajorityNode(count); // O(n)
        solutionSet.push_back(majorityNode); // O(1)
        removeEdgesContainingNode(majorityNode, setSystem, removedEdges); // O(n * m), wenn man edges als sets implementiert, wird das O(log n * m)...
        shrinkSolutionIfApplicable(n, solutionSet, removedEdges, majorityNodeGain, setSystem);
    } // und das alles noch mal O(n)

    return solutionSet;
}

std::vector<uint32_t> updateCount(NumNodes n, NumEdges m, const std::vector<std::vector<Node>>& setSystem) {
    std::vector<uint32_t> count(n, 0);

    for (auto&& edge : setSystem) {
        for (Node node : edge) {
            count[node - 1]++;
        }
    }

    return count;
}

std::tuple<Node, uint32_t> findMajorityNode(const std::vector<uint32_t>& count) {
    Node maxNode = 1;
    uint32_t maxOccurences = count[maxNode - 1];
    for (uint32_t i = 0; i < count.size(); i++) {
        if (count[i] > maxOccurences) {
            maxOccurences = count[i];
            maxNode = i + 1;
        }
    }
    return std::tuple<Node, uint32_t>(maxNode, maxOccurences);
}

void shrinkSolutionIfApplicable(const NumNodes n, std::vector<Node>& solutionSet, std::vector<std::vector<Node>>& removedEdges, uint32_t majorityNodeGain, std::vector<std::vector<Node>>& setSystem) {
    Node minorityNode;
    uint32_t minorityNodeLoss = removedEdges.size();
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
            }
            setSystem.push_back(edge);
        }
    }
}

void removeEdgesContainingNode(Node node, std::vector<std::vector<Node>>& setSystem, std::vector<std::vector<Node>>& removedEdges) {
    setSystem.erase(
        std::remove_if(setSystem.begin(), setSystem.end(),
            [&](auto& edge) {
                if (std::find(edge.begin(), edge.end(), node) != edge.end()) {
                    removedEdges.push_back(std::move(edge));
                    return true; // Mark for removal
                }
                return false; 
            }),
        setSystem.end());
}