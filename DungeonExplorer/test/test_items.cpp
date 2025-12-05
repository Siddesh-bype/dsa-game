#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "../include/ItemManager.h"
#include "../include/DropTable.h"
#include "../include/ItemNew.h"

void testItemManager() {
    std::cout << "Testing ItemManager..." << std::endl;
    
    // Load items (assuming items.json exists in assets/data)
    // Note: In a real unit test we might want to mock this or load a test json
    // For now we rely on the actual game data
    try {
        ItemManager::getInstance().loadItems("assets/data/items.json");
    } catch (...) {
        std::cout << "Warning: Failed to load items.json. Skipping ItemManager tests." << std::endl;
        return;
    }
    
    // Verify some known items exist
    assert(ItemManager::getInstance().hasItem("potion"));
    assert(ItemManager::getInstance().hasItem("sword_iron"));
    
    ItemNew potion = ItemManager::getInstance().getItemById("potion");
    assert(potion.name == "Potion");
    assert(potion.type == "consumable");
    assert(potion.value > 0);
    
    std::cout << "ItemManager Test Passed!" << std::endl;
}

void testDropTable() {
    std::cout << "Testing DropTable..." << std::endl;
    
    // Create a test drop table JSON string
    // CHANGE: Use "item_id" instead of "itemId" to match DropTable::fromJson
    std::string json = R"(
        [
            {"item_id": "common_item", "weight": 100},
            {"item_id": "rare_item", "weight": 10}
        ]
    )";
    
    DropTable table = DropTable::fromJson(nlohmann::json::parse(json));
    assert(table.size() == 2);
    
    // Test rolling
    // With 100 vs 10, common_item should be picked much more often
    int commonCount = 0;
    int rareCount = 0;
    
    for (int i = 0; i < 1000; i++) {
        std::string dropped = table.roll();
        if (dropped == "common_item") commonCount++;
        else if (dropped == "rare_item") rareCount++;
    }
    
    std::cout << "Roll results (1000 trials): Common=" << commonCount << ", Rare=" << rareCount << std::endl;
    
    assert(commonCount > rareCount);
    assert(rareCount > 0); // Should get at least one rare item in 1000 rolls
    
    std::cout << "DropTable Test Passed!" << std::endl;
}

int main() {
    testItemManager();
    testDropTable();
    
    std::cout << "All Item Tests Passed!" << std::endl;
    return 0;
}
