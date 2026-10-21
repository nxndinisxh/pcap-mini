#pragma once

#include "Types.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace pcap {

class InstanceLoader {
public:
    static ProblemInstance load_from_vrp(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open VRP file: " + filepath);
        }

        ProblemInstance instance;
        std::string line;
        int dimension = 0;
        double capacity = 100.0;
        bool in_coords = false;
        bool in_demands = false;

        while (std::getline(file, line)) {
            if (line.find("NAME") != std::string::npos) {
                auto pos = line.find(":");
                if (pos != std::string::npos) instance.name = line.substr(pos + 1);
            } else if (line.find("DIMENSION") != std::string::npos) {
                auto pos = line.find(":");
                if (pos != std::string::npos) dimension = std::stoi(line.substr(pos + 1));
            } else if (line.find("CAPACITY") != std::string::npos) {
                auto pos = line.find(":");
                if (pos != std::string::npos) capacity = std::stod(line.substr(pos + 1));
            } else if (line.find("NODE_COORD_SECTION") != std::string::npos) {
                in_coords = true;
                in_demands = false;
                instance.bins.resize(dimension);
                continue;
            } else if (line.find("DEMAND_SECTION") != std::string::npos) {
                in_coords = false;
                in_demands = true;
                continue;
            } else if (line.find("DEPOT_SECTION") != std::string::npos || line.find("EOF") != std::string::npos) {
                in_coords = false;
                in_demands = false;
                continue;
            }

            if (in_coords) {
                std::istringstream iss(line);
                int id;
                double x, y;
                if (iss >> id >> x >> y) {
                    if (id < dimension) {
                        instance.bins[id].id = id;
                        instance.bins[id].x = x;
                        instance.bins[id].y = y;
                        instance.bins[id].service_time = (id == 0) ? 0.0 : 3.0;
                    }
                }
            } else if (in_demands) {
                std::istringstream iss(line);
                int id;
                double demand;
                if (iss >> id >> demand) {
                    if (id < dimension) {
                        instance.bins[id].demand = demand;
                        instance.bins[id].fill_level = std::min(1.0, demand / 25.0);
                        instance.bins[id].priority = (id == 0) ? 0 : ((id % 3) + 1);
                    }
                }
            }
        }

        instance.num_bins = dimension - 1;
        instance.vehicle_capacity = capacity;
        instance.num_vehicles = std::max(2, (int)std::ceil(dimension / 5.0));
        if (!instance.bins.empty()) {
            instance.depot = instance.bins[0];
        }
        instance.compute_distance_matrix();
        return instance;
    }
};

} // namespace pcap
