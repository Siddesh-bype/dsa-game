// CHANGE: 2025-12-04 - Added GameUtils for consolidated utilities
#include "Player.h"
#include "AssetManager.h"
#include "SoundManager.h"
#include "GameUtils.h"
#include <iostream>

Player::Player() 
    : position(0, 0), health(100), maxHealth(100), mana(50), maxMana(50), 
      experience(0), level(1), attack(10), defense(5), gold(0), skillPointsToGrant(0),
      name("Adventurer"), characterClass("Warrior"), moveSpeed(100.0f),
      equippedWeapon(nullptr), equippedArmor(nullptr) {
}

Player::~Player() {
    // Smart pointers handle cleanup automatically
}

void Player::initialize(int startX, int startY) {
    position = Position(startX, startY);
    pathHistory.clear();
    std::cout << "[Player] Initialized at position (" << startX << ", " << startY << ")" << std::endl;
}

void Player::setCharacter(const std::string& playerName, const std::string& charClass) {
    name = playerName;
    characterClass = charClass;
    
    // Apply class bonuses
    if (characterClass == "Warrior") {
        maxHealth = 120;
        health = 120;
        maxMana = 40;
        mana = 40;
        attack = 18;
        defense = 15;
    } else if (characterClass == "Rogue") {
        maxHealth = 90;
        health = 90;
        maxMana = 50;
        mana = 50;
        attack = 20;
        defense = 8;
        moveSpeed = 120.0f;
    } else if (characterClass == "Mage") {
        maxHealth = 80;
        health = 80;
        maxMana = 100;
        mana = 100;
        attack = 25;
        defense = 5;
    }
    
    std::cout << "[Player] Created " << name << " the " << charClass << std::endl;
    std::cout << "  HP: " << maxHealth << " | MP: " << maxMana << " | ATK: " << attack << " | DEF: " << defense << std::endl;
}

void Player::moveTo(const Position& newPos) {
    pathHistory.push(position);
    position = newPos;
    
    if (pathHistory.size() > 50) {
        Stack<Position> tempHistory;
        int currentSize = pathHistory.size();
        for (int i = 0; i < currentSize - 50; i++) {
            pathHistory.pop();
        }
    }
}

void Player::backtrack() {
    if (!pathHistory.isEmpty()) {
        position = pathHistory.top();
        pathHistory.pop();
        std::cout << "[Player] Backtracked to (" << position.x << ", " << position.y << ")" << std::endl;
    } else {
        std::cout << "[Player] Cannot backtrack - at starting position" << std::endl;
    }
}

void Player::move(int dx, int dy) {
    Position newPos(position.x + dx, position.y + dy);
    moveTo(newPos);
}

int Player::attackEnemy() {
    SoundManager::getInstance().playSound("swing", 0.9f + ((std::rand() % 20) / 100.0f), 0.8f);
    return attack;
}

void Player::addItem(const ItemNew& item) {
    inventoryNew.append(item);
    std::cout << "[Player] Added item: " << item.name << " (rarity: " << item.getRarityName() << ", value: " << item.value << ")" << std::endl;
}

void Player::takeDamage(int damage) {
    if (damage < 0) {
        std::cout << "[Player] ERROR: Negative damage amount: " << damage << std::endl;
        return;
    }
    
    int actualDamage = damage - (defense / 2);
    if (actualDamage < 1) actualDamage = 1;
    
    int oldHealth = health;
    health -= actualDamage;
    health = std::max(0, std::min(health, maxHealth));
    
    int damageTaken = oldHealth - health;
    if (damageTaken > 0) {
        SoundManager::getInstance().playSound("hit", 0.8f, 0.9f);
    }
    std::cout << "[DEBUG] Player HP " << health << "/" << maxHealth << " (took " << damageTaken << " damage)" << std::endl;
}

void Player::heal(int amount) {
    if (amount <= 0) {
        std::cout << "[Player] ERROR: Invalid heal amount: " << amount << std::endl;
        return;
    }
    
    int oldHealth = health;
    health += amount;
    health = std::max(0, std::min(health, maxHealth));
    
    int actualHealed = health - oldHealth;
    std::cout << "[DEBUG] Player HP " << health << "/" << maxHealth << " (healed " << actualHealed << ")" << std::endl;
}

// ═══════════════════════════════════════════════════════════════════════════
// ITEM USAGE SYSTEM (DSA: LinkedList traversal + action dispatch)
// ═══════════════════════════════════════════════════════════════════════════
bool Player::useItem(const std::string& itemId) {
    // Find item in inventory (O(n) LinkedList search)
    ItemNew* foundItem = nullptr;
    int itemIndex = 0;
    
    for (auto& item : inventoryNew) {
        if (item.id == itemId) {
            foundItem = &item;
            break;
        }
        itemIndex++;
    }
    
    if (!foundItem) {
        std::cout << "[Player] Item not found: " << itemId << std::endl;
        return false;
    }
    
    const std::string& action = foundItem->action.kind;
    const auto& params = foundItem->action.params;
    
    std::cout << "[Player] Using item: " << foundItem->name << " (action: " << action << ")" << std::endl;
    
    // ═══════════════════════════════════════════════════════════════════════
    // ACTION DISPATCH - Handle different item actions
    // ═══════════════════════════════════════════════════════════════════════
    
    if (action == "heal") {
        // Heal action
        int healAmount = params.contains("amount") ? params["amount"].get<int>() : 50;
        heal(healAmount);
        SoundManager::getInstance().playSound("heal", 1.0f, 0.7f);
        
        // Trigger hurt animation (reuses the visual feedback)
        hurtAnimTimer = ANIM_HURT_DURATION;
        
        // Remove consumable after use
        removeItemNew(itemId);
        std::cout << "[Player] Healed for " << healAmount << " HP!" << std::endl;
        return true;
    }
    else if (action == "equip") {
        // Equip weapon/armor
        if (foundItem->type == "weapon") {
            // Unequip current weapon first
            if (equippedWeapon) {
                std::cout << "[Player] Unequipping " << equippedWeapon->name << std::endl;
                // Return old weapon to inventory (copy before move)
                addItem(*equippedWeapon);
            }
            
            // Equip new weapon
            equippedWeapon = std::make_unique<ItemNew>(*foundItem);
            
            // Apply attack bonus
            if (params.contains("attack_bonus")) {
                attack += params["attack_bonus"].get<int>();
            } else if (params.contains("damageBonus")) {
                attack += params["damageBonus"].get<int>();
            }
            
            removeItemNew(itemId);
            std::cout << "[Player] Equipped weapon: " << equippedWeapon->name << " (ATK now: " << attack << ")" << std::endl;
            return true;
        }
        else if (foundItem->type == "armor") {
            // Unequip current armor first  
            if (equippedArmor) {
                std::cout << "[Player] Unequipping " << equippedArmor->name << std::endl;
                addItem(*equippedArmor);
            }
            
            // Equip new armor
            equippedArmor = std::make_unique<ItemNew>(*foundItem);
            
            // Apply defense bonus
            if (params.contains("defenseBonus")) {
                defense += params["defenseBonus"].get<int>();
            }
            
            removeItemNew(itemId);
            std::cout << "[Player] Equipped armor: " << equippedArmor->name << " (DEF now: " << defense << ")" << std::endl;
            return true;
        }
    }
    else if (action == "buff") {
        // Temporary buff (simplified - immediate stats boost)
        if (params.contains("attackBonus")) {
            attack += params["attackBonus"].get<int>();
            std::cout << "[Player] Attack boosted! ATK now: " << attack << std::endl;
        }
        if (params.contains("defenseBonus")) {
            defense += params["defenseBonus"].get<int>();
            std::cout << "[Player] Defense boosted! DEF now: " << defense << std::endl;
        }
        removeItemNew(itemId);
        return true;
    }
    else if (action == "collect") {
        // Treasure items just add gold
        gold += foundItem->value;
        removeItemNew(itemId);
        std::cout << "[Player] Collected treasure worth " << foundItem->value << " gold! Total: " << gold << std::endl;
        return true;
    }
    else if (action == "attack") {
        // Attack items (bombs, scrolls)
        std::cout << "[Player] Used attack item: " << foundItem->name << std::endl;
        attackAnimTimer = ANIM_ATTACK_DURATION;
        removeItemNew(itemId);
        return true;  // Actual damage dealt by Game class
    }
    
    std::cout << "[Player] Unknown action: " << action << std::endl;
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// HOTBAR ITEM USAGE (DSA: LinkedList index access)
// ═══════════════════════════════════════════════════════════════════════════
bool Player::useHotbarItem(int slot) {
    // Slot 1-9 maps to inventory index 0-8
    int index = slot - 1;
    
    if (index < 0 || index >= static_cast<int>(inventoryNew.size())) {
        std::cout << "[Player] Hotbar slot " << slot << " is empty" << std::endl;
        return false;
    }
    
    // Get item at index (O(n) LinkedList access)
    int currentIndex = 0;
    for (const auto& item : inventoryNew) {
        if (currentIndex == index) {
            return useItem(item.id);
        }
        currentIndex++;
    }
    
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════
// EQUIPMENT MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════════
bool Player::unequipWeapon() {
    if (!equippedWeapon) {
        std::cout << "[Player] No weapon equipped" << std::endl;
        return false;
    }
    
    // Return weapon to inventory
    addItem(*equippedWeapon);
    std::cout << "[Player] Unequipped weapon: " << equippedWeapon->name << std::endl;
    equippedWeapon.reset();
    return true;
}

bool Player::unequipArmor() {
    if (!equippedArmor) {
        std::cout << "[Player] No armor equipped" << std::endl;
        return false;
    }
    
    // Return armor to inventory
    addItem(*equippedArmor);
    std::cout << "[Player] Unequipped armor: " << equippedArmor->name << std::endl;
    equippedArmor.reset();
    return true;
}

bool Player::usePotion() {
    if (health >= maxHealth) {
        std::cout << "[Player] Already at full health!" << std::endl;
        return false;
    }
    
    // OPTIMIZATION: Use range-based for loop with new iterator support
    for (const auto& item : inventoryNew) {
        if (item.type == "consumable" && item.action.kind == "heal") {
            return useItem(item.id);
        }
    }
    
    std::cout << "[Player] No potions in inventory!" << std::endl;
    return false;
}

bool Player::hasItem(const std::string& itemName) const {
    // OPTIMIZATION: Use iterator find() method for O(n) search with early exit
    auto it = inventoryNew.find(ItemNew());
    
    // Fallback to range-based for loop for actual search (find needs equality operator)
    for (const auto& item : inventoryNew) {
        if (item.name == itemName || item.id == itemName) {
            return true;
        }
    }
    return false;
}

void Player::addExperience(int xp) {
    experience += xp;
    std::cout << "[Player] Gained " << xp << " XP. Total: " << experience << std::endl;
    
    int xpForNextLevel = level * 100;
    while (experience >= xpForNextLevel) {
        level++;
        experience -= xpForNextLevel;
        
        maxHealth += 20;
        health = maxHealth;
        maxMana += 10;
        mana = maxMana;
        attack += 2;
        defense += 1;
        
        int pointsThisLevel = (level % 5 == 0) ? 2 : 1;
        skillPointsToGrant += pointsThisLevel;
        
        std::cout << "╔═══════════════════════════════════════╗" << std::endl;
        std::cout << "║        🎉 LEVEL UP! Level " << level << "        ║" << std::endl;
        std::cout << "╠═══════════════════════════════════════╣" << std::endl;
        std::cout << "║  ❤️  Max HP:    " << maxHealth << " (+" << 20 << ")          ║" << std::endl;
        std::cout << "║  💙 Max Mana:  " << maxMana << " (+" << 10 << ")           ║" << std::endl;
        std::cout << "║  ⚔️  Attack:    " << attack << " (+" << 2 << ")           ║" << std::endl;
        std::cout << "║  🛡️  Defense:   " << defense << " (+" << 1 << ")            ║" << std::endl;
        std::cout << "║  ⭐ Skill Pts: +" << pointsThisLevel << "                  ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════╝" << std::endl;
        
        xpForNextLevel = level * 100;
    }
}

int Player::getSkillPointsEarned() const {
    return skillPointsToGrant;
}

void Player::clearSkillPoints() {
    skillPointsToGrant = 0;
}

void Player::update(float deltaTime) {
    // ═══════════════════════════════════════════════════════════════════════
    // ANIMATION TIMER - Increment for animation frames
    // ═══════════════════════════════════════════════════════════════════════
    animTimer += deltaTime;
    
    // ═══════════════════════════════════════════════════════════════════════
    // DASH COOLDOWN - Tick down when not dashing
    // ═══════════════════════════════════════════════════════════════════════
    if (dashCooldown > 0.f) {
        dashCooldown -= deltaTime;
    }
    
    if (isDashing) {
        dashTimer -= deltaTime;
        if (dashTimer <= 0.f) {
            isDashing = false;
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // SMOOTH VISUAL POSITION - Lerp towards grid position
    // ═══════════════════════════════════════════════════════════════════════
    float targetX = static_cast<float>(position.x);
    float targetY = static_cast<float>(position.y);
    float lerpSpeed = VISUAL_LERP_SPEED * deltaTime;
    
    float prevVisualX = visualX;
    float prevVisualY = visualY;
    
    visualX += (targetX - visualX) * lerpSpeed;
    visualY += (targetY - visualY) * lerpSpeed;
    
    // ═══════════════════════════════════════════════════════════════════════
    // ANIMATION STATE MACHINE
    // ═══════════════════════════════════════════════════════════════════════
    
    // Update facing direction based on movement
    if (visualX > prevVisualX + 0.01f) facingDir = 1;   // Moving right
    else if (visualX < prevVisualX - 0.01f) facingDir = -1;  // Moving left
    
    // Attack animation countdown
    if (attackAnimTimer > 0.f) {
        attackAnimTimer -= deltaTime;
        currentAnim = AnimState::Attacking;
    }
    // Hurt animation countdown
    else if (hurtAnimTimer > 0.f) {
        hurtAnimTimer -= deltaTime;
        currentAnim = AnimState::Hurt;
    }
    // Walking if moving (visual not matching grid)
    else if (std::abs(visualX - targetX) > 0.1f || std::abs(visualY - targetY) > 0.1f) {
        currentAnim = AnimState::Walking;
    }
    // Idle otherwise
    else {
        currentAnim = AnimState::Idle;
    }
}

void Player::render(sf::RenderWindow& window, float tileSize) const {
    // Use smooth visual position for rendering
    float renderX = visualX * tileSize;
    float renderY = visualY * tileSize;
    
    // Animation effects
    float bobY = 0.f;
    float scaleBoost = 1.0f;
    float rotation = 0.f;
    sf::Color tintColor = sf::Color::White;
    
    switch (currentAnim) {
        case AnimState::Walking:
            bobY = std::sin(animTimer * 15.f) * 2.f;  // Walking bob
            break;
        case AnimState::Attacking:
            scaleBoost = 1.15f;  // Scale up when attacking
            rotation = std::sin(animTimer * 30.f) * 5.f;  // Slight shake
            break;
        case AnimState::Hurt:
            tintColor = sf::Color(255, 100, 100);  // Red tint when hurt
            bobY = std::sin(animTimer * 40.f) * 3.f;  // Fast shake
            break;
        case AnimState::Idle:
            bobY = std::sin(animTimer * 2.f) * 1.f;  // Gentle idle bob
            break;
        default:
            break;
    }
    
    // Draw shadow
    sf::CircleShape shadow(10.0f);
    shadow.setFillColor(sf::Color(0, 0, 0, 80));
    shadow.setScale(sf::Vector2f(2.0f, 0.5f));
    shadow.setPosition(sf::Vector2f(renderX + 6.0f, renderY + 28.0f));
    window.draw(shadow);
    
    std::string textureKey = "player_warrior";
    if (characterClass == "Rogue") {
        textureKey = "player_rogue";
    } else if (characterClass == "Mage") {
        textureKey = "player_mage";
    }

    sf::Texture* playerTexture = AssetManager::getInstance().getTexture(textureKey);
    if (playerTexture) {
        sf::Sprite playerSprite(*playerTexture);
        sf::FloatRect bounds = playerSprite.getLocalBounds();
        playerSprite.setOrigin(sf::Vector2f(bounds.size.x / 2.0f, bounds.size.y / 2.0f));
        playerSprite.setPosition(sf::Vector2f(renderX + tileSize / 2.0f, renderY + tileSize / 2.0f + bobY));
        
        float scaleX = (tileSize * 1.2f * scaleBoost) / bounds.size.x * facingDir;
        float scaleY = (tileSize * 1.2f * scaleBoost) / bounds.size.y;
        playerSprite.setScale(sf::Vector2f(scaleX, scaleY));
        playerSprite.setRotation(sf::degrees(rotation));
        playerSprite.setColor(tintColor);
        
        window.draw(playerSprite);
    } else {
        float radius = tileSize * 0.35f;
        sf::CircleShape playerCircle(radius);
        playerCircle.setPosition(sf::Vector2f(renderX + tileSize * 0.15f, 
                                              renderY + tileSize * 0.15f + bobY));
        playerCircle.setFillColor(sf::Color(0, 220, 255));
        playerCircle.setOutlineThickness(2.5f);
        playerCircle.setOutlineColor(sf::Color::White);
        window.draw(playerCircle);
    }
}

void Player::swapInventoryItems(int index1, int index2) {
    inventoryNew.swap(index1, index2);
}

bool Player::useMana(int amount) {
    if (amount < 0) {
        std::cout << "[Player] ERROR: Negative mana cost: " << amount << std::endl;
        return false;
    }
    
    if (mana < amount) {
        std::cout << "[Player] Insufficient mana! Need " << amount << ", have " << mana << std::endl;
        return false;
    }
    
    mana -= amount;
    mana = std::max(0, std::min(mana, maxMana));
    std::cout << "[Player] Used " << amount << " mana. Remaining: " << mana << "/" << maxMana << std::endl;
    return true;
}

void Player::restoreMana(int amount) {
    if (amount <= 0) {
        std::cout << "[Player] ERROR: Invalid mana restore amount: " << amount << std::endl;
        return;
    }
    
    int oldMana = mana;
    mana += amount;
    mana = std::min(mana, maxMana);
    
    std::cout << "[Player] Restored " << (mana - oldMana) << " mana. Current: " << mana << "/" << maxMana << std::endl;
}

void Player::addGold(int amount) {
    if (amount < 0) return;
    gold += amount;
    std::cout << "[Player] Gained " << amount << " gold. Total: " << gold << std::endl;
}

bool Player::spendGold(int amount) {
    if (amount < 0) return false;
    if (gold >= amount) {
        gold -= amount;
        std::cout << "[Player] Spent " << amount << " gold. Remaining: " << gold << std::endl;
        return true;
    }
    std::cout << "[Player] Insufficient gold! Need " << amount << ", have " << gold << std::endl;
    return false;
}

bool Player::removeItemNew(const std::string& itemId) {
    // OPTIMIZATION: Use range-based for loop for cleaner code
    for (const auto& item : inventoryNew) {
        if (item.id == itemId) {
            inventoryNew.remove(item);
            std::cout << "[Player] Removed " << item.name << " from inventory" << std::endl;
            return true;
        }
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════════════
// DASH MECHANIC - Quick movement for dodging
// ═══════════════════════════════════════════════════════════════════════

void Player::performDash(int dirX, int dirY) {
    if (!canDash()) {
        std::cout << "[Player] Dash on cooldown!" << std::endl;
        return;
    }
    
    if (dirX == 0 && dirY == 0) {
        std::cout << "[Player] No direction for dash!" << std::endl;
        return;
    }
    
    isDashing = true;
    dashTimer = DASH_DURATION;
    dashCooldown = DASH_COOLDOWN_TIME;
    dashDirX = dirX;
    dashDirY = dirY;
    
    // Apply dash movement immediately
    position.x += dirX * DASH_DISTANCE;
    position.y += dirY * DASH_DISTANCE;
    
    std::cout << "[Player] DASH! (" << dirX << ", " << dirY << ") to (" 
              << position.x << ", " << position.y << ")" << std::endl;
    
    SoundManager::getInstance().playSound("dash", 1.2f, 0.6f);
}
