/**
 * @file main.cpp
 * @author Wuqiong Zhao & Qinpei Luo
 * @brief Homework 1 Main Program (Binary Search Insertion)
 * @version 0.1
 * @date 2025-01-13
 *
 * @copyright Copyright (c) 2025 Wuqiong Zhao & Qinpei Luo
 *
 */

#include "BinarySearchTree.hpp"
#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <numeric>
#include <random>
#include <vector>

using namespace hw1;

// random permute sequence
std::vector<int> random_seq(size_t n) {
    std::vector<int> seq(n);
    std::iota(seq.begin(), seq.end(), 1);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(seq.begin(), seq.end(), g);
    return seq;
}

int main(int argc, char* argv[]) {
    // number of maximum number of nodes
    const size_t n = argc >= 2 ? std::stoi(argv[1]) : 1000;
    // number of Monte-Carlo simulations
    const size_t m = argc >= 3 ? std::stoi(argv[2]) : 10000;

    std::vector<double> avg_depth(n, 0);
    std::vector<double> avg_comparison(n, 0);

    BinarySearchTree<int> bst;

    for (size_t j = 0; j != m; ++j) {
        auto seq = random_seq(n);
        for (size_t i = 0; i != n; ++i) {
            auto depth      = bst.insert(seq[i]).depth();
            auto comparison = bst.n_compare();
            avg_depth[i] += static_cast<double>(depth) / m;
            avg_comparison[i] += static_cast<double>(comparison) / m;
        }
        bst.clear();
    }

    std::cout << std::format("n: Maximum Number of Nodes = {}\nm: Number of Monte-Carlo Simulation = {}\n", n, m);

    // create directory results if not exists
    if (!std::filesystem::exists("results")) { std::filesystem::create_directory("results"); }

    // Save to files as TikZ coordinates in txt:
    // - results/avg_depth.txt: (n, avg_depth)
    // - results/avg_comparison.txt: (n, avg_comparison)
    std::ofstream avg_depth_file("results/avg_depth.txt");
    std::ofstream avg_comparison_file("results/avg_comparison.txt");

    if (!avg_depth_file || !avg_comparison_file) {
        std::cerr << "Failed to open files for writing." << std::endl;
        return 1;
    }

    for (size_t i = 0; i != n; ++i) {
        avg_depth_file << std::format("({}, {})\n", i + 1, avg_depth[i]);
        avg_comparison_file << std::format("({}, {})\n", i + 1, avg_comparison[i]);
    }

    avg_depth_file.close();
    avg_comparison_file.close();

    std::cout << "Results saved to results/{avg_depth, avg_comparison}.txt" << std::endl;

    return 0;
}
