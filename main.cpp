#include "utils.hpp"
#include "greedyState.hpp"
#include "adaptiveGreedyState.hpp"
#include "vcState.hpp"
#include "branchAndReduceState.hpp"
#include "kernelization.hpp"
#include "sigtermHandler.hpp"
#include "cxxopts.hpp"
#include <iostream>
#include <fstream>

#define LOGGING

int main(int argc, char* argv[]) {
#if _DEBUG
    //"--kernelization_unitEdgeRule", "--kernelization_vertexDominationRule", "--kernelization_edgeDominationRule"
    const char* fakeArgv[] = { argv[0], "-a", "vc", "-i", "exact_009.hgr", "--kernelization_unitEdgeRule", "--kernelization_vertexDominationRule" };
    argc = sizeof(fakeArgv) / sizeof(fakeArgv[0]);
    argv = const_cast<char**>(fakeArgv);
#endif

    cxxopts::Options options("Hittingset",
        "A heuristic solver for the Minimum Hitting Set problem.\n"
        "Allows selection of solving algorithms and optional kernelization rules to reduce instance size.\n"
        "Use the flags below to configure the solver.\n");

    options.add_options()
        ("a,algorithm", "Algorithm to use (e.g., Greedy, AdaptiveGreedy, BranchAndReduce)", cxxopts::value<std::string>())
        ("kernelization_unitEdgeRule", "Apply the Unit Edge Rule kernelization method")
        ("kernelization_vertexDominationRule", "Apply the Vertex Domination Rule kernelization method")
        ("kernelization_edgeDominationRule", "Apply the Edge Domination Rule kernelization method")
        ("kernelization_criticalCoreRule", "Apply the Critical Core Rule kernelization method")
        ("kernelization_allRules", "Apply all kernelization rules")
        ("i,input", "Use the specified input file instead of reading from stdin", cxxopts::value<std::string>())
        ("h,help", "Show this help message and exit");

    const cxxopts::ParseResult& optionsResult = options.parse(argc, argv);

    if (optionsResult.count("help") || !optionsResult.count("algorithm")) {
        std::cout << options.help() << std::endl;
        std::cout << "\nExamples:\n";
        std::cout << "  ./hittingset -a Greedy --kernelization_unitEdgeRule --kernelization_vertexDominationRule\n";
        std::cout << "  ./hittingset --algorithm=AdaptiveGreedy --kernelization_allRules\n";
        return 0;
    }

    // Read input
#if _DEBUG
    std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> result;
    std::ifstream inputFile(optionsResult["input"].as<std::string>());
    if (inputFile.is_open()) {
        result = parseInputStream(inputFile);
    }
    else {
        std::cerr << "File couldnt be read!" << std::endl;
        result = parseInputStream(std::cin);
    }
    auto [n, m, setSystem] = result;
#endif
#ifndef _DEBUG
    auto [n, m, setSystem] = parseInputStream(std::cin);
#endif

    std::signal(SIGTERM, signalHandler);

    // Solve
    auto toLower = [](std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
        };
    std::string algorithm = toLower(optionsResult["algorithm"].as<std::string>());

    Solution solution;
    if (algorithm == "greedy") {
        GreedyState state = GreedyState(n, m, std::move(setSystem), optionsResult); // O(n * log n + m * deg_edge)
        solution = state.calculateSolution();
    }
    else if (algorithm == "adaptivegreedy") {
        AdaptiveGreedyState state = AdaptiveGreedyState(n, m, std::move(setSystem), optionsResult);
        solution = state.calculateSolution();
    }
    else if (algorithm == "vc") {
        VCState state = VCState(n, m, std::move(setSystem), optionsResult);
        solution = state.calculateSolution();
    }
    else if (algorithm == "branchandreduce") {
        BranchAndReduceState state = BranchAndReduceState(n, m, std::move(setSystem), optionsResult);
        solution = state.calculateSolution();
    }
    else {
        throw std::runtime_error("Es wurde kein Algorithmus ausgewählt.");
    }

    // Output
    writeToStdOut(solution.getSolution());

    return 0;
}