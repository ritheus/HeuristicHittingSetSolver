#pragma once

#include "aliases.hpp"
#include <vector>
#include <unordered_set>
#include "pq.hpp"

std::vector<Node> calculateSolution(NumNodes&, NumEdges&, std::vector<std::vector<Node>>&);
std::tuple<Node, std::uint32_t> findMajorityNode(const PriorityQueue<Node, uint32_t>&);
void updateCount(PriorityQueue<Node, uint32_t>&, const std::vector<std::vector<Node>>&, const std::vector<std::vector<Node>>&);
std::vector<std::vector<Node>> shrinkSolutionIfApplicable(const NumNodes, std::vector<Node>&, std::vector<std::vector<Node>>&, std::uint32_t, std::vector<std::vector<Node>>&);
std::vector<std::vector<Node>> removeEdgesContainingNode(Node, std::vector<std::vector<Node>>&, std::vector<std::vector<Node>>&);