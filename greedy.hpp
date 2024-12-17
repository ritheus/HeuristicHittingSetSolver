#pragma once

#include "aliases.hpp"
#include <vector>
#include <unordered_set>

std::vector<Node> calculateSolution(NumNodes, NumEdges, std::vector<std::vector<Node>>&);
std::vector<uint32_t> updateCount(NumNodes, NumEdges, const std::vector<std::vector<Node>>&);
std::tuple<Node, uint32_t> findMajorityNode(const std::vector<uint32_t>&);
void shrinkSolutionIfApplicable(const NumNodes, std::vector<Node>&, std::vector<std::vector<Node>>&, uint32_t, std::vector<std::vector<Node>>&);
void removeEdgesContainingNode(Node, std::vector<std::vector<Node>>&, std::vector<std::vector<Node>>&);