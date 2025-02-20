#pragma once

#include "aliases.hpp"
#include <unordered_set>

struct Hypergraph {
public:
    uint32_t n;
    uint32_t m;
    std::vector<bool> edgeDeletedFlags;
    uint32_t numUnhitEdges;
    uint32_t numHitEdges;
    std::vector<std::vector<Node>> nodesIncidentToEdge;
    std::vector<std::vector<EdgeIndex>> edgesIncidentToNode;

    Hypergraph() {}

    Hypergraph(NumNodes n, NumNodes m, std::vector<Edge>& setSystem) {
        this->n = n;
        this->m = m;
        this->nodesIncidentToEdge = std::move(setSystem);
        edgeDeletedFlags.assign(this->nodesIncidentToEdge.size(), false);
        edgesIncidentToNode.clear();
        edgesIncidentToNode.resize(n + 1); // n+1 because the given format enumerates nodes beginning with 1 and i dont want to deal with conversion
        numUnhitEdges = m;
        numHitEdges = 0;
    }

    Hypergraph(NumNodes n, NumNodes m, std::vector<Edge>& setSystem, uint32_t FAKE) {
        this->n = n;
        this->m = m;
        this->nodesIncidentToEdge = std::move(setSystem);
        edgeDeletedFlags.assign(this->nodesIncidentToEdge.size(), false);
        edgesIncidentToNode.clear();
        edgesIncidentToNode.resize(n + 1); // n+1 because the given format enumerates nodes beginning with 1 and i dont want to deal with conversion
        numUnhitEdges = m;
        numHitEdges = 0;
        for (EdgeIndex edgeIndex = 0; edgeIndex < nodesIncidentToEdge.size(); edgeIndex++) {
            for (Node node : nodesIncidentToEdge[edgeIndex]) {
                edgesIncidentToNode[node].push_back(edgeIndex);
            } // O(deg_edge)
        } // O(m * deg_edge)
    }

    Edge getEdge(EdgeIndex edgeIndex) {
        return nodesIncidentToEdge[edgeIndex];
    }

    std::vector<EdgeIndex> getIncidentEdgeIndizes(Node node) {
        return edgesIncidentToNode[node];
    }

    /**
        Deletes an edge given by edgeIndex.

        It does not alter the underlying nodesIncidentToEdge or edgesIncidentToNode. It merely flips the corresponding bool in the edgeDeletedFlags vector.
        
        @return true if the edge existed and got deleted by this invokation and false otherwise
    */
    bool deleteEdge(EdgeIndex edgeIndex) {
        if (edgeDeletedFlags[edgeIndex] == false) {
            edgeDeletedFlags[edgeIndex] = true;
            numUnhitEdges--;
            numHitEdges++;
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