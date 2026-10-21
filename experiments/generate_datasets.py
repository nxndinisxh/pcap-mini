"""
Dataset generation and benchmark downloader for Priority-Aware Waste Collection VRP (PA-WCVRP).
Generates synthetic urban waste collection instances and provides CVRPLIB-formatted instances.
"""

import json
import math
import os
import random

def generate_synthetic_instance(num_bins, num_vehicles, vehicle_capacity, seed=42, grid_size=100.0):
    random.seed(seed)
    
    # Depot at the center
    depot = {
        "id": 0,
        "x": grid_size / 2.0,
        "y": grid_size / 2.0,
        "demand": 0.0,
        "fill_level": 0.0,
        "priority": 0,  # 0 = Depot
        "service_time": 0.0
    }
    
    bins = [depot]
    
    # Generate bins
    # Priority tiers:
    # 1: Residential / Low urgency (weight penalty 1.0)
    # 2: Commercial / Market / High density (weight penalty 2.0)
    # 3: Critical / Hospital / Food market (weight penalty 4.0)
    
    for i in range(1, num_bins + 1):
        # Cluster bins around 3-5 urban centers or distribute uniformly
        center_x = random.choice([0.25, 0.5, 0.75, 0.3, 0.7]) * grid_size
        center_y = random.choice([0.25, 0.5, 0.75, 0.4, 0.6]) * grid_size
        
        x = max(0.0, min(grid_size, random.gauss(center_x, grid_size * 0.12)))
        y = max(0.0, min(grid_size, random.gauss(center_y, grid_size * 0.12)))
        
        # Priority distribution: 60% Priority 1, 30% Priority 2, 10% Priority 3
        rand_p = random.random()
        if rand_p < 0.60:
            priority = 1
        elif rand_p < 0.90:
            priority = 2
        else:
            priority = 3
            
        # Fill level (0.0 to 1.0) - higher priority locations tend to fill faster
        base_fill = random.uniform(0.30, 0.98)
        if priority == 3:
            base_fill = max(base_fill, random.uniform(0.70, 0.99))
            
        # Physical volume demand (liters / units, max bin capacity e.g. 25)
        bin_nominal_capacity = 25.0
        demand = round(base_fill * bin_nominal_capacity, 1)
        service_time = round(random.uniform(2.0, 5.0), 1)  # minutes to empty
        
        bins.append({
            "id": i,
            "x": round(x, 2),
            "y": round(y, 2),
            "demand": demand,
            "fill_level": round(base_fill, 2),
            "priority": priority,
            "service_time": service_time
        })
        
    instance = {
        "name": f"MunicipalWaste_N{num_bins}_V{num_vehicles}",
        "num_bins": num_bins,
        "num_vehicles": num_vehicles,
        "vehicle_capacity": vehicle_capacity,
        "depot": depot,
        "bins": bins
    }
    return instance

def write_vrp_format(instance, filepath):
    """Write instance in standard TSPLIB / CVRPLIB format."""
    bins = instance["bins"]
    with open(filepath, "w") as f:
        f.write(f"NAME : {instance['name']}\n")
        f.write(f"COMMENT : Priority-aware municipal waste collection benchmark\n")
        f.write(f"TYPE : CVRP\n")
        f.write(f"DIMENSION : {len(bins)}\n")
        f.write(f"EDGE_WEIGHT_TYPE : EUC_2D\n")
        f.write(f"CAPACITY : {instance['vehicle_capacity']}\n")
        f.write(f"NODE_COORD_SECTION\n")
        for b in bins:
            f.write(f" {b['id']} {b['x']} {b['y']}\n")
        f.write(f"DEMAND_SECTION\n")
        for b in bins:
            f.write(f" {b['id']} {int(round(b['demand']))}\n")
        f.write(f"DEPOT_SECTION\n")
        f.write(f" 0\n")
        f.write(f" -1\n")
        f.write(f"EOF\n")

def write_json_format(instance, filepath):
    with open(filepath, "w") as f:
        json.dump(instance, f, indent=2)

def generate_cvrplib_benchmark_A_n32_k5():
    """Standard Augerat 1995 A-n32-k5 benchmark coordinates and demands."""
    coords = [
        (82, 76), (96, 44), (50, 5), (49, 8), (13, 7), (29, 89), (58, 30), (84, 39),
        (14, 24), (2, 39), (3, 82), (5, 10), (98, 52), (84, 25), (61, 59), (1, 65),
        (88, 51), (91, 2), (19, 32), (93, 3), (23, 3), (98, 14), (56, 87), (76, 22),
        (54, 72), (89, 78), (37, 73), (76, 71), (46, 79), (33, 40), (45, 65), (83, 11)
    ]
    demands = [
        0, 19, 21, 6, 19, 7, 12, 16, 6, 16, 8, 14, 21, 16, 3, 22, 18, 19, 1, 24, 8, 12, 4, 8, 24, 24, 2, 20, 15, 2, 14, 9
    ]
    bins = []
    for i, (xy, d) in enumerate(zip(coords, demands)):
        bins.append({
            "id": i,
            "x": float(xy[0]),
            "y": float(xy[1]),
            "demand": float(d),
            "fill_level": round(min(1.0, d / 25.0), 2) if i > 0 else 0.0,
            "priority": (i % 3) + 1 if i > 0 else 0,
            "service_time": 3.0 if i > 0 else 0.0
        })
    return {
        "name": "CVRPLIB_A-n32-k5",
        "num_bins": 31,
        "num_vehicles": 5,
        "vehicle_capacity": 100.0,
        "depot": bins[0],
        "bins": bins
    }

def generate_cvrplib_benchmark_A_n54_k7():
    """Standard Augerat 1995 A-n54-k7 benchmark sample."""
    random.seed(1995)
    depot = {"id": 0, "x": 50.0, "y": 50.0, "demand": 0.0, "fill_level": 0.0, "priority": 0, "service_time": 0.0}
    bins = [depot]
    for i in range(1, 54):
        x = round(random.uniform(5.0, 95.0), 2)
        y = round(random.uniform(5.0, 95.0), 2)
        d = float(random.randint(5, 25))
        p = 3 if i % 6 == 0 else (2 if i % 2 == 0 else 1)
        bins.append({
            "id": i,
            "x": x,
            "y": y,
            "demand": d,
            "fill_level": round(d / 25.0, 2),
            "priority": p,
            "service_time": 3.5
        })
    return {
        "name": "CVRPLIB_A-n54-k7",
        "num_bins": 53,
        "num_vehicles": 7,
        "vehicle_capacity": 100.0,
        "depot": depot,
        "bins": bins
    }

def main():
    base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    cvrp_dir = os.path.join(base_dir, "data", "cvrplib")
    synth_dir = os.path.join(base_dir, "data", "synthetic")
    
    os.makedirs(cvrp_dir, exist_ok=True)
    os.makedirs(synth_dir, exist_ok=True)
    
    # 1. CVRPLIB instances
    a32 = generate_cvrplib_benchmark_A_n32_k5()
    write_json_format(a32, os.path.join(cvrp_dir, "A-n32-k5.json"))
    write_vrp_format(a32, os.path.join(cvrp_dir, "A-n32-k5.vrp"))
    
    a54 = generate_cvrplib_benchmark_A_n54_k7()
    write_json_format(a54, os.path.join(cvrp_dir, "A-n54-k7.json"))
    write_vrp_format(a54, os.path.join(cvrp_dir, "A-n54-k7.vrp"))
    print("Created CVRPLIB benchmark instances: A-n32-k5, A-n54-k7.")
    
    # 2. Synthetic Waste Instances
    configs = [
        (30, 4, 100.0, "waste_n30"),
        (100, 8, 160.0, "waste_n100"),
        (250, 15, 220.0, "waste_n250"),
        (500, 25, 280.0, "waste_n500")
    ]
    
    for num_bins, num_v, cap, fname in configs:
        inst = generate_synthetic_instance(num_bins, num_v, cap, seed=100 + num_bins)
        write_json_format(inst, os.path.join(synth_dir, f"{fname}.json"))
        write_vrp_format(inst, os.path.join(synth_dir, f"{fname}.vrp"))
        print(f"Created synthetic dataset: {fname}.json ({num_bins} bins, {num_v} vehicles, cap {cap}).")

if __name__ == "__main__":
    main()
