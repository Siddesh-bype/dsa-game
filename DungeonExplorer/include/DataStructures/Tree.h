#pragma once
#include <iostream>
#include <memory>
#include <functional>
#include <queue>

template<typename T>
class BinaryTree {
public:
    struct Node {
        T data;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        
        Node(const T& value) : data(value), left(nullptr), right(nullptr) {}
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

public:
    BinaryTree() : root(nullptr) {}
    
    std::shared_ptr<Node> getRoot() { return root; }
    const std::shared_ptr<Node> getRoot() const { return root; }
    
    void setRoot(const T& value) {
        root = std::make_shared<Node>(value);
        std::cout << "[DSA-Tree] Set root node" << std::endl;
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
};
