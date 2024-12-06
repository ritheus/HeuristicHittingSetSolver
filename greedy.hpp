#pragma once

#include <vector>
#include <unordered_set>
#include "aliases.hpp"

std::vector<Node> calculateSolution(NumNodes, NumEdges, std::vector<std::vector<Node>>&);
std::vector<uint32_t> updateCount(NumNodes, NumEdges, const std::vector<std::vector<Node>>&);
Node findDominantNode(const std::vector<uint32_t>&);
void removeEdgesContainingNode(Node, std::vector<std::vector<Node>>&);
bool exitCondition(const std::vector<std::vector<Node>>&);