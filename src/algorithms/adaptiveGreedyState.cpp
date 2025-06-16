#include <vector>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include "algorithms/adaptiveGreedyState.hpp"
#include "hypergraph.hpp"
#include "kernelization.hpp"
#include "peripheral/aliases.hpp"
#include "peripheral/sigtermHandler.hpp"
#include "peripheral/logger.hpp"

AdaptiveGreedyState::AdaptiveGreedyState(NumNodes n, NumEdges m, std::vector<Edge>&& setSystem, const cxxopts::ParseResult& optionsResult) : AlgorithmState(n, m, std::move(setSystem), optionsResult) { // O(n * log n + m * deg_edge)
    nodesHittingEdge.resize(m);

    solutionNodeSingleResponsibilities.reserve(n);
    potentialNodeImpact.reserve(n);
    for (Node node : hypergraph.getNodes()) {
        potentialNodeImpact.push(node, hypergraph.getIncidentEdgeIndizes(node).size()); // O(log n)
    } // O(n * log n)
}

AdaptiveGreedyState::AdaptiveGreedyState(Hypergraph& hypergraph, const cxxopts::ParseResult& optionsResult) : AlgorithmState(hypergraph, optionsResult) { // O(n * log n + m * deg_edge)
    nodesHittingEdge.resize(hypergraph.getMaximumEdgeIndex() + 1);

    solutionNodeSingleResponsibilities.reserve(hypergraph.getMaximumNode());
    potentialNodeImpact.reserve(hypergraph.getMaximumNode());
    for (Node node : hypergraph.getNodes()) {
        potentialNodeImpact.push(node, hypergraph.getIncidentEdgeIndizes(node).size()); // O(log n)
    } // O(n * log n)
}

Solution AdaptiveGreedyState::calculateSolution(bool applyKernelization) {
	LOG("Using AdaptiveGreedy algorithm...");
    uint32_t switches = 0;

    if (applyKernelization) kernelization::applyKernelization(*this, optionsResult);

    while (!hypergraph.isSolved() && keep_running()) { // O(1)
        auto [highestImpact, highestImpactNode] = getHighestImpactNode(); // O(1)
        addToSolution(highestImpactNode); // O(deg_node * (deg_node + deg_edge * log n))
        switches += shrinkSolutionIfApplicable(highestImpact); // O(deg_node * deg_edge * log n)
    } // O(n? more? * ( deg_node^2 + deg_node * deg_edge * log n))
        
	LOG("Switches: " << switches);

    return getSolution();
}

bool AdaptiveGreedyState::addToSolution(Node node) {
    if (solution.contains(node)) return false;
    updateNodeAge(node);
    solution.insert(node);

    for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
        nodesHittingEdge[edgeIndex].push_back(node);
        if (nodesHittingEdge[edgeIndex].size() == 1) {
            addToEdgesOnlyHitByNode(node, edgeIndex); // O(log n)
        }
        else if (nodesHittingEdge[edgeIndex].size() == 2) {
            Node otherNode = nodesHittingEdge[edgeIndex][0];
            solutionNodeSingleResponsibilities.update(otherNode, solutionNodeSingleResponsibilities.get_priority(otherNode).second + 1); // again plus because we want a min queue; O(log n)
        }
    } // O(deg_node * (log n + deg_node))

    for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
        if (hypergraph.incrementHitCount(edgeIndex)) { // O(deg_edge * log n)
            for (Node node : hypergraph.getEdge(edgeIndex)) {
                updateImpact(node, getImpact(node) - 1); // decrement potential impact; O(log n)
            } // O(deg_edge * log n)
        }
    } // O(deg_node * deg_edge * log n)

	return true;
}

void AdaptiveGreedyState::setSolution(Solution& solution) {
    for (Node node : solution.getSolution()) {
        addToSolution(node);
	}
}

bool AdaptiveGreedyState::removeFromSolution(Node node) {
    if (!solution.contains(node)) return false;
    solution.erase(node);
    for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
        nodesHittingEdge[edgeIndex].erase(std::find(nodesHittingEdge[edgeIndex].begin(), nodesHittingEdge[edgeIndex].end(), node)); // O(deg_edge)
        if (nodesHittingEdge[edgeIndex].size() == 1) {
            hypergraph.decrementHitCount(edgeIndex);
            Node otherNode = nodesHittingEdge[edgeIndex][0];
            addToEdgesOnlyHitByNode(otherNode, edgeIndex); // O(log n)
        }
        else if (nodesHittingEdge[edgeIndex].size() == 0) {
            if (hypergraph.decrementHitCount(edgeIndex)) { // O(1)
                for (Node node : hypergraph.getEdge(edgeIndex)) {
                    updateImpact(node, getImpact(node) + 1); // increment potential impact; O(log n)
                } // O(deg_edge * log n)
            }
        }
        else {
            hypergraph.decrementHitCount(edgeIndex);
        }
    } // O(deg_node) * O(deg_edge) * O(log n)

    clearEdgesHitByNode(node); // O(deg_node)
    return true;
}

bool AdaptiveGreedyState::shrinkSolutionIfApplicable(uint32_t highestImpact) {
    Node leastImpactfulSolutionNode = solutionNodeSingleResponsibilities.top().key;
    int32_t lowestImpact = solutionNodeSingleResponsibilities.top().priority - hypergraph.getM();

    if (-lowestImpact < highestImpact) {
        removeFromSolution(leastImpactfulSolutionNode); // O(n_sol + deg_node * deg_edge * log n)
        return true;
    }
    return false;
}

void AdaptiveGreedyState::addToEdgesOnlyHitByNode(Node node, EdgeIndex edgeIndex) {
    solutionNodeSingleResponsibilities.set(node, hypergraph.getM() - 1, solutionNodeSingleResponsibilities.get_priority(node).second - 1); // minus because we want a min queue, so this node gets MORE impact and + m because the pq can only handle uint types, making m the "new 0"; O(log n)
}

void AdaptiveGreedyState::clearEdgesHitByNode(Node node) {
    solutionNodeSingleResponsibilities.remove(node);
}

void AdaptiveGreedyState::deleteNodes(const std::vector<Node>& nodesToRemove) {
    for (const Node node : nodesToRemove) {
        potentialNodeImpact.remove(node);
        for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
            if (hypergraph.getEdge(edgeIndex).size() == 2) {
                for (Node otherNode : hypergraph.getEdge(edgeIndex)) {
                    if (otherNode != node) {
                        addToEdgesOnlyHitByNode(otherNode, edgeIndex);
					}
                }
            }
        }
    }

    hypergraph.deleteNodes(nodesToRemove);
}

void AdaptiveGreedyState::deleteEdges(std::vector<EdgeIndex>& edgeIndizesToRemove) {
    for (EdgeIndex edgeIndexToRemove : edgeIndizesToRemove) {
        for (Node node : hypergraph.getEdge(edgeIndexToRemove)) {
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
