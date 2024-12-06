#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include "aliases.hpp"
#include "greedy.hpp"

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
 */
std::vector<Node> getAllNodesFromEdge(const std::string& edge) {
    std::vector<Node> result;
    std::istringstream ss(edge);
    Node tmp;
    while(ss >> tmp) {
        result.push_back(tmp);
    }
    return result;
}

std::string toString(std::vector<std::vector<Node>>& setSystem) {
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

std::string toString(std::vector<Node>& vec) {
    std::stringstream ss;
    for (const Node node : vec) {
        ss << node << '\n';
    }
    return ss.str();
}

std::string toString(std::unordered_set<Node>& set) {
    std::stringstream ss;
    for (const Node node : set) {
        ss << node << '\n';
    }
    return ss.str();
}

int main(int argc, char *argv[]) {
#if _DEBUG
    argc = 2;
    const char* fakeArgv[] = { argv[0], "bremen_subgraph_20.hgr" };
    argv = const_cast<char**>(fakeArgv);
#endif

    // Fehlererkennung
    if (argc != 2) {
        throw std::invalid_argument("Es muss genau der Pfad zum Set System übergeben werden.");
    }
    std::string inputFileName = argv[1];

    // Modell
    std::vector<std::vector<Node>> setSystem;
    NumNodes n;
    NumEdges m;

    // Eingabedatei parsen und Modell füllen
    std::ifstream inputFile (inputFileName);
    if (inputFile.is_open()) {
        std::string line;
        while (getline(inputFile, line)) {
            if (line[0] == 'c') {

            }
            else if (line[0] == 'p') {
                std::tie(n, m) = getSetSystemParameters(line);
                if (n > 0 && m > 0) {
                    //setSystem.reserve(m); ergibt keinen sinn
                }
                else {
                    std::cout << "setSystemParameters konnten nicht ermittelt werden." << std::endl;
                }
            }
            else {
                setSystem.push_back(getAllNodesFromEdge(line));
            }
        }
        inputFile.close();
    }

    // Eingabe ausgeben
    std::cout << "Eingabe:" << std::endl;
    std::cout << "n: " << n << " m: " << m << std::endl;
    std::cout << toString(setSystem) << std::endl;

    // Berechnung
    std::vector<Node> solution = calculateSolution(n, m, setSystem);
    std::cout << "Folgende Loesung wurde berechnet:" << std::endl;
    std::cout << solution.size() << std::endl;
    std::cout << toString(solution) << std::endl;

    // Lösung in eine Datei schreiben
    std::string outputFileName = inputFileName.replace(inputFileName.find(".hgr"), 4, ".sol");
    std::ofstream outputFile(outputFileName);
    if (outputFile.is_open()) {
        outputFile << solution.size() << '\n';
        for (Node node : solution) {
            outputFile << node << '\n';
        }
        outputFile.close();
    }

    return 0;
}