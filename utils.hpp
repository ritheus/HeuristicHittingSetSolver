#pragma once

#include "aliases.hpp"
#include <string>
#include <vector>
#include <unordered_set>

std::string toString(const std::vector<std::vector<Node>>&);
std::string toString(const std::vector<Node>&);
std::string toString(const std::unordered_set<Node>&);
std::tuple<NumNodes, NumEdges> getSetSystemParameters(const std::string&);
std::vector<Node> getAllNodesFromEdge(const std::string&, std::istringstream&);
std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> parseInputFile(const std::string&);
void writeToFile(const std::string&, const std::vector<Node>&);
void writeToStdOut(const std::vector<Node>&);
std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> parseStdIn();