#include "hypergraph.hpp"
#include "solution.hpp"
#include "algorithmState.hpp"
#include "greedyState.hpp"
#include "updatable_priority_queue.hpp"
#include "kernelization.hpp"
#include "cxxopts.hpp"
#include "sigtermHandler.hpp"

GreedyState::GreedyState(NumNodes n, NumEdges m, std::vector<Edge>&& setSystem, const cxxopts::ParseResult& optionsResult) : AlgorithmState(n, m, std::move(setSystem), optionsResult) { // O(n * log n + m * deg_edge)
	potentialNodeImpact.reserve(n);
	for (Node node : hypergraph.getNodes()) {
		potentialNodeImpact.push(node, hypergraph.getIncidentEdgeIndizes(node).size()); // O(log n)
	} // O(n * log n)
}

GreedyState::GreedyState(Hypergraph& hypergraph, const cxxopts::ParseResult& optionsResult) : AlgorithmState(hypergraph, optionsResult) { // O(n * log n + m * deg_edge)
	potentialNodeImpact.reserve(hypergraph.getN());
	for (Node node : hypergraph.getNodes()) {
		potentialNodeImpact.push(node, hypergraph.getIncidentEdgeIndizes(node).size()); // O(log n)
	} // O(n * log n)
}

Solution GreedyState::calculateSolution(bool applyKernelization) {
	LOG("Using greedy algorithm...");

	if (applyKernelization) kernelization::applyKernelization(*this, optionsResult);

	while (!hypergraph.isSolved() && keep_running()) { // O(1)
		//uint32_t oldSolutionSize = getSolution().size();
		auto [highestPriorityNodeGain, highestPriorityNode] = getHighestImpactNode(); // O(1)
		addToSolution(highestPriorityNode); // O(deg_node * deg_edge * log n)
		//if (oldSolutionSize == getSolution().size()) {
		//	return {};
		//}
	} // O(n log n * deg_node * deg_edge)

	return getSolution();
}

std::vector<Node> GreedyState::repairSolution() {
	std::vector<Node> addedNodes;
	addedNodes.reserve(hypergraph.getNumUnhitEdges());
	while (!hypergraph.isSolved() && keep_running()) { // O(1)
		auto [highestPriorityNodeGain, highestPriorityNode] = getHighestImpactNode();
		addToSolution(highestPriorityNode); // O(deg_node * deg_edge * log n)
		addedNodes.push_back(highestPriorityNode);
	} // O(n log n * deg_node * deg_edge)

	return addedNodes;
}

void GreedyState::deleteNodes(const std::vector<Node>& nodesToRemove) {
	for (const Node node : nodesToRemove) {
		potentialNodeImpact.remove(node);
	}

	hypergraph.deleteNodes(nodesToRemove);
}

void GreedyState::deleteEdges(std::vector<EdgeIndex>& edgeIndizesToRemove) {
	for (EdgeIndex edgeIndexToRemove : edgeIndizesToRemove) {
		for (Node node : hypergraph.getEdge(edgeIndexToRemove)) {
			updateImpact(node, getImpact(node) - 1);
		}
	}
	hypergraph.deleteEdges(edgeIndizesToRemove);
}

void GreedyState::setSolution(Solution& newSolution) {
	std::vector<Node> nodesToRemove;
	std::vector<Node> nodesToAdd;
	for (Node node : solution.getSolution()) {
		if (!newSolution.contains(node))
		{
			nodesToRemove.push_back(node);
		}
	}
	for (Node node : nodesToRemove) {
		removeFromSolution(node);
	}
	for (Node node : newSolution.getSolution()) {
		if (!solution.contains(node))
		{
			nodesToAdd.push_back(node);
		}
	}
	for (Node node : nodesToAdd) {
		addToSolution(node);
	}
	if (solution.size() != newSolution.size()) {
		throw std::runtime_error("Solution size does not match");
	}
}

/**
*	Adds the given node to the solution, marks the incident edges as hit and updates the potential impact of all incident edges.
*/
bool GreedyState::addToSolution(Node node) {
	if (solution.contains(node)) return false;
	updateNodeAge(node);
	solution.insert(node);
	for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
		if (hypergraph.incrementHitCount(edgeIndex)) { // O(deg_edge * log n)
			for (Node adjacentNode : hypergraph.getEdge(edgeIndex)) {
				updateImpact(adjacentNode, getImpact(adjacentNode) - 1); // decrement potential impact; O(log n)
			} // O(deg_edge * log n)
		}
	} // O(deg_node * deg_edge * log n)
	return true;
}

bool GreedyState::removeFromSolution(Node node) {
	if (!solution.contains(node)) return false;
	solution.erase(node);
	for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
		if (hypergraph.decrementHitCount(edgeIndex)) { // O(deg_edge * log n)
			for (Node adjacentNode : hypergraph.getEdge(edgeIndex)) {
				updateImpact(adjacentNode, getImpact(adjacentNode) + 1); // increment potential impact; O(log n)
			} // O(deg_edge * log n)
		}
	}
	return true;
}

void GreedyState::banFromSolution(Node node) {
	bannedNodes.insert(node);
	potentialNodeImpact.remove(node);
}

std::pair<uint32_t, Node> GreedyState::getHighestImpactNode() {
	return potentialNodeImpact.top();
	/*std::vector<std::pair<uint32_t, Node>> toBeReinserted;
	do {
		Node node = potentialNodeImpact.top().key;
		if (std::find(bannedNodes.begin(), bannedNodes.end(), node) != bannedNodes.end()) {
			toBeReinserted.push_back(potentialNodeImpact.top());
			potentialNodeImpact.pop();
		}
		else {
			for (auto&& pair : toBeReinserted) {
				potentialNodeImpact.push(pair.second, pair.first);
			}
			return potentialNodeImpact.top();
		}
	} while (potentialNodeImpact.size() > 0);
	throw std::runtime_error("empty priority queue");*/
}

uint32_t GreedyState::getImpact(Node node) {
	return potentialNodeImpact.get_priority(node).second;
}

bool GreedyState::updateImpact(const Node& node, const uint32_t new_priority, bool only_if_higher, bool only_non_tabu) {
	if (!only_non_tabu) {
		return potentialNodeImpact.update(node, new_priority, only_if_higher); // O(log n)
	}
	else {
		if (std::find(bannedNodes.begin(), bannedNodes.end(), node) != bannedNodes.end()) {
			return false;
		}
		else {
			return potentialNodeImpact.update(node, new_priority, only_if_higher);
		}
	}
}

void GreedyState::removeFromPQ(Node node) {
	potentialNodeImpact.remove(node);
}