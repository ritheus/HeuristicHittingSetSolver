#pragma once

#include "peripheral/aliases.hpp"
#include "updatable_priority_queue.hpp"
#include "hypergraph.hpp"
#include "solution.hpp"
#include "algorithms/algorithmState.hpp"
#include "kernelization.hpp"
#include "peripheral/cxxopts.hpp"
#include <vector>
#include <unordered_set>
#include <atomic>

struct AdaptiveGreedyState : AlgorithmState {
public:
    std::vector<std::vector<Node>> nodesHittingEdge;
    pq::updatable_priority_queue<Node, uint32_t> solutionNodeSingleResponsibilities;
    pq::updatable_priority_queue<Node, uint32_t> potentialNodeImpact;

    AdaptiveGreedyState(NumNodes, NumEdges, std::vector<Edge>&&, const cxxopts::ParseResult&); // O(n * log n + m * deg_edge)
    AdaptiveGreedyState(Hypergraph&, const cxxopts::ParseResult&); // O(n * log n + m * deg_edge)

    Solution calculateSolution(bool=true);
    bool addToSolution(Node) override; // O(deg_node * deg_edge * log n)
    virtual void setSolution(Solution&) override;
    bool removeFromSolution(Node);
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