#pragma once
#include <vector>
#include <memory>
#include <functional>

// ═══════════════════════════════════════════════════════════════════════════
// OBJECT POOL (DSA: Pre-allocated Memory Pool)
// Time Complexity: Acquire O(1), Release O(1)
// Space Complexity: O(n) pre-allocated
// Used for: Particles, projectiles, effects - avoids new/delete overhead
// ═══════════════════════════════════════════════════════════════════════════

template<typename T>
class ObjectPool {
private:
    std::vector<T> pool;
    std::vector<bool> inUse;
    size_t poolSize;
    size_t activeCount;
    std::function<void(T&)> resetFunc;

public:
    ObjectPool(size_t size, std::function<void(T&)> resetFn = nullptr) 
        : poolSize(size), activeCount(0), resetFunc(resetFn) {
        pool.resize(size);
        inUse.resize(size, false);
    }
    
    // Acquire an object from the pool (O(1) amortized)
    T* acquire() {
        // Find first available object
        for (size_t i = 0; i < poolSize; ++i) {
            if (!inUse[i]) {
                inUse[i] = true;
                activeCount++;
                if (resetFunc) resetFunc(pool[i]);
                return &pool[i];
            }
        }
        
        // Pool exhausted - expand (amortized O(1))
        size_t oldSize = poolSize;
        poolSize *= 2;
        pool.resize(poolSize);
        inUse.resize(poolSize, false);
        
        inUse[oldSize] = true;
        activeCount++;
        if (resetFunc) resetFunc(pool[oldSize]);
        return &pool[oldSize];
    }
    
    // Release object back to pool (O(1))
    void release(T* obj) {
        // Find object in pool
        for (size_t i = 0; i < pool.size(); ++i) {
            if (&pool[i] == obj && inUse[i]) {
                inUse[i] = false;
                activeCount--;
                return;
            }
        }
    }
    
    // Release by index (O(1))
    void releaseByIndex(size_t index) {
        if (index < poolSize && inUse[index]) {
            inUse[index] = false;
            activeCount--;
        }
    }
    
    // Clear all objects
    void clear() {
        std::fill(inUse.begin(), inUse.end(), false);
        activeCount = 0;
    }
    
    // Iterate over active objects
    template<typename Func>
    void forEach(Func fn) {
        for (size_t i = 0; i < pool.size(); ++i) {
            if (inUse[i]) {
                fn(pool[i], i);
            }
        }
    }
    
    // Remove objects that match predicate
    template<typename Pred>
    void removeIf(Pred predicate) {
        for (size_t i = 0; i < pool.size(); ++i) {
            if (inUse[i] && predicate(pool[i])) {
                inUse[i] = false;
                activeCount--;
            }
        }
    }
    
    size_t getActiveCount() const { return activeCount; }
    size_t getPoolSize() const { return poolSize; }
    float getUtilization() const { 
        return poolSize > 0 ? static_cast<float>(activeCount) / poolSize : 0.f; 
    }
    
    // Direct access for iteration
    T& operator[](size_t index) { return pool[index]; }
    const T& operator[](size_t index) const { return pool[index]; }
    bool isActive(size_t index) const { return index < poolSize && inUse[index]; }
};
