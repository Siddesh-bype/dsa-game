#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Player.h"
#include "Dungeon.h"
#include "DSAVisualizer.h"

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
    
    // Combat feedback
    std::vector<FloatingText> floatingTexts;
    float screenFlashTimer;
    sf::Color screenFlashColor;
    
    // HUD elements
    sf::RectangleShape hudBackground;
    sf::RectangleShape healthBarBg;
    sf::RectangleShape healthBarFg;
    sf::RectangleShape xpBarBg;
    sf::RectangleShape xpBarFg;
    
    // Panel visibility flags
    bool inventoryVisible;
    bool skillTreeVisible;
    bool miniMapVisible;
    
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
    
    void createMainMenu();
    void createHUD();
    void createInventoryScreen();
    void createSkillTreeScreen();

public:
    UIManager(Game* game);
    
    void initialize();
    void update(float deltaTime);
    void render(sf::RenderWindow& window, const Player& player, const Dungeon& dungeon, const EnemyManager& enemies, int currentFloor = 1);
    
    // Split rendering for layer control
    void renderGraphPathsOnly(sf::RenderWindow& window, const Dungeon& dungeon);
    void renderUI(sf::RenderWindow& window, const Player& player, const Dungeon& dungeon, const EnemyManager& enemies, class SkillTree* skillTree, int currentFloor = 1);
    void renderContextualPrompt(sf::RenderWindow& window, const std::string& prompt);  // TASK D & I: Public for Game.cpp
    
    // Combat feedback
    void addFloatingText(const std::string& text, float x, float y, sf::Color color);
    void triggerScreenFlash(sf::Color color, float duration = 0.3f);
    
    void showMainMenu();
    void showHUD();
    void showInventory();
    void showSkillTree();
    void hideAll();
    void toggleInventory();
    void toggleSkillTree();
    void toggleMiniMap();
    
    void updateHUD(const Player& player);
    void updateInventory(const Player& player);
    
    bool loadFont(const std::string& path);
    const sf::Font& getFont() const { return font; }
    bool isFontLoaded() const { return fontLoaded; }
};
