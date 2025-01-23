#include "utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>

std::string toString(const std::vector<std::vector<Node>>& setSystem) {
    std::stringstream ss;
    for (auto&& edge : setSystem) {
        ss << "[ ";
        for (const Node node : edge) {
            ss << node << " ";
        }
        ss << "]" << '\n';
    }
    return ss.str();
}

std::string toString(const std::vector<Node>& vec) {
    std::stringstream ss;
    for (const Node node : vec) {
        ss << node << '\n';
    }
    return ss.str();
}

std::string toString(const std::unordered_set<Node>& set) {
    std::stringstream ss;
    for (const Node node : set) {
        ss << node << '\n';
    }
    return ss.str();
}

std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> parseInputStream(std::basic_istream<char>& input) {
    std::istringstream ss;
    std::vector<std::vector<Node>> setSystem;
    NumNodes n = 0;
    NumEdges m = 0;

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) continue;
        if (line[0] == 'c') {
            continue;
        }
        else if (line[0] == 'p') {
            std::tie(n, m) = getSetSystemParameters(line);
            if (n <= 0 || m <= 0) {
                std::cerr << "setSystemParameters konnten nicht ermittelt werden." << std::endl;
            }
        }
        else {
            setSystem.push_back(getAllNodesFromEdge(line, ss));
        }
    }

    return {n, m, std::move(setSystem)};
}

std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> parseInputFile(const std::string& inputFileName) {
    std::istringstream ss;
    std::vector<std::vector<Node>> setSystem;
    NumNodes n = 0;
    NumEdges m = 0;

    std::ifstream inputFile(inputFileName);
    if (inputFile.is_open()) {
        return parseInputStream(inputFile);
    }
}

std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> parseStdIn() {
    return parseInputStream(std::cin);
}

void writeToStdOut(const std::unordered_set<Node>& solution) {
    std::cout << solution.size() << '\n';
    for (Node node : solution) {
        std::cout << node << '\n';
    }
    std::cout << std::flush;
}

std::tuple<NumNodes, NumEdges> getSetSystemParameters(const std::string& s) {
    NumNodes n;
    NumEdges m;
    std::istringstream ss(s);
    std::string token;
    ss >> token;
    if (token != "p") {
        std::cerr << "Invalid Format!" << std::endl;
    }
    ss >> token;
    if (token != "hs") {
        std::cerr << "Invalid Format!" << std::endl;
    }
    ss >> n;
    ss >> m;
    return std::make_tuple(n, m);
}

/**
 * Extracts all integer values from the given string and returns them as a vector.
 *
 * **Interpretation:**
 * The string represents a hyperedge in a hypergraph.
 * Each integer in the string corresponds to a node connected by this hyperedge.
 * 
 * **Example Input:**
 * "7 14 18 22 26 29 32"
 */
std::vector<Node> getAllNodesFromEdge(const std::string& edge, std::istringstream& iss) {
    iss.clear();
    iss.str(edge);

    std::vector<Node> values((std::istream_iterator<Node>(iss)), std::istream_iterator<Node>());

    return values;
}