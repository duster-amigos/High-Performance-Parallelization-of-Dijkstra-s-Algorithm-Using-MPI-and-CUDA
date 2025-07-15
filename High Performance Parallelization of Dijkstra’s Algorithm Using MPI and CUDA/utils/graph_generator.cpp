#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <getopt.h>

void print_usage() {
    std::cout << "Usage: graph_generator -n <nodes> -e <edges> [-d (directed)] -o <output_file>\n";
}

int main(int argc, char* argv[]) {
    int n = 0, e = 0;
    bool directed = false;
    std::string output_file;
    int opt;
    while ((opt = getopt(argc, argv, "n:e:do:")) != -1) {
        switch (opt) {
            case 'n': n = std::stoi(optarg); break;
            case 'e': e = std::stoi(optarg); break;
            case 'd': directed = true; break;
            case 'o': output_file = optarg; break;
            default: print_usage(); return 1;
        }
    }
    if (n <= 0 || e <= 0 || output_file.empty()) {
        print_usage();
        return 1;
    }
    try {
        std::ofstream ofs(output_file);
        if (!ofs) throw std::runtime_error("Cannot open output file");
        std::set<std::pair<int, int>> edges;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, n-1);
        std::uniform_int_distribution<> wdist(1, 10);
        while ((int)edges.size() < e) {
            int u = dist(gen), v = dist(gen);
            if (u == v) continue;
            auto edge = std::make_pair(u, v);
            auto rev = std::make_pair(v, u);
            if (edges.count(edge) || (!directed && edges.count(rev))) continue;
            edges.insert(edge);
            int w = wdist(gen);
            ofs << u << " " << v << " " << w << "\n";
            if (!directed) ofs << v << " " << u << " " << w << "\n";
        }
        ofs.close();
        std::cout << "Graph generated: " << output_file << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
} 