#pragma once
#include <iostream>
#include <memory>
#include <functional>
#include <utility>

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        std::shared_ptr<Node> next;
        
        Node(const T& value) : data(value), next(nullptr) {}
        Node(T&& value) : data(std::move(value)), next(nullptr) {}
    };
    
    std::shared_ptr<Node> head;
    size_t listSize;

public:
    // Iterator class
    class Iterator {
    private:
        std::shared_ptr<Node> current;
        
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        Iterator(std::shared_ptr<Node> node) : current(node) {}
        
        T& operator*() { return current->data; }
        T* operator->() { return &(current->data); }
        
        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }
        
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const Iterator& other) const {
            return current == other.current;
        }
        
        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };
    
    // Const iterator class
    class ConstIterator {
    private:
        std::shared_ptr<Node> current;
        
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;
        
        ConstIterator(std::shared_ptr<Node> node) : current(node) {}
        
        const T& operator*() const { return current->data; }
        const T* operator->() const { return &(current->data); }
        
        ConstIterator& operator++() {
            if (current) current = current->next;
            return *this;
        }
        
        ConstIterator operator++(int) {
            ConstIterator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const ConstIterator& other) const {
            return current == other.current;
        }
        
        bool operator!=(const ConstIterator& other) const {
            return current != other.current;
        }
    };
    
    // Constructors
    LinkedList() : head(nullptr), listSize(0) {}
    
    // Move constructor
    LinkedList(LinkedList&& other) noexcept 
        : head(std::move(other.head)), listSize(other.listSize) {
        other.listSize = 0;
    }
    
    // Move assignment operator
    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            head = std::move(other.head);
            listSize = other.listSize;
            other.listSize = 0;
        }
        return *this;
    }
    
    // Copy constructor and assignment (default is fine for shared_ptr)
    LinkedList(const LinkedList&) = default;
    LinkedList& operator=(const LinkedList&) = default;
    
    // Iterator methods
    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
    ConstIterator begin() const { return ConstIterator(head); }
    ConstIterator end() const { return ConstIterator(nullptr); }
    ConstIterator cbegin() const { return ConstIterator(head); }
    ConstIterator cend() const { return ConstIterator(nullptr); }
    
    // Insert operations
    void insert(const T& value) {
        auto newNode = std::make_shared<Node>(value);
        newNode->next = head;
        head = newNode;
        listSize++;
        // CHANGE: 2025-11-14 - Reduce console spam by removing verbose logging in release builds
        // std::cout << "[DSA-LinkedList] Inserted element. List size: " << listSize << std::endl;
    }
    
    void insert(T&& value) {
        auto newNode = std::make_shared<Node>(std::move(value));
        newNode->next = head;
        head = newNode;
        listSize++;
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
    
    void append(T&& value) {
        auto newNode = std::make_shared<Node>(std::move(value));
        
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
    }
    
    // Insert at specific index
    bool insertAt(size_t index, const T& value) {
        if (index > listSize) return false;
        
        if (index == 0) {
            insert(value);
            return true;
        }
        
        auto current = head;
        for (size_t i = 0; i < index - 1 && current; i++) {
            current = current->next;
        }
        
        if (current) {
            auto newNode = std::make_shared<Node>(value);
            newNode->next = current->next;
            current->next = newNode;
            listSize++;
            return true;
        }
        
        return false;
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
    
    // Find - returns iterator to element
    Iterator find(const T& value) {
        auto current = head;
        while (current) {
            if (current->data == value) {
                return Iterator(current);
            }
            current = current->next;
        }
        return end();
    }
    
    ConstIterator find(const T& value) const {
        auto current = head;
        while (current) {
            if (current->data == value) {
                return ConstIterator(current);
            }
            current = current->next;
        }
        return end();
    }
    
    void traverse(std::function<void(const T&)> func) const {
        auto current = head;
        while (current) {
            func(current->data);
            current = current->next;
        }
    }
    
    // Front and back accessors
    T& front() {
        if (!head) throw std::out_of_range("List is empty");
        return head->data;
    }
    
    const T& front() const {
        if (!head) throw std::out_of_range("List is empty");
        return head->data;
    }
    
    T& back() {
        if (!head) throw std::out_of_range("List is empty");
        auto current = head;
        while (current->next) {
            current = current->next;
        }
        return current->data;
    }
    
    const T& back() const {
        if (!head) throw std::out_of_range("List is empty");
        auto current = head;
        while (current->next) {
            current = current->next;
        }
        return current->data;
    }
    
    // Reverse the list
    void reverse() {
        std::shared_ptr<Node> prev = nullptr;
        auto current = head;
        
        while (current) {
            auto next = current->next;
            current->next = prev;
            prev = current;
            current = next;
        }
        
        head = prev;
    }
    
    size_t size() const { return listSize; }
    bool isEmpty() const { return head == nullptr; }
    
    // Get item at index (for inventory UI)
    T* getItemAt(int index) {
        if (index < 0 || index >= static_cast<int>(listSize)) return nullptr;
        
        auto current = head;
        for (int i = 0; i < index && current; i++) {
            current = current->next;
        }
        
        return current ? &current->data : nullptr;
    }
    
    // Remove item at index (for inventory UI)
    bool removeItemAt(int index) {
        if (index < 0 || index >= static_cast<int>(listSize)) return false;
        
        if (index == 0) {
            head = head->next;
            listSize--;
            return true;
        }
        
        auto current = head;
        for (int i = 0; i < index - 1 && current; i++) {
            current = current->next;
        }
        
        if (current && current->next) {
            current->next = current->next->next;
            listSize--;
            return true;
        }
        
        return false;
    }

    // Swap items at two indices (for inventory reordering)
    bool swap(int index1, int index2) {
        if (index1 < 0 || index1 >= static_cast<int>(listSize) ||
            index2 < 0 || index2 >= static_cast<int>(listSize) ||
            index1 == index2) {
            return false;
        }

        auto node1 = head;
        for (int i = 0; i < index1; ++i) node1 = node1->next;

        auto node2 = head;
        for (int i = 0; i < index2; ++i) node2 = node2->next;

        // Swap data between nodes
        T temp = node1->data;
        node1->data = node2->data;
        node2->data = temp;
        
        return true;
    }
    
    // Get size (alias for compatibility)
    size_t getSize() const { return listSize; }
    
    void clear() {
        head = nullptr;
        listSize = 0;
        std::cout << "[DSA-LinkedList] Cleared list" << std::endl;
    }
};
