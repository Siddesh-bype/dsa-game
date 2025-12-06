// CHANGE: 2025-11-10 — TASK D: Stairs placement for floor transitions
// - Added stairs position tracking
// - Place stairs in furthest room from entrance using Dijkstra
// - Added debug logging for stairs placement
// CHANGE: 2025-12-04 - Refactored with GameUtils for code deduplication

#include "Dungeon.h"
#include "AssetManager.h"
#include "GameUtils.h"
#include <iostream>
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
            if (dist > maxDistance && dist < INFINITE_DISTANCE) {
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
                
                // Place single stair tile
                stairsX = centerX;
                stairsY = centerY;
                
                if (grid[centerY][centerX] != TileType::Door) {
                    grid[centerY][centerX] = TileType::Exit;
                }
                
                // CHANGE: 2025-11-14 - Reduce debug spam from generation
                // std::cout << "[DEBUG] Stairs placed..." << std::endl;
                break;
            }
        }
    }
    
    // OPTIMIZATION: Use Graph's isConnected() to validate dungeon
    if (rooms.size() > 1) {
        bool connected = roomGraph.isConnected();
        std::cout << "[Dungeon] Graph connectivity check: " 
                  << (connected ? "CONNECTED" : "DISCONNECTED") << std::endl;
        
        if (!connected) {
            std::cerr << "[WARNING] Dungeon graph is not fully connected!" << std::endl;
        }
    }
    
    // 🎮 Generate decorative elements
    generateDecorations();
    
    std::cout << "[Dungeon] Generation complete!" << std::endl;
}

void Dungeon::generateRooms(int numRooms) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    for (int i = 0; i < numRooms; i++) {
        int x = RandomUtils::randomInt(ROOM_MARGIN, GRID_WIDTH - ROOM_SAFE_ZONE);
        int y = RandomUtils::randomInt(ROOM_MARGIN, GRID_HEIGHT - ROOM_SAFE_ZONE);
        int w = RandomUtils::randomInt(MIN_ROOM_SIZE, MIN_ROOM_SIZE + MAX_ROOM_SIZE_RANGE);
        int h = RandomUtils::randomInt(MIN_ROOM_SIZE, MIN_ROOM_SIZE + MAX_ROOM_SIZE_RANGE);
        
        // Ensure room fits with proper bounds checking
        if (x + w >= GRID_WIDTH - 1) {
            w = GRID_WIDTH - x - ROOM_MARGIN;
        }
        if (y + h >= GRID_HEIGHT - 1) {
            h = GRID_HEIGHT - y - ROOM_MARGIN;
        }
        
        // Ensure minimum room size
        if (w < MIN_ROOM_SIZE) w = MIN_ROOM_SIZE;
        if (h < MIN_ROOM_SIZE) h = MIN_ROOM_SIZE;
        
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
            int r1 = RandomUtils::randomInt(0, static_cast<int>(rooms.size()) - 1);
            int r2 = RandomUtils::randomInt(0, static_cast<int>(rooms.size()) - 1);
            if (r1 != r2) {
                int weight = std::abs(rooms[r1].x - rooms[r2].x) + std::abs(rooms[r1].y - rooms[r2].y);
                roomGraph.addBidirectionalEdge(rooms[r1].id, rooms[r2].id, weight);
            }
        }
    }
    
    // OPTIMIZATION: Display graph statistics
    std::cout << "[Dungeon] Graph stats - Vertices: " << rooms.size() 
              << ", Edges: " << roomGraph.edgeCount() << std::endl;
}

void Dungeon::fillGrid() {
    // Fill rooms into grid - ALWAYS use Floor, stairs are single tiles
    for (const auto& room : rooms) {
        for (int y = room.y; y < room.y + room.height && y < GRID_HEIGHT; y++) {
            for (int x = room.x; x < room.x + room.width && x < GRID_WIDTH; x++) {
                grid[y][x] = TileType::Floor;  // All rooms are floor tiles
            }
        }
        
        // Place single stair tile at center of Start/Exit rooms
        int centerX = room.x + room.width / 2;
        int centerY = room.y + room.height / 2;
        if (centerX < GRID_WIDTH && centerY < GRID_HEIGHT) {
            if (room.type == TileType::Start) {
                grid[centerY][centerX] = TileType::Start;
            } else if (room.type == TileType::Exit) {
                grid[centerY][centerX] = TileType::Exit;
                stairsX = centerX;
                stairsY = centerY;
            }
        }
    }
    
    // Clear doors before carving corridors
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
            // Only place a door if we are breaking a wall at a room boundary
            bool isRoomBoundary = false;
            int currentRoom = getRoomIdAt(x, y);
            
            // Check if we are entering/leaving r1
            if ((x == r1.x && x > x2) || (x == r1.x + r1.width - 1 && x < x2)) {
                isRoomBoundary = true;
            }
            // Check if we are entering/leaving r2
            else if ((x == r2.x && x > x1) || (x == r2.x + r2.width - 1 && x < x1)) {
                isRoomBoundary = true;
            }

            if (grid[y][x] == TileType::Wall && isRoomBoundary) {
                grid[y][x] = TileType::Door;
                bool requiresKey = (std::rand() % 100) < DOOR_KEY_CHANCE_PERCENT;
                doors.push_back(DoorData(x, y, r1.id, r2.id, requiresKey, true));
            } else if (grid[y][x] == TileType::Wall || grid[y][x] == TileType::Empty) {
                grid[y][x] = TileType::Floor;
            }
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
            // Only place a door if we are breaking a wall at a room boundary
            bool isRoomBoundary = false;
            
            // Check if we are entering/leaving r1
            if ((y == r1.y && y > y2) || (y == r1.y + r1.height - 1 && y < y2)) {
                isRoomBoundary = true;
            }
            // Check if we are entering/leaving r2
            else if ((y == r2.y && y > y1) || (y == r2.y + r2.height - 1 && y < y1)) {
                isRoomBoundary = true;
            }

            if (grid[y][x] == TileType::Wall && isRoomBoundary) {
                grid[y][x] = TileType::Door;
                bool requiresKey = (std::rand() % 100) < DOOR_KEY_CHANCE_PERCENT;
                doors.push_back(DoorData(x, y, r1.id, r2.id, requiresKey, true));
            } else if (grid[y][x] == TileType::Wall || grid[y][x] == TileType::Empty) {
                grid[y][x] = TileType::Floor;
            }
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
    if (isInBounds(x, y)) {
        grid[y][x] = type;
    }
}

// ═══════════════════════════════════════════════════════════════════════
// HELPER FUNCTIONS - Reduce code duplication
// ═══════════════════════════════════════════════════════════════════════

bool Dungeon::isInBounds(int x, int y) const {
    return x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT;
}

std::vector<std::pair<int, int>> Dungeon::getNeighbors(int x, int y) const {
    return {
        {x, y - 1},      // Up
        {x, y + 1},      // Down
        {x - 1, y},      // Left
        {x + 1, y}       // Right
    };
}

std::vector<std::pair<int, int>> Dungeon::getValidNeighbors(int x, int y) const {
    std::vector<std::pair<int, int>> validMoves;
    for (const auto& neighbor : getNeighbors(x, y)) {
        if (isWalkable(neighbor.first, neighbor.second)) {
            validMoves.push_back(neighbor);
        }
    }
    return validMoves;
}

std::string Dungeon::getFloorTextureKey(int x, int y, int currentFloor) const {
    // CHANGE: 2025-12-06 - Simplified floor texture selection by floor level
    // Floors 1-3: Brick (dungeon entrance)
    // Floors 4-6: Brimstone (shadow temple/crystal mines)  
    // Floors 7-10: Rock (fortress/lava/obsidian)
    
    if (currentFloor <= 3) {
        return "floor";  // sprBrick.gif
    } else if (currentFloor <= 6) {
        return "floor_variant_1";  // sprBrimstone.gif
    } else if (currentFloor <= 8) {
        return "floor_variant_2";  // sprBrimstone.gif
    } else {
        return "floor_variant_3";  // sprRock.gif
    }
}

int Dungeon::getDeterministicVariant(int x, int y, int maxVariants) const {
    return (x * VARIANT_PRIME_X + y * VARIANT_PRIME_Y) % maxVariants;
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
        
        // Use helper function for neighbors
        for (const auto& next : getNeighbors(current.first, current.second)) {
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

// A* Pathfinding (Smart)
std::pair<int, int> Dungeon::findNextMoveToPlayerAStar(int enemyX, int enemyY, int playerX, int playerY) const {
    // If already at player position, don't move
    if (enemyX == playerX && enemyY == playerY) return {enemyX, enemyY};

    // Priority queue for A* (min-heap based on f_score)
    // Pair: <f_score, <x, y>>
    using Node = std::pair<int, std::pair<int, int>>;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    
    std::map<std::pair<int, int>, std::pair<int, int>> cameFrom;
    std::map<std::pair<int, int>, int> gScore;
    
    auto start = std::make_pair(enemyX, enemyY);
    auto goal = std::make_pair(playerX, playerY);
    
    gScore[start] = 0;
    // Heuristic: Manhattan distance
    int hScore = std::abs(enemyX - playerX) + std::abs(enemyY - playerY);
    openSet.push({hScore, start});
    
    bool found = false;
    
    while (!openSet.empty()) {
        auto current = openSet.top().second;
        openSet.pop();
        
        if (current == goal) {
            found = true;
            break;
        }
        
        // Use helper function for neighbors
        for (const auto& next : getNeighbors(current.first, current.second)) {
            // Check bounds and walkability (allow moving into player position for attack range check)
            if (!isWalkable(next.first, next.second) && next != goal) continue;
            
            int tentative_gScore = gScore[current] + 1;
            
            if (gScore.find(next) == gScore.end() || tentative_gScore < gScore[next]) {
                cameFrom[next] = current;
                gScore[next] = tentative_gScore;
                int fScore = tentative_gScore + std::abs(next.first - playerX) + std::abs(next.second - playerY);
                openSet.push({fScore, next});
            }
        }
    }
    
    if (!found) return {enemyX, enemyY}; // No path
    
    // Reconstruct path
    auto current = goal;
    auto nextMove = current;
    
    while (cameFrom[current] != start) {
        nextMove = current;
        current = cameFrom[current];
        // Safety break for infinite loops (shouldn't happen with valid A*)
        if (current == start) break;
    }
    
    // If path is direct (1 step), nextMove is goal. Otherwise it's the step after start.
    // Actually, we want the step immediately after start.
    // The loop backtracks from goal. When it stops, 'current' is the node that came from start.
    return current;
}

// Random Movement (Erratic)
std::pair<int, int> Dungeon::findNextMoveRandom(int enemyX, int enemyY) const {
    auto validMoves = getValidNeighbors(enemyX, enemyY);
    
    if (validMoves.empty()) return {enemyX, enemyY};
    
    // 20% chance to stay still
    if (std::rand() % 100 < 20) return {enemyX, enemyY};
    
    return validMoves[std::rand() % validMoves.size()];
}

// Flank Movement (Strategic)
std::pair<int, int> Dungeon::findNextMoveFlank(int enemyX, int enemyY, int playerX, int playerY) const {
    int dx = std::abs(enemyX - playerX);
    int dy = std::abs(enemyY - playerY);
    int dist = dx + dy;
    
    // If too far, chase (A*)
    if (dist > 4) return findNextMoveToPlayerAStar(enemyX, enemyY, playerX, playerY);
    
    // If too close, try to back away
    if (dist < 2) {
        // Find move that increases distance
        for (const auto& next : getNeighbors(enemyX, enemyY)) {
            if (isWalkable(next.first, next.second)) {
                int newDist = std::abs(next.first - playerX) + std::abs(next.second - playerY);
                if (newDist > dist) return next;
            }
        }
    }
    
    // If at good range, try to move sideways (maintain distance but change angle)
    auto neighbors = getNeighbors(enemyX, enemyY);
    
    // Shuffle to make it unpredictable
    if (std::rand() % 2 == 0) std::swap(neighbors[0], neighbors[1]);
    if (std::rand() % 2 == 0) std::swap(neighbors[2], neighbors[3]);
    
    for (const auto& next : neighbors) {
        if (isWalkable(next.first, next.second)) {
            int newDist = std::abs(next.first - playerX) + std::abs(next.second - playerY);
            // Accept move if distance is maintained (2-4 range)
            if (newDist >= 2 && newDist <= 4) return next;
        }
    }
    
    // Fallback: just chase
    return findNextMoveToPlayerAStar(enemyX, enemyY, playerX, playerY);
}

std::vector<int> Dungeon::getReachableRooms(int roomId) {
    return roomGraph.bfs(roomId);
}

std::vector<int> Dungeon::findShortestPath(int fromRoom, int toRoom) {
    // OPTIMIZATION: Use Graph's findPath() method for cleaner code
    auto path = roomGraph.findPath(fromRoom, toRoom);
    
    if (path.empty()) {
        std::cout << "[Dungeon] No path found from room " << fromRoom 
                  << " to room " << toRoom << std::endl;
    } else {
        std::cout << "[Dungeon] Path found with " << path.size() << " rooms" << std::endl;
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
    // CHANGE: 2025-12-06 - ROOT CAUSE FIX: Removed all spritesheet logic
    // The game uses individual PNG/GIF files from "Debts in the Depths" asset pack
    // Each texture file represents a complete tile - just scale to fit tileSize
    
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            sf::Texture* texture = nullptr;
            sf::Color fallbackColor;
            
            // Get texture key for this tile
            switch (grid[y][x]) {
                case TileType::Empty:
                    fallbackColor = sf::Color(15, 15, 20);
                    break;
                    
                case TileType::Wall:
                    texture = AssetManager::getInstance().getTexture("wall");
                    fallbackColor = sf::Color(60, 60, 70);
                    break;
                    
                case TileType::Floor: {
                    std::string floorKey = getFloorTextureKey(x, y, currentFloor);
                    texture = AssetManager::getInstance().getTexture(floorKey);
                    fallbackColor = sf::Color(80, 70, 60);
                    break;
                }
                
                case TileType::Start:
                    texture = AssetManager::getInstance().getTexture("stairs_down");
                    fallbackColor = sf::Color(50, 100, 200);
                    break;
                    
                case TileType::Exit:
                    texture = AssetManager::getInstance().getTexture("stairs_up");
                    fallbackColor = sf::Color(255, 200, 50);
                    break;
                    
                case TileType::Door: {
                    bool isOpen = false;
                    for (const auto& door : doors) {
                        if (door.x == x && door.y == y && door.isOpen) {
                            isOpen = true;
                            break;
                        }
                    }
                    texture = AssetManager::getInstance().getTexture(isOpen ? "door_open" : "door_closed");
                    fallbackColor = isOpen ? sf::Color(80, 70, 60) : sf::Color(150, 120, 60);
                    break;
                }
                
                case TileType::Treasure:
                    texture = AssetManager::getInstance().getTexture("chest");
                    fallbackColor = sf::Color(255, 215, 0);
                    break;
                    
                case TileType::Enemy: {
                    std::string floorKey = getFloorTextureKey(x, y, currentFloor);
                    texture = AssetManager::getInstance().getTexture(floorKey);
                    fallbackColor = sf::Color(120, 40, 40);
                    break;
                }
            }
            
            // Render the tile
            if (texture) {
                sf::Sprite tileSprite(*texture);
                tileSprite.setPosition(sf::Vector2f(x * tileSize, y * tileSize));
                
                // Simply scale entire texture to fit the tile size
                float scaleX = tileSize / static_cast<float>(texture->getSize().x);
                float scaleY = tileSize / static_cast<float>(texture->getSize().y);
                tileSprite.setScale(sf::Vector2f(scaleX, scaleY));
                
                window.draw(tileSprite);
                
                // Add sparkle effect on stairs
                if (grid[y][x] == TileType::Start || grid[y][x] == TileType::Exit) {
                    sf::Texture* sparkle = AssetManager::getInstance().getTexture("effect_sparkle");
                    if (sparkle) {
                        sf::Sprite sparkleSprite(*sparkle);
                        sparkleSprite.setPosition(sf::Vector2f(x * tileSize, y * tileSize));
                        float sparkleScale = tileSize / static_cast<float>(sparkle->getSize().x);
                        sparkleSprite.setScale(sf::Vector2f(sparkleScale, sparkleScale));
                        sparkleSprite.setColor(sf::Color(255, 255, 255, 180));
                        window.draw(sparkleSprite);
                    }
                }
            } else {
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

// ═══════════════════════════════════════════════════════════════════════
// DOOR MANAGEMENT - Simplified and consistent door state handling  
// CHANGE: 2025-12-06 - Refactored for cleaner logic
// ═══════════════════════════════════════════════════════════════════════

DoorData* Dungeon::getDoorAt(int x, int y) {
    for (auto& door : doors) {
        if (door.x == x && door.y == y) return &door;
    }
    return nullptr;
}

const DoorData* Dungeon::getDoorAt(int x, int y) const {
    for (const auto& door : doors) {
        if (door.x == x && door.y == y) return &door;
    }
    return nullptr;
}

void Dungeon::openDoor(int x, int y) {
    if (DoorData* door = getDoorAt(x, y)) {
        if (!door->isOpen) {
            door->isOpen = true;
            // Note: Grid stays as TileType::Door - isWalkable() checks door.isOpen
            std::cout << "[Door] Opened at (" << x << ", " << y << ")" << std::endl;
        }
    }
}

void Dungeon::closeDoor(int x, int y) {
    if (DoorData* door = getDoorAt(x, y)) {
        if (door->isOpen) {
            door->isOpen = false;
            std::cout << "[Door] Closed at (" << x << ", " << y << ")" << std::endl;
        }
    }
}

bool Dungeon::canPassDoor(int x, int y) const {
    if (const DoorData* door = getDoorAt(x, y)) {
        return door->isOpen || !door->requiresKey;
    }
    return true;  // No door at this position
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

// CHANGE: 2025-11-14 - Helper function to get room ID at a position
int Dungeon::getRoomIdAt(int x, int y) const {
    for (size_t i = 0; i < rooms.size(); i++) {
        const auto& room = rooms[i];
        if (x >= room.x && x < room.x + room.width &&
            y >= room.y && y < room.y + room.height) {
            return static_cast<int>(i);
        }
    }
    return -1;  // Not in any room (in corridor)
}

// ═══════════════════════════════════════════════════════════════════════
// DECORATIVE TILE SYSTEM - Environmental props and particles
// ═══════════════════════════════════════════════════════════════════════

void Dungeon::generateDecorations() {
    decorTiles.clear();
    
    static const char* decorTypes[] = {"torch", "skull", "barrel", "crate", "bones", "cobweb"};
    static const int numTypes = 6;
    
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[y][x] == TileType::Floor) {
                // Random chance to place decoration
                float roll = static_cast<float>(std::rand() % 100) / 100.f;
                if (roll < DECOR_CHANCE) {
                    // Check not near doors or stairs
                    bool nearImportant = false;
                    for (const auto& door : doors) {
                        if (std::abs(x - door.x) <= 1 && std::abs(y - door.y) <= 1) {
                            nearImportant = true;
                            break;
                        }
                    }
                    if (x == stairsX && y == stairsY) nearImportant = true;
                    
                    if (!nearImportant) {
                        int typeIdx = std::rand() % numTypes;
                        decorTiles.emplace_back(x, y, decorTypes[typeIdx]);
                    }
                }
            }
            // Place torches on walls near rooms
            else if (grid[y][x] == TileType::Wall && y > 0 && grid[y-1][x] == TileType::Floor) {
                if ((std::rand() % 100) < 10) {  // 10% chance
                    decorTiles.emplace_back(x, y, "wall_torch");
                }
            }
        }
    }
    
    std::cout << "[Dungeon] Generated " << decorTiles.size() << " decorations" << std::endl;
}

void Dungeon::renderDecorations(sf::RenderWindow& window, float tileSize) const {
    for (const auto& decor : decorTiles) {
        // Map decoration types to existing loaded assets
        std::string textureKey;
        if (decor.type == "torch" || decor.type == "wall_torch") {
            textureKey = "props_inferno";  // Fire props
        } else if (decor.type == "skull" || decor.type == "bones") {
            textureKey = "props_corpses";  // Corpse/bone props
        } else if (decor.type == "cobweb") {
            textureKey = "props_catacombs";  // Catacomb decorations
        } else {
            textureKey = "props_swamp";  // Barrel, crate -> swamp props
        }
        
        sf::Texture* texture = AssetManager::getInstance().getTexture(textureKey);
        
        if (texture) {
            sf::Sprite sprite(*texture);
            sprite.setPosition(sf::Vector2f(decor.x * tileSize, decor.y * tileSize));
            
            float scale = tileSize / static_cast<float>(texture->getSize().x);
            sprite.setScale(sf::Vector2f(scale * 0.5f, scale * 0.5f));  // Smaller props
            sprite.setColor(sf::Color(255, 255, 255, 150));  // Semi-transparent
            
            window.draw(sprite);
        } else {
            // Fallback: simple colored circle (no log spam)
            sf::CircleShape shape(tileSize * 0.12f);
            shape.setPosition(sf::Vector2f(decor.x * tileSize + tileSize * 0.38f, 
                                           decor.y * tileSize + tileSize * 0.38f));
            
            if (decor.type == "torch" || decor.type == "wall_torch") {
                shape.setFillColor(sf::Color(255, 150, 50, 180));  // Orange for torch
            } else if (decor.type == "skull" || decor.type == "bones") {
                shape.setFillColor(sf::Color(200, 200, 180, 120));  // Bone white
            } else {
                shape.setFillColor(sf::Color(139, 90, 43, 120));  // Brown for crates/barrels
            }
            window.draw(shape);
        }
    }
    
    // Render ambient particles
    for (const auto& particle : ambientParticles) {
        sf::CircleShape shape(2.f);
        shape.setPosition(sf::Vector2f(particle.x, particle.y));
        
        uint8_t alpha = static_cast<uint8_t>(particle.alpha * (particle.lifetime / 3.f));
        if (particle.type == "dust") {
            shape.setFillColor(sf::Color(150, 140, 120, alpha));
        } else if (particle.type == "spark") {
            shape.setFillColor(sf::Color(255, 200, 100, alpha));
        } else {
            shape.setFillColor(sf::Color(100, 100, 120, alpha / 2));  // Fog
        }
        window.draw(shape);
    }
}

void Dungeon::updateAmbientParticles(float deltaTime) const {
    // Update existing particles
    for (auto it = ambientParticles.begin(); it != ambientParticles.end();) {
        it->lifetime -= deltaTime;
        it->x += it->vx * deltaTime;
        it->y += it->vy * deltaTime;
        it->alpha = 255.f * (it->lifetime / 3.f);
        
        if (it->lifetime <= 0.f) {
            it = ambientParticles.erase(it);
        } else {
            ++it;
        }
    }
    
    // Spawn new particles from torches
    particleSpawnTimer += deltaTime;
    if (particleSpawnTimer >= PARTICLE_SPAWN_INTERVAL && 
        static_cast<int>(ambientParticles.size()) < MAX_AMBIENT_PARTICLES) {
        particleSpawnTimer = 0.f;
        
        // Find a torch to spawn from
        for (const auto& decor : decorTiles) {
            if ((decor.type == "torch" || decor.type == "wall_torch") && 
                static_cast<int>(ambientParticles.size()) < MAX_AMBIENT_PARTICLES) {
                if ((std::rand() % 100) < 30) {  // 30% chance per torch
                    float px = decor.x * 32.f + 16.f + (std::rand() % 10 - 5);
                    float py = decor.y * 32.f + 5.f;
                    ambientParticles.emplace_back(px, py, "spark");
                    ambientParticles.back().vy = -20.f - (std::rand() % 10);
                    ambientParticles.back().vx = (std::rand() % 10 - 5);
                    break;  // One per update
                }
            }
        }
    }
}
