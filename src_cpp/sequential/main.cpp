#include "SequentialGA.hpp"
#include "../common/InstanceLoader.hpp"
#include <iostream>
#include <chrono>

int main(int argc, char** argv) {
    std::string dataset_path = (argc > 1) ? argv[1] : "data/synthetic/waste_n100.vrp";
    int pop_size = (argc > 2) ? std::stoi(argv[2]) : 100;
    int gens = (argc > 3) ? std::stoi(argv[3]) : 150;

    std::cout << "[Sequential] Loading instance: " << dataset_path << std::endl;
    pcap::ProblemInstance instance = pcap::InstanceLoader::load_from_vrp(dataset_path);
    std::cout << "[Sequential] Bins: " << instance.num_bins 
              << ", Vehicles: " << instance.num_vehicles 
              << ", Capacity: " << instance.vehicle_capacity << std::endl;

    pcap::GAParameters params;
    params.population_size = pop_size;
    params.generations = gens;

    pcap::SequentialGA solver(instance, params, 42);

    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<double> history;
    pcap::Solution best = solver.solve(history);
    auto t1 = std::chrono::high_resolution_clock::now();

    double elapsed_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::cout << "[Sequential] Finished in " << elapsed_ms << " ms" << std::endl;
    std::cout << "[Sequential] Best Fitness: " << best.fitness 
              << " (Distance: " << best.total_distance 
              << ", Priority Penalty: " << best.priority_penalty 
              << ", Routes: " << best.routes.size() << ")" << std::endl;

    return 0;
}
