#include "utils.hpp"
#include "greedy.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
#if _DEBUG
    argc = 2;
    const char* fakeArgv[] = { argv[0], "bremen_subgraph_20.hgr" };
    argv = const_cast<char**>(fakeArgv);
#endif

    // Modell einlesen
    //auto [n, m, setSystem] = parseInputFile(inputFileName);
    auto [n, m, setSystem] = parseStdIn();

    // Berechnung
    std::vector<Node> solution = calculateSolution(n, m, setSystem);

    // Lösung in stdout ausgeben
    writeToStdOut(solution);

    return 0;
}