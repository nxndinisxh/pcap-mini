cmake_minimum_required(VERSION 3.14)
project(HybridWasteRoutingOptimization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Optimization flags
if(MSVC)
    add_compile_options(/O2 /W3)
else()
    add_compile_options(-O3 -Wall -Wextra)
endif()

# Find OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found: ${OpenMP_CXX_VERSION}")
else()
    message(WARNING "OpenMP not found, multi-threading will be disabled")
endif()

# Find MPI
find_package(MPI)
if(MPI_CXX_FOUND)
    message(STATUS "MPI found: ${MPI_CXX_VERSION}")
else()
    message(WARNING "MPI not found, distributed targets will be disabled")
endif()

include_directories(
    src_cpp/common
    ${MPI_CXX_INCLUDE_PATH}
)

# 1. Sequential Target
add_executable(solver_sequential src_cpp/sequential/main.cpp)

# 2. OpenMP Target
if(OpenMP_CXX_FOUND)
    add_executable(solver_openmp src_cpp/openmp/main.cpp)
    target_link_libraries(solver_openmp PRIVATE OpenMP::OpenMP_CXX)
endif()

# 3. MPI Target
if(MPI_CXX_FOUND)
    add_executable(solver_mpi src_cpp/mpi/main.cpp)
    target_link_libraries(solver_mpi PRIVATE MPI::MPI_CXX)
endif()

# 4. Hybrid MPI-OpenMP Target
if(MPI_CXX_FOUND AND OpenMP_CXX_FOUND)
    add_executable(solver_hybrid src_cpp/hybrid/main.cpp)
    target_link_libraries(solver_hybrid PRIVATE MPI::MPI_CXX OpenMP::OpenMP_CXX)
endif()

# 5. Unit Tests
add_executable(test_correctness src_cpp/tests/test_correctness.cpp)
enable_testing()
add_test(NAME CorrectnessTests COMMAND test_correctness)
