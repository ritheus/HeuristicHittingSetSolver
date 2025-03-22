#include "hypergraph.hpp"
#include "solution.hpp"
#include "algorithmState.hpp"
#include "greedyState.hpp"
#include "updatable_priority_queue.hpp"
#include "kernelization.hpp"
#include "cxxopts.hpp"
#include "sigtermHandler.hpp"

GreedyState::GreedyState(NumNodes n, NumEdges m, std::vector<Edge>&& setSystem) : AlgorithmState(n, m, std::move(setSystem)) { // O(n * log n + m * deg_edge)
	potentialNodeImpact.reserve(n);
	for (Node node : hypergraph.getNodes()) {
		potentialNodeImpact.push(node, hypergraph.getIncidentEdgeIndizes(node).size()); // O(log n)
	} // O(n * log n)
}

std::unordered_set<Node> GreedyState::calculateSolution(GreedyState& state, cxxopts::ParseResult& optionsResult) {
	LOG("Using greedy algorithm...");

	kernelization::applyKernelization(state, optionsResult);

	while (!state.hypergraph.isEmpty() && keep_running()) { // O(1)
		auto [highestPriorityNodeGain, highestPriorityNode] = state.getHighestImpactNode(); // O(1)
		state.addToSolution(highestPriorityNode); // O(deg_node * deg_edge * log n)
	} // O(n log n * deg_node * deg_edge)

	return state.getSolution();
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

/**
*	Adds the given node to the solution, marks the incident edges as hit and updates the potential impact of all incident edges.
*/
void GreedyState::addToSolution(Node node) {
	solution.insert(node);
	for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
		if (hypergraph.setEdgeHit(edgeIndex)) { // O(deg_edge * log n)
			for (Node adjacentNode : hypergraph.getEdge(edgeIndex)) {
				updateImpact(adjacentNode, getImpact(adjacentNode) - 1); // decrement potential impact; O(log n)
			} // O(deg_edge * log n)
		}
	} // O(deg_node * deg_edge * log n)
}

std::pair<uint32_t, Node> GreedyState::getHighestImpactNode() {
	return potentialNodeImpact.top();
}

uint32_t GreedyState::getImpact(Node node) {
	return potentialNodeImpact.get_priority(node).second;
}

bool GreedyState::updateImpact(const Node& node, const uint32_t new_priority, bool only_if_higher) {
	return potentialNodeImpact.update(node, new_priority, only_if_higher); // O(log n)
}