#pragma once

#include "aliases.hpp"
#include <unordered_set>
#include <unordered_map>

struct Solution {
	std::unordered_set<Node> solutionSet;
	std::unordered_map<Node, uint32_t> nodeToIndex;
	std::vector<Node> solutionVector;
	Node mostRecentlyAddedNode;

	Solution() {}

	void insert(Node node) {
		solutionSet.insert(node);
		solutionVector.push_back(node);
		nodeToIndex.insert({ node, solutionVector.size() - 1 });
		mostRecentlyAddedNode = node;
	}

	void erase(Node node) {
		auto it = nodeToIndex.find(node);
		if (it == nodeToIndex.end()) return;

		uint32_t index = it->second;
		uint32_t lastIndex = solutionVector.size() - 1;

		if (index != lastIndex) {
			Node lastNode = solutionVector[lastIndex];
			std::swap(solutionVector[index], solutionVector[lastIndex]);
			nodeToIndex[lastNode] = index;
		}

		solutionVector.pop_back();
		nodeToIndex.erase(node);
		solutionSet.erase(node);
	}

	bool contains(Node node) {
		return solutionSet.find(node) != solutionSet.end();
	}

	std::unordered_set<Node>& getSolution() {
		return solutionSet;
	}

	size_t size() {
		return solutionSet.size();
	}

	bool empty() {
		return solutionSet.empty();
	}

	bool operator==(const Solution& other) const {
		return solutionSet == other.solutionSet;
	}
};