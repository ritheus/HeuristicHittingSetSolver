#pragma once

#include "aliases.hpp"
#include "pq.hpp"
#include <vector>
#include <unordered_set>
#include <atomic>


extern std::atomic<bool> sigterm_received;

std::vector<Node> calculateSolution(NumNodes&, NumEdges&, std::vector<std::vector<Node>>&);
std::tuple<Node, uint32_t> findMajorityNode(const PriorityQueue<Node, uint32_t>&);
void updateCount(PriorityQueue<Node, uint32_t>&, const std::vector<std::vector<Node>>&, const std::vector<std::vector<Node>>&);
std::vector<std::vector<Node>> shrinkSolutionIfApplicable(const NumNodes, std::vector<Node>&, std::vector<std::vector<Node>>&, uint32_t, std::vector<std::vector<Node>>&);
std::vector<std::vector<Node>> removeEdgesContainingNode(Node, std::vector<std::vector<Node>>&, std::vector<std::vector<Node>>&);