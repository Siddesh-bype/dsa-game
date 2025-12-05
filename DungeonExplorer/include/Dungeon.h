#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "DataStructures/Graph.h"

enum class TileType {
    Empty,
    Floor,
    Wall,
    Door,
    Treasure,
    Enemy,
    Start,
    Exit
};

struct Room {
    int id;
    int x, y;
    int width, height;
    TileType type;
    bool cleared;  // Track if room enemies are defeated
    
    Room(int id = 0, int x = 0, int y = 0, int w = 3, int h = 3, TileType t = TileType::Floor)
        : id(id), x(x), y(y), width(w), height(h), type(t), cleared(false) {}
};

// CHANGE: 2025-11-11 - Door structure for interactive doors
struct DoorData {
    int x, y;
    int roomA, roomB;  // Connected rooms
    bool isOpen;
    bool requiresKey;
    bool openOnClear;  // Opens when room is cleared
    
    DoorData(int posX, int posY, int rA, int rB, bool key = false, bool clear = false)
        : x(posX), y(posY), roomA(rA), roomB(rB), isOpen(false), requiresKey(key), openOnClear(clear) {}
};

// 🎮 DECORATIVE TILE SYSTEM - Environmental props and particles
struct DecorTile {
    int x, y;
    std::string type;  // "torch", "skull", "barrel", "crate", "bones", "cobweb"
    float animTimer = 0.f;
    
    DecorTile(int px, int py, const std::string& t) : x(px), y(py), type(t) {}
};

struct AmbientParticle {
    float x, y;
    float vx, vy;
    float lifetime;
    std::string type;  // "dust", "spark", "fog"
    float alpha = 255.f;
    
    AmbientParticle(float px, float py, const std::string& t)
        : x(px), y(py), vx(0), vy(-10.f), lifetime(3.f), type(t) {}
};

// Hash function for Room to use in Graph
namespace std {
    template<>
    struct hash<Room> {
        size_t operator()(const Room& r) const {
            return std::hash<int>()(r.id);
        }
    };
}

inline bool operator==(const Room& a, const Room& b) {
    return a.id == b.id;
}

class Dungeon {
private:
    static const int GRID_WIDTH = 25;
    static const int GRID_HEIGHT = 18;
    
    // ═══════════════════════════════════════════════════════════════════════
    // CONFIGURATION CONSTANTS - Centralized magic numbers
    // ═══════════════════════════════════════════════════════════════════════
    
    // Room generation
    static constexpr int ROOM_MARGIN = 2;
    static constexpr int ROOM_SAFE_ZONE = 10;
    static constexpr int MIN_ROOM_SIZE = 3;
    static constexpr int MAX_ROOM_SIZE_RANGE = 4;
    
    // Pathfinding
    static constexpr int INFINITE_DISTANCE = 999999;
    
    // Texture variation (deterministic pseudo-random)
    static constexpr int VARIANT_PRIME_X = 7;
    static constexpr int VARIANT_PRIME_Y = 13;
    static constexpr int MAX_FLOOR_VARIANTS = 5;
    
    // Door generation
    static constexpr int DOOR_KEY_CHANCE_PERCENT = 20;
    
    std::vector<std::vector<TileType>> grid;
    Graph<int> roomGraph;  // Rooms connected as graph
    std::vector<Room> rooms;
    std::vector<DoorData> doors;  // Track all doors in dungeon
    
    int startRoomId;
    int currentRoomId;
    int stairsX, stairsY;  // TASK D: Position of stairs to next floor
    
    sf::Texture floorTexture;
    sf::Texture wallTexture;
    
    // 🎮 Decorative elements
    std::vector<DecorTile> decorTiles;
    mutable std::vector<AmbientParticle> ambientParticles;
    
    // Decoration constants
    static constexpr float DECOR_CHANCE = 0.15f;  // 15% chance per floor tile
    static constexpr int MAX_AMBIENT_PARTICLES = 50;
    static constexpr float PARTICLE_SPAWN_INTERVAL = 0.5f;
    mutable float particleSpawnTimer = 0.f;
    
    void generateRooms(int numRooms);
    void connectRooms();
    void fillGrid();
    void carveHorizontalCorridor(const Room& r1, const Room& r2);
    void carveVerticalCorridor(const Room& r1, const Room& r2);
    
    // ═══════════════════════════════════════════════════════════════════════
    // HELPER FUNCTIONS - Reduce code duplication
    // ═══════════════════════════════════════════════════════════════════════
    bool isInBounds(int x, int y) const;
    std::vector<std::pair<int, int>> getNeighbors(int x, int y) const;
    std::vector<std::pair<int, int>> getValidNeighbors(int x, int y) const;
    std::string getFloorTextureKey(int x, int y, int currentFloor) const;
    int getDeterministicVariant(int x, int y, int maxVariants) const;

public:
    Dungeon();
    
    void generate(int numRooms = 8);
    
    TileType getTile(int x, int y) const;
    void setTile(int x, int y, TileType type);
    
    bool isWalkable(int x, int y) const;
    std::pair<int, int> findNextMoveToPlayer(int enemyX, int enemyY, int playerX, int playerY) const; // BFS (Standard)
    std::pair<int, int> findNextMoveToPlayerAStar(int enemyX, int enemyY, int playerX, int playerY) const; // A* (Smart)
    std::pair<int, int> findNextMoveRandom(int enemyX, int enemyY) const; // Random (Erratic)
    std::pair<int, int> findNextMoveFlank(int enemyX, int enemyY, int playerX, int playerY) const; // Flank (Strategic)
    
    std::vector<int> getReachableRooms(int roomId);
    std::vector<int> findShortestPath(int fromRoom, int toRoom);
    
    void visualizeBFS(int startRoom);
    void visualizeDFS(int startRoom);
    void visualizeDijkstra(int startRoom);
    
    void render(sf::RenderWindow& window, float tileSize, int currentFloor = 1) const;
    void renderDecorations(sf::RenderWindow& window, float tileSize) const;
    void updateAmbientParticles(float deltaTime) const;
    void generateDecorations();
    
    int getWidth() const { return GRID_WIDTH; }
    int getHeight() const { return GRID_HEIGHT; }
    
    const Graph<int>& getGraph() const { return roomGraph; }
    const std::vector<Room>& getRooms() const { return rooms; }
    
    // TASK D: Stairs access for floor transitions
    int getStairsX() const { return stairsX; }
    int getStairsY() const { return stairsY; }
    bool isAdjacentToStairs(int x, int y) const {
        return (std::abs(x - stairsX) <= 1 && std::abs(y - stairsY) <= 1);
    }
    
    // CHANGE: 2025-11-11 - Door management
    const std::vector<DoorData>& getDoors() const { return doors; }
    DoorData* getDoorAt(int x, int y);
    const DoorData* getDoorAt(int x, int y) const;
    void openDoor(int x, int y);
    void closeDoor(int x, int y);
    bool canPassDoor(int x, int y) const;
    void checkRoomClear(int roomId, int enemyCount);
    
    // CHANGE: 2025-11-14 - Helper to find which room contains a position
    int getRoomIdAt(int x, int y) const;
    
    // CHANGE: 2025-11-14 - Door spawning support
    void spawnDoors();  // Spawn doors during level generation
    void clearDoors();  // Clear all doors for floor reset
};
