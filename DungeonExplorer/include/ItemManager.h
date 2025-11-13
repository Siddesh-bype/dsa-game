// CHANGE: 2025-11-10 - ItemManager using Hash Table for fast item lookup
// Loads items.json and provides O(1) access to item definitions

#pragma once
#include "ItemNew.h"
#include <unordered_map>
#include <string>
#include <memory>

// Singleton ItemManager for global item database access
class ItemManager {
private:
    std::unordered_map<std::string, ItemNew> itemDB;  // Hash Table: id -> Item
    static std::unique_ptr<ItemManager> instance;
    
    ItemManager() = default;

public:
    ItemManager(const ItemManager&) = delete;
    ItemManager& operator=(const ItemManager&) = delete;
    
    // Get singleton instance
    static ItemManager& getInstance() {
        if (!instance) {
            instance = std::unique_ptr<ItemManager>(new ItemManager());
        }
        return *instance;
    }
    
    // Load items from JSON file into hash table
    void loadItems(const std::string& path);
    
    // Get item by ID (O(1) hash table lookup)
    ItemNew getItemById(const std::string& id) const;
    
    // Check if item exists
    bool hasItem(const std::string& id) const {
        return itemDB.find(id) != itemDB.end();
    }
    
    // Get all items (for debugging/inventory display)
    const std::unordered_map<std::string, ItemNew>& getAllItems() const {
        return itemDB;
    }
    
    // Get count of loaded items
    size_t getItemCount() const {
        return itemDB.size();
    }
};
