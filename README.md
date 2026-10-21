# Hybrid MPI-OpenMP Based Intelligent Waste Collection Route Optimization

A Parallel Computing Approach to Large-Scale, Priority-Aware Waste Collection Route Optimization.

**Subject:** Parallel Computing Applications (PCAP)  
**Team Members:**  
- **Nandini Sah** (Registration Number: 240905552, Branch: CSE, Section: D)  
- **Prisha Gupta** (Registration Number: 240905612, Branch: CSE, Section: D)  

---

## 📌 Project Overview
Municipal solid waste collection in rapidly expanding urban centers incurs immense financial, logistical, and environmental costs. Standard waste collection schedules dispatch trucks on static routes, servicing underfilled bins (<30%) and causing hazardous overflows in high-density commercial areas or healthcare facilities.

This project designs, implements, benchmarks, and documents an intelligent, parallel route optimization engine for the **Priority-Aware Capacitated Waste Collection Vehicle Routing Problem (PA-WCVRP)**. To scale to city-sized networks ($N=500$ bins), we design a hierarchical, dual-layer parallel computing system:
1. **Macro-Parallelism (MPI):** Coarse-grained distributed-memory island model where sub-populations evolve across distinct computing nodes and periodically exchange elite chromosomes via an asynchronous ring migration topology (`MPI_Sendrecv`).
2. **Micro-Parallelism (OpenMP):** Fine-grained shared-memory multi-threading within each MPI process, parallelizing chromosome decoding, multi-objective fitness calculation, and compute-intensive 2-opt memetic local search across local CPU cores (`#pragma omp parallel for schedule(dynamic)`).

---

## 🗂️ Repository Directory Structure
```
d:\Personal Data\Desktop\projects\pcap-mini\
├── data\
│   ├── cvrplib\                       # Standard CVRPLIB benchmarks (A-n32-k5, A-n54-k7)
│   └── synthetic\                     # Synthetic urban waste datasets (N=30, 100, 250, 500 bins)
├── src_cpp\
│   ├── common\                        # Shared core data models, evaluators, local search
│   │   ├── Types.hpp                  # Bin, Vehicle, Route, Solution, GAParameters
│   │   ├── InstanceLoader.hpp         # VRP benchmark file parser
│   │   ├── FitnessEvaluator.hpp       # Multi-objective fitness & priority penalty logic
│   │   └── LocalSearch.hpp            # 2-opt memetic local search operator
│   ├── sequential\                    # Baseline sequential Genetic Algorithm
│   │   ├── SequentialGA.hpp
│   │   └── main.cpp
│   ├── openmp\                        # OpenMP multi-threaded parallel GA
│   │   ├── OpenMPGA.hpp
│   │   └── main.cpp
│   ├── mpi\                           # Distributed-memory MPI island-model GA
│   │   ├── MPIGA.hpp
│   │   └── main.cpp
│   ├── hybrid\                        # Hierarchical Hybrid MPI-OpenMP GA
│   │   ├── HybridGA.hpp
│   │   └── main.cpp
│   └── tests\                         # Correctness test suite
│       └── test_correctness.cpp
├── experiments\
│   ├── benchmark_runner.py            # High-performance parallel benchmarking harness
│   ├── generate_datasets.py           # Dataset generator for CVRPLIB & synthetic benchmarks
│   ├── generate_plots.py              # Publication-quality vector figure generator
│   ├── results\                       # Benchmark CSV summary and convergence histories
│   └── figures\                       # 300+ DPI figures (speedup, efficiency, convergence, maps)
├── docs\
│   ├── literature_survey.md           # Systematic literature survey (10 verified papers)
│   ├── project_presentation.md        # Full slide deck & live demonstration walkthrough
│   └── conference_submission_guide.md # Target conference analysis & submission package
├── paper\
│   ├── research_paper.md              # Full research paper manuscript in Markdown
│   ├── ieee\
│   │   ├── paper_ieee.tex             # IEEE Conference 2-column LaTeX manuscript
│   │   └── references.bib             # Verified BibTeX citations
│   └── springer\
│       ├── paper_springer.tex         # Springer Nature LNCS LaTeX manuscript
│       └── references.bib             # Verified BibTeX citations
├── CMakeLists.txt                     # Root CMake build configuration
├── Dockerfile                         # Containerized Linux HPC environment
└── README.md                          # Project documentation
```

---

## ⚡ Quickstart & Reproduction Guide

### Prerequisites
- Python 3.10+ (with `numpy`, `matplotlib`)
- C++17 compatible compiler (GCC / Clang / MSVC)
- OpenMP runtime
- MPI Library (OpenMPI / MPICH / Microsoft MPI)
- CMake 3.14+

### 1. Generate Datasets
```bash
python experiments/generate_datasets.py
```

### 2. Run Comprehensive Benchmarks
```bash
python experiments/benchmark_runner.py
```
This executes sequential, OpenMP (2, 4, 8 threads), MPI (2, 4 ranks), and Hybrid MPI-OpenMP ($2P \times 2T$, $2P \times 4T$, $4P \times 2T$) configurations across all problem scales with 3 repetitions, outputting `experiments/results/benchmark_summary.csv`.

### 3. Generate Scientific Figures
```bash
python experiments/generate_plots.py
```
Outputs high-resolution vector figures in `experiments/figures/`:
- `fig1_speedup_scaling.png`: Amdahl speedup curves vs core count.
- `fig2_parallel_efficiency.png`: Parallel efficiency curves ($E = S / P$).
- `fig3_convergence_comparison.png`: Generational convergence showing island-model superiority.
- `fig4_route_visualization.png`: Urban route map with priority clusters and vehicle tours.
- `fig5_execution_time_breakdown.png`: Execution time breakdown across dataset scales.

### 4. Build C++ Executables via CMake
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j
ctest --output-on-failure
```

Run individual solvers:
```bash
# Sequential Baseline
./solver_sequential ../data/synthetic/waste_n100.vrp 100 150

# OpenMP Multi-threaded (4 threads)
./solver_openmp ../data/synthetic/waste_n100.vrp 4 100 150

# MPI Island Model (4 processes)
mpirun -np 4 ./solver_mpi ../data/synthetic/waste_n100.vrp 100 150

# Hybrid MPI-OpenMP (2 processes x 4 threads = 8 cores)
mpirun -np 2 ./solver_hybrid ../data/synthetic/waste_n100.vrp 4 100 150
```

### 5. Dockerized HPC Cluster Build
```bash
docker build -t pcap-waste-opt .
docker run --rm -it pcap-waste-opt
```

---

## 📊 Key Experimental Findings
- **Peak Speedup:** The Hybrid MPI-OpenMP solver ($4P \times 2T$) achieved a speedup of **6.81x** on 8 cores with **85.1% parallel efficiency** on the 500-bin metropolitan dataset.
- **Memory Contention Advantage:** Pure OpenMP was throttled at 5.76x on 8 threads due to memory bus saturation during intensive 2-opt reversals. Hybrid parallelism isolates memory spaces across 4 MPI processes, avoiding cache thrashing.
- **Algorithmic Quality:** The distributed island model with ring migration discovered routes with **4.5% to 8.2% lower overall objective cost** (saving over 1,180 km of driving distance on $N=500$) compared to sequential single-population search.
- **Priority Compliance:** High-hazard medical and market waste bins were serviced within the first 25% of vehicle route durations, completely eliminating overflow penalties.

---

## 📄 Conference Submission Packages
Complete manuscripts prepared per course guidelines:
1. **Springer Nature Format:** `paper/springer/paper_springer.tex` (LNCS single-column format).
2. **IEEE Conference Format:** `paper/ieee/paper_ieee.tex` (IEEEtran two-column format).
3. **BibTeX Citations:** `paper/ieee/references.bib` (10 verified peer-reviewed citations).
