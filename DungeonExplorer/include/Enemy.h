// CHANGE: 2025-11-10 — Added adaptive AI system for 10-floor dungeon
// AI levels scale with floor depth for progressive challenge
// CHANGE: 2025-11-10 — Added drop table support for item drops on death
// CHANGE: 2025-12-04 — Added SpatialHash for O(k) enemy lookup

#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "DataStructures/Queue.h"
#include "DataStructures/SpatialHash.h"

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
    std::string textureKey;  // CHANGE: 2025-11-14 - Cache texture key to avoid string searches
    
    // 🎮 Animation state
    float spawnTimer = 1.0f;        // Fade-in timer (starts at 1, decreases to 0)
    float bobOffset = 0.f;          // Vertical bob animation
    bool isSpawning = true;         // True during spawn animation
    
    // 🚶 Patrol and movement system
    float visualX = 0.f, visualY = 0.f;  // Smooth visual position (lerped)
    int patrolStartX = 0, patrolStartY = 0;  // Original spawn point
    int patrolTargetX = 0, patrolTargetY = 0;  // Current patrol target
    int targetX = 0, targetY = 0;    // Current chase target (player position)
    int range = 1;                    // Attack range (alias for attackRange)
    float patrolTimer = 0.f;         // Time until next patrol move
    bool isPatrolling = true;        // Currently patrolling vs chasing
    bool isAlerted = false;          // Detected player
    float alertTimer = 0.f;          // Time remaining in alert state
    float moveTimer = 0.f;           // Smooth movement timer
    
    // Patrol constants
    static constexpr float PATROL_INTERVAL = 2.0f;  // Seconds between patrol moves
    static constexpr float ALERT_DURATION = 5.0f;   // Seconds to chase after losing sight
    static constexpr float MOVE_LERP_SPEED = 5.0f;  // Smooth movement speed
    static constexpr int PATROL_RADIUS = 3;          // Tiles from spawn to patrol within
    
    EnemyData(int id = 0, const std::string& name = "Enemy", const std::string& type = "melee", 
              int hp = 50, int dmg = 10, int x = 0, int y = 0, int range = 1, float speed = 1.0f)
        : id(id), name(name), type(type), health(hp), maxHealth(hp), damage(dmg), 
          x(x), y(y), attackRange(range), moveSpeed(speed), aiLevel(0), floorLevel(1), 
          textureKey("goblin"), visualX(static_cast<float>(x)), visualY(static_cast<float>(y)),
          patrolStartX(x), patrolStartY(y) {}  // Default texture
};

class EnemyManager {
private:
    Queue<EnemyData*> turnQueue;  // Queue for turn-based combat
    std::vector<EnemyData> enemies;
    
    // CHANGE: 2025-11-25 - Cache for enemy name to texture key mapping
    std::unordered_map<std::string, std::string> textureMap;
    
    // CHANGE: 2025-12-04 - SpatialHash for O(k) enemy lookup (DSA)
    SpatialHash<int> enemyGrid;  // Stores enemy IDs for spatial queries
    static constexpr int SPATIAL_CELL_SIZE = 32;  // Pixels per cell
    
    sf::Texture enemyTexture;
    int nextEnemyId;
    
    // ═══════════════════════════════════════════════════════════════════════
    // CONFIGURATION CONSTANTS
    // ═══════════════════════════════════════════════════════════════════════
    
    // AI Level thresholds
    static constexpr int AI_RANDOM_MAX_FLOOR = 2;
    static constexpr int AI_CHASE_MAX_FLOOR = 4;
    static constexpr int AI_DIJKSTRA_MAX_FLOOR = 6;
    static constexpr int AI_FLANK_MAX_FLOOR = 8;
    static constexpr int AI_LEVEL_BOSS = 4;
    
    // Rendering constants
    static constexpr float SHADOW_RADIUS = 10.0f;
    static constexpr float SHADOW_SCALE_X = 2.0f;
    static constexpr float SHADOW_SCALE_Y = 0.5f;
    static constexpr float SHADOW_OFFSET_X = 6.0f;
    static constexpr float SHADOW_OFFSET_Y = 28.0f;
    static constexpr int SHADOW_ALPHA = 80;
    
    static constexpr float BOSS_SCALE_MULT = 1.8f;
    static constexpr float NORMAL_SCALE_MULT = 1.3f;
    static constexpr float FALLBACK_RADIUS_MULT = 0.35f;
    static constexpr float BOSS_RADIUS_MULT = 0.45f;
    
    // Health bar constants
    static constexpr float HEALTH_BAR_WIDTH_MULT = 0.7f;
    static constexpr float HEALTH_BAR_HEIGHT = 5.0f;
    static constexpr float HEALTH_BAR_X_OFFSET = 0.15f;
    static constexpr float HEALTH_BAR_Y_OFFSET = 0.05f;
    
    static constexpr float HEALTH_GOOD_THRESHOLD = 0.6f;   // Green above 60%
    static constexpr float HEALTH_WARN_THRESHOLD = 0.3f;   // Yellow above 30%
    // Red below 30%

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
    
    // Accessors
    const std::vector<EnemyData>& getEnemies() const { return enemies; }
    std::vector<EnemyData>& getMutableEnemies() { return enemies; }  // OPTIMIZATION: For combat modifications
    EnemyData* findNearestEnemy(int playerX, int playerY);
    EnemyData* getEnemyById(int id);
    bool isEmpty() const { return enemies.empty(); }
    
    // ═══════════════════════════════════════════════════════════════════════
    // SPATIAL INDEX (DSA: SpatialHash for O(k) lookup)
    // ═══════════════════════════════════════════════════════════════════════
    void buildSpatialIndex();  // Rebuild index after enemies move
    std::vector<EnemyData*> findNearbyEnemies(int x, int y, int radius);
};
