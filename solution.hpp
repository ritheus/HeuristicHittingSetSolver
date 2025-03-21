#pragma once

#include "aliases.hpp"

struct Solution {
	std::unordered_set<Node> solutionSet;

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

	std::unordered_set<Node>& getSolution() {
		return solutionSet;
	}

	bool empty() {
		return solutionSet.empty();
	}
};