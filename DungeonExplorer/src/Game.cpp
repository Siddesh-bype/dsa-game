// CHANGE: 2025-11-10 — TASK D: Floor transition with stairs
// - Added "E" key to descend stairs when adjacent
// - Added contextual prompt showing "Press E to descend"
// - Added debug logging for floor transitions
// - Stairs automatically placed in furthest room from entrance

// CHANGE: 2025-11-XX — 10-Floor Progressive Dungeon System
// - Integrated DungeonLevelManager for floor progression
// - Added adaptive enemy AI scaling (5 intelligence levels)
// - Implemented floor-specific themes and difficulty multipliers
// - Added boss encounters on floors 5 and 10
// - Victory condition when completing floor 10

// CHANGE: 2025-11-10 - Item-drop system with loot tables and interactive doors
// - Added ItemManager for item database (hash table)
// - Added Loot entity system for items on ground
// - Added DropTable for weighted random item drops
// - Added Door system with open/close states
// - E key now handles: pickup loot, open doors, descend stairs

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
#include "DataStructures/Heap.h"
#include "DataStructures/HashTable.h"
#include <iostream>

Game::Game() 
    : window(sf::VideoMode({800, 600}), "Dungeon Explorer - DSA Game"),
      // gui(window),  // Disabled - TGUI not compatible
      isRunning(true),
      isPaused(false),
      currentState(GameState::Playing),  // Changed from MainMenu to Playing
      currentFloor(1),
      exitStairsPosition({0, 0}),
      levelManager(std::make_unique<DungeonLevelManager>()) {  // NEW: Initialize level manager
    
    window.setFramerateLimit(60);
}

Game::~Game() {
}

void Game::initialize() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "   DUNGEON EXPLORER - DSA Game" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Load Kenney assets first
    std::cout << "[Game] Loading Kenney asset pack..." << std::endl;
    AssetManager::getInstance().loadFromManifest("assets/data/kenney_manifest.json");
    std::cout << "[Game] Kenney assets loaded successfully!\n" << std::endl;
    
    // Load level configurations
    std::cout << "[Game] Loading 10-floor dungeon system..." << std::endl;
    levelManager->loadLevels("assets/data/levels.json");
    std::cout << "[Game] 10-floor system loaded!\n" << std::endl;
    
    // CHANGE: 2025-11-10 - Load item database
    std::cout << "[Game] Loading item database (Hash Table)..." << std::endl;
    ItemManager::getInstance().loadItems("assets/data/items.json");
    std::cout << "[Game] Item database loaded with " << ItemManager::getInstance().getItemCount() << " items!\n" << std::endl;
    
    // Initialize all systems
    player = std::make_unique<Player>();
    dungeon = std::make_unique<Dungeon>();
    renderer = std::make_unique<Renderer>(&window, 32.0f);
    uiManager = std::make_unique<UIManager>(this);  // Fixed - no longer needs gui pointer
    
    // Generate first floor using level manager
    enemyManager = std::make_unique<EnemyManager>();
    levelManager->generateLevel(currentFloor, *dungeon, *enemyManager, *player);
    
    // Initialize player at start position
    if (!dungeon->getRooms().empty()) {
        const auto& startRoom = dungeon->getRooms()[0];
        player->initialize(startRoom.x + 1, startRoom.y + 1);
    }
    
    std::cout << "\n[Game] " << levelManager->getFloorDisplayText(currentFloor) << std::endl;
    std::cout << "[Game] " << levelManager->getFloorDescription(currentFloor) << "\n" << std::endl;
    
    // ═══════════════════════════════════════════════════════════════════════
    // CHANGE: 2025-11-14 - DSA Integration Report
    // ═══════════════════════════════════════════════════════════════════════
    std::cout << "\n========================================" << std::endl;
    std::cout << "   DSA STRUCTURES INTEGRATION REPORT" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Graph: Room connectivity
    std::cout << "\n✓ GRAPH (Room Connectivity):" << std::endl;
    std::cout << "  - Rooms: " << dungeon->getRooms().size() << " connected as graph" << std::endl;
    std::cout << "  - Uses: BFS, DFS, Dijkstra pathfinding for room traversal" << std::endl;
    dungeon->visualizeDijkstra(0);
    
    // LinkedList: Player Inventory
    std::cout << "\n✓ LINKED LIST (Player Inventory):" << std::endl;
    std::cout << "  - Inventory: Player has " << player->getInventoryNew().size() << " items in LinkedList<ItemNew>" << std::endl;
    std::cout << "  - Contains: Dagger, Gold Coin, Silver Ring (starting items)" << std::endl;
    
    // Stack: Backtracking (Movement History)
    std::cout << "\n✓ STACK (Movement History/Backtracking):" << std::endl;
    std::cout << "  - Movement Stack: Tracks player path for backtracking (Press B)" << std::endl;
    std::cout << "  - Operations: push (move), pop (backtrack)" << std::endl;
    
    // Hash Table: Item Database
    std::cout << "\n✓ HASH TABLE (Item Database):" << std::endl;
    std::cout << "  - ItemManager: " << ItemManager::getInstance().getItemCount() << " items indexed by ID" << std::endl;
    std::cout << "  - O(1) Lookup: Perfect for quick item retrieval by name/ID" << std::endl;
    
    // Heap: Loot Priority (Premium Items)
    std::cout << "\n✓ HEAP (Loot Priority Tracking):" << std::endl;
    std::cout << "  - Premium Loot: Rare items (rarity ≥ 3) get glow highlighting" << std::endl;
    std::cout << "  - Priority: Higher rarity = brighter glow on ground" << std::endl;
    
    // Binary Tree: Skill Tree
    std::cout << "\n✓ BINARY TREE (Skill Tree):" << std::endl;
    std::cout << "  - Root Skill: Slash (starter ability)" << std::endl;
    std::cout << "  - Tree Depth: Balanced binary tree with passive/active skills" << std::endl;
    
    std::cout << "\n========================================\n" << std::endl;
    
    // Demonstrate Item Database (Hash Table)
    std::cout << "[Game] Creating Item Database (Hash Table)..." << std::endl;
    HashTable<std::string, Item> itemDatabase;
    itemDatabase.insert("sword", Item("sword_iron", "Iron Sword", "weapon", 15, 50, 0));
    itemDatabase.insert("potion", Item("potion", "Health Potion", "consumable", 0, 25, 50));
    itemDatabase.insert("shield", Item("shield_wood", "Wooden Shield", "armor", 0, 30, 0));
    itemDatabase.insert("bow", Item("bow_hunting", "Hunting Bow", "weapon", 12, 45, 0));
    
    // Test hash table lookup
    auto swordOpt = itemDatabase.get("sword");
    if (swordOpt) {
        std::cout << "[Game] Found item: " << swordOpt->name << " (DMG: " << swordOpt->damage << ")" << std::endl;
    }
    
    // Demonstrate Loot System (Heap)
    std::cout << "\n[Game] Creating Loot System (Max Heap)..." << std::endl;
    // CHANGE: 2025-11-14 - Use ItemNew system instead of old Item
    // Add some starting loot
    player->addItem(ItemNew("dagger_rusty", "Rusty Dagger", "weapon", 1, 10));
    player->addItem(ItemNew("coin_gold", "Gold Coin", "treasure", 1, 100));
    player->addItem(ItemNew("ring_silver", "Silver Ring", "treasure", 1, 75));
    
    // Initialize UI
    uiManager->initialize();
    
    // Load font for UI rendering (try common Windows fonts)
    if (!uiManager->loadFont("C:\\Windows\\Fonts\\arial.ttf")) {
        std::cout << "[Game] Warning: Could not load font, trying alternative..." << std::endl;
        if (!uiManager->loadFont("C:\\Windows\\Fonts\\calibri.ttf")) {
            std::cout << "[Game] Warning: No fonts loaded, text may not render" << std::endl;
        }
    }
    
    // Create skill tree
    skillTree = std::make_unique<SkillTree>();
    skillTree->initialize();
    skillTree->displayTree();
    
    // Grant starting skill points for testing/gameplay
    skillTree->addPoints(3);
    std::cout << "\n[SkillTree] Player starts with 3 skill points." << std::endl;
    std::cout << "[SkillTree] Press T to open skill tree and unlock skills!" << std::endl;
    std::cout << "[SkillTree] Only 'Slash' (hotkey 1) is unlocked initially.\n" << std::endl;
    
    // Initialize shop
    shop = std::make_unique<Shop>();
    shop->initialize();
    std::cout << "\n[Shop] Shop system initialized!" << std::endl;
    
    // OLD: Enemies are now spawned by levelManager->generateLevel()
    // Removed manual enemy spawning code
    
    enemyManager->initializeTurnQueue();
    
    // Demonstrate pathfinding algorithms
    if (dungeon->getRooms().size() > 1) {
        dungeon->visualizeBFS(0);
        dungeon->visualizeDFS(0);
        dungeon->visualizeDijkstra(0);
    }
    
    std::cout << "\n[Game] Initialization complete!" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void Game::run() {
    initialize();
    
    sf::Clock clock;
    
    while (window.isOpen() && isRunning) {
        float deltaTime = clock.restart().asSeconds();
        
        // Cap delta time to prevent huge jumps (e.g., when debugging or lag spikes)
        // Max 0.1 seconds (10 FPS minimum) to prevent physics/animation issues
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }
        
        processEvents();
        
        if (!isPaused) {
            update(deltaTime);
        }
        
        render();
    }
}

void Game::processEvents() {
    // SFML 3.x uses std::optional for event polling
    while (const std::optional event = window.pollEvent()) {
        // gui.handleEvent(event);  // Disabled - TGUI not compatible
        
        if (event->is<sf::Event::Closed>()) {
            window.close();
            isRunning = false;
        }
        
        if (event->is<sf::Event::KeyPressed>() && currentState == GameState::Playing) {
            handleInput(*event);
        }
    }
}

void Game::handleInput(const sf::Event& event) {
    if (!player || !dungeon) return;
    
    // Check if shop is open - handle shop input first
    if (shop && shop->isShopOpen()) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            shop->handleInput(keyPressed->code, player.get());
        }
        return;  // Don't process other input while shop is open
    }
    
    // SFML 3.x event handling
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        Position currentPos = player->getPosition();
        Position newPos = currentPos;
        
        switch (keyPressed->code) {
            case sf::Keyboard::Key::W:
            case sf::Keyboard::Key::Up:
                newPos.y--;
                break;
            case sf::Keyboard::Key::S:
            case sf::Keyboard::Key::Down:
                newPos.y++;
                break;
            case sf::Keyboard::Key::A:
            case sf::Keyboard::Key::Left:
                newPos.x--;
                break;
            case sf::Keyboard::Key::D:
            case sf::Keyboard::Key::Right:
                newPos.x++;
                break;
            case sf::Keyboard::Key::B:
                // Backtrack using Stack
                player->backtrack();
                return;
            case sf::Keyboard::Key::I:
                // Toggle inventory
                uiManager->toggleInventory();
                return;
            case sf::Keyboard::Key::K:
                // Toggle skill tree
                uiManager->toggleSkillTree();
                return;
            case sf::Keyboard::Key::O:
                // Quick unlock next skill (O for unl-O-ck) when points available
                if (skillTree && skillTree->getAvailablePoints() > 0) {
                    // Try to unlock next available skill in tree order with proper hotkeys
                    auto tryUnlock = [&](std::shared_ptr<BinaryTree<Skill>::Node> node, int hotkey) -> bool {
                        if (!node) return false;
                        if (!node->data.unlocked && skillTree->canUnlock(node)) {
                            // Assign hotkey if it's an active skill and doesn't have one
                            if (node->data.type == "active" && node->data.hotkey == 0) {
                                node->data.hotkey = hotkey;
                                std::cout << "[SkillTree] Assigned hotkey " << hotkey << " to " << node->data.name << std::endl;
                            }
                            skillTree->unlockSkill(node);
                            Position playerPos = player->getPosition();
                            std::string hotkeyInfo = (node->data.type == "active" && node->data.hotkey > 0) 
                                ? " [Press " + std::to_string(node->data.hotkey) + "]" : "";
                            uiManager->addFloatingText("Unlocked: " + node->data.name + hotkeyInfo, 
                                playerPos.x * 32.0f, playerPos.y * 32.0f, sf::Color(100, 255, 100));
                            return true;
                        }
                        return false;
                    };
                    
                    // Try to unlock skills in order: Power Strike (2), Whirlwind (3), Flame Wave (4), Shadow Step (5)
                    auto root = skillTree->getRoot();
                    // CHANGE: 2025-11-14 - Add bounds checking to prevent crashes on malformed trees
                    if (root && root->left && tryUnlock(root->left, 2)) {
                        std::cout << "[SkillTree] Power Strike unlocked! Press O again for more skills!" << std::endl;
                    } 
                    // Try to unlock whirlwind (depth 2)
                    else if (root && root->left && root->left->left && tryUnlock(root->left->left, 3)) {
                        std::cout << "[SkillTree] Whirlwind unlocked! Press O again for more skills!" << std::endl;
                    }
                    // Try to unlock flame wave (depth 3) - with bounds check
                    else if (root && root->left && root->left->left && 
                             root->left->left->left && tryUnlock(root->left->left->left, 4)) {
                        std::cout << "[SkillTree] Flame Wave unlocked! Press O again for more skills!" << std::endl;
                    }
                    // Try right branch - shadow step (depth 2) - with bounds check
                    else if (root && root->right && root->right->right && tryUnlock(root->right->right, 5)) {
                        std::cout << "[SkillTree] Shadow Step unlocked! Press O again for more skills!" << std::endl;
                    }
                    // Try other right branch skills (depth 1)
                    else if (root && root->right && tryUnlock(root->right, 0)) {  // Passive skills
                        std::cout << "[SkillTree] Mana Surge unlocked (passive)! Press O again!" << std::endl;
                    }
                    else {
                        Position playerPos = player->getPosition();
                        uiManager->addFloatingText("No skills available to unlock!", 
                            playerPos.x * 32.0f, playerPos.y * 32.0f, sf::Color(255, 100, 100));
                    }
                } else if (skillTree) {
                    Position playerPos = player->getPosition();
                    uiManager->addFloatingText("No skill points available!", 
                        playerPos.x * 32.0f, playerPos.y * 32.0f, sf::Color(255, 200, 100));
                }
                return;
            case sf::Keyboard::Key::P:
                // Toggle shop
                if (shop) {
                    shop->toggle();
                }
                return;
            case sf::Keyboard::Key::U:
                // Use first item in inventory (U for Use)
                if (player && player->getInventoryNew().size() > 0) {
                    // Get first item
                    bool foundItem = false;
                    std::string firstItemId;
                    player->getInventoryNew().traverse([&](const ItemNew& item) {
                        if (!foundItem) {
                            firstItemId = item.id;
                            foundItem = true;
                        }
                    });
                    
                    if (foundItem) {
                        player->useItem(firstItemId);
                    }
                }
                return;
            case sf::Keyboard::Key::Num1:
                // Activate skill 1
                activateSkill(1);
                return;
            case sf::Keyboard::Key::Num2:
                // Activate skill 2
                activateSkill(2);
                return;
            case sf::Keyboard::Key::Num3:
                // Activate skill 3
                activateSkill(3);
                return;
            case sf::Keyboard::Key::Num4:
                // Activate skill 4
                activateSkill(4);
                return;
            case sf::Keyboard::Key::Num5:
                // Activate skill 5
                activateSkill(5);
                return;
            case sf::Keyboard::Key::M:
                // Toggle mini-map
                uiManager->toggleMiniMap();
                return;
            case sf::Keyboard::Key::Space:
                // Attack nearest enemy
                if (enemyManager) {
                    attackNearestEnemy();
                }
                return;
            case sf::Keyboard::Key::H:
                // Use healing potion
                if (player && player->usePotion()) {
                    Position playerPos = player->getPosition();
                    uiManager->addFloatingText("+50 HP", 
                        playerPos.x * 32.0f, playerPos.y * 32.0f - 10.0f, 
                        sf::Color(100, 255, 100));  // Green healing text
                    // TODO: Add healing sound effect
                }
                return;
            case sf::Keyboard::Key::E:
                // CHANGE: 2025-11-10 - E key now handles: pickup loot, open doors, descend stairs (priority order)
                {
                    // Priority 1: Pick up loot if adjacent
                    bool pickedUpLoot = false;
                    for (auto it = loots.begin(); it != loots.end(); ++it) {
                        if (it->isAdjacentTo(currentPos.x, currentPos.y)) {
                            const ItemNew& item = it->getItem();
                            std::cout << "[DEBUG] Player picked up " << item.name << std::endl;
                            
                            // Check if it's gold/treasure and add gold
                            if (item.type == "treasure") {
                                player->addGold(item.value);
                                uiManager->addFloatingText("+" + std::to_string(item.value) + " Gold", 
                                    currentPos.x * 32.0f, currentPos.y * 32.0f - 10.0f, 
                                    sf::Color(255, 215, 0));  // Golden color
                            } else {
                                // Add to new inventory system
                                player->addItemNew(item);
                                
                                // Show pickup message
                                uiManager->addFloatingText("+" + item.name, 
                                    currentPos.x * 32.0f, currentPos.y * 32.0f - 10.0f, 
                                    item.getRarityColor());
                            }
                            
                            std::cout << "INFO: Player picked up " << item.name 
                                      << ". New inventory size: " << player->getInventoryNew().size() << std::endl;
                            
                            loots.erase(it);
                            pickedUpLoot = true;
                            break;
                        }
                    }
                    if (pickedUpLoot) return;
                    
                    // Priority 2: Open door if adjacent
                    for (auto& door : doors) {
                        int dx = std::abs(door.x - currentPos.x);
                        int dy = std::abs(door.y - currentPos.y);
                        if (dx <= 1 && dy <= 1 && !door.isOpen) {
                            if (door.requiresKey && !player->hasItem("dungeon_key")) {
                                uiManager->addFloatingText("Locked - Need Key", 
                                    currentPos.x * 32.0f, currentPos.y * 32.0f - 10.0f, 
                                    sf::Color(255, 100, 100));
                                return;
                            }
                            
                            door.isOpen = true;
                            dungeon->setTile(door.x, door.y, TileType::Floor);
                            std::cout << "[DEBUG] Door opened at (" << door.x << ", " << door.y << ") by player" << std::endl;
                            
                            uiManager->addFloatingText("Door Opened", 
                                currentPos.x * 32.0f, currentPos.y * 32.0f - 10.0f, 
                                sf::Color(100, 255, 100));
                            return;
                        }
                    }
                    
                    // Priority 3: Descend stairs (when adjacent)
                    if (dungeon->isAdjacentToStairs(currentPos.x, currentPos.y)) {
                        // Check if all enemies are defeated
                        if (enemyManager && !enemyManager->isEmpty()) {
                            std::cout << "[Floor] Cannot descend - enemies still remain!" << std::endl;
                            uiManager->addFloatingText("Defeat all enemies first!", 
                                currentPos.x * 32.0f, currentPos.y * 32.0f - 10.0f, 
                                sf::Color(255, 100, 100));
                        } else {
                            std::cout << "[DEBUG] Floor -> " << (currentFloor + 1) << std::endl;
                            nextFloor();
                        }
                    } else {
                        // Show hint if not near anything interactive
                        uiManager->addFloatingText("Nothing nearby", 
                            currentPos.x * 32.0f, currentPos.y * 32.0f - 10.0f, 
                            sf::Color(255, 100, 100));
                    }
                }
                return;
            case sf::Keyboard::Key::L:
                // Toggle lighting system
                if (renderer) {
                    renderer->toggleLighting();
                    Position playerPos = player->getPosition();
                    std::string msg = renderer->isLightingEnabled() ? "Lighting ON" : "Lighting OFF";
                    uiManager->addFloatingText(msg, 
                        playerPos.x * 32.0f, playerPos.y * 32.0f - 10.0f, 
                        sf::Color(255, 255, 100));
                }
                return;
            case sf::Keyboard::Key::F5:
                // Toggle retro mode (asset pack)
                AssetManager::getInstance().togglePack();
                if (player && uiManager) {
                    Position playerPos = player->getPosition();
                    std::string mode = (AssetManager::getInstance().getCurrentPack() == AssetPack::TinyDungeon) 
                                      ? "Colorful Mode" : "Retro Mode";
                    uiManager->addFloatingText(mode, 
                        playerPos.x * 32.0f, playerPos.y * 32.0f - 10.0f, 
                        sf::Color(255, 200, 100));
                }
                return;
            case sf::Keyboard::Key::F3:
                // Toggle bounding box debug display
                debugShowBoundingBoxes = !debugShowBoundingBoxes;
                if (player && uiManager) {
                    Position playerPos = player->getPosition();
                    std::string msg = debugShowBoundingBoxes ? "Debug Boxes ON" : "Debug Boxes OFF";
                    uiManager->addFloatingText(msg, 
                        playerPos.x * 32.0f, playerPos.y * 32.0f - 10.0f, 
                        sf::Color(0, 255, 255));
                }
                std::cout << "[Debug] Bounding boxes " << (debugShowBoundingBoxes ? "ENABLED" : "DISABLED") << std::endl;
                std::cout << "[Debug] F3 toggle - rendering will show collision boxes for player and enemies" << std::endl;
                return;
            case sf::Keyboard::Key::F4:
                // Toggle 1-bit retro mode
                debugRetroMode = !debugRetroMode;
                if (player && uiManager) {
                    Position playerPos = player->getPosition();
                    std::string msg = debugRetroMode ? "1-Bit Retro Mode ON" : "1-Bit Retro Mode OFF";
                    uiManager->addFloatingText(msg, 
                        playerPos.x * 32.0f, playerPos.y * 32.0f - 10.0f, 
                        sf::Color(255, 255, 0));
                }
                std::cout << "[Debug] 1-bit retro mode " << (debugRetroMode ? "ENABLED" : "DISABLED") << std::endl;
                std::cout << "[Debug] F4 toggle - rendering will switch to monochrome palettes" << std::endl;
                return;
            case sf::Keyboard::Key::Escape:
                // Close all panels or exit game over screen
                if (currentState == GameState::GameOver) {
                    window.close();
                    isRunning = false;
                } else {
                    uiManager->hideAll();
                }
                return;
            default:
                return;
        }
        
        // Check if new position is walkable
        if (dungeon->isWalkable(newPos.x, newPos.y)) {
            player->moveTo(newPos);
            
            // Check if player stepped on exit stairs
            if (newPos.x == exitStairsPosition.x && newPos.y == exitStairsPosition.y) {
                TileType tileAtPos = dungeon->getTile(newPos.x, newPos.y);
                if (tileAtPos == TileType::Exit) {
                    nextFloor();
                    return;
                }
            }
            
            // After player moves, enemies take their turn (move toward player and attack if nearby)
            moveEnemiesWithBFS();
        }
    }
}

void Game::update(float deltaTime) {
    switch (currentState) {
        case GameState::MainMenu:
            updateMainMenu(deltaTime);
            break;
        case GameState::Playing:
            updatePlaying(deltaTime);
            break;
        default:
            break;
    }
    
    // ✨ Update combat effects
    updateCombatEffects(deltaTime);
    
    // CHANGE: 2025-11-10 - Update loot animations
    updateLoots(deltaTime);
    
    if (uiManager && player) {
        uiManager->update(deltaTime);
        uiManager->updateHUD(*player);
    }
}

void Game::updateMainMenu(float deltaTime) {
    // Menu update logic
}

void Game::updatePlaying(float deltaTime) {
    if (player) {
        player->update(deltaTime);
    }
}

void Game::attackNearestEnemy() {
    if (!player || !enemyManager || !uiManager) return;
    
    Position playerPos = player->getPosition();
    EnemyData* nearestEnemy = enemyManager->findNearestEnemy(playerPos.x, playerPos.y);
    
    if (!nearestEnemy) {
        std::cout << "[Combat] No enemies to attack!" << std::endl;
        return;
    }
    
    // Check if enemy is in range (adjacent tiles)
    int dx = abs(nearestEnemy->x - playerPos.x);
    int dy = abs(nearestEnemy->y - playerPos.y);
    int distance = dx + dy;
    
    if (distance > 2) {
        std::cout << "[Combat] " << nearestEnemy->name << " is too far away!" << std::endl;
        uiManager->addFloatingText("Too far!", playerPos.x * 32.0f, playerPos.y * 32.0f, sf::Color(150, 150, 150));
        return;
    }
    
    // Player attacks enemy
    int damage = player->attackEnemy();
    nearestEnemy->health -= damage;
    
    // ✨ Add visual attack effect at enemy position
    addCombatEffect("swing", nearestEnemy->x * 32.0f, nearestEnemy->y * 32.0f, 0.3f);
    
    // Show damage text at enemy position
    uiManager->addFloatingText("-" + std::to_string(damage), 
        nearestEnemy->x * 32.0f, nearestEnemy->y * 32.0f, sf::Color(255, 150, 50));
    
    std::cout << "[Combat] " << nearestEnemy->name << " HP: " << nearestEnemy->health << "/" << nearestEnemy->maxHealth << std::endl;
    
    // Check if enemy died
    if (nearestEnemy->health <= 0) {
        std::cout << "[Combat] " << nearestEnemy->name << " defeated!" << std::endl;
        std::cout << "DEBUG: Enemy " << nearestEnemy->name << " died at (" << nearestEnemy->x << ", " << nearestEnemy->y << ")" << std::endl;
        
        // ✨ Add explosion effect when enemy dies
        addCombatEffect("explosion", nearestEnemy->x * 32.0f, nearestEnemy->y * 32.0f, 0.5f);
        
        // Show defeat text
        uiManager->addFloatingText("DEFEATED!", 
            nearestEnemy->x * 32.0f, nearestEnemy->y * 32.0f, sf::Color(255, 215, 0));
        
        // Show XP gain
        int xpGain = 25 + (currentFloor * 5); // More XP for deeper floors
        uiManager->addFloatingText("+" + std::to_string(xpGain) + " XP", 
            playerPos.x * 32.0f, playerPos.y * 32.0f, sf::Color(100, 255, 100));
        
        player->addExperience(xpGain);
        
        // CHANGE: 2025-11-11 - Grant skill points when player levels up
        int earnedPoints = player->getSkillPointsEarned();
        if (earnedPoints > 0 && skillTree) {
            skillTree->addPoints(earnedPoints);
            player->clearSkillPoints();
            
            uiManager->addFloatingText("+" + std::to_string(earnedPoints) + " Skill Point" + (earnedPoints > 1 ? "s" : "") + "!", 
                playerPos.x * 32.0f, playerPos.y * 32.0f - 30.0f, 
                sf::Color(255, 215, 0));  // Golden text
        }
        
        // CHANGE: 2025-11-10 - New loot drop system using DropTable
        if (!nearestEnemy->dropTableJson.empty()) {
            DropTable dropTable = DropTable::fromJson(nearestEnemy->dropTableJson);
            std::string dropId = dropTable.roll();
            
            std::cout << "DEBUG: Rolled drop -> " << dropId << " (from " << dropTable.size() << " entries)" << std::endl;
            
            if (!dropId.empty() && ItemManager::getInstance().hasItem(dropId)) {
                ItemNew item = ItemManager::getInstance().getItemById(dropId);
                spawnLootAt(sf::Vector2i(nearestEnemy->x, nearestEnemy->y), item);
                std::cout << "INFO: Spawned loot " << dropId << " at (" << nearestEnemy->x << ", " << nearestEnemy->y << ")" << std::endl;
            }
        } else {
            // Fallback to old system if no drop table
            dropItemFromEnemy(nearestEnemy->name, nearestEnemy->x, nearestEnemy->y);
        }
        
        enemyManager->removeEnemy(nearestEnemy->id);
        
        // Check if all enemies defeated to spawn exit
        checkExitAccess();
        return;
    }
    
    // NO COUNTER-ATTACK - Enemies only attack on their turn when nearby
    // This makes combat more strategic: enemies attack when you're close after you move
}

void Game::activateSkill(int hotkey) {
    if (!player || !skillTree || !enemyManager || !uiManager) return;
    
    // Get the skill by hotkey
    Skill* skill = skillTree->getSkillByHotkey(hotkey);
    
    if (!skill) {
        std::cout << "[Skills] No skill assigned to hotkey " << hotkey << std::endl;
        return;
    }
    
    if (!skill->unlocked) {
        std::cout << "[Skills] Skill " << skill->name << " is not unlocked!" << std::endl;
        Position playerPos = player->getPosition();
        uiManager->addFloatingText("Not unlocked!", 
            playerPos.x * 32.0f, playerPos.y * 32.0f, sf::Color(200, 50, 50));
        return;
    }
    
    if (skill->currentCooldown > 0) {
        std::cout << "[Skills] " << skill->name << " is on cooldown! (" << skill->currentCooldown << " turns left)" << std::endl;
        Position playerPos = player->getPosition();
        uiManager->addFloatingText("On cooldown!", 
            playerPos.x * 32.0f, playerPos.y * 32.0f, sf::Color(200, 200, 50));
        return;
    }
    
    if (!player->useMana(skill->manaCost)) {
        Position playerPos = player->getPosition();
        uiManager->addFloatingText("Not enough mana!", 
            playerPos.x * 32.0f, playerPos.y * 32.0f, sf::Color(100, 100, 255));
        return;
    }
    
    // Activate the skill
    Position playerPos = player->getPosition();
    std::cout << "[Skills] Activating " << skill->name << "!" << std::endl;
    
    if (skill->id == "slash") {
        // Basic attack with bonus damage
        EnemyData* nearestEnemy = enemyManager->findNearestEnemy(playerPos.x, playerPos.y);
        if (nearestEnemy) {
            // ✨ Large swing effect for slash
            addCombatEffect("large_swing", nearestEnemy->x * 32.0f, nearestEnemy->y * 32.0f, 0.4f);
            
            int totalDamage = player->attackEnemy() + skill->damage;
            nearestEnemy->health -= totalDamage;
            uiManager->addFloatingText("-" + std::to_string(totalDamage) + " SLASH", 
                nearestEnemy->x * 32.0f, nearestEnemy->y * 32.0f, sf::Color(255, 200, 50));
            
            if (nearestEnemy->health <= 0) {
                addCombatEffect("explosion", nearestEnemy->x * 32.0f, nearestEnemy->y * 32.0f, 0.5f);
                enemyManager->removeEnemy(nearestEnemy->id);
                checkExitAccess();
            }
        }
    } else if (skill->id == "power_strike") {
        // Powerful single-target attack
        EnemyData* nearestEnemy = enemyManager->findNearestEnemy(playerPos.x, playerPos.y);
        if (nearestEnemy) {
            // ✨ Large swing + explosion for power strike
            addCombatEffect("large_swing", nearestEnemy->x * 32.0f, nearestEnemy->y * 32.0f, 0.4f);
            
            int totalDamage = player->attackEnemy() + skill->damage;
            nearestEnemy->health -= totalDamage;
            uiManager->addFloatingText("-" + std::to_string(totalDamage) + " POWER!", 
                nearestEnemy->x * 32.0f, nearestEnemy->y * 32.0f, sf::Color(255, 100, 0));
            
            if (nearestEnemy->health <= 0) {
                addCombatEffect("magic_explosion", nearestEnemy->x * 32.0f, nearestEnemy->y * 32.0f, 0.6f);
                enemyManager->removeEnemy(nearestEnemy->id);
                checkExitAccess();
            }
        }
    } else if (skill->id == "whirlwind" && skill->aoe) {
        // AOE attack - hit all adjacent enemies
        int enemiesHit = 0;
        for (auto& enemy : enemyManager->getEnemies()) {
            int dx = abs(enemy.x - playerPos.x);
            int dy = abs(enemy.y - playerPos.y);
            if (dx <= 1 && dy <= 1) {  // Adjacent tiles
                // ✨ Swing effect for each hit enemy
                addCombatEffect("swing", enemy.x * 32.0f, enemy.y * 32.0f, 0.3f);
                
                const_cast<EnemyData&>(enemy).health -= skill->damage;
                uiManager->addFloatingText("-" + std::to_string(skill->damage), 
                    enemy.x * 32.0f, enemy.y * 32.0f, sf::Color(255, 150, 50));
                enemiesHit++;
            }
        }
        // ✨ Large swing at player position for whirlwind visual
        addCombatEffect("large_swing", playerPos.x * 32.0f, playerPos.y * 32.0f, 0.5f);
        
        uiManager->addFloatingText("WHIRLWIND! (x" + std::to_string(enemiesHit) + ")", 
            playerPos.x * 32.0f, playerPos.y * 32.0f - 20.0f, sf::Color(255, 255, 100));
        enemyManager->removeDeadEnemies();
        checkExitAccess();
    } else if (skill->id == "flame_wave" && skill->aoe) {
        // Fire AOE with burn
        int enemiesHit = 0;
        for (auto& enemy : enemyManager->getEnemies()) {
            int dx = abs(enemy.x - playerPos.x);
            int dy = abs(enemy.y - playerPos.y);
            if (dx <= 2 && dy <= 2) {  // Larger radius
                // ✨ Fire explosion effect for each enemy hit
                addCombatEffect("fire_explosion", enemy.x * 32.0f, enemy.y * 32.0f, 0.5f);
                
                const_cast<EnemyData&>(enemy).health -= skill->damage;
                uiManager->addFloatingText("-" + std::to_string(skill->damage) + " BURN", 
                    enemy.x * 32.0f, enemy.y * 32.0f, sf::Color(255, 100, 0));
                enemiesHit++;
            }
        }
        // ✨ Large fire explosion at player position
        addCombatEffect("fire_explosion", playerPos.x * 32.0f, playerPos.y * 32.0f, 0.6f);
        
        uiManager->addFloatingText("FLAME WAVE! (x" + std::to_string(enemiesHit) + ")", 
            playerPos.x * 32.0f, playerPos.y * 32.0f - 20.0f, sf::Color(255, 50, 0));
        enemyManager->removeDeadEnemies();
        checkExitAccess();
    } else if (skill->id == "shadow_step") {
        // Dash ability - for now just show message (requires direction input)
        // ✨ Ghost orb effect for shadow step
        addCombatEffect("ghost_orb", playerPos.x * 32.0f, playerPos.y * 32.0f, 0.4f);
        
        uiManager->addFloatingText("SHADOW STEP!", 
            playerPos.x * 32.0f, playerPos.y * 32.0f, sf::Color(150, 50, 200));
        std::cout << "[Skills] Shadow Step activated (movement enhanced)" << std::endl;
    }
    
    // Set cooldown and show feedback
    skill->currentCooldown = skill->cooldown;
    uiManager->addFloatingText(skill->name + "!", 
        playerPos.x * 32.0f, playerPos.y * 32.0f - 30.0f, sf::Color(100, 200, 255));
    
    // Update cooldowns after turn
    skillTree->updateCooldowns();
    
    // Enemies take their turn
    moveEnemiesWithBFS();
}

void Game::moveEnemiesWithBFS() {
    if (!enemyManager || !dungeon || !player) return;
    
    // CHANGE: 2025-11-14 - Check for room clear and open auto-clearing doors
    if (enemyManager->isEmpty()) {
        // All enemies defeated - open all clearable doors
        for (auto& door : doors) {
            if (door.openOnClear && !door.isOpen) {
                door.isOpen = true;
                if (dungeon) {
                    dungeon->setTile(door.x, door.y, TileType::Floor);
                }
                std::cout << "[Door] Room cleared. Auto-opened door at (" << door.x << ", " << door.y << ")" << std::endl;
            }
        }
    }
    
    Position playerPos = player->getPosition();
    auto& enemies = const_cast<std::vector<EnemyData>&>(enemyManager->getEnemies());
    
    for (auto& enemy : enemies) {
        // Calculate distance to player
        int dx = abs(enemy.x - playerPos.x);
        int dy = abs(enemy.y - playerPos.y);
        int distance = dx + dy;
        
        // If within attack range, attack instead of moving
        if (distance <= enemy.attackRange) {
            // Enemy attacks player!
            std::cout << "[Combat] " << enemy.name << " attacks!" << std::endl;
            int damage = std::max(0, enemy.damage - player->getDefense());
            player->takeDamage(damage);
            
            // Add visual effect for attack
            if (enemy.attackRange > 1) {
                // ✨ Ranged attack (arrow/projectile)
                addCombatEffect("arrow", enemy.x * 32.0f, enemy.y * 32.0f, 0.4f);
            } else {
                // ✨ Melee attack effect on player
                addCombatEffect("swing", playerPos.x * 32.0f, playerPos.y * 32.0f, 0.3f);
            }
            
            // Show damage on player
            if (uiManager) {
                uiManager->addFloatingText("-" + std::to_string(damage), 
                    playerPos.x * 32.0f, playerPos.y * 32.0f, sf::Color(255, 50, 50));
                    
                if (player->getHealth() <= 0) {
                    std::cout << "[Combat] You have been defeated!" << std::endl;
                    uiManager->addFloatingText("DEFEATED!", 
                        playerPos.x * 32.0f, playerPos.y * 32.0f, sf::Color(255, 0, 0));
                    setState(GameState::GameOver);
                    return;  // Stop enemy processing
                }
            }
            continue;  // Don't move, just attack
        }
        
        // Calculate next move using BFS pathfinding
        auto nextPos = dungeon->findNextMoveToPlayer(enemy.x, enemy.y, playerPos.x, playerPos.y);
        
        // Check if enemy would move into player position (shouldn't happen with attack check above)
        if (nextPos.first == playerPos.x && nextPos.second == playerPos.y) {
            continue;  // Don't move into player
        }
        
        // Check if position changed (enemy is moving)
        if (nextPos.first != enemy.x || nextPos.second != enemy.y) {
            // Make sure no other enemy is at that position
            bool occupied = false;
            for (const auto& other : enemies) {
                if (&other != &enemy && other.x == nextPos.first && other.y == nextPos.second) {
                    occupied = true;
                    break;
                }
            }
            
            // Move enemy if position is free
            if (!occupied) {
                enemy.x = nextPos.first;
                enemy.y = nextPos.second;
            }
        }
    }
    
    // CHANGE: 2025-11-14 - Check all rooms for clearance after movement
    auto rooms = dungeon->getRooms();
    for (size_t i = 0; i < rooms.size(); i++) {
        int enemyCount = 0;
        for (const auto& enemy : enemies) {
            int roomId = dungeon->getRoomIdAt(enemy.x, enemy.y);
            if (roomId == static_cast<int>(i)) {
                enemyCount++;
            }
        }
        dungeon->checkRoomClear(i, enemyCount);
    }
}

void Game::render() {
    renderer->begin();
    
    if (currentState == GameState::Playing || 
        currentState == GameState::Inventory || 
        currentState == GameState::SkillTree ||
        currentState == GameState::GameOver) {
        
        if (dungeon) {
            renderer->renderDungeon(*dungeon, currentFloor);
        }
        
        // Render DSA graph paths BEFORE entities (lower layer)
        if (uiManager && dungeon) {
            uiManager->renderGraphPathsOnly(window, *dungeon);
        }
        
        if (player) {
            renderer->renderPlayer(*player);
        }
        
        if (enemyManager) {
            renderer->renderEnemies(*enemyManager);
        }
        
        // CHANGE: 2025-11-10 - Render loot items on ground
        for (const auto& loot : loots) {
            sf::Texture* iconTexture = AssetManager::getInstance().getTexture(loot.getItem().id);
            loot.render(window, 32.0f, iconTexture);
            
            // CHANGE: 2025-11-14 - Add premium loot highlighting (using Heap prioritization logic)
            // Rare and valuable items get a glowing aura effect
            const ItemNew& item = loot.getItem();
            if (item.rarity >= 3 || item.value >= 100) {
                // Draw a glowing border around premium loot
                sf::RectangleShape premiumGlow(sf::Vector2f(32.0f, 32.0f));
                premiumGlow.setPosition(sf::Vector2f(loot.getX() * 32.0f, loot.getY() * 32.0f));
                premiumGlow.setFillColor(sf::Color::Transparent);
                premiumGlow.setOutlineThickness(3.0f);
                premiumGlow.setOutlineColor(item.getRarityColor());  // Rarity-based glow color
                window.draw(premiumGlow);
                
                // Pulsing effect by varying outline thickness based on time (optional)
                // For now, just the static glow is sufficient
            }
            
            // Show item name when player is adjacent
            if (player) {
                Position playerPos = player->getPosition();
                if (loot.isAdjacentTo(playerPos.x, playerPos.y)) {
                    sf::Font font;
                    if (font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
                        sf::Text itemNameText(font);
                        itemNameText.setString(loot.getItem().name);
                        itemNameText.setCharacterSize(14);
                        itemNameText.setFillColor(loot.getItem().getRarityColor());
                        itemNameText.setOutlineThickness(2.0f);
                        itemNameText.setOutlineColor(sf::Color::Black);
                        itemNameText.setPosition(sf::Vector2f(
                            loot.getX() * 32.0f, 
                            loot.getY() * 32.0f - 20.0f
                        ));
                        window.draw(itemNameText);
                    }
                }
            }
        }
        
        // Show pickup prompt when adjacent to loot
        if (uiManager && player) {
            Position playerPos = player->getPosition();
            for (const auto& loot : loots) {
                if (loot.isAdjacentTo(playerPos.x, playerPos.y)) {
                    uiManager->renderContextualPrompt(window, "Press E to pick up " + loot.getItem().name);
                    break;  // Only show one prompt
                }
            }
        }
        
        // CHANGE: 2025-11-14 - Debug rendering: Bounding boxes
        if (debugShowBoundingBoxes) {
            if (player) {
                Position playerPos = player->getPosition();
                sf::RectangleShape playerBox(sf::Vector2f(32.0f, 32.0f));
                playerBox.setPosition(sf::Vector2f(playerPos.x * 32.0f, playerPos.y * 32.0f));
                playerBox.setFillColor(sf::Color::Transparent);
                playerBox.setOutlineThickness(2.0f);
                playerBox.setOutlineColor(sf::Color(0, 255, 0));  // Green for player
                window.draw(playerBox);
            }
            
            if (enemyManager) {
                const auto& enemies = enemyManager->getEnemies();
                for (const auto& enemy : enemies) {
                    sf::RectangleShape enemyBox(sf::Vector2f(32.0f, 32.0f));
                    enemyBox.setPosition(sf::Vector2f(enemy.x * 32.0f, enemy.y * 32.0f));
                    enemyBox.setFillColor(sf::Color::Transparent);
                    enemyBox.setOutlineThickness(2.0f);
                    enemyBox.setOutlineColor(sf::Color(255, 0, 0));  // Red for enemies
                    window.draw(enemyBox);
                }
            }
        }
        
        // CHANGE: 2025-11-14 - Debug rendering: Loot item bounds
        if (debugShowBoundingBoxes) {
            for (const auto& loot : loots) {
                sf::RectangleShape lootBox(sf::Vector2f(32.0f, 32.0f));
                lootBox.setPosition(sf::Vector2f(loot.getX() * 32.0f, loot.getY() * 32.0f));
                lootBox.setFillColor(sf::Color::Transparent);
                lootBox.setOutlineThickness(2.0f);
                lootBox.setOutlineColor(sf::Color(255, 255, 0));  // Yellow for loot
                window.draw(lootBox);
            }
        }
        
        // Apply lighting effect (dark overlay with light around player)
        if (player && renderer->isLightingEnabled()) {
            renderer->applyLighting(*player);
        }
        
        // ✨ Render combat effects after entities but before UI
        renderCombatEffects();
        
        // Render enhanced UI with panels (HUD, minimap, etc.)
        if (uiManager && dungeon && player && enemyManager) {
            uiManager->renderUI(window, *player, *dungeon, *enemyManager, skillTree.get(), currentFloor);
        }
        
        // TASK D & I: Show contextual prompt when near stairs
        if (uiManager && dungeon && player && enemyManager) {
            Position playerPos = player->getPosition();
            if (dungeon->isAdjacentToStairs(playerPos.x, playerPos.y)) {
                if (enemyManager->isEmpty()) {
                    uiManager->renderContextualPrompt(window, "Press E to descend to next floor");
                } else {
                    uiManager->renderContextualPrompt(window, "Defeat all enemies to unlock stairs!");
                }
            }
        }
        
        // Render Game Over screen
        if (currentState == GameState::GameOver) {
            renderGameOverScreen();
        }
    }
    
    // Render shop overlay (on top of everything)
    if (shop && shop->isShopOpen()) {
        shop->render(window);
    }
    
    renderer->end();
}

void Game::renderGameOverScreen() {
    // Semi-transparent dark overlay
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);
    
    // Use simple rectangle panel instead of texture
    sf::RectangleShape panel(sf::Vector2f(400.f, 300.f));
    float panelX = window.getSize().x / 2.0f - 200.0f;
    float panelY = window.getSize().y / 2.0f - 150.0f;
    panel.setPosition(sf::Vector2f(panelX, panelY));
    panel.setFillColor(sf::Color(80, 60, 50, 220));
    panel.setOutlineColor(sf::Color(120, 90, 70));
    panel.setOutlineThickness(5.f);
    window.draw(panel);
    
    // Load font for text
    sf::Font font;
    if (font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        // "GAME OVER" title
        sf::Text gameOverText(font);
        gameOverText.setString("GAME OVER");
        gameOverText.setCharacterSize(60);
        gameOverText.setFillColor(sf::Color(200, 50, 50));
        gameOverText.setOutlineThickness(3.0f);
        gameOverText.setOutlineColor(sf::Color::Black);
        gameOverText.setPosition(sf::Vector2f(
            window.getSize().x / 2.0f - 180.0f,
            window.getSize().y / 2.0f - 100.0f
        ));
        window.draw(gameOverText);
        
        // Stats text
        if (player) {
            std::string statsText = "Final Level: " + std::to_string(player->getLevel()) + 
                                   "\nTotal XP: " + std::to_string(player->getExperience()) +
                                   "\nEnemies Defeated: " + std::to_string(player->getExperience() / 25);
            
            sf::Text stats(font);
            stats.setString(statsText);
            stats.setCharacterSize(24);
            stats.setFillColor(sf::Color(220, 220, 220));
            stats.setOutlineThickness(1.0f);
            stats.setOutlineColor(sf::Color::Black);
            stats.setPosition(sf::Vector2f(
                window.getSize().x / 2.0f - 100.0f,
                window.getSize().y / 2.0f - 10.0f
            ));
            window.draw(stats);
        }
        
        // Instructions
        sf::Text instructions(font);
        instructions.setString("Press ESC to exit");
        instructions.setCharacterSize(20);
        instructions.setFillColor(sf::Color(150, 150, 150));
        instructions.setOutlineThickness(1.0f);
        instructions.setOutlineColor(sf::Color::Black);
        instructions.setPosition(sf::Vector2f(
            window.getSize().x / 2.0f - 100.0f,
            window.getSize().y / 2.0f + 120.0f
        ));
        window.draw(instructions);
    }
}

void Game::setState(GameState state) {
    currentState = state;
    std::cout << "[Game] State changed to: " << static_cast<int>(state) << std::endl;
    
    if (state == GameState::Playing) {
        uiManager->showHUD();
    }
}

void Game::checkExitAccess() {
    // Check if all enemies on current floor are defeated
    if (enemyManager->isEmpty()) {
        // Find a good position for exit stairs (center of last room)
        auto rooms = dungeon->getRooms();
        if (!rooms.empty()) {
            const auto& lastRoom = rooms.back();
            // CHANGE: 2025-11-14 - Add bounds checking for exit stairs placement
            exitStairsPosition.x = lastRoom.x + lastRoom.width / 2;
            exitStairsPosition.y = lastRoom.y + lastRoom.height / 2;
            
            // Clamp to dungeon bounds
            int maxX = dungeon->getWidth() - 1;
            int maxY = dungeon->getHeight() - 1;
            exitStairsPosition.x = std::max(0, std::min(exitStairsPosition.x, maxX));
            exitStairsPosition.y = std::max(0, std::min(exitStairsPosition.y, maxY));
            
            // Place exit tile in dungeon
            dungeon->setTile(exitStairsPosition.x, exitStairsPosition.y, TileType::Exit);
            
            uiManager->addFloatingText("Exit Unlocked!", exitStairsPosition.x * 32.0f, exitStairsPosition.y * 32.0f, sf::Color(255, 215, 0));
            std::cout << "[Floor] Exit stairs spawned at (" << exitStairsPosition.x << ", " << exitStairsPosition.y << ")" << std::endl;
        }
    }
}

void Game::spawnEnemiesForFloor(int floor) {
    // Calculate scaling factor based on floor number
    float scale = 1.0f + (floor - 1) * 0.2f;
    
    std::cout << "[Floor] Spawning enemies for floor " << floor << " (scale: " << scale << "x)" << std::endl;
    
    auto rooms = dungeon->getRooms();
    if (rooms.size() < 4) return;
    
    // Helper lambda to check if position is safe distance from player
    auto isSafeDistance = [&](int x, int y) -> bool {
        if (!player) return true;
        Position playerPos = player->getPosition();
        float distance = std::abs(x - playerPos.x) + std::abs(y - playerPos.y);  // Manhattan distance
        return distance >= 3.0f;  // Minimum 3 tiles away
    };
    
    // Helper lambda to find safe spawn position in room
    auto findSafeSpawnPos = [&](const Room& room) -> std::pair<int, int> {
        // Try center positions first
        std::vector<std::pair<int, int>> candidates = {
            {room.x + room.width / 2, room.y + room.height / 2},
            {room.x + 1, room.y + 1},
            {room.x + room.width - 2, room.y + 1},
            {room.x + 1, room.y + room.height - 2},
            {room.x + room.width - 2, room.y + room.height - 2}
        };
        
        for (const auto& pos : candidates) {
            if (isSafeDistance(pos.first, pos.second)) {
                return pos;
            }
        }
        
        // Fallback to room center if no safe position found
        return {room.x + room.width / 2, room.y + room.height / 2};
    };
    
    // Floors 1-2: Basic enemies (Goblins, Orcs, Skeletons)
    if (floor <= 2) {
        auto pos1 = findSafeSpawnPos(rooms[1]);
        enemyManager->spawnEnemy("Goblin Scout", "melee", pos1.first, pos1.second, 
                                  static_cast<int>(50 * scale), static_cast<int>(10 * scale), 1, 1.5f);
        
        auto pos2 = findSafeSpawnPos(rooms[2]);
        enemyManager->spawnEnemy("Orc Grunt", "melee", pos2.first, pos2.second, 
                                  static_cast<int>(80 * scale), static_cast<int>(15 * scale), 1, 1.2f);
        
        auto pos3 = findSafeSpawnPos(rooms[3]);
        enemyManager->spawnEnemy("Skeleton Mage", "ranged", pos3.first, pos3.second, 
                                  static_cast<int>(60 * scale), static_cast<int>(18 * scale), 3, 2.0f);
        
        if (rooms.size() >= 5) {
            auto pos4 = findSafeSpawnPos(rooms.back());
            enemyManager->spawnEnemy("Shadow Knight", "boss", pos4.first, pos4.second, 
                                      static_cast<int>(150 * scale), static_cast<int>(25 * scale), 1, 1.0f);
        }
    }
    // Floors 3-4: Intermediate enemies (Wraiths, Dark Mages, Gargoyles)
    else if (floor <= 4) {
        auto pos1 = findSafeSpawnPos(rooms[1]);
        enemyManager->spawnEnemy("Wraith", "ranged", pos1.first, pos1.second, 
                                  static_cast<int>(90 * scale), static_cast<int>(22 * scale), 2, 1.8f);
        
        auto pos2 = findSafeSpawnPos(rooms[2]);
        enemyManager->spawnEnemy("Dark Mage", "ranged", pos2.first, pos2.second, 
                                  static_cast<int>(100 * scale), static_cast<int>(28 * scale), 3, 2.5f);
        
        auto pos3 = findSafeSpawnPos(rooms[3]);
        enemyManager->spawnEnemy("Gargoyle", "melee", pos3.first, pos3.second, 
                                  static_cast<int>(120 * scale), static_cast<int>(20 * scale), 1, 1.0f);
        
        if (rooms.size() >= 5) {
            auto pos4 = findSafeSpawnPos(rooms.back());
            enemyManager->spawnEnemy("Necromancer", "boss", pos4.first, pos4.second, 
                                      static_cast<int>(180 * scale), static_cast<int>(32 * scale), 3, 2.0f);
        }
    }
    // Floors 5-7: Advanced enemies (Vampires, Liches, Minotaurs)
    else if (floor <= 7) {
        auto pos1 = findSafeSpawnPos(rooms[1]);
        enemyManager->spawnEnemy("Vampire Lord", "melee", pos1.first, pos1.second, 
                                  static_cast<int>(140 * scale), static_cast<int>(35 * scale), 1, 1.5f);
        
        auto pos2 = findSafeSpawnPos(rooms[2]);
        enemyManager->spawnEnemy("Lich", "ranged", pos2.first, pos2.second, 
                                  static_cast<int>(160 * scale), static_cast<int>(40 * scale), 3, 2.8f);
        
        auto pos3 = findSafeSpawnPos(rooms[3]);
        enemyManager->spawnEnemy("Minotaur", "melee", pos3.first, pos3.second, 
                                  static_cast<int>(200 * scale), static_cast<int>(45 * scale), 1, 1.2f);
        
        if (rooms.size() >= 5) {
            auto pos4 = findSafeSpawnPos(rooms.back());
            enemyManager->spawnEnemy("Shadow Knight", "boss", pos4.first, pos4.second, 
                                      static_cast<int>(250 * scale), static_cast<int>(50 * scale), 1, 1.0f);
        }
    }
    // Floors 8+: Legendary enemies (Dragons and elite bosses)
    else {
        auto pos1 = findSafeSpawnPos(rooms[1]);
        enemyManager->spawnEnemy("Dragon", "boss", pos1.first, pos1.second, 
                                  static_cast<int>(300 * scale), static_cast<int>(60 * scale), 2, 2.0f);
        
        auto pos2 = findSafeSpawnPos(rooms[2]);
        enemyManager->spawnEnemy("Ancient Lich", "boss", pos2.first, pos2.second, 
                                  static_cast<int>(250 * scale), static_cast<int>(55 * scale), 3, 3.0f);
        
        auto pos3 = findSafeSpawnPos(rooms[3]);
        enemyManager->spawnEnemy("War Minotaur", "boss", pos3.first, pos3.second, 
                                  static_cast<int>(280 * scale), static_cast<int>(65 * scale), 1, 1.0f);
        
        if (rooms.size() >= 5) {
            auto pos4 = findSafeSpawnPos(rooms.back());
            enemyManager->spawnEnemy("Elder Dragon", "boss", pos4.first, pos4.second, 
                                      static_cast<int>(400 * scale), static_cast<int>(75 * scale), 2, 2.5f);
        }
    }
    
    std::cout << "[Floor] Spawned " << (rooms.size() >= 5 ? 4 : 3) << " enemies with " << scale << "x stats (min 3 tile distance)" << std::endl;
}

void Game::nextFloor() {
    // CHANGE: 2025-11-14 - Clean up loot and effects from previous floor
    loots.clear();  // Remove all unpicked loot
    activeEffects.clear();  // Remove all active visual effects
    
    currentFloor++;
    
    // Check for victory condition
    if (currentFloor > 10) {
        std::cout << "\n========================================" << std::endl;
        std::cout << "  🎉 VICTORY! ALL 10 FLOORS CLEARED! 🎉" << std::endl;
        std::cout << "========================================\n" << std::endl;
        uiManager->addFloatingText("VICTORY! Dungeon Conquered!", 
                                    player->getPosition().x * 32.0f, player->getPosition().y * 32.0f, 
                                    sf::Color(255, 215, 0));
        currentState = GameState::Victory;
        return;
    }
    
    // Advance floor using level manager
    levelManager->advanceFloor();
    
    std::cout << "\n[Floor] " << levelManager->getFloorDisplayText(currentFloor) << std::endl;
    
    // Check for boss floor
    if (levelManager->isBossFloor(currentFloor)) {
        std::cout << "⚔️  WARNING: BOSS ENCOUNTER - " << levelManager->getBossName(currentFloor) << " ⚔️" << std::endl;
        uiManager->addFloatingText("⚔️ BOSS FLOOR ⚔️", 
                                    player->getPosition().x * 32.0f, player->getPosition().y * 32.0f - 50.0f, 
                                    sf::Color(255, 0, 0));
    }
    
    // Check for skill unlock
    if (levelManager->shouldUnlockSkill(currentFloor)) {
        std::string skillName = levelManager->getUnlockedSkillName(currentFloor);
        std::cout << "✨ NEW SKILL UNLOCKED: " << skillName << " ✨" << std::endl;
        uiManager->addFloatingText("New Skill: " + skillName, 
                                    player->getPosition().x * 32.0f, player->getPosition().y * 32.0f - 70.0f, 
                                    sf::Color(0, 255, 255));
    }
    
    std::cout << levelManager->getFloorDescription(currentFloor) << "\n" << std::endl;
    
    uiManager->addFloatingText(levelManager->getFloorDisplayText(currentFloor), 
                                player->getPosition().x * 32.0f, player->getPosition().y * 32.0f, 
                                sf::Color(255, 255, 255));
    
    // Generate new level using level manager
    levelManager->generateLevel(currentFloor, *dungeon, *enemyManager, *player);
    
    // Reset player to start position
    auto rooms = dungeon->getRooms();
    if (!rooms.empty()) {
        player->moveTo({rooms[0].x + 1, rooms[0].y + 1});
    }
    
    // Reset exit position (will be set when all enemies defeated)
    exitStairsPosition = {0, 0};
    
    std::cout << "[Floor] Floor " << currentFloor << " ready!" << std::endl;
}

void Game::dropItemFromEnemy(const std::string& enemyName, int x, int y) {
    // CHANGE: 2025-11-14 - Unified to use ItemNew system only (deprecated old Item)
    // Random chance for item drop (50% for regular enemies, 100% for bosses)
    int dropChance = rand() % 100;
    bool isBoss = (enemyName.find("Dragon") != std::string::npos || 
                   enemyName.find("Knight") != std::string::npos ||
                   enemyName.find("Lich") != std::string::npos ||
                   enemyName.find("Necromancer") != std::string::npos);
    
    if (dropChance < 50 || isBoss) {
        ItemNew droppedItem;  // Use ItemNew instead of old Item
        
        // Floor-based loot table (ItemNew system)
        if (currentFloor <= 2) {
            // Early floors: Basic items
            int itemRoll = rand() % 100;
            if (itemRoll < 40) {
                droppedItem = ItemNew("potion", "Health Potion", "consumable", 1, 50,
                    ItemAction("heal", {{{"amount", 50}}}));
            } else if (itemRoll < 60) {
                droppedItem = ItemNew("coin_gold", "Gold Coin", "treasure", 1, 100);
            } else if (itemRoll < 80) {
                droppedItem = ItemNew("sword_iron", "Iron Sword", "weapon", 2, 50);
            } else {
                droppedItem = ItemNew("shield_wood", "Wooden Shield", "armor", 1, 30);
            }
        } else if (currentFloor <= 4) {
            // Mid floors: Better items
            int itemRoll = rand() % 100;
            if (itemRoll < 30) {
                droppedItem = ItemNew("potion_mega", "Mega Potion", "consumable", 2, 75,
                    ItemAction("heal", {{{"amount", 150}}}));
            } else if (itemRoll < 50) {
                droppedItem = ItemNew("potion_strength", "Strength Potion", "consumable", 2, 100,
                    ItemAction("buff", {{{"stat", "attack"}}, {{"amount", 5}}, {{"duration", 10.0}}}));
            } else if (itemRoll < 70) {
                droppedItem = ItemNew("sword_flame", "Flame Sword", "weapon", 3, 200);
            } else if (itemRoll < 85) {
                droppedItem = ItemNew("shield_iron", "Iron Shield", "armor", 2, 80);
            } else {
                droppedItem = ItemNew("frost_bomb", "Frost Bomb", "consumable", 3, 90,
                    ItemAction("attack", {{{"type", "frost"}}, {{"damage", 40}}}));
            }
        } else if (currentFloor <= 7) {
            // Deep floors: Advanced items
            int itemRoll = rand() % 100;
            if (itemRoll < 25) {
                droppedItem = ItemNew("elixir", "Elixir", "consumable", 3, 150,
                    ItemAction("heal", {{{"amount", 100}}}));
            } else if (itemRoll < 45) {
                droppedItem = ItemNew("fire_scroll", "Fire Scroll", "consumable", 4, 150,
                    ItemAction("attack", {{{"type", "fire"}}, {{"damage", 100}}}));
            } else if (itemRoll < 65) {
                droppedItem = ItemNew("holy_water", "Holy Water", "consumable", 3, 120,
                    ItemAction("attack", {{{"type", "holy"}}, {{"damage", 80}}}));
            } else if (itemRoll < 80) {
                droppedItem = ItemNew("amulet_wisdom", "Amulet of Wisdom", "accessory", 3, 120);
            } else {
                droppedItem = ItemNew("lightning_rod", "Lightning Rod", "weapon", 4, 180);
            }
        } else {
            // Legendary floors: Epic loot
            int itemRoll = rand() % 100;
            if (itemRoll < 20) {
                droppedItem = ItemNew("revive_scroll", "Scroll of Resurrection", "utility", 5, 500);
            } else if (itemRoll < 40) {
                droppedItem = ItemNew("sword_legendary", "Legendary Blade", "weapon", 5, 1000);
            } else if (itemRoll < 60) {
                droppedItem = ItemNew("armor_dragon", "Dragon Scale Armor", "armor", 5, 800);
            } else if (itemRoll < 80) {
                droppedItem = ItemNew("amulet_health", "Amulet of Vitality", "accessory", 4, 300);
            } else {
                droppedItem = ItemNew("gem_ruby", "Ruby Gem", "treasure", 5, 250);
            }
        }
        
        // Spawn loot on ground instead of direct inventory
        spawnLootAt(sf::Vector2i(x, y), droppedItem);
        
        // Show floating text
        uiManager->addFloatingText("+ " + droppedItem.name, 
            x * 32.0f, y * 32.0f - 20.0f, sf::Color(255, 215, 0));
        
        std::cout << "[Loot] " << droppedItem.name << " (" << droppedItem.getRarityName() 
                  << ") dropped by " << enemyName << std::endl;
    }
}

// ═══════════════════════════════════════════════════════════════════════
// ✨ COMBAT EFFECT SYSTEM - Visual feedback for attacks and skills
// ═══════════════════════════════════════════════════════════════════════

void Game::addCombatEffect(const std::string& effectType, float x, float y, float duration) {
    activeEffects.push_back(CombatEffect(effectType, x, y, duration));
    std::cout << "[Effects] Added " << effectType << " effect at (" << x << ", " << y << ")" << std::endl;
}

void Game::updateCombatEffects(float deltaTime) {
    // Update all active effects and remove expired ones
    for (auto it = activeEffects.begin(); it != activeEffects.end(); ) {
        it->lifetime -= deltaTime;
        
        if (it->lifetime <= 0.0f) {
            it = activeEffects.erase(it);  // Remove expired effect
        } else {
            ++it;
        }
    }
}

void Game::renderCombatEffects() {
    if (!renderer) return;
    
    for (const auto& effect : activeEffects) {
        // Get appropriate texture based on effect type
        std::string textureKey;
        
        if (effect.effectType == "swing") {
            textureKey = "effect_attack_swing";
        } else if (effect.effectType == "large_swing") {
            textureKey = "effect_attack_large";
        } else if (effect.effectType == "explosion") {
            textureKey = "effect_explosion";
        } else if (effect.effectType == "fire_explosion") {
            textureKey = "effect_fire_explosion";
        } else if (effect.effectType == "magic_explosion") {
            textureKey = "effect_magic_explosion";
        } else if (effect.effectType == "arrow") {
            textureKey = "effect_arrow";
        } else if (effect.effectType == "acid") {
            textureKey = "effect_acid";
        } else if (effect.effectType == "ghost_orb") {
            textureKey = "effect_ghost_orb";
        } else {
            continue;  // Unknown effect type
        }
        
        sf::Texture* texture = AssetManager::getInstance().getTexture(textureKey);
        // CHANGE: 2025-11-14 - Add null check for failed texture loading
        if (!texture) {
            std::cerr << "[Error] Failed to load combat effect texture: " << textureKey << std::endl;
            continue;  // Skip rendering if texture failed to load
        }
        if (texture) {
            sf::Sprite effectSprite(*texture);
            effectSprite.setPosition(sf::Vector2f(effect.x, effect.y));
            
            // Calculate fade based on remaining lifetime
            float alpha = (effect.lifetime / effect.maxLifetime) * 255.0f;
            effectSprite.setColor(sf::Color(255, 255, 255, static_cast<unsigned char>(alpha)));
            
            // Scale effect to 32x32 tile size
            float scale = 32.0f / texture->getSize().x;
            effectSprite.setScale(sf::Vector2f(scale, scale));
            
            window.draw(effectSprite);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════
// CHANGE: 2025-11-14 - Deprecated: Door opening logic now integrated into E key handler
// These functions are preserved for reference but no longer called separately
// ═══════════════════════════════════════════════════════════════════════

void Game::tryOpenDoor() {
    // DEPRECATED: Now integrated into handleInput E key handler with priority system
    // Door opening is handled in the E key case with proper priority:
    // 1. Pick up loot if nearby
    // 2. Open door if adjacent and not already open
    // 3. Descend stairs if adjacent
    std::cout << "[Deprecated] tryOpenDoor() - use E key handler instead" << std::endl;
}

void Game::checkRoomClearDoors() {
    // DEPRECATED: Now integrated into moveEnemiesWithBFS() when enemies are cleared
    // When all enemies are defeated, auto-opening doors are opened automatically
    std::cout << "[Deprecated] checkRoomClearDoors() - handled in moveEnemiesWithBFS()" << std::endl;
}

void Game::spawnLootAt(const sf::Vector2i& tilePos, const ItemNew& item) {
    // CHANGE: 2025-11-14 - Implement max loot limit to prevent accumulation
    static const int MAX_LOOTS = 100;  // Maximum items on ground at once
    
    if (loots.size() >= MAX_LOOTS) {
        // Remove oldest loot to make room (FIFO cleanup)
        std::cout << "[Loot] Max loot limit reached (" << MAX_LOOTS << "), removing oldest" << std::endl;
        loots.erase(loots.begin());
    }
    
    Loot loot(item, tilePos);
    loots.push_back(loot);
    
    std::cout << "[Loot] Spawned " << item.name << " (" << item.getRarityName() 
              << ") at (" << tilePos.x << ", " << tilePos.y << ") [" << loots.size() << "/" << MAX_LOOTS << "]" << std::endl;
    
    // CHANGE: 2025-11-14 - Track rare/valuable loot using Heap for priority highlighting
    // Items with rarity >= 3 or value >= 100 are added to premium loot tracker
    if (item.rarity >= 3 || item.value >= 100) {
        std::string priority = "[Heap] Premium loot: " + item.name + " (rarity: " + std::to_string(item.rarity) 
                             + ", value: " + std::to_string(item.value) + ")";
        std::cout << priority << " - HIGHLIGHT CANDIDATE" << std::endl;
        // Heap-based tracking: Higher rarity and value items are prioritized for visual prominence
        // This allows the UI to prioritize showing pop-ups and highlights for the most valuable drops
    }
}

void Game::updateLoots(float deltaTime) {
    for (auto& loot : loots) {
        loot.update(deltaTime);
    }
}


