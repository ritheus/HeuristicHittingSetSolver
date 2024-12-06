#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "../aliases.hpp"
#include "greedy.hpp"

std::vector<Node> calculateSolution(NumNodes n, NumEdges m, std::vector<std::vector<Node>>& setSystem) {
    std::cout << "(using greedy algorithm...)" << std::endl;

    std::vector<uint32_t> count(m);
    std::vector<Node> solutionSet;
    Node dominantNode;

    while (!exitCondition(setSystem)) { // Das sollte irgendwie einfacher gehen, schau ich noch
        count = updateCount(n, m, setSystem);
        dominantNode = findDominantNode(count);
        solutionSet.push_back(dominantNode);
        removeEdgesContainingNode(dominantNode, setSystem);
    }

    return solutionSet;
}

std::vector<uint32_t> updateCount(NumNodes n, NumEdges m, const std::vector<std::vector<Node>>& setSystem) {
    std::vector<uint32_t> count(m);

    for (auto&& edge : setSystem) {
        for (Node node : edge) {
            count[node-1]++;
        }
    }

    return count;
}

Node findDominantNode(const std::vector<uint32_t>& count) {
    Node maxNode = 1;
    uint32_t maxOccurences = 0;
    for (uint32_t i=0; i<count.size(); i++) {
        if (count[i] > maxOccurences) {
            maxOccurences = count[i];
            maxNode = i+1;
        }
    }
    return maxNode;
}

void removeEdgesContainingNode(Node node, std::vector<std::vector<Node>>& setSystem) {
    for (auto&& edge : setSystem) {
        bool cond = find(edge.begin(), edge.end(), node) != edge.end();
        if (cond) {
            edge = std::vector<Node>(0);
        }
    }
}

bool exitCondition(const std::vector<std::vector<Node>>& setSystem) {
    for (auto&& edge : setSystem) {
        if (!edge.empty()) { // edge does not exist anymore because it contained a node that got deleted
            return false;
        }
    }
    return true;
}