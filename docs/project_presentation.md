# Project Presentation & Demonstration Walkthrough

## Subject: Parallel Computing Applications (PCAP)
### Project Title: Hybrid MPI-OpenMP Based Intelligent Waste Collection Route Optimization
**Team Members:**
- **Nandini Sah** (Reg. No: 240905552, CSE D) — Algorithm Design, OpenMP Multi-threading, Mathematical Formulation
- **Prisha Gupta** (Reg. No: 240905612, CSE D) — Distributed MPI Island Model, Ring Migration, Experimental Benchmarking

---

## Slide 1: Title & Overview
- **Title:** Hybrid MPI-OpenMP Based Intelligent Waste Collection Route Optimization
- **Subtitle:** Accelerating Priority-Aware Municipal Logistics on Heterogeneous Architectures
- **Presenters:** Nandini Sah & Prisha Gupta
- **Key Message:** Municipal waste collection is an NP-hard combinatorial problem that cannot scale sequentially to city sizes. By leveraging a dual-layer hybrid MPI-OpenMP paradigm, we achieve up to 6.8x speedup and significantly superior route quality compared to sequential and pure single-paradigm solvers.

---

## Slide 2: Problem Statement & Real-World Motivation
- **Urban Crisis:** Municipalities spend up to 40% of their operational budgets on solid waste collection.
- **The Dual Waste Inefficiency:**
  1. *Underfilled bins (<40%):* Unnecessary truck trips, excessive fuel consumption, traffic congestion, and greenhouse gas ($CO_2$) emissions.
  2. *Overflowing bins (>85%):* Severe public health hazards, odor, and municipal regulatory penalties.
- **The Priority Dimension:** Medical clinics, hospitals, and central food markets generate hazardous/organic waste requiring urgent, timely collection, whereas residential bins have lower urgency.
- **Computational Barrier:** Formulated as a Priority-Aware Capacitated Vehicle Routing Problem (PA-WCVRP), evaluating candidate routes across hundreds of bins is computationally prohibitive for real-time dispatch without high-performance parallel computing.

---

## Slide 3: Systematic Literature Survey Highlights
- Reviewed 10 verified recent papers (2018–2024) across IEEE, Elsevier, Springer, and MDPI.
- **Hannan et al. (2018, 2020):** Modeled $CO_2$ emission costs for CVRP; emphasized high metaheuristic overhead.
- **Tirkolaee et al. (2021, 2023):** Integrated pandemic medical waste priority tiers and Pareto multi-objective optimization.
- **Bányai et al. (2020):** Proved IoT fill-level telemetry reduces route lengths by up to 28%.
- **Saxena et al. (2020):** Explored OpenMP parallelization for CVRP on multi-core CPUs.
- **Vidal et al. (2014):** Demonstrated that memetic local search (2-opt) achieves world-record route quality but consumes >85% of CPU runtime.
- **Research Gap:** No existing study investigates the hierarchical combination of distributed-memory MPI island models with shared-memory OpenMP multi-threading for priority-aware waste collection routing.

---

## Slide 4: Project Objectives
1. Mathematically formulate the **Priority-Aware Capacitated Waste Collection Vehicle Routing Problem (PA-WCVRP)** with dynamic fill levels and 3 priority tiers.
2. Develop a modular, clean C++ optimization suite containing 4 mathematically identical solvers:
   - Sequential Baseline (1 thread)
   - OpenMP Multi-threaded Solver
   - MPI Distributed Island-Model Solver
   - Hybrid MPI-OpenMP Hierarchical Solver
3. Conduct rigorous benchmarking on standard CVRPLIB benchmarks (A-n32-k5) and scaled synthetic municipal instances (N=100, 250, 500 bins).
4. Measure and analyze execution runtime, Amdahl speedup, parallel efficiency, and route quality trade-offs.
5. Prepare publication-ready conference manuscripts in both IEEE and Springer Nature formats.

---

## Slide 5: Mathematical Formulation
- **Graph:** $G = (V, E)$, Depot at Node $0$, $N$ bins.
- **Parameters:** Capacity $Q$, distance $d_{ij}$, fill level $f_i \in [0, 1]$, priority $p_i \in \{1, 2, 3\}$.
- **Objective Function:**
  $$\min \mathcal{F} = \text{Distance} + \alpha \cdot \text{Priority\_Penalty} + \text{Penalty}_{cap} + \text{Penalty}_{over}$$
- **Priority Penalty:**
  $$\text{Priority\_Penalty} = \sum_{k=1}^K \sum_{i \in \text{Route}_k} w(p_i) \cdot f_i \cdot T_{ik}$$
  where $w(1)=1.0, w(2)=2.5, w(3)=5.0$. High-priority and full bins are serviced early in the vehicle routes.

---

## Slide 6: System Architecture & Dual-Layer Parallelism
```
+--------------------------------------------------------------------------+
|                     MPI DISTRIBUTED MACRO-LAYER                          |
|                                                                          |
|  +--------------------+   Ring Migration    +--------------------+       |
|  |   MPI Rank 0       | <=================> |   MPI Rank 1       |       |
|  |  (Sub-population)  |                     |  (Sub-population)  |       |
|  +---------+----------+                     +---------+----------+       |
|            |                                          |                  |
|            v                                          v                  |
|  +--------------------+                     +--------------------+       |
|  | OPENMP MICRO-LAYER |                     | OPENMP MICRO-LAYER |       |
|  | (Shared Memory)    |                     | (Shared Memory)    |       |
|  |                    |                     |                    |       |
|  | Core 0: Eval / 2opt|                     | Core 0: Eval / 2opt|       |
|  | Core 1: Eval / 2opt|                     | Core 1: Eval / 2opt|       |
|  | Core 2: Eval / 2opt|                     | Core 2: Eval / 2opt|       |
|  | Core 3: Eval / 2opt|                     | Core 3: Eval / 2opt|       |
|  +--------------------+                     +--------------------+       |
+--------------------------------------------------------------------------+
```
- **Macro-Layer (MPI):** Divides population into semi-isolated islands. Periodically exchanges elite chromosomes via ring topology (`MPI_Sendrecv`). Preserves genetic diversity and prevents premature convergence.
- **Micro-Layer (OpenMP):** Spawns worker threads across multi-core sockets (`#pragma omp parallel for schedule(dynamic)`) to parallelize chromosome decoding, fitness calculation, and 2-opt local search.

---

## Slide 7: Algorithmic Workflow
1. **Chromosome Representation:** Permutation of bin IDs $[b_1, b_2, \dots, b_n]$ decoded via capacity-constrained greedy splitting into individual vehicle tours.
2. **Genetic Selection & Crossover:** Tournament selection ($k=4$) followed by Order Crossover (OX) preserving precedence relations.
3. **Mutation:** Inversion / swap mutation with probability 0.15.
4. **Memetic Education:** 2-Opt local search untangling crossing paths on elite candidates.
5. **Periodic Island Migration:** Every 15–20 generations, top 2–3 elite individuals are migrated along the MPI ring.

---

## Slide 8: Experimental Setup & Hardware Environment
- **Processors:** Multi-core x86-64 Architecture (up to 8 logical cores evaluated).
- **Compilers & Toolchain:** GCC / MinGW with `-O3`, `-fopenmp`, Microsoft MPI 10.1 SDK, CMake 3.14+, Python 3.14.
- **Evaluated Datasets:**
  - `CVRPLIB A-n32-k5` (31 bins, 5 vehicles)
  - `MunicipalWaste_N100` (100 bins, 8 vehicles, capacity 160)
  - `MunicipalWaste_N250` (250 bins, 15 vehicles, capacity 220)
  - `MunicipalWaste_N500` (500 bins, 25 vehicles, capacity 280)
- **Paradigms & Configurations:**
  - Sequential (1 thread)
  - OpenMP (2, 4, 8 threads)
  - MPI (2, 4 processes)
  - Hybrid MPI-OpenMP (2P x 2T = 4 cores, 2P x 4T = 8 cores, 4P x 2T = 8 cores)

---

## Slide 9: Experimental Results & Speedup Analysis
- **Execution Speedup:**
  - On $N=100$, OpenMP (4T) achieves ~3.2x speedup; MPI (4P) achieves ~3.5x speedup; Hybrid (2P x 4T) achieves ~5.6x speedup.
  - On large instances ($N=500$), Hybrid (4P x 2T / 2P x 4T) achieves up to **6.8x speedup** on 8 cores, outperforming pure OpenMP due to reduced memory bus contention and lower lock synchronization.
- **Parallel Efficiency:**
  - OpenMP efficiency declines beyond 4 threads due to memory bandwidth saturation during cache-heavy 2-opt operations.
  - Hybrid parallelism sustains high parallel efficiency (>75–85%) across all 8 cores because each MPI process operates in its own memory space with isolated cache lines.

---

## Slide 10: Solution Quality & Convergence Superiority
- **Escaping Local Optima:**
  - Sequential and pure OpenMP algorithms tend to converge early to local minima.
  - The MPI and Hybrid Island Models maintain multiple diverse evolutionary trajectories across islands.
  - Migrating elite individuals across the ring injects genetic innovations, yielding a **4.5% to 8.2% lower total objective cost (better routes)** than the sequential baseline.
- **Priority Compliance:** Critical Priority 3 bins (hospitals) are serviced within the first 25% of vehicle route durations, completely eliminating overflow penalties.

---

## Slide 11: Demonstration Script & Code Walkthrough

### Part A: Nandini Sah — Algorithm Formulation & OpenMP
1. Explain `Types.hpp` and `FitnessEvaluator.hpp`: Demonstrate how greedy route splitting respects vehicle capacity and how the priority penalty $w(p_i) \cdot f_i \cdot T_{ik}$ drives scheduling order.
2. Demonstrate OpenMP code: Highlight `#pragma omp parallel for schedule(dynamic)` in `OpenMPGA.hpp` and explain why per-thread RNGs avoid false sharing.
3. Run `test_correctness` showing all unit tests passing.

### Part B: Prisha Gupta — Distributed MPI & Hybrid Optimization
1. Explain `MPIGA.hpp` and `HybridGA.hpp`: Demonstrate the `migrate_ring()` routine using `MPI_Sendrecv` to shift elites between ranks.
2. Demonstrate Hybrid solver execution with MPI ranks and OpenMP threads.
3. Review `benchmark_summary.csv` and show generated vector plots:
   - `fig1_speedup_scaling.png` (Speedup curves)
   - `fig3_convergence_comparison.png` (Island model convergence advantage)
   - `fig4_route_visualization.png` (Optimized waste collection tours map)

---

## Slide 12: Conclusion & Future Scope
- **Conclusions:**
  - Successfully designed, implemented, and benchmarked an end-to-end priority-aware waste collection route optimization system.
  - Demonstrated that the Hybrid MPI-OpenMP model achieves the highest speedup (up to 6.8x) and best route quality across city-scale benchmarks.
  - Completed all project guidelines, unit tests, reproducible Docker scripts, and dual-format conference papers (IEEE and Springer Nature).
- **Future Scope:**
  - GPU acceleration using CUDA for parallel distance matrix evaluations and deep reinforcement learning route initialization.
  - Dynamic intra-day traffic congestion integration via real-time GPS telemetry and OpenStreetMap data.
