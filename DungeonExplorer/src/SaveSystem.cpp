// SaveSystem.cpp - Game save/load persistence system
#include "SaveSystem.h"
#include "Player.h"
#include "Dungeon.h"
#include "SkillTree.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// ═══════════════════════════════════════════════════════════════════════════
// SAVE/LOAD OPERATIONS
// ═══════════════════════════════════════════════════════════════════════════

bool SaveSystem::saveGame(const std::string& slotName, const Player& player, 
                          const Dungeon& dungeon, const SkillTree& skillTree, int currentFloor) {
    if (!fs::exists(saveDirectory)) {
        fs::create_directory(saveDirectory);
    }

    nlohmann::json j;
    j["player"] = serializePlayer(player);
    j["dungeon"] = serializeDungeon(dungeon);
    j["skillTree"] = serializeSkillTree(skillTree);
    j["currentFloor"] = currentFloor;

    const std::string path = getSavePath(slotName);
    std::ofstream file(path);
    if (!file.is_open()) return false;

    file << j.dump(4);
    return true;
}

bool SaveSystem::loadGame(const std::string& slotName, Player& player, 
                          Dungeon& dungeon, SkillTree& skillTree, int& currentFloor) {
    const std::string path = getSavePath(slotName);
    std::ifstream file(path);
    if (!file.is_open()) return false;

    nlohmann::json j;
    file >> j;

    deserializePlayer(j["player"], player);
    deserializeDungeon(j["dungeon"], dungeon);
    deserializeSkillTree(j["skillTree"], skillTree);
    currentFloor = j["currentFloor"];

    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// SAVE MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════════

bool SaveSystem::saveExists(const std::string& slotName) const {
    return fs::exists(getSavePath(slotName));
}

void SaveSystem::deleteSave(const std::string& slotName) {
    if (saveExists(slotName)) {
        fs::remove(getSavePath(slotName));
    }
}

std::vector<std::string> SaveSystem::listSaves() const {
    std::vector<std::string> saves;
    if (!fs::exists(saveDirectory)) return saves;

    for (const auto& entry : fs::directory_iterator(saveDirectory)) {
        if (entry.path().extension() == ".json") {
            saves.push_back(entry.path().stem().string());
        }
    }
    return saves;
}

void SaveSystem::autoSave(const Player& player, const Dungeon& dungeon, 
                          const SkillTree& skillTree, int currentFloor) {
    if (autoSaveEnabled) {
        saveGame("autosave", player, dungeon, skillTree, currentFloor);
    }
}

std::string SaveSystem::getSavePath(const std::string& slotName) const {
    return saveDirectory + slotName + ".json";
}

// ═══════════════════════════════════════════════════════════════════════════
// SERIALIZATION - PLAYER
// ═══════════════════════════════════════════════════════════════════════════

nlohmann::json SaveSystem::serializePlayer(const Player& player) const {
    nlohmann::json j;
    j["name"] = player.getName();
    j["level"] = player.getLevel();
    j["exp"] = player.getExperience();
    j["hp"] = player.getHealth();
    j["maxHp"] = player.getMaxHealth();
    j["gold"] = player.getGold();
    j["mana"] = player.getMana();
    j["maxMana"] = player.getMaxMana();
    // TODO: Add inventory serialization
    return j;
}

void SaveSystem::deserializePlayer(const nlohmann::json& j, Player& player) {
    // TODO: Implement player deserialization
    // Requires adding setter methods to Player class
    (void)j;      // Suppress unused parameter warning
    (void)player;
}

// ═══════════════════════════════════════════════════════════════════════════
// SERIALIZATION - DUNGEON
// ═══════════════════════════════════════════════════════════════════════════

nlohmann::json SaveSystem::serializeDungeon(const Dungeon& dungeon) const {
    nlohmann::json j;
    // TODO: Serialize dungeon state (seed, rooms, tiles)
    j["width"] = dungeon.getWidth();
    j["height"] = dungeon.getHeight();
    return j;
}

void SaveSystem::deserializeDungeon(const nlohmann::json& j, Dungeon& dungeon) {
    // TODO: Implement dungeon restoration
    (void)j;
    (void)dungeon;
}

// ═══════════════════════════════════════════════════════════════════════════
// SERIALIZATION - SKILL TREE
// ═══════════════════════════════════════════════════════════════════════════

nlohmann::json SaveSystem::serializeSkillTree(const SkillTree& skillTree) const {
    nlohmann::json j;
    j["availablePoints"] = skillTree.getAvailablePoints();
    j["unlockedCount"] = skillTree.getUnlockedSkillCount();
    // TODO: Serialize individual unlocked skills
    return j;
}

void SaveSystem::deserializeSkillTree(const nlohmann::json& j, SkillTree& skillTree) {
    // TODO: Implement skill tree restoration
    (void)j;
    (void)skillTree;
}

