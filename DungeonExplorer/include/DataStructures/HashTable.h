#pragma once
#include <unordered_map>
#include <iostream>
#include <optional>
#include <utility>

template<typename K, typename V>
class HashTable {
private:
    std::unordered_map<K, V> table;

public:
    // Iterator types
    using iterator = typename std::unordered_map<K, V>::iterator;
    using const_iterator = typename std::unordered_map<K, V>::const_iterator;
    
    // Constructors
    HashTable() = default;
    
    // Move constructor
    HashTable(HashTable&& other) noexcept : table(std::move(other.table)) {}
    
    // Move assignment operator
    HashTable& operator=(HashTable&& other) noexcept {
        if (this != &other) {
            table = std::move(other.table);
        }
        return *this;
    }
    
    // Copy constructor and assignment (default is fine)
    HashTable(const HashTable&) = default;
    HashTable& operator=(const HashTable&) = default;
    
    // Iterator methods
    iterator begin() { return table.begin(); }
    iterator end() { return table.end(); }
    const_iterator begin() const { return table.begin(); }
    const_iterator end() const { return table.end(); }
    const_iterator cbegin() const { return table.cbegin(); }
    const_iterator cend() const { return table.cend(); }
    
    // Insert operations
    void insert(const K& key, const V& value) {
        table[key] = value;
        std::cout << "[DSA-HashTable] Inserted key. Table size: " << table.size() << std::endl;
    }
    
    // Move insert
    void insert(K&& key, V&& value) {
        table[std::move(key)] = std::move(value);
    }
    
    // Emplace - construct value in place
    template<typename... Args>
    void emplace(K&& key, Args&&... args) {
        table.emplace(std::move(key), std::forward<Args>(args)...);
    }
    
    std::optional<V> get(const K& key) const {
        auto it = table.find(key);
        if (it != table.end()) {
            std::cout << "[DSA-HashTable] Key found" << std::endl;
            return it->second;
        }
        std::cout << "[DSA-HashTable] Key not found" << std::endl;
        return std::nullopt;
    }
    
    // Access with bounds checking
    V& at(const K& key) {
        return table.at(key);
    }
    
    const V& at(const K& key) const {
        return table.at(key);
    }
    
    // Operator[] for convenient access/insertion
    V& operator[](const K& key) {
        return table[key];
    }
    
    bool remove(const K& key) {
        auto it = table.find(key);
        if (it != table.end()) {
            table.erase(it);
            std::cout << "[DSA-HashTable] Removed key. Table size: " << table.size() << std::endl;
            return true;
        }
        return false;
    }
    
    bool contains(const K& key) const {
        return table.find(key) != table.end();
    }
    
    size_t size() const {
        return table.size();
    }
    
    bool isEmpty() const {
        return table.empty();
    }
    
    // Dynamic capacity features
    void reserve(size_t count) {
        table.reserve(count);
    }
    
    float load_factor() const {
        return table.load_factor();
    }
    
    float max_load_factor() const {
        return table.max_load_factor();
    }
    
    void max_load_factor(float ml) {
        table.max_load_factor(ml);
    }
    
    void clear() {
        table.clear();
        std::cout << "[DSA-HashTable] Cleared hash table" << std::endl;
    }
    
    std::unordered_map<K, V>& getTable() {
        return table;
    }
    
    const std::unordered_map<K, V>& getTable() const {
        return table;
    }
};
