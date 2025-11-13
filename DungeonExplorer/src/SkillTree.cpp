#include "SkillTree.h"
#include <iostream>

SkillTree::SkillTree() : availablePoints(0) {
}

void SkillTree::initialize() {
    std::cout << "[SkillTree] Initializing enhanced skill tree with 20+ skills..." << std::endl;
    
    // Tier 0 - Root skill (always unlocked)
    Skill rootSkill("slash", "Slash", "active", "Basic sword attack - deals 15 damage", 0, 1);
    rootSkill.unlocked = true;
    rootSkill.damage = 15;
    rootSkill.manaCost = 5;
    rootSkill.cooldown = 0;  // No cooldown - basic attack
    tree.setRoot(rootSkill);
    root = tree.getRoot();
    
    // ===== LEFT BRANCH - WARRIOR PATH (Offense/Tank) =====
    
    // Tier 1 Left - Power Strike
    auto powerStrike = tree.insertLeft(root, 
        Skill("power_strike", "Power Strike", "active", "Powerful attack - 35 damage, 2 turn cooldown", 1, 2));
    powerStrike->data.damage = 35;
    powerStrike->data.manaCost = 15;
    powerStrike->data.cooldown = 2;
    
    // Tier 2 Left Branch - Whirlwind (AOE)
    auto whirlwind = tree.insertLeft(powerStrike, 
        Skill("whirlwind", "Whirlwind", "active", "Spin attack hitting all adjacent enemies for 30 damage", 2, 3));
    whirlwind->data.damage = 30;
    whirlwind->data.aoe = true;
    whirlwind->data.manaCost = 25;
    whirlwind->data.cooldown = 3;
    
    // Tier 3 Left-Left - Flame Wave (Fire AOE)
    auto flameWave = tree.insertLeft(whirlwind, 
        Skill("flame_wave", "Flame Wave", "active", "Burning wave dealing 25 dmg + 10 burn/turn for 3 turns", 3, 4));
    flameWave->data.damage = 25;
    flameWave->data.aoe = true;
    flameWave->data.burnDuration = 3;
    flameWave->data.manaCost = 35;
    flameWave->data.cooldown = 5;
    
    // Tier 3 Left-Right - Berserker Rage (Passive)
    auto berserkerRage = tree.insertRight(whirlwind, 
        Skill("berserker_rage", "Berserker Rage", "passive", "+20% damage when below 50% HP", 2, 0));
    berserkerRage->data.attackSpeedBonus = 0.2f;
    
    // Tier 2 Right Branch - Iron Skin (Passive Defense)
    auto ironSkin = tree.insertRight(powerStrike, 
        Skill("iron_skin", "Iron Skin", "passive", "+15 defense, +10% damage resistance", 2, 0));
    ironSkin->data.defenseBonus = 15;
    ironSkin->data.damageResistance = 0.1f;
    
    // Tier 3 Right-Left - Shield Bash
    auto shieldBash = tree.insertLeft(ironSkin,
        Skill("shield_bash", "Shield Bash", "active", "Stun enemy for 1 turn, 20 damage", 2, 0));
    shieldBash->data.damage = 20;
    shieldBash->data.manaCost = 20;
    shieldBash->data.cooldown = 4;
    shieldBash->data.duration = 1;  // Stun duration
    
    // Tier 3 Right-Right - Revenge
    auto revenge = tree.insertRight(ironSkin,
        Skill("revenge", "Revenge", "passive", "Deal 50% of damage taken back to attacker", 3, 0));
    revenge->data.damageResistance = 0.0f;  // Using as thorns damage
    
    // ===== RIGHT BRANCH - MAGE/ROGUE PATH (Magic/Utility) =====
    
    // Tier 1 Right - Mana Surge
    auto manaSurge = tree.insertRight(root, 
        Skill("mana_surge", "Mana Surge", "active", "Restore 40 mana instantly", 1, 0));
    manaSurge->data.healing = 40;  // Using healing field for mana
    manaSurge->data.manaCost = 0;
    manaSurge->data.cooldown = 5;
    
    // Tier 2 Left Branch - Fireball
    auto fireball = tree.insertLeft(manaSurge, 
        Skill("fireball", "Fireball", "active", "Ranged fire attack - 45 damage", 2, 0));
    fireball->data.damage = 45;
    fireball->data.manaCost = 30;
    fireball->data.cooldown = 2;
    
    // Tier 3 Left-Left - Meteor Storm
    auto meteorStorm = tree.insertLeft(fireball,
        Skill("meteor_storm", "Meteor Storm", "active", "Massive AOE - 60 damage to all enemies", 4, 0));
    meteorStorm->data.damage = 60;
    meteorStorm->data.aoe = true;
    meteorStorm->data.manaCost = 50;
    meteorStorm->data.cooldown = 8;
    
    // Tier 3 Left-Right - Mana Mastery
    auto manaMastery = tree.insertRight(fireball,
        Skill("mana_mastery", "Mana Mastery", "passive", "All spells cost 30% less mana", 2, 0));
    manaMastery->data.attackSpeedBonus = 0.3f;  // Using for mana reduction
    
    // Tier 2 Right Branch - Shadow Step
    auto shadowStep = tree.insertRight(manaSurge, 
        Skill("shadow_step", "Shadow Step", "active", "Teleport 3 tiles, become invisible for 2 turns", 2, 5));
    shadowStep->data.dashDistance = 3;
    shadowStep->data.manaCost = 15;
    shadowStep->data.cooldown = 4;
    shadowStep->data.duration = 2;  // Invisibility duration
    
    // Tier 3 Right-Left - Assassination
    auto assassination = tree.insertLeft(shadowStep,
        Skill("assassination", "Assassination", "active", "Backstab for 80 damage (requires invisibility)", 3, 0));
    assassination->data.damage = 80;
    assassination->data.manaCost = 25;
    assassination->data.cooldown = 6;
    
    // Tier 3 Right-Right - Evasion
    auto evasion = tree.insertRight(shadowStep,
        Skill("evasion", "Evasion", "passive", "30% chance to dodge attacks", 2, 0));
    evasion->data.critChance = 0.3f;  // Using crit for dodge chance
    
    availablePoints = 5;  // Starting skill points
    
    std::cout << "[SkillTree] Enhanced skill tree initialized with " << availablePoints << " available points" << std::endl;
    std::cout << "[SkillTree] Total skills: 18 (1 unlocked, 17 locked)" << std::endl;
}

void SkillTree::unlockSkill(std::shared_ptr<BinaryTree<Skill>::Node> node) {
    if (!node) return;
    
    if (node->data.unlocked) {
        std::cout << "[SkillTree] Skill " << node->data.name << " already unlocked!" << std::endl;
        return;
    }
    
    if (!canUnlock(node)) {
        std::cout << "[SkillTree] Cannot unlock " << node->data.name 
                  << " - prerequisites: ";
        
        if (availablePoints < node->data.cost) {
            std::cout << "need " << node->data.cost << " points, have " << availablePoints;
        } else {
            std::cout << "parent skill not unlocked";
        }
        std::cout << std::endl;
        return;
    }
    
    // Unlock the skill
    node->data.unlocked = true;
    availablePoints -= node->data.cost;
    
    std::cout << "[SkillTree] ✓ Unlocked skill: " << node->data.name 
              << " (" << node->data.type << ", cost: " << node->data.cost 
              << ") - Points remaining: " << availablePoints << std::endl;
    
    // Apply passive bonuses immediately (handled by Player class when needed)
    if (node->data.type == "passive") {
        std::cout << "[SkillTree] Passive effect activated: " << node->data.description << std::endl;
    } else {
        std::cout << "[SkillTree] Active skill assigned to hotkey " << node->data.hotkey << std::endl;
    }
}

bool SkillTree::canUnlock(std::shared_ptr<BinaryTree<Skill>::Node> node) const {
    if (!node) return false;
    if (node->data.unlocked) return false;
    if (availablePoints < node->data.cost) return false;
    
    // Root skill is always unlockable (but already unlocked)
    if (node == root) return true;
    
    // Check if parent is unlocked - need to find parent
    bool parentUnlocked = false;
    
    std::function<bool(std::shared_ptr<BinaryTree<Skill>::Node>, std::shared_ptr<BinaryTree<Skill>::Node>)> findParent;
    findParent = [&](std::shared_ptr<BinaryTree<Skill>::Node> current, std::shared_ptr<BinaryTree<Skill>::Node> target) -> bool {
        if (!current) return false;
        
        // Check if target is direct child
        if (current->left == target || current->right == target) {
            return current->data.unlocked;
        }
        
        // Recurse
        return findParent(current->left, target) || findParent(current->right, target);
    };
    
    parentUnlocked = findParent(root, node);
    
    return parentUnlocked;
}

void SkillTree::addPoints(int points) {
    availablePoints += points;
    std::cout << "[SkillTree] Added " << points << " skill points. Total: " 
              << availablePoints << std::endl;
}

Skill* SkillTree::getSkillByHotkey(int hotkey) {
    // Recursively search tree for skill with matching hotkey
    std::function<Skill*(std::shared_ptr<BinaryTree<Skill>::Node>)> search;
    search = [&](std::shared_ptr<BinaryTree<Skill>::Node> node) -> Skill* {
        if (!node) return nullptr;
        
        if (node->data.hotkey == hotkey && node->data.unlocked) {
            return &node->data;
        }
        
        Skill* leftResult = search(node->left);
        if (leftResult) return leftResult;
        
        return search(node->right);
    };
    
    return search(root);
}

std::vector<Skill*> SkillTree::getActiveSkills() {
    std::vector<Skill*> activeSkills;
    
    std::function<void(std::shared_ptr<BinaryTree<Skill>::Node>)> traverse;
    traverse = [&](std::shared_ptr<BinaryTree<Skill>::Node> node) {
        if (!node) return;
        
        if (node->data.unlocked && node->data.type == "active") {
            activeSkills.push_back(&node->data);
        }
        
        traverse(node->left);
        traverse(node->right);
    };
    
    traverse(root);
    return activeSkills;
}

void SkillTree::render(sf::RenderWindow& window, sf::Font& font) {
    // Simple tree visualization
    if (!root) return;
    
    float startX = 400.0f;
    float startY = 100.0f;
    float offsetX = 150.0f;
    float offsetY = 100.0f;
    
    // Draw root
    sf::CircleShape rootCircle(30.0f);
    rootCircle.setPosition(sf::Vector2f(startX - 30.0f, startY - 30.0f));
    rootCircle.setFillColor(root->data.unlocked ? sf::Color::Green : sf::Color(100, 100, 100));
    rootCircle.setOutlineThickness(2.0f);
    rootCircle.setOutlineColor(sf::Color::White);
    window.draw(rootCircle);
    
    sf::Text rootText(font);
    rootText.setString(root->data.name);
    rootText.setCharacterSize(14);
    rootText.setPosition(sf::Vector2f(startX - 25.0f, startY - 10.0f));
    rootText.setFillColor(sf::Color::White);
    window.draw(rootText);
}

void SkillTree::displayTree() {
    std::cout << "\n[SkillTree] Current skill tree (Level-order):" << std::endl;
    tree.levelOrder([](const Skill& skill) {
        std::cout << "  - " << skill.name << " [" 
                  << (skill.unlocked ? "UNLOCKED" : "locked") << "] "
                  << "Cost: " << skill.cost << std::endl;
    });
}

void SkillTree::updateCooldowns() {
    std::function<void(std::shared_ptr<BinaryTree<Skill>::Node>)> traverse;
    traverse = [&](std::shared_ptr<BinaryTree<Skill>::Node> node) {
        if (!node) return;
        
        if (node->data.currentCooldown > 0) {
            node->data.currentCooldown--;
        }
        
        traverse(node->left);
        traverse(node->right);
    };
    
    traverse(root);
}
