#pragma once

#include "peripheral/aliases.hpp"
#include <unordered_set>
#include <unordered_map>
#include <vector>

struct Solution {
	FastSet solutionSet;
	FastMap nodeToIndex;
	std::vector<Node> solutionVector;

	Solution() {}

	Solution(uint32_t n) {
		solutionVector.reserve(n);
		nodeToIndex.reserve(n);
		solutionSet.reserve(n);
	}

	void insert(const Node& node) {
		auto [_, inserted] = solutionSet.insert(node);
		if (!inserted) return;

		solutionVector.push_back(node);
		nodeToIndex[node] = solutionVector.size() - 1;
	}

	void erase(Node node) {
		auto it = nodeToIndex.find(node);
		if (it == nodeToIndex.end()) return;

		std::size_t index = it->second;
		std::size_t lastIndex = solutionVector.size() - 1;

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

	FastSet& getSolution() {
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

	bool operator!=(const Solution& other) const {
		return solutionSet != other.solutionSet;
	}

	FastSet diff(const Solution& other) const {
		FastSet diffSet;

		for (const Node& node : solutionSet) {
			if (other.solutionSet.find(node) == other.solutionSet.end()) {
				diffSet.insert(node);
			}
		}
		for (const Node& node : other.solutionSet) {
			if (solutionSet.find(node) == solutionSet.end()) {
				diffSet.insert(node);
			}
		}

		return diffSet;
	}
};