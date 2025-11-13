// CHANGE: 2025-11-10 — TASK D: Stairs placement for floor transitions
// - Added stairs position tracking
// - Place stairs in furthest room from entrance using Dijkstra
// - Added debug logging for stairs placement

#include "Dungeon.h"
#include "AssetManager.h"
#include <iostream>
#include <random>
#include <ctime>
#include <queue>
#include <map>

Dungeon::Dungeon() : startRoomId(0), currentRoomId(0), stairsX(-1), stairsY(-1) {
    grid.resize(GRID_HEIGHT, std::vector<TileType>(GRID_WIDTH, TileType::Empty));
}

void Dungeon::generate(int numRooms) {
    std::cout << "\n[Dungeon] Generating dungeon with " << numRooms << " rooms..." << std::endl;
    std::cout << "[Dungeon] Grid size: " << GRID_WIDTH << "x" << GRID_HEIGHT << " tiles" << std::endl;
    
    // Clear previous data
    grid.clear();
    grid.resize(GRID_HEIGHT, std::vector<TileType>(GRID_WIDTH, TileType::Wall));
    rooms.clear();
    roomGraph.clear();
    
    generateRooms(numRooms);
    connectRooms();
    fillGrid();
    
    // TASK D: Place stairs in furthest room from entrance
    if (rooms.size() > 1) {
        // Use Dijkstra to find furthest room
        std::unordered_map<int, int> distances = roomGraph.dijkstra(startRoomId);
        int furthestRoom = startRoomId;
        int maxDistance = 0;
        
        for (const auto& [roomId, dist] : distances) {
            if (dist > maxDistance && dist < 999999) {
                maxDistance = dist;
                furthestRoom = roomId;
            }
        }
        
        // Place stairs as a 2x2 block in center of furthest room for better visibility
        for (const auto& room : rooms) {
            if (room.id == furthestRoom) {
                // Center position
                int centerX = room.x + room.width / 2;
                int centerY = room.y + room.height / 2;
                
                // Place 2x2 stairs block
                stairsX = centerX;
                stairsY = centerY;
                
                // Place stairs tiles in a 2x2 pattern
                for (int dy = 0; dy < 2; dy++) {
                    for (int dx = 0; dx < 2; dx++) {
                        int sx = centerX + dx;
                        int sy = centerY + dy;
                        
                        // Ensure within room bounds
                        if (sx < room.x + room.width && sy < room.y + room.height) {
                            grid[sy][sx] = TileType::Exit;
                        }
                    }
                }
                
                std::cout << "[DEBUG] Stairs (2x2) placed at (" << stairsX << ", " << stairsY << ") in room " << furthestRoom << std::endl;
                break;
            }
        }
    }
    
    std::cout << "[Dungeon] Generation complete!" << std::endl;
}

void Dungeon::generateRooms(int numRooms) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    for (int i = 0; i < numRooms; i++) {
        int x = 2 + (std::rand() % (GRID_WIDTH - 10));  // More margin
        int y = 2 + (std::rand() % (GRID_HEIGHT - 10));
        int w = 3 + (std::rand() % 4);
        int h = 3 + (std::rand() % 4);
        
        // Ensure room fits with proper bounds checking
        if (x + w >= GRID_WIDTH - 1) {
            w = GRID_WIDTH - x - 2;
        }
        if (y + h >= GRID_HEIGHT - 1) {
            h = GRID_HEIGHT - y - 2;
        }
        
        // Ensure minimum room size
        if (w < 3) w = 3;
        if (h < 3) h = 3;
        
        TileType type = TileType::Floor;
        if (i == 0) type = TileType::Start;
        else if (i == numRooms - 1) type = TileType::Exit;
        
        Room room(i, x, y, w, h, type);
        rooms.push_back(room);
        roomGraph.addVertex(i);
        
        std::cout << "[Dungeon] Created room " << i << " at (" << x << ", " << y << ") size " << w << "x" << h << std::endl;
    }
    
    if (!rooms.empty()) {
        startRoomId = rooms[0].id;
        currentRoomId = startRoomId;
    }
}

void Dungeon::connectRooms() {
    // Safety check
    if (rooms.size() < 2) {
        std::cout << "[Dungeon] Not enough rooms to connect (need at least 2)" << std::endl;
        return;
    }
    
    // Connect rooms in sequence and add some random connections
    for (size_t i = 0; i < rooms.size() - 1; i++) {
        int weight = std::abs(rooms[i].x - rooms[i+1].x) + std::abs(rooms[i].y - rooms[i+1].y);
        roomGraph.addBidirectionalEdge(rooms[i].id, rooms[i+1].id, weight);
    }
    
    // Add some random connections
    if (rooms.size() > 3) {
        for (size_t i = 0; i < rooms.size() / 2; i++) {
            int r1 = std::rand() % rooms.size();
            int r2 = std::rand() % rooms.size();
            if (r1 != r2) {
                int weight = std::abs(rooms[r1].x - rooms[r2].x) + std::abs(rooms[r1].y - rooms[r2].y);
                roomGraph.addBidirectionalEdge(rooms[r1].id, rooms[r2].id, weight);
            }
        }
    }
}

void Dungeon::fillGrid() {
    // Fill rooms into grid
    for (const auto& room : rooms) {
        for (int y = room.y; y < room.y + room.height && y < GRID_HEIGHT; y++) {
            for (int x = room.x; x < room.x + room.width && x < GRID_WIDTH; x++) {
                grid[y][x] = room.type;
            }
        }
    }
    
    // CHANGE: 2025-11-11 - Clear doors before carving new ones
    doors.clear();
    
    // Carve corridors between connected rooms
    for (size_t i = 0; i + 1 < rooms.size(); i++) {
        carveHorizontalCorridor(rooms[i], rooms[i+1]);
        carveVerticalCorridor(rooms[i], rooms[i+1]);
    }
    
    std::cout << "[Dungeon] Generated " << doors.size() << " doors" << std::endl;
}

void Dungeon::carveHorizontalCorridor(const Room& r1, const Room& r2) {
    int x1 = r1.x + r1.width / 2;
    int x2 = r2.x + r2.width / 2;
    int y = r1.y + r1.height / 2;
    
    int startX = std::min(x1, x2);
    int endX = std::max(x1, x2);
    
    for (int x = startX; x <= endX; x++) {
        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
            if (grid[y][x] == TileType::Wall || grid[y][x] == TileType::Empty) {
                grid[y][x] = TileType::Floor;
            }
        }
    }
    
    // CHANGE: 2025-11-11 - Place doors ONLY at room boundaries
    // Check left side of r1
    if (x1 < x2 && r1.x + r1.width == startX + 1) {
        int doorX = r1.x + r1.width;
        int doorY = r1.y + r1.height / 2;
        if (doorX >= 0 && doorX < GRID_WIDTH && doorY >= 0 && doorY < GRID_HEIGHT) {
            grid[doorY][doorX] = TileType::Door;
            doors.push_back(DoorData(doorX, doorY, r1.id, r2.id, false, true));
            std::cout << "[DEBUG] Door placed at (" << doorX << ", " << doorY << ") between rooms " << r1.id << " and " << r2.id << std::endl;
        }
    }
    
    // Check left side of r2
    if (x2 < x1 && r2.x + r2.width == startX + 1) {
        int doorX = r2.x + r2.width;
        int doorY = r2.y + r2.height / 2;
        if (doorX >= 0 && doorX < GRID_WIDTH && doorY >= 0 && doorY < GRID_HEIGHT) {
            grid[doorY][doorX] = TileType::Door;
            doors.push_back(DoorData(doorX, doorY, r1.id, r2.id, false, true));
            std::cout << "[DEBUG] Door placed at (" << doorX << ", " << doorY << ") between rooms " << r1.id << " and " << r2.id << std::endl;
        }
    }
}

void Dungeon::carveVerticalCorridor(const Room& r1, const Room& r2) {
    int y1 = r1.y + r1.height / 2;
    int y2 = r2.y + r2.height / 2;
    int x = r2.x + r2.width / 2;
    
    int startY = std::min(y1, y2);
    int endY = std::max(y1, y2);
    
    for (int y = startY; y <= endY; y++) {
        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
            if (grid[y][x] == TileType::Wall || grid[y][x] == TileType::Empty) {
                grid[y][x] = TileType::Floor;
            }
        }
    }
    
    // CHANGE: 2025-11-11 - Place doors ONLY at room boundaries
    // Check top/bottom of r2
    if (y1 < y2 && r2.y == startY) {
        int doorX = r2.x + r2.width / 2;
        int doorY = r2.y - 1;
        if (doorX >= 0 && doorX < GRID_WIDTH && doorY >= 0 && doorY < GRID_HEIGHT) {
            grid[doorY][doorX] = TileType::Door;
            doors.push_back(DoorData(doorX, doorY, r1.id, r2.id, false, true));
            std::cout << "[DEBUG] Door placed at (" << doorX << ", " << doorY << ") between rooms " << r1.id << " and " << r2.id << std::endl;
        }
    }
    
    if (y2 < y1 && r2.y + r2.height == endY) {
        int doorX = r2.x + r2.width / 2;
        int doorY = r2.y + r2.height;
        if (doorX >= 0 && doorX < GRID_WIDTH && doorY >= 0 && doorY < GRID_HEIGHT) {
            grid[doorY][doorX] = TileType::Door;
            doors.push_back(DoorData(doorX, doorY, r1.id, r2.id, false, true));
            std::cout << "[DEBUG] Door placed at (" << doorX << ", " << doorY << ") between rooms " << r1.id << " and " << r2.id << std::endl;
        }
    }
}

TileType Dungeon::getTile(int x, int y) const {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
        return TileType::Wall;
    }
    return grid[y][x];
}

void Dungeon::setTile(int x, int y, TileType type) {
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
        grid[y][x] = type;
    }
}

bool Dungeon::isWalkable(int x, int y) const {
    TileType tile = getTile(x, y);
    
    // CHANGE: 2025-11-11 - Check if door is passable
    if (tile == TileType::Door) {
        return canPassDoor(x, y);
    }
    
    return tile != TileType::Empty && tile != TileType::Wall;
}

std::pair<int, int> Dungeon::findNextMoveToPlayer(int enemyX, int enemyY, int playerX, int playerY) const {
    // If already at player position, don't move
    if (enemyX == playerX && enemyY == playerY) {
        return {enemyX, enemyY};
    }
    
    // BFS to find shortest path to player
    std::queue<std::pair<int, int>> frontier;
    std::map<std::pair<int, int>, std::pair<int, int>> cameFrom;
    
    frontier.push({enemyX, enemyY});
    cameFrom[{enemyX, enemyY}] = {-1, -1};  // Start has no parent
    
    bool found = false;
    
    // BFS search
    while (!frontier.empty() && !found) {
        auto current = frontier.front();
        frontier.pop();
        
        // Check all 4 directions (up, down, left, right)
        std::vector<std::pair<int, int>> neighbors = {
            {current.first, current.second - 1},  // Up
            {current.first, current.second + 1},  // Down
            {current.first - 1, current.second},  // Left
            {current.first + 1, current.second}   // Right
        };
        
        for (const auto& next : neighbors) {
            // Check if already visited
            if (cameFrom.find(next) != cameFrom.end()) {
                continue;
            }
            
            // Check if walkable
            if (!isWalkable(next.first, next.second)) {
                continue;
            }
            
            frontier.push(next);
            cameFrom[next] = current;
            
            // Found the player!
            if (next.first == playerX && next.second == playerY) {
                found = true;
                break;
            }
        }
    }
    
    // If no path found, stay in place
    if (!found || cameFrom.find({playerX, playerY}) == cameFrom.end()) {
        return {enemyX, enemyY};
    }
    
    // Reconstruct path from player back to enemy
    std::pair<int, int> current = {playerX, playerY};
    std::pair<int, int> nextMove = current;
    
    while (cameFrom[current].first != -1) {
        nextMove = current;
        current = cameFrom[current];
        
        // If we're one step from the enemy, return that step
        if (current.first == enemyX && current.second == enemyY) {
            return nextMove;
        }
    }
    
    // Fallback: stay in place
    return {enemyX, enemyY};
}

std::vector<int> Dungeon::getReachableRooms(int roomId) {
    return roomGraph.bfs(roomId);
}

std::vector<int> Dungeon::findShortestPath(int fromRoom, int toRoom) {
    auto distances = roomGraph.dijkstra(fromRoom);
    std::vector<int> path;
    
    // Simple path reconstruction (just return rooms in distance order)
    for (const auto& pair : distances) {
        if (pair.second < std::numeric_limits<int>::max()) {
            path.push_back(pair.first);
        }
    }
    
    return path;
}

void Dungeon::visualizeBFS(int startRoom) {
    std::cout << "\n[Dungeon] Visualizing BFS from room " << startRoom << std::endl;
    auto visited = roomGraph.bfs(startRoom);
    
    for (int roomId : visited) {
        std::cout << "  -> Visited room " << roomId << std::endl;
    }
}

void Dungeon::visualizeDFS(int startRoom) {
    std::cout << "\n[Dungeon] Visualizing DFS from room " << startRoom << std::endl;
    auto visited = roomGraph.dfs(startRoom);
    
    for (int roomId : visited) {
        std::cout << "  -> Visited room " << roomId << std::endl;
    }
}

void Dungeon::visualizeDijkstra(int startRoom) {
    std::cout << "\n[Dungeon] Visualizing Dijkstra from room " << startRoom << std::endl;
    auto distances = roomGraph.dijkstra(startRoom);
    
    for (const auto& pair : distances) {
        if (pair.second < std::numeric_limits<int>::max()) {
            std::cout << "  -> Room " << pair.first << " distance: " << pair.second << std::endl;
        }
    }
}

void Dungeon::render(sf::RenderWindow& window, float tileSize, int currentFloor) const {
    // Debug: Verify grid size at render time
    static bool debugPrinted = false;
    if (!debugPrinted) {
        std::cout << "[Dungeon] Rendering grid: " << GRID_WIDTH << "x" << GRID_HEIGHT 
                  << " (pixels: " << (GRID_WIDTH * tileSize) << "x" << (GRID_HEIGHT * tileSize) << ")" << std::endl;
        std::cout << "[Dungeon] Current floor: " << currentFloor << std::endl;
        debugPrinted = true;
    }
    
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            sf::Texture* texture = nullptr;
            sf::Color fallbackColor;
            
            // ═══════════════════════════════════════════════════════════════════════
            // DEBTS IN THE DEPTHS TILE RENDERING - Individual PNG/GIF assets
            // ═══════════════════════════════════════════════════════════════════════
            
            switch (grid[y][x]) {
                case TileType::Empty:
                    fallbackColor = sf::Color(15, 15, 20);  // Dark void
                    break;
                case TileType::Wall:
                    texture = AssetManager::getInstance().getTexture("wall");
                    fallbackColor = sf::Color(60, 60, 70);  // Stone gray
                    break;
                case TileType::Floor: {
                    // Randomize floor tile variant based on position (deterministic)
                    int variant = (x * 7 + y * 13) % 5;  // 0-4 for 5 variants
                    
                    // Choose floor type based on current floor level
                    std::string floorKey;
                    if (currentFloor <= 3) {
                        // Floors 1-3: Mostly Brick (cave-like)
                        floorKey = (variant == 0) ? "floor" : "floor";  // All brick
                    } else if (currentFloor <= 6) {
                        // Floors 4-6: Mix of Brick and Rock (rocky cavern)
                        floorKey = (variant < 2) ? "floor" : "floor_variant_2";  // Brick or Rock
                    } else {
                        // Floors 7-10: Brimstone (hellish deep dungeon)
                        floorKey = (variant < 3) ? "floor_variant_1" : "floor_variant_2";  // Brimstone or Rock
                    }
                    
                    texture = AssetManager::getInstance().getTexture(floorKey);
                    fallbackColor = sf::Color(80, 70, 60);  // Brown floor
                    break;
                }
                case TileType::Start:
                    texture = AssetManager::getInstance().getTexture("stairs_down");
                    fallbackColor = sf::Color(50, 100, 200);  // Bright blue
                    break;
                case TileType::Exit:
                    texture = AssetManager::getInstance().getTexture("stairs_up");
                    fallbackColor = sf::Color(255, 200, 50);  // Golden exit
                    break;
                case TileType::Door:
                    texture = AssetManager::getInstance().getTexture("door_closed");
                    fallbackColor = sf::Color(150, 120, 60);  // Gold trim
                    break;
                case TileType::Treasure:
                    texture = AssetManager::getInstance().getTexture("chest");
                    fallbackColor = sf::Color(255, 215, 0);  // Gold
                    break;
                case TileType::Enemy:
                    // Enemy tiles use randomized floor
                    int variant = (x * 7 + y * 13) % 5;
                    std::string floorKey = (variant == 0) ? "floor" : 
                                          ("floor_variant_" + std::to_string(variant));
                    texture = AssetManager::getInstance().getTexture(floorKey);
                    fallbackColor = sf::Color(120, 40, 40);  // Dark red
                    break;
            }
            
            // Draw texture if available
            bool drawn = false;

            if (texture) {
                sf::Sprite tileSprite(*texture);
                tileSprite.setPosition(sf::Vector2f(x * tileSize, y * tileSize));
                
                // Scale to fit tile size (DebtsInTheDepths sprites may vary in size)
                float scaleX = tileSize / texture->getSize().x;
                float scaleY = tileSize / texture->getSize().y;
                tileSprite.setScale(sf::Vector2f(scaleX, scaleY));
                
                window.draw(tileSprite);
                drawn = true;
                
                // ✨ Add sparkle effect on stairs for visual enhancement
                if (grid[y][x] == TileType::Start || grid[y][x] == TileType::Exit) {
                    sf::Texture* sparkleTexture = AssetManager::getInstance().getTexture("effect_sparkle");
                    if (sparkleTexture) {
                        sf::Sprite sparkle(*sparkleTexture);
                        sparkle.setPosition(sf::Vector2f(x * tileSize, y * tileSize));
                        
                        float sparkleScale = tileSize / sparkleTexture->getSize().x;
                        sparkle.setScale(sf::Vector2f(sparkleScale, sparkleScale));
                        
                        // Make sparkles semi-transparent for overlay effect
                        sparkle.setColor(sf::Color(255, 255, 255, 180));
                        
                        window.draw(sparkle);
                    }
                }
            }

            int tileIndexToUse = -1;

            // ═══════════════════════════════════════════════════════════════════════
            // TILE MAPPING - No longer using Kenney spritesheet, using individual assets
            // ═══════════════════════════════════════════════════════════════════════

            if (!drawn) {
                // Fallback to colored rectangle
                sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));
                tile.setPosition(sf::Vector2f(x * tileSize, y * tileSize));
                tile.setFillColor(fallbackColor);
                
                if (grid[y][x] != TileType::Empty) {
                    tile.setOutlineThickness(0.5f);
                    tile.setOutlineColor(sf::Color(30, 30, 35));
                }
                window.draw(tile);
            }
        }
    }
}

// CHANGE: 2025-11-11 - Door management implementation
DoorData* Dungeon::getDoorAt(int x, int y) {
    for (auto& door : doors) {
        if (door.x == x && door.y == y) {
            return &door;
        }
    }
    return nullptr;
}

void Dungeon::openDoor(int x, int y) {
    DoorData* door = getDoorAt(x, y);
    if (door && !door->isOpen) {
        door->isOpen = true;
        grid[y][x] = TileType::Floor;  // Make passable
        std::cout << "[DEBUG] Door opened at (" << x << ", " << y << ")" << std::endl;
    }
}

void Dungeon::closeDoor(int x, int y) {
    DoorData* door = getDoorAt(x, y);
    if (door && door->isOpen) {
        door->isOpen = false;
        grid[y][x] = TileType::Door;  // Make impassable
        std::cout << "[DEBUG] Door closed at (" << x << ", " << y << ")" << std::endl;
    }
}

bool Dungeon::canPassDoor(int x, int y) const {
    for (const auto& door : doors) {
        if (door.x == x && door.y == y) {
            return door.isOpen || !door.requiresKey;
        }
    }
    return true;  // No door here, can pass
}

void Dungeon::checkRoomClear(int roomId, int enemyCount) {
    if (roomId < 0 || roomId >= static_cast<int>(rooms.size())) return;
    
    if (enemyCount == 0 && !rooms[roomId].cleared) {
        rooms[roomId].cleared = true;
        std::cout << "[Dungeon] Room " << roomId << " cleared! Opening doors..." << std::endl;
        
        // Open all doors connected to this room that have openOnClear
        for (auto& door : doors) {
            if ((door.roomA == roomId || door.roomB == roomId) && door.openOnClear) {
                openDoor(door.x, door.y);
            }
        }
    }
}
