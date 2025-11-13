// CHANGE: 2025-11-10 — 10-Floor Dungeon Expansion System Implementation
// Manages progressive dungeon generation with scaling difficulty and themed floors

#include "DungeonLevelManager.h"
#include "Dungeon.h"
#include "Enemy.h"
#include "Player.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <ctime>

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
    std::cout << "[DungeonLevelManager] Loading level configuration from " << jsonPath << std::endl;
    
    // For now, use hardcoded data (JSON parsing can be added with nlohmann/json library)
    // This matches the levels.json structure
    levels.clear();
    
    // Floor 1: Cavern Entrance
    LevelData floor1;
    floor1.floor = 1;
    floor1.theme = "Cavern Entrance";
    floor1.description = "A dark cave entrance filled with weak creatures";
    floor1.roomCount = 10;
    floor1.enemyTypes = {"Goblin Scout", "Slime"};
    floor1.enemyCount = 3;
    floor1.difficulty = 1.0f;
    floor1.lightColor = sf::Color(255, 220, 180, 200);
    floor1.ambientLevel = 0.5f;
    floor1.boss = false;
    levels.push_back(floor1);
    
    // Floor 2: Ruined Halls
    LevelData floor2;
    floor2.floor = 2;
    floor2.theme = "Ruined Halls";
    floor2.roomCount = 12;
    floor2.enemyTypes = {"Orc Grunt", "Skeleton Warrior"};
    floor2.enemyCount = 4;
    floor2.difficulty = 1.3f;
    floor2.lightColor = sf::Color(230, 210, 180, 200);
    floor2.ambientLevel = 0.45f;
    levels.push_back(floor2);
    
    // Floor 3: Undercity
    LevelData floor3;
    floor3.floor = 3;
    floor3.theme = "Undercity";
    floor3.roomCount = 14;
    floor3.enemyTypes = {"Cave Bat", "Skeleton Mage", "Orc Warrior"};
    floor3.enemyCount = 5;
    floor3.difficulty = 1.6f;
    floor3.lightColor = sf::Color(180, 200, 220, 180);
    floor3.ambientLevel = 0.4f;
    levels.push_back(floor3);
    
    // Floor 4: Shadow Temple
    LevelData floor4;
    floor4.floor = 4;
    floor4.theme = "Shadow Temple";
    floor4.roomCount = 16;
    floor4.enemyTypes = {"Shadow Wraith", "Shadow Knight", "Dark Cultist"};
    floor4.enemyCount = 6;
    floor4.difficulty = 2.0f;
    floor4.lightColor = sf::Color(200, 150, 150, 160);
    floor4.ambientLevel = 0.35f;
    levels.push_back(floor4);
    
    // Floor 5: Abyss Core (MINI BOSS)
    LevelData floor5;
    floor5.floor = 5;
    floor5.theme = "Abyss Core";
    floor5.roomCount = 8;
    floor5.enemyTypes = {"Abyss Knight"};
    floor5.enemyCount = 1;
    floor5.difficulty = 2.5f;
    floor5.lightColor = sf::Color(255, 100, 100, 180);
    floor5.ambientLevel = 0.3f;
    floor5.boss = true;
    floor5.bossName = "Abyss Knight";
    levels.push_back(floor5);
    
    // Floor 6: Crystal Mines
    LevelData floor6;
    floor6.floor = 6;
    floor6.theme = "Crystal Mines";
    floor6.roomCount = 18;
    floor6.enemyTypes = {"Crystal Golem", "Cave Bat", "Miner Wraith"};
    floor6.enemyCount = 7;
    floor6.difficulty = 2.8f;
    floor6.lightColor = sf::Color(150, 200, 255, 200);
    floor6.ambientLevel = 0.4f;
    levels.push_back(floor6);
    
    // Floor 7: Forgotten Fortress
    LevelData floor7;
    floor7.floor = 7;
    floor7.theme = "Forgotten Fortress";
    floor7.roomCount = 20;
    floor7.enemyTypes = {"Armored Orc", "Fortress Specter", "War Hound"};
    floor7.enemyCount = 8;
    floor7.difficulty = 3.2f;
    floor7.lightColor = sf::Color(200, 200, 200, 160);
    floor7.ambientLevel = 0.35f;
    levels.push_back(floor7);
    
    // Floor 8: Lava Catacombs
    LevelData floor8;
    floor8.floor = 8;
    floor8.theme = "Lava Catacombs";
    floor8.roomCount = 22;
    floor8.enemyTypes = {"Fire Spirit", "Flame Knight", "Lava Elemental"};
    floor8.enemyCount = 9;
    floor8.difficulty = 3.8f;
    floor8.lightColor = sf::Color(255, 180, 120, 200);
    floor8.ambientLevel = 0.45f;
    levels.push_back(floor8);
    
    // Floor 9: Obsidian Keep
    LevelData floor9;
    floor9.floor = 9;
    floor9.theme = "Obsidian Keep";
    floor9.roomCount = 24;
    floor9.enemyTypes = {"Dark Mage", "Warlord", "Death Knight"};
    floor9.enemyCount = 10;
    floor9.difficulty = 4.3f;
    floor9.lightColor = sf::Color(120, 80, 80, 180);
    floor9.ambientLevel = 0.25f;
    levels.push_back(floor9);
    
    // Floor 10: The Nexus (FINAL BOSS)
    LevelData floor10;
    floor10.floor = 10;
    floor10.theme = "The Nexus";
    floor10.roomCount = 12;
    floor10.enemyTypes = {"Eternal Shade"};
    floor10.enemyCount = 1;
    floor10.difficulty = 5.0f;
    floor10.lightColor = sf::Color(255, 255, 255, 150);
    floor10.ambientLevel = 0.2f;
    floor10.boss = true;
    floor10.bossName = "Eternal Shade";
    levels.push_back(floor10);
    
    maxFloors = levels.size();
    levelsLoaded = true;
    
    std::cout << "[DungeonLevelManager] Loaded " << maxFloors << " floor configurations" << std::endl;
    return true;
}

void DungeonLevelManager::initialize() {
    currentFloor = 1;
    std::cout << "[DungeonLevelManager] Initialized at Floor 1" << std::endl;
}

int DungeonLevelManager::calculateEnemyHP(int baseHP, int floor) const {
    // Formula: HP = baseHP * (1.15^floor)
    return static_cast<int>(baseHP * std::pow(1.15, floor - 1));
}

int DungeonLevelManager::calculateEnemyAttack(int baseAttack, int floor) const {
    // Formula: Attack = baseAttack * (1.12^floor)
    return static_cast<int>(baseAttack * std::pow(1.12, floor - 1));
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
