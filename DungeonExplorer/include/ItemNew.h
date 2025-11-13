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
    
    ItemNew() : rarity(1), value(0), cooldown(0.0f) {}
    
    ItemNew(const std::string& itemId, const std::string& itemName, 
            const std::string& itemType, int itemRarity, int itemValue,
            const ItemAction& itemAction = ItemAction(), 
            const std::string& icon = "", float cd = 0.0f)
        : id(itemId), name(itemName), type(itemType), rarity(itemRarity), 
          value(itemValue), action(itemAction), iconPath(icon), cooldown(cd) {}
    
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
            case 1: return "Common";
            case 2: return "Uncommon";
            case 3: return "Rare";
            case 4: return "Epic";
            case 5: return "Legendary";
            default: return "Unknown";
        }
    }
    
    // Get rarity color for UI
    sf::Color getRarityColor() const {
        switch(rarity) {
            case 1: return sf::Color(200, 200, 200); // Gray
            case 2: return sf::Color(50, 200, 50);   // Green
            case 3: return sf::Color(50, 100, 255);  // Blue
            case 4: return sf::Color(200, 50, 255);  // Purple
            case 5: return sf::Color(255, 165, 0);   // Orange/Gold
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
    item.rarity = j.value("rarity", 1);
    item.value = j.value("value", 0);
    item.iconPath = j.value("icon", "");
    item.cooldown = j.value("cooldown", 0.0f);
    
    if (j.contains("action")) {
        item.action = itemActionFromJson(j["action"]);
    }
    
    return item;
}
