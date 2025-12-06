// CHANGE: 2025-11-10 - Item-drop system with loot tables and item metadata
// Complete item system with actions (heal, equip, buff), rarity, and effects

#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

// Represents an action/effect that an item can perform
struct ItemAction {
    std::string kind;         // "heal", "equip", "buff", "teleport", "use"
    nlohmann::json params;    // Flexible parameters (amount, attack_bonus, etc.)
    
    ItemAction() : kind("none") {}
    ItemAction(const std::string& k, const nlohmann::json& p = {}) 
        : kind(k), params(p) {}
};

// ═══════════════════════════════════════════════════════════════════════
// ITEM SYSTEM CONSTANTS
// ═══════════════════════════════════════════════════════════════════════

// Rarity levels
static constexpr int RARITY_COMMON = 1;
static constexpr int RARITY_UNCOMMON = 2;
static constexpr int RARITY_RARE = 3;
static constexpr int RARITY_EPIC = 4;
static constexpr int RARITY_LEGENDARY = 5;

// Default values
static constexpr int DEFAULT_RARITY = RARITY_COMMON;
static constexpr int DEFAULT_VALUE = 0;
static constexpr float DEFAULT_COOLDOWN = 0.0f;
static constexpr int MAX_STACK_SIZE = 64;  // CHANGE: 2025-12-06 - Max items per stack

// Rarity colors (RGB)
namespace RarityColors {
    static constexpr int COMMON_R = 200, COMMON_G = 200, COMMON_B = 200;       // Gray
    static constexpr int UNCOMMON_R = 50, UNCOMMON_G = 200, UNCOMMON_B = 50;   // Green
    static constexpr int RARE_R = 50, RARE_G = 100, RARE_B = 255;              // Blue
    static constexpr int EPIC_R = 200, EPIC_G = 50, EPIC_B = 255;              // Purple
    static constexpr int LEGENDARY_R = 255, LEGENDARY_G = 165, LEGENDARY_B = 0; // Orange/Gold
}

// Complete item definition with all metadata
struct ItemNew {
    std::string id;           // Unique identifier (e.g., "potion_small")
    std::string name;         // Display name (e.g., "Small Potion")
    std::string type;         // "consumable", "weapon", "armor", "quest"
    int rarity;               // 1=common, 2=uncommon, 3=rare, 4=epic, 5=legendary
    int value;                // Gold value or score
    ItemAction action;        // What the item does when used
    std::string iconPath;     // Path to icon texture
    float cooldown;           // Cooldown in seconds (for active items)
    int stackCount = 1;       // CHANGE: 2025-12-06 - Number of items in stack (max 64)
    
    ItemNew() : rarity(DEFAULT_RARITY), value(DEFAULT_VALUE), cooldown(DEFAULT_COOLDOWN), stackCount(1) {}
    
    ItemNew(const std::string& itemId, const std::string& itemName, 
            const std::string& itemType, int itemRarity, int itemValue,
            const ItemAction& itemAction = ItemAction(), 
            const std::string& icon = "", float cd = 0.0f, int stack = 1)
        : id(itemId), name(itemName), type(itemType), rarity(itemRarity), 
          value(itemValue), action(itemAction), iconPath(icon), cooldown(cd), stackCount(stack) {}
    
    // Comparison for priority queue (heap) - higher rarity = higher priority
    bool operator<(const ItemNew& other) const {
        return rarity < other.rarity;
    }
    
    bool operator>(const ItemNew& other) const {
        return rarity > other.rarity;
    }
    
    // Equality operators for LinkedList operations
    bool operator==(const ItemNew& other) const {
        return id == other.id;
    }
    
    bool operator!=(const ItemNew& other) const {
        return id != other.id;
    }
    
    // Get rarity name for display
    std::string getRarityName() const {
        switch(rarity) {
            case RARITY_COMMON: return "Common";
            case RARITY_UNCOMMON: return "Uncommon";
            case RARITY_RARE: return "Rare";
            case RARITY_EPIC: return "Epic";
            case RARITY_LEGENDARY: return "Legendary";
            default: return "Unknown";
        }
    }
    
    // Get rarity color for UI
    sf::Color getRarityColor() const {
        switch(rarity) {
            case RARITY_COMMON: return sf::Color(RarityColors::COMMON_R, RarityColors::COMMON_G, RarityColors::COMMON_B);
            case RARITY_UNCOMMON: return sf::Color(RarityColors::UNCOMMON_R, RarityColors::UNCOMMON_G, RarityColors::UNCOMMON_B);
            case RARITY_RARE: return sf::Color(RarityColors::RARE_R, RarityColors::RARE_G, RarityColors::RARE_B);
            case RARITY_EPIC: return sf::Color(RarityColors::EPIC_R, RarityColors::EPIC_G, RarityColors::EPIC_B);
            case RARITY_LEGENDARY: return sf::Color(RarityColors::LEGENDARY_R, RarityColors::LEGENDARY_G, RarityColors::LEGENDARY_B);
            default: return sf::Color::White;
        }
    }
};

// Load ItemAction from JSON
inline ItemAction itemActionFromJson(const nlohmann::json& j) {
    ItemAction action;
    if (j.contains("kind")) {
        action.kind = j["kind"].get<std::string>();
    }
    action.params = j;
    return action;
}

// Load ItemNew from JSON
inline ItemNew itemNewFromJson(const nlohmann::json& j) {
    ItemNew item;
    item.id = j.value("id", "");
    item.name = j.value("name", "Unknown Item");
    item.type = j.value("type", "misc");
    item.rarity = j.value("rarity", DEFAULT_RARITY);
    item.value = j.value("value", DEFAULT_VALUE);
    item.iconPath = j.value("icon", "");
    item.cooldown = j.value("cooldown", DEFAULT_COOLDOWN);
    
    if (j.contains("action")) {
        item.action = itemActionFromJson(j["action"]);
    }
    
    return item;
}
