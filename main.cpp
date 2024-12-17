#include "utils.hpp"
#include "greedy.hpp"
#include <iostream>

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

    // Modell füllen
    auto [n, m, setSystem] = parseInputFile(inputFileName);

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
    writeToFile(outputFileName, solution);

    return 0;
}