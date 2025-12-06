#include "SkillTree.h"
#include "GameUtils.h"

// ═══════════════════════════════════════════════════════════════════════════
// SKILL CONFIGURATION CONSTANTS
// ═══════════════════════════════════════════════════════════════════════════

namespace {
    // Root Skill (Slash)
    constexpr int SLASH_DAMAGE = 15;
    constexpr int SLASH_MANA_COST = 5;
    constexpr int SLASH_COOLDOWN = 0;
    
    // Warrior Path
    constexpr int POWER_STRIKE_DAMAGE = 35;
    constexpr int POWER_STRIKE_MANA = 15;
    constexpr int POWER_STRIKE_COOLDOWN = 2;
    
    constexpr int WHIRLWIND_DAMAGE = 30;
    constexpr int WHIRLWIND_MANA = 25;
    constexpr int WHIRLWIND_COOLDOWN = 3;
    
    constexpr int FLAME_WAVE_DAMAGE = 25;
    constexpr int FLAME_WAVE_BURN_DURATION = 3;
    constexpr int FLAME_WAVE_MANA = 35;
    constexpr int FLAME_WAVE_COOLDOWN = 5;
    
    constexpr float BERSERKER_DAMAGE_BONUS = 0.2f;
    constexpr int IRON_SKIN_DEFENSE = 15;
    constexpr float IRON_SKIN_RESIST = 0.1f;
    
    constexpr int SHIELD_BASH_DAMAGE = 20;
    constexpr int SHIELD_BASH_MANA = 20;
    constexpr int SHIELD_BASH_COOLDOWN = 4;
    constexpr int SHIELD_BASH_STUN = 1;
    
    // Mage/Rogue Path
    constexpr int MANA_SURGE_RESTORE = 40;
    constexpr int MANA_SURGE_COOLDOWN = 5;
    
    constexpr int FIREBALL_DAMAGE = 45;
    constexpr int FIREBALL_MANA = 30;
    constexpr int FIREBALL_COOLDOWN = 2;
    
    constexpr int METEOR_DAMAGE = 60;
    constexpr int METEOR_MANA = 50;
    constexpr int METEOR_COOLDOWN = 8;
    
    constexpr float MANA_MASTERY_REDUCTION = 0.3f;
    
    constexpr int SHADOW_STEP_DISTANCE = 3;
    constexpr int SHADOW_STEP_MANA = 15;
    constexpr int SHADOW_STEP_COOLDOWN = 4;
    constexpr int SHADOW_STEP_INVIS = 2;
    
    constexpr int ASSASSINATION_DAMAGE = 80;
    constexpr int ASSASSINATION_MANA = 25;
    constexpr int ASSASSINATION_COOLDOWN = 6;
    
    constexpr float EVASION_DODGE_CHANCE = 0.3f;
    
    // Synergy bonuses
    constexpr float SYNERGY_POWER_IRON = 0.15f;
    constexpr float SYNERGY_WHIRL_RAGE = 0.1f;
    constexpr float SYNERGY_FLAME_WHIRL = 0.2f;
    constexpr float SYNERGY_FIRE_MANA = 0.25f;
    constexpr float SYNERGY_METEOR_FIRE = 0.1f;
    constexpr float SYNERGY_SHADOW_BACK = 0.3f;
    constexpr float SYNERGY_POISON_BACK = 0.2f;
    constexpr float SYNERGY_IRON_REGEN = 0.1f;
    constexpr float SYNERGY_MANA_REGEN = 0.15f;
}

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTOR
// ═══════════════════════════════════════════════════════════════════════════

SkillTree::SkillTree() : availablePoints(0) {
}

// ═══════════════════════════════════════════════════════════════════════════
// INITIALIZATION
// ═══════════════════════════════════════════════════════════════════════════

void SkillTree::initialize() {
    // Tier 0 - Root skill (always unlocked)
    Skill rootSkill("slash", "Slash", "active", "Basic sword attack - deals 15 damage", 0, 1);
    rootSkill.unlocked = true;
    rootSkill.damage = SLASH_DAMAGE;
    rootSkill.manaCost = SLASH_MANA_COST;
    rootSkill.cooldown = SLASH_COOLDOWN;
    tree.setRoot(rootSkill);
    root = tree.getRoot();
    
    // Initialize skill branches
    initializeWarriorBranch();
    initializeMageBranch();
    
    availablePoints = INITIAL_SKILL_POINTS;
    
    // Initialize synergies
    initializeSynergies();
}

void SkillTree::initializeWarriorBranch() {
    // Tier 1 Left - Power Strike (Hotkey 2)
    auto powerStrike = tree.insertLeft(root, 
        Skill("power_strike", "Power Strike", "active", "Powerful attack - 35 damage, 2 turn cooldown", 1, 2));
    powerStrike->data.damage = POWER_STRIKE_DAMAGE;
    powerStrike->data.manaCost = POWER_STRIKE_MANA;
    powerStrike->data.cooldown = POWER_STRIKE_COOLDOWN;
    powerStrike->data.unlocked = true;
    
    // Tier 2 Left Branch - Whirlwind (AOE) (Hotkey 3)
    auto whirlwind = tree.insertLeft(powerStrike, 
        Skill("whirlwind", "Whirlwind", "active", "Spin attack hitting all adjacent enemies for 30 damage", 2, 3));
    whirlwind->data.damage = WHIRLWIND_DAMAGE;
    whirlwind->data.aoe = true;
    whirlwind->data.manaCost = WHIRLWIND_MANA;
    whirlwind->data.cooldown = WHIRLWIND_COOLDOWN;
    whirlwind->data.unlocked = true;
    
    // Tier 3 Left-Left - Flame Wave (Fire AOE)
    auto flameWave = tree.insertLeft(whirlwind, 
        Skill("flame_wave", "Flame Wave", "active", "Burning wave dealing 25 dmg + 10 burn/turn for 3 turns", 3, 4));
    flameWave->data.damage = FLAME_WAVE_DAMAGE;
    flameWave->data.aoe = true;
    flameWave->data.burnDuration = FLAME_WAVE_BURN_DURATION;
    flameWave->data.manaCost = FLAME_WAVE_MANA;
    flameWave->data.cooldown = FLAME_WAVE_COOLDOWN;
    
    // Tier 3 Left-Right - Berserker Rage (Passive)
    auto berserkerRage = tree.insertRight(whirlwind, 
        Skill("berserker_rage", "Berserker Rage", "passive", "+20% damage when below 50% HP", 2, 0));
    berserkerRage->data.attackSpeedBonus = BERSERKER_DAMAGE_BONUS;
    
    // Tier 2 Right Branch - Iron Skin (Passive Defense)
    auto ironSkin = tree.insertRight(powerStrike, 
        Skill("iron_skin", "Iron Skin", "passive", "+15 defense, +10% damage resistance", 2, 0));
    ironSkin->data.defenseBonus = IRON_SKIN_DEFENSE;
    ironSkin->data.damageResistance = IRON_SKIN_RESIST;
    
    // Tier 3 Right-Left - Shield Bash
    auto shieldBash = tree.insertLeft(ironSkin,
        Skill("shield_bash", "Shield Bash", "active", "Stun enemy for 1 turn, 20 damage", 2, 0));
    shieldBash->data.damage = SHIELD_BASH_DAMAGE;
    shieldBash->data.manaCost = SHIELD_BASH_MANA;
    shieldBash->data.cooldown = SHIELD_BASH_COOLDOWN;
    shieldBash->data.duration = SHIELD_BASH_STUN;
    
    // Tier 3 Right-Right - Revenge
    auto revenge = tree.insertRight(ironSkin,
        Skill("revenge", "Revenge", "passive", "Deal 50% of damage taken back to attacker", 3, 0));
    revenge->data.damageResistance = 0.0f;
}

void SkillTree::initializeMageBranch() {
    // Tier 1 Right - Mana Surge (Hotkey 4)
    auto manaSurge = tree.insertRight(root, 
        Skill("mana_surge", "Mana Surge", "active", "Restore 40 mana instantly", 1, 4));
    manaSurge->data.healing = MANA_SURGE_RESTORE;
    manaSurge->data.manaCost = 0;
    manaSurge->data.cooldown = MANA_SURGE_COOLDOWN;
    manaSurge->data.unlocked = true;
    
    // Tier 2 Left Branch - Fireball (Hotkey 5)
    auto fireball = tree.insertLeft(manaSurge, 
        Skill("fireball", "Fireball", "active", "Ranged fire attack - 45 damage", 2, 5));
    fireball->data.damage = FIREBALL_DAMAGE;
    fireball->data.manaCost = FIREBALL_MANA;
    fireball->data.cooldown = FIREBALL_COOLDOWN;
    fireball->data.unlocked = true;
    
    // Tier 3 Left-Left - Meteor Storm
    auto meteorStorm = tree.insertLeft(fireball,
        Skill("meteor_storm", "Meteor Storm", "active", "Massive AOE - 60 damage to all enemies", 4, 0));
    meteorStorm->data.damage = METEOR_DAMAGE;
    meteorStorm->data.aoe = true;
    meteorStorm->data.manaCost = METEOR_MANA;
    meteorStorm->data.cooldown = METEOR_COOLDOWN;
    
    // Tier 3 Left-Right - Mana Mastery
    auto manaMastery = tree.insertRight(fireball,
        Skill("mana_mastery", "Mana Mastery", "passive", "All spells cost 30% less mana", 2, 0));
    manaMastery->data.attackSpeedBonus = MANA_MASTERY_REDUCTION;
    
    // Tier 2 Right Branch - Shadow Step
    auto shadowStep = tree.insertRight(manaSurge, 
        Skill("shadow_step", "Shadow Step", "active", "Teleport 3 tiles, become invisible for 2 turns", 2, 5));
    shadowStep->data.dashDistance = SHADOW_STEP_DISTANCE;
    shadowStep->data.manaCost = SHADOW_STEP_MANA;
    shadowStep->data.cooldown = SHADOW_STEP_COOLDOWN;
    shadowStep->data.duration = SHADOW_STEP_INVIS;
    
    // Tier 3 Right-Left - Assassination
    auto assassination = tree.insertLeft(shadowStep,
        Skill("assassination", "Assassination", "active", "Backstab for 80 damage (requires invisibility)", 3, 0));
    assassination->data.damage = ASSASSINATION_DAMAGE;
    assassination->data.manaCost = ASSASSINATION_MANA;
    assassination->data.cooldown = ASSASSINATION_COOLDOWN;
    
    // Tier 3 Right-Right - Evasion
    auto evasion = tree.insertRight(shadowStep,
        Skill("evasion", "Evasion", "passive", "30% chance to dodge attacks", 2, 0));
    evasion->data.critChance = EVASION_DODGE_CHANCE;
}

// ═══════════════════════════════════════════════════════════════════════════
// SKILL UNLOCKING
// ═══════════════════════════════════════════════════════════════════════════

void SkillTree::unlockSkill(std::shared_ptr<BinaryTree<Skill>::Node> node) {
    if (!node) return;
    if (node->data.unlocked) return;
    if (!canUnlock(node)) return;
    
    node->data.unlocked = true;
    availablePoints -= node->data.cost;
}

bool SkillTree::canUnlock(std::shared_ptr<BinaryTree<Skill>::Node> node) const {
    if (!node) return false;
    if (node->data.unlocked) return false;
    if (availablePoints < node->data.cost) return false;
    
    // Root skill is always unlockable
    if (node == root) return true;
    
    // Check if parent is unlocked
    std::function<bool(std::shared_ptr<BinaryTree<Skill>::Node>, std::shared_ptr<BinaryTree<Skill>::Node>)> findParent;
    findParent = [&](std::shared_ptr<BinaryTree<Skill>::Node> current, 
                     std::shared_ptr<BinaryTree<Skill>::Node> target) -> bool {
        if (!current) return false;
        
        if (current->left == target || current->right == target) {
            return current->data.unlocked;
        }
        
        return findParent(current->left, target) || findParent(current->right, target);
    };
    
    return findParent(root, node);
}

void SkillTree::addPoints(int points) {
    availablePoints += points;
}

// ═══════════════════════════════════════════════════════════════════════════
// SKILL LOOKUP
// ═══════════════════════════════════════════════════════════════════════════

Skill* SkillTree::getSkillByHotkey(int hotkey) {
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

// ═══════════════════════════════════════════════════════════════════════════
// COOLDOWN MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════════

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

// ═══════════════════════════════════════════════════════════════════════════
// RENDERING
// ═══════════════════════════════════════════════════════════════════════════

void SkillTree::render(sf::RenderWindow& window, sf::Font& font, sf::Vector2f mousePos) {
    if (!root) return;
    
    std::function<void(std::shared_ptr<BinaryTree<Skill>::Node>, float, float, float)> renderNode;
    renderNode = [&](std::shared_ptr<BinaryTree<Skill>::Node> node, float x, float y, float width) {
        if (!node) return;
        
        const sf::Color lineColor(LINE_GRAY, LINE_GRAY, LINE_GRAY);
        
        // Draw connections to children
        if (node->left) {
            const float childX = x - width / 2.0f;
            const float childY = y + LEVEL_HEIGHT;
            
            sf::Vertex line[] = {
                sf::Vertex{{x, y}, lineColor},
                sf::Vertex{{childX, childY}, lineColor}
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);
            
            renderNode(node->left, childX, childY, width / 2.0f);
        }
        
        if (node->right) {
            const float childX = x + width / 2.0f;
            const float childY = y + LEVEL_HEIGHT;
            
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
        
        // Hover effect
        const float dist = pointDistance(mousePos.x, mousePos.y, x, y);
        const bool hovered = (dist <= NODE_RADIUS);
        
        if (hovered) {
            outlineColor = sf::Color::Cyan;
        }
        
        circle.setFillColor(fillColor);
        circle.setOutlineColor(outlineColor);
        circle.setOutlineThickness(hovered ? OUTLINE_HOVER : OUTLINE_NORMAL);
        window.draw(circle);
        
        // Draw icon
        sf::Text iconText(font);
        iconText.setString(node->data.name.substr(0, 1));
        iconText.setCharacterSize(ICON_FONT_SIZE);
        iconText.setFillColor(sf::Color::White);
        const sf::FloatRect bounds = iconText.getLocalBounds();
        iconText.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
        iconText.setPosition({x, y - ICON_Y_OFFSET});
        window.draw(iconText);
        
        // Draw tooltip on hover
        if (hovered) {
            renderTooltip(window, font, mousePos, node);
        }
    };
    
    renderNode(root, TREE_START_X, TREE_START_Y, BASE_WIDTH);
}

void SkillTree::renderTooltip(sf::RenderWindow& window, sf::Font& font, 
                              sf::Vector2f mousePos, 
                              std::shared_ptr<BinaryTree<Skill>::Node> node) const {
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

void SkillTree::displayTree() {
    tree.levelOrder([](const Skill& skill) {
        // Silent traversal - no console output
    });
}

// ═══════════════════════════════════════════════════════════════════════════
// CLICK HANDLING
// ═══════════════════════════════════════════════════════════════════════════

bool SkillTree::tryUnlockAt(sf::Vector2f mousePos) {
    if (!root) return false;
    
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
        
        const float dist = pointDistance(mousePos.x, mousePos.y, x, y);
        
        if (dist <= NODE_RADIUS && !node->data.unlocked && canUnlock(node)) {
            unlockSkill(node);
            unlockedSomething = true;
            unlockAnimations.push_back({node->data.id, 0.f, 1.0f});
        }
    };
    
    checkClick(root, TREE_START_X, TREE_START_Y, BASE_WIDTH);
    return unlockedSomething;
}

// ═══════════════════════════════════════════════════════════════════════════
// SYNERGY SYSTEM
// ═══════════════════════════════════════════════════════════════════════════

void SkillTree::initializeSynergies() {
    synergies.clear();
    
    // Warrior synergies
    synergies.push_back({"power_strike", "iron_skin", "damage", SYNERGY_POWER_IRON});
    synergies.push_back({"whirlwind", "berserker_rage", "attack_speed", SYNERGY_WHIRL_RAGE});
    synergies.push_back({"flame_wave", "whirlwind", "aoe_damage", SYNERGY_FLAME_WHIRL});
    
    // Mage synergies
    synergies.push_back({"fireball", "mana_mastery", "spell_damage", SYNERGY_FIRE_MANA});
    synergies.push_back({"meteor_storm", "fireball", "critical", SYNERGY_METEOR_FIRE});
    
    // Rogue synergies  
    synergies.push_back({"shadow_step", "backstab", "stealth_damage", SYNERGY_SHADOW_BACK});
    synergies.push_back({"poison_blade", "backstab", "dot_damage", SYNERGY_POISON_BACK});
    
    // Cross-class synergies
    synergies.push_back({"iron_skin", "regeneration", "max_hp", SYNERGY_IRON_REGEN});
    synergies.push_back({"mana_mastery", "regeneration", "mana_regen", SYNERGY_MANA_REGEN});
}

float SkillTree::getSynergyBonus(const std::string& bonusType) const {
    float total = 0.f;
    
    for (const auto& synergy : synergies) {
        bool skill1Unlocked = false;
        bool skill2Unlocked = false;
        
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

// ═══════════════════════════════════════════════════════════════════════════
// ANIMATION SYSTEM
// ═══════════════════════════════════════════════════════════════════════════

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

// ═══════════════════════════════════════════════════════════════════════════
// PASSIVE BONUSES
// ═══════════════════════════════════════════════════════════════════════════

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

