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
#include "randomLPLocalSearch.hpp"
#include "randomLocalSearch.hpp"
#include "randomTabuLocalSearch.hpp"
#include "neighborhoodStrategy.hpp"
#include "flatNeighborhoodStrategy.hpp"
#include "shrinkingNeighborhoodStrategy.hpp"
#include "oscillatingNeighborhoodStrategy.hpp"
#include <iostream>
#include <fstream>

//#define LOGGING
//#define PROFILER

int main(int argc, char* argv[]) {
#if _DEBUG
    //"--kernelization_unitEdgeRule", "--kernelization_vertexDominationRule", "--kernelization_edgeDominationRule"
    const char* fakeArgv[] = { argv[0], "-a", "greedy", "-i", "exact_001.hgr", "--localSearch_numIterations", "100000", "--localSearch_random", "--localSearch_numDeletions", "50", "--localSearch_tabuLength", "0", "--neighborhood_oscillating", "--neighborhood_period", "1000"};
    argc = sizeof(fakeArgv) / sizeof(fakeArgv[0]);
    argv = const_cast<char**>(fakeArgv);
#endif
#ifdef PROFILER
    const char* fakeArgv[] = { argv[0], "-a", "greedy", "-i", "coauth_3700k.hgr", "--localSearch_numIterations", "100000", "--localSearch_numDeletions", "5", "--kernelization_unitEdgeRule"};
    argc = sizeof(fakeArgv) / sizeof(fakeArgv[0]);
    argv = const_cast<char**>(fakeArgv);
#endif

    cxxopts::Options options("Hittingset",
        "A heuristic solver for the Minimum Hitting Set problem.\n"
        "Allows selection of solving algorithms and optional kernelization rules to reduce instance size.\n"
        "Use the flags below to configure the solver.\n");

    options.add_options()
        ("a,algorithm", "Algorithm to use (e.g. Greedy, AdaptiveGreedy, BranchAndReduce, VC)", cxxopts::value<std::string>())
        ("vc_numIterations", "Randomly round the fractional solution of the VC algorithm to the specified number of iterations", cxxopts::value<uint32_t>()->default_value("5"))
        ("localSearch_numIterations", "How many local search iterations to run", cxxopts::value<uint32_t>()->default_value("5"))
        ("localSearch_numDeletions", "How many nodes to delete per local search iteration", cxxopts::value<uint32_t>()->default_value("5"))
        ("localSearch_tabuLength", "How long the tabu list should be", cxxopts::value<uint32_t>()->default_value("10"))
        ("kernelization_unitEdgeRule", "Apply the Unit Edge Rule kernelization method")
        ("kernelization_vertexDominationRule", "Apply the Vertex Domination Rule kernelization method")
        ("kernelization_edgeDominationRule", "Apply the Edge Domination Rule kernelization method")
        ("kernelization_criticalCoreRule", "Apply the Critical Core Rule kernelization method")
        ("kernelization_allRules", "Apply all kernelization rules")
        ("localSearch_tabu", "Apply adaptive greedy tabu local search")
        ("localSearch_random", "Apply random local search")
        ("localSearch_randomTabu", "Apply random local search with tabu list")
        ("localSearch_LP", "Apply LP local search")
        ("localSearch_randomLP", "Apply LP local search with random repair")
        ("neighborhood_flat", "Apply local search to a flat neighborhood")
        ("neighborhood_shrinking", "Apply local search to a shrinking neighborhood")
        ("neighborhood_oscillating", "Apply local search to an oscillating neighborhood")
        ("neighborhood_minDeletions", "How many nodes to delete per local search iteration at least", cxxopts::value<uint32_t>()->default_value("5"))
        ("neighborhood_stepInterval", "After how many iterations to reduce the number of deletions", cxxopts::value<uint32_t>()->default_value("1000"))
        ("neighborhood_period", "After how many iterations to cycle the number of deletions", cxxopts::value<uint32_t>()->default_value("1000"))
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
#ifdef PROFILER
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
#ifndef PROFILER
    auto [n, m, setSystem] = parseInputStream(std::cin);
#endif
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
    }
    else if (algorithm == "adaptivegreedy") {
        state = std::make_unique<AdaptiveGreedyState>(n, m, std::move(setSystem), optionsResult);
    }
    else if (algorithm == "vc") {
        state = std::make_unique<VCState>(n, m, std::move(setSystem), optionsResult);
    }
    else if (algorithm == "branchandreduce") {
        state = std::make_unique<BranchAndReduceState>(n, m, std::move(setSystem), optionsResult);
    }
    else {
        throw std::runtime_error("Es wurde kein Algorithmus ausgewählt.");
    }
    solution = state->calculateSolution();

    uint32_t numIterations = optionsResult["localSearch_numIterations"].as<uint32_t>();
    uint32_t numDeletions = optionsResult["localSearch_numDeletions"].as<uint32_t>();
    uint32_t tabuLength = optionsResult["localSearch_tabuLength"].as<uint32_t>();
    if (optionsResult.count("localSearch_tabu") || optionsResult.count("localSearch_random") || optionsResult.count("localSearch_randomLP") || optionsResult.count("localSearch_LP") || optionsResult.count("localSearch_randomTabu")) {
        std::unique_ptr<LocalSearchStrategy> localSearchStrategy;
        if (optionsResult.count("localSearch_tabu")) {
            state = std::make_unique<AdaptiveGreedyState>(state->hypergraph, optionsResult);
            state->setSolution(solution);
			localSearchStrategy = std::make_unique<AdaptiveGreedyTabuLocalSearch>(state->hypergraph, state->getSolution(), tabuLength);
		}
		else if (optionsResult.count("localSearch_random")) {
            state = std::make_unique<GreedyState>(state->hypergraph, optionsResult);
			state->setSolution(solution);
			localSearchStrategy = std::make_unique<RandomLocalSearch>();
		}
        else if (optionsResult.count("localSearch_LP")) {
            auto* vcState = dynamic_cast<VCState*>(state.get());
            GreedyState greedyState(state->hypergraph, {});
            if (vcState == nullptr) {
                localSearchStrategy = std::make_unique<LPLocalSearch>(greedyState);
            }
            else {
                localSearchStrategy = std::make_unique<LPLocalSearch>(greedyState, vcState->getOrderedFractionalSolution());
            }
        }
        else if (optionsResult.count("localSearch_randomLP")) {
            auto* vcState = dynamic_cast<VCState*>(state.get());
            if (vcState == nullptr) {
                localSearchStrategy = std::make_unique<RandomLPLocalSearch>(state->hypergraph, state->solution);
            }
            else {
                localSearchStrategy = std::make_unique<RandomLPLocalSearch>(vcState->getOrderedFractionalSolution(), state->solution);
            }
        }
        else if (optionsResult.count("localSearch_randomTabu")) {
            localSearchStrategy = std::make_unique<RandomTabuLocalSearch>(state->hypergraph, solution, tabuLength);
        }
        LocalSearch localSearch(std::move(state), std::move(localSearchStrategy), optionsResult);

        std::unique_ptr<NeighborhoodStrategy> neighborhoodStrategy;
        if (optionsResult.count("neighborhood_flat")) {
            neighborhoodStrategy = std::make_unique<FlatNeighborhoodStrategy>(numIterations, numDeletions);
        }
        else if (optionsResult.count("neighborhood_shrinking")) {
            uint32_t minNumNodesToDelete = optionsResult["neighborhood_minDeletions"].as<uint32_t>();
            uint32_t stepInterval = optionsResult["neighborhood_stepInterval"].as<uint32_t>();
            neighborhoodStrategy = std::make_unique<ShrinkingNeighborhoodStrategy>(numIterations, minNumNodesToDelete, numDeletions, stepInterval);
        }
        else if (optionsResult.count("neighborhood_oscillating")) {
            uint32_t minNumNodesToDelete = optionsResult["neighborhood_minDeletions"].as<uint32_t>();
            uint32_t period = optionsResult["neighborhood_period"].as<uint32_t>();
            neighborhoodStrategy = std::make_unique<OscillatingNeighborhoodStrategy>(numIterations, minNumNodesToDelete, numDeletions, period);
        }

        solution = localSearch.run(std::move(neighborhoodStrategy));
	}
    

    // Output
    writeToStdOut(solution.getSolution());

    return 0;
}