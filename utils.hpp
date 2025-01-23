#pragma once

#include "aliases.hpp"
#include <string>
#include <vector>
#include <unordered_set>

std::string toString(const std::vector<std::vector<Node>>&);
std::string toString(const std::vector<Node>&);
std::string toString(const std::unordered_set<Node>&);
std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> parseInputStream(std::basic_istream<char>& input);
std::tuple<NumNodes, NumEdges> getSetSystemParameters(const std::string&);
std::vector<Node> getAllNodesFromEdge(const std::string&, std::istringstream&);
void writeToStdOut(const std::unordered_set<Node>&);