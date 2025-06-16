
# Hittingset

**Hittingset** is a command-line tool offering several **heuristic algorithms to solve the Minimum Hitting Set Problem**.  
It is highly configurable via command-line parameters and intended for research, benchmarking, or educational purposes.

---

## Features

- Multiple algorithms: Greedy, Adaptive Greedy, VC, (a very bad) Branch-and-Reduce
- Advanced local search options
- Kernelization rules (reduction rules) for problem simplification
- Configurable neighborhood strategies for local search

---

## Requirements

### General

- C++17-compatible compiler
- [GLPK (GNU Linear Programming Kit)](https://www.gnu.org/software/glpk/)
  - Must be installed and available via `<glpk.h>`

### Windows (Visual Studio 2022)

- Developed and tested under **Visual Studio 2022** with C++17
- GLPK integrated locally (e.g., via NuGet package or manual linking)

### Linux

Install GLPK via:

```bash
sudo apt update
sudo apt install build-essential cmake libglpk-dev
```

Build project via:

```bash
mkdir build
cd build
cmake ..
make
```

---

## Recommended Usage Examples

Basic greedy run:

```bash
./hittingset -a greedy -i BigExample.hgr
```

Greedy with explorative local search (uniform random destruction + greedy repair) and flat neighborhood:

```bash
./hittingset -a greedy -i BigExample.hgr --kernelization_unitEdgeRule --localSearch_random --localSearch_numIterations 100000 --localSearch_numDeletions 5 --neighborhood_flat
```

VC-based algorithm with explorative LP-based local search (LP-weighted random destruction + greedy repair) and shrinking neighborhood. This is particularly useful if you already have an LP solution, but the code needs to be tweaked in this case:

```bash
./hittingset -a VC -i SmallExample.hgr --kernelization_allRules --localSearch_LP --localSearch_numIterations 100000 --localSearch_numDeletions 100 --neighborhood_shrinking --neighborhood_minDeletions 5 --neighborhood_stepInterval 800
```

---

## Command-Line Parameters

The program accepts a wide range of parameters to control its behavior.

### General Options

| Option                | Description                                                             |
|-----------------------|-------------------------------------------------------------------------|
| `-a`, `--algorithm`   | Selects algorithm (`Greedy`, `AdaptiveGreedy`, `BranchAndReduce`, `VC`) |
| `-i`, `--input`       | Input file in `.hgr` format (if omitted, reads from `stdin`)            |
| `-h`, `--help`        | Display help information and exit                                       |

### VC-specific Options

| Option               | Description                                   | Default |
|----------------------|-----------------------------------------------|---------|
| `--vc_numIterations` | Number of iterations for random rounding      | `1000`  |

### Local Search Options

| Option                        | Description                                 | Default |
|-------------------------------|---------------------------------------------|---------|
| `--localSearch_numIterations` | Number of local search iterations           | `0`     |
| `--localSearch_numDeletions`  | Number of nodes deleted per iteration       | `0`     |
| `--localSearch_tabuLength`    | Length of the tabu list                     | `10`    |
| `--localSearch_tabu`          | Enable adaptive greedy tabu search          |         |
| `--localSearch_random`        | Enable random local search                  |         |
| `--localSearch_randomTabu`    | Random local search with tabu list          |         |
| `--localSearch_LP`            | LP-based local search                       |         |
| `--localSearch_randomLP`      | LP-based local search with random repair    |         |

### Kernelization (Reduction) Rules

| Option                                 | Description                       |
|----------------------------------------|-----------------------------------|
| `--kernelization_unitEdgeRule`         | Apply Unit-Edge rule              |
| `--kernelization_vertexDominationRule` | Apply Vertex-Domination rule      |
| `--kernelization_edgeDominationRule`   | Apply Edge-Domination rule        |
| `--kernelization_criticalCoreRule`     | Apply Critical-Core rule          |
| `--kernelization_allRules`             | Apply all kernelization rules     |

### Neighborhood Strategies

| Option                                 | Description                                                | Default |
|----------------------------------------|------------------------------------------------------------|---------|
| `--neighborhood_flat`                  | Use flat neighborhood                                      |         |
| `--neighborhood_shrinking`             | Use shrinking neighborhood                                 |         |
| `--neighborhood_oscillating`           | Use oscillating neighborhood                               |         |
| `--neighborhood_shrinking_oscillating` | Use shrinking first, then oscillating neighborhood         |         |
| `--neighborhood_minDeletions`          | Minimum deletions per iteration                            | `5`     |
| `--neighborhood_stepInterval`          | Interval (in iterations) for deletion count reduction      | `1000`  |
| `--neighborhood_period`                | Cycle length for oscillating behavior                      | `1000`  |
| `--neighborhood_intensifying`          | Return to best solution after several worsening iterations |         |
| `--neighborhood_numIntensify`          | Number of worsening iterations before revert               | `1`     |
| `--neighborhood_exploring`             | Disable revert to best solution                            |         |

---

## Contact

If you encounter any issues or have questions, feel free to contact:  
📧 **richardtheus7@gmail.com**
