#pragma once

#include "../common/Types.hpp"
#include "../common/FitnessEvaluator.hpp"
#include "../common/LocalSearch.hpp"
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <chrono>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace pcap {

class OpenMPGA {
private:
    ProblemInstance instance;
    GAParameters params;

public:
    OpenMPGA(const ProblemInstance& inst, const GAParameters& p)
        : instance(inst), params(p) {}

    Solution tournament_select(const std::vector<Solution>& population, std::mt19937& rng) {
        std::uniform_int_distribution<int> dist(0, population.size() - 1);
        Solution best = population[dist(rng)];
        for (int i = 1; i < params.tournament_size; ++i) {
            const auto& candidate = population[dist(rng)];
            if (candidate.fitness < best.fitness) {
                best = candidate;
            }
        }
        return best;
    }

    Solution order_crossover(const Solution& p1, const Solution& p2, std::mt19937& rng) {
        int n = p1.chromosome.size();
        std::uniform_int_distribution<int> dist(0, n - 1);
        int pt1 = dist(rng);
        int pt2 = dist(rng);
        if (pt1 > pt2) std::swap(pt1, pt2);

        Solution child;
        child.chromosome.assign(n, -1);
        std::vector<bool> visited(instance.bins.size() + 1, false);

        for (int i = pt1; i <= pt2; ++i) {
            child.chromosome[i] = p1.chromosome[i];
            visited[p1.chromosome[i]] = true;
        }

        int curr_idx = (pt2 + 1) % n;
        for (int i = 0; i < n; ++i) {
            int p2_elem = p2.chromosome[(pt2 + 1 + i) % n];
            if (!visited[p2_elem]) {
                child.chromosome[curr_idx] = p2_elem;
                visited[p2_elem] = true;
                curr_idx = (curr_idx + 1) % n;
            }
        }
        return child;
    }

    void mutate(Solution& sol, std::mt19937& rng) {
        std::uniform_real_distribution<double> r_dist(0.0, 1.0);
        if (r_dist(rng) < params.mutation_rate) {
            int n = sol.chromosome.size();
            std::uniform_int_distribution<int> idx_dist(0, n - 1);
            int i = idx_dist(rng);
            int j = idx_dist(rng);
            if (i != j) {
                if (i > j) std::swap(i, j);
                std::reverse(sol.chromosome.begin() + i, sol.chromosome.begin() + j + 1);
            }
        }
    }

    Solution solve(std::vector<double>& convergence_history) {
        int n_threads = params.num_threads;
#ifdef _OPENMP
        omp_set_num_threads(n_threads);
#endif
        int n = instance.num_bins;

        // Initialize per-thread RNGs to prevent false sharing and race conditions
        std::vector<std::mt19937> thread_rngs(n_threads);
        for (int t = 0; t < n_threads; ++t) {
            thread_rngs[t].seed(42 + t * 997);
        }

        std::vector<Solution> population(params.population_size);
        std::vector<int> base_perm(n);
        for (int i = 0; i < n; ++i) base_perm[i] = i + 1;

        // Parallel population initialization
#pragma omp parallel for schedule(static)
        for (int i = 0; i < params.population_size; ++i) {
            int tid = 0;
#ifdef _OPENMP
            tid = omp_get_thread_num();
#endif
            population[i].chromosome = base_perm;
            std::shuffle(population[i].chromosome.begin(), population[i].chromosome.end(), thread_rngs[tid]);
            FitnessEvaluator::decode_and_evaluate(instance, population[i]);
        }

        std::sort(population.begin(), population.end());
        Solution global_best = population[0];
        LocalSearch::apply_2opt(instance, global_best);
        population[0] = global_best;

        convergence_history.clear();
        convergence_history.push_back(global_best.fitness);

        for (int gen = 1; gen <= params.generations; ++gen) {
            std::vector<Solution> offspring(params.population_size);
            
            // Retain top 2 elites
            offspring[0] = population[0];
            offspring[1] = population[1];

            // Parallel offspring generation and evaluation across OpenMP threads
#pragma omp parallel for schedule(dynamic)
            for (int i = 2; i < params.population_size; ++i) {
                int tid = 0;
#ifdef _OPENMP
                tid = omp_get_thread_num();
#endif
                Solution parent1 = tournament_select(population, thread_rngs[tid]);
                Solution parent2 = tournament_select(population, thread_rngs[tid]);
                Solution child = order_crossover(parent1, parent2, thread_rngs[tid]);
                mutate(child, thread_rngs[tid]);
                FitnessEvaluator::decode_and_evaluate(instance, child);
                offspring[i] = child;
            }

            population = std::move(offspring);

            // Parallel memetic refinement: apply 2-opt to top candidates
            int memetic_candidates = std::min(4, params.population_size);
#pragma omp parallel for schedule(static)
            for (int k = 0; k < memetic_candidates; ++k) {
                LocalSearch::apply_2opt(instance, population[k]);
            }

            std::sort(population.begin(), population.end());
            if (population[0].fitness < global_best.fitness) {
                global_best = population[0];
            }
            convergence_history.push_back(global_best.fitness);
        }

        return global_best;
    }
};

} // namespace pcap
