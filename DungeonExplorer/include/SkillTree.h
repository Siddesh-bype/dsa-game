#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include <vector>
#include "DataStructures/Tree.h"

struct Skill {
    std::string id;
    std::string name;
    std::string type;  // "active" or "passive"
    std::string description;
    int cost;
    bool unlocked;
    int hotkey;  // 1-5 for active skills
    int manaCost;  // Mana required to use
    int cooldown;  // Cooldown in turns
    int currentCooldown;  // Remaining cooldown
    
    // Active skill properties
    int damage;
    int healing;
    bool aoe;
    int burnDuration;
    int dashDistance;
    float damageReduction;
    int duration;
    
    // Passive skill properties
    float attackSpeedBonus;
    float damageResistance;
    int defenseBonus;
    int hpPerSecond;
    float critChance;
    
    Skill(const std::string& id = "", const std::string& n = "", const std::string& t = "passive", 
          const std::string& d = "", int c = 1, int hk = 0)
        : id(id), name(n), type(t), description(d), cost(c), unlocked(false), hotkey(hk),
          manaCost(10), cooldown(0), currentCooldown(0),
          damage(0), healing(0), aoe(false), burnDuration(0), dashDistance(0), 
          damageReduction(0.0f), duration(0), attackSpeedBonus(0.0f), damageResistance(0.0f),
          defenseBonus(0), hpPerSecond(0), critChance(0.0f) {}
};

class SkillTree {
private:
    BinaryTree<Skill> tree;
    std::shared_ptr<BinaryTree<Skill>::Node> root;
    
    int availablePoints;

public:
    SkillTree();
    
    void initialize();
    void unlockSkill(std::shared_ptr<BinaryTree<Skill>::Node> node);
    
    bool canUnlock(std::shared_ptr<BinaryTree<Skill>::Node> node) const;
    void addPoints(int points);
    
    // Get skill by hotkey (1-5)
    Skill* getSkillByHotkey(int hotkey);
    
    // Get all unlocked active skills
    std::vector<Skill*> getActiveSkills();
    
    // Update cooldowns (call each turn)
    void updateCooldowns();
    
    const BinaryTree<Skill>& getTree() const { return tree; }
    std::shared_ptr<BinaryTree<Skill>::Node> getRoot() { return root; }
    
    int getAvailablePoints() const { return availablePoints; }
    
    void render(sf::RenderWindow& window, sf::Font& font);
    void displayTree();
};
