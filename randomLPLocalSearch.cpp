#include "randomlplocalsearch.hpp"
#include "solution.hpp"
#include <random>
#include <optional>

void RandomLPLocalSearch::removeNodes(uint32_t numNodesToRemove = 2) {
	// Build sampling distribution
	std::vector<double> inverseLPValues;
	std::vector<Node> nodes;
	inverseLPValues.reserve(fractionalLPSolution.size());
	nodes.reserve(fractionalLPSolution.size());
	for (const auto& [node, value] : fractionalLPSolution) {
		if (algorithmState->getSolution().contains(node)) {
			double inverseValue = 1 - fractionalLPSolution[node];
			if (inverseValue > 0.0) {
				inverseLPValues.push_back(inverseValue);
				nodes.push_back(node);
			}
		}
	}

	// Sample nodes to remove
	std::random_device rd;
	std::mt19937 gen(rd());
	std::discrete_distribution samplingDistribution = std::discrete_distribution(inverseLPValues.begin(), inverseLPValues.end());
	std::vector<Node> nodesToRemove;
	for (uint32_t i = 0; i < numNodesToRemove; i++) {
		if (nodes.size() > nodesToRemove.size()) {
			uint32_t nodeToRemove = nodes[samplingDistribution(gen)];
			if (std::find(nodesToRemove.begin(), nodesToRemove.end(), nodeToRemove) == nodesToRemove.end()) {
				nodesToRemove.push_back(nodeToRemove);
			}
		}
	}

	// Remove nodes
	for (Node node : nodesToRemove) {
		removeFromSolution(node);
	}
}

void RandomLPLocalSearch::repairPartialSolution() {
	std::unordered_set<EdgeIndex> hitEdges;
	for (EdgeIndex unhitEdgeIndex : unhitEdges) {
		if (hitEdges.find(unhitEdgeIndex) != hitEdges.end()) {
			continue;
		}
		Edge edge = algorithmState->hypergraph.getEdge(unhitEdgeIndex);
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, edge.size() - 1);
		Node node = edge[dist(gen)];
		addToSolution(node);
		for (EdgeIndex incidentEdgeIndex : algorithmState->hypergraph.getIncidentEdgeIndizes(node)) {
			hitEdges.insert(incidentEdgeIndex);
		}
		
		if (algorithmState->hypergraph.isSolved()) {
			break;
		}
	}
	if (!algorithmState->hypergraph.isSolved()) {
		throw std::runtime_error("Could not repair partial solution");
	}

	unhitEdges.clear();
}

void RandomLPLocalSearch::initializeAlgorithmState(std::unique_ptr<AlgorithmState> state) {
	algorithmState = std::move(state);
}

void RandomLPLocalSearch::addToSolution(Node node) {
	for (EdgeIndex edgeIndex : algorithmState->hypergraph.getIncidentEdgeIndizes(node)) {
		algorithmState->hypergraph.incrementHitCount(edgeIndex);
	}
	algorithmState->solution.insert(node);
}

void RandomLPLocalSearch::removeFromSolution(Node node) {
	for (EdgeIndex edgeIndex : algorithmState->hypergraph.getIncidentEdgeIndizes(node)) {
		if (algorithmState->hypergraph.decrementHitCount(edgeIndex)) {
			unhitEdges.insert(edgeIndex);
		}
	}
	algorithmState->solution.erase(node);
}