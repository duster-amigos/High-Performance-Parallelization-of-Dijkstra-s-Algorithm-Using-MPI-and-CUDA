#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <string>
#include <getopt.h>

void print_usage() {
    std::cout << "Usage: adjacency_matrix -i <input_edge_list> -n <nodes> [-d (directed)] -o <output_matrix>\n";
}

int main(int argc, char* argv[]) {
    int n = 0;
    bool directed = false;
    std::string input_file, output_file;
    int opt;
    while ((opt = getopt(argc, argv, "i:n:do:")) != -1) {
        switch (opt) {
            case 'i': input_file = optarg; break;
            case 'n': n = std::stoi(optarg); break;
            case 'd': directed = true; break;
            case 'o': output_file = optarg; break;
            default: print_usage(); return 1;
        }
    }
    if (n <= 0 || input_file.empty() || output_file.empty()) {
        print_usage();
        return 1;
    }
    try {
        std::vector<std::vector<int>> adj(n, std::vector<int>(n, std::numeric_limits<int>::max()));
        for (int i = 0; i < n; ++i) adj[i][i] = 0;
        std::ifstream ifs(input_file);
        if (!ifs) throw std::runtime_error("Cannot open input file");
        int u, v, w;
        while (ifs >> u >> v >> w) {
            adj[u][v] = w;
            if (!directed) adj[v][u] = w;
        }
        ifs.close();
        std::ofstream ofs(output_file);
        if (!ofs) throw std::runtime_error("Cannot open output file");
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                ofs << (adj[i][j] == std::numeric_limits<int>::max() ? -1 : adj[i][j]);
                if (j < n-1) ofs << " ";
            }
            ofs << "\n";
        }
        ofs.close();
        std::cout << "Adjacency matrix written to: " << output_file << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
} 