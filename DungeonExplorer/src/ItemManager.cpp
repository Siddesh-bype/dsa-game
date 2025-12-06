// ItemManager.cpp - Item database and loot drop system
// CHANGE: 2025-11-10 - ItemManager implementation with hash table and asset loading
// CHANGE: 2025-12-04 - Refactored to use GameUtils for code deduplication
// CHANGE: 2025-12-07 - Removed debug output, improved const correctness

#include "ItemManager.h"
#include "AssetManager.h"
#include "GameUtils.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>

namespace {
    constexpr float MAX_FLOOR_BONUS = 0.5f;           // Maximum +50% bonus from floor level
    constexpr float MAX_DROP_CHANCE = 0.95f;          // Maximum 95% drop chance
    constexpr float ENEMY_LEVEL_DROP_BONUS = 0.02f;   // +2% per enemy level
    constexpr float ADDITIONAL_DROP_FACTOR = 0.5f;    // Each additional item is half as likely
    constexpr int GUARANTEED_GOLD_FLOOR = 3;          // Gold guaranteed on floor 3+
    constexpr int RARITY_COUNT = 5;
}

std::unique_ptr<ItemManager> ItemManager::instance = nullptr;

// ═══════════════════════════════════════════════════════════════════════════
// ITEM LOADING
// ═══════════════════════════════════════════════════════════════════════════

void ItemManager::loadDefaultItems() {
    ItemNew potion("potion", "Potion", "consumable", DEFAULT_ITEM_RARITY, DEFAULT_POTION_VALUE, 
                   ItemAction("heal", {{"amount", DEFAULT_POTION_HEAL}}));
    ItemNew dagger("dagger_rusty", "Rusty Dagger", "weapon", DEFAULT_ITEM_RARITY, DEFAULT_DAGGER_VALUE);
    ItemNew coin("coin_gold", "Gold Coin", "treasure", DEFAULT_ITEM_RARITY, DEFAULT_COIN_VALUE);
    
    itemDB["potion"] = potion;
    itemDB["dagger_rusty"] = dagger;
    itemDB["coin_gold"] = coin;
}

void ItemManager::loadItems(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        loadDefaultItems();
        return;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        file.close();
        
        if (!j.contains("items") || !j["items"].is_array()) {
            loadDefaultItems();
            return;
        }
        
        const size_t itemCount = j["items"].size();
        itemDB.reserve(itemCount);
        
        for (const auto& itemJson : j["items"]) {
            if (!itemJson.contains("id") || !itemJson.contains("name")) {
                continue;
            }
            
            ItemNew item = itemNewFromJson(itemJson);
            const std::string itemId = item.id;
            const std::string iconPath = item.iconPath;
            
            itemDB[itemId] = std::move(item);
            
            if (!iconPath.empty()) {
                AssetManager::getInstance().loadTexture(itemId, iconPath);
            }
        }
    } catch (const std::exception&) {
        loadDefaultItems();
    }
}

ItemNew ItemManager::getItemById(const std::string& id) const {
    const auto it = itemDB.find(id);
    if (it != itemDB.end()) {
        return it->second;
    }
    return ItemNew();
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
    const float floorBonus = std::min(floorLevel * FLOOR_BONUS_PER_LEVEL, MAX_FLOOR_BONUS);
    
    // Weighted random rarity selection (DSA: Cumulative Distribution)
    const float weights[RARITY_COUNT] = {
        RARITY_WEIGHT_COMMON * (1.0f - floorBonus),
        RARITY_WEIGHT_UNCOMMON,
        RARITY_WEIGHT_RARE * (1.0f + floorBonus * 0.5f),
        RARITY_WEIGHT_EPIC * (1.0f + floorBonus),
        RARITY_WEIGHT_LEGENDARY * (1.0f + floorBonus * 2.0f)
    };
    
    float totalWeight = 0.f;
    for (int i = 0; i < RARITY_COUNT; ++i) {
        totalWeight += weights[i];
    }
    const float roll = RandomUtils::randomFloat() * totalWeight;
    
    // Cumulative sum to determine selected rarity
    int selectedRarity = 1;
    float cumulative = 0.f;
    for (int i = 0; i < RARITY_COUNT; ++i) {
        cumulative += weights[i];
        if (roll < cumulative) {
            selectedRarity = i + 1;
            break;
        }
    }
    
    return getRandomDropByRarity(selectedRarity, floorLevel);
}

ItemNew ItemManager::getRandomDropByType(const std::string& type, int floorLevel) const {
    (void)floorLevel;  // Reserved for future use
    const std::vector<ItemNew> filtered = getItemsByType(type);
    
    if (filtered.empty()) {
        return ItemNew();
    }
    
    const size_t idx = RandomUtils::pickIndex(filtered);
    return filtered[idx];
}

ItemNew ItemManager::getRandomDropByRarity(int targetRarity, int floorLevel) const {
    (void)floorLevel;  // Reserved for future use
    std::vector<ItemNew> filtered = getItemsByRarity(targetRarity);
    
    // If no items at target rarity, try adjacent rarities
    if (filtered.empty() && targetRarity > 1) {
        filtered = getItemsByRarity(targetRarity - 1);
    }
    if (filtered.empty() && targetRarity < RARITY_COUNT) {
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
    
    const size_t idx = RandomUtils::pickIndex(filtered);
    return filtered[idx];
}

// ═══════════════════════════════════════════════════════════════════════════
// LOOT TABLE GENERATION (DSA: Multiple weighted selections with diminishing returns)
// ═══════════════════════════════════════════════════════════════════════════
std::vector<ItemNew> ItemManager::generateLootTable(int enemyLevel, int floorLevel, int maxItems) const {
    std::vector<ItemNew> loot;
    
    // Base drop chance + enemy level bonus + floor bonus
    float dropChance = BASE_DROP_CHANCE + (enemyLevel * ENEMY_LEVEL_DROP_BONUS) + (floorLevel * FLOOR_BONUS_PER_LEVEL);
    dropChance = std::min(dropChance, MAX_DROP_CHANCE);
    
    // Generate items with diminishing probability for each additional item
    for (int i = 0; i < maxItems; ++i) {
        const float roll = RandomUtils::randomFloat();
        const float adjustedChance = dropChance * std::pow(ADDITIONAL_DROP_FACTOR, i);
        
        if (roll < adjustedChance) {
            ItemNew drop = getRandomDrop(floorLevel);
            if (!drop.id.empty()) {
                loot.push_back(std::move(drop));
            }
        }
    }
    
    // Guaranteed gold drop on higher floors when no loot dropped
    if (floorLevel >= GUARANTEED_GOLD_FLOOR && loot.empty()) {
        ItemNew goldDrop = getRandomDropByType("treasure", floorLevel);
        if (!goldDrop.id.empty()) {
            loot.push_back(std::move(goldDrop));
        }
    }
    
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
