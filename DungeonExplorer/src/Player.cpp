#include "Player.h"
#include "AssetManager.h"
#include "SoundManager.h"
#include "GameUtils.h"

// ═══════════════════════════════════════════════════════════════════════════
// PLAYER CONSTANTS
// ═══════════════════════════════════════════════════════════════════════════

namespace {
    // Class Base Stats: {maxHealth, maxMana, attack, defense, moveSpeed}
    struct ClassStats {
        int maxHealth;
        int maxMana;
        int attack;
        int defense;
        float moveSpeed;
    };
    
    constexpr ClassStats WARRIOR_STATS = {120, 40, 18, 15, 100.0f};
    constexpr ClassStats ROGUE_STATS   = {90, 50, 20, 8, 120.0f};
    constexpr ClassStats MAGE_STATS    = {80, 100, 25, 5, 100.0f};
    constexpr ClassStats DEFAULT_STATS = {100, 50, 10, 5, 100.0f};

    // Level-up bonuses
    constexpr int LEVEL_HP_BONUS = 20;
    constexpr int LEVEL_MANA_BONUS = 10;
    constexpr int LEVEL_ATTACK_BONUS = 2;
    constexpr int LEVEL_DEFENSE_BONUS = 1;
    constexpr int XP_PER_LEVEL_MULTIPLIER = 100;
    constexpr int BONUS_SKILL_POINT_LEVEL_INTERVAL = 5;
    
    // Inventory
    constexpr int MAX_PATH_HISTORY = 50;
    constexpr int MIN_DAMAGE = 1;
    
    // Animation thresholds
    constexpr float MOVEMENT_THRESHOLD = 0.1f;
    constexpr float DIRECTION_THRESHOLD = 0.01f;
    
    // Rendering
    constexpr float SHADOW_RADIUS = 10.0f;
    constexpr float SHADOW_ALPHA = 80.0f;
    constexpr float PLAYER_SCALE = 1.2f;
    constexpr float ATTACK_SCALE_BOOST = 1.15f;
    constexpr float FALLBACK_CIRCLE_SCALE = 0.35f;
}

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTOR & INITIALIZATION
// ═══════════════════════════════════════════════════════════════════════════

Player::Player() 
    : position(0, 0), health(DEFAULT_STATS.maxHealth), maxHealth(DEFAULT_STATS.maxHealth), 
      mana(DEFAULT_STATS.maxMana), maxMana(DEFAULT_STATS.maxMana), 
      experience(0), level(1), attack(DEFAULT_STATS.attack), defense(DEFAULT_STATS.defense), 
      gold(0), skillPointsToGrant(0),
      name("Adventurer"), characterClass("Warrior"), moveSpeed(DEFAULT_STATS.moveSpeed),
      equippedWeapon(nullptr), equippedArmor(nullptr) {
}

Player::~Player() {
    // Smart pointers handle cleanup
}

void Player::initialize(int startX, int startY) {
    position = Position(startX, startY);
    pathHistory.clear();
}

void Player::setCharacter(const std::string& playerName, const std::string& charClass) {
    name = playerName;
    characterClass = charClass;
    
    // Apply class-specific stats
    ClassStats stats = DEFAULT_STATS;
    if (charClass == "Warrior") {
        stats = WARRIOR_STATS;
    } else if (charClass == "Rogue") {
        stats = ROGUE_STATS;
    } else if (charClass == "Mage") {
        stats = MAGE_STATS;
    }
    
    maxHealth = stats.maxHealth;
    health = stats.maxHealth;
    maxMana = stats.maxMana;
    mana = stats.maxMana;
    attack = stats.attack;
    defense = stats.defense;
    moveSpeed = stats.moveSpeed;
}

// ═══════════════════════════════════════════════════════════════════════════
// MOVEMENT & PATHFINDING (DSA: Stack for backtracking)
// ═══════════════════════════════════════════════════════════════════════════

void Player::moveTo(const Position& newPos) {
    pathHistory.push(position);
    position = newPos;
    
    // Limit path history size
    if (pathHistory.size() > MAX_PATH_HISTORY) {
        const int currentSize = static_cast<int>(pathHistory.size());
        for (int i = 0; i < currentSize - MAX_PATH_HISTORY; ++i) {
            pathHistory.pop();
        }
    }
}

void Player::backtrack() {
    if (!pathHistory.isEmpty()) {
        position = pathHistory.top();
        pathHistory.pop();
    }
}

void Player::move(int dx, int dy) {
    Position newPos(position.x + dx, position.y + dy);
    moveTo(newPos);
}

// ═══════════════════════════════════════════════════════════════════════════
// COMBAT
// ═══════════════════════════════════════════════════════════════════════════

int Player::attackEnemy() {
    const float pitchVariation = 0.9f + (static_cast<float>(std::rand() % 20) / 100.0f);
    SoundManager::getInstance().playSound("swing", pitchVariation, 0.8f);
    return attack;
}

void Player::takeDamage(int damage) {
    if (damage < 0) return;
    
    int actualDamage = damage - (defense / 2);
    if (actualDamage < MIN_DAMAGE) actualDamage = MIN_DAMAGE;
    
    const int oldHealth = health;
    health -= actualDamage;
    health = std::clamp(health, 0, maxHealth);
    
    const int damageTaken = oldHealth - health;
    if (damageTaken > 0) {
        SoundManager::getInstance().playSound("hit", 0.8f, 0.9f);
    }
}

void Player::heal(int amount) {
    if (amount <= 0) return;
    
    health += amount;
    health = std::clamp(health, 0, maxHealth);
}

// ═══════════════════════════════════════════════════════════════════════════
// ITEM MANAGEMENT (DSA: LinkedList traversal)
// ═══════════════════════════════════════════════════════════════════════════

void Player::addItem(const ItemNew& item) {
    // Try to stack with existing item of same ID
    for (auto& existingItem : inventoryNew) {
        if (existingItem.id == item.id && existingItem.stackCount < MAX_STACK_SIZE) {
            const int spaceAvailable = MAX_STACK_SIZE - existingItem.stackCount;
            const int toAdd = std::min(item.stackCount, spaceAvailable);
            existingItem.stackCount += toAdd;
            
            // Create new stack with remainder if needed
            if (item.stackCount > toAdd) {
                ItemNew remainder = item;
                remainder.stackCount = item.stackCount - toAdd;
                inventoryNew.append(remainder);
            }
            return;
        }
    }
    
    // No existing stack found - add new entry
    inventoryNew.append(item);
}

bool Player::removeItemNew(const std::string& itemId) {
    for (const auto& item : inventoryNew) {
        if (item.id == itemId) {
            inventoryNew.remove(item);
            return true;
        }
    }
    return false;
}

bool Player::hasItem(const std::string& itemName) const {
    for (const auto& item : inventoryNew) {
        if (item.name == itemName || item.id == itemName) {
            return true;
        }
    }
    return false;
}

void Player::swapInventoryItems(int index1, int index2) {
    inventoryNew.swap(index1, index2);
}

// ═══════════════════════════════════════════════════════════════════════════
// ITEM USAGE SYSTEM (DSA: LinkedList search + action dispatch)
// ═══════════════════════════════════════════════════════════════════════════

bool Player::useItem(const std::string& itemId) {
    // Find item in inventory (O(n) LinkedList search)
    ItemNew* foundItem = nullptr;
    
    for (auto& item : inventoryNew) {
        if (item.id == itemId) {
            foundItem = &item;
            break;
        }
    }
    
    if (!foundItem) {
        return false;
    }
    
    const std::string& action = foundItem->action.kind;
    const auto& params = foundItem->action.params;
    
    // Action dispatch
    if (action == "heal") {
        const int healAmount = params.contains("amount") ? params["amount"].get<int>() : 50;
        heal(healAmount);
        SoundManager::getInstance().playSound("heal", 1.0f, 0.7f);
        hurtAnimTimer = ANIM_HURT_DURATION;  // Visual feedback
        
        foundItem->stackCount--;
        if (foundItem->stackCount <= 0) {
            removeItemNew(itemId);
        }
        return true;
    }
    
    if (action == "equip") {
        return handleEquipAction(foundItem, params, itemId);
    }
    
    if (action == "buff") {
        if (params.contains("attackBonus")) {
            attack += params["attackBonus"].get<int>();
        }
        if (params.contains("defenseBonus")) {
            defense += params["defenseBonus"].get<int>();
        }
        
        foundItem->stackCount--;
        if (foundItem->stackCount <= 0) {
            removeItemNew(itemId);
        }
        return true;
    }
    
    if (action == "collect") {
        gold += foundItem->value;
        removeItemNew(itemId);
        return true;
    }
    
    if (action == "attack") {
        attackAnimTimer = ANIM_ATTACK_DURATION;
        foundItem->stackCount--;
        if (foundItem->stackCount <= 0) {
            removeItemNew(itemId);
        }
        return true;  // Actual damage dealt by Game class
    }
    
    return false;  // Unknown action
}

bool Player::handleEquipAction(ItemNew* item, const nlohmann::json& params, const std::string& itemId) {
    if (item->type == "weapon") {
        // Unequip current weapon first
        if (equippedWeapon) {
            addItem(*equippedWeapon);
        }
        
        equippedWeapon = std::make_unique<ItemNew>(*item);
        
        // Apply attack bonus
        if (params.contains("attack_bonus")) {
            attack += params["attack_bonus"].get<int>();
        } else if (params.contains("damageBonus")) {
            attack += params["damageBonus"].get<int>();
        }
        
        removeItemNew(itemId);
        return true;
    }
    
    if (item->type == "armor") {
        // Unequip current armor first
        if (equippedArmor) {
            addItem(*equippedArmor);
        }
        
        equippedArmor = std::make_unique<ItemNew>(*item);
        
        if (params.contains("defenseBonus")) {
            defense += params["defenseBonus"].get<int>();
        }
        
        removeItemNew(itemId);
        return true;
    }
    
    return false;
}

bool Player::useHotbarItem(int slot) {
    const int index = slot - 1;
    
    if (index < 0 || index >= static_cast<int>(inventoryNew.size())) {
        return false;
    }
    
    // O(n) LinkedList access to find item at index
    int currentIndex = 0;
    for (const auto& item : inventoryNew) {
        if (currentIndex == index) {
            return useItem(item.id);
        }
        ++currentIndex;
    }
    
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// EQUIPMENT MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════════

bool Player::unequipWeapon() {
    if (!equippedWeapon) {
        return false;
    }
    
    addItem(*equippedWeapon);
    equippedWeapon.reset();
    return true;
}

bool Player::unequipArmor() {
    if (!equippedArmor) {
        return false;
    }
    
    addItem(*equippedArmor);
    equippedArmor.reset();
    return true;
}

bool Player::usePotion() {
    if (health >= maxHealth) {
        return false;  // Already at full health
    }
    
    for (const auto& item : inventoryNew) {
        if (item.type == "consumable" && item.action.kind == "heal") {
            return useItem(item.id);
        }
    }
    
    return false;  // No potions found
}

// ═══════════════════════════════════════════════════════════════════════════
// MANA & GOLD
// ═══════════════════════════════════════════════════════════════════════════

bool Player::useMana(int amount) {
    if (amount < 0 || mana < amount) {
        return false;
    }
    
    mana -= amount;
    mana = std::clamp(mana, 0, maxMana);
    return true;
}

void Player::restoreMana(int amount) {
    if (amount <= 0) return;
    
    mana += amount;
    mana = std::min(mana, maxMana);
}

void Player::addGold(int amount) {
    if (amount > 0) {
        gold += amount;
    }
}

bool Player::spendGold(int amount) {
    if (amount < 0 || gold < amount) {
        return false;
    }
    
    gold -= amount;
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// EXPERIENCE & LEVELING
// ═══════════════════════════════════════════════════════════════════════════

void Player::addExperience(int xp) {
    experience += xp;
    
    int xpForNextLevel = level * XP_PER_LEVEL_MULTIPLIER;
    while (experience >= xpForNextLevel) {
        level++;
        experience -= xpForNextLevel;
        
        // Apply level-up bonuses
        maxHealth += LEVEL_HP_BONUS;
        health = maxHealth;
        maxMana += LEVEL_MANA_BONUS;
        mana = maxMana;
        attack += LEVEL_ATTACK_BONUS;
        defense += LEVEL_DEFENSE_BONUS;
        
        // Grant skill points (bonus every 5 levels)
        const int pointsThisLevel = (level % BONUS_SKILL_POINT_LEVEL_INTERVAL == 0) ? 2 : 1;
        skillPointsToGrant += pointsThisLevel;
        
        // Play level-up sound
        SoundManager::getInstance().playSound("levelup", 1.0f, 0.8f);
        
        xpForNextLevel = level * XP_PER_LEVEL_MULTIPLIER;
    }
}

int Player::getSkillPointsEarned() const {
    return skillPointsToGrant;
}

void Player::clearSkillPoints() {
    skillPointsToGrant = 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// UPDATE LOOP
// ═══════════════════════════════════════════════════════════════════════════

void Player::update(float deltaTime) {
    animTimer += deltaTime;
    
    // Dash cooldown
    if (dashCooldown > 0.f) {
        dashCooldown -= deltaTime;
    }
    
    if (isDashing) {
        dashTimer -= deltaTime;
        if (dashTimer <= 0.f) {
            isDashing = false;
        }
    }
    
    // Smooth visual position interpolation
    const float targetX = static_cast<float>(position.x);
    const float targetY = static_cast<float>(position.y);
    const float lerpSpeed = VISUAL_LERP_SPEED * deltaTime;
    
    const float prevVisualX = visualX;
    const float prevVisualY = visualY;
    
    visualX += (targetX - visualX) * lerpSpeed;
    visualY += (targetY - visualY) * lerpSpeed;
    
    // Update facing direction
    if (visualX > prevVisualX + DIRECTION_THRESHOLD) {
        facingDir = 1;
    } else if (visualX < prevVisualX - DIRECTION_THRESHOLD) {
        facingDir = -1;
    }
    
    // Animation state machine
    if (attackAnimTimer > 0.f) {
        attackAnimTimer -= deltaTime;
        currentAnim = AnimState::Attacking;
    } else if (hurtAnimTimer > 0.f) {
        hurtAnimTimer -= deltaTime;
        currentAnim = AnimState::Hurt;
    } else if (std::abs(visualX - targetX) > MOVEMENT_THRESHOLD || 
               std::abs(visualY - targetY) > MOVEMENT_THRESHOLD) {
        currentAnim = AnimState::Walking;
    } else {
        currentAnim = AnimState::Idle;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// RENDERING
// ═══════════════════════════════════════════════════════════════════════════

void Player::render(sf::RenderWindow& window, float tileSize) const {
    const float renderX = visualX * tileSize;
    const float renderY = visualY * tileSize;
    
    // Animation effects
    float bobY = 0.f;
    float scaleBoost = 1.0f;
    float rotation = 0.f;
    sf::Color tintColor = sf::Color::White;
    
    switch (currentAnim) {
        case AnimState::Walking:
            bobY = std::sin(animTimer * 15.f) * 2.f;
            break;
        case AnimState::Attacking:
            scaleBoost = ATTACK_SCALE_BOOST;
            rotation = std::sin(animTimer * 30.f) * 5.f;
            break;
        case AnimState::Hurt:
            tintColor = sf::Color(255, 100, 100);
            bobY = std::sin(animTimer * 40.f) * 3.f;
            break;
        case AnimState::Idle:
            bobY = std::sin(animTimer * 2.f) * 1.f;
            break;
        default:
            break;
    }
    
    // Shadow
    sf::CircleShape shadow(SHADOW_RADIUS);
    shadow.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(SHADOW_ALPHA)));
    shadow.setScale(sf::Vector2f(2.0f, 0.5f));
    shadow.setPosition(sf::Vector2f(renderX + 6.0f, renderY + 28.0f));
    window.draw(shadow);
    
    // Select texture based on animation state
    std::string textureKey = "player_wizard_idle";
    if (characterClass == "Mage" || characterClass == "Rogue" || characterClass == "Warrior") {
        switch (currentAnim) {
            case AnimState::Attacking:
            case AnimState::Casting:
                textureKey = "player_wizard_fire";
                break;
            case AnimState::Hurt:
                textureKey = "player_wizard_hurt";
                break;
            default:
                textureKey = "player_wizard_idle";
                break;
        }
    }

    sf::Texture* playerTexture = AssetManager::getInstance().getTexture(textureKey);
    if (playerTexture) {
        sf::Sprite playerSprite(*playerTexture);
        const sf::FloatRect bounds = playerSprite.getLocalBounds();
        playerSprite.setOrigin(sf::Vector2f(bounds.size.x / 2.0f, bounds.size.y / 2.0f));
        playerSprite.setPosition(sf::Vector2f(
            renderX + tileSize / 2.0f, 
            renderY + tileSize / 2.0f + bobY
        ));
        
        const float scaleX = (tileSize * PLAYER_SCALE * scaleBoost) / bounds.size.x * static_cast<float>(facingDir);
        const float scaleY = (tileSize * PLAYER_SCALE * scaleBoost) / bounds.size.y;
        playerSprite.setScale(sf::Vector2f(scaleX, scaleY));
        playerSprite.setRotation(sf::degrees(rotation));
        playerSprite.setColor(tintColor);
        
        window.draw(playerSprite);
    } else {
        // Fallback circle rendering
        const float radius = tileSize * FALLBACK_CIRCLE_SCALE;
        sf::CircleShape playerCircle(radius);
        playerCircle.setPosition(sf::Vector2f(
            renderX + tileSize * 0.15f, 
            renderY + tileSize * 0.15f + bobY
        ));
        playerCircle.setFillColor(sf::Color(0, 220, 255));
        playerCircle.setOutlineThickness(2.5f);
        playerCircle.setOutlineColor(sf::Color::White);
        window.draw(playerCircle);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// DASH MECHANIC
// ═══════════════════════════════════════════════════════════════════════════

void Player::performDash(int dirX, int dirY) {
    if (!canDash()) {
        return;
    }
    
    if (dirX == 0 && dirY == 0) {
        return;
    }
    
    isDashing = true;
    dashTimer = DASH_DURATION;
    dashCooldown = DASH_COOLDOWN_TIME;
    dashDirX = dirX;
    dashDirY = dirY;
    
    // Apply dash movement
    position.x += dirX * DASH_DISTANCE;
    position.y += dirY * DASH_DISTANCE;
    
    SoundManager::getInstance().playSound("dash", 1.2f, 0.6f);
}

