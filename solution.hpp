#pragma once

#include "aliases.hpp"
#include <unordered_set>

struct Solution {
	std::unordered_set<Node> solutionSet;
	Node mostRecentlyAddedNode;

	Solution() {}

	Solution(std::unordered_set<Node> solutionSet) {
		this->solutionSet = solutionSet;
	}

	void insert(Node node) {
		solutionSet.insert(node);
		mostRecentlyAddedNode = node;
	}

	void erase(Node node) {
		solutionSet.erase(node);
	}

	void combine(Solution& solution2) {
		for (Node node : solution2.solutionSet) {
			insert(node);
			mostRecentlyAddedNode = node;
		}
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