// CHANGE: 2025-11-10 - ItemManager implementation with hash table and asset loading

#include "ItemManager.h"
#include "AssetManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

std::unique_ptr<ItemManager> ItemManager::instance = nullptr;

void ItemManager::loadItems(const std::string& path) {
    std::cout << "[ItemManager] Loading items from " << path << "..." << std::endl;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Failed to open items file: " << path << std::endl;
        return;
    }
    
    nlohmann::json j;
    file >> j;
    file.close();
    
    if (!j.contains("items") || !j["items"].is_array()) {
        std::cerr << "[ERROR] Invalid items.json format - missing 'items' array" << std::endl;
        return;
    }
    
    int loadedCount = 0;
    for (const auto& itemJson : j["items"]) {
        ItemNew item = itemNewFromJson(itemJson);
        
        // Insert into hash table
        itemDB[item.id] = item;
        
        // Load icon texture into AssetManager
        if (!item.iconPath.empty()) {
            AssetManager::getInstance().loadTexture(item.id, item.iconPath);
            std::cout << "[ItemManager] Loaded item: " << item.name 
                      << " (" << item.getRarityName() << ") - Icon: " << item.iconPath << std::endl;
        } else {
            std::cout << "[ItemManager] Loaded item: " << item.name 
                      << " (" << item.getRarityName() << ") - No icon" << std::endl;
        }
        
        loadedCount++;
    }
    
    std::cout << "[ItemManager] Successfully loaded " << loadedCount << " items into hash table" << std::endl;
}

ItemNew ItemManager::getItemById(const std::string& id) const {
    auto it = itemDB.find(id);
    if (it != itemDB.end()) {
        return it->second;
    }
    
    std::cerr << "[ERROR] Item not found: " << id << std::endl;
    return ItemNew(); // Return empty item
}
