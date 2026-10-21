#pragma once

#include "Types.hpp"
#include "FitnessEvaluator.hpp"
#include <algorithm>

namespace pcap {

class LocalSearch {
public:
    // Performs intra-route 2-opt moves to untangle crossing paths
    static void apply_2opt(const ProblemInstance& instance, Solution& sol) {
        if (sol.chromosome.size() < 4) return;

        bool improved = true;
        int iterations = 0;
        const int MAX_ITER = 30;

        while (improved && iterations < MAX_ITER) {
            improved = false;
            iterations++;

            int n = sol.chromosome.size();
            for (int i = 0; i < n - 1; ++i) {
                for (int j = i + 2; j < n; ++j) {
                    // Try reversing sub-segment [i+1, j]
                    std::reverse(sol.chromosome.begin() + i + 1, sol.chromosome.begin() + j + 1);
                    
                    Solution candidate = sol;
                    FitnessEvaluator::decode_and_evaluate(instance, candidate);

                    if (candidate.fitness < sol.fitness) {
                        sol = candidate;
                        improved = true;
                        break;
                    } else {
                        // Revert move
                        std::reverse(sol.chromosome.begin() + i + 1, sol.chromosome.begin() + j + 1);
                    }
                }
                if (improved) break;
            }
        }
    }
};

} // namespace pcap
