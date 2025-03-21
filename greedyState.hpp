#pragma once

#include "hypergraph.hpp"
#include "solution.hpp"
#include "algorithmState.hpp"

struct GreedyState : AlgorithmState {
public:
	pq::updatable_priority_queue<Node, uint32_t> potentialNodeImpact;

	GreedyState(NumNodes n, NumEdges m, std::vector<Edge>&& setSystem) : AlgorithmState(n, m, std::move(setSystem)) { // O(n * log n + m * deg_edge)
		potentialNodeImpact.reserve(n);
		for (Node node : hypergraph.getNodes()) {
			potentialNodeImpact.push(node, hypergraph.getIncidentEdgeIndizes(node).size()); // O(log n)
		} // O(n * log n)
	}

	void deleteNodes(const std::vector<Node>& nodesToRemove) override {
		for (const Node node : nodesToRemove) {
			potentialNodeImpact.remove(node);
		}

		hypergraph.deleteNodes(nodesToRemove);
	}

	void deleteEdges(std::vector<EdgeIndex>& edgeIndizesToRemove) override {
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
	void addToSolution(Node node) override {
		solution.insert(node);
		for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
			if (hypergraph.setEdgeHit(edgeIndex)) { // O(deg_edge * log n)
				for (Node adjacentNode : hypergraph.getEdge(edgeIndex)) {
					updateImpact(adjacentNode, getImpact(adjacentNode) - 1); // decrement potential impact; O(log n)
				} // O(deg_edge * log n)
			}
		} // O(deg_node * deg_edge * log n)
	}

	std::pair<uint32_t, Node> getHighestImpactNode() {
		return potentialNodeImpact.top();
	}

	uint32_t getImpact(Node node) {
		return potentialNodeImpact.get_priority(node).second;
	}

	bool updateImpact(const Node& node, const uint32_t new_priority, bool only_if_higher = false) {
		return potentialNodeImpact.update(node, new_priority, only_if_higher); // O(log n)
	}
};