#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include "DataStructures/Stack.h"
#include "DataStructures/LinkedList.h"
#include "ItemNew.h"

struct Position {
    int x, y;
    
    Position() : x(0), y(0) {}
    Position(int x, int y) : x(x), y(y) {}
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

// Old Item struct - kept for backwards compatibility
struct Item {
    std::string id;
    std::string name;
    std::string type;
    int damage;
    int value;
    int healing;
    
    Item(const std::string& itemId = "", const std::string& n = "", const std::string& t = "", 
         int d = 0, int v = 0, int h = 0)
        : id(itemId), name(n), type(t), damage(d), value(v), healing(h) {}
    
    bool operator==(const Item& other) const {
        return id == other.id || name == other.name;
    }
    
    bool operator!=(const Item& other) const {
        return !(*this == other);
    }
};

class Player {
private:
    Position position;
    int health;
    int maxHealth;
    int mana;
    int maxMana;
    int experience;
    int level;
    int attack;
    int defense;
    int gold;  // Player's currency
    int skillPointsToGrant;  // Skill points earned from leveling
    std::string name;
    std::string characterClass;  // Warrior, Rogue, Mage
    
    // Note: sf::Sprite removed - SFML 3.x requires texture, using shapes instead
    
    Stack<Position> pathHistory;  // For backtracking
    // DEPRECATION: 2025-11-14 - Old Item system replaced by ItemNew
    // LinkedList<Item> inventory;   // DEPRECATED - Use inventoryNew instead
    LinkedList<ItemNew> inventoryNew;  // Primary item system with actions and effects
    
    // CHANGE: 2025-11-14 - Use smart pointers for automatic memory management
    // Replaces raw pointers: ItemNew* equippedWeapon; ItemNew* equippedArmor;
    std::unique_ptr<ItemNew> equippedWeapon;
    std::unique_ptr<ItemNew> equippedArmor;
    
    float moveSpeed;
    sf::Vector2f velocity;
    
    // 🎮 Dash mechanic
    float dashCooldown = 0.f;
    bool isDashing = false;
    float dashTimer = 0.f;
    int dashDirX = 0, dashDirY = 0;
    static constexpr float DASH_COOLDOWN_TIME = 2.0f;
    static constexpr float DASH_DURATION = 0.2f;
    static constexpr int DASH_DISTANCE = 3;
    
    // 🎭 Animation system
    enum class AnimState { Idle, Walking, Attacking, Hurt, Casting };
    AnimState currentAnim = AnimState::Idle;
    float animTimer = 0.f;
    float visualX = 0.f, visualY = 0.f;  // Smooth visual position
    int facingDir = 1;  // 1 = right, -1 = left
    float attackAnimTimer = 0.f;
    float hurtAnimTimer = 0.f;
    static constexpr float ANIM_WALK_SPEED = 0.15f;
    static constexpr float ANIM_ATTACK_DURATION = 0.3f;
    static constexpr float ANIM_HURT_DURATION = 0.2f;
    static constexpr float VISUAL_LERP_SPEED = 8.0f;
    
    // ═══════════════════════════════════════════════════════════════════════
    // PLAYER CONFIGURATION CONSTANTS
    // ═══════════════════════════════════════════════════════════════════════
    
    // Default stats
    static constexpr int DEFAULT_HEALTH = 100;
    static constexpr int DEFAULT_MANA = 50;
    static constexpr int DEFAULT_ATTACK = 10;
    static constexpr int DEFAULT_DEFENSE = 5;
    static constexpr float DEFAULT_MOVE_SPEED = 100.0f;
    
    // Warrior class
    static constexpr int WARRIOR_HEALTH = 120;
    static constexpr int WARRIOR_MANA = 40;
    static constexpr int WARRIOR_ATTACK = 18;
    static constexpr int WARRIOR_DEFENSE = 15;
    
    // Rogue class
    static constexpr int ROGUE_HEALTH = 90;
    static constexpr int ROGUE_MANA = 50;
    static constexpr int ROGUE_ATTACK = 20;
    static constexpr int ROGUE_DEFENSE = 8;
    static constexpr float ROGUE_MOVE_SPEED = 120.0f;
    
    // Mage class
    static constexpr int MAGE_HEALTH = 80;
    static constexpr int MAGE_MANA = 100;
    static constexpr int MAGE_ATTACK = 25;
    static constexpr int MAGE_DEFENSE = 5;
    
    // Leveling
    static constexpr int XP_PER_LEVEL_MULT = 100;
    static constexpr int HEALTH_PER_LEVEL = 20;
    static constexpr int MANA_PER_LEVEL = 10;
    static constexpr int ATTACK_PER_LEVEL = 2;
    static constexpr int DEFENSE_PER_LEVEL = 1;
    static constexpr int SKILL_POINTS_NORMAL = 1;
    static constexpr int SKILL_POINTS_BONUS = 2;
    static constexpr int BONUS_LEVEL_INTERVAL = 5;
    
    // Combat
    static constexpr int DEFENSE_DAMAGE_REDUCTION_DIV = 2;
    static constexpr int MIN_DAMAGE = 1;
    
    // Path history
    static constexpr int MAX_PATH_HISTORY = 50;
    
    // Rendering
    static constexpr float SHADOW_RADIUS = 10.0f;
    static constexpr float SHADOW_SCALE_X = 2.0f;
    static constexpr float SHADOW_SCALE_Y = 0.5f;
    static constexpr float SHADOW_OFFSET_X = 6.0f;
    static constexpr float SHADOW_OFFSET_Y = 28.0f;
    static constexpr int SHADOW_ALPHA = 80;
    static constexpr float PLAYER_SCALE_MULT = 1.2f;
    static constexpr float FALLBACK_RADIUS_MULT = 0.35f;
    static constexpr float FALLBACK_OFFSET_MULT = 0.15f;
    static constexpr float FALLBACK_OUTLINE = 2.5f;

public:
    Player();
    ~Player();  // Destructor to clean up allocated memory
    
    void initialize(int startX, int startY);
    void setCharacter(const std::string& playerName, const std::string& charClass);
    
    void move(int dx, int dy);
    void moveTo(const Position& pos);
    void backtrack();
    
    int attackEnemy();  // Returns damage dealt
    void takeDamage(int damage);
    
    // DEPRECATED: 2025-11-14 - Old Item system replaced
    // void addItem(const Item& item);
    // bool removeItem(const Item& item);
    // bool hasItem(const std::string& itemName) const;
    // const LinkedList<Item>& getInventory() const;
    
    // Primary inventory system (ItemNew)
    void addItem(const ItemNew& item);  // Unified interface
    bool removeItemNew(const std::string& itemId);
    bool useItem(const std::string& itemId);  // Use item by ID, returns true if successful
    bool useHotbarItem(int slot);  // Use item in inventory by slot index (1-9)
    bool hasItem(const std::string& itemName) const;  // Check if player has item (now uses ItemNew)
    const LinkedList<ItemNew>& getInventoryNew() const { return inventoryNew; }
    LinkedList<ItemNew>& getInventoryNew() { return inventoryNew; }  // Non-const version for modification
    
    // Equipment system
    const ItemNew* getEquippedWeapon() const { return equippedWeapon.get(); }
    const ItemNew* getEquippedArmor() const { return equippedArmor.get(); }
    bool unequipWeapon();
    bool unequipArmor();
    
    // Inventory management
    void swapInventoryItems(int index1, int index2);
    
    void heal(int amount);
    bool usePotion();  // Returns true if potion was used successfully
    void addExperience(int xp);
    int getSkillPointsEarned() const;  // Get skill points from leveling
    void clearSkillPoints();  // Clear after granting to skill tree
    
    bool useMana(int amount);  // Returns true if enough mana
    void restoreMana(int amount);
    
    // Gold management
    void addGold(int amount);
    bool spendGold(int amount);  // Returns true if player had enough gold
    int getGold() const { return gold; }
    
    Position getPosition() const { return position; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getMana() const { return mana; }
    int getMaxMana() const { return maxMana; }
    int getExperience() const { return experience; }
    int getLevel() const { return level; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    std::string getName() const { return name; }
    std::string getClass() const { return characterClass; }
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window, float tileSize) const;
    
    // 🎮 Dash mechanic
    bool canDash() const { return dashCooldown <= 0.f && !isDashing; }
    void performDash(int dirX, int dirY);
    bool getIsDashing() const { return isDashing; }
    
    const Stack<Position>& getPathHistory() const { return pathHistory; }
};
