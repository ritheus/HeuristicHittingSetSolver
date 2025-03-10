#pragma once

#include "hypergraph.hpp"

struct AdaptiveGreedyState {
private:

public:
	Hypergraph hypergraph;
	std::unordered_set<Node> solutionSet;
	std::vector<std::vector<EdgeIndex>> edgesHitByNode; // alternative to checking for inclusion in solutionSet in addition to hypergraph.getIncidentEdgeIndizes(node)
	std::vector<std::vector<EdgeIndex>> edgesOnlyHitByNode; // maybe unnecessary?
	std::vector<std::vector<Node>> nodesHittingEdge;
	pq::updatable_priority_queue<Node, uint32_t> solutionNodeSingleResponsibilities;
	NumEdges m;
    pq::updatable_priority_queue<Node, uint32_t> potentialNodeImpact; // Speicher reservieren?

	AdaptiveGreedyState(NumNodes n, NumEdges m, std::vector<std::vector<Node>>& setSystem) {
		this->m = m;
		edgesHitByNode.resize(n + 1);
		edgesOnlyHitByNode.resize(n + 1);
		nodesHittingEdge.resize(m);

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

    void addToSolution(Node node) {
        solutionSet.insert(node);
        edgesHitByNode[node] = hypergraph.getIncidentEdgeIndizes(node); // O(1)

        for (EdgeIndex edgeIndex : edgesHitByNode[node]) {
            nodesHittingEdge[edgeIndex].push_back(node);
            if (nodesHittingEdge[edgeIndex].size() == 1) {
                addToEdgesOnlyHitByNode(node, edgeIndex); // O(log n)
            }
            else if (nodesHittingEdge[edgeIndex].size() == 2) {
                Node otherNode = nodesHittingEdge[edgeIndex][0];
                auto it = std::find(edgesOnlyHitByNode[otherNode].begin(), edgesOnlyHitByNode[otherNode].end(), edgeIndex); // O(deg_node)
                if (it != edgesOnlyHitByNode[otherNode].end()) {
                    edgesOnlyHitByNode[otherNode].erase(it); // O(deg_node)
                }
                solutionNodeSingleResponsibilities.update(otherNode, solutionNodeSingleResponsibilities.get_priority(otherNode).second + 1); // again plus because we want a min queue; O(log n)
            }
        } // O(deg_node * (log n + deg_node))

        deleteEdgesContainingNode(node); // O(deg_node * deg_edge * log n)
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

    void removeFromSolution(Node node) {
        solutionSet.erase(node); // O(1)
        for (EdgeIndex edgeIndex : edgesHitByNode[node]) {
            nodesHittingEdge[edgeIndex].erase(std::find(nodesHittingEdge[edgeIndex].begin(), nodesHittingEdge[edgeIndex].end(), node)); // O(deg_edge)
            if (nodesHittingEdge[edgeIndex].size() == 1) {
                Node otherNode = nodesHittingEdge[edgeIndex][0];
                addToEdgesOnlyHitByNode(otherNode, edgeIndex); // O(log n)
            }
            if (nodesHittingEdge[edgeIndex].size() == 0) {
                if (hypergraph.addEdge(edgeIndex)) { // O(1)
                    for (Node node : hypergraph.nodesIncidentToEdge[edgeIndex]) {
                        updateImpact(node, getImpact(node) + 1); // increment potential impact; O(log n)
                    } // O(deg_edge * log n)
                }
            }
        } // O(deg_node) * O(deg_edge) * O(log n)

        clearEdgesHitByNode(node); // O(deg_node)
    }

    bool shrinkSolutionIfApplicable(uint32_t highestImpact) {
        Node leastImpactfulSolutionNode = solutionNodeSingleResponsibilities.top().key;
        int32_t lowestImpact = solutionNodeSingleResponsibilities.top().priority - m;

        if (-lowestImpact < highestImpact) {
            removeFromSolution(leastImpactfulSolutionNode); // O(n_sol + deg_node * deg_edge * log n)
            return true;
        }
        return false;
    }

    void addToEdgesOnlyHitByNode(Node node, EdgeIndex edgeIndex) {
        edgesOnlyHitByNode[node].push_back(edgeIndex); // O(1)
        solutionNodeSingleResponsibilities.set(node, m - 1, solutionNodeSingleResponsibilities.get_priority(node).second - 1); // minus because we want a min queue, so this node gets MORE impact and + m because the pq can only handle uint types, making m the "new 0"; O(log n)
    }

    void clearEdgesHitByNode(Node node) {
        edgesHitByNode[node].clear(); // O(deg_node)
        edgesOnlyHitByNode[node].clear(); // O(deg_node)
        solutionNodeSingleResponsibilities.remove(node);
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