// CHANGE: 2025-11-10 — 10-Floor Dungeon Expansion System
// DungeonLevelManager: Manages progressive dungeon floors with themes, difficulty, and enemies
// - Loads level configuration from levels.json
// - Handles floor transitions and scaling
// - Manages enemy spawning based on floor
// - Controls visual themes and lighting per floor

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

struct LevelData {
    int floor;
    std::string theme;
    std::string description;
    int roomCount;
    std::vector<std::string> enemyTypes;
    int enemyCount;
    float difficulty;
    sf::Color lightColor;
    float ambientLevel;
    std::string floorTile;
    std::string wallTile;
    std::string music;
    bool boss;
    std::string bossName;
    
    LevelData() : floor(1), roomCount(7), enemyCount(3), difficulty(1.0f), 
                  ambientLevel(0.5f), boss(false) {}
};

class Dungeon;
class EnemyManager;
class Player;

class DungeonLevelManager {
private:
    std::vector<LevelData> levels;
    int currentFloor;
    int maxFloors;
    bool levelsLoaded;
    
    // ═══════════════════════════════════════════════════════════════════════
    // SCALING CONSTANTS - Difficulty progression tuning
    // ═══════════════════════════════════════════════════════════════════════
    static constexpr float HP_SCALE_FACTOR = 1.15f;      // 15% HP increase per floor
    static constexpr float ATTACK_SCALE_FACTOR = 1.12f;  // 12% attack increase per floor
    static constexpr float XP_SCALE_FACTOR = 1.20f;      // 20% XP reward increase per floor
    
    // ═══════════════════════════════════════════════════════════════════════
    // HELPER FUNCTIONS - Reduce code duplication
    // ═══════════════════════════════════════════════════════════════════════
    LevelData createLevel(int floor, const std::string& theme,
                         const std::string& desc, int rooms,
                         const std::vector<std::string>& enemies,
                         int enemyCount, float difficulty,
                         sf::Color light, float ambient,
                         bool isBoss = false,
                         const std::string& bossName = "");
    
    int scaleValue(int baseValue, int floor, float scaleFactor) const;
    
    // Difficulty scaling formulas
    int calculateEnemyHP(int baseHP, int floor) const;
    int calculateEnemyAttack(int baseAttack, int floor) const;
    int calculateXPReward(int baseXP, int floor) const;
    
    // Floor-specific visuals
    void applyFloorTheme(int floor);
    
public:
    DungeonLevelManager();
    
    // Initialization
    bool loadLevels(const std::string& jsonPath);
    void initialize();
    
    // Floor management
    bool advanceFloor();
    void generateLevel(int floor, Dungeon& dungeon, EnemyManager& enemies, Player& player);
    void resetToFloor(int floor);
    
    // Getters
    int getCurrentFloor() const { return currentFloor; }
    int getMaxFloors() const { return maxFloors; }
    const LevelData& getCurrentLevelData() const;
    const LevelData& getLevelData(int floor) const;
    bool isCurrentFloorBoss() const;
    bool isVictoryAchieved() const { return currentFloor > maxFloors; }
    float getDifficultyMultiplier() const;
    
    // Enemy spawning
    void spawnFloorEnemies(EnemyManager& enemies, Dungeon& dungeon, int floor);
    std::string getRandomEnemyType(int floor) const;
    
    // Skill unlocks
    bool shouldUnlockSkill(int floor) const;
    std::string getUnlockedSkillName(int floor) const;
    
    // UI helpers
    std::string getFloorDisplayText() const;
    std::string getFloorDisplayText(int floor) const;  // NEW: Get display text for any floor
    std::string getFloorDescription(int floor) const;  // NEW: Get floor description
    std::string getThemeName() const;
    bool isBossFloor(int floor) const;  // NEW: Check if floor is boss floor
    std::string getBossName(int floor) const;  // NEW: Get boss name for floor
    sf::Color getFloorLightColor() const;
    float getFloorAmbientLevel() const;
};
