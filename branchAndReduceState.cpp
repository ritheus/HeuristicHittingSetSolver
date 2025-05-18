#include "branchAndReduceState.hpp"
#include "kernelization.hpp"
#include "greedyState.hpp"
#include "logger.hpp"
#include <algorithm>

BranchAndReduceState::BranchAndReduceState(
	Hypergraph& hypergraph, 
	const cxxopts::ParseResult& optionsResult) : globalUpperBound(std::numeric_limits<uint32_t>::max()), AlgorithmState(hypergraph, optionsResult) {}

BranchAndReduceState::BranchAndReduceState(
	NumNodes n, 
	NumEdges m, 
	std::vector<Edge>&& setSystem, 
	const cxxopts::ParseResult& optionsResult) : globalUpperBound(std::numeric_limits<uint32_t>::max()), AlgorithmState(n, m, std::move(setSystem), optionsResult) {}

Solution BranchAndReduceState::calculateSolution(bool applyKernelization) {
	GreedyState state = GreedyState(hypergraph, optionsResult);
	//solution = state.calculateSolution(applyKernelization);

	branchAndReduce(state, std::unordered_set<Node>(), std::unordered_set<Node>());

	return solution;
}

std::optional<Solution> BranchAndReduceState::branchAndReduce(GreedyState state, std::unordered_set<Node> include, std::unordered_set<Node> exclude) { // might be merged with calculateSolution()
	uint32_t lowerBound = calculateSolutionSizeLowerBound(state);
	Solution recursiveSolution = state.calculateSolution(false);
	uint32_t upperBound = recursiveSolution.size();
	if (upperBound == 0) { // unsolvable
		return std::nullopt;
	}
	if (upperBound < globalUpperBound) {
		globalUpperBound = upperBound;
	}
	else {
		upperBound = globalUpperBound;
	}

	LOGBNR("LB: " << lowerBound << ", UB: " << upperBound);

	if (lowerBound >= upperBound) {
		if (state.isSolved()) {
			return recursiveSolution;
		}
		else { // unsolvable
			return std::nullopt;
		}
	}

	reduce(state);

	GreedyState newState = GreedyState(state.hypergraph, optionsResult);
	for (Node node : include) {
		newState.addToSolution(node);
	}
	for (Node node : exclude) {
		newState.banFromSolution(node);
	}
	GreedyState newState2 = newState;
	Node nextNode = chooseNextNode(newState);
	newState.addToSolution(nextNode);
	newState2.banFromSolution(nextNode);
	std::unordered_set<Node> newInclude = include;
	std::unordered_set<Node> newExclude = exclude;
	newInclude.insert(nextNode);
	newExclude.insert(nextNode);

	auto includeResult = branchAndReduce(newState, newInclude, exclude);
	auto excludeResult = branchAndReduce(newState2, include, newExclude);

	if (includeResult && excludeResult) {
		solution = (includeResult->size() < excludeResult->size()) ? *includeResult : *excludeResult;
		return solution;
	}
	if (includeResult) {
		solution = *includeResult;
		return solution;
	}
	if (excludeResult) {
		solution = *excludeResult;
		return solution;
	}

	return std::nullopt;
}

uint32_t BranchAndReduceState::calculateSolutionSizeLowerBound(GreedyState& state) {
	//int maxDegreeBound = calculateMaxDegreeBound(state);
	int efficiencyBound = calculateEfficiencyBound(state);
	return std::max(0,efficiencyBound);
}

int BranchAndReduceState::calculateMaxDegreeBound(GreedyState& state) {
	uint32_t maxDegree = state.hypergraph.getMaximumVertexDegree();
	uint32_t m = state.hypergraph.getNumUnhitEdges();
	uint32_t includeSize = state.getSolution().size();

	return std::ceil(static_cast<double>(m) / maxDegree) + includeSize;
}

int BranchAndReduceState::calculateEfficiencyBound(GreedyState& state) {
	std::vector<double> edgeEfficiency;

	edgeEfficiency.resize(state.hypergraph.getEdges().size());
	double efficiency;
	for (EdgeIndex edgeIndex = 0; edgeIndex < state.hypergraph.getEdges().size(); edgeIndex++) {
		if (!state.hypergraph.isEdgeHit(edgeIndex)) {
			efficiency = std::numeric_limits<double>::max();
			for (Node node : state.hypergraph.getEdge(edgeIndex)) {
				double eff = 1.0 / state.hypergraph.getIncidentEdgeIndizes(node).size();
				if (eff < efficiency) {
					efficiency = eff;
				}
			}
			edgeEfficiency[edgeIndex] = efficiency;
		}
		
	}
	return std::ceil(std::accumulate(edgeEfficiency.begin(), edgeEfficiency.end(), 0.0)) + state.getSolution().size();
}

void BranchAndReduceState::reduce(GreedyState& state) {
	bool changed = true;
	while (changed) {
		changed = kernelization::applyKernelization(state, optionsResult);
	}
}

Node BranchAndReduceState::chooseNextNode(GreedyState& state) {
	return state.getHighestImpactNode().second;
}





bool BranchAndReduceState::addToSolution(Node) {
	return false;
}

void BranchAndReduceState::deleteNodes(const std::vector<Node>&) {

}

void BranchAndReduceState::deleteEdges(std::vector<EdgeIndex>&) {

}