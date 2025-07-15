# High-Performance Parallelization of Dijkstra’s Algorithm Using MPI and CUDA

This project implements and compares three versions of Dijkstra's algorithm:
- **Serial (CPU)**
- **MPI-based Parallel (CPU Cluster)**
- **CUDA-based Parallel (GPU)**

It follows the methodology and experiments described in [this paper](https://arxiv.org/pdf/2504.03667), including graph generation, timing, and visualization of results.

## Project Structure

- `serial/` — Serial Dijkstra implementation (C++)
- `mpi/` — MPI-based parallel Dijkstra implementation (C++)
- `cuda/` — CUDA-based parallel Dijkstra implementation (C++/CUDA)
- `utils/` — Utilities for graph generation and adjacency matrix conversion (C++)
- `visualization/` — Python scripts for plotting and analysis

## Requirements

- C++17 or later (for all C++ code)
- MPI (e.g., OpenMPI) for MPI version
- CUDA Toolkit (for GPU version)
- Python 3.x (for visualization)
- Python packages: `matplotlib`, `numpy`, `pandas`

## Building the Project

Build each component using its Makefile:

```
cd utils && make        # Build graph_generator and adjacency_matrix
cd serial && make       # Build serial Dijkstra
cd mpi && make          # Build MPI Dijkstra
cd cuda && make         # Build CUDA Dijkstra
```

## Workflow: End-to-End Example

### 1. Generate a Graph (Edge List)
```
./utils/graph_generator -n <nodes> -e <edges> [-d] -o <output_edge_list>
# Example: ./utils/graph_generator -n 100 -e 4950 -o graph.edgelist
```
- `-n`: Number of nodes
- `-e`: Number of edges
- `-d`: (Optional) Directed graph
- `-o`: Output file

### 2. Convert Edge List to Adjacency Matrix
```
./utils/adjacency_matrix -i <input_edge_list> -n <nodes> [-d] -o <output_matrix>
# Example: ./utils/adjacency_matrix -i graph.edgelist -n 100 -o graph.adj
```
- `-i`: Input edge list
- `-n`: Number of nodes
- `-d`: (Optional) Directed graph
- `-o`: Output file

### 3. Run Serial Dijkstra
```
./serial/dijkstra_serial -i <adj_matrix> -s <source> -n <nodes> > results/serial_100.log
```
- `-i`: Input adjacency matrix
- `-s`: Source node (0-based)
- `-n`: Number of nodes

### 4. Run MPI Dijkstra
```
mpirun -np <num_procs> ./mpi/dijkstra_mpi -i <adj_matrix> -s <source> -n <nodes> > results/mpi_100.log
```
- `-i`: Input adjacency matrix
- `-s`: Source node (0-based)
- `-n`: Number of nodes
- `-np`: Number of MPI processes

### 5. Run CUDA Dijkstra
```
./cuda/dijkstra_cuda -i <adj_matrix> -s <source> -n <nodes> > results/cuda_100.log
```
- `-i`: Input adjacency matrix
- `-s`: Source node (0-based)
- `-n`: Number of nodes

### 6. Visualize Results
```
cd visualization
python plot_results.py
```
- Expects logs in a `../results/` directory.
- Generates plots: performance (linear/log), scaling efficiency, and tables.

## Output & Logging
- All implementations print timing, distances, and predecessors.
- Errors are clearly reported.
- Logs can be redirected to files for later analysis and plotting.

## Troubleshooting
- Ensure all required dependencies are installed (see Requirements).
- For MPI, use the correct `mpirun` command and number of processes.
- For CUDA, ensure a compatible GPU and CUDA toolkit are available.
- If you encounter file errors, check file paths and permissions.
- Visualization expects logs in the `results/` directory and parses timing lines.

## Directory Structure Example
```
High Performance Parallelization of Dijkstra’s Algorithm Using MPI and CUDA/
├── serial/
├── mpi/
├── cuda/
├── utils/
├── visualization/
├── results/ # mkdir for keeping logs
└── README.md
```

## Reference
- [High-Performance Parallelization of Dijkstra’s Algorithm Using MPI and CUDA](https://arxiv.org/pdf/2504.03667)

---