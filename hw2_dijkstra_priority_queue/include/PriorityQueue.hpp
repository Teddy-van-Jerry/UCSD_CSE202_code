/**
 * @file PriorityQueue.hpp
 * @author Wuqiong Zhao & Qinpei Luo
 * @brief Priority Queue Implementation
 * @version 0.1
 * @date 2025-01-26
 *
 * @copyright Copyright (c) 2025 Wuqiong Zhao & Qinpei Luo
 *
 */

#ifndef __PRIORITY_QUEUE_HPP__
#define __PRIORITY_QUEUE_HPP__

#include <limits>
#include <vector>

namespace hw2 {

template <typename T>
class PriorityQueue {
  private:
    std::vector<std::pair<size_t, T>> heap;
    std::vector<size_t> pos; // position of each node in the heap
    size_t size_;
    size_t decrease_key_cnt_;

    constexpr static inline size_t NOT_FOUND_POS = std::numeric_limits<size_t>::max();

  public:
    PriorityQueue() : heap(), pos(), size_(0), decrease_key_cnt_(0) {}

    void insert(size_t node, T priority) {
        if (node >= pos.size()) { pos.resize(node + 1, -1); }
        heap.emplace_back(node, priority);
        pos[node] = size_;
        size_++;
        heapify_up(size_ - 1);
    }

    bool size() const noexcept { return size_; }

    bool empty() const noexcept { return size() == 0; }

    std::pair<size_t, T> delete_min() {
        if (empty()) { throw std::runtime_error("PriorityQueue is empty"); }
        auto min_elem       = heap[0];
        heap[0]             = heap[size_ - 1];
        pos[heap[0].first]  = 0;
        pos[min_elem.first] = NOT_FOUND_POS;
        --size_;
        heap.pop_back();
        heapify_down(0);
        return min_elem;
    }

    void decrease_key(size_t node, T new_priority) {
        auto i = pos[node];
        if (i == NOT_FOUND_POS) { throw std::runtime_error("Node not found in PriorityQueue"); }
        if (heap[i].second <= new_priority) { throw std::runtime_error("New priority is not smaller"); }
        heap[i].second = new_priority;
        heapify_up(i);
        ++decrease_key_cnt_;
    }

    size_t decrease_key_cnt() const { return decrease_key_cnt_; }

    void reset_decrease_key_cnt() { decrease_key_cnt_ = 0; }

  private:
    void heapify_up(size_t i) {
        while (i > 0) {
            if (size_t parent = (i - 1) / 2; heap[i].second < heap[parent].second) {
                std::swap(heap[i], heap[parent]);
                pos[heap[i].first]      = i;
                pos[heap[parent].first] = parent;
                i                       = parent;
            } else break;
        }
    }

    void heapify_down(size_t i) {
        while (true) {
            size_t smallest = i;
            size_t left     = 2 * i + 1;
            size_t right    = 2 * i + 2;
            if (left < size_ && heap[left].second < heap[smallest].second) { smallest = left; }
            if (right < size_ && heap[right].second < heap[smallest].second) { smallest = right; }
            if (smallest != i) {
                std::swap(heap[i], heap[smallest]);
                pos[heap[i].first]        = i;
                pos[heap[smallest].first] = smallest;
                i                         = smallest;
            } else break;
        }
    }
};

} // namespace hw2

#endif
