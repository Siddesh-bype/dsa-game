#pragma once
#include <unordered_map>
#include <iostream>
#include <optional>

template<typename K, typename V>
class HashTable {
private:
    std::unordered_map<K, V> table;

public:
    HashTable() = default;
    
    void insert(const K& key, const V& value) {
        table[key] = value;
        std::cout << "[DSA-HashTable] Inserted key. Table size: " << table.size() << std::endl;
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
