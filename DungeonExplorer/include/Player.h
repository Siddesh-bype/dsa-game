#pragma once
#include <SFML/Graphics.hpp>
#include <string>
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
    
    // Equipment slots for equipped items
    ItemNew* equippedWeapon;
    ItemNew* equippedArmor;
    
    float moveSpeed;
    sf::Vector2f velocity;

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
    void addItemNew(const ItemNew& item);
    bool removeItem(const ItemNew& item);  // DEPRECATED: Old Item version - now handles ItemNew
    bool removeItemNew(const std::string& itemId);
    bool useItem(const std::string& itemId);  // Use item by ID, returns true if successful
    bool hasItem(const std::string& itemName) const;  // Check if player has item (now uses ItemNew)
    const LinkedList<ItemNew>& getInventoryNew() const { return inventoryNew; }
    
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
    
    const Stack<Position>& getPathHistory() const { return pathHistory; }
};
