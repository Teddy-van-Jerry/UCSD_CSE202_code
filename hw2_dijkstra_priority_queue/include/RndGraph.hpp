/**
 * @file RndGraph.hpp
 * @author Wuqiong Zhao & Qinpei Luo
 * @brief Graph with random weights.
 * @version 0.1
 * @date 2025-01-26
 *
 * @copyright Copyright (c) 2025 Wuqiong Zhao & Qinpei Luo
 *
 */

#ifndef __RND_GRAPH_HPP__
#define __RND_GRAPH_HPP__

#include "PriorityQueue.hpp"
#include <format>
#include <iostream>
#include <random>

namespace hw2 {

// Graph class template
template <typename T>
class RndGraph {
  private:
    size_t n;
    std::vector<std::vector<std::pair<size_t, T>>> adj;

    constexpr static inline T T_INF = std::numeric_limits<T>::infinity();

  public:
    RndGraph(size_t num_nodes) : n(num_nodes), adj(num_nodes, std::vector<std::pair<size_t, T>>()) {
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<T> dist(0.0, 1.0);

        for (size_t u = 0; u < n; ++u) {
            for (size_t v = 0; v < n; ++v) {
                if (u != v) { adj[u].emplace_back(v, dist(rng)); }
            }
        }
    }

    void print() const {
        for (size_t u = 0; u < n; ++u) {
            std::cout << std::format("Node {}: ", u);
            for (auto&& [v, weight] : adj[u]) { std::cout << std::format("({}, {:.2f}) ", v, weight); }
            std::cout << std::endl;
        }
    }

    // Dijkstra's algorithm from source node
    // Returns the number of decrease-key operations
    size_t dijkstra(size_t source, std::vector<T>& distances) const {
        distances.assign(n, T_INF);
        distances[source] = 0;

        auto pq = make_queue(source);

        while (!pq.empty()) {
            auto [u, dist_u] = pq.delete_min();
            if (dist_u == T_INF) break;

            for (auto&& [v, weight] : adj[u]) {
                if (T alt = dist_u + weight; alt < distances[v]) {
                    distances[v] = alt;
                    pq.decrease_key(v, alt);
                }
            }
        }

        return pq.decrease_key_cnt();
    }

    size_t dijkstra_lazy(size_t source, std::vector<T>& distances) const {
        distances.assign(n, T_INF);
        distances[source] = 0;

        std::priority_queue<std::pair<T, size_t>, std::vector<std::pair<T, size_t>>, std::greater<>> pq;

        size_t decrease_key_count = 0;

        std::vector<bool> in_queue(n, false);
        pq.emplace(0, source);
        in_queue[source] = true;

        while (!pq.empty()) {
            auto [dist_u, u] = pq.top();
            pq.pop();
            in_queue[u] = false;

            // Skip if we already found a better path
            if (dist_u > distances[u]) continue;

            for (auto&& [v, weight] : adj[u]) {
                if (T alt = distances[u] + weight; alt < distances[v]) {
                    distances[v] = alt;
                    decrease_key_count++;

                    // Instead of decrease_key, we just add another entry
                    if (!in_queue[v]) {
                        pq.emplace(alt, v);
                        in_queue[v] = true;
                    }
                }
            }
        }

        return decrease_key_count;
    }

  private:
    PriorityQueue<T> make_queue(size_t source = 0) const {
        PriorityQueue<T> pq;
        for (size_t i = 0; i < n; ++i) { pq.insert(i, (i == source) ? 0 : T_INF); }
        return pq;
    }
};

} // namespace hw2

#endif
