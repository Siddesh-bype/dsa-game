// Game.cpp - Core game loop and system initialization
// DSA structures: Graph (rooms), Stack (backtrack), HashTable (items), Heap (loot), BinaryTree (skills)

#include "Game.h"
#include "Player.h"
#include "Dungeon.h"
#include "Enemy.h"
#include "SkillTree.h"
#include "UIManager.h"
#include "Renderer.h"
#include "AssetManager.h"
#include "DungeonLevelManager.h"
#include "ItemManager.h"
#include "ItemNew.h"
#include "Loot.h"
#include "DropTable.h"
#include "Shop.h"
#include "SaveSystem.h"
#include "SettingsManager.h"
#include "DataStructures/Heap.h"
#include "DataStructures/HashTable.h"
#include "DataStructures/AStar.h"
#include "DataStructures/SpatialHash.h"
#include "DataStructures/ObjectPool.h"
#include "DataStructures/LRUCache.h"
#include <iostream>

Game::Game() 
    : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Dungeon Explorer - DSA Game"),
      isRunning(true),
      isPaused(false),
      currentState(GameState::Playing),
      currentFloor(STARTING_FLOOR),
      exitStairsPosition({0, 0}),
      levelManager(std::make_unique<DungeonLevelManager>()) {
    window.setFramerateLimit(static_cast<unsigned int>(targetFPS));
}

Game::~Game() = default;

void Game::initialize() {
    std::cout << "\n╔════════════════════════════════════════╗\n"
              <<   "║    DUNGEON EXPLORER - DSA Edition      ║\n"
              <<   "╚════════════════════════════════════════╝\n" << std::endl;
    
    // Load core data files
    AssetManager::getInstance().loadFromManifest("assets/data/kenney_manifest.json");
    levelManager->loadLevels("assets/data/levels.json");
    ItemManager::getInstance().loadItems("assets/data/items.json");
    
    // Initialize core systems
    player = std::make_unique<Player>();
    dungeon = std::make_unique<Dungeon>();
    renderer = std::make_unique<Renderer>(&window, TILE_SIZE);
    uiManager = std::make_unique<UIManager>(this);
    enemyManager = std::make_unique<EnemyManager>();
    
    // Generate first floor
    levelManager->generateLevel(currentFloor, *dungeon, *enemyManager, *player);
    
    // Spawn player in first room
    if (!dungeon->getRooms().empty()) {
        const auto& room = dungeon->getRooms()[0];
        player->initialize(room.x + 1, room.y + 1);
    }
    
    // Starting items
    player->addItem(ItemNew("dagger_rusty", "Rusty Dagger", "weapon", 1, 10));
    player->addItem(ItemNew("coin_gold", "Gold Coin", "treasure", 1, 100));
    player->addItem(ItemNew("ring_silver", "Silver Ring", "treasure", 1, 75));
    
    // UI setup
    uiManager->initialize();
    if (!uiManager->loadFont("C:\\Windows\\Fonts\\arial.ttf")) {
        uiManager->loadFont("C:\\Windows\\Fonts\\calibri.ttf");
    }
    
    // Skill tree
    skillTree = std::make_unique<SkillTree>();
    skillTree->initialize();
    skillTree->addPoints(STARTING_SKILL_POINTS);
    
    // Shop
    shop = std::make_unique<Shop>();
    shop->initialize();
    
    // Note: SaveSystem and SettingsManager are singletons - access via ::getInstance()
    
    // Enemy turn queue
    enemyManager->initializeTurnQueue();
    
    // DSA summary
    std::cout << "DSA Structures Active:\n"
              << "  • Graph: " << dungeon->getRooms().size() << " rooms (BFS/DFS/Dijkstra)\n"
              << "  • HashTable: " << ItemManager::getInstance().getItemCount() << " items\n"
              << "  • LinkedList: Player inventory\n"
              << "  • Stack: Movement backtracking [B]\n"
              << "  • BinaryTree: Skill tree [K]\n"
              << "  • Heap: Loot priority system\n" << std::endl;
    
    std::cout << levelManager->getFloorDisplayText(currentFloor) << "\n"
              << "Controls: WASD=Move, Space=Attack, E=Interact, I=Inventory\n" << std::endl;
}

void Game::run() {
    initialize();
    
    sf::Clock clock;
    while (window.isOpen() && isRunning) {
        float deltaTime = std::min(clock.restart().asSeconds(), MAX_DELTA_TIME);
        
        processEvents();
        if (!isPaused) update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            isRunning = false;
        }
        if (event->is<sf::Event::KeyPressed>() && currentState == GameState::Playing) {
            handleInput(*event);
        }
        
        // Mouse movement for ItemWheel hover
        if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
            if (uiManager && uiManager->isItemWheelOpen()) {
                uiManager->getItemWheel().handleMouseMove(
                    sf::Vector2f(static_cast<float>(mouseMoved->position.x), 
                                 static_cast<float>(mouseMoved->position.y)));
            }
        }
        
        // Mouse click for ItemWheel item selection
        if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePressed->button == sf::Mouse::Button::Left) {
                if (uiManager && uiManager->isItemWheelOpen() && player) {
                    const int selectedIndex = uiManager->getItemWheel().handleClick();
                    if (selectedIndex >= 0) {
                        // Use selected item (1-indexed slot)
                        if (player->useHotbarItem(selectedIndex + 1)) {
                            const auto playerPos = player->getPosition();
                            uiManager->addFloatingText(
                                "Item Used!", 
                                playerPos.x * TILE_SIZE, 
                                playerPos.y * TILE_SIZE - 20, 
                                sf::Color(100, 255, 100)
                            );
                        }
                        uiManager->getItemWheel().close();
                    }
                }
            }
        }
    }
}


void Game::handleInput(const sf::Event& event) {
    if (!player || !dungeon) return;
    
    // Shop intercepts all input when open
    if (shop && shop->isShopOpen()) {
        if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
            shop->handleInput(key->code, player.get());
        }
        return;
    }
    
    const auto* key = event.getIf<sf::Event::KeyPressed>();
    if (!key) return;
    
    const Position currentPos = player->getPosition();
    const float px = currentPos.x * TILE_SIZE;
    const float py = currentPos.y * TILE_SIZE;
    
    // Helper for floating text at player position
    auto showText = [&](const std::string& msg, sf::Color color, float yOffset = -10.f) {
        if (uiManager) uiManager->addFloatingText(msg, px, py + yOffset, color);
    };
    
    Position newPos = currentPos;
    
    switch (key->code) {
        // Movement
        case sf::Keyboard::Key::W: case sf::Keyboard::Key::Up:    newPos.y--; break;
        case sf::Keyboard::Key::S: case sf::Keyboard::Key::Down:  newPos.y++; break;
        case sf::Keyboard::Key::A: case sf::Keyboard::Key::Left:  newPos.x--; break;
        case sf::Keyboard::Key::D: case sf::Keyboard::Key::Right: newPos.x++; break;
        
        // Backtrack
        case sf::Keyboard::Key::B:
            player->backtrack();
            return;
        
        // UI toggles
        case sf::Keyboard::Key::I: uiManager->toggleInventory(); return;
        case sf::Keyboard::Key::K: uiManager->toggleSkillTree(); return;
        case sf::Keyboard::Key::M: uiManager->toggleMiniMap(); return;
        case sf::Keyboard::Key::P: if (shop) shop->toggle(); return;
        case sf::Keyboard::Key::Tab:
            if (uiManager) {
                const auto winSize = window.getSize();
                uiManager->toggleItemWheel({winSize.x / 2.f, winSize.y / 2.f}, player.get());
            }
            return;
        
        // Quick Save/Load
        case sf::Keyboard::Key::F6:
            if (player && dungeon && skillTree) {
                auto& save = SaveSystem::getInstance();
                if (save.saveGame("quicksave", *player, *dungeon, *skillTree, currentFloor)) {
                    showText("Game Saved!", sf::Color(100, 255, 100));
                } else {
                    showText("Save Failed!", sf::Color(255, 100, 100));
                }
            }
            return;
        
        case sf::Keyboard::Key::F7:
            if (player && dungeon && skillTree) {
                auto& save = SaveSystem::getInstance();
                if (save.loadGame("quicksave", *player, *dungeon, *skillTree, currentFloor)) {
                    showText("Game Loaded!", sf::Color(100, 255, 100));
                } else {
                    showText("No Save Found!", sf::Color(255, 200, 100));
                }
            }
            return;

        
        // Quick skill unlock
        case sf::Keyboard::Key::O:
            if (skillTree && skillTree->getAvailablePoints() > 0) {
                auto tryUnlock = [&](auto node, int hotkey) -> bool {
                    if (!node || node->data.unlocked || !skillTree->canUnlock(node)) return false;
                    if (node->data.type == "active" && node->data.hotkey == 0) node->data.hotkey = hotkey;
                    skillTree->unlockSkill(node);
                    std::string info = (node->data.hotkey > 0) ? " [" + std::to_string(node->data.hotkey) + "]" : "";
                    showText("Unlocked: " + node->data.name + info, sf::Color(100, 255, 100), 0);
                    return true;
                };
                
                auto root = skillTree->getRoot();
                if (!root || 
                    !(root->left && tryUnlock(root->left, 2)) &&
                    !(root->left && root->left->left && tryUnlock(root->left->left, 3)) &&
                    !(root->left && root->left->left && root->left->left->left && tryUnlock(root->left->left->left, 4)) &&
                    !(root->right && root->right->right && tryUnlock(root->right->right, 5)) &&
                    !(root->right && tryUnlock(root->right, 0))) {
                    showText("No skills available!", sf::Color(255, 100, 100));
                }
            } else if (skillTree) {
                showText("No skill points!", sf::Color(255, 200, 100));
            }
            return;
        
        // Use first inventory item
        case sf::Keyboard::Key::U:
            if (player->getInventoryNew().size() > 0) {
                std::string firstId;
                player->getInventoryNew().traverse([&](const ItemNew& item) {
                    if (firstId.empty()) firstId = item.id;
                });
                if (!firstId.empty()) player->useItem(firstId);
            }
            return;
        
        // Skills (1-5)
        case sf::Keyboard::Key::Num1: activateSkill(1); return;
        case sf::Keyboard::Key::Num2: activateSkill(2); return;
        case sf::Keyboard::Key::Num3: activateSkill(3); return;
        case sf::Keyboard::Key::Num4: activateSkill(4); return;
        case sf::Keyboard::Key::Num5: activateSkill(5); return;
        
        // Combat
        case sf::Keyboard::Key::Space:
            if (enemyManager) attackNearestEnemy();
            return;
        
        // Healing potion
        case sf::Keyboard::Key::H:
            if (player->usePotion()) showText("+50 HP", sf::Color(100, 255, 100));
            return;
        
        // Interaction (E key) - pickup loot, open doors, descend stairs
        case sf::Keyboard::Key::E: {
            // Priority 1: Loot pickup
            for (auto it = loots.begin(); it != loots.end(); ++it) {
                if (it->isAdjacentTo(currentPos.x, currentPos.y)) {
                    const ItemNew& item = it->getItem();
                    if (item.type == "treasure") {
                        player->addGold(item.value);
                        showText("+" + std::to_string(item.value) + " Gold", sf::Color(255, 215, 0));
                    } else {
                        player->addItem(item);
                        showText("+" + item.name, item.getRarityColor());
                    }
                    loots.erase(it);
                    return;
                }
            }
            
            // Priority 2: Door
            for (auto& door : doors) {
                if (std::abs(door.x - currentPos.x) <= 1 && std::abs(door.y - currentPos.y) <= 1 && !door.isOpen) {
                    if (door.requiresKey && !player->hasItem("dungeon_key")) {
                        showText("Locked - Need Key", sf::Color(255, 100, 100));
                        return;
                    }
                    door.isOpen = true;
                    dungeon->setTile(door.x, door.y, TileType::Floor);
                    showText("Door Opened", sf::Color(100, 255, 100));
                    return;
                }
            }
            
            // Priority 3: Stairs
            if (dungeon->isAdjacentToStairs(currentPos.x, currentPos.y)) {
                if (enemyManager && !enemyManager->isEmpty()) {
                    showText("Defeat all enemies first!", sf::Color(255, 100, 100));
                } else {
                    nextFloor();
                }
            } else {
                showText("Nothing nearby", sf::Color(150, 150, 150));
            }
            return;
        }
        
        // Lighting toggle
        case sf::Keyboard::Key::L:
            if (renderer) {
                renderer->toggleLighting();
                showText(renderer->isLightingEnabled() ? "Lighting ON" : "Lighting OFF", sf::Color(255, 255, 100));
            }
            return;
        
        // Debug toggles
        case sf::Keyboard::Key::F3:
            debugShowBoundingBoxes = !debugShowBoundingBoxes;
            showText(debugShowBoundingBoxes ? "Debug Boxes ON" : "Debug Boxes OFF", sf::Color(0, 255, 255));
            return;
        
        case sf::Keyboard::Key::F4:
            debugRetroMode = !debugRetroMode;
            showText(debugRetroMode ? "1-Bit Mode ON" : "1-Bit Mode OFF", sf::Color(255, 255, 0));
            return;
        
        case sf::Keyboard::Key::F5:
            AssetManager::getInstance().togglePack();
            showText(AssetManager::getInstance().getCurrentPack() == AssetPack::TinyDungeon ? "Colorful Mode" : "Retro Mode", 
                     sf::Color(255, 200, 100));
            return;
        
        // Escape
        case sf::Keyboard::Key::Escape:
            if (currentState == GameState::GameOver) {
                window.close();
                isRunning = false;
            } else {
                uiManager->hideAll();
            }
            return;
        
        default: return;
    }
    
    // Movement execution
    if (dungeon->isWalkable(newPos.x, newPos.y)) {
        player->moveTo(newPos);
        
        // Auto-descend if stepping on exit
        if (newPos.x == exitStairsPosition.x && newPos.y == exitStairsPosition.y &&
            dungeon->getTile(newPos.x, newPos.y) == TileType::Exit) {
            nextFloor();
            return;
        }
        
        moveEnemiesWithBFS();
    }
}


void Game::update(float deltaTime) {
    // Timing
    totalTime += deltaTime;
    currentFPS = (deltaTime > 0.f) ? 1.f / deltaTime : 0.f;
    
    // Combo decay
    if (comboTimer > 0.f && (comboTimer -= deltaTime) <= 0.f) {
        resetCombo();
    }
    
    // Core systems
    updateScreenShake(deltaTime);
    updateCamera(deltaTime);
    
    // State-specific updates
    switch (currentState) {
        case GameState::MainMenu: updateMainMenu(deltaTime); break;
        case GameState::Playing:  updatePlaying(deltaTime);  break;
        default: break;
    }
    
    // World updates (always active for animations)
    updateCombatEffects(deltaTime);
    updateLoots(deltaTime);
    
    // UI
    if (uiManager && player) {
        uiManager->update(deltaTime);
        uiManager->updateHUD(*player);
    }
}

void Game::updateMainMenu(float /* deltaTime */) {
    // Menu update logic - handled by UI state
}

void Game::updatePlaying(float deltaTime) {
    // Real-time updates for animations
    if (player) player->update(deltaTime);
    
    // Update enemy targeting and AI
    if (enemyManager && player) {
        const Position playerPos = player->getPosition();
        for (auto& enemy : enemyManager->getMutableEnemies()) {
            enemy.targetX = playerPos.x;
            enemy.targetY = playerPos.y;
        }
        enemyManager->update(deltaTime);
    }
    
    // Dungeon ambient effects
    if (dungeon) dungeon->updateAmbientParticles(deltaTime);
}

void Game::attackNearestEnemy() {
    if (!player || !enemyManager || !uiManager) return;
    
    const Position playerPos = player->getPosition();
    const float px = playerPos.x * TILE_SIZE;
    const float py = playerPos.y * TILE_SIZE;
    
    EnemyData* enemy = enemyManager->findNearestEnemy(playerPos.x, playerPos.y);
    if (!enemy) return;
    
    // Range check
    const int distance = std::abs(enemy->x - playerPos.x) + std::abs(enemy->y - playerPos.y);
    if (distance > ATTACK_RANGE_TILES) {
        uiManager->addFloatingText("Too far!", px, py, sf::Color(150, 150, 150));
        return;
    }
    
    // Apply damage
    const float ex = enemy->x * TILE_SIZE;
    const float ey = enemy->y * TILE_SIZE;
    const int damage = player->attackEnemy();
    enemy->health -= damage;
    
    addCombatEffect("swing", ex, ey, EFFECT_DURATION_SHORT);
    uiManager->addFloatingText("-" + std::to_string(damage), ex, ey, sf::Color(255, 150, 50));
    
    // Handle enemy death
    if (enemy->health <= 0) {
        incrementCombo();
        
        // Visual feedback scaled by combo
        const float scale = 1.f + comboCounter * 0.1f;
        addCombatEffect("explosion", ex, ey, EFFECT_DURATION_LONG * scale);
        applyScreenShake(5.f + comboCounter * 0.5f, 0.2f);
        uiManager->setCombo(comboCounter);
        
        // Combo text
        const std::string msg = comboCounter > 1 
            ? "x" + std::to_string(comboCounter) + " COMBO!" 
            : "DEFEATED!";
        const sf::Color color = comboCounter > 3 ? sf::Color(255, 100, 100) : sf::Color(255, 215, 0);
        uiManager->addFloatingText(msg, ex, ey, color);
        
        // XP with combo bonus
        int xp = BASE_XP_GAIN + currentFloor * XP_PER_FLOOR;
        xp = static_cast<int>(xp * (1.f + comboCounter * COMBO_DAMAGE_MULT));
        uiManager->addFloatingText("+" + std::to_string(xp) + " XP", px, py, sf::Color(100, 255, 100));
        player->addExperience(xp);
        
        // Skill points on level up
        if (int pts = player->getSkillPointsEarned(); pts > 0 && skillTree) {
            skillTree->addPoints(pts);
            player->clearSkillPoints();
            uiManager->addFloatingText("+" + std::to_string(pts) + " Skill Point" + (pts > 1 ? "s" : "") + "!", 
                px, py - 30.f, sf::Color(255, 215, 0));
        }
        
        // Loot drops
        auto& items = ItemManager::getInstance();
        if (!enemy->dropTableJson.empty()) {
            DropTable table = DropTable::fromJson(enemy->dropTableJson);
            if (std::string dropId = table.roll(); !dropId.empty() && items.hasItem(dropId)) {
                spawnLootAt(sf::Vector2i(enemy->x, enemy->y), items.getItemById(dropId));
            }
        } else {
            int level = std::max(1, enemy->maxHealth / 20);
            auto loot = items.generateLootTable(level, currentFloor, 2);
            for (const auto& item : loot) {
                spawnLootAt(sf::Vector2i(enemy->x, enemy->y), item);
            }
            if (loot.empty()) {
                dropItemFromEnemy(enemy->name, enemy->x, enemy->y);
            }
        }
        
        enemyManager->removeEnemy(enemy->id);
        checkExitAccess();
    }
}

void Game::activateSkill(int hotkey) {
    if (!player || !skillTree || !enemyManager || !uiManager) return;
    
    Skill* skill = skillTree->getSkillByHotkey(hotkey);
    if (!skill) return;
    
    const Position playerPos = player->getPosition();
    const float px = playerPos.x * TILE_SIZE;
    const float py = playerPos.y * TILE_SIZE;
    
    // Validation checks with visual feedback
    if (!skill->unlocked) {
        uiManager->addFloatingText("Not unlocked!", px, py, sf::Color(200, 50, 50));
        return;
    }
    if (skill->currentCooldown > 0) {
        uiManager->addFloatingText("On cooldown!", px, py, sf::Color(200, 200, 50));
        return;
    }
    if (!player->useMana(skill->manaCost)) {
        uiManager->addFloatingText("Not enough mana!", px, py, sf::Color(100, 100, 255));
        return;
    }
    
    // Helper: Apply single-target damage to nearest enemy
    auto attackNearest = [&](const std::string& effect, const std::string& label, sf::Color color) -> bool {
        EnemyData* target = enemyManager->findNearestEnemy(playerPos.x, playerPos.y);
        if (!target) return false;
        
        addCombatEffect(effect, target->x * TILE_SIZE, target->y * TILE_SIZE, EFFECT_DURATION_MEDIUM);
        
        int damage = player->attackEnemy() + skill->damage;
        target->health -= damage;
        uiManager->addFloatingText("-" + std::to_string(damage) + " " + label, 
            target->x * TILE_SIZE, target->y * TILE_SIZE, color);
        
        if (target->health <= 0) {
            addCombatEffect("explosion", target->x * TILE_SIZE, target->y * TILE_SIZE, EFFECT_DURATION_LONG);
            enemyManager->removeEnemy(target->id);
            checkExitAccess();
        }
        return true;
    };
    
    // Helper: AOE damage to enemies within range
    auto aoeAttack = [&](int range, const std::string& effect, const std::string& label, sf::Color color) -> int {
        int hits = 0;
        for (auto& enemy : enemyManager->getEnemies()) {
            if (std::abs(enemy.x - playerPos.x) <= range && std::abs(enemy.y - playerPos.y) <= range) {
                addCombatEffect(effect, enemy.x * TILE_SIZE, enemy.y * TILE_SIZE, EFFECT_DURATION_SHORT);
                const_cast<EnemyData&>(enemy).health -= skill->damage;
                uiManager->addFloatingText("-" + std::to_string(skill->damage) + label, 
                    enemy.x * TILE_SIZE, enemy.y * TILE_SIZE, color);
                hits++;
            }
        }
        enemyManager->removeDeadEnemies();
        checkExitAccess();
        return hits;
    };
    
    // Execute skill based on ID
    if (skill->id == "slash") {
        attackNearest("large_swing", "SLASH", sf::Color(255, 200, 50));
    } 
    else if (skill->id == "power_strike") {
        attackNearest("large_swing", "POWER!", sf::Color(255, 100, 0));
    } 
    else if (skill->id == "whirlwind" && skill->aoe) {
        int hits = aoeAttack(1, "swing", "", sf::Color(255, 150, 50));
        addCombatEffect("large_swing", px, py, EFFECT_DURATION_LONG);
        uiManager->addFloatingText("WHIRLWIND! (x" + std::to_string(hits) + ")", px, py - 20.f, sf::Color(255, 255, 100));
    } 
    else if (skill->id == "flame_wave" && skill->aoe) {
        int hits = aoeAttack(2, "fire_explosion", " BURN", sf::Color(255, 100, 0));
        addCombatEffect("fire_explosion", px, py, EFFECT_DURATION_LONG);
        uiManager->addFloatingText("FLAME WAVE! (x" + std::to_string(hits) + ")", px, py - 20.f, sf::Color(255, 50, 0));
    } 
    else if (skill->id == "shadow_step") {
        addCombatEffect("ghost_orb", px, py, EFFECT_DURATION_SHORT);
        uiManager->addFloatingText("SHADOW STEP!", px, py, sf::Color(150, 50, 200));
    }
    
    // Set cooldown and trigger enemy turn
    skill->currentCooldown = skill->cooldown;
    uiManager->addFloatingText(skill->name + "!", px, py - 30.f, sf::Color(100, 200, 255));
    skillTree->updateCooldowns();
    moveEnemiesWithBFS();
}

void Game::moveEnemiesWithBFS() {
    if (!enemyManager || !dungeon || !player) return;
    
    // Open auto-clearing doors when all enemies are defeated
    if (enemyManager->isEmpty()) {
        for (auto& door : doors) {
            if (door.openOnClear && !door.isOpen) {
                door.isOpen = true;
                dungeon->setTile(door.x, door.y, TileType::Floor);
            }
        }
        return;  // No enemies to process
    }
    
    const Position playerPos = player->getPosition();
    auto& enemies = const_cast<std::vector<EnemyData>&>(enemyManager->getEnemies());
    
    for (auto& enemy : enemies) {
        const int dx = std::abs(enemy.x - playerPos.x);
        const int dy = std::abs(enemy.y - playerPos.y);
        const int distance = dx + dy;
        
        // Attack if in range
        if (distance <= enemy.attackRange) {
            int damage = std::max(0, enemy.damage - player->getDefense());
            player->takeDamage(damage);
            
            // Visual effect
            const float effectX = (enemy.attackRange > 1 ? enemy.x : playerPos.x) * TILE_SIZE;
            const float effectY = (enemy.attackRange > 1 ? enemy.y : playerPos.y) * TILE_SIZE;
            addCombatEffect(enemy.attackRange > 1 ? "arrow" : "swing", effectX, effectY, EFFECT_DURATION_SHORT);
            
            if (uiManager) {
                uiManager->addFloatingText("-" + std::to_string(damage), 
                    playerPos.x * TILE_SIZE, playerPos.y * TILE_SIZE, sf::Color(255, 50, 50));
                
                if (player->getHealth() <= 0) {
                    uiManager->addFloatingText("DEFEATED!", playerPos.x * TILE_SIZE, playerPos.y * TILE_SIZE, sf::Color::Red);
                    setState(GameState::GameOver);
                    return;
                }
            }
            continue;  // Attack instead of move
        }
        
        // Move via BFS pathfinding
        auto [nextX, nextY] = dungeon->findNextMoveToPlayer(enemy.x, enemy.y, playerPos.x, playerPos.y);
        
        // Skip if moving into player or no change
        if ((nextX == playerPos.x && nextY == playerPos.y) || 
            (nextX == enemy.x && nextY == enemy.y)) {
            continue;
        }
        
        // Check if target tile is occupied by another enemy
        bool occupied = std::any_of(enemies.begin(), enemies.end(), 
            [&](const EnemyData& other) { return &other != &enemy && other.x == nextX && other.y == nextY; });
        
        if (!occupied) {
            enemy.x = nextX;
            enemy.y = nextY;
        }
    }
    
    // Update room clearance status
    const auto& rooms = dungeon->getRooms();
    for (size_t i = 0; i < rooms.size(); ++i) {
        int count = std::count_if(enemies.begin(), enemies.end(), 
            [&](const EnemyData& e) { return dungeon->getRoomIdAt(e.x, e.y) == static_cast<int>(i); });
        dungeon->checkRoomClear(i, count);
    }
}


void Game::render() {
    renderer->begin();
    
    const bool inGameState = (currentState == GameState::Playing || 
                              currentState == GameState::Inventory || 
                              currentState == GameState::SkillTree ||
                              currentState == GameState::GameOver);
    
    if (inGameState) {
        // Layer 1: Dungeon tiles
        if (dungeon) {
            renderer->renderDungeon(*dungeon, currentFloor);
        }
        
        // Layer 2: DSA graph visualization
        if (uiManager && dungeon) {
            uiManager->renderGraphPathsOnly(window, *dungeon);
        }
        
        // Layer 3: Entities
        if (player) renderer->renderPlayer(*player);
        if (enemyManager) renderer->renderEnemies(*enemyManager);
        
        // Layer 4: Loot items
        auto& assets = AssetManager::getInstance();
        const Position playerPos = player ? player->getPosition() : Position{-100, -100};
        
        for (const auto& loot : loots) {
            sf::Texture* iconTex = assets.getTexture(loot.getItem().id);
            loot.render(window, TILE_SIZE, iconTex);
            
            const ItemNew& item = loot.getItem();
            const float lootX = loot.getX() * TILE_SIZE;
            const float lootY = loot.getY() * TILE_SIZE;
            
            // Premium loot glow with pulsing effect
            if (item.rarity >= 3 || item.value >= 100) {
                float pulse = 2.f + std::sin(totalTime * 4.f) * 1.5f;
                sf::RectangleShape glow({TILE_SIZE, TILE_SIZE});
                glow.setPosition({lootX, lootY});
                glow.setFillColor(sf::Color::Transparent);
                glow.setOutlineThickness(pulse);
                glow.setOutlineColor(item.getRarityColor());
                window.draw(glow);
            }
            
            // Item name label when adjacent
            if (loot.isAdjacentTo(playerPos.x, playerPos.y) && uiManager && uiManager->isFontLoaded()) {
                const auto& font = uiManager->getFont();
                sf::Text label(font, item.name, 14);
                label.setPosition({lootX, lootY - 20.f});
                label.setFillColor(item.getRarityColor());
                label.setOutlineThickness(2.f);
                label.setOutlineColor(sf::Color::Black);
                window.draw(label);
            }
        }
        
        // Pickup prompt (first adjacent loot only)
        if (uiManager && player) {
            for (const auto& loot : loots) {
                if (loot.isAdjacentTo(playerPos.x, playerPos.y)) {
                    uiManager->renderContextualPrompt(window, "Press E to pick up " + loot.getItem().name);
                    break;
                }
            }
        }
        
        // Debug: Bounding boxes
        if (debugShowBoundingBoxes) {
            auto drawBox = [&](float x, float y, sf::Color color) {
                sf::RectangleShape box({TILE_SIZE, TILE_SIZE});
                box.setPosition({x * TILE_SIZE, y * TILE_SIZE});
                box.setFillColor(sf::Color::Transparent);
                box.setOutlineThickness(2.f);
                box.setOutlineColor(color);
                window.draw(box);
            };
            
            if (player) drawBox(static_cast<float>(playerPos.x), static_cast<float>(playerPos.y), sf::Color::Green);
            if (enemyManager) {
                for (const auto& e : enemyManager->getEnemies()) {
                    drawBox(static_cast<float>(e.x), static_cast<float>(e.y), sf::Color::Red);
                }
            }
            for (const auto& loot : loots) {
                drawBox(static_cast<float>(loot.getX()), static_cast<float>(loot.getY()), sf::Color::Yellow);
            }
        }
        
        // Layer 5: Lighting
        if (renderer->isLightingEnabled()) {
            renderer->clearLights();
            
            // Player torch
            if (player) {
                Renderer::Light torch(
                    {playerPos.x * TILE_SIZE + TILE_SIZE / 2.f, playerPos.y * TILE_SIZE + TILE_SIZE / 2.f},
                    sf::Color(255, 200, 150), 300.f, 1.f
                );
                torch.flickerSpeed = 5.f;
                torch.flickerRange = 0.1f;
                renderer->addLight(torch);
            }
            
            // Enemy glow
            if (enemyManager) {
                for (const auto& enemy : enemyManager->getEnemies()) {
                    sf::Color color = sf::Color(255, 50, 50);
                    float radius = 100.f, intensity = 0.6f;
                    
                    if (enemy.type == "ranged") { color = sf::Color(100, 50, 255); radius = 120.f; }
                    else if (enemy.type == "boss") { color = sf::Color(255, 100, 0); radius = 180.f; intensity = 0.8f; }
                    
                    renderer->addLight(Renderer::Light(
                        {enemy.x * TILE_SIZE + TILE_SIZE / 2.f, enemy.y * TILE_SIZE + TILE_SIZE / 2.f},
                        color, radius, intensity
                    ));
                }
            }
            
            // Rare loot glow
            for (const auto& loot : loots) {
                if (loot.getItem().rarity >= 3) {
                    renderer->addLight(Renderer::Light(
                        {loot.getX() * TILE_SIZE + TILE_SIZE / 2.f, loot.getY() * TILE_SIZE + TILE_SIZE / 2.f},
                        loot.getItem().getRarityColor(), 80.f, 0.5f
                    ));
                }
            }
            
            renderer->updateLights(totalTime);
            if (player) renderer->applyLighting(*player);
        }
        
        // Layer 6: Combat effects
        renderCombatEffects();
        
        // Layer 7: UI
        if (uiManager && dungeon && player && enemyManager) {
            uiManager->renderUI(window, *player, *dungeon, *enemyManager, skillTree.get(), currentFloor);
            
            // Stairs prompt
            if (dungeon->isAdjacentToStairs(playerPos.x, playerPos.y)) {
                const char* msg = enemyManager->isEmpty() 
                    ? "Press E to descend to next floor" 
                    : "Defeat all enemies to unlock stairs!";
                uiManager->renderContextualPrompt(window, msg);
            }
        }
        
        // Game over overlay
        if (currentState == GameState::GameOver) {
            renderGameOverScreen();
        }
    }
    
    // Shop overlay (top layer)
    if (shop && shop->isShopOpen()) {
        shop->render(window);
    }
    
    renderer->end();
}

void Game::renderGameOverScreen() {
    const auto winSize = window.getSize();
    const float centerX = winSize.x / 2.f;
    const float centerY = winSize.y / 2.f;
    
    // Dark overlay
    sf::RectangleShape overlay({static_cast<float>(winSize.x), static_cast<float>(winSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);
    
    // Panel
    constexpr float panelW = 400.f, panelH = 300.f;
    sf::RectangleShape panel({panelW, panelH});
    panel.setPosition({centerX - panelW / 2.f, centerY - panelH / 2.f});
    panel.setFillColor(sf::Color(80, 60, 50, 220));
    panel.setOutlineColor(sf::Color(120, 90, 70));
    panel.setOutlineThickness(5.f);
    window.draw(panel);
    
    // Use UIManager font if available
    if (!uiManager || !uiManager->isFontLoaded()) return;
    const auto& font = uiManager->getFont();
    
    // Title
    sf::Text title(font, "GAME OVER", 60);
    title.setFillColor(sf::Color(200, 50, 50));
    title.setOutlineThickness(3.f);
    title.setOutlineColor(sf::Color::Black);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({centerX - titleBounds.size.x / 2.f, centerY - 100.f});
    window.draw(title);
    
    // Stats
    if (player) {
        std::string stats = "Level: " + std::to_string(player->getLevel()) + 
                           "  |  XP: " + std::to_string(player->getExperience()) +
                           "  |  Floor: " + std::to_string(currentFloor);
        sf::Text statsText(font, stats, 22);
        statsText.setFillColor(sf::Color(220, 220, 220));
        sf::FloatRect statsBounds = statsText.getLocalBounds();
        statsText.setPosition({centerX - statsBounds.size.x / 2.f, centerY});
        window.draw(statsText);
    }
    
    // Instructions
    sf::Text hint(font, "Press ESC to exit  |  R to restart", 18);
    hint.setFillColor(sf::Color(150, 150, 150));
    sf::FloatRect hintBounds = hint.getLocalBounds();
    hint.setPosition({centerX - hintBounds.size.x / 2.f, centerY + 100.f});
    window.draw(hint);
}

void Game::setState(GameState state) {
    currentState = state;
    if (state == GameState::Playing && uiManager) {
        uiManager->showHUD();
    }
}

void Game::checkExitAccess() {
    if (!enemyManager || !enemyManager->isEmpty() || !dungeon || !uiManager) return;
    
    const auto& rooms = dungeon->getRooms();
    if (rooms.empty()) return;
    
    // Place exit at center of last room
    const auto& lastRoom = rooms.back();
    exitStairsPosition.x = std::clamp(lastRoom.x + lastRoom.width / 2, 0, dungeon->getWidth() - 1);
    exitStairsPosition.y = std::clamp(lastRoom.y + lastRoom.height / 2, 0, dungeon->getHeight() - 1);
    
    dungeon->setTile(exitStairsPosition.x, exitStairsPosition.y, TileType::Exit);
    uiManager->addFloatingText("Exit Unlocked!", exitStairsPosition.x * TILE_SIZE, exitStairsPosition.y * TILE_SIZE, sf::Color(255, 215, 0));
}

void Game::spawnEnemiesForFloor(int floor) {
    if (!enemyManager || !dungeon || !player) return;
    
    const auto& rooms = dungeon->getRooms();
    if (rooms.size() < 2) return;
    
    const float scale = 1.f + (floor - 1) * 0.25f;
    const Position playerPos = player->getPosition();
    
    // Enemy definitions by floor tier
    struct EnemyDef { const char* name; const char* type; int hp; int dmg; int range; };
    static const EnemyDef tier1[] = {{"Goblin", "melee", 50, 10, 1}, {"Orc", "melee", 80, 15, 1}, {"Skeleton", "ranged", 60, 12, 2}};
    static const EnemyDef tier2[] = {{"Wraith", "ranged", 100, 20, 2}, {"Dark Mage", "ranged", 90, 25, 3}, {"Gargoyle", "melee", 120, 18, 1}};
    static const EnemyDef tier3[] = {{"Vampire", "melee", 150, 30, 1}, {"Lich", "boss", 200, 40, 3}, {"Dragon", "boss", 300, 50, 2}};
    
    const EnemyDef* enemies = (floor <= 3) ? tier1 : (floor <= 6) ? tier2 : tier3;
    constexpr int enemyCount = 3;
    
    int spawned = 0;
    for (size_t i = 1; i < rooms.size() && spawned < enemyCount; i++) {
        const auto& room = rooms[i];
        int cx = room.x + room.width / 2;
        int cy = room.y + room.height / 2;
        
        // Ensure min 3 tiles from player
        if (std::abs(cx - playerPos.x) + std::abs(cy - playerPos.y) < 3) {
            cx = room.x + 1;
            cy = room.y + 1;
        }
        
        const auto& e = enemies[spawned];
        enemyManager->spawnEnemy(e.name, e.type, cx, cy, 
            static_cast<int>(e.hp * scale), static_cast<int>(e.dmg * scale), e.range, 1.f, floor);
        spawned++;
    }
}

void Game::nextFloor() {
    if (!player || !dungeon || !enemyManager || !levelManager || !uiManager) return;
    
    loots.clear();
    activeEffects.clear();
    currentFloor++;
    
    // Victory condition
    if (currentFloor > 10) {
        Position pos = player->getPosition();
        uiManager->addFloatingText("VICTORY!", pos.x * TILE_SIZE, pos.y * TILE_SIZE, sf::Color(255, 215, 0));
        currentState = GameState::Victory;
        return;
    }
    
    levelManager->advanceFloor();
    
    // Boss floor warning
    if (levelManager->isBossFloor(currentFloor)) {
        Position pos = player->getPosition();
        uiManager->addFloatingText("BOSS FLOOR!", pos.x * TILE_SIZE, pos.y * TILE_SIZE - 50.f, sf::Color::Red);
    }
    
    levelManager->generateLevel(currentFloor, *dungeon, *enemyManager, *player);
    
    // Place player in first room
    const auto& rooms = dungeon->getRooms();
    if (!rooms.empty()) {
        player->moveTo({rooms[0].x + 1, rooms[0].y + 1});
    }
    
    exitStairsPosition = {0, 0};
    Position pos = player->getPosition();
    uiManager->addFloatingText("Floor " + std::to_string(currentFloor), pos.x * TILE_SIZE, pos.y * TILE_SIZE, sf::Color::White);
}

void Game::dropItemFromEnemy(const std::string& enemyName, int x, int y) {
    if (!uiManager) return;
    
    // Boss detection
    static const char* bossNames[] = {"Dragon", "Knight", "Lich", "Necromancer", "Boss"};
    bool isBoss = std::any_of(std::begin(bossNames), std::end(bossNames), 
        [&](const char* name) { return enemyName.find(name) != std::string::npos; });
    
    // Drop chance: 100% for bosses, 50% for regular
    if (!isBoss && (rand() % 100) >= 50) return;
    
    // Floor-based loot tables
    struct LootEntry { const char* id; int weight; };
    static const LootEntry loot1[] = {{"potion", 40}, {"gem_ruby", 60}, {"sword_iron", 80}, {"shield", 100}};
    static const LootEntry loot2[] = {{"potion_mega", 30}, {"potion_strength", 50}, {"sword_flame", 70}, {"cloak", 85}, {"frost_bomb", 100}};
    static const LootEntry loot3[] = {{"elixir", 25}, {"fire_scroll", 45}, {"holy_water", 65}, {"amulet_wisdom", 80}, {"lightning_rod", 100}};
    static const LootEntry loot4[] = {{"elixir", 20}, {"sword_legendary", 40}, {"armor_dragon", 60}, {"amulet_health", 80}, {"gem_ruby", 100}};
    
    const LootEntry* table = (currentFloor <= 2) ? loot1 : (currentFloor <= 4) ? loot2 : (currentFloor <= 7) ? loot3 : loot4;
    constexpr int tableSize = 4;
    
    // Roll for item
    auto& itemMgr = ItemManager::getInstance();
    int roll = rand() % 100;
    ItemNew item;
    
    for (int i = 0; i < tableSize; ++i) {
        if (roll < table[i].weight) {
            item = itemMgr.getItemById(table[i].id);
            break;
        }
    }
    
    // Fallback
    if (item.id.empty()) {
        item = itemMgr.getItemById("potion");
        if (item.id.empty()) {
            item = ItemNew("potion", "Health Potion", "consumable", 1, 50,
                ItemAction("heal", {{{"amount", 50}}}), "assets/kenney/tiles_packed.png");
        }
    }
    
    // Stack for consumables
    if (item.type == "consumable") {
        item.stackCount = 1 + (rand() % 3);
    }
    
    spawnLootAt(sf::Vector2i(x, y), item);
    
    // Floating text
    std::string text = (item.stackCount > 1) 
        ? "+ " + item.name + " x" + std::to_string(item.stackCount)
        : "+ " + item.name;
    uiManager->addFloatingText(text, x * TILE_SIZE, y * TILE_SIZE - 20.f, item.getRarityColor());
}

// ═══════════════════════════════════════════════════════════════════════
// ✨ COMBAT EFFECT SYSTEM - Visual feedback for attacks and skills
// ═══════════════════════════════════════════════════════════════════════

void Game::addCombatEffect(const std::string& effectType, float x, float y, float duration) {
    activeEffects.emplace_back(effectType, x, y, duration);
}

void Game::updateCombatEffects(float deltaTime) {
    // Efficient removal of expired effects using erase-remove idiom
    activeEffects.erase(
        std::remove_if(activeEffects.begin(), activeEffects.end(),
            [deltaTime](CombatEffect& effect) {
                effect.lifetime -= deltaTime;
                return effect.lifetime <= 0.f;
            }),
        activeEffects.end()
    );
}

void Game::renderCombatEffects() {
    if (activeEffects.empty()) return;
    
    // Static texture mapping (loaded once)
    static const std::unordered_map<std::string, std::string> effectTextures = {
        {"swing", "effect_attack_swing"},
        {"large_swing", "effect_attack_large"},
        {"explosion", "effect_explosion"},
        {"fire_explosion", "effect_fire_explosion"},
        {"magic_explosion", "effect_magic_explosion"},
        {"arrow", "effect_arrow"},
        {"acid", "effect_acid"},
        {"ghost_orb", "effect_ghost_orb"},
        {"heal", "effect_heal"}
    };
    
    auto& assets = AssetManager::getInstance();
    
    for (const auto& effect : activeEffects) {
        const auto it = effectTextures.find(effect.effectType);
        if (it == effectTextures.end()) continue;
        
        sf::Texture* texture = assets.getTexture(it->second);
        if (!texture) continue;
        
        // Create sprite with fade based on lifetime
        sf::Sprite sprite(*texture);
        sprite.setPosition({effect.x, effect.y});
        
        const float lifetimeRatio = effect.lifetime / effect.maxLifetime;
        const uint8_t alpha = static_cast<uint8_t>(lifetimeRatio * 255.f);
        sprite.setColor(sf::Color(255, 255, 255, alpha));
        
        // Scale to uniform TILE_SIZE with slight expansion on spawn
        const float baseScale = TILE_SIZE / static_cast<float>(texture->getSize().x);
        const float dynamicScale = baseScale * (1.f + (1.f - lifetimeRatio) * 0.2f);
        sprite.setScale({dynamicScale, dynamicScale});
        
        window.draw(sprite);
    }
}

// ═══════════════════════════════════════════════════════════════════════
// LOOT SPAWNING AND MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════

static constexpr int MAX_LOOTS = 100;

void Game::spawnLootAt(const sf::Vector2i& tilePos, const ItemNew& item) {
    // Enforce maximum loot limit (FIFO eviction)
    if (loots.size() >= MAX_LOOTS) {
        loots.erase(loots.begin());
    }
    
    loots.emplace_back(item, tilePos);
    
    // Visual highlight for rare/valuable items
    if (item.rarity >= 3 || item.value >= 100) {
        if (uiManager) {
            uiManager->addFloatingText(item.name + "!", 
                tilePos.x * TILE_SIZE, tilePos.y * TILE_SIZE, 
                sf::Color(255, 215, 0));  // Gold text for premium loot
        }
    }
}

void Game::updateLoots(float deltaTime) {
    for (auto& loot : loots) {
        loot.update(deltaTime);
    }
}

// ═══════════════════════════════════════════════════════════════════════
// DYNAMIC GAME SYSTEMS - Screen shake, combos, camera, FPS
// ═══════════════════════════════════════════════════════════════════════

void Game::applyScreenShake(float intensity, float duration) {
    // Stack with existing shake if stronger
    screenShakeIntensity = std::max(screenShakeIntensity, intensity);
    screenShakeTimer = std::max(screenShakeTimer, duration);
}

void Game::updateScreenShake(float deltaTime) {
    if (screenShakeTimer <= 0.f) return;
    
    screenShakeTimer -= deltaTime;
    screenShakeIntensity *= (1.f - SCREEN_SHAKE_DECAY * deltaTime);
    
    if (screenShakeTimer <= 0.f) {
        screenShakeIntensity = 0.f;
        screenShakeTimer = 0.f;
    }
}

void Game::updateCamera(float deltaTime) {
    if (!player) return;
    
    // Camera target: center on player
    const Position playerPos = player->getPosition();
    cameraTarget.x = playerPos.x * TILE_SIZE - WINDOW_WIDTH / 2.f + TILE_SIZE / 2.f;
    cameraTarget.y = playerPos.y * TILE_SIZE - WINDOW_HEIGHT / 2.f + TILE_SIZE / 2.f;
    
    // Exponential smoothing (frame-rate independent)
    const float lerpFactor = 1.f - std::exp(-CAMERA_LERP_SPEED * deltaTime);
    cameraOffset.x += (cameraTarget.x - cameraOffset.x) * lerpFactor;
    cameraOffset.y += (cameraTarget.y - cameraOffset.y) * lerpFactor;
    
    // Screen shake offset (random direction)
    if (screenShakeIntensity > 0.1f) {
        const float randX = (std::rand() % 200 - 100) / 100.f;  // -1.0 to 1.0
        const float randY = (std::rand() % 200 - 100) / 100.f;
        cameraOffset.x += randX * screenShakeIntensity;
        cameraOffset.y += randY * screenShakeIntensity;
    }
}

void Game::incrementCombo() {
    comboCounter++;
    comboTimer = COMBO_TIMEOUT;
    
    // Notify UI of combo update
    if (uiManager) {
        uiManager->setCombo(comboCounter);
    }
    
    // Screen shake scales with combo (threshold: 5+)
    if (comboCounter >= 5) {
        applyScreenShake(2.f + comboCounter * 0.4f, 0.12f);
    }
}

void Game::resetCombo() {
    comboCounter = 0;
    comboTimer = 0.f;
}

void Game::setTargetFPS(float fps) {
    targetFPS = std::clamp(fps, static_cast<float>(FRAMERATE_MIN), static_cast<float>(FRAMERATE_MAX));
    window.setFramerateLimit(static_cast<unsigned int>(targetFPS));
}
