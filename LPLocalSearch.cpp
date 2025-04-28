#include "lplocalsearch.hpp"
#include "solution.hpp"
#include <random>
#include <optional>

void LPLocalSearch::removeNodes(uint32_t numNodesToRemove = 2) {
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
		uint32_t nodeToRemove = nodes[samplingDistribution(gen)];
		if (std::find(nodesToRemove.begin(), nodesToRemove.end(), nodeToRemove) == nodesToRemove.end()) {
			nodesToRemove.push_back(nodeToRemove);
		}
	}

	// Remove nodes
	for (Node node : nodesToRemove) {
		algorithmState->removeFromSolution(node);
	}
}

void LPLocalSearch::repairPartialSolution() {
	algorithmState->calculateSolution();
}

void LPLocalSearch::initializeAlgorithmState(std::unique_ptr<AlgorithmState> state) {
	algorithmState = std::move(state);
}