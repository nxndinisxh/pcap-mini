#pragma once

#include "../common/Types.hpp"
#include "../common/FitnessEvaluator.hpp"
#include "../common/LocalSearch.hpp"
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <chrono>

namespace pcap {

class SequentialGA {
private:
    ProblemInstance instance;
    GAParameters params;
    std::mt19937 rng;

public:
    SequentialGA(const ProblemInstance& inst, const GAParameters& p, unsigned int seed = 42)
        : instance(inst), params(p), rng(seed) {}

    Solution tournament_select(const std::vector<Solution>& population) {
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

    // Order Crossover (OX) for permutation chromosomes
    Solution order_crossover(const Solution& p1, const Solution& p2) {
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

    void mutate(Solution& sol) {
        std::uniform_real_distribution<double> r_dist(0.0, 1.0);
        if (r_dist(rng) < params.mutation_rate) {
            int n = sol.chromosome.size();
            std::uniform_int_distribution<int> idx_dist(0, n - 1);
            int i = idx_dist(rng);
            int j = idx_dist(rng);
            if (i != j) {
                // Inversion mutation
                if (i > j) std::swap(i, j);
                std::reverse(sol.chromosome.begin() + i, sol.chromosome.begin() + j + 1);
            }
        }
    }

    Solution solve(std::vector<double>& convergence_history) {
        auto start_time = std::chrono::high_resolution_clock::now();
        int n = instance.num_bins;

        // Initialize population
        std::vector<Solution> population(params.population_size);
        std::vector<int> base_perm(n);
        for (int i = 0; i < n; ++i) base_perm[i] = i + 1;

        for (int i = 0; i < params.population_size; ++i) {
            population[i].chromosome = base_perm;
            std::shuffle(population[i].chromosome.begin(), population[i].chromosome.end(), rng);
            FitnessEvaluator::decode_and_evaluate(instance, population[i]);
        }

        std::sort(population.begin(), population.end());
        Solution global_best = population[0];
        LocalSearch::apply_2opt(instance, global_best);
        population[0] = global_best;

        convergence_history.clear();
        convergence_history.push_back(global_best.fitness);

        // Generational loop
        for (int gen = 1; gen <= params.generations; ++gen) {
            std::vector<Solution> new_pop;
            new_pop.reserve(params.population_size);

            // Elitism: retain top 2
            new_pop.push_back(population[0]);
            new_pop.push_back(population[1]);

            while (new_pop.size() < static_cast<size_t>(params.population_size)) {
                Solution parent1 = tournament_select(population);
                Solution parent2 = tournament_select(population);
                Solution child = order_crossover(parent1, parent2);
                mutate(child);
                FitnessEvaluator::decode_and_evaluate(instance, child);
                new_pop.push_back(child);
            }

            population = std::move(new_pop);

            // Apply 2-opt local search to the top solutions (memetic enhancement)
            LocalSearch::apply_2opt(instance, population[0]);
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
