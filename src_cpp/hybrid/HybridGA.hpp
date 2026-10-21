#pragma once

#include "../common/Types.hpp"
#include "../common/FitnessEvaluator.hpp"
#include "../common/LocalSearch.hpp"
#include <mpi.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <chrono>

namespace pcap {

class HybridGA {
private:
    ProblemInstance instance;
    GAParameters params;
    int rank;
    int world_size;

public:
    HybridGA(const ProblemInstance& inst, const GAParameters& p, int mpi_rank, int mpi_size)
        : instance(inst), params(p), rank(mpi_rank), world_size(mpi_size) {}

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

    void migrate_ring(std::vector<Solution>& sub_pop, int chrom_len) {
        if (world_size <= 1) return;

        int send_dest = (rank + 1) % world_size;
        int recv_source = (rank - 1 + world_size) % world_size;

        int num_migrants = std::min(params.migration_count, (int)sub_pop.size());
        std::vector<int> send_buf(num_migrants * chrom_len);
        std::vector<int> recv_buf(num_migrants * chrom_len);

        for (int m = 0; m < num_migrants; ++m) {
            for (int k = 0; k < chrom_len; ++k) {
                send_buf[m * chrom_len + k] = sub_pop[m].chromosome[k];
            }
        }

        MPI_Status status;
        MPI_Sendrecv(send_buf.data(), send_buf.size(), MPI_INT, send_dest, 200,
                     recv_buf.data(), recv_buf.size(), MPI_INT, recv_source, 200,
                     MPI_COMM_WORLD, &status);

        int replace_offset = sub_pop.size() - num_migrants;
        for (int m = 0; m < num_migrants; ++m) {
            Solution immigrant;
            immigrant.chromosome.assign(chrom_len, 0);
            for (int k = 0; k < chrom_len; ++k) {
                immigrant.chromosome[k] = recv_buf[m * chrom_len + k];
            }
            FitnessEvaluator::decode_and_evaluate(instance, immigrant);
            sub_pop[replace_offset + m] = immigrant;
        }

        std::sort(sub_pop.begin(), sub_pop.end());
    }

    Solution solve(std::vector<double>& convergence_history) {
        int n_threads = params.num_threads;
#ifdef _OPENMP
        omp_set_num_threads(n_threads);
#endif
        int n = instance.num_bins;
        int local_pop_size = std::max(10, params.population_size / world_size);

        // Per-thread RNGs per MPI rank to ensure statistical independence
        std::vector<std::mt19937> thread_rngs(n_threads);
        for (int t = 0; t < n_threads; ++t) {
            thread_rngs[t].seed(10007 * (rank + 1) + t * 997);
        }

        std::vector<Solution> sub_pop(local_pop_size);
        std::vector<int> base_perm(n);
        for (int i = 0; i < n; ++i) base_perm[i] = i + 1;

        // OpenMP parallel sub-population initialization
#pragma omp parallel for schedule(static)
        for (int i = 0; i < local_pop_size; ++i) {
            int tid = 0;
#ifdef _OPENMP
            tid = omp_get_thread_num();
#endif
            sub_pop[i].chromosome = base_perm;
            std::shuffle(sub_pop[i].chromosome.begin(), sub_pop[i].chromosome.end(), thread_rngs[tid]);
            FitnessEvaluator::decode_and_evaluate(instance, sub_pop[i]);
        }

        std::sort(sub_pop.begin(), sub_pop.end());
        LocalSearch::apply_2opt(instance, sub_pop[0]);

        Solution local_best = sub_pop[0];
        convergence_history.clear();
        convergence_history.push_back(local_best.fitness);

        for (int gen = 1; gen <= params.generations; ++gen) {
            std::vector<Solution> next_gen(local_pop_size);
            next_gen[0] = sub_pop[0];
            next_gen[1] = sub_pop[1];

            // OpenMP parallel offspring reproduction and evaluation
#pragma omp parallel for schedule(dynamic)
            for (int i = 2; i < local_pop_size; ++i) {
                int tid = 0;
#ifdef _OPENMP
                tid = omp_get_thread_num();
#endif
                Solution p1 = tournament_select(sub_pop, thread_rngs[tid]);
                Solution p2 = tournament_select(sub_pop, thread_rngs[tid]);
                Solution child = order_crossover(p1, p2, thread_rngs[tid]);
                mutate(child, thread_rngs[tid]);
                FitnessEvaluator::decode_and_evaluate(instance, child);
                next_gen[i] = child;
            }

            sub_pop = std::move(next_gen);

            // Parallel local search on top candidates
            int memetic_candidates = std::min(2, local_pop_size);
#pragma omp parallel for schedule(static)
            for (int k = 0; k < memetic_candidates; ++k) {
                LocalSearch::apply_2opt(instance, sub_pop[k]);
            }

            std::sort(sub_pop.begin(), sub_pop.end());

            // MPI Island Migration (Master thread performs communication)
            if (gen % params.migration_interval == 0) {
                migrate_ring(sub_pop, n);
            }

            if (sub_pop[0].fitness < local_best.fitness) {
                local_best = sub_pop[0];
            }
            convergence_history.push_back(local_best.fitness);
        }

        // Global reduction across all MPI ranks
        struct {
            double fitness;
            int rank;
        } local_min, global_min;

        local_min.fitness = local_best.fitness;
        local_min.rank = rank;

        MPI_Allreduce(&local_min, &global_min, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

        std::vector<int> best_chrom(n);
        if (rank == global_min.rank) {
            best_chrom = local_best.chromosome;
        }
        MPI_Bcast(best_chrom.data(), n, MPI_INT, global_min.rank, MPI_COMM_WORLD);

        Solution global_best;
        global_best.chromosome = best_chrom;
        FitnessEvaluator::decode_and_evaluate(instance, global_best);

        return global_best;
    }
};

} // namespace pcap
