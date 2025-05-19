#pragma once

#include "aliases.hpp"
#include <string>
#include <vector>
#include <unordered_set>

std::string toString(const std::vector<std::vector<Node>>&);
std::string toString(const std::vector<Node>&);
std::string toString(const FastSet&);
std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> parseInputStream(std::istream& input);
std::tuple<NumNodes, NumEdges> getSetSystemParameters(const std::string&);
std::vector<Node> getAllNodesFromEdge(const std::string&);
void writeToStdOut(const FastSet&);