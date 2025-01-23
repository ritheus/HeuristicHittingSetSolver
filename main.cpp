#include "utils.hpp"
#include "adaptiveGreedy.hpp"
#include "greedy.hpp"
#include "sigtermHandler.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
#if _DEBUG
    argc = 3;
    const char* fakeArgv[] = { argv[0], "adaptiveGreedy", "bremen_subgraph_20.hgr"};
    argv = const_cast<char**>(fakeArgv);
#endif

    bool useGreedy = false;
    bool useAdaptiveGreedy = false;
    if (strcmp(argv[1], "greedy") == 0) {
        useGreedy = true;
    }
    if (strcmp(argv[1], "adaptiveGreedy") == 0) {
        useAdaptiveGreedy = true;
    }

    // Modell einlesen
    std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> result;
#if _DEBUG
    std::ifstream inputFile(argv[2]);
    if (inputFile.is_open()) {
        result = parseInputStream(inputFile);
    }
    else {
        std::cout << "File couldnt be read!" << std::endl;
        result = parseInputStream(std::cin);
    }
    auto [n, m, setSystem] = result;
#endif
#ifndef _DEBUG
    auto [n, m, setSystem] = parseInputStream(std::cin);
#endif

    std::signal(SIGTERM, signalHandler);

    // Berechnung
    std::unordered_set<Node> solution;
    if (useGreedy) {
        solution = Greedy::calculateSolution(n, m, setSystem);
    }
    else if (useAdaptiveGreedy) {
        solution = AdaptiveGreedy::calculateSolution(n, m, setSystem);
    }

    // Lösung in stdout ausgeben
    writeToStdOut(solution);

    return 0;
}