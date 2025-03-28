#pragma once

#include "aliases.hpp"
#include "updatable_priority_queue.hpp"
#include "hypergraph.hpp"
#include "solution.hpp"
#include "algorithmState.hpp"
#include "kernelization.hpp"
#include "cxxopts.hpp"
#include <vector>
#include <unordered_set>
#include <atomic>

struct AdaptiveGreedyState : AlgorithmState {
public:
    std::vector<std::vector<EdgeIndex>> edgesHitByNode; // alternative to checking for inclusion in solutionSet in addition to hypergraph.getIncidentEdgeIndizes(node)
    std::vector<std::vector<EdgeIndex>> edgesOnlyHitByNode; // maybe unnecessary?
    std::vector<std::vector<Node>> nodesHittingEdge;
    pq::updatable_priority_queue<Node, uint32_t> solutionNodeSingleResponsibilities;
    pq::updatable_priority_queue<Node, uint32_t> potentialNodeImpact;

    AdaptiveGreedyState(NumNodes, NumEdges, std::vector<Edge>&&, const cxxopts::ParseResult&); // O(n * log n + m * deg_edge)

    Solution calculateSolution(bool=true);
    void addToSolution(Node) override; // O(deg_node * deg_edge * log n)
    void removeFromSolution(Node);
    bool shrinkSolutionIfApplicable(uint32_t);
    void addToEdgesOnlyHitByNode(Node, EdgeIndex);
    void clearEdgesHitByNode(Node);
    void deleteNodes(const std::vector<Node>&) override;
    void deleteEdges(std::vector<EdgeIndex>&) override;

    std::pair<uint32_t, Node> getHighestImpactNode();
    uint32_t getImpact(Node);
    bool updateImpact(const Node&, const uint32_t, bool=false); // O(log n)
    bool updateResponsibility(Node, int32_t, bool=false); // O(log n)
    uint32_t getResponsibility(Node);
};