#include "adaptiveGreedyTabuLocalSearch.hpp"
#include "adaptiveGreedyState.hpp"
#include <random>
#include <unordered_map>

AdaptiveGreedyTabuLocalSearch::AdaptiveGreedyTabuLocalSearch(Hypergraph& hypergraph, Solution& solution, uint32_t tabuLength) : tabuLength(tabuLength) {
	for (Node node : solution.getSolution()) {
		for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(node)) {
			solutionNodeSingleResponsibilities.push(node, hypergraph.getM());
			if (nodeHasSingleResponsibility(hypergraph, solution, node, edgeIndex)) {
				solutionNodeSingleResponsibilities.update(node, solutionNodeSingleResponsibilities.get_priority(node).second - 1);
			}
		}
	}
}

bool AdaptiveGreedyTabuLocalSearch::nodeHasSingleResponsibility(Hypergraph& hypergraph, Solution& solution, Node node, EdgeIndex edgeIndex) {
	for (Node adjacentNode : hypergraph.getEdge(edgeIndex)) {
		if (solution.contains(adjacentNode) && adjacentNode != node) {
			return false;
		}
	}
	return true;
}

Solution AdaptiveGreedyTabuLocalSearch::removeNodes(Hypergraph& hypergraph, Solution& newPartialSolution, uint32_t numNodesToRemove = 2) {
	// Remove nodes
	std::unordered_map<Node, uint32_t> toBeReinserted; // vector is better
	Node nodeToRemove;
	uint32_t i = 0;
	while (i < numNodesToRemove) {
		nodeToRemove = solutionNodeSingleResponsibilities.top().key;
		if (canBeBanned(nodeToRemove, hypergraph)) {
			addToTabuList(nodeToRemove, tabuLength);
			newPartialSolution.erase(nodeToRemove);
			solutionNodeSingleResponsibilities.pop();
			i++;
		}
		else {
			solutionNodeSingleResponsibilities.pop();
			toBeReinserted.insert({ nodeToRemove, solutionNodeSingleResponsibilities.get_priority(nodeToRemove).second });
		}
	}

	// Fix solutionNodeSingleResponsibilities
	for (const auto& [node, value] : toBeReinserted) {
		solutionNodeSingleResponsibilities.push(node, value);
	}

	return newPartialSolution;
}

Solution AdaptiveGreedyTabuLocalSearch::repairPartialSolution(Hypergraph& hypergraph, Solution& partialSolution) {
	if (!hypergraph.isSolvedBy(partialSolution)) {
		AdaptiveGreedyState state = AdaptiveGreedyState(hypergraph, {});
		state.setNodeAges(nodeAges);
		state.setHighestAge(highestAge);
		state.setSolution(partialSolution);
		partialSolution = state.calculateSolution();
		solutionNodeSingleResponsibilities = state.solutionNodeSingleResponsibilities;
		nodeAges = state.nodeAges;
		highestAge = state.highestAge;
	}
	return partialSolution;
}

bool AdaptiveGreedyTabuLocalSearch::canBeBanned(Node bannedNode, Hypergraph& hypergraph) {
	for (EdgeIndex edgeIndex : hypergraph.getIncidentEdgeIndizes(bannedNode)) {
		bool edgeSolvable = false;
		for (Node node : hypergraph.getEdge(edgeIndex)) {
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

void AdaptiveGreedyTabuLocalSearch::addToTabuList(Node node, uint32_t tabuLength) {
	tabuList.push_back(node);
	if (tabuList.size() > tabuLength) {
		tabuList.erase(tabuList.begin());
	}
}

bool AdaptiveGreedyTabuLocalSearch::isTabu(Node node) {
	for (Node bannedNode : tabuList) {
		if (node == bannedNode) {
			return true;
		}
	}
	return false;
}
