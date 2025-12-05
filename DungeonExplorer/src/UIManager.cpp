// CHANGE: 2025-12-04 - Added GameUtils for consolidated utilities
#include "UIManager.h"
#include "Game.h"
#include "Player.h"
#include "Dungeon.h"
#include "Enemy.h"
#include "SkillTree.h"
#include "InventoryRenderer.h"
#include "DSAVisualizer.h"
#include "SoundManager.h"
#include "AssetManager.h"
#include "UIVisualEffects.h"
#include "GameUtils.h"
#include <iostream>
#include <sstream>
#include <iomanip>

UIManager::UIManager(Game* game) : 
      game(game),
      fontLoaded(false), inventoryVisible(false), 
      skillTreeVisible(false), miniMapVisible(true), animationTime(0.f),
      selectedInventoryIndex(-1), hoveredInventoryIndex(-1),
      screenFlashTimer(0.f), screenFlashColor(sf::Color::Transparent),
      healthBarProgress(1.0f), xpBarProgress(0.0f) {
    dsaViz = std::make_unique<DSAVisualizer>();
    invRenderer = std::make_unique<InventoryRenderer>();
}

void UIManager::initialize() {
    std::cout << "[UIManager] Initializing enhanced SFML UI..." << std::endl;
    
    if (!font.openFromFile("assets/fonts/Roboto-Regular.ttf")) {
        // Try fallback
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
             std::cerr << "[UIManager] Failed to load font!" << std::endl;
        } else {
            fontLoaded = true;
        }
    } else {
        fontLoaded = true;
    }
    
    if (fontLoaded) {
        dsaViz->setFont(&font);
    }
    
    // HUD background bar
    hudBackground.setSize({800.f, 75.f});
    hudBackground.setPosition(sf::Vector2f({0.f, 0.f}));
    hudBackground.setFillColor(sf::Color(20, 20, 30, 210));
    
    // Health bar
    healthBarBg.setSize({220.f, 22.f});
    healthBarBg.setPosition(sf::Vector2f({15.f, 42.f}));
    healthBarBg.setFillColor(sf::Color(50, 30, 30));
    
    healthBarFg.setSize({220.f, 22.f});
    healthBarFg.setPosition(sf::Vector2f({15.f, 42.f}));
    healthBarFg.setFillColor(sf::Color(200, 50, 50));
    
    // XP bar
    xpBarBg.setSize({180.f, 14.f});
    xpBarBg.setPosition(sf::Vector2f({260.f, 50.f}));
    xpBarBg.setFillColor(sf::Color(30, 30, 50));
    
    xpBarFg.setSize({180.f, 14.f});
    xpBarFg.setPosition(sf::Vector2f({260.f, 50.f}));
    xpBarFg.setFillColor(sf::Color(100, 100, 255));
    
    std::cout << "[UIManager] UI initialized" << std::endl;
}

void UIManager::update(float deltaTime) {
    animationTime += deltaTime;
    
    // Update combo display timer
    if (comboDisplayTimer > 0.f) {
        comboDisplayTimer -= deltaTime;
        if (comboDisplayTimer <= 0.f) {
            currentCombo = 0;
        }
    }
    
    // OPTIMIZATION: Update floating texts with physics-based movement
    for (auto it = floatingTexts.begin(); it != floatingTexts.end();) {
        it->lifetime -= deltaTime;
        if (it->lifetime <= 0.0f) {
            it = floatingTexts.erase(it);
        } else {
            // Smooth deceleration as text fades
            float speedMultiplier = (it->lifetime / it->maxLifetime);
            it->position.y -= 30.0f * deltaTime * (1.0f + speedMultiplier);
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

void UIManager::updateMousePosition(sf::Vector2f mousePos) {
    lastMousePos = mousePos;
}

bool UIManager::handleMouseClick(sf::Vector2f mousePos) {
    SoundManager::getInstance().playSound("ui_click", 1.0f, 0.5f);

    if (skillTreeVisible) {
        if (game && game->getSkillTree()) {
            bool unlocked = game->getSkillTree()->tryUnlockAt(mousePos);
            if (unlocked) {
                SoundManager::getInstance().playSound("levelup", 1.2f, 0.6f);
            }
            return true;
        }
    }
    
    if (inventoryVisible) {
        if (hoveredInventoryIndex != -1) {
            if (swapSourceIndex == -1) {
                swapSourceIndex = hoveredInventoryIndex;
                selectedInventoryIndex = hoveredInventoryIndex;
            } else {
                if (swapSourceIndex == hoveredInventoryIndex) {
                    swapSourceIndex = -1;
                    selectedInventoryIndex = -1;
                } else {
                    if (game && game->getPlayer()) {
                        game->getPlayer()->swapInventoryItems(swapSourceIndex, hoveredInventoryIndex);
                        swapSourceIndex = -1;
                        selectedInventoryIndex = hoveredInventoryIndex;
                    }
                }
            }
            return true;
        } else {
            swapSourceIndex = -1;
            selectedInventoryIndex = -1;
        }
    }
    
    return false;
}

void UIManager::render(sf::RenderWindow& window, const Player& player, 
                        const Dungeon& dungeon, const EnemyManager& enemies, int currentFloor) {
    renderUI(window, player, dungeon, enemies, nullptr, currentFloor);
}

void UIManager::renderUI(sf::RenderWindow& window, const Player& player, 
                          const Dungeon& dungeon, const EnemyManager& enemies, SkillTree* skillTree, int currentFloor) {
    if (dsaViz) {
        dsaViz->renderStackTrail(window, player, TILE_SIZE);
        if (dungeon.getRooms().size() > 2) {
            const auto& room = dungeon.getRooms()[2];
            float lootX = (room.x + room.width / 2.f) * TILE_SIZE;
            float lootY = (room.y + room.height / 2.f) * TILE_SIZE;
            dsaViz->renderHeapGlow(window, sf::Vector2f(lootX, lootY), 100);
        }
    }
    
    renderHUD(window, player, skillTree, currentFloor);
    
    if (miniMapVisible) {
        renderMiniMap(window, dungeon, player, enemies);
    }
    
    if (inventoryVisible) {
        renderInventoryPanel(window, player);
    }
    
    if (skillTreeVisible) {
        renderSkillTreePanel(window);
    }
    
    renderTurnQueue(window, enemies);
    renderSkillHotkeys(window, player, skillTree);
    renderCharacterNames(window, player, enemies, TILE_SIZE);
    renderFloatingTexts(window);
    renderComboCounter(window);  // 🎮 Combo display
    renderScreenFlash(window);
}

void UIManager::renderGraphPathsOnly(sf::RenderWindow& window, const Dungeon& dungeon) {
    if (dsaViz) {
        dsaViz->renderGraphPaths(window, dungeon, TILE_SIZE);
    }
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
        // OPTIMIZATION: Smooth fade with easing
        float normalizedLife = floatText.lifetime / floatText.maxLifetime;
        float alpha = normalizedLife * 255.0f;
        
        // Add slight ease-out for smoother fade
        alpha = alpha * (2.0f - normalizedLife);
        alpha = std::min(255.0f, std::max(0.0f, alpha));
        
        sf::Color fadeColor = floatText.color;
        fadeColor.a = static_cast<std::uint8_t>(alpha);
        
        sf::Text text(font, floatText.text, 20);
        text.setPosition(floatText.position);
        text.setFillColor(fadeColor);
        
        // OPTIMIZATION: Dynamic outline based on alpha for better visibility
        float outlineAlpha = std::min(alpha * 1.2f, 255.0f);
        text.setOutlineThickness(2.0f);
        text.setOutlineColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(outlineAlpha)));
        
        window.draw(text);
    }
}

void UIManager::renderScreenFlash(sf::RenderWindow& window) {
    if (screenFlashTimer <= 0.0f) return;
    
    // OPTIMIZATION: Smooth fade with exponential decay
    float normalizedTime = screenFlashTimer / 0.3f;
    float alpha = normalizedTime * normalizedTime * 150.0f;  // Quadratic fade
    
    sf::Color flashColor = screenFlashColor;
    flashColor.a = static_cast<std::uint8_t>(std::min(alpha, 255.0f));
    
    sf::RectangleShape flash(sf::Vector2f(window.getSize().x, window.getSize().y));
    flash.setFillColor(flashColor);
    window.draw(flash);
}

void UIManager::renderCharacterNames(sf::RenderWindow& window, const Player& player, const EnemyManager& enemies, float tileSize) {
    if (!fontLoaded) return;
    
    Position playerPos = player.getPosition();
    float playerX = playerPos.x * tileSize + tileSize / 2.f;
    float playerY = playerPos.y * tileSize - tileSize / 3.f;
    
    sf::Text playerName(font, player.getName(), 11);
    playerName.setFillColor(sf::Color(100, 255, 100));
    playerName.setOutlineThickness(1.5f);
    playerName.setOutlineColor(sf::Color::Black);
    sf::FloatRect playerBounds = playerName.getGlobalBounds();
    playerName.setPosition(sf::Vector2f(playerX - playerBounds.size.x / 2.f, playerY));
    window.draw(playerName);
    
    const auto& enemyList = enemies.getEnemies();
    for (const auto& enemy : enemyList) {
        float enemyX = enemy.x * tileSize + tileSize / 2.f;
        float enemyY = enemy.y * tileSize - tileSize / 3.f;
        
        sf::Text enemyName(font, enemy.name, 10);
        if (enemy.type == "boss") {
            enemyName.setFillColor(sf::Color(255, 50, 50));
        } else if (enemy.type == "ranged") {
            enemyName.setFillColor(sf::Color(255, 200, 50));
        } else {
            enemyName.setFillColor(sf::Color(255, 150, 150));
        }
        enemyName.setOutlineThickness(1.5f);
        enemyName.setOutlineColor(sf::Color::Black);
        
        sf::FloatRect enemyBounds = enemyName.getGlobalBounds();
        enemyName.setPosition(sf::Vector2f(enemyX - enemyBounds.size.x / 2.f, enemyY));
        window.draw(enemyName);
    }
}

void UIManager::renderContextualPrompt(sf::RenderWindow& window, const std::string& prompt) {
    if (!fontLoaded || prompt.empty()) return;
    
    sf::Text promptText(font, prompt, 16);
    promptText.setFillColor(sf::Color(255, 255, 100));
    promptText.setOutlineThickness(2.f);
    promptText.setOutlineColor(sf::Color::Black);
    
    sf::FloatRect bounds = promptText.getGlobalBounds();
    float centerX = (window.getSize().x - bounds.size.x) / 2.f;
    float bottomY = window.getSize().y - 100.f;
    
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
    // MODERN HUD with glassmorphism panel
    sf::FloatRect hudRect(sf::Vector2f(5.f, 5.f), sf::Vector2f(400.f, 70.f));
    UIVisualEffects::drawStyledPanel(
        window,
        hudRect,
        UIVisualEffects::PanelStyle::Glass,
        sf::Color(40, 35, 30, 220),
        sf::Color(100, 80, 60)
    );
    
    // ANIMATED HEALTH BAR with gradient
    float healthPercent = static_cast<float>(player.getHealth()) / player.getMaxHealth();
    float barWidth = 200.f;
    float barHeight = 20.f;
    float barX = 15.f;
    float barY = 38.f;
    
    // Use animated bar with smooth transitions
    sf::Color healthGradStart = healthPercent > 0.3f ? sf::Color(220, 50, 50) : sf::Color(180, 30, 30);
    sf::Color healthGradEnd = healthPercent > 0.3f ? sf::Color(180, 20, 20) : sf::Color(120, 10, 10);
    
    sf::FloatRect healthBarRect(sf::Vector2f(barX, barY), sf::Vector2f(barWidth, barHeight));
    UIVisualEffects::drawAnimatedBar(
        window,
        healthBarRect,
        healthBarProgress,
        healthPercent,
        0.016f,  // ~60fps assumption
        healthGradStart,
        healthGradEnd,
        true  // animated
    );
    
    // ANIMATED XP BAR with blue gradient
    float xpBarX = 230.f;
    float xpBarY = 48.f;
    float xpBarWidth = 150.f;
    float xpBarHeight = 15.f;
    
    int xpForNextLevel = (player.getLevel() + 1) * 100;
    float xpPercent = static_cast<float>(player.getExperience() % xpForNextLevel) / xpForNextLevel;
    
    sf::FloatRect xpBarRect(sf::Vector2f(xpBarX, xpBarY), sf::Vector2f(xpBarWidth, xpBarHeight));
    UIVisualEffects::drawAnimatedBar(
        window,
        xpBarRect,
        xpBarProgress,
        xpPercent,
        0.016f,
        sf::Color(80, 160, 255),
        sf::Color(40, 100, 200),
        true
    );
    
    if (fontLoaded) {
        std::stringstream ss;
        ss << "HP " << player.getHealth() << "/" << player.getMaxHealth();
        window.draw(makeText(ss.str(), 15, {18.f, 12.f}));
        
        window.draw(makeText("Gold: " + std::to_string(player.getGold()), 14, {18.f, 62.f}, sf::Color(255, 215, 0)));
        window.draw(makeText("LVL " + std::to_string(player.getLevel()), 18, {265.f, 10.f}, sf::Color(200, 220, 255)));
        window.draw(makeText("XP", 10, {265.f, 37.f}, sf::Color(150, 150, 200), 0.5f));
        
        sf::Text floorText = makeText("Floor " + std::to_string(currentFloor), 18, {0.f, 12.f}, sf::Color(255, 215, 100));
        sf::FloatRect bounds = floorText.getGlobalBounds();
        floorText.setPosition(sf::Vector2f(window.getSize().x - bounds.size.x - 20.f, 12.f));
        window.draw(floorText);
        
        window.draw(makeText("XP: " + std::to_string(player.getExperience()), 10, {390.f, 50.f}, sf::Color(180, 180, 220), 0.f));
        
        std::stringstream manaStream;
        manaStream << "MP " << player.getMana() << "/" << player.getMaxMana();
        window.draw(makeText(manaStream.str(), 14, {120.f, 62.f}, sf::Color(100, 180, 255)));
        
        if (skillTree) {
            sf::Text spText = makeText("SP: " + std::to_string(skillTree->getAvailablePoints()), 16, {340.f, 12.f},
                                       skillTree->getAvailablePoints() > 0 ? sf::Color(255, 255, 100) : sf::Color(180, 180, 200));
            if (skillTree->getAvailablePoints() > 0) spText.setStyle(sf::Text::Bold);
            window.draw(spText);
            
            if (skillTree->getAvailablePoints() > 0) {
                window.draw(makeText("(Press T)", 10, {340.f, 32.f}, sf::Color(200, 200, 100), 0.f));
            }
        }
        
        window.draw(makeText("I:Inv T:Skills O:Unlock Space:Attack 1-5:Skill U:UseItem E:Interact Z:Back", 9, 
                             {10.f, window.getSize().y - 25.f}, sf::Color(160, 160, 180), 0.f));
    }
}

void UIManager::renderMiniMap(sf::RenderWindow& window, const Dungeon& dungeon, const Player& player, const EnemyManager& enemies) {
    // MODERN MINIMAP with shadow
    sf::FloatRect miniMapRect(sf::Vector2f(620.f, 420.f), sf::Vector2f(165.f, 165.f));
    UIVisualEffects::drawStyledPanel(
        window,
        miniMapRect,
        UIVisualEffects::PanelStyle::Bordered,
        sf::Color(25, 25, 30, 245),
        sf::Color(80, 80, 100)
    );
    
    if (fontLoaded) {
        sf::Text title(font);
        title.setCharacterSize(12);
        title.setString("MAP");
        title.setPosition(sf::Vector2f({685.f, 426.f}));
        title.setFillColor(sf::Color(220, 220, 240));
        title.setStyle(sf::Text::Bold);
        
        // Text with shadow
        UIVisualEffects::drawTextWithShadow(window, title, {1.f, 1.f}, sf::Color(0, 0, 0, 150));
    }
    
    float scale = 0.6f;
    float offsetX = 625.f;
    float offsetY = 445.f;
    
    const auto& rooms = dungeon.getRooms();
    for (const auto& room : rooms) {
        sf::RectangleShape roomRect(sf::Vector2f(room.width * scale - 1.f, room.height * scale - 1.f));
        roomRect.setPosition(sf::Vector2f(offsetX + room.x * scale, offsetY + room.y * scale));
        roomRect.setFillColor(sf::Color(70, 120, 70));
        roomRect.setOutlineThickness(1.0f);
        roomRect.setOutlineColor(sf::Color(100, 150, 100));
        window.draw(roomRect);
    }
    
    const auto& enemyList = enemies.getEnemies();
    for (const auto& enemy : enemyList) {
        sf::CircleShape enemyDot(2.f);
        enemyDot.setPosition(sf::Vector2f(offsetX + enemy.x * scale - 2.f, 
                                          offsetY + enemy.y * scale - 2.f));
        enemyDot.setFillColor(sf::Color(255, 50, 50));
        window.draw(enemyDot);
    }
    
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
    // MODERN INVENTORY PANEL with glass effect
    sf::FloatRect invRect(sf::Vector2f(200.f, 50.f), sf::Vector2f(400.f, 500.f));
    UIVisualEffects::drawStyledPanel(
        window,
        invRect,
        UIVisualEffects::PanelStyle::Glass,
        sf::Color(20, 20, 30, 250),
        sf::Color(80, 80, 120)
    );
    
    if (!fontLoaded || !invRenderer) return;
    
    sf::Text title(font);
    title.setCharacterSize(24);
    title.setString("INVENTORY");
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(sf::Vector2f(200.f + (400.f - titleBounds.size.x) / 2.f, 65.f));
    title.setFillColor(sf::Color(220, 220, 255));
    title.setStyle(sf::Text::Bold);
    
    // Title with shadow
    UIVisualEffects::drawTextWithShadow(window, title, {2.f, 2.f}, sf::Color(0, 0, 0, 180));
    
    const int cols = 5;
    const float startX = 230.f;
    const float startY = 120.f;
    const float padding = 10.f;
    const float slotSize = 64.0f;
    
    int index = 0;
    const ItemNew* hoveredItem = nullptr;
    
    sf::Vector2f prevPos;
    bool first = true;
    
    player.getInventoryNew().traverse([&](const ItemNew& item) {
        int row = index / cols;
        int col = index % cols;
        
        sf::Vector2f pos(startX + col * (slotSize + padding), startY + row * (slotSize + padding));
        
        if (!first) {
            sf::Vector2f start = prevPos + sf::Vector2f(slotSize, slotSize / 2.f);
            sf::Vector2f end = pos + sf::Vector2f(0.f, slotSize / 2.f);
            
            if (col > 0) {
                invRenderer->renderLinkedListArrow(window, start, end);
            }
        }
        
        bool isHovered = false;
        sf::FloatRect slotRect(pos, {slotSize, slotSize});
        if (slotRect.contains(lastMousePos)) {
            isHovered = true;
            hoveredItem = &item;
            hoveredInventoryIndex = index;
        }
        
        bool isSelected = (index == selectedInventoryIndex);
        
        invRenderer->renderItemSlot(window, item, pos, isSelected, isHovered);
        
        prevPos = pos;
        first = false;
        index++;
    });
    
    if (hoveredItem) {
        invRenderer->renderTooltip(window, *hoveredItem, lastMousePos, font);
    }
    
    sf::Text closeText(font);
    closeText.setCharacterSize(12);
    
    if (swapSourceIndex != -1) {
        closeText.setString("Select destination slot to SWAP items");
        closeText.setFillColor(sf::Color(255, 200, 100));
    } else {
        closeText.setString("Press I to close | Click to select/swap | U to use | X to drop");
        closeText.setFillColor(sf::Color(140, 140, 160));
    }
    
    sf::FloatRect closeBounds = closeText.getLocalBounds();
    closeText.setPosition(sf::Vector2f(200.f + (400.f - closeBounds.size.x) / 2.f, 520.f));
    window.draw(closeText);
}

void UIManager::renderSkillTreePanel(sf::RenderWindow& window) {
    // MODERN SKILL TREE PANEL
    sf::FloatRect treeRect(sf::Vector2f(125.f, 60.f), sf::Vector2f(550.f, 480.f));
    UIVisualEffects::drawStyledPanel(
        window,
        treeRect,
        UIVisualEffects::PanelStyle::Glass,
        sf::Color(15, 20, 30, 250),
        sf::Color(80, 100, 140)
    );
    
    if (!fontLoaded) return;
    
    sf::Text title(font);
    title.setCharacterSize(26);
    title.setString("=== SKILL TREE ===");
    title.setPosition(sf::Vector2f({240.f, 75.f}));
    title.setFillColor(sf::Color(180, 220, 255));
    title.setStyle(sf::Text::Bold);
    
    // Title with glow effect
    UIVisualEffects::drawTextWithShadow(window, title, {2.f, 2.f}, sf::Color(0, 0, 0, 200));
    
    sf::RectangleShape instructionBg(sf::Vector2f({510.f, 380.f}));
    instructionBg.setPosition(sf::Vector2f({145.f, 115.f}));
    instructionBg.setFillColor(sf::Color(15, 20, 30, 220));
    instructionBg.setOutlineThickness(2.0f);
    instructionBg.setOutlineColor(sf::Color(80, 100, 130));
    window.draw(instructionBg);
    
    if (game && game->getSkillTree()) {
        game->getSkillTree()->render(window, font, lastMousePos);
    }
    
    sf::Text closeText(font);
    closeText.setCharacterSize(12);
    closeText.setString("Press T to close  |  Click to unlock skills  |  Level up to earn points");
    closeText.setPosition(sf::Vector2f({145.f, 505.f}));
    closeText.setFillColor(sf::Color(200, 200, 200));
    window.draw(closeText);
}

void UIManager::renderTurnQueue(sf::RenderWindow& window, const EnemyManager& enemies) {
    if (enemies.isEmpty()) return;
    
    // MODERN TURN QUEUE panel
    float panelWidth = 200.f;
    float panelHeight = 90.f;
    sf::FloatRect queueRect(sf::Vector2f(10.f, 495.f), sf::Vector2f(panelWidth, panelHeight));
    UIVisualEffects::drawStyledPanel(
        window,
        queueRect,
        UIVisualEffects::PanelStyle::Bordered,
        sf::Color(40, 20, 20, 230),
        sf::Color(140, 70, 70)
    );
    
    if (fontLoaded) {
        sf::Text title(font);
        title.setCharacterSize(12);
        title.setString("TURN ORDER:");
        title.setPosition(sf::Vector2f({15.f, 500.f}));
        title.setFillColor(sf::Color(255, 180, 180));
        window.draw(title);
        
        const auto& enemyList = enemies.getEnemies();
        size_t displayCount = std::min(enemyList.size(), size_t(6));
        
        if (displayCount > 0) {
            float iconSpacing = 50.f;
            float totalWidth = displayCount * iconSpacing;
            float startX = 10.f + (panelWidth - totalWidth) / 2.0f;
            
            for (size_t i = 0; i < displayCount; i++) {
                float iconX = startX + i * iconSpacing;
                
                sf::CircleShape enemyIcon(12.f);
                enemyIcon.setPosition(sf::Vector2f(iconX, 530.f));
                enemyIcon.setFillColor(sf::Color(200, 50, 50));
                enemyIcon.setOutlineThickness(2.0f);
                enemyIcon.setOutlineColor(sf::Color(100, 0, 0));
                window.draw(enemyIcon);
                
                sf::Text enemyName(font);
                enemyName.setCharacterSize(8);
                const std::string& fullName = enemyList[i].name;
                std::string shortName = fullName.length() > 5 ? fullName.substr(0, 5) : fullName;
                enemyName.setString(shortName);
                
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
    
    float panelWidth = 650.0f;
    float panelHeight = 70.0f;
    float panelX = (800.0f - panelWidth) / 2.0f;
    float panelY = 535.0f;
    
    // MODERN HOTBAR panel with shadow
    sf::FloatRect hotbarRect(sf::Vector2f(panelX, panelY), sf::Vector2f(panelWidth, panelHeight));
    UIVisualEffects::drawStyledPanel(
        window,
        hotbarRect,
        UIVisualEffects::PanelStyle::Bordered,
        sf::Color(18, 18, 25, 235),
        sf::Color(90, 90, 110)
    );
    
    sf::Text title(font);
    title.setCharacterSize(10);
    title.setString("HOTBAR - Items & Skills");
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(sf::Vector2f(panelX + panelWidth / 2.0f - titleBounds.size.x / 2.0f, panelY + 2.0f));
    title.setFillColor(sf::Color(150, 150, 180));
    window.draw(title);
    
    // Render hotkeys logic
    float slotSize = 50.0f;
    float padding = 10.0f;
    float startX = panelX + 20.0f;
    float startY = panelY + 15.0f;
    
    // Render Inventory Hotkeys (1-5) - mapped to first 5 inventory items for now
    int invCount = 0;
    player.getInventoryNew().traverse([&](const ItemNew& item) {
        if (invCount >= 5) return;
        
        float x = startX + invCount * (slotSize + padding);
        
        // Slot background
        sf::RectangleShape slot(sf::Vector2f(slotSize, slotSize));
        slot.setPosition(sf::Vector2f(x, startY));
        slot.setFillColor(sf::Color(40, 40, 50));
        slot.setOutlineThickness(1.0f);
        slot.setOutlineColor(sf::Color(100, 100, 100));
        window.draw(slot);
        
        // Item Icon - use item.id as key (ItemManager registers textures by ID)
        if (!item.id.empty()) {
            sf::Texture* icon = AssetManager::getInstance().getTexture(item.id);
            if (icon) {
                sf::Sprite sprite(*icon);
                sprite.setPosition(sf::Vector2f(x + 5.f, startY + 5.f));
                float scaleX = (slotSize - 10.f) / icon->getSize().x;
                float scaleY = (slotSize - 10.f) / icon->getSize().y;
                sprite.setScale(sf::Vector2f(scaleX, scaleY));
                window.draw(sprite);
            }
        }
        
        // Hotkey Number
        if (fontLoaded) {
            sf::Text keyText(font, std::to_string(invCount + 1), 10);
            keyText.setPosition(sf::Vector2f(x + 2.f, startY + 2.f));
            keyText.setFillColor(sf::Color::Yellow);
            window.draw(keyText);
        }
        
        invCount++;
    });
    
    // Fill remaining empty slots up to 5
    for (int i = invCount; i < 5; i++) {
        float x = startX + i * (slotSize + padding);
        sf::RectangleShape slot(sf::Vector2f(slotSize, slotSize));
        slot.setPosition(sf::Vector2f(x, startY));
        slot.setFillColor(sf::Color(30, 30, 40)); // Darker for empty
        slot.setOutlineThickness(1.0f);
        slot.setOutlineColor(sf::Color(60, 60, 60));
        window.draw(slot);
        
        if (fontLoaded) {
            sf::Text keyText(font, std::to_string(i + 1), 10);
            keyText.setPosition(sf::Vector2f(x + 2.f, startY + 2.f));
            keyText.setFillColor(sf::Color(100, 100, 100));
            window.draw(keyText);
        }
    }
    
    // Render Skill Hotkeys (Space, Q, W, E, R) - Placeholder for now
    // ... (Skills would go here)
}

// OPTIMIZATION COMPLETE: Implemented missing TODO methods
void UIManager::showMainMenu() { 
    hideAll();
    // TODO: Future main menu rendering will go here
    std::cout << "[UIManager] Showing main menu" << std::endl;
}

void UIManager::showHUD() { 
    hideAll();
    // HUD is always visible in render(), this just hides panels
    std::cout << "[UIManager] Showing HUD (panels hidden)" << std::endl;
}

void UIManager::showInventory() { inventoryVisible = true; }
void UIManager::showSkillTree() { skillTreeVisible = true; }
void UIManager::hideAll() { inventoryVisible = false; skillTreeVisible = false; }
void UIManager::toggleInventory() { 
    inventoryVisible = !inventoryVisible; 
    if (inventoryVisible) skillTreeVisible = false;
}
void UIManager::toggleSkillTree() { 
    skillTreeVisible = !skillTreeVisible; 
    if (skillTreeVisible) inventoryVisible = false;
}
void UIManager::toggleMiniMap() { miniMapVisible = !miniMapVisible; }
void UIManager::updateHUD(const Player& player) {
    // Smooth health/mana bar interpolation
    float targetHealth = static_cast<float>(player.getHealth());
    float targetMana = static_cast<float>(player.getMana());
    displayedHealth += (targetHealth - displayedHealth) * 0.15f;  // Smooth lerp
    displayedMana += (targetMana - displayedMana) * 0.15f;
}
void UIManager::updateInventory(const Player& player) { /* Handled in render */ }
bool UIManager::loadFont(const std::string& path) { return font.openFromFile(path); }

// ═══════════════════════════════════════════════════════════════════════
// DYNAMIC UI RENDER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════

void UIManager::renderRadialCooldown(sf::RenderWindow& window, sf::Vector2f center, float radius, float progress, sf::Color color) {
    const int segments = 32;
    sf::VertexArray pie(sf::PrimitiveType::TriangleFan, segments + 2);
    
    pie[0].position = center;
    pie[0].color = sf::Color(color.r, color.g, color.b, 150);
    
    float angle = progress * 2.f * 3.14159f;
    for (int i = 0; i <= segments; ++i) {
        float theta = -3.14159f / 2.f + (static_cast<float>(i) / segments) * angle;
        pie[i + 1].position = sf::Vector2f(center.x + std::cos(theta) * radius, center.y + std::sin(theta) * radius);
        pie[i + 1].color = sf::Color(color.r, color.g, color.b, 100);
    }
    
    window.draw(pie);
    
    // Outline circle
    sf::CircleShape outline(radius);
    outline.setPosition(sf::Vector2f(center.x - radius, center.y - radius));
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineThickness(2.f);
    outline.setOutlineColor(sf::Color(color.r, color.g, color.b, 200));
    window.draw(outline);
}

void UIManager::renderComboCounter(sf::RenderWindow& window) {
    if (currentCombo < 2 || comboDisplayTimer <= 0.f) return;
    
    float alpha = std::min(1.f, comboDisplayTimer / COMBO_FADE_TIME) * 255.f;
    float scale = 1.f + (currentCombo * 0.05f);  // Scale up with combo
    
    std::string comboText = "x" + std::to_string(currentCombo) + " COMBO!";
    sf::Color comboColor = currentCombo > 5 ? sf::Color(255, 100, 100, static_cast<uint8_t>(alpha)) 
                                            : sf::Color(255, 215, 0, static_cast<uint8_t>(alpha));
    
    if (fontLoaded) {
        sf::Text text(font, comboText, static_cast<unsigned int>(24 * scale));
        text.setPosition(sf::Vector2f(400.f - text.getLocalBounds().size.x / 2.f, 120.f));
        text.setFillColor(comboColor);
        text.setOutlineThickness(2.f);
        text.setOutlineColor(sf::Color(0, 0, 0, static_cast<uint8_t>(alpha)));
        window.draw(text);
    }
}

void UIManager::setCombo(int combo) {
    currentCombo = combo;
    comboDisplayTimer = COMBO_FADE_TIME;
}
