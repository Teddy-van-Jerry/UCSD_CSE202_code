/**
 * @file BinarySearchTree.hpp
 * @author Wuqiong Zhao & Qinpei Luo
 * @brief (Unbalanced) Binary Search Tree With Insertion Tracking
 * @version 0.1
 * @date 2025-01-13
 *
 * @copyright Copyright (c) Wuqiong Zhao & Qinpei Luo
 *
 */

#ifndef __BINARY_SEARCH_TREE_HPP__
#define __BINARY_SEARCH_TREE_HPP__

#include <functional>
#include <iostream>
#include <format>

using std::literals::string_literals::operator""s;

namespace hw1 {

/**
 * @brief A node in the binary search tree
 *
 * @tparam T The type of the data stored in the node
 */
template <typename T>
class Node {
  private:
    T _data;
    size_t _depth;
    Node<T>* left;
    Node<T>* right;

  public:
    /**
     * @brief Construct a new Node object
     *
     * @param data The data to be stored in the node
     * @param depth The depth of the node
     * @param left The left child of the node
     * @param right The right child of the node
     */
    Node(T data, size_t depth = 0u, Node<T>* left = nullptr, Node<T>* right = nullptr)
        : _data(data), _depth(depth), left(left), right(right) {}

    /**
     * @brief Destroy the Node object
     *
     */
    ~Node() = default;

    /**
     * @brief Get the data stored in the node
     *
     * @return T The data stored in the node
     */
    T data() const { return _data; }

    /**
     * @brief Get the read-only left child of the node
     *
     * @return Node<T>* The left child of the node
     */
    const Node<T>* lChild() const { return left; }

    /**
     * @brief Get the writable left child of the node
     *
     * @return Node<T>* The left child of the node
     */
    Node<T>*& lChild() { return left; }

    /**
     * @brief Get the read-only right child of the node
     *
     * @return Node<T>* The right child of the node
     */
    const Node<T>* rChild() const { return right; }

    /**
     * @brief Get the writable right child of the node
     *
     * @return Node<T>* The right child of the node
     */
    Node<T>*& rChild() { return right; }

    /**
     * @brief Get the depth of the node
     *
     * @return size_t The depth of the node
     */
    size_t depth() const { return _depth; }

    /**
     * @brief Get the data stored in the node
     *
     * This is an alias of the data() method.
     * @return T
     */
    T operator*() const { return _data; }
};

template <typename T>
class BinarySearchTree {
  private:
    Node<T>* _root    = nullptr;
    size_t _size      = 0u;
    size_t _depth     = 0u;
    size_t _n_compare = 0u;

  public:
    /**
     * @brief Construct a new Binary Search Tree object
     *
     */
    BinarySearchTree() = default;

    /**
     * @brief Construct a new Binary Search Tree object from another tree
     *
     * So far, this is not implemented and this operation is not permitted.
     */
    BinarySearchTree(const BinarySearchTree<T>&) = delete;

    /**
     * @brief Destroy the Binary Search Tree object
     *
     * This will call the clear() method to remove all elements in the tree.
     */
    ~BinarySearchTree() { clear(); }

    /**
     * @brief Remove all elements from the tree
     *
     * This will recursively delete all nodes in the tree.
     */
    void clear() {
        if (!_root) return;

        std::function<void(Node<T>*)> deleteNode = [&](Node<T>* node) {
            if (node->lChild()) { deleteNode(node->lChild()); }
            if (node->rChild()) { deleteNode(node->rChild()); }
            delete node;
        };

        deleteNode(_root);
        _root      = nullptr;
        _size      = 0u;
        _depth     = 0u;
        _n_compare = 0u;
    }

    /**
     * @brief Get the number of elements in the tree
     *
     * @return size_t The number of elements in the tree
     */
    size_t size() const { return _size; }

    /**
     * @brief Get the depth of the tree
     *
     * @return size_t The depth of the tree
     */
    size_t depth() const { return _depth; }

    /**
     * @brief Get the number of comparisons made during insertion
     *
     * @return size_t The number of comparisons made during insertion
     */
    size_t n_compare() const { return _n_compare; }

    /**
     * @brief Get the read-only root node of the tree
     *
     * @return const Node<T>& The root node of the tree
     */
    const Node<T>& root() const { return *_root; }

    /**
     * @brief Get the writable root node of the tree
     *
     * @return Node<T>& The root node of the tree
     */
    Node<T>& root() { return *_root; }

    /**
     * @brief Insert a new element into the tree
     *
     * In this simple design, duplicates are dropped.
     * @param data The data to be inserted
     * @return BinarySearchTree<T>& The reference to the tree
     */
    BinarySearchTree<T>& insert(T data) {
        if (!_root) {
            _root = new Node<T>(data);
            ++_size;
            return *this;
        }

        Node<T>* current = _root;
        while (true) {
            ++_n_compare;
            if (data < **current) {
                if (!current->lChild()) {
                    current->lChild() = new Node<T>(data, current->depth() + 1);
                    ++_size;
                    _depth = std::max(_depth, current->depth() + 1);
                    break;
                }
                current = current->lChild();
            } else if (data > **current) {
                if (!current->rChild()) {
                    current->rChild() = new Node<T>(data, current->depth() + 1);
                    ++_size;
                    _depth = std::max(_depth, current->depth() + 1);
                    break;
                }
                current = current->rChild();
            } else {
                // Oops, drop duplicates!
                break;
            }
        }

        return *this;
    }

    BinarySearchTree<T>& traverse(std::function<void(const Node<T>&)> callback) {
        std::function<void(const Node<T>&)> traverseNode = [&](const Node<T>& node) {
            callback(node);
            if (node.lChild()) { traverseNode(*node.lChild()); }
            if (node.rChild()) { traverseNode(*node.rChild()); }
        };

        if (_root) { traverseNode(root()); }

        return *this;
    }

    void traversePrint() {
        auto nodeStr = [](const Node<T>* node) {
            return node ? std::format("{}", **node) : "null"s;
        };
        traverse([nodeStr](const Node<T>& node) { std::cout << std::format("{} (L: {}, R: {})\n", *node, nodeStr(node.lChild()), nodeStr(node.rChild())); });
        std::cout << std::flush;
    }
};

} // namespace hw1

#endif
