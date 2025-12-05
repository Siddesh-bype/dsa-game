// SaveSystem.h - JSON-based save/load system for game state persistence
#pragma once
#include <string>
#include <nlohmann/json.hpp>

class Player;
class Dungeon;
class SkillTree;

class SaveSystem {
public:
    static SaveSystem& getInstance() {
        static SaveSystem instance;
        return instance;
    }
    
    // Save/Load operations
    bool saveGame(const std::string& slotName, const Player& player, 
                  const Dungeon& dungeon, const SkillTree& skillTree, int currentFloor);
    bool loadGame(const std::string& slotName, Player& player, 
                  Dungeon& dungeon, SkillTree& skillTree, int& currentFloor);
    
    // Slot management
    bool saveExists(const std::string& slotName) const;
    void deleteSave(const std::string& slotName);
    std::vector<std::string> listSaves() const;
    
    // Auto-save
    void enableAutoSave(bool enable) { autoSaveEnabled = enable; }
    void autoSave(const Player& player, const Dungeon& dungeon, 
                  const SkillTree& skillTree, int currentFloor);
    
private:
    SaveSystem() : autoSaveEnabled(true) {}
    SaveSystem(const SaveSystem&) = delete;
    SaveSystem& operator=(const SaveSystem&) = delete;
    
    std::string getSavePath(const std::string& slotName) const;
    nlohmann::json serializePlayer(const Player& player) const;
    nlohmann::json serializeDungeon(const Dungeon& dungeon) const;
    nlohmann::json serializeSkillTree(const SkillTree& skillTree) const;
    
    void deserializePlayer(const nlohmann::json& j, Player& player);
    void deserializeDungeon(const nlohmann::json& j, Dungeon& dungeon);
    void deserializeSkillTree(const nlohmann::json& j, SkillTree& skillTree);
    
    bool autoSaveEnabled;
    const std::string saveDirectory = "saves/";
};
