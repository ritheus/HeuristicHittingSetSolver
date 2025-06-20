#include "peripheral/utils.hpp"
#include "algorithms/greedyState.hpp"
#include "algorithms/adaptiveGreedyState.hpp"
#include "algorithms/vcState.hpp"
#include "algorithms/branchAndReduceState.hpp"
#include "kernelization.hpp"
#include "peripheral/sigtermHandler.hpp"
#include "peripheral/cxxopts.hpp"
#include "localSearch/localSearch.hpp"
#include "localSearch/localSearchStrategy.hpp"
#include "localSearch/adaptiveGreedyTabuLocalSearch.hpp"
#include "localSearch/LPLocalSearch.hpp"
#include "localSearch/randomLPLocalSearch.hpp"
#include "localSearch/randomLocalSearch.hpp"
#include "localSearch/randomTabuLocalSearch.hpp"
#include "localSearch/neighborhood/neighborhoodStrategy.hpp"
#include "localSearch/neighborhood/flatNeighborhoodStrategy.hpp"
#include "localSearch/neighborhood/shrinkingNeighborhoodStrategy.hpp"
#include "localSearch/neighborhood/oscillatingNeighborhoodStrategy.hpp"
#include "localSearch/neighborhood/shrinkAndOscillateNeighborhoodStrategy.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

//#define LOGGING
//#define PROFILER

int main(int argc, char* argv[]) {
#if _DEBUG
    //"--kernelization_unitEdgeRule", "--kernelization_vertexDominationRule", "--kernelization_edgeDominationRule"
    const char* fakeArgv[] = { argv[0], "-a", "greedy", "-i", "SmallExample.hgr", "--localSearch_random", "--localSearch_numIterations", "100000", "--localSearch_numDeletions", "5" };
    argc = sizeof(fakeArgv) / sizeof(fakeArgv[0]);
    argv = const_cast<char**>(fakeArgv);
    std::cout << "DEBUG MODE ACTIVATED" << std::endl;
#endif
#ifdef PROFILER
    const char* fakeArgv[] = { argv[0], "-a", "greedy", "-i", "BigExample.hgr", "--localSearch_random", "--localSearch_numIterations", "1000", "--neighborhood_minDeletions", "50", "--localSearch_numDeletions", "10000", "--kernelization_unitEdgeRule", "--neighborhood_flat", "--neighborhood_period", "800", "--neighborhood_stepInterval", "800"};
    argc = sizeof(fakeArgv) / sizeof(fakeArgv[0]);
    argv = const_cast<char**>(fakeArgv);
#endif

    cxxopts::Options options("Hittingset",
        "A heuristic solver for the Minimum Hitting Set problem.\n"
        "Allows selection of solving algorithms and optional kernelization rules to reduce instance size.\n"
        "Use the flags below to configure the solver.\n");

    options.add_options()
        ("a,algorithm", "Algorithm to use (e.g. Greedy, AdaptiveGreedy, BranchAndReduce, VC)", cxxopts::value<std::string>())
        ("vc_numIterations", "Randomly round the fractional solution of the VC algorithm to the specified number of iterations", cxxopts::value<uint32_t>()->default_value("1000"))
        ("localSearch_numIterations", "How many local search iterations to run", cxxopts::value<uint32_t>()->default_value("100000"))
        ("localSearch_numDeletions", "How many nodes to delete per local search iteration", cxxopts::value<uint32_t>()->default_value("5"))
        ("localSearch_tabuLength", "How long the tabu list should be", cxxopts::value<uint32_t>()->default_value("25"))
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
        ("neighborhood_shrinking_oscillating", "Apply local search to a first shrinking, then oscillating neighborhood")
        ("neighborhood_minDeletions", "How many nodes to delete per local search iteration at least", cxxopts::value<uint32_t>()->default_value("5"))
        ("neighborhood_stepInterval", "After how many iterations to reduce the number of deletions", cxxopts::value<uint32_t>()->default_value("800"))
        ("neighborhood_period", "After how many iterations to cycle the number of deletions", cxxopts::value<uint32_t>()->default_value("800"))
        ("neighborhood_intensifying", "Revert to best solution after a fixed number of worse current solutions")
        ("neighborhood_numIntensify", "After how many bad iterations to revert back to best solution", cxxopts::value<uint32_t>()->default_value("400"))
        ("neighborhood_exploring", "Do not revert to best solution after a fixed number of worse current solutions")
        ("i,input", "Use the specified input file instead of reading from stdin", cxxopts::value<std::string>())
        ("h,help", "Show this help message and exit");

    const cxxopts::ParseResult& optionsResult = options.parse(argc, argv);

    if (optionsResult.count("help")) {
        std::cout << options.help() << std::endl;
        std::cout << "\nExamples:\n";
        std::cout << "  ./hittingset -a greedy -i BigExample.hgr --kernelization_unitEdgeRule --localSearch_random --localSearch_numIterations 100000 --neighborhood_minDeletions 5 --localSearch_numDeletions 100 --neighborhood_shrinking --neighborhood_stepInterval 800\n";
        std::cout << "  ./hittingset -a VC -i SmallExample.hgr --kernelization_allRules --localSearch_LP --localSearch_numIterations 100000 --neighborhood_minDeletions 5 --localSearch_numDeletions 100 --neighborhood_shrinking --neighborhood_stepInterval 800\n";
        return 0;
    }

    std::cout << "Starting Hittingset Solver" << std::endl;

    // Read input
    std::tuple<NumNodes, NumEdges, std::vector<std::vector<Node>>> result;
    std::ifstream inputFile(optionsResult["input"].as<std::string>());
    if (inputFile.is_open()) {
        std::cout << "Reading from file..." << std::endl;
        result = parseInputStream(inputFile);
    }
    else {
        std::cout << "File couldnt be read! Reading from stdin..." << std::endl;
        result = parseInputStream(std::cin);
    }
    auto [n, m, setSystem] = result;

    std::signal(SIGTERM, signalHandler);

    // Solve
    auto toLower = [](std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
        };
    std::string algorithm = "greedy";
    if (optionsResult.count("algorithm")) {
        algorithm = toLower(optionsResult["algorithm"].as<std::string>());
    }

    Solution solution = Solution(n);
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
        throw std::runtime_error("No algorithm was chosen.");
    }
    solution = state->calculateSolution();

    uint32_t numIterations = optionsResult["localSearch_numIterations"].as<uint32_t>();
    uint32_t numDeletions = optionsResult["localSearch_numDeletions"].as<uint32_t>();
    if (numDeletions == 0) {
        numDeletions = std::max<size_t>(solution.size() / 30, 5);
    }
    bool intensify = optionsResult.count("neighborhood_intensifying");
    uint32_t intensifyThreshold = optionsResult["neighborhood_numIntensify"].as<uint32_t>();
    uint32_t tabuLength = optionsResult["localSearch_tabuLength"].as<uint32_t>();

    if (optionsResult.count("localSearch_tabu") || optionsResult.count("localSearch_random") || optionsResult.count("localSearch_randomLP") || optionsResult.count("localSearch_LP") || optionsResult.count("localSearch_randomTabu")) {
        std::unique_ptr<LocalSearchStrategy> localSearchStrategy;
        std::unique_ptr<LocalSearchStrategy> localSearchStrategy2;
        GreedyState greedyState(state->hypergraph, {});
        if (optionsResult.count("localSearch_tabu")) {
            state = std::make_unique<AdaptiveGreedyState>(state->hypergraph, optionsResult);
            state->setSolution(solution);
			localSearchStrategy = std::make_unique<AdaptiveGreedyTabuLocalSearch>(state->hypergraph, state->getSolution(), tabuLength);
		}
        else if (optionsResult.count("localSearch_LP")) {
            auto* vcState = dynamic_cast<VCState*>(state.get());
            if (vcState == nullptr) {
                localSearchStrategy = std::make_unique<LPLocalSearch>(greedyState);
            }
            else {
                localSearchStrategy = std::make_unique<LPLocalSearch>(greedyState, vcState->getOrderedFractionalSolution());
            }
        }
        else if (optionsResult.count("localSearch_random")) {
            state = std::make_unique<GreedyState>(state->hypergraph, optionsResult);
            state->setSolution(solution);
            localSearchStrategy = std::make_unique<RandomLocalSearch>();
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
        
        uint32_t revertSolutionThreshold = optionsResult["neighborhood_numIntensify"].as<uint32_t>();
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
        else if (optionsResult.count("neighborhood_shrinking_oscillating")) {
            uint32_t minNumNodesToDelete = optionsResult["neighborhood_minDeletions"].as<uint32_t>();
            uint32_t period = optionsResult["neighborhood_period"].as<uint32_t>();
            uint32_t stepInterval = optionsResult["neighborhood_stepInterval"].as<uint32_t>();
            neighborhoodStrategy = std::make_unique<ShrinkAndOscillateNeighborhoodStrategy>(numIterations, minNumNodesToDelete, numDeletions, stepInterval, period);
        }
        LocalSearch localSearch(std::move(state), std::move(localSearchStrategy), std::move(neighborhoodStrategy), optionsResult);
        localSearch.strategy->initializeAlgorithmState(std::move(localSearch.state));
        solution = localSearch.run(intensify, revertSolutionThreshold);
	}
    

    // Output
    writeToStdOut(solution.getSolution());

    return 0;
}