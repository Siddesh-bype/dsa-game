// CHANGE: 2025-11-10 - DropTable for weighted random item drops
// Implements probabilistic loot system with configurable drop rates

#pragma once
#include <vector>
#include <string>
#include <random>
#include <nlohmann/json.hpp>

// Single entry in a drop table
struct DropEntry {
    std::string item_id;  // ID of the item to drop
    int weight;           // Drop weight (higher = more likely)
    
    DropEntry() : weight(0) {}
    DropEntry(const std::string& id, int w) : item_id(id), weight(w) {}
};

// Weighted random drop table for enemies
class DropTable {
private:
    std::vector<DropEntry> entries;
    int totalWeight;
    std::mt19937 rng;  // Random number generator
    
public:
    DropTable() : totalWeight(0), rng(std::random_device{}()) {}
    
    // Add a drop entry
    void add(const std::string& item_id, int weight) {
        if (weight > 0) {
            entries.push_back(DropEntry(item_id, weight));
            totalWeight += weight;
        }
    }
    
    // Roll for a random drop (empty string if no drop)
    std::string roll() {
        if (totalWeight == 0 || entries.empty()) {
            return "";  // No drops configured
        }
        
        std::uniform_int_distribution<int> dist(0, totalWeight - 1);
        int r = dist(rng);
        
        for (const auto& entry : entries) {
            if (r < entry.weight) {
                return entry.item_id;
            }
            r -= entry.weight;
        }
        
        return "";  // Should never reach here
    }
    
    // Load from JSON array
    static DropTable fromJson(const nlohmann::json& j) {
        DropTable table;
        
        if (j.is_array()) {
            for (const auto& entry : j) {
                std::string item_id = entry.value("item_id", "");
                int weight = entry.value("weight", 0);
                
                if (!item_id.empty() && weight > 0) {
                    table.add(item_id, weight);
                }
            }
        }
        
        return table;
    }
    
    // Get total weight (for debugging)
    int getTotalWeight() const { return totalWeight; }
    
    // Get number of entries
    size_t size() const { return entries.size(); }
    
    // Clear all entries
    void clear() {
        entries.clear();
        totalWeight = 0;
    }
};
