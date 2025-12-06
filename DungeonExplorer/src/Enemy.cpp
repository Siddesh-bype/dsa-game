// Enemy AI system with adaptive difficulty based on floor level
// AI scales from floors 1-10: Random → Chase → Dijkstra → Flank → Boss

#include "Enemy.h"
#include "AssetManager.h"
#include "TileManager.h"
#include "GameUtils.h"
#include "DataStructures/AStar.h"
#include "DataStructures/SpatialHash.h"
#include <unordered_set>

// ═══════════════════════════════════════════════════════════════════════════
// ENEMY CONFIGURATION CONSTANTS
// ═══════════════════════════════════════════════════════════════════════════

namespace {
    // AI behavior
    constexpr int DEFAULT_DETECTION_RANGE = 5;
    constexpr float MOVE_INTERVAL_BASE = 0.5f;
    constexpr float MOVE_INTERVAL_VARIANCE = 0.5f;
    constexpr float SPAWN_FADE_SPEED = 2.0f;
    
    // Animation
    constexpr float IDLE_BOB_FREQUENCY = 3.0f;
    constexpr float IDLE_BOB_AMPLITUDE = 2.0f;
    constexpr float IDLE_BOB_PHASE_OFFSET = 0.5f;
    
    // Spawn animation
    constexpr std::uint8_t SPAWN_MIN_ALPHA = 128;
    constexpr std::uint8_t SPAWN_MAX_ALPHA = 255;
    constexpr float SPAWN_SCALE_REDUCTION = 0.3f;
    
    // Alert indicator
    constexpr float ALERT_MARKER_RADIUS = 4.0f;
    constexpr float ALERT_MARKER_Y_OFFSET = -10.0f;
    
    // AI level names for clamping
    constexpr int AI_LEVEL_COUNT = 5;
}

// ═══════════════════════════════════════════════════════════════════════════
// TEXTURE MAPPING
// ═══════════════════════════════════════════════════════════════════════════

namespace {
    // Texture lookup pairs: {keyword, texture_key}
    static const std::pair<const char*, const char*> TEXTURE_MAPPINGS[] = {
        // Floor 1-2: Basic enemies
        {"Slime", "slime"}, {"Goblin", "goblin"},
        // Floor 2-3: Undead and orcs  
        {"Orc", "orc"}, {"Skeleton", "skeleton"},
        // Floor 3-4: Cave and shadow
        {"Bat", "goblin"}, {"Shadow", "wraith"}, {"Wraith", "wraith"},
        {"Cultist", "dark_mage"},
        // Floor 5-6: Bosses and mines
        {"Abyss", "dragon"}, {"Golem", "gargoyle"}, {"Miner", "wraith"},
        // Floor 7: Fortress
        {"Armored", "orc"}, {"Specter", "wraith"}, {"Hound", "goblin"},
        // Floor 8: Lava
        {"Fire", "demon"}, {"Flame", "demon"}, {"Lava", "demon"}, {"Elemental", "demon"},
        // Floor 9: Obsidian
        {"Mage", "dark_mage"}, {"Warlord", "orc"}, {"Death Knight", "skeleton"},
        // Floor 10: Final boss
        {"Eternal", "dragon"}, {"Shade", "dragon"},
        // Legacy
        {"Vampire", "vampire"}, {"Batilisk", "vampire"}, {"Lich", "lich"},
        {"Necromancer", "lich"}, {"Dragon", "dragon"}, {"Gargoyle", "gargoyle"},
        {"Minotaur", "minotaur"}, {"Knight", "skeleton"}
    };
    
    std::string resolveTextureKey(const std::string& name) {
        for (const auto& [keyword, texture] : TEXTURE_MAPPINGS) {
            if (name.find(keyword) != std::string::npos) {
                return texture;
            }
        }
        return "goblin";  // Default fallback
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTOR & DESTRUCTOR
// ═══════════════════════════════════════════════════════════════════════════

EnemyManager::EnemyManager() : nextEnemyId(0) {
}

EnemyManager::~EnemyManager() {
}

// ═══════════════════════════════════════════════════════════════════════════
// AI LEVEL SYSTEM
// ═══════════════════════════════════════════════════════════════════════════

int EnemyManager::calculateAILevel(int floor) const {
    if (floor <= AI_RANDOM_MAX_FLOOR) return 0;      // Floors 1-2: Random walk
    if (floor <= AI_CHASE_MAX_FLOOR) return 1;       // Floors 3-4: BFS Chase
    if (floor <= AI_DIJKSTRA_MAX_FLOOR) return 2;    // Floors 5-6: Dijkstra path
    if (floor <= AI_FLANK_MAX_FLOOR) return 3;       // Floors 7-8: Cooperative flank
    return AI_LEVEL_BOSS;                            // Floors 9-10: Boss AI
}

void EnemyManager::setEnemyAILevel(EnemyData& enemy, int floor) {
    enemy.floorLevel = floor;
    
    // Bosses always get max AI
    if (enemy.type == "boss") {
        enemy.aiLevel = AI_LEVEL_BOSS;
    } else {
        enemy.aiLevel = calculateAILevel(floor);
    }
    
    // Clamp AI level to valid range
    if (enemy.aiLevel < 0 || enemy.aiLevel >= AI_LEVEL_COUNT) {
        enemy.aiLevel = 0;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// ENEMY SPAWNING
// ═══════════════════════════════════════════════════════════════════════════

void EnemyManager::spawnEnemy(const std::string& name, const std::string& type, int x, int y, 
                              int health, int damage, int range, float speed, int floor) {
    enemies.emplace_back(nextEnemyId++, name, type, health, damage, x, y, range, speed);
    
    EnemyData& enemy = enemies.back();
    setEnemyAILevel(enemy, floor);
    
    // Initialize visual and patrol positions
    enemy.visualX = static_cast<float>(x);
    enemy.visualY = static_cast<float>(y);
    enemy.patrolStartX = x;
    enemy.patrolStartY = y;
    
    // Resolve texture key
    if (textureMap.find(name) == textureMap.end()) {
        textureMap[name] = resolveTextureKey(name);
    }
    enemy.textureKey = textureMap[name];
}

void EnemyManager::spawnEnemyWithDrops(const std::string& name, const std::string& type, int x, int y, 
                                       int health, int damage, int range, float speed, int floor, 
                                       const nlohmann::json& dropTable) {
    enemies.emplace_back(nextEnemyId++, name, type, health, damage, x, y, range, speed);
    
    EnemyData& enemy = enemies.back();
    setEnemyAILevel(enemy, floor);
    enemy.dropTableJson = dropTable;
    
    // Initialize visual and patrol positions
    enemy.visualX = static_cast<float>(x);
    enemy.visualY = static_cast<float>(y);
    enemy.patrolStartX = x;
    enemy.patrolStartY = y;
    
    // Resolve texture key
    if (textureMap.find(name) == textureMap.end()) {
        textureMap[name] = resolveTextureKey(name);
    }
    enemy.textureKey = textureMap[name];
}

// ═══════════════════════════════════════════════════════════════════════════
// ENEMY REMOVAL
// ═══════════════════════════════════════════════════════════════════════════

void EnemyManager::removeEnemy(int id) {
    for (auto it = enemies.begin(); it != enemies.end(); ++it) {
        if (it->id == id) {
            enemies.erase(it);
            return;
        }
    }
}

void EnemyManager::removeDeadEnemies() {
    auto it = enemies.begin();
    while (it != enemies.end()) {
        if (it->health <= 0) {
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// TURN QUEUE SYSTEM (DSA: Queue for round-robin processing)
// ═══════════════════════════════════════════════════════════════════════════

void EnemyManager::initializeTurnQueue() {
    turnQueue.clear();
    
    for (auto& enemy : enemies) {
        turnQueue.enqueue(&enemy);
    }
}

EnemyData* EnemyManager::getNextEnemy() {
    if (turnQueue.isEmpty()) {
        initializeTurnQueue();
    }
    
    if (!turnQueue.isEmpty()) {
        return turnQueue.front();
    }
    
    return nullptr;
}

void EnemyManager::processNextTurn() {
    if (turnQueue.isEmpty()) {
        initializeTurnQueue();
    }
    
    if (!turnQueue.isEmpty()) {
        EnemyData* enemy = turnQueue.front();
        turnQueue.dequeue();
        turnQueue.enqueue(enemy);  // Re-enqueue for next round
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// UPDATE LOOP
// ═══════════════════════════════════════════════════════════════════════════

void EnemyManager::update(float deltaTime) {
    static float globalBobTime = 0.f;
    globalBobTime += deltaTime;
    
    for (auto& enemy : enemies) {
        // Spawn fade-in animation
        if (enemy.isSpawning) {
            enemy.spawnTimer -= deltaTime * SPAWN_FADE_SPEED;
            if (enemy.spawnTimer <= 0.f) {
                enemy.spawnTimer = 0.f;
                enemy.isSpawning = false;
            }
        }
        
        // Idle bobbing animation
        enemy.bobOffset = std::sin(globalBobTime * IDLE_BOB_FREQUENCY + 
                                   static_cast<float>(enemy.id) * IDLE_BOB_PHASE_OFFSET) * IDLE_BOB_AMPLITUDE;
        
        // Movement timer - enemies move at intervals
        enemy.moveTimer -= deltaTime;
        
        if (enemy.moveTimer <= 0.f) {
            enemy.moveTimer = MOVE_INTERVAL_BASE + 
                              static_cast<float>(std::rand() % 50) / 100.f * MOVE_INTERVAL_VARIANCE;
            
            // Player detection
            const int detectionRange = DEFAULT_DETECTION_RANGE;
            const int attackRange = enemy.range;
            
            const int dx = enemy.targetX - enemy.x;
            const int dy = enemy.targetY - enemy.y;
            const float distToPlayer = std::sqrt(static_cast<float>(dx * dx + dy * dy));
            
            if (distToPlayer <= static_cast<float>(attackRange)) {
                // In attack range
                enemy.isAlerted = true;
                enemy.alertTimer = EnemyData::ALERT_DURATION;
                enemy.isPatrolling = false;
            }
            else if (distToPlayer <= static_cast<float>(detectionRange)) {
                // Chase mode
                enemy.isAlerted = true;
                enemy.alertTimer = EnemyData::ALERT_DURATION;
                enemy.isPatrolling = false;
                
                const int moveX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
                const int moveY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
                
                enemy.x += moveX;
                enemy.y += moveY;
            }
            else if (enemy.isPatrolling) {
                // Patrol AI
                const int pdx = enemy.patrolTargetX - enemy.x;
                const int pdy = enemy.patrolTargetY - enemy.y;
                
                if (pdx != 0 || pdy != 0) {
                    const int moveX = (pdx > 0) ? 1 : (pdx < 0) ? -1 : 0;
                    const int moveY = (pdy > 0) ? 1 : (pdy < 0) ? -1 : 0;
                    enemy.x += moveX;
                    enemy.y += moveY;
                } else {
                    enemy.patrolTimer = 0.f;
                }
            }
            
            // Pick new patrol target periodically
            if (enemy.isPatrolling && !enemy.isAlerted) {
                enemy.patrolTimer -= MOVE_INTERVAL_BASE;
                if (enemy.patrolTimer <= 0.f) {
                    enemy.patrolTimer = EnemyData::PATROL_INTERVAL;
                    const int rx = (std::rand() % (EnemyData::PATROL_RADIUS * 2 + 1)) - EnemyData::PATROL_RADIUS;
                    const int ry = (std::rand() % (EnemyData::PATROL_RADIUS * 2 + 1)) - EnemyData::PATROL_RADIUS;
                    enemy.patrolTargetX = enemy.patrolStartX + rx;
                    enemy.patrolTargetY = enemy.patrolStartY + ry;
                }
            }
        }
        
        // Smooth visual position lerp
        const float targetX = static_cast<float>(enemy.x);
        const float targetY = static_cast<float>(enemy.y);
        const float lerpSpeed = EnemyData::MOVE_LERP_SPEED * deltaTime;
        enemy.visualX += (targetX - enemy.visualX) * lerpSpeed;
        enemy.visualY += (targetY - enemy.visualY) * lerpSpeed;
        
        // Alert timer countdown
        if (enemy.isAlerted) {
            enemy.alertTimer -= deltaTime;
            if (enemy.alertTimer <= 0.f) {
                enemy.isAlerted = false;
                enemy.isPatrolling = true;
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// RENDERING
// ═══════════════════════════════════════════════════════════════════════════

void EnemyManager::render(sf::RenderWindow& window, float tileSize) const {
    for (const auto& enemy : enemies) {
        // Spawn animation alpha
        std::uint8_t spawnAlpha = SPAWN_MAX_ALPHA;
        if (enemy.isSpawning && enemy.spawnTimer > 0.f) {
            spawnAlpha = static_cast<std::uint8_t>(
                SPAWN_MIN_ALPHA + (SPAWN_MAX_ALPHA - SPAWN_MIN_ALPHA) * (1.f - enemy.spawnTimer)
            );
        }
        
        const float bobY = enemy.bobOffset;
        const float renderX = enemy.visualX * tileSize;
        const float renderY = enemy.visualY * tileSize;
        
        // Shadow
        sf::CircleShape shadow(SHADOW_RADIUS);
        shadow.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(SHADOW_ALPHA * spawnAlpha / 255)));
        shadow.setScale(sf::Vector2f(SHADOW_SCALE_X, SHADOW_SCALE_Y));
        shadow.setPosition(sf::Vector2f(renderX + SHADOW_OFFSET_X, renderY + SHADOW_OFFSET_Y));
        window.draw(shadow);
        
        // Alert indicator
        if (enemy.isAlerted) {
            sf::CircleShape alertMark(ALERT_MARKER_RADIUS);
            alertMark.setFillColor(sf::Color(255, 50, 50));
            alertMark.setPosition(sf::Vector2f(
                renderX + tileSize / 2.f - ALERT_MARKER_RADIUS, 
                renderY + ALERT_MARKER_Y_OFFSET
            ));
            window.draw(alertMark);
        }
        
        // Enemy sprite
        const std::string textureKey = enemy.textureKey.empty() ? "goblin" : enemy.textureKey;
        sf::Texture* enemyTexture = AssetManager::getInstance().getTexture(textureKey);
        
        if (enemyTexture) {
            sf::Sprite enemySprite(*enemyTexture);
            
            const sf::FloatRect bounds = enemySprite.getLocalBounds();
            enemySprite.setOrigin(sf::Vector2f(bounds.size.x / 2.0f, bounds.size.y / 2.0f));
            enemySprite.setPosition(sf::Vector2f(
                renderX + tileSize / 2.0f, 
                renderY + tileSize / 2.0f + bobY
            ));
            
            // Scale based on enemy type
            const float scaleMult = (enemy.type == "boss") ? BOSS_SCALE_MULT : NORMAL_SCALE_MULT;
            const float spawnScale = enemy.isSpawning ? (1.f - enemy.spawnTimer * SPAWN_SCALE_REDUCTION) : 1.f;
            const float scaleX = (tileSize * scaleMult * spawnScale) / bounds.size.x;
            const float scaleY = (tileSize * scaleMult * spawnScale) / bounds.size.y;
            enemySprite.setScale(sf::Vector2f(scaleX, scaleY));
            
            enemySprite.setColor(sf::Color(255, 255, 255, spawnAlpha));
            window.draw(enemySprite);
        } else {
            // Fallback circle rendering
            const float radius = (enemy.type == "boss") 
                ? tileSize * BOSS_RADIUS_MULT 
                : tileSize * FALLBACK_RADIUS_MULT;
            
            sf::CircleShape enemyCircle(radius);
            enemyCircle.setPosition(sf::Vector2f(
                static_cast<float>(enemy.x) * tileSize + tileSize * 0.15f, 
                static_cast<float>(enemy.y) * tileSize + tileSize * 0.15f
            ));
            
            // Color based on enemy type
            if (enemy.type == "melee") {
                enemyCircle.setFillColor(sf::Color(200, 50, 50));
            } else if (enemy.type == "ranged") {
                enemyCircle.setFillColor(sf::Color(100, 100, 200));
            } else if (enemy.type == "boss") {
                enemyCircle.setFillColor(sf::Color(120, 0, 120));
            }
            
            enemyCircle.setOutlineThickness(2.0f);
            enemyCircle.setOutlineColor(sf::Color::Black);
            window.draw(enemyCircle);
        }
        
        // Health bar
        const float healthBarX = static_cast<float>(enemy.x) * tileSize + tileSize * HEALTH_BAR_X_OFFSET;
        const float healthBarY = static_cast<float>(enemy.y) * tileSize + tileSize * HEALTH_BAR_Y_OFFSET;
        
        sf::RectangleShape healthBg(sf::Vector2f(tileSize * HEALTH_BAR_WIDTH_MULT, HEALTH_BAR_HEIGHT));
        healthBg.setPosition(sf::Vector2f(healthBarX, healthBarY));
        healthBg.setFillColor(sf::Color(50, 50, 50));
        window.draw(healthBg);
        
        const float healthPercent = static_cast<float>(enemy.health) / static_cast<float>(enemy.maxHealth);
        sf::RectangleShape healthBar(sf::Vector2f(tileSize * HEALTH_BAR_WIDTH_MULT * healthPercent, HEALTH_BAR_HEIGHT));
        healthBar.setPosition(sf::Vector2f(healthBarX, healthBarY));
        
        // Health bar color based on health level
        if (healthPercent > HEALTH_GOOD_THRESHOLD) {
            healthBar.setFillColor(sf::Color(50, 200, 50));
        } else if (healthPercent > HEALTH_WARN_THRESHOLD) {
            healthBar.setFillColor(sf::Color(255, 200, 0));
        } else {
            healthBar.setFillColor(sf::Color(255, 50, 50));
        }
        window.draw(healthBar);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// ENEMY LOOKUP
// ═══════════════════════════════════════════════════════════════════════════

EnemyData* EnemyManager::findNearestEnemy(int playerX, int playerY) {
    if (enemies.empty()) {
        return nullptr;
    }
    
    EnemyData* nearest = nullptr;
    int minDistance = INT_MAX;
    
    for (auto& enemy : enemies) {
        const int dx = enemy.x - playerX;
        const int dy = enemy.y - playerY;
        const int distance = dx * dx + dy * dy;  // Squared distance
        
        if (distance < minDistance) {
            minDistance = distance;
            nearest = &enemy;
        }
    }
    
    return nearest;
}

EnemyData* EnemyManager::getEnemyById(int id) {
    for (auto& enemy : enemies) {
        if (enemy.id == id) {
            return &enemy;
        }
    }
    return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// SPATIAL INDEX (DSA: SpatialHash for O(k) enemy lookup)
// ═══════════════════════════════════════════════════════════════════════════

void EnemyManager::buildSpatialIndex() {
    enemyGrid.clear();
    
    for (auto& enemy : enemies) {
        if (enemy.health > 0) {
            enemyGrid.insert(enemy.x * SPATIAL_CELL_SIZE, enemy.y * SPATIAL_CELL_SIZE, enemy.id);
        }
    }
}

std::vector<EnemyData*> EnemyManager::findNearbyEnemies(int x, int y, int radius) {
    std::vector<EnemyData*> result;
    
    auto nearbyIds = enemyGrid.queryNearby(
        x * SPATIAL_CELL_SIZE, 
        y * SPATIAL_CELL_SIZE, 
        radius * SPATIAL_CELL_SIZE
    );
    
    for (const int id : nearbyIds) {
        EnemyData* enemy = getEnemyById(id);
        if (enemy && enemy->health > 0) {
            const int dx = enemy->x - x;
            const int dy = enemy->y - y;
            const float dist = std::sqrt(static_cast<float>(dx * dx + dy * dy));
            
            if (dist <= static_cast<float>(radius)) {
                result.push_back(enemy);
            }
        }
    }
    
    return result;
}

