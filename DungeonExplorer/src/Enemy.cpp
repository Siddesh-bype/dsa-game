// CHANGE: 2025-11-10 — Added adaptive AI system for progressive difficulty
// Enemy AI scales from floors 1-10 with increasing intelligence
// CHANGE: 2025-12-04 - Integrated A* pathfinding and GameUtils for code deduplication

#include "Enemy.h"
#include "AssetManager.h"
#include "TileManager.h"
#include "GameUtils.h"
#include "DataStructures/AStar.h"
#include "DataStructures/SpatialHash.h"
#include <iostream>
#include <unordered_set>

EnemyManager::EnemyManager() : nextEnemyId(0) {
}

EnemyManager::~EnemyManager() {
}

// Calculate AI level based on floor (0=Random, 1=Chase, 2=Dijkstra, 3=Flank, 4=Boss)
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
    
    // CHANGE: 2025-11-14 - Add bounds checking before array access
    static const char* aiNames[] = {"Random", "Chase", "Dijkstra", "Flank", "Boss"};
    static const int aiNamesSize = 5;
    
    if (enemy.aiLevel < 0 || enemy.aiLevel >= aiNamesSize) {
        std::cerr << "[ERROR] AI level " << enemy.aiLevel << " out of bounds [0-" << aiNamesSize-1 << "]" << std::endl;
        enemy.aiLevel = 0;  // Clamp to valid range
    }
    
    std::cout << "[EnemyAI] " << enemy.name << " assigned AI Level " << enemy.aiLevel 
              << " (" << aiNames[enemy.aiLevel] << ") for floor " << floor << std::endl;
}

// Helper to resolve texture key from name using lookup table
// Matches enemy names from DungeonLevelManager level definitions
static std::string resolveTextureKey(const std::string& name) {
    // Texture lookup pairs: {keyword, texture_key}
    static const std::pair<const char*, const char*> textureMappings[] = {
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
    
    for (const auto& [keyword, texture] : textureMappings) {
        if (name.find(keyword) != std::string::npos) {
            return texture;
        }
    }
    return "goblin"; // Default fallback
}

void EnemyManager::spawnEnemy(const std::string& name, const std::string& type, int x, int y, 
                              int health, int damage, int range, float speed, int floor) {
    // OPTIMIZATION: Use emplace_back for in-place construction
    enemies.emplace_back(nextEnemyId++, name, type, health, damage, x, y, range, speed);
    
    EnemyData& enemy = enemies.back();
    setEnemyAILevel(enemy, floor);
    
    // 🎮 Initialize visual and patrol positions
    enemy.visualX = static_cast<float>(x);
    enemy.visualY = static_cast<float>(y);
    enemy.patrolStartX = x;
    enemy.patrolStartY = y;
    
    // CHANGE: 2025-11-25 - Use cached texture key
    if (textureMap.find(name) == textureMap.end()) {
        textureMap[name] = (type == "boss") ? "dragon" : resolveTextureKey(name);
    }
    enemy.textureKey = textureMap[name];
}

// CHANGE: 2025-11-10 - Spawn enemy with drop table for loot system
void EnemyManager::spawnEnemyWithDrops(const std::string& name, const std::string& type, int x, int y, 
                                       int health, int damage, int range, float speed, int floor, 
                                       const nlohmann::json& dropTable) {
    // OPTIMIZATION: Use emplace_back for in-place construction
    enemies.emplace_back(nextEnemyId++, name, type, health, damage, x, y, range, speed);
    
    EnemyData& enemy = enemies.back();
    setEnemyAILevel(enemy, floor);
    enemy.dropTableJson = dropTable;  // Store drop table
    
    // 🎮 Initialize visual and patrol positions
    enemy.visualX = static_cast<float>(x);
    enemy.visualY = static_cast<float>(y);
    enemy.patrolStartX = x;
    enemy.patrolStartY = y;
    
    // CHANGE: 2025-11-25 - Use cached texture key
    if (textureMap.find(name) == textureMap.end()) {
        textureMap[name] = (type == "boss") ? "dragon" : resolveTextureKey(name);
    }
    enemy.textureKey = textureMap[name];
}

void EnemyManager::removeEnemy(int id) {
    for (auto it = enemies.begin(); it != enemies.end(); ++it) {
        if (it->id == id) {
            // CHANGE: 2025-11-14 - Remove debug spam
            // std::cout << "[EnemyManager] Removed enemy: " << it->name << std::endl;
            enemies.erase(it);
            return;
        }
    }
}

void EnemyManager::removeDeadEnemies() {
    auto it = enemies.begin();
    while (it != enemies.end()) {
        if (it->health <= 0) {
            // CHANGE: 2025-11-14 - Remove debug spam from hot path
            // std::cout << "[EnemyManager] Removed dead enemy: " << it->name << std::endl;
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }
}

void EnemyManager::initializeTurnQueue() {
    turnQueue.clear();
    
    // OPTIMIZATION: Pre-allocate queue capacity if possible
    // Note: Queue uses deque internally, no reserve() needed
    
    // CHANGE: 2025-11-14 - Remove debug spam
    // std::cout << "[EnemyManager] Initializing turn queue with " << enemies.size() << " enemies" << std::endl;
    
    // OPTIMIZATION: Use range-based for loop for cleaner code
    for (auto& enemy : enemies) {
        turnQueue.enqueue(&enemy);
    }
    
    // OPTIMIZATION: Use back() to verify last enemy added
    if (!turnQueue.isEmpty()) {
        // Verify queue is properly initialized
        std::cout << "[EnemyManager] Turn queue initialized with " << turnQueue.size() << " enemies" << std::endl;
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
        // CHANGE: 2025-11-14 - Remove debug spam
        // std::cout << "[EnemyManager] Turn queue empty, reinitializing..." << std::endl;
        initializeTurnQueue();
    }
    
    if (!turnQueue.isEmpty()) {
        EnemyData* enemy = turnQueue.front();
        turnQueue.dequeue();
        
        // CHANGE: 2025-11-14 - Remove debug spam
        // std::cout << "[EnemyManager] Processing turn for: " << enemy->name << std::endl;
        
        // Re-enqueue for next round
        turnQueue.enqueue(enemy);
    }
}

void EnemyManager::update(float deltaTime) {
    static float globalBobTime = 0.f;
    globalBobTime += deltaTime;
    
    // Update each enemy's animation and AI behavior
    for (auto& enemy : enemies) {
        // Spawn fade-in animation
        if (enemy.isSpawning) {
            enemy.spawnTimer -= deltaTime * 2.f;  // 0.5 seconds to fade in
            if (enemy.spawnTimer <= 0.f) {
                enemy.spawnTimer = 0.f;
                enemy.isSpawning = false;
            }
        }
        
        // Idle bobbing animation (sinusoidal)
        enemy.bobOffset = std::sin(globalBobTime * 3.f + enemy.id * 0.5f) * 2.f;
        
        // ═══════════════════════════════════════════════════════════════════════
        // MOVEMENT TIMER - Enemies move at intervals, not every frame
        // ═══════════════════════════════════════════════════════════════════════
        enemy.moveTimer -= deltaTime;
        
        if (enemy.moveTimer <= 0.f) {
            enemy.moveTimer = 0.5f + (std::rand() % 50) / 100.f;  // Move every 0.5-1.0 seconds
            
            // ═══════════════════════════════════════════════════════════════════════
            // PLAYER DETECTION - Check if player is within range
            // ═══════════════════════════════════════════════════════════════════════
            int detectionRange = 5;
            int attackRange = enemy.range;
            
            // Calculate distance to player target (set by Game class)
            int dx = enemy.targetX - enemy.x;
            int dy = enemy.targetY - enemy.y;
            float distToPlayer = std::sqrt(static_cast<float>(dx * dx + dy * dy));
            
            if (distToPlayer <= attackRange) {
                // In attack range - stop and prepare attack
                enemy.isAlerted = true;
                enemy.alertTimer = EnemyData::ALERT_DURATION;
                enemy.isPatrolling = false;
            }
            else if (distToPlayer <= detectionRange) {
                // Detected player - chase!
                enemy.isAlerted = true;
                enemy.alertTimer = EnemyData::ALERT_DURATION;
                enemy.isPatrolling = false;
                
                // Move towards player
                int moveX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
                int moveY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
                
                // Move one step (prefer diagonal if possible)
                enemy.x += moveX;
                enemy.y += moveY;
            }
            else if (enemy.isPatrolling) {
                // ═══════════════════════════════════════════════════════════════════════
                // PATROL AI - Move towards patrol target
                // ═══════════════════════════════════════════════════════════════════════
                int pdx = enemy.patrolTargetX - enemy.x;
                int pdy = enemy.patrolTargetY - enemy.y;
                
                if (pdx != 0 || pdy != 0) {
                    // Move one step towards patrol target
                    int moveX = (pdx > 0) ? 1 : (pdx < 0) ? -1 : 0;
                    int moveY = (pdy > 0) ? 1 : (pdy < 0) ? -1 : 0;
                    enemy.x += moveX;
                    enemy.y += moveY;
                } else {
                    // Reached patrol target, pick new one
                    enemy.patrolTimer = 0.f;
                }
            }
            
            // Pick new patrol target periodically
            if (enemy.isPatrolling && !enemy.isAlerted) {
                enemy.patrolTimer -= 0.5f;  // Decrease by move interval
                if (enemy.patrolTimer <= 0.f) {
                    enemy.patrolTimer = EnemyData::PATROL_INTERVAL;
                    int rx = (std::rand() % (EnemyData::PATROL_RADIUS * 2 + 1)) - EnemyData::PATROL_RADIUS;
                    int ry = (std::rand() % (EnemyData::PATROL_RADIUS * 2 + 1)) - EnemyData::PATROL_RADIUS;
                    enemy.patrolTargetX = enemy.patrolStartX + rx;
                    enemy.patrolTargetY = enemy.patrolStartY + ry;
                }
            }
        }
        
        // ═══════════════════════════════════════════════════════════════════════
        // SMOOTH VISUAL POSITION LERP
        // ═══════════════════════════════════════════════════════════════════════
        float targetX = static_cast<float>(enemy.x);
        float targetY = static_cast<float>(enemy.y);
        float lerpSpeed = EnemyData::MOVE_LERP_SPEED * deltaTime;
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

void EnemyManager::render(sf::RenderWindow& window, float tileSize) const {
    for (const auto& enemy : enemies) {
        // Calculate spawn animation alpha - ensure visible even during spawn
        uint8_t spawnAlpha = 255;
        if (enemy.isSpawning && enemy.spawnTimer > 0.f) {
            // Fade from 50% to 100% during spawn
            spawnAlpha = static_cast<uint8_t>(128 + 127 * (1.f - enemy.spawnTimer));
        }
        
        // Calculate bob offset for Y position
        float bobY = enemy.bobOffset;
        
        // Use smooth visual position for rendering
        float renderX = enemy.visualX * tileSize;
        float renderY = enemy.visualY * tileSize;
        
        // Draw shadow first (below character)
        sf::CircleShape shadow(SHADOW_RADIUS);
        shadow.setFillColor(sf::Color(0, 0, 0, static_cast<uint8_t>(SHADOW_ALPHA * spawnAlpha / 255)));
        shadow.setScale(sf::Vector2f(SHADOW_SCALE_X, SHADOW_SCALE_Y));
        shadow.setPosition(sf::Vector2f(renderX + SHADOW_OFFSET_X, renderY + SHADOW_OFFSET_Y));
        window.draw(shadow);
        
        // 🎮 Alert indicator (exclamation mark above enemy)
        if (enemy.isAlerted) {
            sf::CircleShape alertMark(4.f);
            alertMark.setFillColor(sf::Color(255, 50, 50));
            alertMark.setPosition(sf::Vector2f(renderX + tileSize / 2.f - 4.f, renderY - 10.f));
            window.draw(alertMark);
        }
        
        // ═══════════════════════════════════════════════════════════════════════
        // ENEMY SPRITES - Using DebtsInTheDepthsAssets from AssetManager
        // ═══════════════════════════════════════════════════════════════════════
        
        // Use cached texture key from enemy data
        std::string textureKey = enemy.textureKey.empty() ? "goblin" : enemy.textureKey;
        
        // Debug log (once per enemy)
        static std::unordered_set<int> loggedEnemies;
        if (loggedEnemies.find(enemy.id) == loggedEnemies.end()) {
            bool hasTexture = AssetManager::getInstance().hasTexture(textureKey);
            std::cout << "[DEBUG] Enemy " << enemy.name << " textureKey: '" << textureKey 
                      << "' exists=" << (hasTexture ? "YES" : "NO") << std::endl;
            loggedEnemies.insert(enemy.id);
        }
        
        sf::Texture* enemyTexture = AssetManager::getInstance().getTexture(textureKey);
        if (enemyTexture) {
            sf::Sprite enemySprite(*enemyTexture);
            
            // Center the sprite
            sf::FloatRect bounds = enemySprite.getLocalBounds();
            enemySprite.setOrigin(sf::Vector2f(bounds.size.x / 2.0f, bounds.size.y / 2.0f));
            
            // Apply smooth position and bob offset
            enemySprite.setPosition(sf::Vector2f(
                renderX + tileSize / 2.0f, 
                renderY + tileSize / 2.0f + bobY
            ));
            
            // Scale based on enemy type + spawn animation scale
            float scaleMult = (enemy.type == "boss") ? BOSS_SCALE_MULT : NORMAL_SCALE_MULT;
            float spawnScale = enemy.isSpawning ? (1.f - enemy.spawnTimer * 0.3f) : 1.f;
            float scaleX = (tileSize * scaleMult * spawnScale) / bounds.size.x;
            float scaleY = (tileSize * scaleMult * spawnScale) / bounds.size.y;
            enemySprite.setScale(sf::Vector2f(scaleX, scaleY));
            
            // Apply spawn fade alpha
            enemySprite.setColor(sf::Color(255, 255, 255, spawnAlpha));
            
            window.draw(enemySprite);
        } else {
            // Fallback to circles if texture not loaded
            float radius = tileSize * FALLBACK_RADIUS_MULT;
            if (enemy.type == "boss") {
                radius = tileSize * BOSS_RADIUS_MULT;
            }
            
            sf::CircleShape enemyCircle(radius);
            enemyCircle.setPosition(sf::Vector2f(enemy.x * tileSize + tileSize * 0.15f, 
                                                 enemy.y * tileSize + tileSize * 0.15f));
            
            // Color based on enemy type
            if (enemy.type == "melee") {
                enemyCircle.setFillColor(sf::Color(200, 50, 50));
            } else if (enemy.type == "ranged") {
                enemyCircle.setFillColor(sf::Color(100, 100, 200));
            } else if (enemy.type == "boss") {
                enemyCircle.setFillColor(sf::Color(120, 0, 120));
            }
            
            enemyCircle.setOutlineThickness(2.0f);
            enemyCircle.setOutlineColor(sf::Color(0, 0, 0));
            window.draw(enemyCircle);
        }
        
        // Health bar background
        sf::RectangleShape healthBg(sf::Vector2f(tileSize * HEALTH_BAR_WIDTH_MULT, HEALTH_BAR_HEIGHT));
        healthBg.setPosition(sf::Vector2f(enemy.x * tileSize + tileSize * HEALTH_BAR_X_OFFSET, 
                                          enemy.y * tileSize + tileSize * HEALTH_BAR_Y_OFFSET));
        healthBg.setFillColor(sf::Color(50, 50, 50));
        window.draw(healthBg);
        
        // Health bar foreground
        float healthPercent = static_cast<float>(enemy.health) / enemy.maxHealth;
        sf::RectangleShape healthBar(sf::Vector2f(tileSize * HEALTH_BAR_WIDTH_MULT * healthPercent, HEALTH_BAR_HEIGHT));
        healthBar.setPosition(sf::Vector2f(enemy.x * tileSize + tileSize * HEALTH_BAR_X_OFFSET, 
                                           enemy.y * tileSize + tileSize * HEALTH_BAR_Y_OFFSET));
        
        // Health bar color changes with health level
        if (healthPercent > HEALTH_GOOD_THRESHOLD) {
            healthBar.setFillColor(sf::Color(50, 200, 50));  // Green
        } else if (healthPercent > HEALTH_WARN_THRESHOLD) {
            healthBar.setFillColor(sf::Color(255, 200, 0));  // Yellow
        } else {
            healthBar.setFillColor(sf::Color(255, 50, 50));  // Red
        }
        window.draw(healthBar);
    }
}

EnemyData* EnemyManager::findNearestEnemy(int playerX, int playerY) {
    if (enemies.empty()) {
        // CHANGE: 2025-11-14 - Add diagnostic logging
        std::cerr << "[DEBUG] findNearestEnemy: No enemies available" << std::endl;
        return nullptr;
    }
    
    EnemyData* nearest = nullptr;
    int minDistance = INT_MAX;
    
    for (auto& enemy : enemies) {
        int dx = enemy.x - playerX;
        int dy = enemy.y - playerY;
        int distance = dx * dx + dy * dy;  // Squared distance (no need for sqrt)
        
        if (distance < minDistance) {
            minDistance = distance;
            nearest = &enemy;
        }
    }
    
    if (!nearest) {
        std::cerr << "[DEBUG] findNearestEnemy: Found no nearest enemy despite " << enemies.size() << " enemies available" << std::endl;
    }
    
    return nearest;
}

EnemyData* EnemyManager::getEnemyById(int id) {
    for (auto& enemy : enemies) {
        if (enemy.id == id) {
            return &enemy;
        }
    }
    
    // CHANGE: 2025-11-14 - Add warning instead of silent nullptr
    std::cerr << "[WARNING] getEnemyById: Enemy with ID " << id << " not found" << std::endl;
    return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// SPATIAL INDEX (DSA: SpatialHash for O(k) enemy lookup)
// Builds a spatial partition grid for fast proximity queries
// ═══════════════════════════════════════════════════════════════════════════

void EnemyManager::buildSpatialIndex() {
    enemyGrid.clear();
    
    // Insert all living enemies into spatial hash (O(n))
    for (auto& enemy : enemies) {
        if (enemy.health > 0) {
            // Use tile coordinates * cell size for world position
            enemyGrid.insert(enemy.x * SPATIAL_CELL_SIZE, enemy.y * SPATIAL_CELL_SIZE, enemy.id);
        }
    }
    
    std::cout << "[EnemyManager] Spatial index built with " << enemyGrid.size() 
              << " enemies in " << enemyGrid.bucketCount() << " cells" << std::endl;
}

std::vector<EnemyData*> EnemyManager::findNearbyEnemies(int x, int y, int radius) {
    std::vector<EnemyData*> result;
    
    // Query spatial hash for nearby enemy IDs (O(k) where k = nearby enemies)
    auto nearbyIds = enemyGrid.queryNearby(x * SPATIAL_CELL_SIZE, y * SPATIAL_CELL_SIZE, radius * SPATIAL_CELL_SIZE);
    
    // Resolve IDs to EnemyData pointers
    for (int id : nearbyIds) {
        EnemyData* enemy = getEnemyById(id);
        if (enemy && enemy->health > 0) {
            // Verify actual distance (spatial hash gives approximate results)
            int dx = enemy->x - x;
            int dy = enemy->y - y;
            float dist = std::sqrt(static_cast<float>(dx * dx + dy * dy));
            
            if (dist <= radius) {
                result.push_back(enemy);
            }
        }
    }
    
    return result;
}
