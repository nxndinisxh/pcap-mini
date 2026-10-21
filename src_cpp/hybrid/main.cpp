#include "HybridGA.hpp"
#include "../common/InstanceLoader.hpp"
#include <mpi.h>
#include <iostream>
#include <chrono>

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::string dataset_path = (argc > 1) ? argv[1] : "data/synthetic/waste_n100.vrp";
    int threads_per_proc = (argc > 2) ? std::stoi(argv[2]) : 4;
    int pop_size = (argc > 3) ? std::stoi(argv[3]) : 100;
    int gens = (argc > 4) ? std::stoi(argv[4]) : 150;

    if (rank == 0) {
        std::cout << "[Hybrid MPI-OpenMP] MPI Ranks: " << size 
                  << ", Threads/Rank: " << threads_per_proc 
                  << " (Total Cores: " << size * threads_per_proc << ")" << std::endl;
        std::cout << "[Hybrid MPI-OpenMP] Loading dataset: " << dataset_path << std::endl;
    }

    pcap::ProblemInstance instance = pcap::InstanceLoader::load_from_vrp(dataset_path);

    pcap::GAParameters params;
    params.num_threads = threads_per_proc;
    params.num_processes = size;
    params.population_size = pop_size;
    params.generations = gens;

    pcap::HybridGA solver(instance, params, rank, size);

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    std::vector<double> history;
    pcap::Solution best = solver.solve(history);

    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    double elapsed_ms = (t1 - t0) * 1000.0;

    if (rank == 0) {
        std::cout << "[Hybrid MPI-OpenMP] Completed in " << elapsed_ms << " ms" << std::endl;
        std::cout << "[Hybrid MPI-OpenMP] Global Best Fitness: " << best.fitness 
                  << " (Distance: " << best.total_distance 
                  << ", Priority Penalty: " << best.priority_penalty 
                  << ", Overflow Penalty: " << best.overflow_penalty 
                  << ", Routes: " << best.routes.size() << ")" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
