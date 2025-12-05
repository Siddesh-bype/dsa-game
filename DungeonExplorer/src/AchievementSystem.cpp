// AchievementSystem.cpp - Enhanced achievement tracking with comprehensive game integration
// CHANGE: 2025-12-03 — Refactored with expanded achievements, progress tracking, and better logic

#include "AchievementSystem.h"
#include "GameUtils.h"
#include <iostream>

void AchievementSystem::initialize() {
    std::cout << "[Achievements] Initializing achievement system..." << std::endl;
    
    // ═══════════════════════════════════════════════════════════════════════
    // COMBAT ACHIEVEMENTS
    // ═══════════════════════════════════════════════════════════════════════
    achievements.insert("first_blood", Achievement("first_blood", "First Blood", 
        "Defeat your first enemy", 50, ""));
    achievements.insert("slayer", Achievement("slayer", "Slayer", 
        "Defeat 100 enemies", 200, "health_potion"));
    achievements.insert("genocide", Achievement("genocide", "Genocide", 
        "Defeat 500 enemies", 500, "legendary_sword"));
    achievements.insert("boss_slayer", Achievement("boss_slayer", "Boss Slayer", 
        "Defeat 5 bosses", 300, "boss_key"));
    achievements.insert("boss_master", Achievement("boss_master", "Boss Master", 
        "Defeat 10 bosses", 600, "dragon_scale"));
    achievements.insert("perfect_kill", Achievement("perfect_kill", "Perfect Kill", 
        "Defeat an enemy without taking damage", 100, ""));
    achievements.insert("critical_master", Achievement("critical_master", "Critical Master", 
        "Land 50 critical hits", 250, "crit_ring"));
    
    // ═══════════════════════════════════════════════════════════════════════
    // EXPLORATION ACHIEVEMENTS
    // ═══════════════════════════════════════════════════════════════════════
    achievements.insert("floor_5", Achievement("floor_5", "Halfway There", 
        "Reach floor 5", 100, "potion"));
    achievements.insert("floor_10", Achievement("floor_10", "Dungeon Master", 
        "Complete all 10 floors", 500, "legendary_sword"));
    achievements.insert("explorer", Achievement("explorer", "Explorer", 
        "Visit 100 rooms", 150, ""));
    achievements.insert("treasure_hunter", Achievement("treasure_hunter", "Treasure Hunter", 
        "Open 25 chests", 200, "treasure_map"));
    achievements.insert("key_collector", Achievement("key_collector", "Key Collector", 
        "Collect 10 keys", 100, ""));
    
    // ═══════════════════════════════════════════════════════════════════════
    // ITEM & COLLECTION ACHIEVEMENTS
    // ═══════════════════════════════════════════════════════════════════════
    achievements.insert("collector", Achievement("collector", "Collector", 
        "Collect 50 items", 200, ""));
    achievements.insert("hoarder", Achievement("hoarder", "Hoarder", 
        "Collect 200 items", 400, "bag_of_holding"));
    achievements.insert("gold_digger", Achievement("gold_digger", "Gold Digger", 
        "Collect 1000 gold", 150, ""));
    achievements.insert("millionaire", Achievement("millionaire", "Millionaire", 
        "Collect 10000 gold", 1000, "golden_crown"));
    achievements.insert("potion_master", Achievement("potion_master", "Potion Master", 
        "Use 50 potions", 100, ""));
    
    // ═══════════════════════════════════════════════════════════════════════
    // SKILL & PROGRESSION ACHIEVEMENTS
    // ═══════════════════════════════════════════════════════════════════════
    achievements.insert("skill_master", Achievement("skill_master", "Skill Master", 
        "Unlock 10 skills", 150, ""));
    achievements.insert("skill_god", Achievement("skill_god", "Skill God", 
        "Unlock all skills", 500, "skill_tome"));
    achievements.insert("level_10", Achievement("level_10", "Experienced", 
        "Reach level 10", 200, ""));
    achievements.insert("level_20", Achievement("level_20", "Veteran", 
        "Reach level 20", 400, "veteran_badge"));
    achievements.insert("max_level", Achievement("max_level", "Maximum Power", 
        "Reach maximum level", 1000, "infinity_stone"));
    
    // ═══════════════════════════════════════════════════════════════════════
    // SPECIAL & CHALLENGE ACHIEVEMENTS
    // ═══════════════════════════════════════════════════════════════════════
    achievements.insert("speedrunner", Achievement("speedrunner", "Speedrunner", 
        "Complete floor 1 in under 2 minutes", 300, ""));
    achievements.insert("pacifist", Achievement("pacifist", "Pacifist", 
        "Complete a floor without killing any enemies", 250, ""));
    achievements.insert("no_damage", Achievement("no_damage", "Untouchable", 
        "Complete a floor without taking damage", 400, "shield_of_invincibility"));
    achievements.insert("glass_cannon", Achievement("glass_cannon", "Glass Cannon", 
        "Defeat a boss with 1 HP remaining", 350, ""));
    achievements.insert("merchant", Achievement("merchant", "Merchant", 
        "Buy 20 items from the shop", 150, ""));
    achievements.insert("savior", Achievement("savior", "Savior", 
        "Save the game 10 times", 50, ""));
    
    std::cout << "[Achievements] Initialized with " << achievements.size() << " achievements" << std::endl;
}

bool AchievementSystem::unlockAchievement(const std::string& achievementId) {
    auto achPtr = achievements.get(achievementId);
    if (achPtr && !achPtr->unlocked) {
        achPtr->unlocked = true;
        achPtr->unlockedTimestamp = std::time(nullptr);
        
        // Visual feedback
        std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
        std::cout << "║  🏆 ACHIEVEMENT UNLOCKED! 🏆          ║" << std::endl;
        std::cout << "╠════════════════════════════════════════╣" << std::endl;
        std::cout << "║  " << achPtr->name << std::string(38 - achPtr->name.length(), ' ') << "║" << std::endl;
        std::cout << "║  " << achPtr->description << std::string(38 - achPtr->description.length(), ' ') << "║" << std::endl;
        
        if (achPtr->rewardGold > 0) {
            std::cout << "║  💰 Reward: " << achPtr->rewardGold << " gold" << std::string(24 - std::to_string(achPtr->rewardGold).length(), ' ') << "║" << std::endl;
        }
        if (!achPtr->rewardItem.empty()) {
            std::cout << "║  🎁 Item: " << achPtr->rewardItem << std::string(28 - achPtr->rewardItem.length(), ' ') << "║" << std::endl;
        }
        
        std::cout << "╚════════════════════════════════════════╝\n" << std::endl;
        
        totalUnlockedCount++;
        return true;
    }
    return false;
}

bool AchievementSystem::isUnlocked(const std::string& achievementId) const {
    auto achPtr = achievements.get(achievementId);
    return achPtr && achPtr->unlocked;
}

// ═══════════════════════════════════════════════════════════════════════
// COMBAT TRACKING
// ═══════════════════════════════════════════════════════════════════════

void AchievementSystem::trackKill(const std::string& enemyType) {
    totalKills++;
    
    // Boss tracking
    if (enemyType == "boss" || enemyType == "dragon" || enemyType == "lich") {
        bossKills++;
        if (bossKills >= 5) unlockAchievement("boss_slayer");
        if (bossKills >= 10) unlockAchievement("boss_master");
    }
    
    // General kill milestones
    if (totalKills == 1) unlockAchievement("first_blood");
    if (totalKills >= 100) unlockAchievement("slayer");
    if (totalKills >= 500) unlockAchievement("genocide");
}

void AchievementSystem::trackCriticalHit() {
    criticalHits++;
    if (criticalHits >= 50) {
        unlockAchievement("critical_master");
    }
}

void AchievementSystem::trackPerfectKill() {
    unlockAchievement("perfect_kill");
}

void AchievementSystem::trackBossKilledAtLowHP(int remainingHP) {
    if (remainingHP == 1) {
        unlockAchievement("glass_cannon");
    }
}

// ═══════════════════════════════════════════════════════════════════════
// EXPLORATION TRACKING
// ═══════════════════════════════════════════════════════════════════════

void AchievementSystem::trackFloorComplete(int floor) {
    floorsCompleted = std::max(floorsCompleted, floor);
    
    if (floor >= 5) unlockAchievement("floor_5");
    if (floor >= 10) unlockAchievement("floor_10");
}

void AchievementSystem::trackRoomVisited() {
    roomsVisited++;
    if (roomsVisited >= 100) {
        unlockAchievement("explorer");
    }
}

void AchievementSystem::trackChestOpened() {
    chestsOpened++;
    if (chestsOpened >= 25) {
        unlockAchievement("treasure_hunter");
    }
}

void AchievementSystem::trackKeyCollected() {
    keysCollected++;
    if (keysCollected >= 10) {
        unlockAchievement("key_collector");
    }
}

void AchievementSystem::trackFloorCompletedNoDamage() {
    unlockAchievement("no_damage");
}

void AchievementSystem::trackFloorCompletedNoKills() {
    unlockAchievement("pacifist");
}

void AchievementSystem::trackFloorCompletedFast(float timeSeconds) {
    if (timeSeconds < 120.0f) { // Under 2 minutes
        unlockAchievement("speedrunner");
    }
}

// ═══════════════════════════════════════════════════════════════════════
// ITEM & COLLECTION TRACKING
// ═══════════════════════════════════════════════════════════════════════

void AchievementSystem::trackItemCollected(const std::string& itemId) {
    itemsCollected++;
    
    if (itemsCollected >= 50) unlockAchievement("collector");
    if (itemsCollected >= 200) unlockAchievement("hoarder");
}

void AchievementSystem::trackGoldCollected(int amount) {
    totalGoldCollected += amount;
    
    if (totalGoldCollected >= 1000) unlockAchievement("gold_digger");
    if (totalGoldCollected >= 10000) unlockAchievement("millionaire");
}

void AchievementSystem::trackPotionUsed() {
    potionsUsed++;
    if (potionsUsed >= 50) {
        unlockAchievement("potion_master");
    }
}

void AchievementSystem::trackShopPurchase() {
    shopPurchases++;
    if (shopPurchases >= 20) {
        unlockAchievement("merchant");
    }
}

// ═══════════════════════════════════════════════════════════════════════
// SKILL & PROGRESSION TRACKING
// ═══════════════════════════════════════════════════════════════════════

void AchievementSystem::trackSkillUnlocked(const std::string& skillId) {
    skillsUnlocked++;
    
    if (skillsUnlocked >= 10) unlockAchievement("skill_master");
    // Assuming max skills is around 20-30
    if (skillsUnlocked >= 25) unlockAchievement("skill_god");
}

void AchievementSystem::trackLevelUp(int newLevel) {
    currentLevel = newLevel;
    
    if (newLevel >= 10) unlockAchievement("level_10");
    if (newLevel >= 20) unlockAchievement("level_20");
    if (newLevel >= 50) unlockAchievement("max_level");
}

void AchievementSystem::trackGameSaved() {
    saveCount++;
    if (saveCount >= 10) {
        unlockAchievement("savior");
    }
}

// ═══════════════════════════════════════════════════════════════════════
// GETTERS & UTILITY
// ═══════════════════════════════════════════════════════════════════════

const Achievement* AchievementSystem::getAchievement(const std::string& id) const {
    auto opt = achievements.get(id);
    return opt ? &(*opt) : nullptr;
}

std::vector<Achievement> AchievementSystem::getAllAchievements() const {
    std::vector<Achievement> result;
    // Note: This requires manual tracking of all achievement IDs
    // In a production system, you'd want HashTable to support iteration
    std::vector<std::string> ids = {
        "first_blood", "slayer", "genocide", "boss_slayer", "boss_master",
        "perfect_kill", "critical_master", "floor_5", "floor_10", "explorer",
        "treasure_hunter", "key_collector", "collector", "hoarder", "gold_digger",
        "millionaire", "potion_master", "skill_master", "skill_god", "level_10",
        "level_20", "max_level", "speedrunner", "pacifist", "no_damage",
        "glass_cannon", "merchant", "savior"
    };
    
    for (const auto& id : ids) {
        auto achPtr = achievements.get(id);
        if (achPtr) {
            result.push_back(*achPtr);
        }
    }
    
    return result;
}

std::vector<Achievement> AchievementSystem::getUnlockedAchievements() const {
    std::vector<Achievement> all = getAllAchievements();
    std::vector<Achievement> unlocked;
    
    for (const auto& ach : all) {
        if (ach.unlocked) {
            unlocked.push_back(ach);
        }
    }
    
    return unlocked;
}

int AchievementSystem::getTotalUnlocked() const {
    return totalUnlockedCount;
}

float AchievementSystem::getCompletionPercentage() const {
    int total = getAllAchievements().size();
    if (total == 0) return 0.0f;
    return (static_cast<float>(totalUnlockedCount) / total) * 100.0f;
}

void AchievementSystem::printProgress() const {
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║      ACHIEVEMENT PROGRESS              ║" << std::endl;
    std::cout << "╠════════════════════════════════════════╣" << std::endl;
    std::cout << "║  Unlocked: " << totalUnlockedCount << " / " << getAllAchievements().size() 
              << " (" << static_cast<int>(getCompletionPercentage()) << "%)        ║" << std::endl;
    std::cout << "║                                        ║" << std::endl;
    std::cout << "║  Combat Stats:                         ║" << std::endl;
    std::cout << "║    Total Kills: " << totalKills << std::string(23 - std::to_string(totalKills).length(), ' ') << "║" << std::endl;
    std::cout << "║    Boss Kills: " << bossKills << std::string(24 - std::to_string(bossKills).length(), ' ') << "║" << std::endl;
    std::cout << "║    Critical Hits: " << criticalHits << std::string(20 - std::to_string(criticalHits).length(), ' ') << "║" << std::endl;
    std::cout << "║                                        ║" << std::endl;
    std::cout << "║  Exploration:                          ║" << std::endl;
    std::cout << "║    Floors Completed: " << floorsCompleted << std::string(16 - std::to_string(floorsCompleted).length(), ' ') << "║" << std::endl;
    std::cout << "║    Rooms Visited: " << roomsVisited << std::string(19 - std::to_string(roomsVisited).length(), ' ') << "║" << std::endl;
    std::cout << "║    Chests Opened: " << chestsOpened << std::string(19 - std::to_string(chestsOpened).length(), ' ') << "║" << std::endl;
    std::cout << "║                                        ║" << std::endl;
    std::cout << "║  Collection:                           ║" << std::endl;
    std::cout << "║    Items: " << itemsCollected << std::string(29 - std::to_string(itemsCollected).length(), ' ') << "║" << std::endl;
    std::cout << "║    Gold: " << totalGoldCollected << std::string(30 - std::to_string(totalGoldCollected).length(), ' ') << "║" << std::endl;
    std::cout << "║    Skills: " << skillsUnlocked << std::string(28 - std::to_string(skillsUnlocked).length(), ' ') << "║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝\n" << std::endl;
}
