#include "localSearch/randomlplocalsearch.hpp"
#include "solution.hpp"
#include <random>
#include <optional>

std::vector<Node> RandomLPLocalSearch::removeNodes(uint32_t numNodesToRemove = 2) {
	// Sample nodes to remove
	std::random_device rd;
	std::mt19937 gen(rd());
	std::discrete_distribution samplingDistribution = std::discrete_distribution(inverseFractionalLPValues.begin(), inverseFractionalLPValues.end());
	std::vector<Node> nodesToRemove;
	for (uint32_t i = 0; i < numNodesToRemove; i++) {
		if (inverseFractionalLPNodes.size() > nodesToRemove.size()) {
			uint32_t nodeToRemove = inverseFractionalLPNodes[samplingDistribution(gen)];
			if (std::find(nodesToRemove.begin(), nodesToRemove.end(), nodeToRemove) == nodesToRemove.end()) {
				nodesToRemove.push_back(nodeToRemove);
			}
		}
	}

	// Remove nodes
	for (Node node : nodesToRemove) {
		removeFromSolution(node);
	}

	return nodesToRemove;
}

std::vector<Node> RandomLPLocalSearch::repairPartialSolution() {
	std::unordered_set<EdgeIndex> hitEdges;
	std::vector<Node> addedNodes;
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
		addedNodes.push_back(node);
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
	return addedNodes;
}

void RandomLPLocalSearch::initializeAlgorithmState(std::unique_ptr<AlgorithmState> state) {
	algorithmState = std::move(state);
}

void RandomLPLocalSearch::addToSolution(Node node) {
	for (EdgeIndex edgeIndex : algorithmState->hypergraph.getIncidentEdgeIndizes(node)) {
		algorithmState->hypergraph.incrementHitCount(edgeIndex);
	}
	algorithmState->solution.insert(node);

	double inverseValue = 1 - fractionalLPSolution[node];
	if (inverseValue > 0.0) {
		inverseFractionalLPValues.push_back(inverseValue);
		inverseFractionalLPNodes.push_back(node);
		inverseFractionalLPNodesToIndex[node] = inverseFractionalLPValues.size() - 1;
	}
}

void RandomLPLocalSearch::removeFromSolution(Node node) {
	for (EdgeIndex edgeIndex : algorithmState->hypergraph.getIncidentEdgeIndizes(node)) {
		if (algorithmState->hypergraph.decrementHitCount(edgeIndex)) {
			unhitEdges.insert(edgeIndex);
		}
	}
	algorithmState->solution.erase(node);

	int indexToRemove = inverseFractionalLPNodesToIndex[node];
	int lastIndex = inverseFractionalLPNodes.size() - 1;
	if (indexToRemove != lastIndex) {
		auto lastNode = inverseFractionalLPNodes.back();
		auto lastValue = inverseFractionalLPValues.back();
		inverseFractionalLPNodes[indexToRemove] = lastNode;
		inverseFractionalLPValues[indexToRemove] = lastValue;
		inverseFractionalLPNodesToIndex[lastNode] = indexToRemove;
	}
	inverseFractionalLPNodes.pop_back();
	inverseFractionalLPValues.pop_back();
	inverseFractionalLPNodesToIndex.erase(node);
}

void RandomLPLocalSearch::buildInverseFractionalLPSolution(Solution& solution) {
	inverseFractionalLPValues.reserve(fractionalLPSolution.size());
	inverseFractionalLPNodes.reserve(fractionalLPSolution.size());
	inverseFractionalLPNodesToIndex.reserve(fractionalLPSolution.size());
	for (const auto& [node, value] : fractionalLPSolution) {
		if (solution.contains(node)) {
			double inverseValue = 1 - fractionalLPSolution[node];
			if (inverseValue > 0.0) {
				inverseFractionalLPValues.push_back(inverseValue);
				inverseFractionalLPNodes.push_back(node);
				inverseFractionalLPNodesToIndex[node] = inverseFractionalLPValues.size() - 1;
			}
		}
	}
}