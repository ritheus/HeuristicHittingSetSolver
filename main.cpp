#include "utils.hpp"
#include "greedyState.hpp"
#include "adaptiveGreedyState.hpp"
#include "vcState.hpp"
#include "branchAndReduceState.hpp"
#include "kernelization.hpp"
#include "sigtermHandler.hpp"
#include "cxxopts.hpp"
#include "localSearch.hpp"
#include "localSearchStrategy.hpp"
#include "adaptiveGreedyTabuLocalSearch.hpp"
#include "LPLocalSearch.hpp"
#include "randomLocalSearch.hpp"
#include <iostream>
#include <fstream>

#define LOGGING

int main(int argc, char* argv[]) {
#if _DEBUG
    //"--kernelization_unitEdgeRule", "--kernelization_vertexDominationRule", "--kernelization_edgeDominationRule"
    const char* fakeArgv[] = { argv[0], "-a", "vc", "-i", "exact_004.hgr", "--kernelization_unitEdgeRule", "--kernelization_vertexDominationRule", "--kernelization_edgeDominationRule", "--localSearch_tabu"};
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
        ("localSearch_tabu", "Apply greedy tabu local search")
        ("localSearch_random", "Apply random local search")
        ("localSearch_LP", "Apply LP local search")
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
    std::unique_ptr<AlgorithmState> state;
    if (algorithm == "greedy") {
        state = std::make_unique<GreedyState>(n, m, std::move(setSystem), optionsResult); // O(n * log n + m * deg_edge)
        solution = state->calculateSolution();
    }
    else if (algorithm == "adaptivegreedy") {
        state = std::make_unique<AdaptiveGreedyState>(n, m, std::move(setSystem), optionsResult);
        solution = state->calculateSolution();
    }
    else if (algorithm == "vc") {
        state = std::make_unique<VCState>(n, m, std::move(setSystem), optionsResult);
        solution = state->calculateSolution();
    }
    else if (algorithm == "branchandreduce") {
        state = std::make_unique<BranchAndReduceState>(n, m, std::move(setSystem), optionsResult);
        solution = state->calculateSolution();
    }
    else {
        throw std::runtime_error("Es wurde kein Algorithmus ausgewählt.");
    }


    if (optionsResult.count("localSearch_tabu") || optionsResult.count("localSearch_random") || optionsResult.count("localSearch_LP")) {
        std::unique_ptr<LocalSearchStrategy> localSearchStrategy;
        if (optionsResult.count("localSearch_tabu")) {
			localSearchStrategy = std::make_unique<AdaptiveGreedyTabuLocalSearch>(state->hypergraph, state->getSolution(), 10);
		}
		else if (optionsResult.count("localSearch_random")) {
			localSearchStrategy = std::make_unique<RandomLocalSearch>();
		}
        else if (optionsResult.count("localSearch_LP")) {
            auto* vcState = dynamic_cast<VCState*>(state.get()); // static_cast is faster
			if (vcState == nullptr) {
				throw std::runtime_error("LP Local Search only works for VC State");
			}
            else {
                localSearchStrategy = std::make_unique<LPLocalSearch>(vcState->getOrderedFractionalSolution());
            }
        }
        LocalSearch localSearch(std::move(state), std::move(localSearchStrategy), optionsResult);
        solution = localSearch.run(1000, 2);
	}
    

    // Output
    writeToStdOut(solution.getSolution());

    return 0;
}