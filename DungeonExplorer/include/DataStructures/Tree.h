#pragma once
#include <iostream>
#include <memory>
#include <functional>
#include <queue>
#include <vector>
#include <algorithm>
#include <utility>

template<typename T>
class BinaryTree {
public:
    struct Node {
        T data;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        
        Node(const T& value) : data(value), left(nullptr), right(nullptr) {}
        Node(T&& value) : data(std::move(value)), left(nullptr), right(nullptr) {}
    };
    
private:
    std::shared_ptr<Node> root;
    
    void inorderTraversal(std::shared_ptr<Node> node, std::function<void(const T&)> func) const {
        if (!node) return;
        inorderTraversal(node->left, func);
        func(node->data);
        inorderTraversal(node->right, func);
    }
    
    void preorderTraversal(std::shared_ptr<Node> node, std::function<void(const T&)> func) const {
        if (!node) return;
        func(node->data);
        preorderTraversal(node->left, func);
        preorderTraversal(node->right, func);
    }
    
    void postorderTraversal(std::shared_ptr<Node> node, std::function<void(const T&)> func) const {
        if (!node) return;
        postorderTraversal(node->left, func);
        postorderTraversal(node->right, func);
        func(node->data);
    }
    
    int heightHelper(std::shared_ptr<Node> node) const {
        if (!node) return 0;
        return 1 + std::max(heightHelper(node->left), heightHelper(node->right));
    }
    
    size_t sizeHelper(std::shared_ptr<Node> node) const {
        if (!node) return 0;
        return 1 + sizeHelper(node->left) + sizeHelper(node->right);
    }
    
    bool containsHelper(std::shared_ptr<Node> node, const T& value) const {
        if (!node) return false;
        if (node->data == value) return true;
        return containsHelper(node->left, value) || containsHelper(node->right, value);
    }
    
    std::shared_ptr<Node> findNodeHelper(std::shared_ptr<Node> node, const T& value) const {
        if (!node) return nullptr;
        if (node->data == value) return node;
        
        auto leftResult = findNodeHelper(node->left, value);
        if (leftResult) return leftResult;
        
        return findNodeHelper(node->right, value);
    }
    
    void getLeavesHelper(std::shared_ptr<Node> node, std::vector<T>& leaves) const {
        if (!node) return;
        if (!node->left && !node->right) {
            leaves.push_back(node->data);
            return;
        }
        getLeavesHelper(node->left, leaves);
        getLeavesHelper(node->right, leaves);
    }
    
    bool isBalancedHelper(std::shared_ptr<Node> node, int& height) const {
        if (!node) {
            height = 0;
            return true;
        }
        
        int leftHeight = 0, rightHeight = 0;
        bool leftBalanced = isBalancedHelper(node->left, leftHeight);
        bool rightBalanced = isBalancedHelper(node->right, rightHeight);
        
        height = 1 + std::max(leftHeight, rightHeight);
        
        if (std::abs(leftHeight - rightHeight) > 1) return false;
        
        return leftBalanced && rightBalanced;
    }

public:
    // Constructors
    BinaryTree() : root(nullptr) {}
    
    // Move constructor
    BinaryTree(BinaryTree&& other) noexcept : root(std::move(other.root)) {}
    
    // Move assignment operator
    BinaryTree& operator=(BinaryTree&& other) noexcept {
        if (this != &other) {
            root = std::move(other.root);
        }
        return *this;
    }
    
    // Copy constructor and assignment (default is fine for shared_ptr)
    BinaryTree(const BinaryTree&) = default;
    BinaryTree& operator=(const BinaryTree&) = default;
    
    std::shared_ptr<Node> getRoot() { return root; }
    const std::shared_ptr<Node> getRoot() const { return root; }
    
    void setRoot(const T& value) {
        root = std::make_shared<Node>(value);
        std::cout << "[DSA-Tree] Set root node" << std::endl;
    }
    
    void setRoot(T&& value) {
        root = std::make_shared<Node>(std::move(value));
    }
    
    std::shared_ptr<Node> insertLeft(std::shared_ptr<Node> parent, const T& value) {
        if (!parent) return nullptr;
        parent->left = std::make_shared<Node>(value);
        std::cout << "[DSA-Tree] Inserted left child" << std::endl;
        return parent->left;
    }
    
    std::shared_ptr<Node> insertRight(std::shared_ptr<Node> parent, const T& value) {
        if (!parent) return nullptr;
        parent->right = std::make_shared<Node>(value);
        std::cout << "[DSA-Tree] Inserted right child" << std::endl;
        return parent->right;
    }
    
    void inorder(std::function<void(const T&)> func) const {
        std::cout << "[DSA-Tree] Inorder traversal" << std::endl;
        inorderTraversal(root, func);
    }
    
    void preorder(std::function<void(const T&)> func) const {
        std::cout << "[DSA-Tree] Preorder traversal" << std::endl;
        preorderTraversal(root, func);
    }
    
    void postorder(std::function<void(const T&)> func) const {
        std::cout << "[DSA-Tree] Postorder traversal" << std::endl;
        postorderTraversal(root, func);
    }
    
    void levelOrder(std::function<void(const T&)> func) const {
        if (!root) return;
        std::cout << "[DSA-Tree] Level-order traversal" << std::endl;
        
        std::queue<std::shared_ptr<Node>> q;
        q.push(root);
        
        while (!q.empty()) {
            auto node = q.front();
            q.pop();
            func(node->data);
            
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
    }
    
    bool isEmpty() const {
        return root == nullptr;
    }
    
    // Enhanced features
    int height() const {
        return heightHelper(root);
    }
    
    size_t size() const {
        return sizeHelper(root);
    }
    
    bool contains(const T& value) const {
        return containsHelper(root, value);
    }
    
    std::shared_ptr<Node> findNode(const T& value) const {
        return findNodeHelper(root, value);
    }
    
    std::vector<T> getLeaves() const {
        std::vector<T> leaves;
        getLeavesHelper(root, leaves);
        return leaves;
    }
    
    bool isBalanced() const {
        int height = 0;
        return isBalancedHelper(root, height);
    }
    
    void clear() {
        root = nullptr;
        std::cout << "[DSA-Tree] Cleared tree" << std::endl;
    }
};
