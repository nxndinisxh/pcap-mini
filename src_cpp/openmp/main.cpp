#include "OpenMPGA.hpp"
#include "../common/InstanceLoader.hpp"
#include <iostream>
#include <chrono>

int main(int argc, char** argv) {
    std::string dataset_path = (argc > 1) ? argv[1] : "data/synthetic/waste_n100.vrp";
    int threads = (argc > 2) ? std::stoi(argv[2]) : 4;
    int pop_size = (argc > 3) ? std::stoi(argv[3]) : 100;
    int gens = (argc > 4) ? std::stoi(argv[4]) : 150;

    std::cout << "[OpenMP] Loading instance: " << dataset_path << std::endl;
    pcap::ProblemInstance instance = pcap::InstanceLoader::load_from_vrp(dataset_path);
    std::cout << "[OpenMP] Threads: " << threads 
              << ", Bins: " << instance.num_bins 
              << ", Vehicles: " << instance.num_vehicles << std::endl;

    pcap::GAParameters params;
    params.num_threads = threads;
    params.population_size = pop_size;
    params.generations = gens;

    pcap::OpenMPGA solver(instance, params);

    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<double> history;
    pcap::Solution best = solver.solve(history);
    auto t1 = std::chrono::high_resolution_clock::now();

    double elapsed_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::cout << "[OpenMP] Finished in " << elapsed_ms << " ms" << std::endl;
    std::cout << "[OpenMP] Best Fitness: " << best.fitness 
              << " (Distance: " << best.total_distance 
              << ", Routes: " << best.routes.size() << ")" << std::endl;

    return 0;
}
