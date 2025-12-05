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
    
    // ═══════════════════════════════════════════════════════════════════════
    // DEFAULT ITEM CONSTANTS
    // ═══════════════════════════════════════════════════════════════════════
    static constexpr int DEFAULT_POTION_HEAL = 50;
    static constexpr int DEFAULT_POTION_VALUE = 50;
    static constexpr int DEFAULT_DAGGER_VALUE = 10;
    static constexpr int DEFAULT_COIN_VALUE = 100;
    static constexpr int DEFAULT_ITEM_RARITY = 1;
    
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
    
    // CHANGE: 2025-11-14 - Load default fallback items if JSON loading fails
    void loadDefaultItems();
    
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
    
    // ═══════════════════════════════════════════════════════════════════════
    // LOOT DROP SYSTEM (DSA: Weighted Random Selection Algorithm)
    // ═══════════════════════════════════════════════════════════════════════
    
    // Get random item based on weighted probability
    // floorLevel affects drop rates (higher floors = better items)
    ItemNew getRandomDrop(int floorLevel = 1) const;
    
    // Get random item by type (weapon, armor, consumable, etc)
    ItemNew getRandomDropByType(const std::string& type, int floorLevel = 1) const;
    
    // Get random item by rarity (1=Common, 2=Uncommon, 3=Rare, 4=Epic, 5=Legendary)
    ItemNew getRandomDropByRarity(int targetRarity, int floorLevel = 1) const;
    
    // Generate loot table (multiple items) for enemy death
    std::vector<ItemNew> generateLootTable(int enemyLevel, int floorLevel, int maxItems = 3) const;
    
    // Get all items of a specific type
    std::vector<ItemNew> getItemsByType(const std::string& type) const;
    
    // Get all items of a specific rarity
    std::vector<ItemNew> getItemsByRarity(int rarity) const;
    
    // ═══════════════════════════════════════════════════════════════════════
    // DROP RATE CONSTANTS
    // ═══════════════════════════════════════════════════════════════════════
    static constexpr float BASE_DROP_CHANCE = 0.5f;  // 50% base drop chance
    static constexpr float FLOOR_BONUS_PER_LEVEL = 0.05f;  // +5% per floor
    static constexpr float RARITY_WEIGHT_COMMON = 50.0f;
    static constexpr float RARITY_WEIGHT_UNCOMMON = 30.0f;
    static constexpr float RARITY_WEIGHT_RARE = 15.0f;
    static constexpr float RARITY_WEIGHT_EPIC = 4.0f;
    static constexpr float RARITY_WEIGHT_LEGENDARY = 1.0f;
};
