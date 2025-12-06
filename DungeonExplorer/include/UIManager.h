#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Player.h"
#include "Dungeon.h"
#include "DSAVisualizer.h"
#include "InventoryRenderer.h"
#include "ItemWheel.h"

class Game;
class EnemyManager;

struct FloatingText {
    std::string text;
    sf::Vector2f position;
    sf::Color color;
    float lifetime;
    float maxLifetime;
    
    FloatingText(const std::string& txt, sf::Vector2f pos, sf::Color col, float life = 1.5f)
        : text(txt), position(pos), color(col), lifetime(life), maxLifetime(life) {}
};

class UIManager {
private:
    Game* game;
    
    sf::Font font;
    bool fontLoaded;
    
    // DSA Visualizer
    std::unique_ptr<DSAVisualizer> dsaViz;
    float animationTime;
    
    // Inventory Renderer
    std::unique_ptr<InventoryRenderer> invRenderer;
    int selectedInventoryIndex;
    int hoveredInventoryIndex;
    sf::Vector2f lastMousePos;
    
    // Combat feedback
    std::vector<FloatingText> floatingTexts;
    float screenFlashTimer;
    sf::Color screenFlashColor;
    
    // ═══════════════════════════════════════════════════════════════════════
    // SMOOTH UI ANIMATION STATE
    // ═══════════════════════════════════════════════════════════════════════
    float healthBarProgress;
    float xpBarProgress;
    float displayedHealth = 100.f;   // Smooth interpolated health
    float displayedMana = 50.f;      // Smooth interpolated mana
    int currentCombo = 0;            // For combo display
    float comboDisplayTimer = 0.f;   // Fade out timer for combo
    
    // Animation constants
    static constexpr float BAR_LERP_SPEED = 8.0f;
    static constexpr float COMBO_FADE_TIME = 2.0f;
    
    // HUD elements
    sf::RectangleShape hudBackground;
    sf::RectangleShape healthBarBg;
    sf::RectangleShape healthBarFg;
    sf::RectangleShape xpBarBg;
    sf::RectangleShape xpBarFg;
    
    bool inventoryVisible;
    bool skillTreeVisible;
    bool miniMapVisible;
    
    // CHANGE: 2025-12-06 - Item Wheel for radial item selection
    ItemWheel itemWheel;
    
    // Helper rendering methods
    void renderHUD(sf::RenderWindow& window, const Player& player, class SkillTree* skillTree, int currentFloor = 1);
    void renderMiniMap(sf::RenderWindow& window, const Dungeon& dungeon, const Player& player, const EnemyManager& enemies);
    void renderInventoryPanel(sf::RenderWindow& window, const Player& player);
    void renderSkillTreePanel(sf::RenderWindow& window);
    void renderTurnQueue(sf::RenderWindow& window, const EnemyManager& enemies);
    void renderSkillHotkeys(sf::RenderWindow& window, const Player& player, class SkillTree* skillTree);
    void renderFloatingTexts(sf::RenderWindow& window);
    void renderScreenFlash(sf::RenderWindow& window);
    void renderCharacterNames(sf::RenderWindow& window, const Player& player, const EnemyManager& enemies, float tileSize);
    void renderRadialCooldown(sf::RenderWindow& window, sf::Vector2f center, float radius, float progress, sf::Color color);
    void renderComboCounter(sf::RenderWindow& window);
    
    // Helper: create text with common settings
    sf::Text makeText(const std::string& str, int size, sf::Vector2f pos, 
                      sf::Color fill = sf::Color::White, float outline = 1.f) {
        sf::Text text(font, str, size);
        text.setPosition(pos);
        text.setFillColor(fill);
        text.setOutlineThickness(outline);
        text.setOutlineColor(sf::Color::Black);
        return text;
    }
    
    // UI Constants
    static constexpr float TILE_SIZE = 32.f;
    static constexpr float HUD_WIDTH = 400.f;
    static constexpr float HUD_HEIGHT = 70.f;
    static constexpr float HEALTH_BAR_WIDTH = 200.f;
    static constexpr float HEALTH_BAR_HEIGHT = 20.f;
    static constexpr float XP_BAR_WIDTH = 150.f;
    static constexpr float XP_BAR_HEIGHT = 15.f;
    
    void createMainMenu();
    void createHUD();
    void createInventoryScreen();
    void createSkillTreeScreen();

public:
    UIManager(Game* game);
    
    void initialize();
    void update(float deltaTime);
    void updateMousePosition(sf::Vector2f mousePos);
    bool handleMouseClick(sf::Vector2f mousePos);
    void render(sf::RenderWindow& window, const Player& player, const Dungeon& dungeon, const EnemyManager& enemies, int currentFloor = 1);
    
    // Split rendering for layer control
    void renderGraphPathsOnly(sf::RenderWindow& window, const Dungeon& dungeon);
    void renderUI(sf::RenderWindow& window, const Player& player, const Dungeon& dungeon, const EnemyManager& enemies, class SkillTree* skillTree, int currentFloor = 1);
    void renderContextualPrompt(sf::RenderWindow& window, const std::string& prompt);  // TASK D & I: Public for Game.cpp
    
    // Combat feedback
    void addFloatingText(const std::string& text, float x, float y, sf::Color color);
    void triggerScreenFlash(sf::Color color, float duration = 0.3f);
    void setCombo(int combo);  // Update combo display
    
    void showMainMenu();
    void showHUD();
    void showInventory();
    void showSkillTree();
    void hideAll();
    void toggleInventory();
    void toggleSkillTree();
    void toggleMiniMap();
    
    // CHANGE: 2025-12-06 - Item wheel control
    void toggleItemWheel(sf::Vector2f screenCenter, const Player* player = nullptr);
    bool isItemWheelOpen() const { return itemWheel.isOpen(); }
    ItemWheel& getItemWheel() { return itemWheel; }
    
    
    void updateHUD(const Player& player);
    void updateInventory(const Player& player);
    
    // Inventory selection accessors
    int getSelectedInventoryIndex() const { return selectedInventoryIndex; }
    void selectInventoryIndex(int index) { selectedInventoryIndex = index; }
    void hideInventory() { inventoryVisible = false; }
    void hideSkillTree() { skillTreeVisible = false; }
    
    bool loadFont(const std::string& path);
    const sf::Font& getFont() const { return font; }
    bool isFontLoaded() const { return fontLoaded; }
    
private:
    int swapSourceIndex = -1;  // -1 means no item selected for swapping
};
