#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Player.h"  // Include Player.h for Position struct

// Forward declarations
class Dungeon;
class UIManager;
class Renderer;
class EnemyManager;
class SkillTree;
class DungeonLevelManager;  // 10-floor system
class Loot;  // NEW: Loot entity for items on ground
class Shop;  // NEW: Shop system for buying items
struct ItemNew;  // NEW: Item definition

// ✨ Visual Effect for combat feedback
struct CombatEffect {
    std::string effectType;  // "swing", "explosion", "arrow", etc.
    float x, y;
    float lifetime;
    float maxLifetime;
    
    CombatEffect(const std::string& type, float posX, float posY, float duration = 0.3f)
        : effectType(type), x(posX), y(posY), lifetime(duration), maxLifetime(duration) {}
};

// CHANGE: 2025-11-10 - Door structure for interactive doors
struct Door {
    int x, y;
    bool isOpen;
    bool openOnClear;  // Open when room cleared
    bool requiresKey;  // Requires key item
    
    Door(int posX, int posY, bool clear = false, bool key = false)
        : x(posX), y(posY), isOpen(false), openOnClear(clear), requiresKey(key) {}
};

class Game {
private:
    sf::RenderWindow window;
    
    std::unique_ptr<Player> player;
    std::unique_ptr<Dungeon> dungeon;
    std::unique_ptr<UIManager> uiManager;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<EnemyManager> enemyManager;
    std::unique_ptr<SkillTree> skillTree;
    std::unique_ptr<DungeonLevelManager> levelManager;  // NEW: 10-floor manager
    std::unique_ptr<Shop> shop;  // NEW: Shop system
    
    bool isRunning;
    bool isPaused;
    int currentFloor;
    Position exitStairsPosition;
    
    // CHANGE: 2025-11-10 - Loot system with items on ground
    std::vector<Loot> loots;  // Items on the ground
    std::vector<Door> doors;  // Interactive doors
    
    // ✨ Active combat effects
    std::vector<CombatEffect> activeEffects;
    
    // Debug flags
    bool debugShowBoundingBoxes = false;  // F3: Show collision boxes
    bool debugRetroMode = false;          // F4: 1-bit retro graphics
    
    enum class GameState {
        MainMenu,
        Playing,
        Inventory,
        SkillTree,
        Paused,
        GameOver,
        Victory  // NEW: Victory state when completing all 10 floors
    };
    
    GameState currentState;
    
    void processEvents();
    void update(float deltaTime);
    void render();
    
    void handleInput(const sf::Event& event);  // Fixed for SFML 3.x
    void updateMainMenu(float deltaTime);
    void updatePlaying(float deltaTime);
    void attackNearestEnemy();  // Combat system
    void activateSkill(int hotkey);  // Activate skill by hotkey 1-5
    void moveEnemiesWithBFS();  // Enemy AI pathfinding
    void renderGameOverScreen();  // Game over UI
    void nextFloor();  // Descend to next floor
    void spawnEnemiesForFloor(int floor);  // Spawn scaled enemies
    void checkExitAccess();  // Check if all enemies defeated
    void dropItemFromEnemy(const std::string& enemyName, int x, int y);  // Drop items on defeat
    
    // CHANGE: 2025-11-10 - Loot and door system
    void spawnLootAt(const sf::Vector2i& tilePos, const ItemNew& item);  // Spawn loot on ground
    void pickupLoot();  // Pick up loot when E pressed
    void updateLoots(float deltaTime);  // Update loot animations
    void tryOpenDoor();  // Open door when E pressed adjacent
    void checkRoomClearDoors();  // Auto-open doors when room cleared
    
    // ✨ Combat effect system
    void addCombatEffect(const std::string& effectType, float x, float y, float duration = 0.3f);
    void updateCombatEffects(float deltaTime);
    void renderCombatEffects();
    
public:
    Game();
    ~Game();
    
    void run();
    void initialize();
    
    void setState(GameState state);
    GameState getState() const { return currentState; }
    
    sf::RenderWindow& getWindow() { return window; }
    // tgui::Gui& getGui() { return gui; }  // Disabled - TGUI not compatible
};
