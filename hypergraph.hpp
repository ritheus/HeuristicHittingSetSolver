#pragma once

#include "aliases.hpp"
#include "logger.hpp"
#include "solution.hpp"
#include <unordered_set>
#include <numeric>
#include <algorithm>

struct Hypergraph {
private:
    std::vector<std::vector<Node>> edges;
    std::vector<std::vector<EdgeIndex>> incidentEdgeIndizes;
    std::vector<Node> activeNodes;
    uint32_t maximumEdgeDegree = 0;
    uint32_t maximumVertexDegree = 0;
    std::vector<uint32_t> edgeHitCounts;
    NumEdges numUnhitEdges;
    NumEdges numHitEdges;
    NumNodes n;
    NumEdges m;

public:
    Hypergraph(NumNodes n, NumNodes m, std::vector<Edge>&& setSystem) : n(n), m(m), edges(std::move(setSystem)) { // O(m * deg_E)
        edgeHitCounts.assign(this->edges.size(), 0);
        incidentEdgeIndizes.resize(n + 1); // n+1 because the given format enumerates nodes beginning with 1 and I dont want to deal with conversion
        activeNodes.resize(n + 1);
        std::iota(activeNodes.begin(), activeNodes.end(), 0);
        numUnhitEdges = m;
        numHitEdges = 0;
        for (EdgeIndex edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++) {
            for (Node node : edges[edgeIndex]) {
                incidentEdgeIndizes[node].push_back(edgeIndex);
            }
            if (edges[edgeIndex].size() > maximumEdgeDegree) {
                maximumEdgeDegree = edges[edgeIndex].size();
            }
        }
        for (Node node : activeNodes) {
            if (incidentEdgeIndizes[node].size() > maximumVertexDegree) {
                maximumVertexDegree = incidentEdgeIndizes[node].size();
            }
        }
    }

    void reset() {
		edgeHitCounts.assign(this->edges.size(), 0);
		numUnhitEdges = m;
		numHitEdges = 0;
    }

    const std::vector<Edge> getSubedgesOfDegree(uint32_t d) const {
        std::vector<Edge> subedges;

        for (EdgeIndex edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++) {
            const Edge& edge = getEdge(edgeIndex);

            if (edge.size() < d) continue;

            std::vector<size_t> indices(edge.size());
            std::iota(indices.begin(), indices.end(), 0);

            // Choose all d element combinations
            std::vector<bool> bitmask(edge.size(), false);
            std::fill(bitmask.begin(), bitmask.begin() + d, true);
            do {
                Edge subedge;
                for (size_t i = 0; i < edge.size(); ++i) {
                    if (bitmask[i]) {
                        subedge.push_back(edge[i]);
                    }
                }
                subedges.push_back(std::move(subedge));
            } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
        }

        return subedges;
    }

    /*
    * Returns true if the edge was not hit before
    */
    bool incrementHitCount(EdgeIndex edgeIndex) {
        edgeHitCounts[edgeIndex]++;
        if (edgeHitCounts[edgeIndex] == 1) {
            numUnhitEdges--;
            numHitEdges++;
            return true;
        }
        return false;
    }

    /*
    * Returns true if the edge is unhit now
    */
    bool decrementHitCount(EdgeIndex edgeIndex) {
        if (edgeHitCounts[edgeIndex] == 0) {
            throw std::runtime_error("edgeHitCount negative");
		}
        edgeHitCounts[edgeIndex]--;
        if (edgeHitCounts[edgeIndex] == 0) {
            numUnhitEdges++;
            numHitEdges--;
            return true;
        }
        return false;
    }

    void addEdge(const Edge& edge) {
		edges.push_back(edge);
        if (edge.size() > maximumEdgeDegree) {
			maximumEdgeDegree = edge.size();
		}
		for (Node node : edge) {
			incidentEdgeIndizes[node].push_back(edges.size() - 1);
            if (incidentEdgeIndizes[node].size() > maximumVertexDegree) {
                maximumVertexDegree = incidentEdgeIndizes[node].size();
            }
		}
        m++;
		numUnhitEdges++;
    }

    void deleteFromIncidentEdgeIndizes(EdgeIndex deletedEdgeIndex) {
		for (Node node : edges[deletedEdgeIndex]) {
			auto& incident = incidentEdgeIndizes[node];
			auto it = std::find(incident.begin(), incident.end(), deletedEdgeIndex);
			if (it != incident.end()) {
				incident.erase(it);
			}
		}
    }

    void removeEdgeReferencesFromNodes(EdgeIndex deletedEdgeIndex, bool& recalculateVertexDegree) {
        for (Node node : edges[deletedEdgeIndex]) {
            auto& incident = incidentEdgeIndizes[node];
            auto it = std::find(incident.begin(), incident.end(), deletedEdgeIndex);
            if (it != incident.end()) {
                incident.erase(it);
                if (incident.size() == maximumVertexDegree) {
                    recalculateVertexDegree = true;
                }
            }
        }
    }

    void swapEdges(EdgeIndex toBeRemoved, EdgeIndex lastIndex) {
        // Update edge index for swapped edge in incidentEdgeIndizes
        for (Node node : edges[lastIndex]) {
            auto& incident = incidentEdgeIndizes[node];
            auto it = std::find(incident.begin(), incident.end(), lastIndex);
            if (it != incident.end()) {
                *it = toBeRemoved;
            }
        }

        std::swap(edges[toBeRemoved], edges[lastIndex]);
        uint32_t tmp = edgeHitCounts[toBeRemoved]; // std::swap doesn't work with std::vector<bool>
        edgeHitCounts[toBeRemoved] = edgeHitCounts[lastIndex];
        edgeHitCounts[lastIndex] = tmp;
    }

    void deleteEdge(EdgeIndex edgeIndex, bool& recalculateVertexDegree, bool& recalculateEdgeDegree) {
        uint32_t lastIndex = edges.size() - 1;

        if (edges[edgeIndex].size() == maximumEdgeDegree) {
            recalculateEdgeDegree = true;
        }

        deleteFromIncidentEdgeIndizes(edgeIndex);

        if (edgeIndex != lastIndex) { // Edge must be swapped with last edge
            swapEdges(edgeIndex, lastIndex);
        }

        removeEdgeReferencesFromNodes(lastIndex, recalculateVertexDegree);

        edges.pop_back();
        if (edgeHitCounts.back())
            numHitEdges--;
        else
            numUnhitEdges--;
        edgeHitCounts.pop_back();
        m--;

        LOG("Deleted edge with index " << edgeIndex);
    }

    void deleteEdges(std::vector<EdgeIndex>& edgeIndizesToRemove) {
        bool recalculateVertexDegree = false;
        bool recalculateEdgeDegree = false;

        std::sort(edgeIndizesToRemove.begin(), edgeIndizesToRemove.end(), std::greater<>());

        for (EdgeIndex edgeIndex : edgeIndizesToRemove) {
            deleteEdge(edgeIndex, recalculateVertexDegree, recalculateEdgeDegree);
        }

        if (recalculateVertexDegree) calculateMaximumVertexDegree();
        if (recalculateEdgeDegree) calculateMaximumEdgeDegree();
    }

    void deleteNodes(const std::vector<Node>& nodesToRemove) {
        std::unordered_set<Node> toRemove(nodesToRemove.begin(), nodesToRemove.end());
        n -= toRemove.size();

        std::vector<EdgeIndex> edgeIndizesToRemove;
        bool recalcVertex = false;
        bool recalcEdge = false;

        for (Node node : toRemove) {
            if (incidentEdgeIndizes[node].size() == maximumVertexDegree) {
                recalcVertex = true;
            }

            for (EdgeIndex edgeIndex : incidentEdgeIndizes[node]) {
                auto& edge = edges[edgeIndex];

                if (edge.size() == maximumEdgeDegree)
                    recalcEdge = true;

                auto it = std::find(edge.begin(), edge.end(), node);
                edge.erase(it);

                if (edge.empty()) {
                    edgeIndizesToRemove.push_back(edgeIndex);
                }
            }

            incidentEdgeIndizes[node].clear();
        }

        activeNodes.erase(
            std::remove_if(activeNodes.begin(), activeNodes.end(),
                [&](Node n) { return toRemove.count(n); }),
            activeNodes.end()
        );

        if (!edgeIndizesToRemove.empty()) {
            deleteEdges(edgeIndizesToRemove);
        }

        if (recalcVertex) calculateMaximumVertexDegree();
        if (recalcEdge) calculateMaximumEdgeDegree();
    }


    std::unordered_set<Node> getAdjacentNodes(Node node) const {
        std::unordered_set<Node> adjacentNodes;
        adjacentNodes.reserve(incidentEdgeIndizes[node].size() * maximumEdgeDegree);

		for (EdgeIndex edgeIndex : incidentEdgeIndizes[node]) {
            for (Node adjacentNode : edges[edgeIndex]) {
				adjacentNodes.insert(adjacentNode);
            }
		}

        return adjacentNodes;
    }

    const std::vector<Edge>& getEdges() const {
        return edges;
    }

    const Edge& getEdge(EdgeIndex edgeIndex) const {
        return edges[edgeIndex];
    }

    const std::vector<Node>& getNodes() const {
        return activeNodes;
    }

    const std::vector<EdgeIndex>& getIncidentEdgeIndizes(Node node) const {
        return incidentEdgeIndizes[node];
    }

    uint32_t getMaximumVertexDegree() const {
        return maximumVertexDegree;
    }

    uint32_t getMaximumEdgeDegree() const {
        return maximumEdgeDegree;
    }

    uint32_t getM() const {
        return m;
    }

    uint32_t getN() const {
		return n;
    }

    Node getMaximumNode() const {
		return activeNodes.back();
    }

    EdgeIndex getMaximumEdgeIndex() const {
		return edges.size() - 1;
	}

    uint32_t getNumUnhitEdges() const {
        return numUnhitEdges;
    }

    void setNumUnhitEdges(uint32_t m) {
		numUnhitEdges = m;
    }

    uint32_t getNumHitEdges() const {
        return numHitEdges;
    }

    void setNumHitEdges(uint32_t m) {
        numHitEdges = m;
    }

    bool isEdgeHit(EdgeIndex edgeIndex) const {
        return edgeHitCounts[edgeIndex] > 0;
    }

    bool isSolved() const {
        return numUnhitEdges==0;
    }

    bool isSolvedBy(Solution& solution) const {
        std::vector<bool> isEdgeHit(getMaximumEdgeIndex() + 1, false);
        bool solved = true;
        for (Node node : solution.getSolution()) {
            for (EdgeIndex edgeIndex : incidentEdgeIndizes[node]) {
				isEdgeHit[edgeIndex] = true;
			}
		}
		for (EdgeIndex edgeIndex = 0; edgeIndex < edges.size(); edgeIndex++) {
			if (!isEdgeHit[edgeIndex]) {
				solved = false;
			}
        }
		return solved;
    }

    void calculateMaximumVertexDegree() {
        uint32_t oldMaximumVertexDegree = maximumVertexDegree;
        maximumVertexDegree = 0;
        for (Node node : activeNodes) {
            if (incidentEdgeIndizes[node].size() > maximumVertexDegree) {
                maximumVertexDegree = incidentEdgeIndizes[node].size();
            }
        }
	}

    void calculateMaximumEdgeDegree() {
        uint32_t oldMaximumEdgeDegree = maximumEdgeDegree;
        maximumEdgeDegree = 0;
        for (Edge& edge : edges) {
            if (edge.size() > maximumEdgeDegree) {
                maximumEdgeDegree = edge.size();
            }
        }
    }

    template <typename T>
    void removeByMask(std::vector<T>& vec, const std::vector<bool>& mask) {
        vec.erase(
            std::remove_if(vec.begin(), vec.end(),
                [i = 0, &mask](auto&&) mutable {
                    return mask[i++];
                }),
            vec.end()
        );
    }
};