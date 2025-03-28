#pragma once

#include "aliases.hpp"

struct Solution {
	std::unordered_set<Node> solutionSet;

	Solution() {}

	Solution(std::unordered_set<Node> solutionSet) {
		this->solutionSet = solutionSet;
	}

	void insert(Node node) {
		solutionSet.insert(node);
	}

	void erase(Node node) {
		solutionSet.erase(node);
	}

	void combine(Solution& solution2) {
		for (Node node : solution2.solutionSet) {
			insert(node);
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
};