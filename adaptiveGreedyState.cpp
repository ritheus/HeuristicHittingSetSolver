#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "aliases.hpp"
#include "adaptiveGreedyState.hpp"
#include "hypergraph.hpp"
#include "sigtermHandler.hpp"
#include "adaptiveGreedyState.hpp"
#include "kernelization.hpp"
#include "logger.hpp"

AdaptiveGreedyState::AdaptiveGreedyState(NumNodes n, NumEdges m, std::vector<Edge>&& setSystem) : AlgorithmState(n, m, std::move(setSystem)) { // O(n * log n + m * deg_edge)
    edgesHitByNode.resize(n + 1);
    edgesOnlyHitByNode.resize(n + 1);
    nodesHittingEdge.resize(m);

    solutionNodeSingleResponsibilities.reserve(n);
    potentialNodeImpact.reserve(n);
    for (Node node : hypergraph.getNodes()) {
        potentialNodeImpact.push(node, hypergraph.getIncidentEdgeIndizes(node).size()); // O(log n)
    } // O(n * log n)
}
std::unordered_set<Node> AdaptiveGreedyState::calculateSolution(AdaptiveGreedyState& state, cxxopts::ParseResult& optionsResult) {
	LOG("Using AdaptiveGreedy algorithm...");
    uint32_t switches = 0;

    kernelization::applyKernelization(state, optionsResult);

    while (!state.hypergraph.isEmpty() && keep_running()) { // O(1)
        auto [highestImpact, highestImpactNode] = state.getHighestImpactNode(); // O(1)
        state.addToSolution(highestImpactNode); // O(deg_node * (deg_node + deg_edge * log n))
        switches += state.shrinkSolutionIfApplicable(highestImpact); // O(deg_node * deg_edge * log n)
    } // O(n? more? * ( deg_node^2 + deg_node * deg_edge * log n))
        
	LOG("Switches: " << switches);

    return state.getSolution();
}

void AdaptiveGreedyState::addToSolution(Node node) {
    AlgorithmState::addToSolution(node);
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

    for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
        if (hypergraph.setEdgeHit(edgeIndex)) { // O(deg_edge * log n)
            for (Node node : hypergraph.getEdge(edgeIndex)) {
                updateImpact(node, getImpact(node) - 1); // decrement potential impact; O(log n)
            } // O(deg_edge * log n)
        }
    } // O(deg_node * deg_edge * log n)
}

void AdaptiveGreedyState::removeFromSolution(Node node) {
    solution.erase(node); // O(1)
    for (EdgeIndex edgeIndex : edgesHitByNode[node]) {
        nodesHittingEdge[edgeIndex].erase(std::find(nodesHittingEdge[edgeIndex].begin(), nodesHittingEdge[edgeIndex].end(), node)); // O(deg_edge)
        if (nodesHittingEdge[edgeIndex].size() == 1) {
            Node otherNode = nodesHittingEdge[edgeIndex][0];
            addToEdgesOnlyHitByNode(otherNode, edgeIndex); // O(log n)
        }
        if (nodesHittingEdge[edgeIndex].size() == 0) {
            if (hypergraph.setEdgeUnhit(edgeIndex)) { // O(1)
                for (Node node : hypergraph.getEdge(edgeIndex)) {
                    updateImpact(node, getImpact(node) + 1); // increment potential impact; O(log n)
                } // O(deg_edge * log n)
            }
        }
    } // O(deg_node) * O(deg_edge) * O(log n)

    clearEdgesHitByNode(node); // O(deg_node)
}

bool AdaptiveGreedyState::shrinkSolutionIfApplicable(uint32_t highestImpact) {
    Node leastImpactfulSolutionNode = solutionNodeSingleResponsibilities.top().key;
    int32_t lowestImpact = solutionNodeSingleResponsibilities.top().priority - hypergraph.m;

    if (-lowestImpact < highestImpact) {
        removeFromSolution(leastImpactfulSolutionNode); // O(n_sol + deg_node * deg_edge * log n)
        return true;
    }
    return false;
}

void AdaptiveGreedyState::addToEdgesOnlyHitByNode(Node node, EdgeIndex edgeIndex) {
    edgesOnlyHitByNode[node].push_back(edgeIndex); // O(1)
    solutionNodeSingleResponsibilities.set(node, hypergraph.m - 1, solutionNodeSingleResponsibilities.get_priority(node).second - 1); // minus because we want a min queue, so this node gets MORE impact and + m because the pq can only handle uint types, making m the "new 0"; O(log n)
}

void AdaptiveGreedyState::clearEdgesHitByNode(Node node) {
    edgesHitByNode[node].clear(); // O(deg_node)
    edgesOnlyHitByNode[node].clear(); // O(deg_node)
    solutionNodeSingleResponsibilities.remove(node);
}

void AdaptiveGreedyState::deleteNodes(const std::vector<Node>& nodesToRemove) {
    for (const Node node : nodesToRemove) {
        potentialNodeImpact.remove(node);
    }

    hypergraph.deleteNodes(nodesToRemove);
}

void AdaptiveGreedyState::deleteEdges(std::vector<EdgeIndex>& edgeIndizesToRemove) {
    for (EdgeIndex edgeIndexToRemove : edgeIndizesToRemove) {
        for (Node node : hypergraph.getEdge(edgeIndexToRemove)) {
            auto edgesHitByNodeIt = std::find(edgesHitByNode[node].begin(), edgesHitByNode[node].end(), edgeIndexToRemove);
            if (edgesHitByNodeIt != edgesHitByNode[node].end()) {
                edgesHitByNode[node].erase(edgesHitByNodeIt);
            }
            auto edgesOnlyHitByNodeIt = std::find(edgesOnlyHitByNode[node].begin(), edgesOnlyHitByNode[node].end(), edgeIndexToRemove);
            if (edgesOnlyHitByNodeIt != edgesOnlyHitByNode[node].end()) {
                edgesOnlyHitByNode[node].erase(edgesOnlyHitByNodeIt);
            }
            updateImpact(node, getImpact(node) - 1);
            updateResponsibility(node, getResponsibility(node) - 1);
        }
        nodesHittingEdge[edgeIndexToRemove].clear(); // das muss noch per swap + pop gelöst werden
    }
    hypergraph.deleteEdges(edgeIndizesToRemove);
}


std::pair<uint32_t, Node> AdaptiveGreedyState::getHighestImpactNode() {
    return potentialNodeImpact.top();
}

uint32_t AdaptiveGreedyState::getImpact(Node node) {
    return potentialNodeImpact.get_priority(node).second;
}

bool AdaptiveGreedyState::updateImpact(const Node& node, const uint32_t new_priority, bool only_if_higher) {
    return potentialNodeImpact.update(node, new_priority, only_if_higher); // O(log n)
}

bool AdaptiveGreedyState::updateResponsibility(Node node, int32_t new_priority, bool only_if_higher) {
    return solutionNodeSingleResponsibilities.update(node, new_priority, only_if_higher); // O(log n)
}

uint32_t AdaptiveGreedyState::getResponsibility(Node node) {
    return solutionNodeSingleResponsibilities.get_priority(node).second;
}
