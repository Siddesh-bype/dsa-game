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
#include <sstream>
#include <iomanip>

// ═══════════════════════════════════════════════════════════════════════
// CORE ANIMATION CONSTANTS
// ═══════════════════════════════════════════════════════════════════════

namespace {
    // Floating Text Animation
    constexpr float FLOATING_TEXT_SPEED = 30.0f;
    constexpr float FLOATING_TEXT_LIFETIME = 1.5f;
    constexpr unsigned int FLOATING_TEXT_SIZE = 20;
    constexpr float FLOATING_TEXT_OUTLINE = 2.0f;
    constexpr float OUTLINE_ALPHA_MULTIPLIER = 1.2f;
    
    // Screen Flash Effect
    constexpr float SCREEN_FLASH_DURATION = 0.3f;
    constexpr float SCREEN_FLASH_MAX_ALPHA = 150.0f;
    
    // Character Name Labels
    constexpr unsigned int PLAYER_NAME_SIZE = 11;
    constexpr unsigned int ENEMY_NAME_SIZE = 10;
    constexpr float NAME_OUTLINE_THICKNESS = 1.5f;
    
    // DSA Visualization
    constexpr int HEAP_GLOW_PRIORITY = 100;
    constexpr size_t HEAP_ROOM_INDEX = 2;
    
    // HUD Background (legacy shapes - used in initialize)
    constexpr float HUD_BG_WIDTH = 800.f;
    constexpr float HUD_BG_HEIGHT = 75.f;
}

// ═══════════════════════════════════════════════════════════════════════
// CONSTRUCTOR & INITIALIZATION
// ═══════════════════════════════════════════════════════════════════════

UIManager::UIManager(Game* gamePtr) : 
    game(gamePtr),
    fontLoaded(false), 
    inventoryVisible(false), 
    skillTreeVisible(false), 
    miniMapVisible(true), 
    animationTime(0.f),
    selectedInventoryIndex(-1), 
    hoveredInventoryIndex(-1),
    screenFlashTimer(0.f), 
    screenFlashColor(sf::Color::Transparent),
    healthBarProgress(1.0f), 
    xpBarProgress(0.0f) {
    dsaViz = std::make_unique<DSAVisualizer>();
    invRenderer = std::make_unique<InventoryRenderer>();
}

void UIManager::initialize() {
    // Load primary font with fallback
    fontLoaded = font.openFromFile("assets/fonts/Roboto-Regular.ttf");
    if (!fontLoaded) {
        fontLoaded = font.openFromFile("C:/Windows/Fonts/arial.ttf");
    }
    
    if (fontLoaded) {
        dsaViz->setFont(&font);
    }
    
    // Initialize HUD shapes (legacy, used for fallback rendering)
    hudBackground.setSize({HUD_BG_WIDTH, HUD_BG_HEIGHT});
    hudBackground.setPosition(sf::Vector2f(0.f, 0.f));
    hudBackground.setFillColor(sf::Color(20, 20, 30, 210));
    
    healthBarBg.setSize({220.f, 22.f});
    healthBarBg.setPosition(sf::Vector2f(15.f, 42.f));
    healthBarBg.setFillColor(sf::Color(50, 30, 30));
    
    healthBarFg.setSize({220.f, 22.f});
    healthBarFg.setPosition(sf::Vector2f(15.f, 42.f));
    healthBarFg.setFillColor(sf::Color(200, 50, 50));
    
    xpBarBg.setSize({180.f, 14.f});
    xpBarBg.setPosition(sf::Vector2f(260.f, 50.f));
    xpBarBg.setFillColor(sf::Color(30, 30, 50));
    
    xpBarFg.setSize({180.f, 14.f});
    xpBarFg.setPosition(sf::Vector2f(260.f, 50.f));
    xpBarFg.setFillColor(sf::Color(100, 100, 255));
}

// ═══════════════════════════════════════════════════════════════════════
// UPDATE LOOP
// ═══════════════════════════════════════════════════════════════════════

void UIManager::update(float deltaTime) {
    animationTime += deltaTime;
    
    // Combo display timer
    if (comboDisplayTimer > 0.f) {
        comboDisplayTimer -= deltaTime;
        if (comboDisplayTimer <= 0.f) {
            currentCombo = 0;
        }
    }
    
    // Floating texts - physics-based movement with smooth deceleration
    for (auto it = floatingTexts.begin(); it != floatingTexts.end();) {
        it->lifetime -= deltaTime;
        if (it->lifetime <= 0.0f) {
            it = floatingTexts.erase(it);
        } else {
            const float speedMultiplier = it->lifetime / it->maxLifetime;
            it->position.y -= FLOATING_TEXT_SPEED * deltaTime * (1.0f + speedMultiplier);
            ++it;
        }
    }
    
    // Screen flash decay
    if (screenFlashTimer > 0.0f) {
        screenFlashTimer -= deltaTime;
        if (screenFlashTimer <= 0.0f) {
            screenFlashColor = sf::Color::Transparent;
        }
    }
    
    // Item wheel animation
    itemWheel.update(deltaTime);
}

void UIManager::updateMousePosition(sf::Vector2f mousePos) {
    lastMousePos = mousePos;
    itemWheel.handleMouseMove(mousePos);
}

// ═══════════════════════════════════════════════════════════════════════
// INPUT HANDLING
// ═══════════════════════════════════════════════════════════════════════

bool UIManager::handleMouseClick(sf::Vector2f mousePos) {
    SoundManager::getInstance().playSound("ui_click", 1.0f, 0.5f);

    // Skill tree click handling
    if (skillTreeVisible && game && game->getSkillTree()) {
        const bool unlocked = game->getSkillTree()->tryUnlockAt(mousePos);
        if (unlocked) {
            SoundManager::getInstance().playSound("levelup", 1.2f, 0.6f);
        }
        return true;
    }
    
    // Inventory click handling
    if (inventoryVisible) {
        if (hoveredInventoryIndex != -1) {
            if (swapSourceIndex == -1) {
                // Start swap operation
                swapSourceIndex = hoveredInventoryIndex;
                selectedInventoryIndex = hoveredInventoryIndex;
            } else if (swapSourceIndex == hoveredInventoryIndex) {
                // Cancel swap (clicked same slot)
                swapSourceIndex = -1;
                selectedInventoryIndex = -1;
            } else if (game && game->getPlayer()) {
                // Complete swap operation
                game->getPlayer()->swapInventoryItems(swapSourceIndex, hoveredInventoryIndex);
                swapSourceIndex = -1;
                selectedInventoryIndex = hoveredInventoryIndex;
            }
            return true;
        }
        // Clicked outside inventory slots
        swapSourceIndex = -1;
        selectedInventoryIndex = -1;
    }
    
    return false;
}

// ═══════════════════════════════════════════════════════════════════════
// RENDER ORCHESTRATION
// ═══════════════════════════════════════════════════════════════════════

void UIManager::render(sf::RenderWindow& window, const Player& player, 
                        const Dungeon& dungeon, const EnemyManager& enemies, int currentFloor) {
    renderUI(window, player, dungeon, enemies, nullptr, currentFloor);
}

void UIManager::renderUI(sf::RenderWindow& window, const Player& player, 
                          const Dungeon& dungeon, const EnemyManager& enemies, 
                          SkillTree* skillTree, int currentFloor) {
    // DSA visualizations (stack trail, heap glow)
    if (dsaViz) {
        dsaViz->renderStackTrail(window, player, TILE_SIZE);
        
        const auto& rooms = dungeon.getRooms();
        if (rooms.size() > HEAP_ROOM_INDEX) {
            const auto& room = rooms[HEAP_ROOM_INDEX];
            const float lootX = (room.x + room.width / 2.f) * TILE_SIZE;
            const float lootY = (room.y + room.height / 2.f) * TILE_SIZE;
            dsaViz->renderHeapGlow(window, sf::Vector2f(lootX, lootY), HEAP_GLOW_PRIORITY);
        }
    }
    
    // Core HUD elements
    renderHUD(window, player, skillTree, currentFloor);
    
    // Optional panels
    if (miniMapVisible) {
        renderMiniMap(window, dungeon, player, enemies);
    }
    if (inventoryVisible) {
        renderInventoryPanel(window, player);
    }
    if (skillTreeVisible) {
        renderSkillTreePanel(window);
    }
    
    // Always-visible UI elements
    renderTurnQueue(window, enemies);
    renderSkillHotkeys(window, player, skillTree);
    renderCharacterNames(window, player, enemies, TILE_SIZE);
    renderFloatingTexts(window);
    renderComboCounter(window);
    renderScreenFlash(window);
    
    // Item wheel overlay (topmost layer)
    itemWheel.render(window);
}

void UIManager::renderGraphPathsOnly(sf::RenderWindow& window, const Dungeon& dungeon) {
    if (dsaViz) {
        dsaViz->renderGraphPaths(window, dungeon, TILE_SIZE);
    }
}

// ═══════════════════════════════════════════════════════════════════════
// FLOATING TEXT & SCREEN EFFECTS
// ═══════════════════════════════════════════════════════════════════════

void UIManager::addFloatingText(const std::string& text, float x, float y, sf::Color color) {
    floatingTexts.emplace_back(text, sf::Vector2f(x, y), color, FLOATING_TEXT_LIFETIME);
}

void UIManager::triggerScreenFlash(sf::Color color, float duration) {
    screenFlashColor = color;
    screenFlashTimer = duration;
}

void UIManager::renderFloatingTexts(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    
    for (const auto& floatText : floatingTexts) {
        const float normalizedLife = floatText.lifetime / floatText.maxLifetime;
        
        // Smooth fade with ease-out curve
        float alpha = normalizedLife * 255.0f * (2.0f - normalizedLife);
        alpha = std::clamp(alpha, 0.0f, 255.0f);
        const auto alphaU8 = static_cast<std::uint8_t>(alpha);
        
        sf::Color fadeColor = floatText.color;
        fadeColor.a = alphaU8;
        
        sf::Text text(font, floatText.text, FLOATING_TEXT_SIZE);
        text.setPosition(floatText.position);
        text.setFillColor(fadeColor);
        
        // Proportional outline alpha for visibility
        const float outlineAlpha = std::min(alpha * OUTLINE_ALPHA_MULTIPLIER, 255.0f);
        text.setOutlineThickness(FLOATING_TEXT_OUTLINE);
        text.setOutlineColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(outlineAlpha)));
        
        window.draw(text);
    }
}

void UIManager::renderScreenFlash(sf::RenderWindow& window) {
    if (screenFlashTimer <= 0.0f) return;
    
    // Quadratic fade for smooth decay
    const float normalizedTime = screenFlashTimer / SCREEN_FLASH_DURATION;
    const float alpha = normalizedTime * normalizedTime * SCREEN_FLASH_MAX_ALPHA;
    
    sf::Color flashColor = screenFlashColor;
    flashColor.a = static_cast<std::uint8_t>(std::min(alpha, 255.0f));
    
    sf::RectangleShape flash(sf::Vector2f(
        static_cast<float>(window.getSize().x), 
        static_cast<float>(window.getSize().y)
    ));
    flash.setFillColor(flashColor);
    window.draw(flash);
}

void UIManager::renderCharacterNames(sf::RenderWindow& window, const Player& player, 
                                      const EnemyManager& enemies, float tileSize) {
    if (!fontLoaded) return;
    
    // Player name label
    const Position playerPos = player.getPosition();
    const float playerX = playerPos.x * tileSize + tileSize / 2.f;
    const float playerY = playerPos.y * tileSize - tileSize / 3.f;
    
    sf::Text playerName(font, player.getName(), PLAYER_NAME_SIZE);
    playerName.setFillColor(sf::Color(100, 255, 100));
    playerName.setOutlineThickness(NAME_OUTLINE_THICKNESS);
    playerName.setOutlineColor(sf::Color::Black);
    
    const sf::FloatRect playerBounds = playerName.getGlobalBounds();
    playerName.setPosition(sf::Vector2f(playerX - playerBounds.size.x / 2.f, playerY));
    window.draw(playerName);
    
    // Enemy name labels
    const auto& enemyList = enemies.getEnemies();
    for (const auto& enemy : enemyList) {
        const float enemyX = enemy.x * tileSize + tileSize / 2.f;
        const float enemyY = enemy.y * tileSize - tileSize / 3.f;
        
        sf::Text enemyNameText(font, enemy.name, ENEMY_NAME_SIZE);
        
        // Color by enemy type
        if (enemy.type == "boss") {
            enemyNameText.setFillColor(sf::Color(255, 50, 50));
        } else if (enemy.type == "ranged") {
            enemyNameText.setFillColor(sf::Color(255, 200, 50));
        } else {
            enemyNameText.setFillColor(sf::Color(255, 150, 150));
        }
        
        enemyNameText.setOutlineThickness(NAME_OUTLINE_THICKNESS);
        enemyNameText.setOutlineColor(sf::Color::Black);
        
        const sf::FloatRect enemyBounds = enemyNameText.getGlobalBounds();
        enemyNameText.setPosition(sf::Vector2f(enemyX - enemyBounds.size.x / 2.f, enemyY));
        window.draw(enemyNameText);
    }
}


// ═══════════════════════════════════════════════════════════════════════
// HUD & PANEL LAYOUT CONSTANTS
// ═══════════════════════════════════════════════════════════════════════

namespace {
    // Contextual Prompt
    constexpr float PROMPT_FONT_SIZE = 16.f;
    constexpr float PROMPT_PADDING_X = 20.f;
    constexpr float PROMPT_PADDING_Y = 16.f;
    constexpr float PROMPT_BOTTOM_OFFSET = 100.f;
    
    // HUD Layout
    constexpr float HUD_X = 5.f;
    constexpr float HUD_Y = 5.f;
    constexpr float HUD_WIDTH = 400.f;
    constexpr float HUD_HEIGHT = 70.f;
    
    // Health Bar
    constexpr float HEALTH_BAR_X = 15.f;
    constexpr float HEALTH_BAR_Y = 38.f;
    constexpr float HEALTH_BAR_W = 200.f;
    constexpr float HEALTH_BAR_H = 20.f;
    constexpr float HEALTH_LOW_THRESHOLD = 0.3f;
    
    // XP Bar
    constexpr float XP_BAR_X = 230.f;
    constexpr float XP_BAR_Y = 48.f;
    constexpr float XP_BAR_W = 150.f;
    constexpr float XP_BAR_H = 15.f;
    constexpr int XP_PER_LEVEL = 100;
    
    // Icons
    constexpr float HEART_ICON_SIZE = 20.f;
    constexpr float GOLD_ICON_SIZE = 14.f;
    
    // Minimap Layout
    constexpr float MINIMAP_X = 620.f;
    constexpr float MINIMAP_Y = 420.f;
    constexpr float MINIMAP_SIZE = 165.f;
    constexpr float MINIMAP_SCALE = 0.6f;
    constexpr float MINIMAP_OFFSET_X = 625.f;
    constexpr float MINIMAP_OFFSET_Y = 445.f;
    constexpr float PLAYER_DOT_RADIUS = 3.f;
    constexpr float ENEMY_DOT_RADIUS = 2.f;
    
    // Inventory Panel
    constexpr float INV_PANEL_X = 200.f;
    constexpr float INV_PANEL_Y = 50.f;
    constexpr float INV_PANEL_W = 400.f;
    constexpr float INV_PANEL_H = 500.f;
    constexpr int INV_COLUMNS = 5;
    constexpr float INV_SLOT_SIZE = 64.f;
    constexpr float INV_SLOT_PADDING = 10.f;
    constexpr float INV_START_X = 230.f;
    constexpr float INV_START_Y = 120.f;
    
    // Frame timing assumption
    constexpr float ASSUMED_DELTA_TIME = 0.016f;  // ~60fps
}

void UIManager::renderContextualPrompt(sf::RenderWindow& window, const std::string& prompt) {
    if (!fontLoaded || prompt.empty()) return;
    
    sf::Text promptText(font, prompt, static_cast<unsigned int>(PROMPT_FONT_SIZE));
    promptText.setFillColor(sf::Color(255, 255, 100));
    promptText.setOutlineThickness(2.f);
    promptText.setOutlineColor(sf::Color::Black);
    
    const sf::FloatRect bounds = promptText.getGlobalBounds();
    const float centerX = (static_cast<float>(window.getSize().x) - bounds.size.x) / 2.f;
    const float bottomY = static_cast<float>(window.getSize().y) - PROMPT_BOTTOM_OFFSET;
    
    // Background panel
    sf::RectangleShape promptBg(sf::Vector2f(bounds.size.x + PROMPT_PADDING_X, bounds.size.y + PROMPT_PADDING_Y));
    promptBg.setPosition(sf::Vector2f(centerX - PROMPT_PADDING_X / 2.f, bottomY - PROMPT_PADDING_Y / 2.f));
    promptBg.setFillColor(sf::Color(0, 0, 0, 180));
    promptBg.setOutlineThickness(2.f);
    promptBg.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(promptBg);
    
    promptText.setPosition(sf::Vector2f(centerX, bottomY));
    window.draw(promptText);
}

void UIManager::renderHUD(sf::RenderWindow& window, const Player& player, SkillTree* skillTree, int currentFloor) {
    // Main HUD panel (glassmorphism)
    const sf::FloatRect hudRect(sf::Vector2f(HUD_X, HUD_Y), sf::Vector2f(HUD_WIDTH, HUD_HEIGHT));
    UIVisualEffects::drawStyledPanel(
        window, hudRect,
        UIVisualEffects::PanelStyle::Glass,
        sf::Color(40, 35, 30, 220),
        sf::Color(100, 80, 60)
    );
    
    // Health bar with dynamic gradient
    const float healthPercent = static_cast<float>(player.getHealth()) / static_cast<float>(player.getMaxHealth());
    const sf::Color healthGradStart = (healthPercent > HEALTH_LOW_THRESHOLD) 
        ? sf::Color(220, 50, 50) : sf::Color(180, 30, 30);
    const sf::Color healthGradEnd = (healthPercent > HEALTH_LOW_THRESHOLD) 
        ? sf::Color(180, 20, 20) : sf::Color(120, 10, 10);
    
    const sf::FloatRect healthBarRect(sf::Vector2f(HEALTH_BAR_X, HEALTH_BAR_Y), sf::Vector2f(HEALTH_BAR_W, HEALTH_BAR_H));
    UIVisualEffects::drawAnimatedBar(
        window, healthBarRect, healthBarProgress, healthPercent,
        ASSUMED_DELTA_TIME, healthGradStart, healthGradEnd, true
    );
    
    // XP bar with blue gradient
    const int xpForNextLevel = (player.getLevel() + 1) * XP_PER_LEVEL;
    const float xpPercent = static_cast<float>(player.getExperience() % xpForNextLevel) / static_cast<float>(xpForNextLevel);
    
    const sf::FloatRect xpBarRect(sf::Vector2f(XP_BAR_X, XP_BAR_Y), sf::Vector2f(XP_BAR_W, XP_BAR_H));
    UIVisualEffects::drawAnimatedBar(
        window, xpBarRect, xpBarProgress, xpPercent,
        ASSUMED_DELTA_TIME, sf::Color(80, 160, 255), sf::Color(40, 100, 200), true
    );
    
    if (!fontLoaded) return;
    
    // Heart icon + health text
    if (sf::Texture* heartTex = AssetManager::getInstance().getTexture("ui_heart")) {
        sf::Sprite heartIcon(*heartTex);
        heartIcon.setPosition(sf::Vector2f(18.f, 13.f));
        const float heartScale = HEART_ICON_SIZE / static_cast<float>(heartTex->getSize().x);
        heartIcon.setScale(sf::Vector2f(heartScale, heartScale));
        window.draw(heartIcon);
    }
    
    std::ostringstream healthStr;
    healthStr << player.getHealth() << "/" << player.getMaxHealth();
    window.draw(makeText(healthStr.str(), 15, {42.f, 12.f}));
    
    // Gold icon + gold amount
    if (sf::Texture* goldTex = AssetManager::getInstance().getTexture("ui_gold")) {
        sf::Sprite goldIcon(*goldTex);
        goldIcon.setPosition(sf::Vector2f(18.f, 62.f));
        const float goldScale = GOLD_ICON_SIZE / static_cast<float>(goldTex->getSize().x);
        goldIcon.setScale(sf::Vector2f(goldScale, goldScale));
        window.draw(goldIcon);
    }
    window.draw(makeText(std::to_string(player.getGold()), 14, {36.f, 62.f}, sf::Color(255, 215, 0)));
    
    // Level & XP labels
    window.draw(makeText("LVL " + std::to_string(player.getLevel()), 18, {265.f, 10.f}, sf::Color(200, 220, 255)));
    window.draw(makeText("XP", 10, {265.f, 37.f}, sf::Color(150, 150, 200), 0.5f));
    window.draw(makeText("XP: " + std::to_string(player.getExperience()), 10, {390.f, 50.f}, sf::Color(180, 180, 220), 0.f));
    
    // Floor indicator (right-aligned)
    sf::Text floorText = makeText("Floor " + std::to_string(currentFloor), 18, {0.f, 12.f}, sf::Color(255, 215, 100));
    const sf::FloatRect floorBounds = floorText.getGlobalBounds();
    floorText.setPosition(sf::Vector2f(static_cast<float>(window.getSize().x) - floorBounds.size.x - 20.f, 12.f));
    window.draw(floorText);
    
    // Mana display
    std::ostringstream manaStr;
    manaStr << "MP " << player.getMana() << "/" << player.getMaxMana();
    window.draw(makeText(manaStr.str(), 14, {120.f, 62.f}, sf::Color(100, 180, 255)));
    
    // Skill points display
    if (skillTree) {
        const int availablePoints = skillTree->getAvailablePoints();
        const sf::Color spColor = (availablePoints > 0) ? sf::Color(255, 255, 100) : sf::Color(180, 180, 200);
        sf::Text spText = makeText("SP: " + std::to_string(availablePoints), 16, {340.f, 12.f}, spColor);
        if (availablePoints > 0) {
            spText.setStyle(sf::Text::Bold);
        }
        window.draw(spText);
        
        if (availablePoints > 0) {
            window.draw(makeText("(Press T)", 10, {340.f, 32.f}, sf::Color(200, 200, 100), 0.f));
        }
    }
    
    // Controls hint (bottom of screen)
    const float hintY = static_cast<float>(window.getSize().y) - 25.f;
    window.draw(makeText("I:Inv T:Skills O:Unlock Space:Attack 1-5:Skill U:UseItem E:Interact Z:Back", 9, 
                         {10.f, hintY}, sf::Color(160, 160, 180), 0.f));
}

void UIManager::renderMiniMap(sf::RenderWindow& window, const Dungeon& dungeon, 
                               const Player& player, const EnemyManager& enemies) {
    // Panel background
    const sf::FloatRect miniMapRect(sf::Vector2f(MINIMAP_X, MINIMAP_Y), sf::Vector2f(MINIMAP_SIZE, MINIMAP_SIZE));
    UIVisualEffects::drawStyledPanel(
        window, miniMapRect,
        UIVisualEffects::PanelStyle::Bordered,
        sf::Color(25, 25, 30, 245),
        sf::Color(80, 80, 100)
    );
    
    // Title with shadow
    if (fontLoaded) {
        sf::Text title(font, "MAP", 12);
        title.setPosition(sf::Vector2f(685.f, 426.f));
        title.setFillColor(sf::Color(220, 220, 240));
        title.setStyle(sf::Text::Bold);
        UIVisualEffects::drawTextWithShadow(window, title, {1.f, 1.f}, sf::Color(0, 0, 0, 150));
    }
    
    // Render dungeon rooms
    const auto& rooms = dungeon.getRooms();
    for (const auto& room : rooms) {
        sf::RectangleShape roomRect(sf::Vector2f(
            room.width * MINIMAP_SCALE - 1.f, 
            room.height * MINIMAP_SCALE - 1.f
        ));
        roomRect.setPosition(sf::Vector2f(
            MINIMAP_OFFSET_X + room.x * MINIMAP_SCALE, 
            MINIMAP_OFFSET_Y + room.y * MINIMAP_SCALE
        ));
        roomRect.setFillColor(sf::Color(70, 120, 70));
        roomRect.setOutlineThickness(1.0f);
        roomRect.setOutlineColor(sf::Color(100, 150, 100));
        window.draw(roomRect);
    }
    
    // Render enemy dots
    const auto& enemyList = enemies.getEnemies();
    for (const auto& enemy : enemyList) {
        sf::CircleShape enemyDot(ENEMY_DOT_RADIUS);
        enemyDot.setPosition(sf::Vector2f(
            MINIMAP_OFFSET_X + enemy.x * MINIMAP_SCALE - ENEMY_DOT_RADIUS,
            MINIMAP_OFFSET_Y + enemy.y * MINIMAP_SCALE - ENEMY_DOT_RADIUS
        ));
        enemyDot.setFillColor(sf::Color(255, 50, 50));
        window.draw(enemyDot);
    }
    
    // Render player dot
    const Position playerPos = player.getPosition();
    const float playerX = MINIMAP_OFFSET_X + playerPos.x * MINIMAP_SCALE - PLAYER_DOT_RADIUS;
    const float playerY = MINIMAP_OFFSET_Y + playerPos.y * MINIMAP_SCALE - PLAYER_DOT_RADIUS;
    
    sf::CircleShape playerDot(PLAYER_DOT_RADIUS);
    playerDot.setPosition(sf::Vector2f(playerX, playerY));
    playerDot.setFillColor(sf::Color(0, 255, 255));
    playerDot.setOutlineThickness(1.f);
    playerDot.setOutlineColor(sf::Color(255, 255, 255, 180));
    window.draw(playerDot);
}

void UIManager::renderInventoryPanel(sf::RenderWindow& window, const Player& player) {
    // Panel background (glass effect)
    const sf::FloatRect invRect(sf::Vector2f(INV_PANEL_X, INV_PANEL_Y), sf::Vector2f(INV_PANEL_W, INV_PANEL_H));
    UIVisualEffects::drawStyledPanel(
        window, invRect,
        UIVisualEffects::PanelStyle::Glass,
        sf::Color(20, 20, 30, 250),
        sf::Color(80, 80, 120)
    );
    
    if (!fontLoaded || !invRenderer) return;
    
    // Title with shadow
    sf::Text title(font, "INVENTORY", 24);
    const sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(sf::Vector2f(INV_PANEL_X + (INV_PANEL_W - titleBounds.size.x) / 2.f, 65.f));
    title.setFillColor(sf::Color(220, 220, 255));
    title.setStyle(sf::Text::Bold);
    UIVisualEffects::drawTextWithShadow(window, title, {2.f, 2.f}, sf::Color(0, 0, 0, 180));
    
    // Traverse inventory items
    int index = 0;
    const ItemNew* hoveredItem = nullptr;
    sf::Vector2f prevPos;
    bool first = true;
    
    player.getInventoryNew().traverse([&](const ItemNew& item) {
        const int row = index / INV_COLUMNS;
        const int col = index % INV_COLUMNS;
        
        const sf::Vector2f pos(
            INV_START_X + static_cast<float>(col) * (INV_SLOT_SIZE + INV_SLOT_PADDING),
            INV_START_Y + static_cast<float>(row) * (INV_SLOT_SIZE + INV_SLOT_PADDING)
        );
        
        // Draw linked-list arrow between items
        if (!first && col > 0) {
            const sf::Vector2f start = prevPos + sf::Vector2f(INV_SLOT_SIZE, INV_SLOT_SIZE / 2.f);
            const sf::Vector2f end = pos + sf::Vector2f(0.f, INV_SLOT_SIZE / 2.f);
            invRenderer->renderLinkedListArrow(window, start, end);
        }
        
        // Check hover state
        const sf::FloatRect slotRect(pos, {INV_SLOT_SIZE, INV_SLOT_SIZE});
        const bool isHovered = slotRect.contains(lastMousePos);
        if (isHovered) {
            hoveredItem = &item;
            hoveredInventoryIndex = index;
        }
        
        const bool isSelected = (index == selectedInventoryIndex);
        invRenderer->renderItemSlot(window, item, pos, isSelected, isHovered);
        
        prevPos = pos;
        first = false;
        ++index;
    });
    
    // Tooltip for hovered item
    if (hoveredItem) {
        invRenderer->renderTooltip(window, *hoveredItem, lastMousePos, font);
    }
    
    // Instructions footer
    sf::Text instructions(font);
    instructions.setCharacterSize(12);
    
    if (swapSourceIndex != -1) {
        instructions.setString("Select destination slot to SWAP items");
        instructions.setFillColor(sf::Color(255, 200, 100));
    } else {
        instructions.setString("Press I to close | Click to select/swap | U to use | X to drop");
        instructions.setFillColor(sf::Color(140, 140, 160));
    }
    
    const sf::FloatRect instrBounds = instructions.getLocalBounds();
    instructions.setPosition(sf::Vector2f(INV_PANEL_X + (INV_PANEL_W - instrBounds.size.x) / 2.f, 520.f));
    window.draw(instructions);
}

// ═══════════════════════════════════════════════════════════════════════
// PANEL RENDERING - Skill Tree, Turn Queue, Hotbar
// ═══════════════════════════════════════════════════════════════════════

namespace {
    // Skill Tree Panel Layout
    constexpr float TREE_PANEL_X = 125.f;
    constexpr float TREE_PANEL_Y = 60.f;
    constexpr float TREE_PANEL_W = 550.f;
    constexpr float TREE_PANEL_H = 480.f;
    
    // Turn Queue Layout
    constexpr float QUEUE_PANEL_W = 200.f;
    constexpr float QUEUE_PANEL_H = 90.f;
    constexpr float QUEUE_ICON_RADIUS = 12.f;
    constexpr float QUEUE_ICON_SPACING = 50.f;
    constexpr size_t QUEUE_MAX_DISPLAY = 6;
    constexpr size_t ENEMY_NAME_MAX_LEN = 5;
    
    // Hotbar Layout
    constexpr float HOTBAR_WIDTH = 650.f;
    constexpr float HOTBAR_HEIGHT = 70.f;
    constexpr float HOTBAR_Y = 535.f;
    constexpr float SKILL_SLOT_SIZE = 50.f;
    constexpr float SKILL_SLOT_PADDING = 10.f;
    constexpr int NUM_SKILL_SLOTS = 5;
    constexpr float SCREEN_WIDTH = 800.f;  // TODO: Get from window
}

void UIManager::renderSkillTreePanel(sf::RenderWindow& window) {
    const sf::FloatRect treeRect(
        sf::Vector2f(TREE_PANEL_X, TREE_PANEL_Y), 
        sf::Vector2f(TREE_PANEL_W, TREE_PANEL_H)
    );
    UIVisualEffects::drawStyledPanel(
        window, treeRect,
        UIVisualEffects::PanelStyle::Glass,
        sf::Color(15, 20, 30, 250),
        sf::Color(80, 100, 140)
    );
    
    if (!fontLoaded) return;
    
    // Panel title
    sf::Text title(font);
    title.setCharacterSize(26);
    title.setString("=== SKILL TREE ===");
    title.setPosition(sf::Vector2f(240.f, 75.f));
    title.setFillColor(sf::Color(180, 220, 255));
    title.setStyle(sf::Text::Bold);
    UIVisualEffects::drawTextWithShadow(window, title, {2.f, 2.f}, sf::Color(0, 0, 0, 200));
    
    // Content background
    sf::RectangleShape contentBg(sf::Vector2f(510.f, 380.f));
    contentBg.setPosition(sf::Vector2f(145.f, 115.f));
    contentBg.setFillColor(sf::Color(15, 20, 30, 220));
    contentBg.setOutlineThickness(2.0f);
    contentBg.setOutlineColor(sf::Color(80, 100, 130));
    window.draw(contentBg);
    
    // Render skill tree nodes
    if (game && game->getSkillTree()) {
        game->getSkillTree()->render(window, font, lastMousePos);
    }
    
    // Instructions footer
    sf::Text instructions(font, "Press T to close  |  Click to unlock skills  |  Level up to earn points", 12);
    instructions.setPosition(sf::Vector2f(145.f, 505.f));
    instructions.setFillColor(sf::Color(200, 200, 200));
    window.draw(instructions);
}

void UIManager::renderTurnQueue(sf::RenderWindow& window, const EnemyManager& enemies) {
    if (enemies.isEmpty() || !fontLoaded) return;
    
    const sf::FloatRect queueRect(
        sf::Vector2f(10.f, 495.f), 
        sf::Vector2f(QUEUE_PANEL_W, QUEUE_PANEL_H)
    );
    UIVisualEffects::drawStyledPanel(
        window, queueRect,
        UIVisualEffects::PanelStyle::Bordered,
        sf::Color(40, 20, 20, 230),
        sf::Color(140, 70, 70)
    );
    
    // Title
    sf::Text title(font, "TURN ORDER:", 12);
    title.setPosition(sf::Vector2f(15.f, 500.f));
    title.setFillColor(sf::Color(255, 180, 180));
    window.draw(title);
    
    // Enemy icons
    const auto& enemyList = enemies.getEnemies();
    const size_t displayCount = std::min(enemyList.size(), QUEUE_MAX_DISPLAY);
    if (displayCount == 0) return;
    
    const float totalWidth = static_cast<float>(displayCount) * QUEUE_ICON_SPACING;
    const float startX = 10.f + (QUEUE_PANEL_W - totalWidth) / 2.0f;
    
    for (size_t i = 0; i < displayCount; ++i) {
        const float iconX = startX + static_cast<float>(i) * QUEUE_ICON_SPACING;
        
        // Enemy icon circle
        sf::CircleShape enemyIcon(QUEUE_ICON_RADIUS);
        enemyIcon.setPosition(sf::Vector2f(iconX, 530.f));
        enemyIcon.setFillColor(sf::Color(200, 50, 50));
        enemyIcon.setOutlineThickness(2.0f);
        enemyIcon.setOutlineColor(sf::Color(100, 0, 0));
        window.draw(enemyIcon);
        
        // Enemy name (truncated)
        const std::string& fullName = enemyList[i].name;
        const std::string shortName = (fullName.length() > ENEMY_NAME_MAX_LEN) 
            ? fullName.substr(0, ENEMY_NAME_MAX_LEN) 
            : fullName;
        
        sf::Text enemyNameText(font, shortName, 8);
        const sf::FloatRect textBounds = enemyNameText.getLocalBounds();
        enemyNameText.setPosition(sf::Vector2f(
            iconX + QUEUE_ICON_RADIUS - textBounds.size.x / 2.f, 
            558.f
        ));
        enemyNameText.setFillColor(sf::Color(220, 220, 220));
        window.draw(enemyNameText);
    }
}

void UIManager::renderSkillHotkeys(sf::RenderWindow& window, const Player& /* player */, SkillTree* skillTree) {
    if (!skillTree || !fontLoaded) return;
    
    const float panelX = (SCREEN_WIDTH - HOTBAR_WIDTH) / 2.f;
    const sf::FloatRect hotbarRect(
        sf::Vector2f(panelX, HOTBAR_Y), 
        sf::Vector2f(HOTBAR_WIDTH, HOTBAR_HEIGHT)
    );
    
    UIVisualEffects::drawStyledPanel(
        window, hotbarRect,
        UIVisualEffects::PanelStyle::Bordered,
        sf::Color(18, 18, 25, 235),
        sf::Color(90, 90, 110)
    );
    
    // Panel title (centered)
    sf::Text title(font, "HOTBAR - Items & Skills", 10);
    const sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(sf::Vector2f(
        panelX + HOTBAR_WIDTH / 2.f - titleBounds.size.x / 2.f, 
        HOTBAR_Y + 2.f
    ));
    title.setFillColor(sf::Color(150, 150, 180));
    window.draw(title);
    
    // Skill slot configuration
    static const char* const SKILL_NAMES[] = {"Slash", "Fireball", "Shield", "Heal", "Dash"};
    static const char* const SKILL_KEYS[]  = {"1", "2", "3", "4", "5"};
    static const sf::Color SKILL_COLORS[] = {
        sf::Color(200, 50, 50),    // Slash - Red
        sf::Color(255, 150, 50),   // Fireball - Orange
        sf::Color(50, 150, 200),   // Shield - Blue
        sf::Color(50, 200, 100),   // Heal - Green
        sf::Color(200, 200, 50)    // Dash - Yellow
    };
    
    const float startX = panelX + 20.f;
    const float startY = HOTBAR_Y + 15.f;
    
    for (int i = 0; i < NUM_SKILL_SLOTS; ++i) {
        const float slotX = startX + static_cast<float>(i) * (SKILL_SLOT_SIZE + SKILL_SLOT_PADDING);
        
        // Slot background
        sf::RectangleShape slot(sf::Vector2f(SKILL_SLOT_SIZE, SKILL_SLOT_SIZE));
        slot.setPosition(sf::Vector2f(slotX, startY));
        slot.setFillColor(sf::Color(40, 40, 50));
        slot.setOutlineThickness(2.0f);
        slot.setOutlineColor(SKILL_COLORS[i]);
        window.draw(slot);
        
        // Skill icon (colored circle)
        sf::CircleShape skillIcon(SKILL_SLOT_SIZE * 0.3f);
        skillIcon.setPosition(sf::Vector2f(
            slotX + SKILL_SLOT_SIZE * 0.2f, 
            startY + SKILL_SLOT_SIZE * 0.2f
        ));
        skillIcon.setFillColor(SKILL_COLORS[i]);
        window.draw(skillIcon);
        
        // Hotkey label (top-left corner)
        sf::Text keyText(font, SKILL_KEYS[i], 10);
        keyText.setPosition(sf::Vector2f(slotX + 2.f, startY + 2.f));
        keyText.setFillColor(sf::Color::Yellow);
        window.draw(keyText);
        
        // Skill name (bottom, centered)
        sf::Text nameText(font, SKILL_NAMES[i], 8);
        const sf::FloatRect nameBounds = nameText.getLocalBounds();
        nameText.setPosition(sf::Vector2f(
            slotX + (SKILL_SLOT_SIZE - nameBounds.size.x) / 2.f, 
            startY + SKILL_SLOT_SIZE - 12.f
        ));
        nameText.setFillColor(sf::Color(180, 180, 180));
        window.draw(nameText);
    }
    
    // Item wheel hint
    sf::Text tabHint(font, "[TAB] Items", 10);
    const float tabX = startX + NUM_SKILL_SLOTS * (SKILL_SLOT_SIZE + SKILL_SLOT_PADDING) + 10.f;
    tabHint.setPosition(sf::Vector2f(tabX, startY + SKILL_SLOT_SIZE / 2.f - 5.f));
    tabHint.setFillColor(sf::Color(150, 150, 180));
    window.draw(tabHint);
}

// ═══════════════════════════════════════════════════════════════════════
// UI STATE MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════

namespace {
    constexpr float PI = 3.14159265358979f;
    constexpr float HUD_LERP_SPEED = 0.15f;
    constexpr int COOLDOWN_SEGMENTS = 32;
    constexpr float COMBO_SCALE_PER_HIT = 0.05f;
    constexpr int COMBO_THRESHOLD_HIGH = 5;
}

void UIManager::showMainMenu() { 
    hideAll();
    // Main menu state - actual rendering deferred to render loop
}

void UIManager::showHUD() { 
    hideAll();
    // HUD is always visible in render(), this resets panel visibility
}

void UIManager::showInventory() { inventoryVisible = true; }
void UIManager::showSkillTree() { skillTreeVisible = true; }

void UIManager::hideAll() { 
    inventoryVisible = false; 
    skillTreeVisible = false; 
}

void UIManager::toggleInventory() { 
    inventoryVisible = !inventoryVisible; 
    if (inventoryVisible) skillTreeVisible = false;  // Mutual exclusion
}

void UIManager::toggleSkillTree() { 
    skillTreeVisible = !skillTreeVisible; 
    if (skillTreeVisible) inventoryVisible = false;  // Mutual exclusion
}

void UIManager::toggleMiniMap() { 
    miniMapVisible = !miniMapVisible; 
}

void UIManager::toggleItemWheel(sf::Vector2f screenCenter, const Player* player) {
    if (itemWheel.isOpen()) {
        itemWheel.close();
    } else {
        itemWheel.setFont(&font);
        
        // Populate items from player inventory
        if (player) {
            std::vector<const ItemNew*> inventoryItems;
            player->getInventoryNew().traverse([&](const ItemNew& item) {
                inventoryItems.push_back(&item);
            });
            itemWheel.updateItems(inventoryItems);
        }
        
        itemWheel.open(screenCenter);
    }
}

void UIManager::updateHUD(const Player& player) {
    const float targetHealth = static_cast<float>(player.getHealth());
    const float targetMana = static_cast<float>(player.getMana());
    
    // Smooth lerp interpolation for bar animations
    displayedHealth += (targetHealth - displayedHealth) * HUD_LERP_SPEED;
    displayedMana += (targetMana - displayedMana) * HUD_LERP_SPEED;
}

void UIManager::updateInventory(const Player& /* player */) { 
    // Inventory state handled directly in renderInventoryPanel()
}

bool UIManager::loadFont(const std::string& path) { 
    return font.openFromFile(path); 
}

// ═══════════════════════════════════════════════════════════════════════
// DYNAMIC UI RENDER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════

void UIManager::renderRadialCooldown(sf::RenderWindow& window, sf::Vector2f center, 
                                      float radius, float progress, sf::Color color) {
    sf::VertexArray pie(sf::PrimitiveType::TriangleFan, COOLDOWN_SEGMENTS + 2);
    
    // Center vertex
    pie[0].position = center;
    pie[0].color = sf::Color(color.r, color.g, color.b, 150);
    
    // Arc vertices based on progress (0.0 to 1.0)
    const float angle = progress * 2.f * PI;
    for (int i = 0; i <= COOLDOWN_SEGMENTS; ++i) {
        const float theta = -PI / 2.f + (static_cast<float>(i) / COOLDOWN_SEGMENTS) * angle;
        pie[i + 1].position = sf::Vector2f(
            center.x + std::cos(theta) * radius,
            center.y + std::sin(theta) * radius
        );
        pie[i + 1].color = sf::Color(color.r, color.g, color.b, 100);
    }
    window.draw(pie);
    
    // Outline ring
    sf::CircleShape outline(radius);
    outline.setPosition(sf::Vector2f(center.x - radius, center.y - radius));
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineThickness(2.f);
    outline.setOutlineColor(sf::Color(color.r, color.g, color.b, 200));
    window.draw(outline);
}

void UIManager::renderComboCounter(sf::RenderWindow& window) {
    if (currentCombo < 2 || comboDisplayTimer <= 0.f) return;
    if (!fontLoaded) return;
    
    const float alpha = std::min(1.f, comboDisplayTimer / COMBO_FADE_TIME) * 255.f;
    const float scale = 1.f + (currentCombo * COMBO_SCALE_PER_HIT);
    const auto alphaU8 = static_cast<std::uint8_t>(alpha);
    
    const std::string comboText = "x" + std::to_string(currentCombo) + " COMBO!";
    const sf::Color comboColor = (currentCombo > COMBO_THRESHOLD_HIGH) 
        ? sf::Color(255, 100, 100, alphaU8)   // Red for high combos
        : sf::Color(255, 215, 0, alphaU8);    // Gold for normal combos
    
    sf::Text text(font, comboText, static_cast<unsigned int>(24 * scale));
    text.setPosition(sf::Vector2f(400.f - text.getLocalBounds().size.x / 2.f, 120.f));
    text.setFillColor(comboColor);
    text.setOutlineThickness(2.f);
    text.setOutlineColor(sf::Color(0, 0, 0, alphaU8));
    window.draw(text);
}

void UIManager::setCombo(int combo) {
    currentCombo = combo;
    comboDisplayTimer = COMBO_FADE_TIME;
}
