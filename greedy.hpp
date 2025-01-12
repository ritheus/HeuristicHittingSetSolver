#pragma once

#include "aliases.hpp"
#include "updatable_priority_queue.hpp"
#include <vector>
#include <unordered_set>
#include <atomic>


namespace Greedy {
	extern std::atomic<bool> sigterm_received;

	std::vector<Node> calculateSolution(NumNodes&, NumEdges&, std::vector<std::vector<Node>>&);
	std::tuple<Node, uint32_t> findMajorityNode(const pq::updatable_priority_queue<Node, uint32_t>&);
	void setCount(pq::updatable_priority_queue<Node, uint32_t>&, const std::vector<std::vector<Node>>&);
	void updateCount(pq::updatable_priority_queue<Node, uint32_t>&, const std::vector<std::vector<Node>>&);
	std::vector<std::vector<Node>> removeEdgesContainingNode(Node, std::vector<std::vector<Node>>&, std::vector<std::vector<Node>>&);
}