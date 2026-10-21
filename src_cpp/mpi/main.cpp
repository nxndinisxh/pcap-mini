#include "MPIGA.hpp"
#include "../common/InstanceLoader.hpp"
#include <mpi.h>
#include <iostream>
#include <chrono>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::string dataset_path = (argc > 1) ? argv[1] : "data/synthetic/waste_n100.vrp";
    int pop_size = (argc > 2) ? std::stoi(argv[2]) : 100;
    int gens = (argc > 3) ? std::stoi(argv[3]) : 150;

    if (rank == 0) {
        std::cout << "[MPI] Ranks: " << size << ", Loading instance: " << dataset_path << std::endl;
    }

    pcap::ProblemInstance instance = pcap::InstanceLoader::load_from_vrp(dataset_path);

    pcap::GAParameters params;
    params.population_size = pop_size;
    params.generations = gens;
    params.num_processes = size;

    pcap::MPIGA solver(instance, params, rank, size);

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    std::vector<double> history;
    pcap::Solution best = solver.solve(history);

    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    double elapsed_ms = (t1 - t0) * 1000.0;

    if (rank == 0) {
        std::cout << "[MPI] Finished in " << elapsed_ms << " ms" << std::endl;
        std::cout << "[MPI] Global Best Fitness: " << best.fitness 
                  << " (Distance: " << best.total_distance 
                  << ", Priority Penalty: " << best.priority_penalty 
                  << ", Routes: " << best.routes.size() << ")" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
