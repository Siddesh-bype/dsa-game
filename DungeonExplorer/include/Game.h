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
    float totalTime;  // Track total elapsed time for animations/lighting
    
    // CHANGE: 2025-11-10 - Loot system with items on ground
    std::vector<Loot> loots;  // Items on the ground
    std::vector<Door> doors;  // Interactive doors
    
    // ✨ Active combat effects
    std::vector<CombatEffect> activeEffects;
    
    // ═══════════════════════════════════════════════════════════════════════
    // DYNAMIC GAME STATE - Screen effects, combos, and smooth movement
    // ═══════════════════════════════════════════════════════════════════════
    float screenShakeTimer = 0.f;
    float screenShakeIntensity = 0.f;
    int comboCounter = 0;
    float comboTimer = 0.f;
    float targetFPS = 60.f;
    float currentFPS = 60.f;
    sf::Vector2f cameraOffset = {0.f, 0.f};  // For smooth camera
    sf::Vector2f cameraTarget = {0.f, 0.f};
    
    // Debug flags
    bool debugShowBoundingBoxes = false;  // F3: Show collision boxes
    bool debugRetroMode = false;          // F4: 1-bit retro graphics
    
    // ═══════════════════════════════════════════════════════════════════════
    // GAME CONFIGURATION CONSTANTS
    // ═══════════════════════════════════════════════════════════════════════
    
    // Window settings
    static constexpr unsigned int WINDOW_WIDTH = 800;
    static constexpr unsigned int WINDOW_HEIGHT = 600;
    static constexpr unsigned int FRAMERATE_MIN = 30;
    static constexpr unsigned int FRAMERATE_MAX = 60;
    static constexpr float TILE_SIZE = 32.0f;
    static constexpr float MAX_DELTA_TIME = 0.1f;
    static constexpr float CAMERA_LERP_SPEED = 8.0f;
    static constexpr float SCREEN_SHAKE_DECAY = 5.0f;
    static constexpr float COMBO_TIMEOUT = 2.0f;
    static constexpr float COMBO_DAMAGE_MULT = 0.1f;  // +10% per combo
    
    // Combat settings
    static constexpr int BASE_XP_GAIN = 25;
    static constexpr int XP_PER_FLOOR = 5;
    static constexpr int HEALING_POTION_AMOUNT = 50;
    static constexpr int ATTACK_RANGE_TILES = 2;
    
    // UI offsets
    static constexpr float FLOATING_TEXT_OFFSET_Y = -10.0f;
    static constexpr float FLOATING_TEXT_OFFSET_Y_DOUBLE = -20.0f;
    static constexpr float FLOATING_TEXT_OFFSET_Y_TRIPLE = -30.0f;
    
    // Effect durations
    static constexpr float EFFECT_DURATION_SHORT = 0.3f;
    static constexpr float EFFECT_DURATION_MEDIUM = 0.4f;
    static constexpr float EFFECT_DURATION_LONG = 0.5f;
    static constexpr float EFFECT_DURATION_EXTRA_LONG = 0.6f;
    
    // Skill system
    static constexpr int STARTING_SKILL_POINTS = 3;
    
    // Floor progression
    static constexpr int STARTING_FLOOR = 1;
    static constexpr int MAX_FLOORS = 10;
    
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
    // CHANGE: 2025-11-14 - Door functions integrated into E key handler (kept for backwards compatibility)
    void tryOpenDoor();  // DEPRECATED: Now integrated into E key handler
    void checkRoomClearDoors();  // DEPRECATED: Now integrated into moveEnemiesWithBFS
    
    // ✨ Combat effect system
    void addCombatEffect(const std::string& effectType, float x, float y, float duration = 0.3f);
    void updateCombatEffects(float deltaTime);
    void renderCombatEffects();
    
    // 🎮 Dynamic game systems
    void applyScreenShake(float intensity, float duration = 0.2f);
    void updateScreenShake(float deltaTime);
    void updateCamera(float deltaTime);
    void incrementCombo();
    void resetCombo();
    void setTargetFPS(float fps);
    
public:
    Game();
    ~Game();
    
    void run();
    void initialize();
    
    // Accessors
    Player* getPlayer() const { return player.get(); }
    Dungeon* getDungeon() const { return dungeon.get(); }
    EnemyManager* getEnemyManager() const { return enemyManager.get(); }
    SkillTree* getSkillTree() const { return skillTree.get(); }
    
    void setState(GameState state);
    GameState getState() const { return currentState; }
    
    sf::RenderWindow& getWindow() { return window; }
    // tgui::Gui& getGui() { return gui; }  // Disabled - TGUI not compatible
};
