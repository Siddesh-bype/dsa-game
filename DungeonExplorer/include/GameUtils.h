#pragma once
#include "DataStructures/AStar.h"
#include "DataStructures/LRUCache.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <algorithm>

// ═══════════════════════════════════════════════════════════════════════════
// PATHFINDING CACHE (DSA: A* + LRU Cache combination)
// Caches recent pathfinding results to avoid recomputation
// Time Complexity: Cache hit O(1), Cache miss O(E log V)
// ═══════════════════════════════════════════════════════════════════════════

class PathCache {
private:
    static constexpr int CACHE_SIZE = 100;
    mutable LRUCache<std::string, std::vector<PathNode>> cache;
    int gridWidth;
    int gridHeight;
    
    std::string makeKey(int sx, int sy, int gx, int gy) const {
        return std::to_string(sx) + "," + std::to_string(sy) + "->" + 
               std::to_string(gx) + "," + std::to_string(gy);
    }

public:
    PathCache(int width, int height) 
        : cache(CACHE_SIZE), gridWidth(width), gridHeight(height) {}
    
    // Find path with caching
    std::vector<PathNode> findPath(
        int startX, int startY, 
        int goalX, int goalY,
        std::function<bool(int, int)> isWalkable
    ) {
        std::string key = makeKey(startX, startY, goalX, goalY);
        
        // Check cache first (O(1))
        auto cached = cache.get(key);
        if (cached.has_value()) {
            return cached.value();
        }
        
        // Cache miss - compute path (O(E log V))
        AStarPathfinder pathfinder(gridWidth, gridHeight, isWalkable);
        auto path = pathfinder.findPath({startX, startY}, {goalX, goalY});
        
        // Store in cache
        cache.put(key, path);
        
        return path;
    }
    
    // Get just the next step (most common use case)
    PathNode getNextStep(
        int startX, int startY,
        int goalX, int goalY,
        std::function<bool(int, int)> isWalkable
    ) {
        auto path = findPath(startX, startY, goalX, goalY, isWalkable);
        
        // Return next step (skip current position)
        if (path.size() > 1) {
            return path[1];
        }
        
        // No path found - return current position
        return {startX, startY};
    }
    
    // Clear cache (call when dungeon changes)
    void clear() {
        cache.clear();
    }
    
    // Get cache stats
    size_t getCacheSize() const { return cache.size(); }
    float getCacheUtilization() const { return cache.getUtilization(); }
};

// ═══════════════════════════════════════════════════════════════════════════
// INPUT DEBOUNCE (DSA: Timer-based rate limiting)
// Prevents input spam and accidental double-clicks
// ═══════════════════════════════════════════════════════════════════════════

class InputDebounce {
private:
    float cooldowns[256] = {0};  // Cooldown per key
    float defaultCooldown;
    
public:
    InputDebounce(float cooldown = 0.1f) : defaultCooldown(cooldown) {}
    
    // Check if action is allowed (O(1))
    bool canTrigger(int keyCode) const {
        if (keyCode < 0 || keyCode >= 256) return false;
        return cooldowns[keyCode] <= 0.f;
    }
    
    // Trigger action and start cooldown (O(1))
    void trigger(int keyCode, float customCooldown = -1.f) {
        if (keyCode < 0 || keyCode >= 256) return;
        cooldowns[keyCode] = (customCooldown >= 0.f) ? customCooldown : defaultCooldown;
    }
    
    // Update all cooldowns (call each frame)
    void update(float deltaTime) {
        for (int i = 0; i < 256; ++i) {
            if (cooldowns[i] > 0.f) {
                cooldowns[i] -= deltaTime;
            }
        }
    }
    
    // Check and trigger in one call (most common use)
    bool tryTrigger(int keyCode, float customCooldown = -1.f) {
        if (canTrigger(keyCode)) {
            trigger(keyCode, customCooldown);
            return true;
        }
        return false;
    }
    
    void reset() {
        for (int i = 0; i < 256; ++i) {
            cooldowns[i] = 0.f;
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════
// DISTANCE ALGORITHMS (DSA: Common distance calculations)
// ═══════════════════════════════════════════════════════════════════════════

namespace DistanceAlgorithms {
    // Manhattan distance (grid-based)
    inline int manhattan(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }
    
    // Chebyshev distance (allows diagonal)
    inline int chebyshev(int x1, int y1, int x2, int y2) {
        return std::max(std::abs(x1 - x2), std::abs(y1 - y2));
    }
    
    // Euclidean distance squared (avoids sqrt)
    inline int euclideanSq(int x1, int y1, int x2, int y2) {
        int dx = x1 - x2;
        int dy = y1 - y2;
        return dx * dx + dy * dy;
    }
    
    // Euclidean distance (actual)
    inline float euclidean(int x1, int y1, int x2, int y2) {
        return std::sqrt(static_cast<float>(euclideanSq(x1, y1, x2, y2)));
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// RANDOM UTILITIES (Consolidates std::rand() patterns across codebase)
// Reduces 35+ duplicate random implementations to single source
// ═══════════════════════════════════════════════════════════════════════════

namespace RandomUtils {
    // Random float 0.0 to 1.0
    inline float randomFloat() {
        return static_cast<float>(std::rand()) / RAND_MAX;
    }
    
    // Random float in range [min, max]
    inline float randomFloat(float min, float max) {
        return min + randomFloat() * (max - min);
    }
    
    // Random int in range [min, max] inclusive
    inline int randomInt(int min, int max) {
        if (max <= min) return min;
        return min + (std::rand() % (max - min + 1));
    }
    
    // Random chance check (percent 0-100)
    inline bool chance(int percent) {
        return (std::rand() % 100) < percent;
    }
    
    // Random chance check (probability 0.0-1.0)
    inline bool probability(float prob) {
        return randomFloat() < prob;
    }
    
    // Pick random element from vector (returns index)
    template<typename T>
    inline size_t pickIndex(const std::vector<T>& vec) {
        if (vec.empty()) return 0;
        return std::rand() % vec.size();
    }
    
    // Pick random element from vector (returns copy)
    template<typename T>
    inline T pick(const std::vector<T>& vec, const T& defaultVal = T()) {
        if (vec.empty()) return defaultVal;
        return vec[std::rand() % vec.size()];
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// MATH UTILITIES (Common math operations used across rendering/animation)
// ═══════════════════════════════════════════════════════════════════════════

namespace MathUtils {
    // Linear interpolation
    inline float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
    
    // Clamp value between min and max
    inline float clamp(float value, float min, float max) {
        return std::max(min, std::min(value, max));
    }
    
    inline int clamp(int value, int min, int max) {
        return std::max(min, std::min(value, max));
    }
    
    // Smooth interpolation (ease in-out)
    inline float smoothstep(float t) {
        t = clamp(t, 0.f, 1.f);
        return t * t * (3.f - 2.f * t);
    }
    
    // Percentage calculation
    inline float percent(float value, float max) {
        return max > 0.f ? value / max : 0.f;
    }
    
    inline float percent(int value, int max) {
        return max > 0 ? static_cast<float>(value) / max : 0.f;
    }
    
    // Sign of value (-1, 0, or 1)
    inline int sign(int value) {
        return (value > 0) - (value < 0);
    }
    
    inline float sign(float value) {
        return (value > 0.f) ? 1.f : ((value < 0.f) ? -1.f : 0.f);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// COLOR UTILITIES (Consolidates sf::Color creation patterns)
// ═══════════════════════════════════════════════════════════════════════════

namespace ColorUtils {
    // Create color with alpha
    inline sf::Color withAlpha(const sf::Color& color, uint8_t alpha) {
        return sf::Color(color.r, color.g, color.b, alpha);
    }
    
    // Create grayscale color
    inline sf::Color gray(uint8_t value, uint8_t alpha = 255) {
        return sf::Color(value, value, value, alpha);
    }
    
    // Lerp between two colors
    inline sf::Color lerp(const sf::Color& a, const sf::Color& b, float t) {
        return sf::Color(
            static_cast<uint8_t>(a.r + (b.r - a.r) * t),
            static_cast<uint8_t>(a.g + (b.g - a.g) * t),
            static_cast<uint8_t>(a.b + (b.b - a.b) * t),
            static_cast<uint8_t>(a.a + (b.a - a.a) * t)
        );
    }
    
    // Common game colors (avoid magic numbers)
    namespace Palette {
        inline sf::Color healthGreen() { return sf::Color(100, 255, 100); }
        inline sf::Color damageRed() { return sf::Color(255, 50, 50); }
        inline sf::Color manaBlue() { return sf::Color(100, 100, 255); }
        inline sf::Color goldYellow() { return sf::Color(255, 215, 0); }
        inline sf::Color warningOrange() { return sf::Color(255, 150, 50); }
        inline sf::Color uiBackground() { return sf::Color(20, 20, 30, 210); }
    }
}
