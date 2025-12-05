// CHANGE: 2025-11-10 — 10-Floor Dungeon Expansion System Implementation
// Manages progressive dungeon generation with scaling difficulty and themed floors
// CHANGE: 2025-12-04 - Refactored with GameUtils for code deduplication

#include "DungeonLevelManager.h"
#include "Dungeon.h"
#include "Enemy.h"
#include "Player.h"
#include "GameUtils.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

// Enemy data cache
static nlohmann::json enemyDatabase;
static bool enemyDbLoaded = false;

DungeonLevelManager::DungeonLevelManager() 
    : currentFloor(1), maxFloors(10), levelsLoaded(false) {
    // Load enemy database on first use
    if (!enemyDbLoaded) {
        std::ifstream file("assets/data/enemies.json");
        if (file.is_open()) {
            file >> enemyDatabase;
            file.close();
            enemyDbLoaded = true;
            std::cout << "[DungeonLevelManager] Loaded " << enemyDatabase.size() << " enemy types from database" << std::endl;
        } else {
            std::cerr << "[ERROR] Failed to load assets/data/enemies.json" << std::endl;
        }
    }
}

bool DungeonLevelManager::loadLevels(const std::string& jsonPath) {
    std::cout << "[DungeonLevelManager] Loading level configuration..." << std::endl;
    
    levels.clear();
    
    // ═══════════════════════════════════════════════════════════════════════
    // LEVEL DATA - Using factory pattern for cleaner code
    // ═══════════════════════════════════════════════════════════════════════
    
    levels.push_back(createLevel(1, "Cavern Entrance", 
        "A dark cave entrance filled with weak creatures",
        10, {"Goblin", "Slime"}, 3, 1.0f,
        sf::Color(255, 220, 180, 200), 0.5f));
    
    levels.push_back(createLevel(2, "Ruined Halls",
        "Ancient corridors filled with undead",
        12, {"Orc", "Skeleton"}, 4, 1.3f,
        sf::Color(230, 210, 180, 200), 0.45f));
    
    levels.push_back(createLevel(3, "Undercity",
        "A forgotten underground city",
        14, {"Goblin", "Skeleton", "Orc"}, 5, 1.6f,
        sf::Color(180, 200, 220, 180), 0.4f));
    
    levels.push_back(createLevel(4, "Shadow Temple",
        "A temple consumed by darkness",
        16, {"Wraith", "Skeleton", "Dark Mage"}, 6, 2.0f,
        sf::Color(200, 150, 150, 160), 0.35f));
    
    levels.push_back(createLevel(5, "Abyss Core",
        "The heart of the abyss - BOSS FLOOR",
        8, {"Dragon"}, 1, 2.5f,
        sf::Color(255, 100, 100, 180), 0.3f,
        true, "Abyss Dragon"));
    
    levels.push_back(createLevel(6, "Crystal Mines",
        "Glittering caverns filled with golems",
        18, {"Gargoyle", "Goblin", "Wraith"}, 7, 2.8f,
        sf::Color(150, 200, 255, 200), 0.4f));
    
    levels.push_back(createLevel(7, "Forgotten Fortress",
        "An ancient military stronghold",
        20, {"Orc", "Wraith", "Goblin"}, 8, 3.2f,
        sf::Color(200, 200, 200, 160), 0.35f));
    
    levels.push_back(createLevel(8, "Lava Catacombs",
        "Burning halls of molten rock",
        22, {"Demon", "Demon", "Demon"}, 9, 3.8f,
        sf::Color(255, 180, 120, 200), 0.45f));
    
    levels.push_back(createLevel(9, "Obsidian Keep",
        "A fortress of volcanic glass",
        24, {"Dark Mage", "Orc", "Skeleton"}, 10, 4.3f,
        sf::Color(120, 80, 80, 180), 0.25f));
    
    levels.push_back(createLevel(10, "The Nexus",
        "The final confrontation - FINAL BOSS",
        12, {"Dragon"}, 1, 5.0f,
        sf::Color(255, 255, 255, 150), 0.2f,
        true, "Eternal Dragon"));
    
    maxFloors = levels.size();
    levelsLoaded = true;
    
    std::cout << "[DungeonLevelManager] Loaded " << maxFloors << " floor configurations" << std::endl;
    return true;
}

void DungeonLevelManager::initialize() {
    currentFloor = 1;
    std::cout << "[DungeonLevelManager] Initialized at Floor 1" << std::endl;
}

// ═══════════════════════════════════════════════════════════════════════
// HELPER FUNCTIONS - Reduce code duplication
// ═══════════════════════════════════════════════════════════════════════

LevelData DungeonLevelManager::createLevel(int floor, const std::string& theme,
                                          const std::string& desc, int rooms,
                                          const std::vector<std::string>& enemies,
                                          int enemyCount, float difficulty,
                                          sf::Color light, float ambient,
                                          bool isBoss,
                                          const std::string& bossName) {
    LevelData level;
    level.floor = floor;
    level.theme = theme;
    level.description = desc;
    level.roomCount = rooms;
    level.enemyTypes = enemies;
    level.enemyCount = enemyCount;
    level.difficulty = difficulty;
    level.lightColor = light;
    level.ambientLevel = ambient;
    level.boss = isBoss;
    level.bossName = bossName;
    return level;
}

int DungeonLevelManager::scaleValue(int baseValue, int floor, float scaleFactor) const {
    return static_cast<int>(baseValue * std::pow(scaleFactor, floor - 1));
}

int DungeonLevelManager::calculateEnemyHP(int baseHP, int floor) const {
    return scaleValue(baseHP, floor, HP_SCALE_FACTOR);
}

int DungeonLevelManager::calculateEnemyAttack(int baseAttack, int floor) const {
    return scaleValue(baseAttack, floor, ATTACK_SCALE_FACTOR);
}

int DungeonLevelManager::calculateXPReward(int baseXP, int floor) const {
    // Formula: XP = baseXP * floor * difficulty
    return static_cast<int>(baseXP * floor * getDifficultyMultiplier());
}

const LevelData& DungeonLevelManager::getCurrentLevelData() const {
    if (currentFloor > 0 && currentFloor <= static_cast<int>(levels.size())) {
        return levels[currentFloor - 1];
    }
    return levels[0]; // Fallback to floor 1
}

const LevelData& DungeonLevelManager::getLevelData(int floor) const {
    if (floor > 0 && floor <= static_cast<int>(levels.size())) {
        return levels[floor - 1];
    }
    return levels[0];
}

bool DungeonLevelManager::isCurrentFloorBoss() const {
    return getCurrentLevelData().boss;
}

float DungeonLevelManager::getDifficultyMultiplier() const {
    return getCurrentLevelData().difficulty;
}

bool DungeonLevelManager::advanceFloor() {
    if (currentFloor >= maxFloors) {
        std::cout << "[DungeonLevelManager] Victory! All floors completed!" << std::endl;
        return false; // No more floors
    }
    
    currentFloor++;
    std::cout << "[DungeonLevelManager] Advanced to Floor " << currentFloor << ": " 
              << getCurrentLevelData().theme << std::endl;
    
    if (isCurrentFloorBoss()) {
        std::cout << "[DungeonLevelManager] WARNING: BOSS FLOOR - " 
                  << getCurrentLevelData().bossName << std::endl;
    }
    
    return true;
}

void DungeonLevelManager::generateLevel(int floor, Dungeon& dungeon, EnemyManager& enemies, Player& player) {
    const LevelData& data = getLevelData(floor);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "   FLOOR " << floor << ": " << data.theme << std::endl;
    std::cout << "   Difficulty: x" << data.difficulty << std::endl;
    if (data.boss) {
        std::cout << "   *** BOSS FLOOR: " << data.bossName << " ***" << std::endl;
    }
    std::cout << "========================================\n" << std::endl;
    
    // Generate dungeon with floor-specific room count
    dungeon.generate(data.roomCount);
    
    // Spawn enemies for this floor
    spawnFloorEnemies(enemies, dungeon, floor);
    
    // Apply visual theme (lighting adjustment will happen in Renderer)
    std::cout << "[DungeonLevelManager] Applied theme: " << data.theme << std::endl;
}

void DungeonLevelManager::spawnFloorEnemies(EnemyManager& enemies, Dungeon& dungeon, int floor) {
    const LevelData& data = getLevelData(floor);
    std::srand(static_cast<unsigned>(std::time(nullptr)) + floor);
    
    const auto& rooms = dungeon.getRooms();
    if (rooms.empty()) return;
    
    std::cout << "[DungeonLevelManager] Spawning " << data.enemyCount << " enemies for floor " << floor << std::endl;
    
    for (int i = 0; i < data.enemyCount && i < static_cast<int>(rooms.size()); i++) {
        // Skip first room (player spawn)
        int roomIndex = 1 + (std::rand() % (rooms.size() - 1));
        const auto& room = rooms[roomIndex];
        
        int enemyX = room.x + 1 + (std::rand() % (room.width - 2));
        int enemyY = room.y + 1 + (std::rand() % (room.height - 2));
        
        // Pick random enemy type for this floor
        std::string enemyType = data.enemyTypes[std::rand() % data.enemyTypes.size()];
        
        // Calculate scaled stats
        int baseHP = 50;
        int baseDamage = 10;
        int range = 1;
        nlohmann::json dropTable = nlohmann::json::array();
        
        // Try to load enemy data from database
        if (enemyDbLoaded && !enemyDatabase.empty()) {
            for (const auto& enemyData : enemyDatabase) {
                if (enemyData.contains("name") && enemyData["name"].get<std::string>() == enemyType) {
                    // Found matching enemy in database
                    if (enemyData.contains("hp")) baseHP = enemyData["hp"].get<int>();
                    if (enemyData.contains("attack")) baseDamage = enemyData["attack"].get<int>();
                    if (enemyData.contains("range")) range = enemyData["range"].get<int>();
                    if (enemyData.contains("drop_table")) {
                        dropTable = enemyData["drop_table"];
                        std::cout << "[DungeonLevelManager] Loaded drop table for " << enemyType 
                                  << " with " << dropTable.size() << " entries" << std::endl;
                    }
                    break;
                }
            }
        }
        
        // Boss adjustments
        if (data.boss) {
            baseHP = 200;
            baseDamage = 25;
            range = 2;
        }
        
        int scaledHP = calculateEnemyHP(baseHP, floor);
        int scaledDamage = calculateEnemyAttack(baseDamage, floor);
        
        std::string type = (range > 1) ? "boss" : ((std::rand() % 3 == 0) ? "ranged" : "melee");
        
        // Spawn with drop table if available
        if (!dropTable.empty()) {
            enemies.spawnEnemyWithDrops(enemyType, type, enemyX, enemyY, scaledHP, scaledDamage, range, 1.0f, floor, dropTable);
        } else {
            enemies.spawnEnemy(enemyType, type, enemyX, enemyY, scaledHP, scaledDamage, range, 1.0f, floor);
        }
        
        std::cout << "[Floor " << floor << "] Spawned " << enemyType << " (HP: " << scaledHP 
                  << ", DMG: " << scaledDamage << ") at (" << enemyX << ", " << enemyY << ")" << std::endl;
    }
}

std::string DungeonLevelManager::getRandomEnemyType(int floor) const {
    const LevelData& data = getLevelData(floor);
    if (data.enemyTypes.empty()) return "Enemy";
    return data.enemyTypes[std::rand() % data.enemyTypes.size()];
}

bool DungeonLevelManager::shouldUnlockSkill(int floor) const {
    // Unlock skills at floors 2, 4, 6, 8, 10
    return (floor % 2 == 0 && floor <= 10);
}

std::string DungeonLevelManager::getUnlockedSkillName(int floor) const {
    switch (floor) {
        case 2: return "Healing Light";
        case 4: return "Flame Wave";
        case 6: return "Shadow Step";
        case 8: return "Divine Barrier";
        case 10: return "Soul Strike";
        default: return "";
    }
}

std::string DungeonLevelManager::getFloorDisplayText() const {
    const LevelData& data = getCurrentLevelData();
    return "Floor " + std::to_string(currentFloor) + "/" + std::to_string(maxFloors) + 
           ": " + data.theme;
}

std::string DungeonLevelManager::getFloorDisplayText(int floor) const {
    const LevelData& data = getLevelData(floor);
    return "Floor " + std::to_string(floor) + "/" + std::to_string(maxFloors) + 
           ": " + data.theme;
}

std::string DungeonLevelManager::getFloorDescription(int floor) const {
    const LevelData& data = getLevelData(floor);
    std::string desc = "A " + data.theme + " awaits... ";
    desc += "Difficulty: " + std::to_string(static_cast<int>(data.difficulty * 100)) + "% | ";
    desc += "Rooms: " + std::to_string(data.roomCount) + " | ";
    desc += "Enemies: " + std::to_string(data.enemyCount);
    if (data.boss) {
        desc += " | ⚔️ BOSS: " + data.bossName + " ⚔️";
    }
    return desc;
}

bool DungeonLevelManager::isBossFloor(int floor) const {
    const LevelData& data = getLevelData(floor);
    return data.boss;
}

std::string DungeonLevelManager::getBossName(int floor) const {
    const LevelData& data = getLevelData(floor);
    return data.bossName;
}

std::string DungeonLevelManager::getThemeName() const {
    return getCurrentLevelData().theme;
}

sf::Color DungeonLevelManager::getFloorLightColor() const {
    return getCurrentLevelData().lightColor;
}

float DungeonLevelManager::getFloorAmbientLevel() const {
    return getCurrentLevelData().ambientLevel;
}

void DungeonLevelManager::resetToFloor(int floor) {
    if (floor >= 1 && floor <= maxFloors) {
        currentFloor = floor;
        std::cout << "[DungeonLevelManager] Reset to Floor " << floor << std::endl;
    }
}
