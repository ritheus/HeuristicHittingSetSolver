#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "aliases.hpp"
#include "adaptiveGreedy.hpp"
#include "hypergraph.hpp"
#include "sigtermHandler.hpp"

Hypergraph hypergraph;
std::unordered_set<Node> solutionSet;
std::vector<std::vector<EdgeIndex>> edgesHitByNode; // alternative to checking for inclusion in solutionSet in addition to hypergraph.getIncidentEdgeIndizes(node)
std::vector<std::vector<EdgeIndex>> edgesOnlyHitByNode; // maybe unnecessary?
std::vector<std::vector<Node>> nodesHittingEdge;
pq::updatable_priority_queue<Node, uint32_t> solutionNodeSingleResponsibilities;
uint32_t m;

namespace AdaptiveGreedy {
    std::unordered_set<Node> calculateSolution(NumNodes& n, NumEdges& m, std::vector<std::vector<Node>>& setSystem) {
#if _DEBUG
        std::cout << "(using adaptive greedy algorithm...)" << std::endl;
#endif
        ::m = m;
        edgesHitByNode.resize(n + 1);
        edgesOnlyHitByNode.resize(n + 1);
        nodesHittingEdge.resize(m);

        hypergraph.reset(n, m, setSystem); // O(n * log n + m * deg_edge)

        while (!hypergraph.isEmpty() && keep_running()) { // O(1)
            auto [highestImpact, highestImpactNode] = hypergraph.getHighestImpactNode(); // O(1)
            addToSolution(highestImpactNode); // O(deg_node * (deg_node + deg_edge * log n))
            if (!keep_running()) break;
            shrinkSolutionIfApplicable(highestImpact); // O(n_sol + deg_node * deg_edge * log n)
        } // O(n? more? * ( deg_node^2 + deg_node * deg_edge * log n))
        

        return solutionSet;
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
                edgesOnlyHitByNode[otherNode].erase(
                    std::remove(edgesOnlyHitByNode[otherNode].begin(), edgesOnlyHitByNode[otherNode].end(), edgeIndex),
                    edgesOnlyHitByNode[otherNode].end()
                ); // O(deg_node)
                solutionNodeSingleResponsibilities.update(otherNode, solutionNodeSingleResponsibilities.get_priority(otherNode).second + 1); // again plus because we want a min queue; O(log n)
            }
        } // O(deg_node * (log n + deg_node))
        
        hypergraph.deleteEdgesContainingNode(node); // O(deg_node * deg_edge * log n)
    }

    void removeFromSolution(Node node) {
        auto it = std::find(solutionSet.begin(), solutionSet.end(), node); // O(n_sol)
        if (it != solutionSet.end()) {
            solutionSet.erase(it);
        }
        for (EdgeIndex edgeIndex : edgesHitByNode[node]) {
            nodesHittingEdge[edgeIndex].erase(std::find(nodesHittingEdge[edgeIndex].begin(), nodesHittingEdge[edgeIndex].end(), node)); // O(deg_edge)
            if (nodesHittingEdge[edgeIndex].size() == 1) {
                Node otherNode = nodesHittingEdge[edgeIndex][0];
                addToEdgesOnlyHitByNode(otherNode, edgeIndex); // O(log n)
            }
            if (nodesHittingEdge[edgeIndex].size() == 0) {
                hypergraph.addEdge(edgeIndex); // O(deg_edge) * O(log n)
            }
        } // O(n_sol) + O(deg_node) * O(deg_edge) * O(log n)

        clearEdgesHitByNode(node); // O(deg_node)
    }

    void shrinkSolutionIfApplicable(uint32_t highestImpact) {
        Node leastImpactfulSolutionNode = solutionNodeSingleResponsibilities.top().key;
        int32_t lowestImpact = solutionNodeSingleResponsibilities.top().priority - m;

        if (-lowestImpact < highestImpact) {
            removeFromSolution(leastImpactfulSolutionNode); // O(n_sol + deg_node * deg_edge * log n)
        }
    }
}

namespace {
    void addToEdgesOnlyHitByNode(Node node, EdgeIndex edgeIndex) {
        edgesOnlyHitByNode[node].push_back(edgeIndex); // O(1)
        solutionNodeSingleResponsibilities.set(node, m - 1, solutionNodeSingleResponsibilities.get_priority(node).second - 1); // minus because we want a min queue, so this node gets MORE impact and + m because the pq can only handle uint types, making m the "new 0"; O(log n)
    }

    void clearEdgesHitByNode(Node node) {
        edgesHitByNode[node].clear(); // O(deg_node)
        edgesOnlyHitByNode[node].clear(); // O(deg_node)
        if (solutionNodeSingleResponsibilities.top().key == node) {
            solutionNodeSingleResponsibilities.pop();
        }
    }
}