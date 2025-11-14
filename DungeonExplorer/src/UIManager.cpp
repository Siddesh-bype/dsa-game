// CHANGE: 2025-11-10 — Comprehensive UI/UX fixes
// - Fixed HUD layout to prevent HP/XP/Level overlaps
// - Added character name rendering above sprites
// - Added contextual prompts for interactions
// - Improved spacing and positioning

#include "UIManager.h"
#include "AssetManager.h"
#include "Game.h"
#include "Enemy.h"
#include "SkillTree.h"
#include <iostream>
#include <sstream>
#include <iomanip>

UIManager::UIManager(Game* game) 
    : game(game), fontLoaded(false), inventoryVisible(false), 
      skillTreeVisible(false), miniMapVisible(true), animationTime(0.f),
      screenFlashTimer(0.f), screenFlashColor(sf::Color::Transparent) {
    dsaViz = std::make_unique<DSAVisualizer>();
}

void UIManager::initialize() {
    std::cout << "[UIManager] Initializing enhanced SFML UI..." << std::endl;
    
    // HUD background bar - full width for complete HUD
    hudBackground.setSize({800.f, 75.f});  // Increased height for better spacing
    hudBackground.setPosition(sf::Vector2f({0.f, 0.f}));
    hudBackground.setFillColor(sf::Color(20, 20, 30, 210));
    
    // Health bar (left side, with more room)
    healthBarBg.setSize({220.f, 22.f});  // Slightly larger
    healthBarBg.setPosition(sf::Vector2f({15.f, 42.f}));  // Lower position
    healthBarBg.setFillColor(sf::Color(50, 30, 30));
    
    healthBarFg.setSize({220.f, 22.f});
    healthBarFg.setPosition(sf::Vector2f({15.f, 42.f}));
    healthBarFg.setFillColor(sf::Color(200, 50, 50));
    
    // XP bar (middle, well-separated from level text)
    xpBarBg.setSize({180.f, 14.f});
    xpBarBg.setPosition(sf::Vector2f({260.f, 50.f}));  // Moved right and down
    xpBarBg.setFillColor(sf::Color(30, 30, 50));
    
    xpBarFg.setSize({180.f, 14.f});
    xpBarFg.setPosition(sf::Vector2f({260.f, 50.f}));
    xpBarFg.setFillColor(sf::Color(100, 100, 255));
    
    std::cout << "[UIManager] UI initialized with improved spacing" << std::endl;
    
    // Initialize DSA Visualizer with font
    if (fontLoaded) {
        dsaViz->setFont(&font);
    }
}

void UIManager::createMainMenu() {
    // Main menu logic
}

void UIManager::createHUD() {
    // HUD created in initialize()
}

void UIManager::createInventoryScreen() {
    // Inventory panel logic
}

void UIManager::createSkillTreeScreen() {
    // Skill tree panel logic
}

void UIManager::update(float deltaTime) {
    // Animation updates here
    animationTime += deltaTime;
    
    // Update floating texts
    for (auto it = floatingTexts.begin(); it != floatingTexts.end();) {
        it->lifetime -= deltaTime;
        if (it->lifetime <= 0.0f) {
            it = floatingTexts.erase(it);
        } else {
            // Float upward
            it->position.y -= 30.0f * deltaTime;
            ++it;
        }
    }
    
    // Update screen flash
    if (screenFlashTimer > 0.0f) {
        screenFlashTimer -= deltaTime;
        if (screenFlashTimer <= 0.0f) {
            screenFlashColor = sf::Color::Transparent;
        }
    }
}

void UIManager::render(sf::RenderWindow& window, const Player& player, 
                        const Dungeon& dungeon, const EnemyManager& enemies, int currentFloor) {
    const float tileSize = 32.f;  // Match Renderer tile size
    
    // Render DSA visualizations FIRST (under everything)
    if (dsaViz) {
        dsaViz->renderGraphPaths(window, dungeon, tileSize);
        dsaViz->renderStackTrail(window, player, tileSize);
        
        // Example loot sparkle (you can add actual loot positions later)
        if (dungeon.getRooms().size() > 2) {
            const auto& room = dungeon.getRooms()[2];
            float lootX = (room.x + room.width / 2.f) * tileSize;
            float lootY = (room.y + room.height / 2.f) * tileSize;
            dsaViz->renderHeapGlow(window, sf::Vector2f(lootX, lootY), 100);
        }
    }
    
    // Always render HUD (pass nullptr for skillTree - not available in this function)
    renderHUD(window, player, nullptr, currentFloor);
    
    // Render optional panels
    if (miniMapVisible) {
        renderMiniMap(window, dungeon, player, enemies);
    }
    
    if (inventoryVisible) {
        renderInventoryPanel(window, player);
    }
    
    if (skillTreeVisible) {
        renderSkillTreePanel(window);
    }
    
    // Always render turn queue
    renderTurnQueue(window, enemies);
    
    // Render combat feedback (on top of everything)
    renderFloatingTexts(window);
    renderScreenFlash(window);
}

void UIManager::renderGraphPathsOnly(sf::RenderWindow& window, const Dungeon& dungeon) {
    const float tileSize = 32.f;
    if (dsaViz) {
        dsaViz->renderGraphPaths(window, dungeon, tileSize);
    }
}

void UIManager::renderUI(sf::RenderWindow& window, const Player& player, 
                          const Dungeon& dungeon, const EnemyManager& enemies, SkillTree* skillTree, int currentFloor) {
    const float tileSize = 32.f;
    
    // Render stack trail and other DSA effects (NOT graph paths - already rendered)
    if (dsaViz) {
        dsaViz->renderStackTrail(window, player, tileSize);
        
        // Example loot sparkle
        if (dungeon.getRooms().size() > 2) {
            const auto& room = dungeon.getRooms()[2];
            float lootX = (room.x + room.width / 2.f) * tileSize;
            float lootY = (room.y + room.height / 2.f) * tileSize;
            dsaViz->renderHeapGlow(window, sf::Vector2f(lootX, lootY), 100);
        }
    }
    
    // Always render HUD
    renderHUD(window, player, skillTree, currentFloor);
    
    // Render optional panels
    if (miniMapVisible) {
        renderMiniMap(window, dungeon, player, enemies);
    }
    
    if (inventoryVisible) {
        renderInventoryPanel(window, player);
    }
    
    if (skillTreeVisible) {
        renderSkillTreePanel(window);
    }
    
    // Always render turn queue
    renderTurnQueue(window, enemies);
    
    // Always render skill hotkeys bar
    renderSkillHotkeys(window, player, skillTree);
    
    // TASK C: Render character names above sprites
    renderCharacterNames(window, player, enemies, tileSize);
    
    // Render combat feedback (on top of everything)
    renderFloatingTexts(window);
    renderScreenFlash(window);
}

void UIManager::addFloatingText(const std::string& text, float x, float y, sf::Color color) {
    floatingTexts.emplace_back(text, sf::Vector2f(x, y), color, 1.5f);
}

void UIManager::triggerScreenFlash(sf::Color color, float duration) {
    screenFlashColor = color;
    screenFlashTimer = duration;
}

void UIManager::renderFloatingTexts(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    
    for (const auto& floatText : floatingTexts) {
        float alpha = (floatText.lifetime / floatText.maxLifetime) * 255.0f;
        sf::Color fadeColor = floatText.color;
        fadeColor.a = static_cast<std::uint8_t>(alpha);
        
        sf::Text text(font, floatText.text, 20);
        text.setPosition(floatText.position);
        text.setFillColor(fadeColor);
        text.setOutlineThickness(2.0f);
        text.setOutlineColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(alpha)));
        window.draw(text);
    }
}

void UIManager::renderScreenFlash(sf::RenderWindow& window) {
    if (screenFlashTimer <= 0.0f) return;
    
    float alpha = (screenFlashTimer / 0.3f) * 100.0f; // Max 100 alpha
    sf::Color flashColor = screenFlashColor;
    flashColor.a = static_cast<std::uint8_t>(alpha);
    
    sf::RectangleShape flash(sf::Vector2f(window.getSize().x, window.getSize().y));
    flash.setFillColor(flashColor);
    window.draw(flash);
}

// TASK C: Character names (floating labels above sprites)
void UIManager::renderCharacterNames(sf::RenderWindow& window, const Player& player, const EnemyManager& enemies, float tileSize) {
    if (!fontLoaded) return;
    
    // Render player name
    Position playerPos = player.getPosition();
    float playerX = playerPos.x * tileSize + tileSize / 2.f;
    float playerY = playerPos.y * tileSize - tileSize / 3.f;  // Above sprite
    
    sf::Text playerName(font, player.getName(), 11);
    playerName.setFillColor(sf::Color(100, 255, 100));  // Green for player
    playerName.setOutlineThickness(1.5f);
    playerName.setOutlineColor(sf::Color::Black);
    // Center the text (SFML 3.x: FloatRect.size.x instead of .width)
    sf::FloatRect playerBounds = playerName.getGlobalBounds();
    playerName.setPosition(sf::Vector2f(playerX - playerBounds.size.x / 2.f, playerY));
    window.draw(playerName);
    
    // Render enemy names
    for (const auto& enemy : enemies.getEnemies()) {
        float enemyX = enemy.x * tileSize + tileSize / 2.f;
        float enemyY = enemy.y * tileSize - tileSize / 3.f;
        
        sf::Text enemyName(font, enemy.name, 10);
        // Color based on enemy type
        if (enemy.type == "boss") {
            enemyName.setFillColor(sf::Color(255, 50, 50));  // Red for bosses
        } else if (enemy.type == "ranged") {
            enemyName.setFillColor(sf::Color(255, 200, 50));  // Yellow for ranged
        } else {
            enemyName.setFillColor(sf::Color(255, 150, 150));  // Light red for melee
        }
        enemyName.setOutlineThickness(1.5f);
        enemyName.setOutlineColor(sf::Color::Black);
        
        sf::FloatRect enemyBounds = enemyName.getGlobalBounds();
        enemyName.setPosition(sf::Vector2f(enemyX - enemyBounds.size.x / 2.f, enemyY));
        window.draw(enemyName);
    }
}

// TASK I: Contextual prompts for player interactions
void UIManager::renderContextualPrompt(sf::RenderWindow& window, const std::string& prompt) {
    if (!fontLoaded || prompt.empty()) return;
    
    // Draw prompt at bottom-center of screen
    sf::Text promptText(font, prompt, 16);
    promptText.setFillColor(sf::Color(255, 255, 100));
    promptText.setOutlineThickness(2.f);
    promptText.setOutlineColor(sf::Color::Black);
    
    // Center horizontally, place near bottom (SFML 3.x compatibility)
    sf::FloatRect bounds = promptText.getGlobalBounds();
    float centerX = (window.getSize().x - bounds.size.x) / 2.f;
    float bottomY = window.getSize().y - 100.f;
    
    // Background panel for better visibility
    sf::RectangleShape promptBg(sf::Vector2f(bounds.size.x + 20.f, bounds.size.y + 16.f));
    promptBg.setPosition(sf::Vector2f(centerX - 10.f, bottomY - 8.f));
    promptBg.setFillColor(sf::Color(0, 0, 0, 180));
    promptBg.setOutlineThickness(2.f);
    promptBg.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(promptBg);
    
    promptText.setPosition(sf::Vector2f(centerX, bottomY));
    window.draw(promptText);
}


void UIManager::renderHUD(sf::RenderWindow& window, const Player& player, SkillTree* skillTree, int currentFloor) {
    // ═══════════════════════════════════════════════════════════════════════
    // 🎨 HUD BACKGROUND PANEL - Simple rectangle version
    // ═══════════════════════════════════════════════════════════════════════
    
    // Use simple rectangle instead of texture
    sf::RectangleShape hudPanel(sf::Vector2f(400.f, 70.f));
    hudPanel.setPosition(sf::Vector2f(5.f, 5.f));
    hudPanel.setFillColor(sf::Color(80, 60, 50, 200));
    hudPanel.setOutlineColor(sf::Color(120, 90, 70));
    hudPanel.setOutlineThickness(3.f);
    window.draw(hudPanel);
    
    // ═══════════════════════════════════════════════════════════════════════
    // ❤️ HEALTH BAR (RED) - 3-part system: Left + Middle (repeating) + Right
    // Components: barRed_horizontalLeft.png, barRed_horizontalMid.png, barRed_horizontalRight.png
    // Alternative colors available:
    //   - barGreen_horizontal* (stamina/energy)
    //   - barYellow_horizontal* (experience/warning)
    //   - barPurple_horizontal* (magic/special)
    // ═══════════════════════════════════════════════════════════════════════
    
    float healthPercent = static_cast<float>(player.getHealth()) / player.getMaxHealth();
    float barWidth = 200.f;
    float barHeight = 20.f;  // Reduced from default
    float barX = 15.f;
    float barY = 38.f;  // Adjusted for better spacing
    
    // Use simple rectangle bars instead of textures
    // Background bar (dark grey)
    sf::RectangleShape healthBarBgRect(sf::Vector2f(barWidth, barHeight));
    healthBarBgRect.setPosition(sf::Vector2f(barX, barY));
    healthBarBgRect.setFillColor(sf::Color(50, 50, 50));
    healthBarBgRect.setOutlineColor(sf::Color(30, 30, 30));
    healthBarBgRect.setOutlineThickness(2.f);
    window.draw(healthBarBgRect);
    
    // Filled health bar (red)
    if (healthPercent > 0.f) {
        sf::RectangleShape healthBarRect(sf::Vector2f(barWidth * healthPercent, barHeight));
        healthBarRect.setPosition(sf::Vector2f(barX, barY));
        healthBarRect.setFillColor(sf::Color(220, 50, 50));
        window.draw(healthBarRect);
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // 💙 XP/MANA BAR (BLUE) - Simple rectangle version
    // ═══════════════════════════════════════════════════════════════════════
    
    float xpBarX = 230.f;
    float xpBarY = 48.f;  // Increased spacing to avoid overlap with LVL text
    float xpBarWidth = 150.f;
    float xpBarHeight = 15.f;
    
    // Background for XP bar
    sf::RectangleShape xpBgRect(sf::Vector2f(xpBarWidth, xpBarHeight));
    xpBgRect.setPosition(sf::Vector2f(xpBarX, xpBarY));
    xpBgRect.setFillColor(sf::Color(50, 50, 50));
    xpBgRect.setOutlineColor(sf::Color(30, 30, 30));
    xpBgRect.setOutlineThickness(2.f);
    window.draw(xpBgRect);
    
    // Filled XP bar (blue)
    int xpForNextLevel = (player.getLevel() + 1) * 100;
    float xpPercent = static_cast<float>(player.getExperience() % xpForNextLevel) / xpForNextLevel;
    
    if (xpPercent > 0.f) {
        sf::RectangleShape xpBarRect(sf::Vector2f(xpBarWidth * xpPercent, xpBarHeight));
        xpBarRect.setPosition(sf::Vector2f(xpBarX, xpBarY));
        xpBarRect.setFillColor(sf::Color(50, 150, 255));
        window.draw(xpBarRect);
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // 📊 HUD TEXT LABELS
    // These display player stats like HP, Level, XP, Floor number
    // Font: currently loading from "assets/fonts/Arial.ttf" or similar
    // ═══════════════════════════════════════════════════════════════════════
    
    if (fontLoaded) {
        // Health text (top-left) - shows current/max HP
        sf::Text healthText(font);
        healthText.setCharacterSize(15);
        std::stringstream ss;
        ss << "HP " << player.getHealth() << "/" << player.getMaxHealth();
        healthText.setString(ss.str());
        healthText.setPosition(sf::Vector2f({18.f, 12.f}));  // Aligned with bar below
        healthText.setFillColor(sf::Color::White);
        healthText.setOutlineThickness(1.f);
        healthText.setOutlineColor(sf::Color::Black);
        window.draw(healthText);
        
        // Gold text (below health bar)
        sf::Text goldText(font);
        goldText.setCharacterSize(14);
        goldText.setString("Gold: " + std::to_string(player.getGold()));
        goldText.setPosition(sf::Vector2f({18.f, 62.f}));
        goldText.setFillColor(sf::Color(255, 215, 0));  // Golden color
        goldText.setOutlineThickness(1.f);
        goldText.setOutlineColor(sf::Color::Black);
        window.draw(goldText);
        
        // Level text (top-middle, well above XP bar)
        sf::Text levelText(font);
        levelText.setCharacterSize(18);
        levelText.setString("LVL " + std::to_string(player.getLevel()));
        levelText.setPosition(sf::Vector2f({265.f, 10.f}));  // Higher up, more separation
        levelText.setFillColor(sf::Color(200, 220, 255));
        levelText.setOutlineThickness(1.f);
        levelText.setOutlineColor(sf::Color::Black);
        window.draw(levelText);
        
        // XP text label (small, above XP bar)
        sf::Text xpLabel(font);
        xpLabel.setCharacterSize(10);
        xpLabel.setString("XP");
        xpLabel.setPosition(sf::Vector2f({265.f, 37.f}));
        xpLabel.setFillColor(sf::Color(150, 150, 200));
        xpLabel.setOutlineThickness(0.5f);
        xpLabel.setOutlineColor(sf::Color::Black);
        window.draw(xpLabel);
        
        // Floor text (top-right corner, inside HUD panel)
        sf::Text floorText(font);
        floorText.setCharacterSize(18);
        floorText.setString("Floor " + std::to_string(currentFloor));
        // Right-align by calculating width (SFML 3.x)
        sf::FloatRect bounds = floorText.getGlobalBounds();
        floorText.setPosition(sf::Vector2f(window.getSize().x - bounds.size.x - 20.f, 12.f));
        floorText.setFillColor(sf::Color(255, 215, 100));
        floorText.setOutlineThickness(1.f);
        floorText.setOutlineColor(sf::Color::Black);
        window.draw(floorText);
        
        // XP value text
        sf::Text xpText(font);
        xpText.setCharacterSize(10);
        xpText.setString("XP: " + std::to_string(player.getExperience()));
        xpText.setPosition(sf::Vector2f({390.f, 50.f}));
        xpText.setFillColor(sf::Color(180, 180, 220));
        window.draw(xpText);
        
        // Mana display (below Gold)
        sf::Text manaText(font);
        manaText.setCharacterSize(14);
        std::stringstream manaStream;
        manaStream << "MP " << player.getMana() << "/" << player.getMaxMana();
        manaText.setString(manaStream.str());
        manaText.setPosition(sf::Vector2f({120.f, 62.f}));
        manaText.setFillColor(sf::Color(100, 180, 255));  // Blue color
        manaText.setOutlineThickness(1.f);
        manaText.setOutlineColor(sf::Color::Black);
        window.draw(manaText);
        
        // Skill Points display (top-right, near level)
        if (skillTree) {
            sf::Text skillPointsText(font);
            skillPointsText.setCharacterSize(16);
            skillPointsText.setString("SP: " + std::to_string(skillTree->getAvailablePoints()));
            skillPointsText.setPosition(sf::Vector2f({340.f, 12.f}));
            
            // Highlight if player has unspent skill points
            if (skillTree->getAvailablePoints() > 0) {
                skillPointsText.setFillColor(sf::Color(255, 255, 100));  // Bright yellow
                skillPointsText.setStyle(sf::Text::Bold);
            } else {
                skillPointsText.setFillColor(sf::Color(180, 180, 200));
            }
            skillPointsText.setOutlineThickness(1.f);
            skillPointsText.setOutlineColor(sf::Color::Black);
            window.draw(skillPointsText);
            
            // "Press T" hint if points available
            if (skillTree->getAvailablePoints() > 0) {
                sf::Text skillHint(font);
                skillHint.setCharacterSize(10);
                skillHint.setString("(Press T)");
                skillHint.setPosition(sf::Vector2f({340.f, 32.f}));
                skillHint.setFillColor(sf::Color(200, 200, 100));
                window.draw(skillHint);
            }
        }
        
        // Controls hint (bottom of screen)
        sf::Text controls(font);
        controls.setCharacterSize(9);
        controls.setString("I:Inv T:Skills O:Unlock Space:Attack 1-5:Skill U:UseItem E:Interact Z:Back");
        controls.setPosition(sf::Vector2f({10.f, window.getSize().y - 25.f}));
        controls.setFillColor(sf::Color(160, 160, 180));
        window.draw(controls);
    }
}

void UIManager::renderMiniMap(sf::RenderWindow& window, const Dungeon& dungeon, const Player& player, const EnemyManager& enemies) {
    // ═══════════════════════════════════════════════════════════════════════
    // 🗺️ MINI-MAP PANEL
    // Shows dungeon rooms, player position (cyan), and enemy positions (red)
    // Uses actual dungeon room data for accurate map representation
    // ═══════════════════════════════════════════════════════════════════════
    
    // Mini-map background - darker for better contrast
    sf::RectangleShape miniMapBg({170.f, 170.f});
    miniMapBg.setPosition(sf::Vector2f({615.f, 415.f}));
    miniMapBg.setFillColor(sf::Color(30, 30, 30, 240));
    miniMapBg.setOutlineThickness(2.0f);
    miniMapBg.setOutlineColor(sf::Color(100, 100, 120));
    window.draw(miniMapBg);
    
    // Title
    if (fontLoaded) {
        sf::Text title(font);
        title.setCharacterSize(12);
        title.setString("MAP");
        title.setPosition(sf::Vector2f({680.f, 420.f}));
        title.setFillColor(sf::Color(200, 200, 220));
        window.draw(title);
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // MINI-MAP: RENDER ACTUAL DUNGEON ROOMS
    // Green = walkable rooms, Dark = unexplored/blocked areas
    // ═══════════════════════════════════════════════════════════════════════
    
    float scale = 0.6f;  // Scale factor for fitting dungeon on mini-map
    float offsetX = 625.f;
    float offsetY = 445.f;
    
    // CHANGE: 2025-11-14 - Render actual room data instead of generic grid
    const auto& rooms = dungeon.getRooms();
    for (const auto& room : rooms) {
        sf::RectangleShape roomRect(sf::Vector2f(room.width * scale - 1.f, room.height * scale - 1.f));
        roomRect.setPosition(sf::Vector2f(offsetX + room.x * scale, offsetY + room.y * scale));
        roomRect.setFillColor(sf::Color(70, 120, 70));  // Green for rooms
        roomRect.setOutlineThickness(1.0f);
        roomRect.setOutlineColor(sf::Color(100, 150, 100));
        window.draw(roomRect);
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // MINI-MAP MARKERS
    // Red dots = enemies
    // Cyan dots = player (with white outline for extra visibility)
    // ═══════════════════════════════════════════════════════════════════════
    
    // Draw enemy positions - bright red for visibility
    const auto& enemyList = enemies.getEnemies();
    for (const auto& enemy : enemyList) {
        sf::CircleShape enemyDot(2.f);
        enemyDot.setPosition(sf::Vector2f(offsetX + enemy.x * scale - 2.f, 
                                          offsetY + enemy.y * scale - 2.f));
        enemyDot.setFillColor(sf::Color(255, 50, 50));
        window.draw(enemyDot);
    }
    
    // Draw player position on mini-map - bright cyan for high visibility (ON TOP)
    float playerX = offsetX + player.getPosition().x * scale - 3.f;
    float playerY = offsetY + player.getPosition().y * scale - 3.f;
    sf::CircleShape playerDot(3.f);
    playerDot.setPosition(sf::Vector2f(playerX, playerY));
    playerDot.setFillColor(sf::Color(0, 255, 255));
    playerDot.setOutlineThickness(1.f);
    playerDot.setOutlineColor(sf::Color(255, 255, 255, 180));
    window.draw(playerDot);
}

void UIManager::renderInventoryPanel(sf::RenderWindow& window, const Player& player) {
    // Inventory panel background
    sf::RectangleShape invBg({350.f, 450.f});
    invBg.setPosition(sf::Vector2f({225.f, 75.f}));
    invBg.setFillColor(sf::Color(25, 25, 35, 240));
    invBg.setOutlineThickness(3.0f);
    invBg.setOutlineColor(sf::Color(100, 100, 150));
    window.draw(invBg);
    
    if (fontLoaded) {
        // Title
        sf::Text title(font);
        title.setCharacterSize(24);
        title.setString("=== INVENTORY ===");
        title.setPosition(sf::Vector2f({260.f, 90.f}));
        title.setFillColor(sf::Color(200, 200, 255));
        window.draw(title);
        
        // Items list
        float yOffset = 140.f;
        int itemCount = 0;
        
        // Display new inventory system (ItemNew)
        player.getInventoryNew().traverse([&](const ItemNew& item) {
            // CHANGE: 2025-11-14 - Removed console spam debug output from render loop
            // Item background
            sf::RectangleShape itemBg({320.f, 35.f});
            itemBg.setPosition(sf::Vector2f(240, yOffset));
            itemBg.setFillColor(sf::Color(40, 40, 55));
            itemBg.setOutlineThickness(1.0f);
            itemBg.setOutlineColor(sf::Color(60, 60, 80));
            window.draw(itemBg);
            
            // Item name with rarity color
            sf::Text itemName(font);
            itemName.setCharacterSize(14);
            itemName.setString(item.name);
            itemName.setPosition(sf::Vector2f(250, yOffset + 5));
            itemName.setFillColor(item.getRarityColor());
            window.draw(itemName);
            
            // Item type and rarity
            sf::Text itemType(font);
            itemType.setCharacterSize(10);
            itemType.setString("[" + item.type + " - " + item.getRarityName() + "]");
            itemType.setPosition(sf::Vector2f(250, yOffset + 20));
            itemType.setFillColor(sf::Color(150, 150, 180));
            window.draw(itemType);
            
            // Item value
            sf::Text itemValue(font);
            itemValue.setCharacterSize(12);
            itemValue.setString("$" + std::to_string(item.value));
            itemValue.setPosition(sf::Vector2f(490, yOffset + 10));
            itemValue.setFillColor(sf::Color(255, 215, 0));
            window.draw(itemValue);
            
            yOffset += 40.f;
            itemCount++;
        });
        
        // CHANGE: 2025-11-14 - Only display ItemNew system (old Item system removed)
        // Old inventory system commented out - using unified ItemNew only
        /* DEPRECATED: Old Item system
        player.getInventory().traverse([&](const Item& item) {
            // ... old rendering code ...
        });
        */
        
        if (itemCount == 0) {
            sf::Text emptyText(font);
            emptyText.setCharacterSize(14);
            emptyText.setString("(Empty inventory)");
            emptyText.setPosition(sf::Vector2f({310.f, 250.f}));
            emptyText.setFillColor(sf::Color(120, 120, 140));
            window.draw(emptyText);
        }
        
        // Close instruction
        sf::Text closeText(font);
        closeText.setCharacterSize(12);
        closeText.setString("Press I to close | Press U to use first item");
        closeText.setPosition(sf::Vector2f({250.f, 490.f}));
        closeText.setFillColor(sf::Color(140, 140, 160));
        window.draw(closeText);
    }
}

void UIManager::renderSkillTreePanel(sf::RenderWindow& window) {
    // Skill tree panel background
    sf::RectangleShape treeBg({550.f, 480.f});
    treeBg.setPosition(sf::Vector2f({125.f, 60.f}));
    treeBg.setFillColor(sf::Color(20, 25, 35, 245));
    treeBg.setOutlineThickness(3.0f);
    treeBg.setOutlineColor(sf::Color(100, 120, 150));
    window.draw(treeBg);
    
    if (!fontLoaded) return;
    
    // Title
    sf::Text title(font);
    title.setCharacterSize(26);
    title.setString("=== SKILL TREE ===");
    title.setPosition(sf::Vector2f({240.f, 75.f}));
    title.setFillColor(sf::Color(150, 200, 255));
    window.draw(title);
    
    // Instructions panel
    sf::RectangleShape instructionBg(sf::Vector2f({510.f, 380.f}));
    instructionBg.setPosition(sf::Vector2f({145.f, 115.f}));
    instructionBg.setFillColor(sf::Color(15, 20, 30, 220));
    instructionBg.setOutlineThickness(2.0f);
    instructionBg.setOutlineColor(sf::Color(80, 100, 130));
    window.draw(instructionBg);
    
    // Skill information
    sf::Text infoText(font);
    infoText.setCharacterSize(13);
    infoText.setString(
        "SKILL TREE - Binary Tree Structure\n\n"
        "Available Skills (Press 1-5 to use unlocked skills):\n"
        "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
        "TIER 1 - ROOT (Always Unlocked):\n"
        "  [1] Slash - Basic sword attack\n"
        "      Damage: 15  |  Mana: 5  |  Cooldown: None\n\n"
        
        "TIER 2 - WARRIOR PATH:\n"
        "  [2] Power Strike - Heavy attack\n"
        "      Damage: 35  |  Mana: 15  |  Cooldown: 2 turns\n"
        "      Requires: Slash + 1 skill point\n\n"
        
        "TIER 3 - AOE PATH:\n"
        "  [3] Whirlwind - Spin attack (hits adjacent enemies)\n"
        "      Damage: 30 AOE  |  Mana: 25  |  Cooldown: 3 turns\n"
        "      Requires: Power Strike + 2 skill points\n\n"
        
        "TIER 4 - ADVANCED:\n"
        "  [4] Flame Wave - Fire AOE with burn effect\n"
        "      Damage: 25 + burn  |  Mana: 35  |  Cooldown: 5 turns\n"
        "      Requires: Whirlwind + 3 skill points\n\n"
        
        "TIER 2 - MOBILITY:\n"
        "  [5] Shadow Step - Teleport dash\n"
        "      Range: 3 tiles  |  Mana: 20  |  Cooldown: 4 turns\n"
        "      Requires: Slash + 2 skill points\n\n"
        "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
        "Earn skill points by leveling up (defeat enemies)!\n"
        "Unlocked skills appear in hotkey bar at bottom."
    );
    infoText.setPosition(sf::Vector2f({155.f, 125.f}));
    infoText.setFillColor(sf::Color(200, 210, 230));
    infoText.setLineSpacing(1.15f);
    window.draw(infoText);
    
    // Close instruction
    sf::Text closeText(font);
    closeText.setCharacterSize(12);
    closeText.setString("Press T to close  |  Press O to unlock next skill  |  Level up to earn points");
    closeText.setPosition(sf::Vector2f({145.f, 505.f}));
    closeText.setFillColor(sf::Color(140, 200, 160));
    window.draw(closeText);
}

void UIManager::renderTurnQueue(sf::RenderWindow& window, const EnemyManager& enemies) {
    if (enemies.isEmpty()) return;
    
    // ═══════════════════════════════════════════════════════════════════════
    // 🔄 TURN ORDER PANEL - Shows upcoming combat turns
    // Background: Dark red panel (40, 20, 20, 220) for combat feel
    // Shows first 6 enemies in turn order as red circles with names below
    // ═══════════════════════════════════════════════════════════════════════
    
    // Turn queue panel background - dark red for combat theme
    float panelWidth = 200.f;
    float panelHeight = 90.f;
    sf::RectangleShape queueBg({panelWidth, panelHeight});
    queueBg.setPosition(sf::Vector2f({10.f, 495.f}));
    queueBg.setFillColor(sf::Color(40, 20, 20, 220));  // Dark red
    queueBg.setOutlineThickness(2.0f);
    queueBg.setOutlineColor(sf::Color(120, 60, 60));
    window.draw(queueBg);
    
    if (fontLoaded) {
        // Title
        sf::Text title(font);
        title.setCharacterSize(12);
        title.setString("TURN ORDER:");
        title.setPosition(sf::Vector2f({15.f, 500.f}));
        title.setFillColor(sf::Color(255, 180, 180));
        window.draw(title);
        
        // ═══════════════════════════════════════════════════════════════════
        // ENEMY TURN INDICATORS (Red circles = enemy turns coming up)
        // ═══════════════════════════════════════════════════════════════════
        
        // Draw enemy icons - CENTERED dynamically
        const auto& enemyList = enemies.getEnemies();
        size_t displayCount = std::min(enemyList.size(), size_t(6));  // Cap at 6 visible
        
        if (displayCount > 0) {
            float iconSpacing = 50.f;
            float totalWidth = displayCount * iconSpacing;
            float startX = 10.f + (panelWidth - totalWidth) / 2.0f;  // Center calculation
            
            for (size_t i = 0; i < displayCount; i++) {
                float iconX = startX + i * iconSpacing;
                
                // Red circle = enemy turn
                sf::CircleShape enemyIcon(12.f);
                enemyIcon.setPosition(sf::Vector2f(iconX, 530.f));
                enemyIcon.setFillColor(sf::Color(200, 50, 50));
                enemyIcon.setOutlineThickness(2.0f);
                enemyIcon.setOutlineColor(sf::Color(100, 0, 0));
                window.draw(enemyIcon);
                
                // Enemy name (CHANGE: 2025-11-14 - Cache substring to avoid repeated string ops)
                sf::Text enemyName(font);
                enemyName.setCharacterSize(8);
                const std::string& fullName = enemyList[i].name;
                std::string shortName = fullName.length() > 5 ? fullName.substr(0, 5) : fullName;
                enemyName.setString(shortName);
                
                // Center text under icon
                sf::FloatRect textBounds = enemyName.getLocalBounds();
                enemyName.setPosition(sf::Vector2f(iconX + 12.f - textBounds.size.x / 2.f, 558.f));
                enemyName.setFillColor(sf::Color(220, 220, 220));
                window.draw(enemyName);
            }
        }
    }
}

void UIManager::renderSkillHotkeys(sf::RenderWindow& window, const Player& player, SkillTree* skillTree) {
    if (!skillTree) return;
    
    // Enhanced hotbar - bottom center of screen
    // Shows: [Q-Weapon] [W-Armor] [1-Skill] [2-Skill] [3-Skill] [4-Skill] [5-Skill]
    float panelWidth = 500.0f;
    float panelHeight = 70.0f;
    float panelX = (800.0f - panelWidth) / 2.0f;
    float panelY = 535.0f;
    
    // Background panel
    sf::RectangleShape panel(sf::Vector2f(panelWidth, panelHeight));
    panel.setPosition(sf::Vector2f(panelX, panelY));
    panel.setFillColor(sf::Color(20, 20, 30, 220));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(100, 100, 120));
    window.draw(panel);
    
    // Title
    sf::Text title(font);
    title.setCharacterSize(10);
    title.setString("HOTBAR - Items & Skills");
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(sf::Vector2f(panelX + panelWidth / 2.0f - titleBounds.size.x / 2.0f, panelY + 2.0f));
    title.setFillColor(sf::Color(150, 150, 170));
    window.draw(title);
    
    float slotSize = 50.0f;
    float spacing = 58.0f;
    float startX = panelX + 10.0f;
    float slotY = panelY + 15.0f;
    
    // Get all unlocked active skills
    std::vector<Skill*> activeSkills = skillTree->getActiveSkills();
    
    // Draw 7 slots: Q(Weapon), W(Armor), 1-5(Skills)
    // CHANGE: 2025-11-14 - Use static for unchanging slot data, pre-calculate positions
    static const std::vector<std::string> slotKeys = {"Q", "W", "1", "2", "3", "4", "5"};
    static const std::vector<std::string> slotLabels = {"Weapon", "Armor", "", "", "", "", ""};
    
    for (int i = 0; i < 7; i++) {
        float slotX = startX + i * spacing;
        
        // Slot background
        sf::RectangleShape slot(sf::Vector2f(slotSize, slotSize));
        slot.setPosition(sf::Vector2f(slotX, slotY));
        slot.setFillColor(sf::Color(35, 35, 45, 220));
        slot.setOutlineThickness(2.0f);
        slot.setOutlineColor(sf::Color(70, 70, 80));
        window.draw(slot);
        
        // Key label (top-left) - CHANGE: 2025-11-14 - Cache text bounds
        sf::Text keyText(font);
        keyText.setCharacterSize(11);
        keyText.setString(slotKeys[i]);
        keyText.setPosition(sf::Vector2f(slotX + 3.0f, slotY + 2.0f));
        keyText.setFillColor(sf::Color(180, 180, 200));
        keyText.setStyle(sf::Text::Bold);
        window.draw(keyText);
        
        // Handle equipment slots (Q, W)
        if (i < 2) {
            sf::Text itemName(font);
            itemName.setCharacterSize(8);
            itemName.setString(slotLabels[i]);
            sf::FloatRect nameBounds = itemName.getLocalBounds();
            itemName.setPosition(sf::Vector2f(slotX + slotSize / 2.0f - nameBounds.size.x / 2.0f, 
                                              slotY + slotSize / 2.0f - 2.0f));
            itemName.setFillColor(sf::Color(140, 140, 150));
            window.draw(itemName);
            
            // Show equipped status
            sf::Text equipped(font);
            equipped.setCharacterSize(7);
            equipped.setString("Empty");
            equipped.setPosition(sf::Vector2f(slotX + 4.0f, slotY + slotSize - 10.0f));
            equipped.setFillColor(sf::Color(100, 100, 100));
            window.draw(equipped);
        }
        // Handle skill slots (1-5) - CHANGE: 2025-11-14 - Bounds-check before access
        else {
            int skillHotkey = i - 1;  // Convert to 1-5
            // Verify activeSkills has enough elements and index is valid before accessing
            if (skillHotkey >= 0 && static_cast<size_t>(skillHotkey) < activeSkills.size()) {
                Skill* skill = skillTree->getSkillByHotkey(skillHotkey);
            
            if (skill && skill->unlocked) {
                bool isAvailable = skill->currentCooldown == 0 && player.getMana() >= skill->manaCost;
                
                // Highlight if available
                if (isAvailable) {
                    slot.setFillColor(sf::Color(60, 90, 140, 240));
                    slot.setOutlineColor(sf::Color(100, 150, 255));
                    window.draw(slot);  // Redraw with highlight
                }
                
                // Skill name (centered)
                sf::Text skillName(font);
                skillName.setCharacterSize(9);
                std::string shortName = skill->name.length() > 7 ? skill->name.substr(0, 7) : skill->name;
                skillName.setString(shortName);
                sf::FloatRect nameBounds = skillName.getLocalBounds();
                skillName.setPosition(sf::Vector2f(slotX + slotSize / 2.0f - nameBounds.size.x / 2.0f, 
                                                   slotY + 18.0f));
                skillName.setFillColor(isAvailable ? sf::Color(255, 255, 255) : sf::Color(120, 120, 120));
                window.draw(skillName);
                
                // Mana cost (bottom)
                sf::Text manaCost(font);
                manaCost.setCharacterSize(8);
                manaCost.setString(std::to_string(skill->manaCost) + "MP");
                sf::FloatRect manaBounds = manaCost.getLocalBounds();
                manaCost.setPosition(sf::Vector2f(slotX + slotSize / 2.0f - manaBounds.size.x / 2.0f, 
                                                  slotY + slotSize - 10.0f));
                manaCost.setFillColor(player.getMana() >= skill->manaCost ? 
                                     sf::Color(100, 200, 255) : sf::Color(255, 100, 100));
                window.draw(manaCost);
                
                // Cooldown overlay
                if (skill->currentCooldown > 0) {
                    sf::RectangleShape cooldownOverlay(sf::Vector2f(slotSize, slotSize));
                    cooldownOverlay.setPosition(sf::Vector2f(slotX, slotY));
                    cooldownOverlay.setFillColor(sf::Color(0, 0, 0, 180));
                    window.draw(cooldownOverlay);
                    
                    sf::Text cooldownText(font);
                    cooldownText.setCharacterSize(18);
                    cooldownText.setString(std::to_string(skill->currentCooldown));
                    sf::FloatRect cdBounds = cooldownText.getLocalBounds();
                    cooldownText.setPosition(sf::Vector2f(slotX + slotSize / 2.0f - cdBounds.size.x / 2.0f, 
                                                          slotY + slotSize / 2.0f - cdBounds.size.y / 2.0f - 5.0f));
                    cooldownText.setFillColor(sf::Color(255, 200, 50));
                    cooldownText.setStyle(sf::Text::Bold);
                    window.draw(cooldownText);
                }
            } else {
                // Empty skill slot
                sf::Text emptyText(font);
                emptyText.setCharacterSize(11);
                emptyText.setString("---");
                sf::FloatRect emptyBounds = emptyText.getLocalBounds();
                emptyText.setPosition(sf::Vector2f(slotX + slotSize / 2.0f - emptyBounds.size.x / 2.0f, 
                                                   slotY + slotSize / 2.0f - emptyBounds.size.y / 2.0f - 2.0f));
                emptyText.setFillColor(sf::Color(80, 80, 80));
                window.draw(emptyText);
                
                sf::Text lockText(font);
                lockText.setCharacterSize(7);
                lockText.setString("Locked");
                sf::FloatRect lockBounds = lockText.getLocalBounds();
                lockText.setPosition(sf::Vector2f(slotX + slotSize / 2.0f - lockBounds.size.x / 2.0f, 
                                                  slotY + slotSize - 10.0f));
                lockText.setFillColor(sf::Color(100, 100, 100));
                window.draw(lockText);
            }
            }  // Close bounds-check if statement
        }
    }
}

void UIManager::showMainMenu() {
    std::cout << "[UIManager] Main menu displayed" << std::endl;
}

void UIManager::showHUD() {
    inventoryVisible = false;
    skillTreeVisible = false;
    std::cout << "[UIManager] HUD displayed" << std::endl;
}

void UIManager::showInventory() {
    inventoryVisible = true;
    skillTreeVisible = false;
}

void UIManager::showSkillTree() {
    inventoryVisible = false;
    skillTreeVisible = true;
}

void UIManager::hideAll() {
    inventoryVisible = false;
    skillTreeVisible = false;
}

void UIManager::toggleInventory() {
    inventoryVisible = !inventoryVisible;
    if (inventoryVisible) skillTreeVisible = false;
}

void UIManager::toggleSkillTree() {
    skillTreeVisible = !skillTreeVisible;
    if (skillTreeVisible) inventoryVisible = false;
}

void UIManager::toggleMiniMap() {
    miniMapVisible = !miniMapVisible;
}

void UIManager::updateHUD(const Player& player) {
    float healthPercent = static_cast<float>(player.getHealth()) / player.getMaxHealth();
    healthBarFg.setSize({200.f * healthPercent, 20.f});
}

void UIManager::updateInventory(const Player& player) {
    // Update handled in render
}

bool UIManager::loadFont(const std::string& path) {
    fontLoaded = font.openFromFile(path);
    if (fontLoaded) {
        std::cout << "[UIManager] Font loaded: " << path << std::endl;
        // Pass font to DSA Visualizer
        if (dsaViz) {
            dsaViz->setFont(&font);
        }
    } else {
        std::cout << "[UIManager] Could not load font: " << path << " (continuing without)" << std::endl;
    }
    return fontLoaded;
}
