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

std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> parseInputStream(std::istream& input) {
    std::vector<std::vector<Node>> setSystem;
    NumNodes n = 0;
    NumEdges m = 0;

    const size_t BUFFER_SIZE = 8192; // 8 kb block size
    std::vector<char> buffer(BUFFER_SIZE);
    std::string leftover; // Prefix of lines that exceed the buffer size

    while (input.read(buffer.data(), BUFFER_SIZE) || input.gcount() > 0) {
        size_t bytesRead = input.gcount();
        std::string chunk(buffer.data(), bytesRead);

        if (!leftover.empty()) {
            chunk = leftover + chunk;
            leftover.clear();
        }

        size_t pos = 0;
        size_t lastNewline = 0;
        while ((pos = chunk.find('\n', lastNewline)) != std::string::npos) {
            std::string line = chunk.substr(lastNewline, pos - lastNewline);
            lastNewline = pos + 1; // Next line

            if (line.empty() || line[0] == 'c') continue;

            if (line[0] == 'p') {
                std::tie(n, m) = getSetSystemParameters(line);
                if (n <= 0 || m <= 0) {
                    std::cerr << "setSystemParameters konnten nicht ermittelt werden." << std::endl;
                }
            }
            else {
                setSystem.emplace_back(getAllNodesFromEdge(line));
            }
        }

        // Save prefix of line exceeding block size
        if (lastNewline < chunk.size()) {
            leftover = chunk.substr(lastNewline);
        }
    }

    return { n, m, std::move(setSystem) };
}

std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> parseInputFile(const std::string& inputFileName) {
    std::istringstream ss;

    std::ifstream inputFile(inputFileName);
    if (inputFile.is_open()) {
        return parseInputStream(inputFile);
    }
}

std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> parseStdIn() {
    return parseInputStream(std::cin);
}

void writeToStdOut(const std::unordered_set<Node>& solution) {
    std::string buffer;
    buffer.reserve(solution.size() * 6 + 10); // heuristic: <=6 chars per node (inkl. '\n')

    // n
    buffer += std::to_string(solution.size()) + '\n';

    // write all nodes into buffer
    for (Node node : solution) {
        buffer += std::to_string(node) + '\n';
    }

    std::cout.write(buffer.data(), buffer.size());
    std::cout.flush();
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
std::vector<Node> getAllNodesFromEdge(const std::string& edge) {
    std::vector<Node> values;
    values.reserve(edge.size() / 2);  // Estimate number of nodes in edge

    const char* str = edge.c_str();
    char* end;

    while (*str) {
        while (*str == ' ') ++str;  // Skip spaces
        if (*str == '\0') break;

        Node num = static_cast<Node>(std::strtoul(str, &end, 10));
        values.emplace_back(num);

        str = end;  // Next character
    }

    return values;
}