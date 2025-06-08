#include "lplocalsearch.hpp"
#include "solution.hpp"
#include <random>
#include <optional>

std::vector<Node> LPLocalSearch::removeNodes(uint32_t numNodesToRemove = 2) {
	// Build sampling distribution
	std::vector<double> inverseLPValues;
	std::vector<Node> nodes;
	inverseLPValues.reserve(fractionalLPSolution.size());
	nodes.reserve(fractionalLPSolution.size());
	for (const auto& [node, value] : fractionalLPSolution) {
		if (getSolution().contains(node)) {
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
			if (std::find(nodesToRemove.begin(), nodesToRemove.end(), nodeToRemove) == nodesToRemove.end() && getSolution().contains(nodeToRemove)) {
				nodesToRemove.push_back(nodeToRemove);
			}
		}
	}

	// Remove nodes
	for (Node node : nodesToRemove) {
		algorithmState->removeFromSolution(node);
		greedyState.removeFromSolution(node);
	}

	return nodesToRemove;
}

std::vector<Node> LPLocalSearch::repairPartialSolution() {
	std::vector<Node> addedNodes = greedyState.repairSolution();
	for (Node node : addedNodes) {
		algorithmState->addToSolution(node);
	}
	return addedNodes;
}

void LPLocalSearch::revertSolution(FastSet& removedNodes, FastSet& addedNodes) {
	for (Node node : removedNodes) {
		algorithmState->addToSolution(node);
		greedyState.addToSolution(node);
	}
	for (Node node : addedNodes) {
		algorithmState->removeFromSolution(node);
		greedyState.removeFromSolution(node);
	}
	removedNodes.clear();
	addedNodes.clear();
}

void LPLocalSearch::initializeAlgorithmState(std::unique_ptr<AlgorithmState> state) {
	algorithmState = std::move(state);
	greedyState.setSolution(algorithmState->getSolution());
}

void LPLocalSearch::setSolution(Solution& newSolution) {
	greedyState.setSolution(newSolution);
}

Solution& LPLocalSearch::getSolution() {
	return greedyState.getSolution();
}

bool LPLocalSearch::isSolved() {
	return greedyState.isSolved();
}

bool LPLocalSearch::isSolvedBy(Solution& solution) {
	return greedyState.hypergraph.isSolvedBy(solution);
}