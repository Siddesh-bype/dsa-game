// AchievementSystem.h - Enhanced achievement tracking system using HashTable
// CHANGE: 2025-12-03 — Added comprehensive tracking methods and progress monitoring
#pragma once
#include <string>
#include <vector>
#include <ctime>
#include "DataStructures/HashTable.h"

struct Achievement {
    std::string id;
    std::string name;
    std::string description;
    bool unlocked;
    int rewardGold;
    std::string rewardItem;
    std::time_t unlockedTimestamp;
    
    Achievement(const std::string& i = "", const std::string& n = "", 
                const std::string& desc = "", int gold = 0, const std::string& item = "")
        : id(i), name(n), description(desc), unlocked(false), 
          rewardGold(gold), rewardItem(item), unlockedTimestamp(0) {}
};

class AchievementSystem {
public:
    static AchievementSystem& getInstance() {
        static AchievementSystem instance;
        return instance;
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // ACHIEVEMENT MANAGEMENT
    // ═══════════════════════════════════════════════════════════════════════
    void initialize();
    bool unlockAchievement(const std::string& achievementId);
    bool isUnlocked(const std::string& achievementId) const;
    
    // ═══════════════════════════════════════════════════════════════════════
    // COMBAT TRACKING
    // ═══════════════════════════════════════════════════════════════════════
    void trackKill(const std::string& enemyType);
    void trackCriticalHit();
    void trackPerfectKill();
    void trackBossKilledAtLowHP(int remainingHP);
    
    // ═══════════════════════════════════════════════════════════════════════
    // EXPLORATION TRACKING
    // ═══════════════════════════════════════════════════════════════════════
    void trackFloorComplete(int floor);
    void trackRoomVisited();
    void trackChestOpened();
    void trackKeyCollected();
    void trackFloorCompletedNoDamage();
    void trackFloorCompletedNoKills();
    void trackFloorCompletedFast(float timeSeconds);
    
    // ═══════════════════════════════════════════════════════════════════════
    // ITEM & COLLECTION TRACKING
    // ═══════════════════════════════════════════════════════════════════════
    void trackItemCollected(const std::string& itemId);
    void trackGoldCollected(int amount);
    void trackPotionUsed();
    void trackShopPurchase();
    
    // ═══════════════════════════════════════════════════════════════════════
    // SKILL & PROGRESSION TRACKING
    // ═══════════════════════════════════════════════════════════════════════
    void trackSkillUnlocked(const std::string& skillId);
    void trackLevelUp(int newLevel);
    void trackGameSaved();
    
    // ═══════════════════════════════════════════════════════════════════════
    // GETTERS & UTILITY
    // ═══════════════════════════════════════════════════════════════════════
    const Achievement* getAchievement(const std::string& id) const;
    std::vector<Achievement> getAllAchievements() const;
    std::vector<Achievement> getUnlockedAchievements() const;
    int getTotalUnlocked() const;
    float getCompletionPercentage() const;
    void printProgress() const;
    
private:
    AchievementSystem() = default;
    AchievementSystem(const AchievementSystem&) = delete;
    AchievementSystem& operator=(const AchievementSystem&) = delete;
    
    HashTable<std::string, Achievement> achievements;
    
    // ═══════════════════════════════════════════════════════════════════════
    // PROGRESS COUNTERS
    // ═══════════════════════════════════════════════════════════════════════
    
    // Combat stats
    int totalKills = 0;
    int bossKills = 0;
    int criticalHits = 0;
    
    // Exploration stats
    int floorsCompleted = 0;
    int roomsVisited = 0;
    int chestsOpened = 0;
    int keysCollected = 0;
    
    // Collection stats
    int itemsCollected = 0;
    int totalGoldCollected = 0;
    int potionsUsed = 0;
    int shopPurchases = 0;
    
    // Progression stats
    int skillsUnlocked = 0;
    int currentLevel = 1;
    int saveCount = 0;
    
    // Achievement tracking
    int totalUnlockedCount = 0;
};
