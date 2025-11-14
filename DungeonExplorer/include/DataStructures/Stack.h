#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>

template<typename T>
class Stack {
private:
    std::vector<T> data;

public:
    Stack() = default;
    
    void push(const T& value) {
        data.push_back(value);
        // CHANGE: 2025-11-14 - Reduce console spam during gameplay loops
        // std::cout << "[DSA-Stack] Pushed element. Stack size: " << data.size() << std::endl;
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
    
    void clear() {
        data.clear();
        // CHANGE: 2025-11-14 - Reduce console spam during gameplay
        // std::cout << "[DSA-Stack] Cleared stack" << std::endl;
    }
};
