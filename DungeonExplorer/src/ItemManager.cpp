// CHANGE: 2025-11-10 - ItemManager implementation with hash table and asset loading
// CHANGE: 2025-12-04 - Refactored to use GameUtils for code deduplication

#include "ItemManager.h"
#include "AssetManager.h"
#include "GameUtils.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

std::unique_ptr<ItemManager> ItemManager::instance = nullptr;

void ItemManager::loadDefaultItems() {
    // CHANGE: 2025-11-14 - Fallback items if JSON loading fails
    std::cout << "[ItemManager] Loading default fallback items..." << std::endl;
    
    ItemNew potion("potion", "Potion", "consumable", DEFAULT_ITEM_RARITY, DEFAULT_POTION_VALUE, 
                   ItemAction("heal", {{"amount", DEFAULT_POTION_HEAL}}));
    ItemNew dagger("dagger_rusty", "Rusty Dagger", "weapon", DEFAULT_ITEM_RARITY, DEFAULT_DAGGER_VALUE);
    ItemNew coin("coin_gold", "Gold Coin", "treasure", DEFAULT_ITEM_RARITY, DEFAULT_COIN_VALUE);
    
    itemDB["potion"] = potion;
    itemDB["dagger_rusty"] = dagger;
    itemDB["coin_gold"] = coin;
    
    std::cout << "[ItemManager] Loaded 3 default fallback items" << std::endl;
}

void ItemManager::loadItems(const std::string& path) {
    std::cout << "[ItemManager] Loading items from " << path << "..." << std::endl;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[WARNING] Failed to open items file: " << path << std::endl;
        std::cerr << "[WARNING] Falling back to default items..." << std::endl;
        loadDefaultItems();  // CHANGE: 2025-11-14 - Error recovery
        return;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        file.close();
        
        if (!j.contains("items") || !j["items"].is_array()) {
            std::cerr << "[WARNING] Invalid items.json format - missing 'items' array" << std::endl;
            std::cerr << "[WARNING] Falling back to default items..." << std::endl;
            loadDefaultItems();  // CHANGE: 2025-11-14 - Error recovery
            return;
        }
        
        // OPTIMIZATION: Pre-allocate hash table capacity for known item count
        size_t itemCount = j["items"].size();
        itemDB.reserve(itemCount);
        std::cout << "[ItemManager] Reserved capacity for " << itemCount << " items" << std::endl;
        
        int loadedCount = 0;
        for (const auto& itemJson : j["items"]) {
            // Validate required fields
            if (!itemJson.contains("id") || !itemJson.contains("name")) {
                std::cerr << "[WARNING] Skipping malformed item entry (missing id/name)" << std::endl;
                continue;
            }
            
            ItemNew item = itemNewFromJson(itemJson);
            
            // Log item info BEFORE move (since move invalidates the source)
            std::string itemId = item.id;
            std::string itemName = item.name;
            std::string rarityName = item.getRarityName();
            std::string iconPath = item.iconPath;
            
            // OPTIMIZATION: Use operator[] for cleaner syntax
            itemDB[itemId] = std::move(item);  // Use move semantics
            
            // Load icon texture into AssetManager
            if (!iconPath.empty()) {
                AssetManager::getInstance().loadTexture(itemId, iconPath);
                std::cout << "[ItemManager] Loaded item: " << itemName 
                          << " (" << rarityName << ") - Icon: " << iconPath << std::endl;
            } else {
                std::cout << "[ItemManager] Loaded item: " << itemName 
                          << " (" << rarityName << ") - No icon" << std::endl;
            }
            
            loadedCount++;
        }
        
        // OPTIMIZATION: Display hash table metrics
        std::cout << "[ItemManager] Successfully loaded " << loadedCount << " items into hash table" << std::endl;
        std::cout << "[ItemManager] Hash table load factor: " << itemDB.load_factor() << " / " << itemDB.max_load_factor() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] JSON parsing failed: " << e.what() << std::endl;
        std::cerr << "[WARNING] Falling back to default items..." << std::endl;
        loadDefaultItems();  // CHANGE: 2025-11-14 - Error recovery
    }
}

ItemNew ItemManager::getItemById(const std::string& id) const {
    auto it = itemDB.find(id);
    if (it != itemDB.end()) {
        return it->second;
    }
    
    std::cerr << "[WARNING] Item not found: " << id << " - returning empty item" << std::endl;
    return ItemNew(); // Return empty item
}

// ═══════════════════════════════════════════════════════════════════════════
// LOOT DROP SYSTEM (DSA: Weighted Random Selection Algorithm)
// Time Complexity: O(n) for item filtering, O(1) for random selection
// ═══════════════════════════════════════════════════════════════════════════

ItemNew ItemManager::getRandomDrop(int floorLevel) const {
    if (itemDB.empty()) {
        std::cout << "[ItemManager] No items in database for drop" << std::endl;
        return ItemNew();
    }
    
    // Calculate rarity weights influenced by floor level
    // Higher floors = better chance for rare items
    float floorBonus = std::min(floorLevel * FLOOR_BONUS_PER_LEVEL, 0.5f);  // Max +50%
    
    // Weighted random rarity selection (DSA: Cumulative Distribution)
    float weights[5] = {
        RARITY_WEIGHT_COMMON * (1.0f - floorBonus),    // Less common items at higher floors
        RARITY_WEIGHT_UNCOMMON,
        RARITY_WEIGHT_RARE * (1.0f + floorBonus * 0.5f),
        RARITY_WEIGHT_EPIC * (1.0f + floorBonus),
        RARITY_WEIGHT_LEGENDARY * (1.0f + floorBonus * 2.0f)  // Much better legendary chance
    };
    
    float totalWeight = weights[0] + weights[1] + weights[2] + weights[3] + weights[4];
    float roll = RandomUtils::randomFloat() * totalWeight;
    
    // Cumulative sum to determine selected rarity
    int selectedRarity = 1;
    float cumulative = 0.f;
    for (int i = 0; i < 5; ++i) {
        cumulative += weights[i];
        if (roll < cumulative) {
            selectedRarity = i + 1;
            break;
        }
    }
    
    std::cout << "[ItemManager] Drop roll: rarity=" << selectedRarity 
              << " (floor " << floorLevel << ", bonus=" << (floorBonus * 100) << "%)" << std::endl;
    
    return getRandomDropByRarity(selectedRarity, floorLevel);
}

ItemNew ItemManager::getRandomDropByType(const std::string& type, int floorLevel) const {
    std::vector<ItemNew> filtered = getItemsByType(type);
    
    if (filtered.empty()) {
        std::cout << "[ItemManager] No items of type '" << type << "'" << std::endl;
        return ItemNew();
    }
    
    // Random selection from filtered list
    size_t idx = RandomUtils::pickIndex(filtered);
    std::cout << "[ItemManager] Dropped " << filtered[idx].name << " (type: " << type << ")" << std::endl;
    return filtered[idx];
}

ItemNew ItemManager::getRandomDropByRarity(int targetRarity, int floorLevel) const {
    std::vector<ItemNew> filtered = getItemsByRarity(targetRarity);
    
    // If no items at target rarity, try adjacent rarities
    if (filtered.empty() && targetRarity > 1) {
        filtered = getItemsByRarity(targetRarity - 1);
    }
    if (filtered.empty() && targetRarity < 5) {
        filtered = getItemsByRarity(targetRarity + 1);
    }
    
    if (filtered.empty()) {
        // Ultimate fallback: any item
        for (const auto& [id, item] : itemDB) {
            filtered.push_back(item);
        }
    }
    
    if (filtered.empty()) {
        return ItemNew();
    }
    
    size_t idx = RandomUtils::pickIndex(filtered);
    std::cout << "[ItemManager] Dropped " << filtered[idx].name 
              << " (rarity: " << filtered[idx].getRarityName() << ")" << std::endl;
    return filtered[idx];
}

// ═══════════════════════════════════════════════════════════════════════════
// LOOT TABLE GENERATION (DSA: Multiple weighted selections with diminishing returns)
// ═══════════════════════════════════════════════════════════════════════════
std::vector<ItemNew> ItemManager::generateLootTable(int enemyLevel, int floorLevel, int maxItems) const {
    std::vector<ItemNew> loot;
    
    // Base drop chance + enemy level bonus + floor bonus
    float dropChance = BASE_DROP_CHANCE + (enemyLevel * 0.02f) + (floorLevel * FLOOR_BONUS_PER_LEVEL);
    dropChance = std::min(dropChance, 0.95f);  // Max 95% drop chance
    
    std::cout << "[ItemManager] Generating loot (enemy lvl " << enemyLevel 
              << ", floor " << floorLevel << ", chance=" << (dropChance * 100) << "%)" << std::endl;
    
    // Generate items with diminishing probability for each additional item
    for (int i = 0; i < maxItems; ++i) {
        float roll = RandomUtils::randomFloat();
        float adjustedChance = dropChance * std::pow(0.5f, i);  // Each additional item is half as likely
        
        if (roll < adjustedChance) {
            ItemNew drop = getRandomDrop(floorLevel);
            if (!drop.id.empty()) {
                loot.push_back(drop);
            }
        }
    }
    
    // Guaranteed gold drop on higher floors
    if (floorLevel >= 3 && loot.empty()) {
        ItemNew goldDrop = getRandomDropByType("treasure", floorLevel);
        if (!goldDrop.id.empty()) {
            loot.push_back(goldDrop);
        }
    }
    
    std::cout << "[ItemManager] Generated " << loot.size() << " item(s) as loot" << std::endl;
    return loot;
}

std::vector<ItemNew> ItemManager::getItemsByType(const std::string& type) const {
    std::vector<ItemNew> result;
    for (const auto& [id, item] : itemDB) {
        if (item.type == type) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<ItemNew> ItemManager::getItemsByRarity(int rarity) const {
    std::vector<ItemNew> result;
    for (const auto& [id, item] : itemDB) {
        if (item.rarity == rarity) {
            result.push_back(item);
        }
    }
    return result;
}
