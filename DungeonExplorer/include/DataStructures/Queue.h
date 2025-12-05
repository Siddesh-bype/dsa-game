#pragma once
#include <deque>
#include <iostream>
#include <stdexcept>
#include <utility>

template<typename T>
class Queue {
private:
    std::deque<T> data;

public:
    // Constructors
    Queue() = default;
    
    // Move constructor
    Queue(Queue&& other) noexcept : data(std::move(other.data)) {}
    
    // Move assignment operator
    Queue& operator=(Queue&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
        }
        return *this;
    }
    
    // Copy constructor and assignment (default is fine)
    Queue(const Queue&) = default;
    Queue& operator=(const Queue&) = default;
    
    // Enqueue operations
    void enqueue(const T& value) {
        data.push_back(value);
        std::cout << "[DSA-Queue] Enqueued element. Queue size: " << data.size() << std::endl;
    }
    
    // Move enqueue
    void enqueue(T&& value) {
        data.push_back(std::move(value));
    }
    
    // Emplace - construct in place
    template<typename... Args>
    void emplace(Args&&... args) {
        data.emplace_back(std::forward<Args>(args)...);
    }
    
    void dequeue() {
        if (isEmpty()) {
            throw std::out_of_range("Queue is empty");
        }
        data.pop_front();
        std::cout << "[DSA-Queue] Dequeued element. Queue size: " << data.size() << std::endl;
    }
    
    T& front() {
        if (isEmpty()) {
            throw std::out_of_range("Queue is empty");
        }
        return data.front();
    }
    
    const T& front() const {
        if (isEmpty()) {
            throw std::out_of_range("Queue is empty");
        }
        return data.front();
    }
    
    // Back accessor
    T& back() {
        if (isEmpty()) {
            throw std::out_of_range("Queue is empty");
        }
        return data.back();
    }
    
    const T& back() const {
        if (isEmpty()) {
            throw std::out_of_range("Queue is empty");
        }
        return data.back();
    }
    
    bool isEmpty() const {
        return data.empty();
    }
    
    size_t size() const {
        return data.size();
    }
    
    // Dynamic array features (deque supports these)
    void shrink_to_fit() {
        data.shrink_to_fit();
    }
    
    // Swap
    void swap(Queue& other) noexcept {
        data.swap(other.data);
    }
    
    void clear() {
        data.clear();
        std::cout << "[DSA-Queue] Cleared queue" << std::endl;
    }
};
