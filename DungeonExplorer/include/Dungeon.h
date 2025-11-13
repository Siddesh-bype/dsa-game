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
    
    std::vector<std::vector<TileType>> grid;
    Graph<int> roomGraph;  // Rooms connected as graph
    std::vector<Room> rooms;
    std::vector<DoorData> doors;  // Track all doors in dungeon
    
    int startRoomId;
    int currentRoomId;
    int stairsX, stairsY;  // TASK D: Position of stairs to next floor
    
    sf::Texture floorTexture;
    sf::Texture wallTexture;
    
    void generateRooms(int numRooms);
    void connectRooms();
    void fillGrid();
    void carveHorizontalCorridor(const Room& r1, const Room& r2);
    void carveVerticalCorridor(const Room& r1, const Room& r2);

public:
    Dungeon();
    
    void generate(int numRooms = 8);
    
    TileType getTile(int x, int y) const;
    void setTile(int x, int y, TileType type);
    
    bool isWalkable(int x, int y) const;
    std::pair<int, int> findNextMoveToPlayer(int enemyX, int enemyY, int playerX, int playerY) const;
    
    std::vector<int> getReachableRooms(int roomId);
    std::vector<int> findShortestPath(int fromRoom, int toRoom);
    
    void visualizeBFS(int startRoom);
    void visualizeDFS(int startRoom);
    void visualizeDijkstra(int startRoom);
    
    void render(sf::RenderWindow& window, float tileSize, int currentFloor = 1) const;
    
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
    void openDoor(int x, int y);
    void closeDoor(int x, int y);
    bool canPassDoor(int x, int y) const;
    void checkRoomClear(int roomId, int enemyCount);
};
