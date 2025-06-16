#pragma once

#include "peripheral/aliases.hpp"
#include "hypergraph.hpp"
#include <unordered_map>
#include <map>

std::unordered_map<Node, double> buildAndSolveLP(Hypergraph& hypergraph);

std::map<Node, double> getOrderedFractionalSolution(Hypergraph& hypergraph);