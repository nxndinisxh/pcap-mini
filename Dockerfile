# Multi-stage Dockerfile for Hybrid MPI-OpenMP Waste Routing Optimization
FROM ubuntu:22.04 AS build-env

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    libopenmpi-dev \
    openmpi-bin \
    libomp-dev \
    python3 \
    python3-pip \
    git \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app

# Build C++ executables
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . -j$(nproc)

# Run tests
RUN cd build && ctest --output-on-failure

# Production runtime stage
FROM ubuntu:22.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    openmpi-bin \
    libomp5 \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=build-env /app/build/solver_* /app/bin/
COPY --from=build-env /app/build/test_correctness /app/bin/
COPY --from=build-env /app/data /app/data/
COPY --from=build-env /app/experiments /app/experiments/

ENV PATH="/app/bin:${PATH}"

CMD ["/bin/bash"]
