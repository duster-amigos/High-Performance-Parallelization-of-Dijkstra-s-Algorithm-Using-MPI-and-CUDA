#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <cuda_runtime.h>
#include <chrono>
#include <string>
#include <getopt.h>

#define INF 1000000000

__global__ void dijkstra_kernel(int* adj, int* dist, int* pred, bool* updated, int n) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= n) return;
    for (int v = 0; v < n; ++v) {
        int weight = adj[tid * n + v];
        if (weight != INF && dist[tid] != INF) {
            int new_dist = dist[tid] + weight;
            int old_dist = atomicMin(&dist[v], new_dist);
            if (new_dist < old_dist) {
                pred[v] = tid;
                updated[v] = true;
            }
        }
    }
}

void print_usage() {
    std::cout << "Usage: dijkstra_cuda -i <adj_matrix> -s <source> -n <nodes>\n";
}

int main(int argc, char* argv[]) {
    int n = 0, source = 0;
    std::string input_file;
    int opt;
    while ((opt = getopt(argc, argv, "i:s:n:")) != -1) {
        switch (opt) {
            case 'i': input_file = optarg; break;
            case 's': source = std::stoi(optarg); break;
            case 'n': n = std::stoi(optarg); break;
            default: print_usage(); return 1;
        }
    }
    if (n <= 0 || input_file.empty() || source < 0 || source >= n) {
        print_usage();
        return 1;
    }
    try {
        std::vector<int> h_adj(n * n);
        std::ifstream ifs(input_file);
        if (!ifs) throw std::runtime_error("Cannot open input file");
        for (int i = 0; i < n * n; ++i) {
            int val; ifs >> val;
            h_adj[i] = (val == -1 ? INF : val);
        }
        ifs.close();
        std::vector<int> h_dist(n, INF);
        std::vector<int> h_pred(n, -1);
        std::vector<bool> h_updated(n, false);
        h_dist[source] = 0;
        int *d_adj, *d_dist, *d_pred;
        bool *d_updated;
        cudaMalloc(&d_adj, n * n * sizeof(int));
        cudaMalloc(&d_dist, n * sizeof(int));
        cudaMalloc(&d_pred, n * sizeof(int));
        cudaMalloc(&d_updated, n * sizeof(bool));
        cudaMemcpy(d_adj, h_adj.data(), n * n * sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(d_dist, h_dist.data(), n * sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(d_pred, h_pred.data(), n * sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(d_updated, h_updated.data(), n * sizeof(bool), cudaMemcpyHostToDevice);
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < n; ++i) {
            dijkstra_kernel<<<(n+255)/256, 256>>>(d_adj, d_dist, d_pred, d_updated, n);
            cudaDeviceSynchronize();
        }
        cudaMemcpy(h_dist.data(), d_dist, n * sizeof(int), cudaMemcpyDeviceToHost);
        cudaMemcpy(h_pred.data(), d_pred, n * sizeof(int), cudaMemcpyDeviceToHost);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Dijkstra (CUDA) completed in " << elapsed.count() << " seconds.\n";
        std::cout << "Distances from source " << source << ": ";
        for (int d : h_dist) std::cout << (d == INF ? -1 : d) << " ";
        std::cout << "\nPredecessors: ";
        for (int p : h_pred) std::cout << p << " ";
        std::cout << std::endl;
        cudaFree(d_adj); cudaFree(d_dist); cudaFree(d_pred); cudaFree(d_updated);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
} 