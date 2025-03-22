#include "kernelization.hpp"
#include "cxxopts.hpp"
#include "logger.hpp"
#include <iostream>

namespace kernelization {
	void applyKernelization(AlgorithmState& state, cxxopts::ParseResult& optionsResult) {
		LOG("Applying kernelization rules...");
		const bool all = optionsResult.count("kernelization_allRules") > 0;

		if (all || optionsResult.count("kernelization_vertexDominationRule")) {
			applyVertexDominationRule(state);
			LOG("VertexDominationRule applied");
		}
		if (all || optionsResult.count("kernelization_edgeDominationRule")) {
			applyEdgeDominationRule(state);
			LOG("EdgeDominationRule applied");
		}
		if (all || optionsResult.count("kernelization_criticalCoreRule")) {
			applyCriticalCoreRule(state);
			LOG("CriticalCoreRule applied");
		}
		if (all || optionsResult.count("kernelization_unitEdgeRule")) {
			applyUnitEdgeRule(state);
			LOG("UnitEdgeRule applied");
		}
	}

	void applyUnitEdgeRule(AlgorithmState& state) {
		for (Edge edge : state.hypergraph.getEdges()) {
			if (edge.size() == 1) {
				state.addToSolution(edge[0]);
				LOG("UnitEdgeRule - Node " << edge[0] << " added to solution.");
			}
		}
	}

	void applyEdgeDominationRule(AlgorithmState& state) {
		std::vector<EdgeIndex> dominatedEdgeIndizes = findDominatedEdges(state);
		state.deleteEdges(dominatedEdgeIndizes);
	}

	std::vector<EdgeIndex> findDominatedEdges(AlgorithmState& state) {
		std::vector<EdgeIndex> dominatedEdgeIndizes;

		for (EdgeIndex dominatedEdgeIndex = 0; dominatedEdgeIndex < state.hypergraph.getEdges().size(); dominatedEdgeIndex++) { // vielleicht lieber von hinten nach vorn iterieren, dann muss in deleteEdges() nicht mehr sortiert werden
			for (EdgeIndex dominatingEdgeIndex = 0; dominatingEdgeIndex < state.hypergraph.getEdges().size(); dominatingEdgeIndex++) {
				if (dominatedEdgeIndex != dominatingEdgeIndex) {
					if (edgeDominatesEdge(state.hypergraph.getEdge(dominatingEdgeIndex), state.hypergraph.getEdge(dominatedEdgeIndex), dominatingEdgeIndex, dominatingEdgeIndex)) {
						dominatedEdgeIndizes.push_back(dominatedEdgeIndex);
						LOG("EdgeDominationRule - Edge " << dominatedEdgeIndex << " is dominated by Edge " << dominatingEdgeIndex << " and gets removed from the instance.");
						break;
					}
				}
			}
		}

		return dominatedEdgeIndizes;
	}

	bool edgeDominatesEdge(const Edge& dominating, const Edge& dominated, EdgeIndex dominatingEdgeIndex, EdgeIndex dominatedEdgeIndex) {
		for (Node dominatedEdgeNode : dominated) {
			if (std::find(dominating.begin(), dominating.end(), dominatedEdgeNode) == dominating.end()) {
				return false;
			}
		}
		if (dominated.size() == dominating.size()) {
			return dominatedEdgeIndex > dominatingEdgeIndex; // tie break, higher index gets dominated
		}
		return true;
	}

	void applyVertexDominationRule(AlgorithmState& state) {
		std::unordered_set<Node> dominatedNodes = findDominatedNodes(state);
		state.deleteNodes(std::vector<Node>(dominatedNodes.begin(), dominatedNodes.end()));
	}

	std::unordered_set<Node> findDominatedNodes(AlgorithmState& state) {
		std::unordered_set<Node> dominatedNodes;
		for (Node dominated : state.hypergraph.getNodes()) {
			uint32_t dominatedSize = state.hypergraph.getIncidentEdgeIndizes(dominated).size();
			for (Node dominating : state.hypergraph.getAdjacentNodes(dominated)) {
				if (dominated != dominating && dominatedSize <= state.hypergraph.getIncidentEdgeIndizes(dominating).size()) {
					if (nodeDominatesNode(dominating, dominated, state)) {
						dominatedNodes.insert(dominated);
						LOG("VertexDominationRule - Node " << dominated << " is dominated by node " << dominating << " and gets removed from the instance.");
						break;
					}
				}
			}
		}
		return dominatedNodes;
	}

	bool nodeDominatesNode(Node dominating, Node dominated, AlgorithmState& state) {
		if (dominated == dominating) {
			return false;
		}

		const auto& domEdges = state.hypergraph.getIncidentEdgeIndizes(dominating);
		const auto& subEdges = state.hypergraph.getIncidentEdgeIndizes(dominated);

		std::unordered_set<EdgeIndex> domSet(domEdges.begin(), domEdges.end());

		for (EdgeIndex edge : subEdges) {
			if (domSet.find(edge) == domSet.end()) return false;
		}
		if (domEdges.size() == subEdges.size()) {
			return dominated > dominating; // tie break, higher index gets dominated
		}
		return true;
	}

	void applyHighDegreeRule(AlgorithmState& state) {

	}

	void applyHighOccurrenceRule(AlgorithmState& state) {

	}

	void applyCrownDecompositionRule(AlgorithmState& state) {

	}

	/*
	A critical core is a subedge c so that the link of each subedge that properly includes c is "small" while the link of c itself is "big".
	The link of a subedge is the number of hyperedges in the hypergraph that contain it.
	*/
	void applyCriticalCoreRule(AlgorithmState& state) {
		for (uint32_t coreDegree = 2; coreDegree < state.hypergraph.getMaximumEdgeDegree(); coreDegree++) {
			for (const Edge& core : state.hypergraph.getSubedgesOfDegree(coreDegree)) {
				std::vector<EdgeIndex> coreLink = isCriticalCore(core, state);
				if (!coreLink.empty()) {
					state.deleteEdges(coreLink);
					state.addEdge(core);
#ifdef LOGGING
					for (uint32_t coreLinkIndex : coreLink) {
						LOG("CriticalCoreRule - Edge " << coreLinkIndex << " removed from instance.");
					}
					// "CriticalCoreRule - Edge " << core << " added to instance."
					std::cerr << "CriticalCoreRule - Edge [";
					for (size_t i = 0; i < core.size(); ++i) {
						std::cerr << core[i];
						if (i != core.size() - 1)
							std::cerr << ", ";
					}
					std::cerr << "] added to instance." << std::endl;
#endif // LOGGING
				}
			}
		}
	}

	/*
	Checks whether size of the link of the given core is greater than upperCriticalThreshold while the size of the link of each of its superedges is less or equal lowerCriticalThreshold.
	@return std::vector<EdgeIndex> coreLink - the link of the given core if it is critical, otherwise an empty vector
	*/
	std::vector<EdgeIndex> isCriticalCore(const Edge& core, const AlgorithmState& state) {
		uint32_t coreDegree = core.size();
		std::vector<EdgeIndex> coreLink = calculateLink(core, state);
		uint32_t coreCriticalThreshold = std::pow(state.upperSolutionSizeBound, state.hypergraph.getMaximumEdgeDegree() - coreDegree);

		if (coreLink.size() < coreCriticalThreshold) {
			return {};
		}

		for (uint32_t subedgeDegree = coreDegree + 1; subedgeDegree < state.hypergraph.getMaximumEdgeDegree(); subedgeDegree++) {
			uint32_t otherCriticalThreshold = std::pow(state.upperSolutionSizeBound, state.hypergraph.getMaximumEdgeDegree() - subedgeDegree);
			// iterate through all subedges of degree i and if it contains core, calculate the number of occurences
			for (const Edge& subedge : state.hypergraph.getSubedgesOfDegree(subedgeDegree)) {
				if (subedgeOccursInSuperedge(core, subedge, state)) {
					std::vector<EdgeIndex> subedgeLink = calculateLink(subedge, state);
					if (subedgeLink.size() <= otherCriticalThreshold) {
						return {};
					}
				}
			}
		}

		return coreLink;
	}

	/*
	Calculates the link of the given core by gathering all hyperedges that contain it.
	*/
	std::vector<EdgeIndex> calculateLink(const Edge& core, const AlgorithmState& state) {
		std::vector<EdgeIndex> link;
		for (EdgeIndex edgeIndex = 0; edgeIndex < state.hypergraph.getEdges().size(); edgeIndex++) {
			const Edge& hyperedge = state.hypergraph.getEdge(edgeIndex);
			if (subedgeOccursInSuperedge(core, hyperedge, state)) {
				link.push_back(edgeIndex);
			}
		}
		return link;
	}

	/*
	Checks whether the given subedge occurs in the given superedge.
	*/
	bool subedgeOccursInSuperedge(const Edge& subedge, const Edge& superedge, const AlgorithmState& state) {
		for (Node coreNode : subedge) {
			if (std::find(superedge.begin(), superedge.end(), coreNode) == superedge.end()) {
				return false;
			}
		}
		return true;
	}
}
