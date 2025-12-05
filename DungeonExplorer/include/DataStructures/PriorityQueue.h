#pragma once
#include <vector>
#include <functional>
#include <iostream>

// ═══════════════════════════════════════════════════════════════════════════
// PRIORITY QUEUE - Min-Heap Implementation (DSA: Heap Data Structure)
// Used for: A* pathfinding, enemy turn ordering, loot priority
// Time Complexity: Insert O(log n), Extract-Min O(log n), Peek O(1)
// ═══════════════════════════════════════════════════════════════════════════

template<typename T>
class PriorityQueue {
private:
    std::vector<std::pair<float, T>> heap;  // (priority, data)
    
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }
    
    void heapifyUp(int index) {
        while (index > 0 && heap[parent(index)].first > heap[index].first) {
            std::swap(heap[parent(index)], heap[index]);
            index = parent(index);
        }
    }
    
    void heapifyDown(int index) {
        int minIndex = index;
        int left = leftChild(index);
        int right = rightChild(index);
        
        if (left < static_cast<int>(heap.size()) && heap[left].first < heap[minIndex].first) {
            minIndex = left;
        }
        if (right < static_cast<int>(heap.size()) && heap[right].first < heap[minIndex].first) {
            minIndex = right;
        }
        
        if (minIndex != index) {
            std::swap(heap[index], heap[minIndex]);
            heapifyDown(minIndex);
        }
    }

public:
    PriorityQueue() = default;
    
    void push(float priority, const T& data) {
        heap.push_back({priority, data});
        heapifyUp(heap.size() - 1);
    }
    
    T pop() {
        if (heap.empty()) {
            throw std::runtime_error("PriorityQueue is empty");
        }
        T result = heap[0].second;
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            heapifyDown(0);
        }
        return result;
    }
    
    std::pair<float, T> top() const {
        if (heap.empty()) {
            throw std::runtime_error("PriorityQueue is empty");
        }
        return heap[0];
    }
    
    bool empty() const { return heap.empty(); }
    size_t size() const { return heap.size(); }
    
    void clear() { heap.clear(); }
    
    // Update priority of existing element (for A* open set updates)
    bool updatePriority(const T& data, float newPriority) {
        for (size_t i = 0; i < heap.size(); ++i) {
            if (heap[i].second == data) {
                float oldPriority = heap[i].first;
                heap[i].first = newPriority;
                if (newPriority < oldPriority) {
                    heapifyUp(i);
                } else {
                    heapifyDown(i);
                }
                return true;
            }
        }
        return false;
    }
    
    bool contains(const T& data) const {
        for (const auto& item : heap) {
            if (item.second == data) return true;
        }
        return false;
    }
};
