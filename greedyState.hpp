#pragma once

#include "hypergraph.hpp"

struct GreedyState {
private:

public:
	Hypergraph hypergraph;
	std::unordered_set<Node> solutionSet;
	NumEdges m;
	pq::updatable_priority_queue<Node, uint32_t> potentialNodeImpact; // Speicher reservieren?

	GreedyState() {}

	GreedyState(NumNodes n, NumEdges m, std::vector<std::vector<Node>>& setSystem) {
		this->m = m;
		hypergraph = Hypergraph(n, m, setSystem); // O(n * log n + m * deg_edge)
		// set potentialNodeImpact
		for (EdgeIndex edgeIndex = 0; edgeIndex < hypergraph.nodesIncidentToEdge.size(); edgeIndex++) {
			for (Node node : hypergraph.nodesIncidentToEdge[edgeIndex]) {
				hypergraph.edgesIncidentToNode[node].push_back(edgeIndex);
			} // O(deg_edge)
		} // O(m * deg_edge)
		for (uint32_t i = 1; i <= n; i++) {
			potentialNodeImpact.push(i, hypergraph.edgesIncidentToNode[i].size()); // O(log n)
		} // O(n * log n)
	}

	void deleteEdgesContainingNode(Node node) {
		for (EdgeIndex edgeIndex : hypergraph.edgesIncidentToNode[node]) {
			if (hypergraph.deleteEdge(edgeIndex)) { // O(deg_edge * log n)
				for (Node node : hypergraph.nodesIncidentToEdge[edgeIndex]) {
					updateImpact(node, getImpact(node) - 1); // decrement potential impact; O(log n)
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