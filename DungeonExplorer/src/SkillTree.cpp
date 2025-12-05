#include "SkillTree.h"
#include "GameUtils.h"
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
    
    // Tier 1 Left - Power Strike (Hotkey 2)
    auto powerStrike = tree.insertLeft(root, 
        Skill("power_strike", "Power Strike", "active", "Powerful attack - 35 damage, 2 turn cooldown", 1, 2));
    powerStrike->data.damage = 35;
    powerStrike->data.manaCost = 15;
    powerStrike->data.cooldown = 2;
    powerStrike->data.unlocked = true;  // Auto-unlock for player
    
    // Tier 2 Left Branch - Whirlwind (AOE) (Hotkey 3)
    auto whirlwind = tree.insertLeft(powerStrike, 
        Skill("whirlwind", "Whirlwind", "active", "Spin attack hitting all adjacent enemies for 30 damage", 2, 3));
    whirlwind->data.damage = 30;
    whirlwind->data.aoe = true;
    whirlwind->data.manaCost = 25;
    whirlwind->data.cooldown = 3;
    whirlwind->data.unlocked = true;  // Auto-unlock for player
    
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
    
    // Tier 1 Right - Mana Surge (Hotkey 4)
    auto manaSurge = tree.insertRight(root, 
        Skill("mana_surge", "Mana Surge", "active", "Restore 40 mana instantly", 1, 4));
    manaSurge->data.healing = 40;  // Using healing field for mana
    manaSurge->data.manaCost = 0;
    manaSurge->data.cooldown = 5;
    manaSurge->data.unlocked = true;  // Auto-unlock for player
    
    // Tier 2 Left Branch - Fireball (Hotkey 5)
    auto fireball = tree.insertLeft(manaSurge, 
        Skill("fireball", "Fireball", "active", "Ranged fire attack - 45 damage", 2, 5));
    fireball->data.damage = 45;
    fireball->data.manaCost = 30;
    fireball->data.cooldown = 2;
    fireball->data.unlocked = true;  // Auto-unlock for player
    
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
    
    availablePoints = INITIAL_SKILL_POINTS;  // Starting skill points
    
    // OPTIMIZATION: Use Tree's size() and height() methods for diagnostics
    std::cout << "[SkillTree] Enhanced skill tree initialized with " << availablePoints << " available points" << std::endl;
    std::cout << "[SkillTree] Total skills: " << tree.size() << " (1 unlocked, " << (tree.size() - 1) << " locked)" << std::endl;
    std::cout << "[SkillTree] Tree height: " << tree.height() << " levels" << std::endl;
    std::cout << "[SkillTree] Tree balanced: " << (tree.isBalanced() ? "Yes" : "No") << std::endl;
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
    
    // OPTIMIZATION: Show tree statistics when points are added
    std::cout << "[SkillTree] Skills unlocked: " << getActiveSkills().size() 
              << " / " << tree.size() << std::endl;
}

Skill* SkillTree::getSkillByHotkey(int hotkey) {
    // OPTIMIZATION: Could use tree.contains() for existence check first
    // But we need the actual skill data, so recursive search is still needed
    
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

void SkillTree::render(sf::RenderWindow& window, sf::Font& font, sf::Vector2f mousePos) {
    if (!root) return;
    
    // Recursive render function using class constants
    std::function<void(std::shared_ptr<BinaryTree<Skill>::Node>, float, float, float)> renderNode;
    renderNode = [&](std::shared_ptr<BinaryTree<Skill>::Node> node, float x, float y, float width) {
        if (!node) return;
        
        // Draw connections first (so they are behind nodes)
        sf::Color lineColor(LINE_GRAY, LINE_GRAY, LINE_GRAY);
        
        if (node->left) {
            float childX = x - width / 2.0f;
            float childY = y + LEVEL_HEIGHT;
            
            sf::Vertex line[] = {
                sf::Vertex{{x, y}, lineColor},
                sf::Vertex{{childX, childY}, lineColor}
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);
            
            renderNode(node->left, childX, childY, width / 2.0f);
        }
        
        if (node->right) {
            float childX = x + width / 2.0f;
            float childY = y + LEVEL_HEIGHT;
            
            sf::Vertex line[] = {
                sf::Vertex{{x, y}, lineColor},
                sf::Vertex{{childX, childY}, lineColor}
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);
            
            renderNode(node->right, childX, childY, width / 2.0f);
        }
        
        // Draw node
        sf::CircleShape circle(NODE_RADIUS);
        circle.setOrigin({NODE_RADIUS, NODE_RADIUS});
        circle.setPosition({x, y});
        
        // Determine color based on state
        sf::Color fillColor(LOCKED_GRAY, LOCKED_GRAY, LOCKED_GRAY);
        sf::Color outlineColor = lineColor;
        
        if (node->data.unlocked) {
            fillColor = sf::Color(LOCKED_GRAY, UNLOCKED_GREEN, LOCKED_GRAY);
            outlineColor = sf::Color::White;
        } else if (canUnlock(node)) {
            fillColor = sf::Color(UNLOCKABLE_YELLOW, UNLOCKABLE_YELLOW, LOCKED_GRAY);
            outlineColor = sf::Color::Yellow;
        }
        
        // Hover effect - use helper to eliminate duplicate distance calc
        float dist = pointDistance(mousePos.x, mousePos.y, x, y);
        bool hovered = (dist <= NODE_RADIUS);
        
        if (hovered) {
            outlineColor = sf::Color::Cyan;
        }
        
        circle.setFillColor(fillColor);
        circle.setOutlineColor(outlineColor);
        circle.setOutlineThickness(hovered ? OUTLINE_HOVER : OUTLINE_NORMAL);
        
        window.draw(circle);
        
        // Draw icon/text inside
        sf::Text iconText(font);
        iconText.setString(node->data.name.substr(0, 1));
        iconText.setCharacterSize(ICON_FONT_SIZE);
        iconText.setFillColor(sf::Color::White);
        sf::FloatRect bounds = iconText.getLocalBounds();
        iconText.setOrigin({bounds.size.x/2.f, bounds.size.y/2.f});
        iconText.setPosition({x, y - ICON_Y_OFFSET});
        window.draw(iconText);
        
        if (hovered) {
             // Draw tooltip
             sf::RectangleShape tooltipBg(sf::Vector2f(TOOLTIP_WIDTH, TOOLTIP_HEIGHT));
             tooltipBg.setPosition(mousePos + sf::Vector2f(TOOLTIP_OFFSET, TOOLTIP_OFFSET));
             tooltipBg.setFillColor(sf::Color(0, 0, 0, TOOLTIP_BG_ALPHA));
             tooltipBg.setOutlineThickness(1.f);
             tooltipBg.setOutlineColor(sf::Color::White);
             window.draw(tooltipBg);
             
             sf::Text tooltipText(font);
             tooltipText.setCharacterSize(TOOLTIP_FONT_SIZE);
             tooltipText.setPosition(mousePos + sf::Vector2f(TOOLTIP_PADDING, TOOLTIP_PADDING));
             tooltipText.setFillColor(sf::Color::White);
             
             std::string text = node->data.name + "\n" + node->data.description;
             if (!node->data.unlocked) {
                 text += "\nCost: " + std::to_string(node->data.cost) + " SP";
                 text += canUnlock(node) ? "\n(Click to Unlock)" : "\n(Locked)";
             }
             tooltipText.setString(text);
             window.draw(tooltipText);
        }
    };
    
    renderNode(root, TREE_START_X, TREE_START_Y, BASE_WIDTH);
}

void SkillTree::displayTree() {
    std::cout << "\n[SkillTree] Current skill tree (Level-order):" << std::endl;
    std::cout << "[SkillTree] Tree stats - Height: " << tree.height() 
              << ", Size: " << tree.size() 
              << ", Balanced: " << (tree.isBalanced() ? "Yes" : "No") << std::endl;
    
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

bool SkillTree::tryUnlockAt(sf::Vector2f mousePos) {
    if (!root) return false;
    
    // Uses class constants - no duplicate definitions needed
    bool unlockedSomething = false;
    
    std::function<void(std::shared_ptr<BinaryTree<Skill>::Node>, float, float, float)> checkClick;
    checkClick = [&](std::shared_ptr<BinaryTree<Skill>::Node> node, float x, float y, float width) {
        if (!node || unlockedSomething) return;
        
        // Check children first
        if (node->left) {
            checkClick(node->left, x - width / 2.0f, y + LEVEL_HEIGHT, width / 2.0f);
        }
        if (node->right) {
            checkClick(node->right, x + width / 2.0f, y + LEVEL_HEIGHT, width / 2.0f);
        }
        
        if (unlockedSomething) return;
        
        // Use helper to eliminate duplicate distance calculation
        float dist = pointDistance(mousePos.x, mousePos.y, x, y);
        
        if (dist <= NODE_RADIUS && !node->data.unlocked && canUnlock(node)) {
            unlockSkill(node);
            unlockedSomething = true;
            
            // 🎮 Add unlock animation
            unlockAnimations.push_back({node->data.id, 0.f, 1.0f});
        }
    };
    
    checkClick(root, TREE_START_X, TREE_START_Y, BASE_WIDTH);
    return unlockedSomething;
}

// ═══════════════════════════════════════════════════════════════════════
// SYNERGY AND ANIMATION SYSTEMS
// ═══════════════════════════════════════════════════════════════════════

void SkillTree::initializeSynergies() {
    synergies.clear();
    
    // Warrior synergies
    synergies.push_back({"power_strike", "iron_skin", "damage", 0.15f});      // +15% damage
    synergies.push_back({"whirlwind", "berserker_rage", "attack_speed", 0.1f}); // +10% attack speed
    synergies.push_back({"flame_wave", "whirlwind", "aoe_damage", 0.2f});      // +20% AOE damage
    
    // Mage synergies
    synergies.push_back({"fireball", "mana_mastery", "spell_damage", 0.25f});  // +25% spell damage
    synergies.push_back({"meteor_storm", "fireball", "critical", 0.1f});       // +10% crit chance
    
    // Rogue synergies  
    synergies.push_back({"shadow_step", "backstab", "stealth_damage", 0.3f}); // +30% from stealth
    synergies.push_back({"poison_blade", "backstab", "dot_damage", 0.2f});    // +20% DOT damage
    
    // Cross-class synergies
    synergies.push_back({"iron_skin", "regeneration", "max_hp", 0.1f});       // +10% max HP
    synergies.push_back({"mana_mastery", "regeneration", "mana_regen", 0.15f}); // +15% mana regen
    
    std::cout << "[SkillTree] Initialized " << synergies.size() << " synergy bonuses" << std::endl;
}

float SkillTree::getSynergyBonus(const std::string& bonusType) const {
    float total = 0.f;
    
    for (const auto& synergy : synergies) {
        // Check if both skills are unlocked
        bool skill1Unlocked = false, skill2Unlocked = false;
        
        std::function<void(std::shared_ptr<BinaryTree<Skill>::Node>)> findSkills;
        findSkills = [&](std::shared_ptr<BinaryTree<Skill>::Node> node) {
            if (!node) return;
            if (node->data.id == synergy.skill1 && node->data.unlocked) skill1Unlocked = true;
            if (node->data.id == synergy.skill2 && node->data.unlocked) skill2Unlocked = true;
            findSkills(node->left);
            findSkills(node->right);
        };
        findSkills(root);
        
        if (skill1Unlocked && skill2Unlocked && synergy.bonusName == bonusType) {
            total += synergy.bonusValue;
        }
    }
    
    return total;
}

void SkillTree::updateAnimations(float deltaTime) {
    for (auto it = unlockAnimations.begin(); it != unlockAnimations.end();) {
        it->timer += deltaTime;
        if (it->timer >= it->maxTime) {
            it = unlockAnimations.erase(it);
        } else {
            ++it;
        }
    }
}

int SkillTree::getUnlockedSkillCount() const {
    int count = 0;
    std::function<void(std::shared_ptr<BinaryTree<Skill>::Node>)> countSkills;
    countSkills = [&](std::shared_ptr<BinaryTree<Skill>::Node> node) {
        if (!node) return;
        if (node->data.unlocked) count++;
        countSkills(node->left);
        countSkills(node->right);
    };
    countSkills(root);
    return count;
}

float SkillTree::getTotalPassiveBonus(const std::string& stat) const {
    float total = 0.f;
    
    std::function<void(std::shared_ptr<BinaryTree<Skill>::Node>)> sumPassives;
    sumPassives = [&](std::shared_ptr<BinaryTree<Skill>::Node> node) {
        if (!node) return;
        
        if (node->data.unlocked && node->data.type == "passive") {
            if (stat == "defense") total += node->data.defenseBonus;
            if (stat == "damage_resist") total += node->data.damageResistance;
            if (stat == "attack_speed") total += node->data.attackSpeedBonus;
            if (stat == "crit_chance") total += node->data.critChance;
            if (stat == "hp_regen") total += node->data.hpPerSecond;
        }
        
        sumPassives(node->left);
        sumPassives(node->right);
    };
    sumPassives(root);
    
    // Add synergy bonuses
    total += getSynergyBonus(stat);
    
    return total;
}
