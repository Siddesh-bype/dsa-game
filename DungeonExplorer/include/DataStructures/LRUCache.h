#pragma once
#include <unordered_map>
#include <list>
#include <optional>

// ═══════════════════════════════════════════════════════════════════════════
// LRU CACHE (DSA: Least Recently Used Cache with Hash Map + Doubly Linked List)
// Time Complexity: Get O(1), Put O(1)
// Space Complexity: O(capacity)
// Used for: Texture caching, pathfinding cache, computed value caching
// ═══════════════════════════════════════════════════════════════════════════

template<typename K, typename V>
class LRUCache {
private:
    size_t capacity;
    std::list<std::pair<K, V>> items;  // Doubly linked list for LRU order
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> cache;  // Hash map for O(1) lookup
    
    void moveToFront(typename std::list<std::pair<K, V>>::iterator it) {
        items.splice(items.begin(), items, it);
    }

public:
    LRUCache(size_t cap) : capacity(cap) {}
    
    // Get value (O(1)) - returns nullopt if not found
    std::optional<V> get(const K& key) {
        auto it = cache.find(key);
        if (it == cache.end()) {
            return std::nullopt;
        }
        
        // Move to front (most recently used)
        moveToFront(it->second);
        return it->second->second;
    }
    
    // Get value with default (O(1))
    V getOrDefault(const K& key, const V& defaultValue) {
        auto result = get(key);
        return result.value_or(defaultValue);
    }
    
    // Put value (O(1))
    void put(const K& key, const V& value) {
        auto it = cache.find(key);
        
        if (it != cache.end()) {
            // Update existing value and move to front
            it->second->second = value;
            moveToFront(it->second);
            return;
        }
        
        // Evict if at capacity
        if (items.size() >= capacity) {
            auto& oldest = items.back();
            cache.erase(oldest.first);
            items.pop_back();
        }
        
        // Insert new item at front
        items.emplace_front(key, value);
        cache[key] = items.begin();
    }
    
    // Check if key exists (O(1))
    bool contains(const K& key) const {
        return cache.find(key) != cache.end();
    }
    
    // Remove key (O(1))
    bool remove(const K& key) {
        auto it = cache.find(key);
        if (it != cache.end()) {
            items.erase(it->second);
            cache.erase(it);
            return true;
        }
        return false;
    }
    
    // Clear cache
    void clear() {
        cache.clear();
        items.clear();
    }
    
    // Get current size
    size_t size() const { return items.size(); }
    
    // Get capacity
    size_t getCapacity() const { return capacity; }
    
    // Check if full
    bool isFull() const { return items.size() >= capacity; }
    
    // Get hit rate statistics helper
    float getUtilization() const {
        return capacity > 0 ? static_cast<float>(items.size()) / capacity : 0.f;
    }
    
    // Resize cache (evicts if smaller)
    void resize(size_t newCapacity) {
        while (items.size() > newCapacity) {
            auto& oldest = items.back();
            cache.erase(oldest.first);
            items.pop_back();
        }
        capacity = newCapacity;
    }
};
