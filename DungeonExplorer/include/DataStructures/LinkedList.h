#pragma once
#include <iostream>
#include <memory>
#include <functional>

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        std::shared_ptr<Node> next;
        
        Node(const T& value) : data(value), next(nullptr) {}
    };
    
    std::shared_ptr<Node> head;
    size_t listSize;

public:
    LinkedList() : head(nullptr), listSize(0) {}
    
    void insert(const T& value) {
        auto newNode = std::make_shared<Node>(value);
        newNode->next = head;
        head = newNode;
        listSize++;
        // CHANGE: 2025-11-14 - Reduce console spam by removing verbose logging in release builds
        // std::cout << "[DSA-LinkedList] Inserted element. List size: " << listSize << std::endl;
    }
    
    void append(const T& value) {
        auto newNode = std::make_shared<Node>(value);
        
        if (!head) {
            head = newNode;
        } else {
            auto current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
        listSize++;
        // CHANGE: 2025-11-14 - Reduce console spam
        // std::cout << "[DSA-LinkedList] Appended element. List size: " << listSize << std::endl;
    }
    
    bool remove(const T& value) {
        if (!head) return false;
        
        if (head->data == value) {
            head = head->next;
            listSize--;
            // std::cout << "[DSA-LinkedList] Removed element. List size: " << listSize << std::endl;
            return true;
        }
        
        auto current = head;
        while (current->next && current->next->data != value) {
            current = current->next;
        }
        
        if (current->next) {
            current->next = current->next->next;
            listSize--;
            // std::cout << "[DSA-LinkedList] Removed element. List size: " << listSize << std::endl;
            return true;
        }
        
        return false;
    }
    
    bool contains(const T& value) const {
        auto current = head;
        while (current) {
            if (current->data == value) return true;
            current = current->next;
        }
        return false;
    }
    
    void traverse(std::function<void(const T&)> func) const {
        auto current = head;
        while (current) {
            func(current->data);
            current = current->next;
        }
    }
    
    size_t size() const { return listSize; }
    bool isEmpty() const { return head == nullptr; }
    
    void clear() {
        head = nullptr;
        listSize = 0;
        std::cout << "[DSA-LinkedList] Cleared list" << std::endl;
    }
};
