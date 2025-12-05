#pragma once
#include <unordered_map>
#include <vector>
#include <functional>
#include <cmath>

// ═══════════════════════════════════════════════════════════════════════════
// SPATIAL HASH (DSA: Hash Table + Spatial Partitioning)
// Time Complexity: Insert O(1), Query nearby O(k) where k = entities in cell
// Space Complexity: O(n) where n = total entities  
// Used for: Fast collision detection, finding nearby enemies
// ═══════════════════════════════════════════════════════════════════════════

template<typename T>
class SpatialHash {
private:
    int cellSize;
    std::unordered_map<int64_t, std::vector<T>> grid;
    
    // Convert x,y to cell coordinates
    int64_t getCell(int x, int y) const {
        int cellX = x / cellSize;
        int cellY = y / cellSize;
        // Pack into single int64 key
        return (static_cast<int64_t>(cellX) << 32) | (static_cast<int64_t>(cellY) & 0xFFFFFFFF);
    }
    
    std::pair<int, int> getCellCoords(int x, int y) const {
        return {x / cellSize, y / cellSize};
    }

public:
    SpatialHash(int cellSize = 32) : cellSize(cellSize) {}
    
    void clear() {
        grid.clear();
    }
    
    void insert(int x, int y, const T& entity) {
        int64_t cell = getCell(x, y);
        grid[cell].push_back(entity);
    }
    
    void remove(int x, int y, const T& entity) {
        int64_t cell = getCell(x, y);
        auto it = grid.find(cell);
        if (it != grid.end()) {
            auto& vec = it->second;
            vec.erase(std::remove(vec.begin(), vec.end(), entity), vec.end());
            if (vec.empty()) {
                grid.erase(it);
            }
        }
    }
    
    // Get all entities in a specific cell
    std::vector<T> getCellEntities(int cellX, int cellY) const {
        int64_t cell = (static_cast<int64_t>(cellX) << 32) | (static_cast<int64_t>(cellY) & 0xFFFFFFFF);
        auto it = grid.find(cell);
        if (it != grid.end()) {
            return it->second;
        }
        return {};
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // QUERY NEARBY - Get all entities within radius of a point
    // This is the main DSA optimization - O(k) instead of O(n) for collision
    // ═══════════════════════════════════════════════════════════════════════
    std::vector<T> queryNearby(int x, int y, int radius) const {
        std::vector<T> result;
        
        auto [centerCellX, centerCellY] = getCellCoords(x, y);
        int cellRadius = (radius / cellSize) + 1;
        
        // Check all cells within the radius
        for (int dx = -cellRadius; dx <= cellRadius; ++dx) {
            for (int dy = -cellRadius; dy <= cellRadius; ++dy) {
                int64_t cell = (static_cast<int64_t>(centerCellX + dx) << 32) | 
                               (static_cast<int64_t>(centerCellY + dy) & 0xFFFFFFFF);
                auto it = grid.find(cell);
                if (it != grid.end()) {
                    for (const auto& entity : it->second) {
                        result.push_back(entity);
                    }
                }
            }
        }
        
        return result;
    }
    
    // Query with custom position extractor
    template<typename PosFunc>
    std::vector<T> queryNearbyFiltered(int x, int y, int radius, PosFunc getPos) const {
        std::vector<T> nearby = queryNearby(x, y, radius);
        std::vector<T> result;
        
        float radiusSq = static_cast<float>(radius * radius);
        
        for (const auto& entity : nearby) {
            auto [ex, ey] = getPos(entity);
            float dx = static_cast<float>(ex - x);
            float dy = static_cast<float>(ey - y);
            float distSq = dx * dx + dy * dy;
            
            if (distSq <= radiusSq) {
                result.push_back(entity);
            }
        }
        
        return result;
    }
    
    // Update entity position
    void update(int oldX, int oldY, int newX, int newY, const T& entity) {
        if (getCell(oldX, oldY) != getCell(newX, newY)) {
            remove(oldX, oldY, entity);
            insert(newX, newY, entity);
        }
    }
    
    size_t size() const {
        size_t count = 0;
        for (const auto& [key, vec] : grid) {
            count += vec.size();
        }
        return count;
    }
    
    size_t bucketCount() const {
        return grid.size();
    }
};
