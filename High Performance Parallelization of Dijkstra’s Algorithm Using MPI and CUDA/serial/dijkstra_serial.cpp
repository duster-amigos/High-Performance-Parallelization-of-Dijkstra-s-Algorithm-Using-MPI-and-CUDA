#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <chrono>
#include <string>
#include <getopt.h>

void print_usage() {
    std::cout << "Usage: dijkstra_serial -i <adj_matrix> -s <source> -n <nodes>\n";
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
        std::vector<std::vector<int>> adj(n, std::vector<int>(n));
        std::ifstream ifs(input_file);
        if (!ifs) throw std::runtime_error("Cannot open input file");
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j) {
                int val; ifs >> val;
                adj[i][j] = (val == -1 ? std::numeric_limits<int>::max() : val);
            }
        ifs.close();
        std::vector<int> dist(n, std::numeric_limits<int>::max());
        std::vector<int> pred(n, -1);
        std::vector<bool> visited(n, false);
        dist[source] = 0;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < n; ++i) {
            int u = -1, min_dist = std::numeric_limits<int>::max();
            for (int j = 0; j < n; ++j)
                if (!visited[j] && dist[j] < min_dist) { u = j; min_dist = dist[j]; }
            if (u == -1) break;
            visited[u] = true;
            for (int v = 0; v < n; ++v) {
                if (!visited[v] && adj[u][v] != std::numeric_limits<int>::max() && dist[u] + adj[u][v] < dist[v]) {
                    dist[v] = dist[u] + adj[u][v];
                    pred[v] = u;
                }
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Dijkstra (serial) completed in " << elapsed.count() << " seconds.\n";
        std::cout << "Distances from source " << source << ": ";
        for (int d : dist) std::cout << (d == std::numeric_limits<int>::max() ? -1 : d) << " ";
        std::cout << "\nPredecessors: ";
        for (int p : pred) std::cout << p << " ";
        std::cout << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
} 