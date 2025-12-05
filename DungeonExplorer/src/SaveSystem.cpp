// SaveSystem.cpp - Implementation of SaveSystem
#include "SaveSystem.h"
#include "Player.h"
#include "Dungeon.h"
#include "SkillTree.h"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

bool SaveSystem::saveGame(const std::string& slotName, const Player& player, const Dungeon& dungeon, const SkillTree& skillTree, int currentFloor) {
    
    if (!fs::exists(saveDirectory)) {
        fs::create_directory(saveDirectory);
    }

    nlohmann::json j;
    j["player"] = serializePlayer(player);
    j["dungeon"] = serializeDungeon(dungeon);
    j["skillTree"] = serializeSkillTree(skillTree);
    j["currentFloor"] = currentFloor;

    std::string path = getSavePath(slotName);
    std::ofstream file(path);
    if (!file.is_open()) return false;

    file << j.dump(4);
    return true;
}

bool SaveSystem::loadGame(const std::string& slotName, Player& player, Dungeon& dungeon, SkillTree& skillTree, int& currentFloor) {
    
    std::string path = getSavePath(slotName);
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

void SaveSystem::autoSave(const Player& player, const Dungeon& dungeon, const SkillTree& skillTree, int currentFloor) {
    if (autoSaveEnabled) {
        saveGame("autosave", player, dungeon, skillTree, currentFloor);
        std::cout << "[SaveSystem] Auto-saved game." << std::endl;
    }
}

std::string SaveSystem::getSavePath(const std::string& slotName) const {
    return saveDirectory + slotName + ".json";
}

nlohmann::json SaveSystem::serializePlayer(const Player& player) const {
    nlohmann::json j;
    j["name"] = player.getName();
    j["level"] = player.getLevel();
    j["exp"] = player.getExperience();
    j["hp"] = player.getHealth();
    j["maxHp"] = player.getMaxHealth();
    // Add inventory serialization here
    return j;
}

nlohmann::json SaveSystem::serializeDungeon(const Dungeon& dungeon) const {
    nlohmann::json j;
    // Serialize dungeon state (seed, rooms, etc.)
    return j;
}

nlohmann::json SaveSystem::serializeSkillTree(const SkillTree& skillTree) const {
    nlohmann::json j;
    // Serialize unlocked skills
    return j;
}

void SaveSystem::deserializePlayer(const nlohmann::json& j, Player& player) {
    // player.setName(j["name"]); // Assuming setters exist
    // player.setLevel(j["level"]);
    // ...
}

void SaveSystem::deserializeDungeon(const nlohmann::json& j, Dungeon& dungeon) {
    // Restore dungeon state
}

void SaveSystem::deserializeSkillTree(const nlohmann::json& j, SkillTree& skillTree) {
    // Restore skills
}
