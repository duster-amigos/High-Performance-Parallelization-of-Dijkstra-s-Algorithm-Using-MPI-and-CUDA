#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <chrono>
#include <string>
#include <getopt.h>

void print_usage() {
    if (MPI::COMM_WORLD.Get_rank() == 0)
        std::cout << "Usage: dijkstra_mpi -i <adj_matrix> -s <source> -n <nodes>\n";
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    int n = 0, source = 0;
    std::string input_file;
    int opt;
    while ((opt = getopt(argc, argv, "i:s:n:")) != -1) {
        switch (opt) {
            case 'i': input_file = optarg; break;
            case 's': source = std::stoi(optarg); break;
            case 'n': n = std::stoi(optarg); break;
            default: print_usage(); MPI_Finalize(); return 1;
        }
    }
    if (n <= 0 || input_file.empty() || source < 0 || source >= n) {
        print_usage(); MPI_Finalize(); return 1;
    }
    try {
        int padded_n = n;
        if (procs > n) padded_n = procs;
        else if (n % procs != 0) padded_n = n + (procs - n % procs);
        std::vector<std::vector<int>> adj(padded_n, std::vector<int>(padded_n, std::numeric_limits<int>::max()));
        for (int i = 0; i < n; ++i) adj[i][i] = 0;
        if (rank == 0) {
            std::ifstream ifs(input_file);
            if (!ifs) throw std::runtime_error("Cannot open input file");
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j) {
                    int val; ifs >> val;
                    adj[i][j] = (val == -1 ? std::numeric_limits<int>::max() : val);
                }
            ifs.close();
        }
        for (int i = n; i < padded_n; ++i)
            for (int j = 0; j < padded_n; ++j)
                adj[i][j] = std::numeric_limits<int>::max();
        for (int i = 0; i < padded_n; ++i)
            for (int j = n; j < padded_n; ++j)
                adj[i][j] = std::numeric_limits<int>::max();
        MPI_Bcast(&adj[0][0], padded_n * padded_n, MPI_INT, 0, MPI_COMM_WORLD);
        std::vector<int> dist(padded_n, std::numeric_limits<int>::max());
        std::vector<int> pred(padded_n, -1);
        std::vector<bool> visited(padded_n, false);
        if (rank == 0) dist[source] = 0;
        MPI_Bcast(&dist[0], padded_n, MPI_INT, 0, MPI_COMM_WORLD);
        auto start = std::chrono::high_resolution_clock::now();
        int cols_per_proc = padded_n / procs;
        int col_start = rank * cols_per_proc;
        int col_end = col_start + cols_per_proc;
        for (int i = 0; i < n; ++i) {
            int u = -1, min_dist = std::numeric_limits<int>::max();
            for (int j = col_start; j < col_end; ++j)
                if (!visited[j] && dist[j] < min_dist) { u = j; min_dist = dist[j]; }
            struct { int value; int index; } in, out;
            in.value = min_dist; in.index = u;
            MPI_Allreduce(&in, &out, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);
            u = out.index;
            if (u == -1) break;
            visited[u] = true;
            for (int v = col_start; v < col_end; ++v) {
                if (!visited[v] && adj[u][v] != std::numeric_limits<int>::max() && dist[u] + adj[u][v] < dist[v]) {
                    dist[v] = dist[u] + adj[u][v];
                    pred[v] = u;
                }
            }
            MPI_Allreduce(MPI_IN_PLACE, &dist[0], padded_n, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
            MPI_Allreduce(MPI_IN_PLACE, &pred[0], padded_n, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
            MPI_Allreduce(MPI_IN_PLACE, &visited[0], padded_n, MPI_C_BOOL, MPI_LOR, MPI_COMM_WORLD);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        if (rank == 0) {
            std::cout << "Dijkstra (MPI) completed in " << elapsed.count() << " seconds.\n";
            std::cout << "Distances from source " << source << ": ";
            for (int i = 0; i < n; ++i) std::cout << (dist[i] == std::numeric_limits<int>::max() ? -1 : dist[i]) << " ";
            std::cout << "\nPredecessors: ";
            for (int i = 0; i < n; ++i) std::cout << pred[i] << " ";
            std::cout << std::endl;
        }
    } catch (const std::exception& ex) {
        if (rank == 0) std::cerr << "Error: " << ex.what() << std::endl;
        MPI_Finalize();
        return 1;
    }
    MPI_Finalize();
    return 0;
} 