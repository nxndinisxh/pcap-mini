#pragma once

#include "Types.hpp"
#include <algorithm>
#include <limits>

namespace pcap {

class FitnessEvaluator {
public:
    static constexpr double PENALTY_CAPACITY = 200.0;
    static constexpr double PENALTY_OVERFLOW = 150.0;
    static constexpr double PRIORITY_WEIGHT_TIER1 = 1.0;
    static constexpr double PRIORITY_WEIGHT_TIER2 = 2.5;
    static constexpr double PRIORITY_WEIGHT_TIER3 = 5.0;
    static constexpr double ALPHA_PRIORITY = 0.40; // Scale factor for priority arrival time

    static void decode_and_evaluate(const ProblemInstance& instance, Solution& sol) {
        sol.routes.clear();
        sol.total_distance = 0.0;
        sol.priority_penalty = 0.0;
        sol.capacity_penalty = 0.0;
        sol.overflow_penalty = 0.0;

        if (sol.chromosome.empty()) {
            sol.fitness = std::numeric_limits<double>::infinity();
            return;
        }

        // Greedy split chromosome into vehicle routes respecting vehicle capacity
        Route current_route;
        current_route.total_distance = 0.0;
        current_route.total_load = 0.0;
        current_route.priority_penalty = 0.0;
        current_route.service_time = 0.0;

        int prev_node = 0; // Depot
        double current_time = 0.0;

        for (int bin_id : sol.chromosome) {
            const auto& bin = instance.bins[bin_id];
            double travel_dist = instance.dist_matrix[prev_node][bin_id];
            double travel_time = travel_dist; // Assume unit velocity

            // Check if adding this bin exceeds capacity or if we need a new route
            if (!current_route.bin_ids.empty() && 
                (current_route.total_load + bin.demand > instance.vehicle_capacity)) {
                // Close current route: return to depot
                double return_dist = instance.dist_matrix[prev_node][0];
                current_route.total_distance += return_dist;
                sol.routes.push_back(current_route);

                // Start new route
                current_route.bin_ids.clear();
                current_route.total_distance = 0.0;
                current_route.total_load = 0.0;
                current_route.priority_penalty = 0.0;
                current_route.service_time = 0.0;
                prev_node = 0;
                current_time = 0.0;

                travel_dist = instance.dist_matrix[0][bin_id];
                travel_time = travel_dist;
            }

            // Add bin to route
            current_route.bin_ids.push_back(bin_id);
            current_route.total_load += bin.demand;
            current_route.total_distance += travel_dist;
            current_time += travel_time;

            // Compute priority-weighted penalty
            double p_weight = PRIORITY_WEIGHT_TIER1;
            if (bin.priority == 2) p_weight = PRIORITY_WEIGHT_TIER2;
            else if (bin.priority == 3) p_weight = PRIORITY_WEIGHT_TIER3;

            current_route.priority_penalty += p_weight * bin.fill_level * current_time;

            // Overflow penalty if fill_level exceeds critical 85% threshold
            if (bin.fill_level > 0.85) {
                sol.overflow_penalty += PENALTY_OVERFLOW * (bin.fill_level - 0.85) * (bin.priority * bin.priority);
            }

            current_time += bin.service_time;
            current_route.service_time += bin.service_time;
            prev_node = bin_id;
        }

        // Close the last route
        if (!current_route.bin_ids.empty()) {
            current_route.total_distance += instance.dist_matrix[prev_node][0];
            sol.routes.push_back(current_route);
        }

        // Aggregate route statistics
        for (const auto& r : sol.routes) {
            sol.total_distance += r.total_distance;
            sol.priority_penalty += r.priority_penalty;
            if (r.total_load > instance.vehicle_capacity) {
                sol.capacity_penalty += (r.total_load - instance.vehicle_capacity) * PENALTY_CAPACITY;
            }
        }

        // Penalty for exceeding available vehicle fleet
        if (static_cast<int>(sol.routes.size()) > instance.num_vehicles) {
            int extra_vehicles = sol.routes.size() - instance.num_vehicles;
            sol.capacity_penalty += extra_vehicles * 1000.0;
        }

        sol.fitness = sol.total_distance + 
                      (ALPHA_PRIORITY * sol.priority_penalty) + 
                      sol.capacity_penalty + 
                      sol.overflow_penalty;
    }
};

} // namespace pcap
