#include "utils.hpp"
#include "adaptiveGreedy.hpp"
#include "greedy.hpp"
#include <iostream>
#include <csignal>

void signalHandler(int signal) {
    if (signal == SIGTERM) {
        Greedy::sigterm_received = true;
    }
}

int main(int argc, char *argv[]) {
#if _DEBUG
    argc = 3;
    const char* fakeArgv[] = { argv[0], "greedy", "bremen_subgraph_20.hgr"};
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
#if _DEBUG
    auto [n, m, setSystem] = parseInputFile(argv[2]);
#endif
#ifndef _DEBUG
    auto [n, m, setSystem] = parseStdIn();
#endif

    std::signal(SIGTERM, signalHandler);

    // Berechnung
    std::vector<Node> solution;
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