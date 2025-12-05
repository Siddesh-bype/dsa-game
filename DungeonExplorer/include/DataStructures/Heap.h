#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <utility>

template<typename T, typename Compare = std::less<T>>
class Heap {
private:
    std::vector<T> data;
    Compare comp;
    
    void heapifyUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (comp(data[parent], data[index])) {
                std::swap(data[parent], data[index]);
                index = parent;
            } else {
                break;
            }
        }
    }
    
    void heapifyDown(size_t index) {
        size_t size = data.size();
        while (true) {
            size_t largest = index;
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;
            
            if (left < size && comp(data[largest], data[left])) {
                largest = left;
            }
            if (right < size && comp(data[largest], data[right])) {
                largest = right;
            }
            
            if (largest != index) {
                std::swap(data[index], data[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }

public:
    // Constructors
    Heap() = default;
    
    // Range constructor
    template<typename Iterator>
    Heap(Iterator begin, Iterator end) {
        data.assign(begin, end);
        // Build heap from bottom up
        for (int i = static_cast<int>(data.size() / 2) - 1; i >= 0; --i) {
            heapifyDown(static_cast<size_t>(i));
        }
    }
    
    // Move constructor
    Heap(Heap&& other) noexcept 
        : data(std::move(other.data)), comp(std::move(other.comp)) {}
    
    // Move assignment operator
    Heap& operator=(Heap&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            comp = std::move(other.comp);
        }
        return *this;
    }
    
    // Copy constructor and assignment (default is fine)
    Heap(const Heap&) = default;
    Heap& operator=(const Heap&) = default;
    
    // Insert operations
    void insert(const T& value) {
        data.push_back(value);
        heapifyUp(data.size() - 1);
        // CHANGE: 2025-11-14 - Reduce console spam during gameplay loops
        // std::cout << "[DSA-Heap] Inserted element. Heap size: " << data.size() << std::endl;
    }
    
    // Move insert
    void insert(T&& value) {
        data.push_back(std::move(value));
        heapifyUp(data.size() - 1);
    }
    
    // Emplace - construct in place
    template<typename... Args>
    void emplace(Args&&... args) {
        data.emplace_back(std::forward<Args>(args)...);
        heapifyUp(data.size() - 1);
    }
    
    T extractTop() {
        if (isEmpty()) {
            throw std::out_of_range("Heap is empty");
        }
        
        T top = data[0];
        data[0] = data.back();
        data.pop_back();
        
        if (!isEmpty()) {
            heapifyDown(0);
        }
        
        // CHANGE: 2025-11-14 - Reduce console spam during gameplay loops
        // std::cout << "[DSA-Heap] Extracted top element. Heap size: " << data.size() << std::endl;
        return top;
    }
    
    const T& top() const {
        if (isEmpty()) {
            throw std::out_of_range("Heap is empty");
        }
        return data[0];
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
    void swap(Heap& other) noexcept {
        data.swap(other.data);
        std::swap(comp, other.comp);
    }
    
    void clear() {
        data.clear();
        // CHANGE: 2025-11-14 - Reduce console spam during gameplay
        // std::cout << "[DSA-Heap] Cleared heap" << std::endl;
    }
};
