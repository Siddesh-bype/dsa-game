#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>

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
    Heap() = default;
    
    void insert(const T& value) {
        data.push_back(value);
        heapifyUp(data.size() - 1);
        std::cout << "[DSA-Heap] Inserted element. Heap size: " << data.size() << std::endl;
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
        
        std::cout << "[DSA-Heap] Extracted top element. Heap size: " << data.size() << std::endl;
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
    
    void clear() {
        data.clear();
        std::cout << "[DSA-Heap] Cleared heap" << std::endl;
    }
};
