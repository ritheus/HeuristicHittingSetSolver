#include "randomTabuLocalSearch.hpp"
#include "greedyState.hpp"
#include <random>
#include <unordered_map>

RandomTabuLocalSearch::RandomTabuLocalSearch(Hypergraph& hypergraph, Solution& solution, uint32_t tabuLength) : tabuLength(tabuLength) {

}

bool RandomTabuLocalSearch::nodeHasSingleResponsibility(Hypergraph& hypergraph, Solution& solution, Node node, EdgeIndex edgeIndex) {
	for (Node adjacentNode : hypergraph.getEdge(edgeIndex)) {
		if (solution.contains(adjacentNode) && adjacentNode != node) {
			return false;
		}
	}
	return true;
}

std::vector<Node> RandomTabuLocalSearch::removeNodes(uint32_t numNodesToRemove = 2) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, algorithmState->solution.solutionVector.size() - 1);
	uint32_t sampleIndex = 0;
	Node nodeToRemove;
	std::vector<Node> removedNodes;
	for (uint32_t i = 0; i < numNodesToRemove; i++) {
		do {
			sampleIndex = dist(gen);
			std::uniform_int_distribution<> dist(0, algorithmState->solution.solutionVector.size() - 1);
		} while (sampleIndex >= algorithmState->solution.solutionVector.size() && algorithmState->solution.solutionVector.size() > 0);
		nodeToRemove = algorithmState->solution.solutionVector[sampleIndex];
		algorithmState->removeFromSolution(nodeToRemove);
		removedNodes.push_back(nodeToRemove);
	}
	
	return removedNodes;
}

std::vector<Node> RandomTabuLocalSearch::repairPartialSolution() {
	std::vector<Node> addedNodes = algorithmState->repairSolution();

	for (Node node : addedNodes) {
		if (canBeBanned(node)) {
			addToTabuList(node, tabuLength);
		}
	}

	return addedNodes;
}

void RandomTabuLocalSearch::initializeAlgorithmState(std::unique_ptr<AlgorithmState> state) {
	algorithmState = std::move(state);
}

bool RandomTabuLocalSearch::canBeBanned(Node bannedNode) {
	for (EdgeIndex edgeIndex : algorithmState->hypergraph.getIncidentEdgeIndizes(bannedNode)) {
		bool edgeSolvable = false;
		for (Node node : algorithmState->hypergraph.getEdge(edgeIndex)) {
			if (!isTabu(node) && node != bannedNode) {
				edgeSolvable = true;
				break;
			}
		}
		if (!edgeSolvable) {
			return false;
		}
	}
	return true;
}

void RandomTabuLocalSearch::addToTabuList(Node node, uint32_t tabuLength) {
	tabuList.push_back(node);
	algorithmState->bannedNodes.insert(node);
	if (tabuList.size() > tabuLength) {
		algorithmState->bannedNodes.erase(tabuList[0]);
		algorithmState->unbannedNodes.push_back(tabuList[0]);
		tabuList.erase(tabuList.begin());
	}
}

bool RandomTabuLocalSearch::isTabu(Node node) {
	for (Node bannedNode : algorithmState->bannedNodes) {
		if (node == bannedNode) {
			return true;
		}
	}
	return false;
}
