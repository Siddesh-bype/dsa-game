// CHANGE: 2025-11-10 — Added adaptive AI system for progressive difficulty
// Enemy AI scales from floors 1-10 with increasing intelligence

#include "Enemy.h"
#include "AssetManager.h"
#include <iostream>

EnemyManager::EnemyManager() : nextEnemyId(0) {
}

EnemyManager::~EnemyManager() {
}

// Calculate AI level based on floor (0=Random, 1=Chase, 2=Dijkstra, 3=Flank, 4=Boss)
int EnemyManager::calculateAILevel(int floor) const {
    if (floor <= 2) return 0;  // Floors 1-2: Random walk
    if (floor <= 4) return 1;  // Floors 3-4: BFS Chase
    if (floor <= 6) return 2;  // Floors 5-6: Dijkstra path
    if (floor <= 8) return 3;  // Floors 7-8: Cooperative flank
    return 4;                  // Floors 9-10: Boss AI
}

void EnemyManager::setEnemyAILevel(EnemyData& enemy, int floor) {
    enemy.floorLevel = floor;
    
    // Bosses always get max AI
    if (enemy.type == "boss") {
        enemy.aiLevel = 4;
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

void EnemyManager::spawnEnemy(const std::string& name, const std::string& type, int x, int y, 
                              int health, int damage, int range, float speed, int floor) {
    EnemyData enemy(nextEnemyId++, name, type, health, damage, x, y, range, speed);
    setEnemyAILevel(enemy, floor);
    
    // CHANGE: 2025-11-14 - Cache texture key to avoid string searches every frame
    if (name.find("Slime") != std::string::npos || name.find("Bogslium") != std::string::npos) {
        enemy.textureKey = "slime";
    } else if (name.find("Goblin") != std::string::npos) {
        enemy.textureKey = "goblin";
    } else if (name.find("Orc") != std::string::npos) {
        enemy.textureKey = "orc";
    } else if (name.find("Skeleton") != std::string::npos) {
        enemy.textureKey = "skeleton";
    } else if (name.find("Shadow") != std::string::npos || name.find("Wraith") != std::string::npos) {
        enemy.textureKey = "wraith";
    } else if (name.find("Vampire") != std::string::npos || name.find("Batilisk") != std::string::npos) {
        enemy.textureKey = "vampire";
    } else if (name.find("Lich") != std::string::npos || name.find("Necromancer") != std::string::npos) {
        enemy.textureKey = "lich";
    } else if (name.find("Dragon") != std::string::npos) {
        enemy.textureKey = "dragon";
    } else if (name.find("Dark Mage") != std::string::npos || name.find("Mage") != std::string::npos) {
        enemy.textureKey = "dark_mage";
    } else if (name.find("Gargoyle") != std::string::npos) {
        enemy.textureKey = "gargoyle";
    } else if (name.find("Minotaur") != std::string::npos) {
        enemy.textureKey = "minotaur";
    } else if (type == "boss") {
        enemy.textureKey = "dragon";
    }
    
    enemies.push_back(enemy);
    
    std::cout << "[EnemyManager] Spawned " << name << " (" << type << ") at (" << x << ", " << y 
              << ") with " << health << " HP, range " << range << ", AI=" << enemy.aiLevel << std::endl;
}

// CHANGE: 2025-11-10 - Spawn enemy with drop table for loot system
void EnemyManager::spawnEnemyWithDrops(const std::string& name, const std::string& type, int x, int y, 
                                       int health, int damage, int range, float speed, int floor, 
                                       const nlohmann::json& dropTable) {
    EnemyData enemy(nextEnemyId++, name, type, health, damage, x, y, range, speed);
    setEnemyAILevel(enemy, floor);
    enemy.dropTableJson = dropTable;  // Store drop table
    
    // CHANGE: 2025-11-14 - Cache texture key like in spawnEnemy
    if (name.find("Slime") != std::string::npos || name.find("Bogslium") != std::string::npos) {
        enemy.textureKey = "slime";
    } else if (name.find("Goblin") != std::string::npos) {
        enemy.textureKey = "goblin";
    } else if (name.find("Orc") != std::string::npos) {
        enemy.textureKey = "orc";
    } else if (name.find("Skeleton") != std::string::npos) {
        enemy.textureKey = "skeleton";
    } else if (name.find("Shadow") != std::string::npos || name.find("Wraith") != std::string::npos) {
        enemy.textureKey = "wraith";
    } else if (name.find("Vampire") != std::string::npos || name.find("Batilisk") != std::string::npos) {
        enemy.textureKey = "vampire";
    } else if (name.find("Lich") != std::string::npos || name.find("Necromancer") != std::string::npos) {
        enemy.textureKey = "lich";
    } else if (name.find("Dragon") != std::string::npos) {
        enemy.textureKey = "dragon";
    } else if (name.find("Dark Mage") != std::string::npos || name.find("Mage") != std::string::npos) {
        enemy.textureKey = "dark_mage";
    } else if (name.find("Gargoyle") != std::string::npos) {
        enemy.textureKey = "gargoyle";
    } else if (name.find("Minotaur") != std::string::npos) {
        enemy.textureKey = "minotaur";
    } else if (type == "boss") {
        enemy.textureKey = "dragon";
    }
    
    enemies.push_back(enemy);
    
    std::cout << "[EnemyManager] Spawned " << name << " (" << type << ") at (" << x << ", " << y 
              << ") with " << health << " HP, AI=" << enemy.aiLevel 
              << ", drops=" << dropTable.size() << " items" << std::endl;
}

void EnemyManager::removeEnemy(int id) {
    for (auto it = enemies.begin(); it != enemies.end(); ++it) {
        if (it->id == id) {
            std::cout << "[EnemyManager] Removed enemy: " << it->name << std::endl;
            enemies.erase(it);
            return;
        }
    }
}

void EnemyManager::removeDeadEnemies() {
    auto it = enemies.begin();
    while (it != enemies.end()) {
        if (it->health <= 0) {
            std::cout << "[EnemyManager] Removed dead enemy: " << it->name << std::endl;
            it = enemies.erase(it);
        } else {
            ++it;
        }
    }
}

void EnemyManager::initializeTurnQueue() {
    turnQueue.clear();
    
    std::cout << "[EnemyManager] Initializing turn queue with " << enemies.size() << " enemies" << std::endl;
    
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
        std::cout << "[EnemyManager] Turn queue empty, reinitializing..." << std::endl;
        initializeTurnQueue();
    }
    
    if (!turnQueue.isEmpty()) {
        EnemyData* enemy = turnQueue.front();
        turnQueue.dequeue();
        
        std::cout << "[EnemyManager] Processing turn for: " << enemy->name 
                  << " (DMG: " << enemy->damage << ")" << std::endl;
        
        // Re-enqueue for next round
        turnQueue.enqueue(enemy);
    }
}

void EnemyManager::update(float deltaTime) {
    // Update enemy logic
}

void EnemyManager::render(sf::RenderWindow& window, float tileSize) const {
    for (const auto& enemy : enemies) {
        // Draw shadow first (below character)
        sf::CircleShape shadow(10.0f);
        shadow.setFillColor(sf::Color(0, 0, 0, 80));
        shadow.setScale(sf::Vector2f(2.0f, 0.5f));
        shadow.setPosition(sf::Vector2f(enemy.x * tileSize + 6.0f, enemy.y * tileSize + 28.0f));
        window.draw(shadow);
        
        // ═══════════════════════════════════════════════════════════════════════
        // 👹 ENEMY/MONSTER SPRITES - Using DebtsInTheDepths individual sprites
        // ═══════════════════════════════════════════════════════════════════════
        
        // CHANGE: 2025-11-14 - Use cached textureKey instead of string searching every frame
        sf::Texture* enemyTex = AssetManager::getInstance().getTexture(enemy.textureKey);
        if (enemyTex) {
            sf::Sprite enemySprite(*enemyTex);
            float scale = (enemy.type == "boss") ? 2.0f : 1.5f;
            enemySprite.setOrigin(sf::Vector2f(enemyTex->getSize().x / 2.0f, enemyTex->getSize().y / 2.0f));
            enemySprite.setPosition(sf::Vector2f(enemy.x * tileSize + 16.0f, enemy.y * tileSize + 16.0f));
            enemySprite.setScale(sf::Vector2f(scale, scale));
            window.draw(enemySprite);
        } else {
            // Fallback to circles if texture not loaded
            float radius = tileSize * 0.35f;
            if (enemy.type == "boss") {
                radius = tileSize * 0.45f;
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
        sf::RectangleShape healthBg(sf::Vector2f(tileSize * 0.7f, 5.0f));
        healthBg.setPosition(sf::Vector2f(enemy.x * tileSize + tileSize * 0.15f, 
                                          enemy.y * tileSize + tileSize * 0.05f));
        healthBg.setFillColor(sf::Color(50, 50, 50));
        window.draw(healthBg);
        
        // Health bar foreground
        float healthPercent = static_cast<float>(enemy.health) / enemy.maxHealth;
        sf::RectangleShape healthBar(sf::Vector2f(tileSize * 0.7f * healthPercent, 5.0f));
        healthBar.setPosition(sf::Vector2f(enemy.x * tileSize + tileSize * 0.15f, 
                                           enemy.y * tileSize + tileSize * 0.05f));
        
        // Health bar color changes with health level
        if (healthPercent > 0.6f) {
            healthBar.setFillColor(sf::Color(50, 200, 50));  // Green
        } else if (healthPercent > 0.3f) {
            healthBar.setFillColor(sf::Color(255, 200, 0));  // Yellow
        } else {
            healthBar.setFillColor(sf::Color(255, 50, 50));  // Red
        }
        window.draw(healthBar);
    }
}

EnemyData* EnemyManager::findNearestEnemy(int playerX, int playerY) {
    if (enemies.empty()) return nullptr;
    
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
