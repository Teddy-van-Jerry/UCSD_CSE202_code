#include "RndGraph.hpp"
#include <cassert>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

using namespace hw2;

int main(int argc, char* argv[]) {
    // number of maximum number of nodes
    const size_t N = argc >= 2 ? std::stoi(argv[1]) : 100;
    // N * M as number of Monte-Carlo simulations
    const size_t M = argc >= 3 ? std::stoi(argv[2]) : 10;

    assert(N > 0 && M > 0);

    std::vector<double> avg_decrease_keys(N - 1, 0.0);
    std::vector<double> avg_decrease_keys_lazy(N - 1, 0.0);
    std::vector<size_t> max_decrease_keys(N - 1, 0);
    std::vector<size_t> max_decrease_keys_lazy(N - 1, 0);

    std::cout << std::format("N: Maximum Number of Nodes = {}\nM: Number of Monte-Carlo Simulation = {}\n", N, M);

    for (size_t n = 1; n <= N; ++n) {
        std::cout << std::format(">>> n = {}", n) << std::flush;
        size_t n_sim = std::max(M * n, static_cast<size_t>(2000));
        for (size_t m = 0; m < n_sim; ++m) {
            RndGraph<double> graph(n);

            std::vector<double> distances, distances_lazy;

            size_t decrease_keys      = graph.dijkstra(0, distances);
            size_t decrease_keys_lazy = graph.dijkstra_lazy(0, distances_lazy);
            assert(distances == distances_lazy);

            // Update average and maximum decrease-key operations
            avg_decrease_keys[n - 1] += decrease_keys / static_cast<double>(n_sim);
            avg_decrease_keys_lazy[n - 1] += decrease_keys_lazy / static_cast<double>(n_sim);
            max_decrease_keys[n - 1]      = std::max(max_decrease_keys[n - 1], decrease_keys);
            max_decrease_keys_lazy[n - 1] = std::max(max_decrease_keys_lazy[n - 1], decrease_keys_lazy);
        }
        std::cout << '\r' << std::flush;
    }

    // create directory results if not exists
    if (!std::filesystem::exists("results")) { std::filesystem::create_directory("results"); }

    // Save to files as TikZ coordinates in txt:
    // - results/avg_decrease_keys.txt: (n, avg_decrease_keys)
    // - results/avg_decrease_keys_lazy.txt: (n, avg_decrease_keys_lazy)
    // - results/max_decrease_keys.txt: (n, max_decrease_keys)
    // - results/max_decrease_keys_lazy.txt: (n, max_decrease_keys_lazy)
    std::ofstream avg_decrease_keys_file("results/avg_decrease_keys.txt");
    std::ofstream avg_decrease_keys_file_lazy("results/avg_decrease_keys_lazy.txt");
    std::ofstream max_decrease_keys_file("results/max_decrease_keys.txt");
    std::ofstream max_decrease_keys_file_lazy("results/max_decrease_keys_lazy.txt");

    if (!avg_decrease_keys_file || !avg_decrease_keys_file_lazy || !max_decrease_keys_file ||
        !max_decrease_keys_file_lazy) {
        std::cerr << "Failed to open files for writing." << std::endl;
        return 1;
    }

    for (size_t n = 1; n <= N; ++n) {
        avg_decrease_keys_file << std::format("({}, {})\n", n, avg_decrease_keys[n - 1]);
        avg_decrease_keys_file_lazy << std::format("({}, {})\n", n, avg_decrease_keys_lazy[n - 1]);
        max_decrease_keys_file << std::format("({}, {})\n", n, max_decrease_keys[n - 1]);
        max_decrease_keys_file_lazy << std::format("({}, {})\n", n, max_decrease_keys_lazy[n - 1]);
    }

    avg_decrease_keys_file.close();
    avg_decrease_keys_file_lazy.close();
    max_decrease_keys_file.close();
    max_decrease_keys_file_lazy.close();

    std::cout << "Results saved to results/{avg_decrease_keys[_lazy], max_decrease_keys[_lazy]}.txt" << std::endl;

    return 0;
}
