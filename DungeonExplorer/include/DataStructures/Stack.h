#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include <utility>

template<typename T>
class Stack {
private:
    std::vector<T> data;

public:
    // Constructors
    Stack() = default;
    
    // Move constructor
    Stack(Stack&& other) noexcept : data(std::move(other.data)) {}
    
    // Move assignment operator
    Stack& operator=(Stack&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
        }
        return *this;
    }
    
    // Copy constructor and assignment (default is fine)
    Stack(const Stack&) = default;
    Stack& operator=(const Stack&) = default;
    
    // Push operations
    void push(const T& value) {
        data.push_back(value);
        // CHANGE: 2025-11-14 - Reduce console spam during gameplay loops
        // std::cout << "[DSA-Stack] Pushed element. Stack size: " << data.size() << std::endl;
    }
    
    // Move push
    void push(T&& value) {
        data.push_back(std::move(value));
    }
    
    // Emplace - construct in place
    template<typename... Args>
    void emplace(Args&&... args) {
        data.emplace_back(std::forward<Args>(args)...);
    }
    
    void pop() {
        if (isEmpty()) {
            throw std::out_of_range("Stack is empty");
        }
        data.pop_back();
        // CHANGE: 2025-11-14 - Reduce console spam during gameplay loops
        // std::cout << "[DSA-Stack] Popped element. Stack size: " << data.size() << std::endl;
    }
    
    T& top() {
        if (isEmpty()) {
            throw std::out_of_range("Stack is empty");
        }
        return data.back();
    }
    
    const T& top() const {
        if (isEmpty()) {
            throw std::out_of_range("Stack is empty");
        }
        return data.back();
    }
    
    bool isEmpty() const {
        return data.empty();
    }
    
    size_t size() const {
        return data.size();
    }
    
    // Dynamic array features
    void reserve(size_t capacity) {
        data.reserve(capacity);
    }
    
    size_t capacity() const {
        return data.capacity();
    }
    
    void shrink_to_fit() {
        data.shrink_to_fit();
    }
    
    // Swap
    void swap(Stack& other) noexcept {
        data.swap(other.data);
    }
    
    void clear() {
        data.clear();
        // CHANGE: 2025-11-14 - Reduce console spam during gameplay
        // std::cout << "[DSA-Stack] Cleared stack" << std::endl;
    }
};
