// CHANGE: 2025-11-10 — TASK E: HP clamping and display fixes
// - Ensured HP values always clamped between 0 and maxHP
// - Added debug logging for HP changes
// - Used std::clamp for safer bounds checking

#include "Player.h"
#include "AssetManager.h"
#include <iostream>
#include <algorithm>  // For std::clamp

Player::Player() 
    : position(0, 0), health(100), maxHealth(100), mana(50), maxMana(50),
      experience(0), level(1), attack(15), defense(10), gold(0), skillPointsToGrant(0),
      name("Adventurer"), characterClass("Warrior"), moveSpeed(100.0f),
      equippedWeapon(nullptr), equippedArmor(nullptr) {
}

Player::~Player() {
    // Clean up allocated memory for equipped items
    if (equippedWeapon != nullptr) {
        delete equippedWeapon;
        equippedWeapon = nullptr;
    }
    if (equippedArmor != nullptr) {
        delete equippedArmor;
        equippedArmor = nullptr;
    }
}

void Player::initialize(int startX, int startY) {
    position = Position(startX, startY);
    pathHistory.clear();
    pathHistory.push(position);
    
    std::cout << "[Player] " << name << " the " << characterClass << " initialized at position (" 
              << startX << ", " << startY << ")" << std::endl;
}

void Player::setCharacter(const std::string& playerName, const std::string& charClass) {
    name = playerName;
    characterClass = charClass;
    
    // Apply class bonuses
    if (charClass == "Warrior") {
        maxHealth = 120;
        health = 120;
        maxMana = 40;
        mana = 40;
        attack = 18;
        defense = 15;
    } else if (charClass == "Rogue") {
        maxHealth = 90;
        health = 90;
        maxMana = 50;
        mana = 50;
        attack = 20;
        defense = 8;
        moveSpeed = 120.0f;
    } else if (charClass == "Mage") {
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

void Player::move(int dx, int dy) {
    Position newPos(position.x + dx, position.y + dy);
    moveTo(newPos);
}

void Player::moveTo(const Position& pos) {
    pathHistory.push(position);  // Stack: Save current position before moving
    position = pos;
    std::cout << "[Player] Moved to (" << pos.x << ", " << pos.y << ")" << std::endl;
}

void Player::backtrack() {
    if (pathHistory.size() > 1) {
        pathHistory.pop();  // Remove current position
        position = pathHistory.top();  // Go back to previous
        std::cout << "[Player] Backtracked to (" << position.x << ", " << position.y << ")" << std::endl;
    } else {
        std::cout << "[Player] Cannot backtrack - at starting position" << std::endl;
    }
}

// CHANGE: 2025-11-14 - Unified to use ItemNew only
void Player::addItem(const ItemNew& item) {
    inventoryNew.append(item);  // LinkedList: Add to inventory
    std::cout << "[Player] Added item: " << item.name << " (rarity: " << item.getRarityName() << ", value: " << item.value << ")" << std::endl;
}

// DEPRECATED: Old addItemNew function - now uses unified addItem
void Player::addItemNew(const ItemNew& item) {
    addItem(item);  // Delegate to unified interface
}

// DEPRECATED: Old remove Item function - no longer used
bool Player::removeItem(const ItemNew& item) {
    // This was part of the old Item system, now handled by ItemNew system
    std::cout << "[Deprecated] removeItem() called - old Item system" << std::endl;
    return false;
}

int Player::attackEnemy() {
    // Base damage is attack stat + random variance
    int damage = attack + (rand() % 5);
    std::cout << "[Player] " << name << " attacks for " << damage << " damage!" << std::endl;
    return damage;
}

// TASK E: HP clamping with debug logging + validation
void Player::takeDamage(int damage) {
    if (damage < 0) {
        std::cout << "[Player] ERROR: Negative damage amount: " << damage << std::endl;
        return;
    }
    
    int actualDamage = damage - (defense / 2);
    if (actualDamage < 1) actualDamage = 1;
    
    int oldHealth = health;
    health -= actualDamage;
    health = std::max(0, std::min(health, maxHealth));  // Clamp to [0, maxHP]
    
    int damageTaken = oldHealth - health;
    std::cout << "[DEBUG] Player HP " << health << "/" << maxHealth << " (took " << damageTaken << " damage)" << std::endl;
}

void Player::heal(int amount) {
    if (amount <= 0) {
        std::cout << "[Player] ERROR: Invalid heal amount: " << amount << std::endl;
        return;
    }
    
    int oldHealth = health;
    health += amount;
    health = std::max(0, std::min(health, maxHealth));  // Clamp to [0, maxHP]
    
    int actualHealed = health - oldHealth;
    std::cout << "[DEBUG] Player HP " << health << "/" << maxHealth << " (healed " << actualHealed << ")" << std::endl;
}

bool Player::usePotion() {
    // CHANGE: 2025-11-14 - Check for potion in ItemNew system
    bool hasPotion = false;
    inventoryNew.traverse([&](const ItemNew& item) {
        if (item.type == "consumable" && item.action.kind == "heal") {
            hasPotion = true;
        }
    });
    
    if (!hasPotion) {
        std::cout << "[Player] No potions in inventory!" << std::endl;
        return false;
    }
    
    // Check if already at max health
    if (health >= maxHealth) {
        std::cout << "[Player] Already at full health!" << std::endl;
        return false;
    }
    
    // Find and use first healing potion - we need a mutable version
    bool used = false;
    // Since traverse is const, we need to manually find and remove
    inventoryNew.traverse([&](const ItemNew& item) {
        if (!used && item.type == "consumable" && item.action.kind == "heal") {
            int healAmount = 50;  // Default heal amount
            if (item.action.params.contains("amount")) {
                healAmount = item.action.params["amount"].get<int>();
            }
            heal(healAmount);
            std::cout << "[Player] Used " << item.name << "! (+" << healAmount << " HP)" << std::endl;
            removeItemNew(item.id);
            used = true;
        }
    });
    
    return used;
}

// CHANGE: 2025-11-14 - Unified hasItem to work with ItemNew
bool Player::hasItem(const std::string& itemName) const {
    bool found = false;
    inventoryNew.traverse([&](const ItemNew& item) {
        if (item.name == itemName || item.id == itemName) {
            found = true;
        }
    });
    return found;
}

void Player::addExperience(int xp) {
    experience += xp;
    std::cout << "[Player] Gained " << xp << " XP. Total: " << experience << std::endl;
    
    // Level up system - check for multiple level ups
    int xpForNextLevel = level * 100;
    while (experience >= xpForNextLevel) {
        level++;
        experience -= xpForNextLevel;
        
        // Grant bonuses
        maxHealth += 20;
        health = maxHealth;  // Full heal on level up
        maxMana += 10;
        mana = maxMana;  // Full mana restore
        attack += 2;
        defense += 1;
        
        // Grant skill points (1 per level, 2 every 5 levels)
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
        
        // Calculate XP for next level
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
    // Update logic here
}

void Player::render(sf::RenderWindow& window, float tileSize) const {
    // Draw shadow first (below character)
    sf::CircleShape shadow(10.0f);
    shadow.setFillColor(sf::Color(0, 0, 0, 80));
    shadow.setScale(sf::Vector2f(2.0f, 0.5f));
    shadow.setPosition(sf::Vector2f(position.x * tileSize + 6.0f, position.y * tileSize + 28.0f));
    window.draw(shadow);
    
    // ═══════════════════════════════════════════════════════════════════════
    // 👤 PLAYER CHARACTER - Using DebtsInTheDepths Wizard sprite
    // All classes use the wizard sprite for now (individual PNGs)
    // ═══════════════════════════════════════════════════════════════════════
    
    std::string textureKey = "player_warrior";  // All use wizard for now
    if (characterClass == "Rogue") {
        textureKey = "player_rogue";
    } else if (characterClass == "Mage") {
        textureKey = "player_mage";
    }

    sf::Texture* playerTex = AssetManager::getInstance().getTexture(textureKey);
    if (playerTex) {
        sf::Sprite playerSprite(*playerTex);
        playerSprite.setOrigin(sf::Vector2f(playerTex->getSize().x / 2.0f, playerTex->getSize().y / 2.0f));
        playerSprite.setPosition(sf::Vector2f(position.x * tileSize + 16.0f, position.y * tileSize + 16.0f));
        playerSprite.setScale(sf::Vector2f(1.5f, 1.5f));  // Scale to fit tile
        window.draw(playerSprite);
    } else {
        // Fallback to circle if texture not loaded
        float radius = tileSize * 0.35f;
        sf::CircleShape playerCircle(radius);
        playerCircle.setPosition(sf::Vector2f(position.x * tileSize + tileSize * 0.15f, 
                                              position.y * tileSize + tileSize * 0.15f));
        playerCircle.setFillColor(sf::Color(0, 220, 255));  // Cyan hero
        playerCircle.setOutlineThickness(2.5f);
        playerCircle.setOutlineColor(sf::Color::White);
        window.draw(playerCircle);
    }
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
    mana = std::max(0, std::min(mana, maxMana));  // Clamp to [0, maxMana]
    std::cout << "[Player] Used " << amount << " mana. Remaining: " << mana << "/" << maxMana << std::endl;
    return true;
}

void Player::restoreMana(int amount) {
    if (amount <= 0) {
        std::cout << "[Player] ERROR: Invalid mana restore amount: " << amount << std::endl;
        return;
    }
    
    mana = std::min(mana + amount, maxMana);
    mana = std::max(0, std::min(mana, maxMana));  // Ensure never negative
    std::cout << "[Player] Restored " << amount << " mana. Current: " << mana << "/" << maxMana << std::endl;
}

void Player::addGold(int amount) {
    gold += amount;
    std::cout << "[Player] Gained " << amount << " gold. Total: " << gold << std::endl;
}

bool Player::spendGold(int amount) {
    if (gold >= amount) {
        gold -= amount;
        std::cout << "[Player] Spent " << amount << " gold. Remaining: " << gold << std::endl;
        return true;
    }
    std::cout << "[Player] Insufficient gold! Need " << amount << ", have " << gold << std::endl;
    return false;
}

bool Player::removeItemNew(const std::string& itemId) {
    bool found = false;
    ItemNew toRemove;
    
    inventoryNew.traverse([&](const ItemNew& item) {
        if (item.id == itemId) {
            toRemove = item;
            found = true;
        }
    });
    
    if (found) {
        inventoryNew.remove(toRemove);
        std::cout << "[Player] Removed " << toRemove.name << " from inventory" << std::endl;
        return true;
    }
    return false;
}

bool Player::useItem(const std::string& itemId) {
    // Find the item in inventory
    bool found = false;
    ItemNew itemToUse;
    
    inventoryNew.traverse([&](const ItemNew& item) {
        if (item.id == itemId) {
            itemToUse = item;
            found = true;
        }
    });
    
    if (!found) {
        std::cout << "[Player] Item " << itemId << " not found in inventory!" << std::endl;
        return false;
    }
    
    std::cout << "[DEBUG] Using item: " << itemToUse.name << " (type: " << itemToUse.type 
              << ", action: " << itemToUse.action.kind << ")" << std::endl;
    
    // Apply item action based on kind
    if (itemToUse.action.kind == "heal") {
        // Heal player
        int healAmount = itemToUse.action.params.value("amount", 0);
        if (health >= maxHealth) {
            std::cout << "[Player] Already at full health!" << std::endl;
            return false;
        }
        heal(healAmount);
        std::cout << "[Player] Used " << itemToUse.name << " - healed " << healAmount << " HP!" << std::endl;
        
        // Remove consumable item
        if (itemToUse.type == "consumable") {
            removeItemNew(itemId);
        }
        return true;
        
    } else if (itemToUse.action.kind == "equip") {
        // Equip weapon or armor
        int attackBonus = itemToUse.action.params.value("attack_bonus", 0);
        int defenseBonus = itemToUse.action.params.value("defense_bonus", 0);
        
        if (itemToUse.type == "weapon") {
            // Unequip old weapon if exists
            if (equippedWeapon != nullptr) {
                int oldBonus = equippedWeapon->action.params.value("attack_bonus", 0);
                attack -= oldBonus;
                std::cout << "[Player] Unequipped " << equippedWeapon->name << " (-" << oldBonus << " attack)" << std::endl;
                delete equippedWeapon;
            }
            
            // Equip new weapon
            equippedWeapon = new ItemNew(itemToUse);
            attack += attackBonus;
            std::cout << "[Player] Equipped " << itemToUse.name << " (+" << attackBonus << " attack). Total attack: " << attack << std::endl;
            
        } else if (itemToUse.type == "armor") {
            // Unequip old armor if exists
            if (equippedArmor != nullptr) {
                int oldBonus = equippedArmor->action.params.value("defense_bonus", 0);
                defense -= oldBonus;
                std::cout << "[Player] Unequipped " << equippedArmor->name << " (-" << oldBonus << " defense)" << std::endl;
                delete equippedArmor;
            }
            
            // Equip new armor
            equippedArmor = new ItemNew(itemToUse);
            defense += defenseBonus;
            std::cout << "[Player] Equipped " << itemToUse.name << " (+" << defenseBonus << " defense). Total defense: " << defense << std::endl;
        }
        
        // Remove from inventory after equipping
        removeItemNew(itemId);
        return true;
        
    } else if (itemToUse.action.kind == "buff") {
        // Apply stat buff
        int healthBonus = itemToUse.action.params.value("health_bonus", 0);
        int manaBonus = itemToUse.action.params.value("mana_bonus", 0);
        
        if (healthBonus > 0) {
            maxHealth += healthBonus;
            health += healthBonus;
            std::cout << "[Player] Max health increased by " << healthBonus << "! New max: " << maxHealth << std::endl;
        }
        if (manaBonus > 0) {
            maxMana += manaBonus;
            mana += manaBonus;
            std::cout << "[Player] Max mana increased by " << manaBonus << "! New max: " << maxMana << std::endl;
        }
        
        // Remove consumable buff items
        if (itemToUse.type == "consumable") {
            removeItemNew(itemId);
        }
        return true;
        
    } else if (itemToUse.action.kind == "use") {
        // Special use effects (smoke bomb, teleport, etc.)
        std::string effect = itemToUse.action.params.value("effect", "");
        std::cout << "[Player] Used " << itemToUse.name << " - effect: " << effect << std::endl;
        
        // Remove consumable use items
        if (itemToUse.type == "consumable" || itemToUse.type == "utility") {
            removeItemNew(itemId);
        }
        return true;
        
    } else if (itemToUse.action.kind == "attack") {
        // Attack item (bomb, throwable)
        std::cout << "[Player] Used attack item " << itemToUse.name << std::endl;
        
        // Remove after use
        removeItemNew(itemId);
        return true;
        
    } else {
        std::cout << "[Player] Unknown item action: " << itemToUse.action.kind << std::endl;
        return false;
    }
}
