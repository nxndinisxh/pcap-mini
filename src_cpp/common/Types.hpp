#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <iostream>

namespace pcap {

struct Bin {
    int id;
    double x;
    double y;
    double demand;       // Waste amount (liters / kg)
    double fill_level;   // 0.0 to 1.0
    int priority;        // 1: Normal residential, 2: Commercial, 3: Hazardous/Hospital
    double service_time; // Time required to empty (minutes)
};

struct Vehicle {
    int id;
    double capacity;
    double max_travel_time;
};

struct ProblemInstance {
    std::string name;
    int num_bins;
    int num_vehicles;
    double vehicle_capacity;
    Bin depot;
    std::vector<Bin> bins; // bins[0] is depot, bins[1..n] are customer bins
    std::vector<std::vector<double>> dist_matrix;

    void compute_distance_matrix() {
        int n = bins.size();
        dist_matrix.assign(n, std::vector<double>(n, 0.0));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                double dx = bins[i].x - bins[j].x;
                double dy = bins[i].y - bins[j].y;
                dist_matrix[i][j] = std::sqrt(dx * dx + dy * dy);
            }
        }
    }
};

struct Route {
    std::vector<int> bin_ids; // Sequence of visited bin IDs (excluding start/end depot 0)
    double total_distance;
    double total_load;
    double priority_penalty;
    double service_time;
};

struct Solution {
    std::vector<int> chromosome; // Permutation of bin IDs 1..n
    std::vector<Route> routes;
    double total_distance;
    double priority_penalty;
    double capacity_penalty;
    double overflow_penalty;
    double fitness; // Overall objective value (lower is better)

    bool operator<(const Solution& other) const {
        return fitness < other.fitness;
    }
};

struct GAParameters {
    int population_size = 100;
    int generations = 150;
    double crossover_rate = 0.85;
    double mutation_rate = 0.15;
    int tournament_size = 5;
    int migration_interval = 15; // Generations between island migrations
    int migration_count = 3;     // Number of elite individuals exchanged
    int num_threads = 4;         // OpenMP threads
    int num_processes = 2;       // MPI processes
};

} // namespace pcap
