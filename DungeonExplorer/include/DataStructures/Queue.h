#pragma once
#include <deque>
#include <iostream>
#include <stdexcept>

template<typename T>
class Queue {
private:
    std::deque<T> data;

public:
    Queue() = default;
    
    void enqueue(const T& value) {
        data.push_back(value);
        std::cout << "[DSA-Queue] Enqueued element. Queue size: " << data.size() << std::endl;
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
    
    bool isEmpty() const {
        return data.empty();
    }
    
    size_t size() const {
        return data.size();
    }
    
    void clear() {
        data.clear();
        std::cout << "[DSA-Queue] Cleared queue" << std::endl;
    }
};
