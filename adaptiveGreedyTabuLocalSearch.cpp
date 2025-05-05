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

void AdaptiveGreedyTabuLocalSearch::removeNodes(uint32_t numNodesToRemove = 2) {
	// Remove nodes
	std::unordered_map<Node, uint32_t> toBeReinserted; // vector is better
	Node nodeToRemove;
	uint32_t i = 0;
	while (i < numNodesToRemove && algorithmState->solution.solutionVector.size() > 0) {
		nodeToRemove = solutionNodeSingleResponsibilities.top().key;
		if (canBeBanned(nodeToRemove)) {
			addToTabuList(nodeToRemove, tabuLength);
			algorithmState->removeFromSolution(nodeToRemove);
			solutionNodeSingleResponsibilities.pop();
			i++;
		}
		else {
			solutionNodeSingleResponsibilities.pop();
			toBeReinserted.insert({ nodeToRemove, solutionNodeSingleResponsibilities.get_priority(nodeToRemove).second });
		}
		if (solutionNodeSingleResponsibilities.size() == 0) {
			break;
		}
	}

	// Fix solutionNodeSingleResponsibilities
	for (const auto& [node, value] : toBeReinserted) {
		solutionNodeSingleResponsibilities.push(node, value);
	}
}

void AdaptiveGreedyTabuLocalSearch::repairPartialSolution() {
	algorithmState->calculateSolution();
	auto* adaptiveState = dynamic_cast<AdaptiveGreedyState*>(algorithmState.get()); // static_cast is faster
	if (algorithmState == nullptr) {
		throw std::runtime_error("TabuLocalSearch only works with AdaptiveGreedyState");
	}
	solutionNodeSingleResponsibilities = adaptiveState->solutionNodeSingleResponsibilities;
	//if (!hypergraph.isSolvedBy(partialSolution)) {
		//AdaptiveGreedyState state = AdaptiveGreedyState(hypergraph, {});
		//state.setNodeAges(nodeAges);
		//state.setHighestAge(highestAge);
		//state.setSolution(partialSolution);
		//nodeAges = state.nodeAges;
		//highestAge = state.highestAge;
	//}
}

void AdaptiveGreedyTabuLocalSearch::initializeAlgorithmState(std::unique_ptr<AlgorithmState> state) {
	algorithmState = std::move(state);
}

bool AdaptiveGreedyTabuLocalSearch::canBeBanned(Node bannedNode) {
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
