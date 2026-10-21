#include "../common/Types.hpp"
#include "../common/FitnessEvaluator.hpp"
#include "../common/LocalSearch.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

void test_distance_symmetry() {
    pcap::ProblemInstance inst;
    inst.bins.push_back({0, 0.0, 0.0, 0.0, 0.0, 0, 0.0});
    inst.bins.push_back({1, 10.0, 0.0, 5.0, 0.5, 1, 3.0});
    inst.bins.push_back({2, 0.0, 20.0, 8.0, 0.8, 2, 4.0});
    inst.num_bins = 2;
    inst.vehicle_capacity = 50.0;
    inst.num_vehicles = 2;
    inst.compute_distance_matrix();

    assert(std::abs(inst.dist_matrix[0][1] - 10.0) < 1e-6);
    assert(std::abs(inst.dist_matrix[1][0] - 10.0) < 1e-6);
    assert(std::abs(inst.dist_matrix[0][2] - 20.0) < 1e-6);
    assert(std::abs(inst.dist_matrix[1][2] - std::sqrt(100.0 + 400.0)) < 1e-6);
    std::cout << "[PASS] test_distance_symmetry" << std::endl;
}

void test_route_split_and_capacity() {
    pcap::ProblemInstance inst;
    inst.bins.push_back({0, 0.0, 0.0, 0.0, 0.0, 0, 0.0});
    inst.bins.push_back({1, 5.0, 0.0, 20.0, 0.8, 1, 2.0});
    inst.bins.push_back({2, 10.0, 0.0, 20.0, 0.8, 2, 2.0});
    inst.bins.push_back({3, 15.0, 0.0, 20.0, 0.8, 3, 2.0});
    inst.num_bins = 3;
    inst.vehicle_capacity = 30.0; // Can only hold 1 bin of demand 20 per vehicle or at most 1 before 20+20 > 30
    inst.num_vehicles = 3;
    inst.compute_distance_matrix();

    pcap::Solution sol;
    sol.chromosome = {1, 2, 3};
    pcap::FitnessEvaluator::decode_and_evaluate(inst, sol);

    // Should create 3 separate routes because 20+20 > 30
    assert(sol.routes.size() == 3);
    for (const auto& r : sol.routes) {
        assert(r.total_load <= inst.vehicle_capacity);
    }
    assert(sol.capacity_penalty == 0.0);
    std::cout << "[PASS] test_route_split_and_capacity" << std::endl;
}

void test_2opt_improvement() {
    pcap::ProblemInstance inst;
    inst.bins.push_back({0, 0.0, 0.0, 0.0, 0.0, 0, 0.0});
    inst.bins.push_back({1, 0.0, 10.0, 2.0, 0.2, 1, 1.0});
    inst.bins.push_back({2, 10.0, 10.0, 2.0, 0.2, 1, 1.0});
    inst.bins.push_back({3, 0.0, 20.0, 2.0, 0.2, 1, 1.0});
    inst.bins.push_back({4, 10.0, 20.0, 2.0, 0.2, 1, 1.0});
    inst.num_bins = 4;
    inst.vehicle_capacity = 100.0;
    inst.num_vehicles = 1;
    inst.compute_distance_matrix();

    // Deliberately crossed path: 1 -> 4 -> 3 -> 2
    pcap::Solution sol;
    sol.chromosome = {1, 4, 3, 2};
    pcap::FitnessEvaluator::decode_and_evaluate(inst, sol);
    double initial_fitness = sol.fitness;

    pcap::LocalSearch::apply_2opt(inst, sol);
    assert(sol.fitness <= initial_fitness);
    std::cout << "[PASS] test_2opt_improvement (initial: " << initial_fitness 
              << ", optimized: " << sol.fitness << ")" << std::endl;
}

int main() {
    std::cout << "--- PCAP Unit & Correctness Test Suite ---" << std::endl;
    test_distance_symmetry();
    test_route_split_and_capacity();
    test_2opt_improvement();
    std::cout << "--- All Correctness Checks Passed Successfully! ---" << std::endl;
    return 0;
}
