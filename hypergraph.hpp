#pragma once

#include "aliases.hpp"
#include "updatable_priority_queue.hpp"
#include <unordered_set>

struct Hypergraph {
private:
    std::vector<std::vector<Node>> nodesIncidentToEdge;
    std::vector<std::vector<uint32_t>> edgesIncidentToNode;

public:
    uint32_t n;
    uint32_t m;
    std::vector<bool> edgeDeletedFlags;
    uint32_t numUnhitEdges;
    uint32_t numHitEdges;
    pq::updatable_priority_queue<Node, uint32_t> potentialNodeImpact; // Speicher reservieren?

    Hypergraph() {}

    Hypergraph(NumNodes& n, NumNodes& m, std::vector<Edge>& setSystem) {
        reset(n, m, setSystem); // O(n log n + m * deg_edge)
    }

    void reset(NumNodes& n, NumNodes& m, std::vector<Edge>& setSystem) {
        this->n = n;
        this->m = m;
        this->nodesIncidentToEdge = std::move(setSystem);
        edgeDeletedFlags.resize(this->nodesIncidentToEdge.size());
        edgesIncidentToNode.resize(n + 1); // n+1 because the given format enumerates nodes beginning with 1 and i dont want to deal with conversion
        numUnhitEdges = m;
        numHitEdges = 0;
        // set potentialNodeImpact
        std::vector<uint32_t> potentialNodeImpactBatch(n + 1, 0);
        for (EdgeIndex edgeIndex = 0; edgeIndex < this->nodesIncidentToEdge.size(); edgeIndex++) {
            for (Node node : this->nodesIncidentToEdge[edgeIndex]) {
                potentialNodeImpactBatch[node]++;
                edgesIncidentToNode[node].push_back(edgeIndex);
            } // O(deg_edge)
        } // O(m * deg_edge)
        for (uint32_t i = 1; i < potentialNodeImpactBatch.size(); i++) {
            potentialNodeImpact.push(i, potentialNodeImpactBatch[i]); // O(log n)
        } // O(n * log n)
    }

    Edge getEdge(EdgeIndex edgeIndex) {
        return nodesIncidentToEdge[edgeIndex];
    }

    std::vector<EdgeIndex> getIncidentEdgeIndizes(Node node) {
        return edgesIncidentToNode[node];
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

    /**
        Deletes all edges incident to the given node.

        It does not alter the underlying nodesIncidentToEdge or edgesIncidentToNode. It merely flips the corresponding bool in the edgeDeletedFlags vector.
        
        @return std::vector<Edge> recentlyHitEdges - edges that were removed by this invokation
    */
    void deleteEdgesContainingNode(Node node) {
        for (EdgeIndex edgeIndex : edgesIncidentToNode[node]) { 
            deleteEdge(edgeIndex); // O(deg_edge * log n)
        } // O(deg_node * deg_edge * log n)
    }

    bool deleteEdge(EdgeIndex edgeIndex) {
        if (edgeDeletedFlags[edgeIndex] == false) {
            edgeDeletedFlags[edgeIndex] = true;
            numUnhitEdges--;
            numHitEdges++;
            for (Node node : nodesIncidentToEdge[edgeIndex]) {
                updateImpact(node, getImpact(node) - 1); // decrement potential impact; O(log n)
            } // O(deg_edge * log n)
            return true;
        }
        else {
            return false;
        }
    }

    bool addEdge(EdgeIndex edgeIndex) {
        if (edgeDeletedFlags[edgeIndex] == true) {
            edgeDeletedFlags[edgeIndex] = false;
            numUnhitEdges++;
            numHitEdges--;
            for (Node node : nodesIncidentToEdge[edgeIndex]) {
                updateImpact(node, getImpact(node) + 1); // increment potential impact; O(log n)
            } // O(deg_edge * log n)
            return true;
        }
        else {
            return false;
        }
    }

    bool isEmpty() {
        return numUnhitEdges<=0;
    }
};