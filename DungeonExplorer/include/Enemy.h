// CHANGE: 2025-11-10 — Added adaptive AI system for 10-floor dungeon
// AI levels scale with floor depth for progressive challenge
// CHANGE: 2025-11-10 — Added drop table support for item drops on death

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "DataStructures/Queue.h"

struct EnemyData {
    int id;
    std::string name;
    std::string type;  // "melee", "ranged", "boss"
    int health;
    int maxHealth;
    int damage;
    int x, y;
    int attackRange;  // 1 for melee, 3+ for ranged
    float moveSpeed;  // For different speeds
    int aiLevel;  // 0=Random, 1=Chase, 2=Dijkstra, 3=Flank, 4=Boss
    int floorLevel;  // Which floor this enemy spawned on
    nlohmann::json dropTableJson;  // Drop table for loot
    
    EnemyData(int id = 0, const std::string& name = "Enemy", const std::string& type = "melee", 
              int hp = 50, int dmg = 10, int x = 0, int y = 0, int range = 1, float speed = 1.0f)
        : id(id), name(name), type(type), health(hp), maxHealth(hp), damage(dmg), 
          x(x), y(y), attackRange(range), moveSpeed(speed), aiLevel(0), floorLevel(1) {}
};

class EnemyManager {
private:
    Queue<EnemyData*> turnQueue;  // Queue for turn-based combat
    std::vector<EnemyData> enemies;
    
    sf::Texture enemyTexture;
    int nextEnemyId;

public:
    EnemyManager();
    ~EnemyManager();
    
    void spawnEnemy(const std::string& name, const std::string& type, int x, int y, 
                    int health = 50, int damage = 10, int range = 1, float speed = 1.0f, int floor = 1);
    void spawnEnemyWithDrops(const std::string& name, const std::string& type, int x, int y, 
                             int health, int damage, int range, float speed, int floor, 
                             const nlohmann::json& dropTable);  // NEW: Spawn with drop table
    void removeEnemy(int id);
    void removeDeadEnemies();  // Remove all enemies with health <= 0
    
    void initializeTurnQueue();
    EnemyData* getNextEnemy();
    void processNextTurn();
    
    // Adaptive AI system
    int calculateAILevel(int floor) const;
    void setEnemyAILevel(EnemyData& enemy, int floor);
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window, float tileSize) const;
    
    const std::vector<EnemyData>& getEnemies() const { return enemies; }
    EnemyData* findNearestEnemy(int playerX, int playerY);
    EnemyData* getEnemyById(int id);
    bool isEmpty() const { return enemies.empty(); }
};
