# Dungeon Explorer - Comprehensive DSA Project Report

## 1. Executive Summary

**Dungeon Explorer** is a roguelike dungeon crawler demonstrating **15 Data Structures and Algorithms** in a complete game implementation using C++ and SFML 3.0.

| Metric | Value |
|--------|-------|
| **DSA Implementations** | 15 |
| **Source Files** | 23 |
| **Lines of Code** | ~15,000 |
| **Game Features** | 10 floors, 30+ items, 20+ skills, 50+ achievements |

---

## 2. Data Structures - Detailed Analysis

### 2.1 Stack (LIFO - Last In First Out)

**File**: `include/DataStructures/Stack.h`

**Concept**: A linear data structure following Last-In-First-Out principle. Elements are added and removed from the same end (top).

**Implementation**:
```cpp
template<typename T>
class Stack {
    std::vector<T> data;
public:
    void push(const T& item);  // O(1) - Add to top
    T pop();                    // O(1) - Remove from top
    T peek() const;             // O(1) - View top
    bool isEmpty() const;       // O(1)
    size_t size() const;        // O(1)
};
```

**Game Usage**: Player movement history for backtracking
```cpp
// In Player.cpp
Stack<Position> moveHistory;

void Player::move(int dx, int dy) {
    moveHistory.push(position);  // Save current position
    position.x += dx;
    position.y += dy;
}

void Player::backtrack() {
    if (!moveHistory.isEmpty()) {
        position = moveHistory.pop();  // Return to previous
    }
}
```

**Why Stack?**: Perfect for undo operations - last move made is first to be undone.

---

### 2.2 Queue (FIFO - First In First Out)

**File**: `include/DataStructures/Queue.h`

**Concept**: A linear data structure following First-In-First-Out principle. Elements are added at rear and removed from front.

**Implementation**:
```cpp
template<typename T>
class Queue {
    std::list<T> data;
public:
    void enqueue(const T& item);  // O(1) - Add to back
    T dequeue();                   // O(1) - Remove from front
    T front() const;               // O(1) - View front
    bool isEmpty() const;          // O(1)
};
```

**Game Usage**: Enemy turn order in combat
```cpp
// In EnemyManager.cpp
Queue<int> turnQueue;

void EnemyManager::processTurns() {
    // All enemies get a turn in order they were added
    while (!turnQueue.isEmpty()) {
        int enemyId = turnQueue.dequeue();
        enemies[enemyId].takeTurn();
    }
}

void EnemyManager::queueEnemyTurns() {
    for (auto& enemy : enemies) {
        if (enemy.isAlive()) {
            turnQueue.enqueue(enemy.id);
        }
    }
}
```

**Why Queue?**: Ensures fair turn order - first enemy to enter combat attacks first.

---

### 2.3 Linked List (Dynamic Linear Structure)

**File**: `include/DataStructures/LinkedList.h`

**Concept**: A linear data structure where elements (nodes) are connected via pointers. Each node contains data and a pointer to next node.

**Implementation**:
```cpp
template<typename T>
class LinkedList {
    struct Node {
        T data;
        Node* next;
    };
    Node* head;
    size_t count;
public:
    void append(const T& item);     // O(1) with tail pointer
    void prepend(const T& item);    // O(1)
    bool remove(const T& item);     // O(n) search + O(1) remove
    T get(size_t index) const;      // O(n)
    bool contains(const T& item);   // O(n)
};
```

**Game Usage**: Player inventory storage
```cpp
// In Player.h
LinkedList<ItemNew> inventory;

void Player::addItem(const ItemNew& item) {
    inventory.append(item);  // O(1) add
    std::cout << "Added " << item.name << " to inventory" << std::endl;
}

void Player::useItem(const std::string& itemId) {
    // Find and use item
    for (size_t i = 0; i < inventory.size(); i++) {
        if (inventory.get(i).id == itemId) {
            ItemNew item = inventory.get(i);
            item.use(*this);
            inventory.removeAt(i);
            break;
        }
    }
}
```

**Why Linked List?**: Dynamic size, efficient insertion/deletion without shifting elements.

---

### 2.4 Hash Table (Key-Value Mapping)

**File**: `include/DataStructures/HashTable.h`

**Concept**: A data structure that maps keys to values using a hash function. Provides O(1) average-case lookup.

**Implementation**:
```cpp
template<typename K, typename V>
class HashTable {
    struct Entry { K key; V value; bool occupied; };
    std::vector<Entry> buckets;
    size_t hashFunction(const K& key) const;
public:
    void insert(const K& key, const V& value);  // O(1) average
    V* get(const K& key);                        // O(1) average
    bool contains(const K& key) const;           // O(1) average
    bool remove(const K& key);                   // O(1) average
};
```

**Game Usage**: Item database for instant lookup
```cpp
// In ItemManager.cpp
HashTable<std::string, ItemNew> itemDatabase;

void ItemManager::loadItems() {
    // Load 30+ items from JSON
    itemDatabase.insert("potion", ItemNew("potion", "Health Potion", ...));
    itemDatabase.insert("sword_iron", ItemNew("sword_iron", "Iron Sword", ...));
    // ... more items
}

ItemNew* ItemManager::getItem(const std::string& id) {
    return itemDatabase.get(id);  // O(1) lookup!
}
```

**Why Hash Table?**: O(1) lookup vs O(n) linear search - critical for frequent item lookups.

---

### 2.5 Heap (Priority Queue Structure)

**File**: `include/DataStructures/Heap.h`

**Concept**: A complete binary tree where parent nodes have higher (max-heap) or lower (min-heap) priority than children.

**Implementation**:
```cpp
template<typename T>
class Heap {
    std::vector<T> data;
    void heapifyUp(int index);
    void heapifyDown(int index);
public:
    void insert(const T& item);  // O(log n)
    T extractMax();               // O(log n)
    T peekMax() const;            // O(1)
    bool isEmpty() const;         // O(1)
};
```

**Game Usage**: Premium loot tracking (rare items float to top)
```cpp
// In ItemManager.cpp
Heap<Loot> premiumLoot;  // Max-heap by rarity

void ItemManager::trackLoot(const Loot& loot) {
    if (loot.rarity >= RARE) {
        premiumLoot.insert(loot);  // Rare items tracked
    }
}

Loot ItemManager::getMostValuableLoot() {
    return premiumLoot.peekMax();  // O(1) - highest rarity
}
```

**Why Heap?**: Always know the most valuable item in O(1), maintain sorted order in O(log n).

---

### 2.6 Binary Tree (Hierarchical Structure)

**File**: `include/DataStructures/Tree.h`

**Concept**: A hierarchical structure where each node has at most two children (left and right).

**Implementation**:
```cpp
template<typename T>
class Tree {
    struct Node {
        T data;
        Node* left;
        Node* right;
        Node* parent;
    };
    Node* root;
public:
    void setRoot(const T& data);
    Node* addLeft(Node* parent, const T& data);
    Node* addRight(Node* parent, const T& data);
    std::vector<T> inorderTraversal();
    std::vector<T> preorderTraversal();
    int getDepth(Node* node);
};
```

**Game Usage**: Skill tree with unlock dependencies
```cpp
// In SkillTree.cpp
Tree<Skill> skillTree;

void SkillTree::initialize() {
    // Root skill - always available
    skillTree.setRoot(Skill("slash", "Basic Attack"));
    
    // Branch skills - require parent to unlock
    auto* slash = skillTree.getRoot();
    auto* powerAttack = skillTree.addLeft(slash, Skill("power_attack", "Power Attack"));
    auto* dodge = skillTree.addRight(slash, Skill("dodge", "Dodge"));
    
    // Deeper skills
    skillTree.addLeft(powerAttack, Skill("cleave", "Cleave"));
    skillTree.addRight(powerAttack, Skill("rage", "Rage"));
}

bool SkillTree::canUnlock(const std::string& skillId) {
    Node* node = findNode(skillId);
    return node->parent->data.unlocked;  // Parent must be unlocked
}
```

**Why Binary Tree?**: Natural representation of skill dependencies - unlock parent before children.

---

### 2.7 Graph (Network Structure)

**File**: `include/DataStructures/Graph.h`

**Concept**: A collection of vertices connected by edges. Can be directed/undirected, weighted/unweighted.

**Implementation**:
```cpp
template<typename T>
class Graph {
    std::unordered_map<T, std::vector<std::pair<T, int>>> adjacencyList;
public:
    void addVertex(const T& vertex);
    void addEdge(const T& from, const T& to, int weight = 1);
    std::vector<T> bfs(const T& start);           // O(V + E)
    std::vector<T> dfs(const T& start);           // O(V + E)
    std::unordered_map<T, int> dijkstra(const T& start);  // O(E log V)
    std::vector<T> findPath(const T& from, const T& to);
};
```

**Game Usage**: Dungeon room connectivity
```cpp
// In Dungeon.cpp
Graph<int> roomGraph;

void Dungeon::generate() {
    // Create rooms
    for (int i = 0; i < numRooms; i++) {
        roomGraph.addVertex(i);
    }
    
    // Connect adjacent rooms
    for (auto& room : rooms) {
        for (auto& neighbor : findNeighbors(room)) {
            roomGraph.addEdge(room.id, neighbor.id, distance(room, neighbor));
        }
    }
}

void Dungeon::placeStairs() {
    // Use Dijkstra to find furthest room from entrance
    auto distances = roomGraph.dijkstra(startRoomId);
    int furthestRoom = 0;
    int maxDist = 0;
    for (auto& [roomId, dist] : distances) {
        if (dist > maxDist) {
            maxDist = dist;
            furthestRoom = roomId;
        }
    }
    stairsRoomId = furthestRoom;
}
```

**Why Graph?**: Perfect for representing room connections with BFS/DFS/Dijkstra for navigation.

---

## 3. Algorithms - Detailed Analysis

### 3.1 A* Pathfinding Algorithm

**File**: `include/DataStructures/AStar.h`

**Concept**: A* is an informed search algorithm that finds the shortest path by combining:
- **g(n)**: Actual cost from start to current node
- **h(n)**: Heuristic estimate from current to goal
- **f(n) = g(n) + h(n)**: Total estimated cost

**Algorithm**:
```
1. Initialize open set (priority queue) with start node
2. Initialize closed set (visited nodes)
3. While open set not empty:
   a. Get node with lowest f(n) from open set
   b. If node is goal, reconstruct path
   c. For each neighbor:
      - Calculate tentative g score
      - If better than existing, update path
      - Add to open set if not visited
4. Return empty path if goal unreachable
```

**Implementation**:
```cpp
std::vector<PathNode> AStarPathfinder::findPath(PathNode start, PathNode goal) {
    PriorityQueue<PathNode> openSet;
    std::unordered_set<PathNode> closedSet;
    std::unordered_map<PathNode, PathNode> cameFrom;
    std::unordered_map<PathNode, float> gScore, fScore;
    
    gScore[start] = 0;
    fScore[start] = heuristic(start, goal);
    openSet.push(fScore[start], start);
    
    while (!openSet.empty()) {
        PathNode current = openSet.pop();
        
        if (current == goal) {
            return reconstructPath(cameFrom, current);
        }
        
        closedSet.insert(current);
        
        for (PathNode neighbor : getNeighbors(current)) {
            if (closedSet.count(neighbor)) continue;
            
            float tentativeG = gScore[current] + moveCost(current, neighbor);
            
            if (tentativeG < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentativeG;
                fScore[neighbor] = tentativeG + heuristic(neighbor, goal);
                openSet.push(fScore[neighbor], neighbor);
            }
        }
    }
    return {};  // No path found
}
```

**Game Usage**: Enemy smart navigation around obstacles
```cpp
// In EnemyManager.cpp
void Enemy::moveTowardsPlayer(const Player& player) {
    PathNode start(gridX, gridY);
    PathNode goal(player.gridX, player.gridY);
    
    std::vector<PathNode> path = pathfinder.findPath(start, goal);
    
    if (path.size() > 1) {
        // Move to next step in path
        gridX = path[1].x;
        gridY = path[1].y;
    }
}
```

**Complexity**: O(E log V) where E = edges, V = vertices

---

### 3.2 Dijkstra's Shortest Path Algorithm

**File**: `include/DataStructures/Graph.h`

**Concept**: Finds shortest paths from a source vertex to all other vertices in a weighted graph.

**Algorithm**:
```
1. Set distance to source = 0, all others = infinity
2. Add source to priority queue
3. While queue not empty:
   a. Extract vertex with minimum distance
   b. For each neighbor:
      - Calculate new distance through current vertex
      - If shorter, update distance and add to queue
4. Return distance map
```

**Implementation**:
```cpp
std::unordered_map<T, int> Graph<T>::dijkstra(const T& start) {
    std::unordered_map<T, int> distances;
    
    // Initialize all distances to infinity
    for (auto& [vertex, _] : adjacencyList) {
        distances[vertex] = INT_MAX;
    }
    distances[start] = 0;
    
    // Min-heap priority queue
    std::priority_queue<std::pair<int, T>, 
                        std::vector<std::pair<int, T>>,
                        std::greater<std::pair<int, T>>> pq;
    pq.push({0, start});
    
    while (!pq.empty()) {
        auto [dist, current] = pq.top();
        pq.pop();
        
        if (dist > distances[current]) continue;
        
        for (auto& [neighbor, weight] : adjacencyList[current]) {
            int newDist = dist + weight;
            if (newDist < distances[neighbor]) {
                distances[neighbor] = newDist;
                pq.push({newDist, neighbor});
            }
        }
    }
    return distances;
}
```

**Game Usage**: Finding furthest room for stairs placement
```cpp
// In Dungeon.cpp
void Dungeon::placeStairs() {
    auto distances = roomGraph.dijkstra(entranceRoomId);
    
    // Find room with maximum distance - that's where stairs go
    int stairsRoom = std::max_element(distances.begin(), distances.end(),
        [](auto& a, auto& b) { return a.second < b.second; })->first;
    
    placeStairsInRoom(stairsRoom);
}
```

**Complexity**: O(E log V) with binary heap

---

### 3.3 Breadth-First Search (BFS)

**File**: `include/DataStructures/Graph.h`

**Concept**: Explores graph level by level, visiting all neighbors before moving deeper.

**Algorithm**:
```
1. Start with source in queue
2. While queue not empty:
   a. Dequeue front vertex
   b. Visit vertex
   c. Enqueue all unvisited neighbors
```

**Implementation**:
```cpp
std::vector<T> Graph<T>::bfs(const T& start) {
    std::vector<T> result;
    std::unordered_set<T> visited;
    std::queue<T> q;
    
    q.push(start);
    visited.insert(start);
    
    while (!q.empty()) {
        T current = q.front();
        q.pop();
        result.push_back(current);
        
        for (auto& neighbor : getNeighbors(current)) {
            if (!visited.count(neighbor)) {
                visited.insert(neighbor);
                q.push(neighbor);
            }
        }
    }
    return result;
}
```

**Game Usage**: Finding shortest path between rooms, checking connectivity
```cpp
// In Dungeon.cpp
bool Dungeon::isConnected() {
    auto reachable = roomGraph.bfs(startRoomId);
    return reachable.size() == rooms.size();  // All rooms reachable?
}
```

**Complexity**: O(V + E)

---

### 3.4 Depth-First Search (DFS)

**File**: `include/DataStructures/Graph.h`

**Concept**: Explores graph by going as deep as possible before backtracking.

**Algorithm**:
```
1. Start with source in stack
2. While stack not empty:
   a. Pop top vertex
   b. If not visited, visit it
   c. Push all unvisited neighbors
```

**Implementation**:
```cpp
std::vector<T> Graph<T>::dfs(const T& start) {
    std::vector<T> result;
    std::unordered_set<T> visited;
    std::stack<T> s;
    
    s.push(start);
    
    while (!s.empty()) {
        T current = s.top();
        s.pop();
        
        if (!visited.count(current)) {
            visited.insert(current);
            result.push_back(current);
            
            for (auto& neighbor : getNeighbors(current)) {
                if (!visited.count(neighbor)) {
                    s.push(neighbor);
                }
            }
        }
    }
    return result;
}
```

**Game Usage**: Exploring dungeon paths, maze generation
```cpp
// In Dungeon.cpp - Room exploration order
std::vector<int> Dungeon::getExplorationOrder() {
    return roomGraph.dfs(startRoomId);
}
```

**Complexity**: O(V + E)

---

### 3.5 Spatial Hashing

**File**: `include/DataStructures/SpatialHash.h`

**Concept**: Divides game world into grid cells. Objects are stored in cells based on position, enabling O(1) lookup for nearby entities.

**Implementation**:
```cpp
template<typename T>
class SpatialHash {
    std::unordered_map<int, std::vector<T>> cells;
    int cellSize;
    
    int getCellKey(int x, int y) {
        int cellX = x / cellSize;
        int cellY = y / cellSize;
        return cellX + cellY * 10000;  // Unique key
    }
    
public:
    void insert(int x, int y, const T& item) {
        cells[getCellKey(x, y)].push_back(item);
    }
    
    std::vector<T> query(int x, int y, int radius) {
        std::vector<T> result;
        int cellRadius = radius / cellSize + 1;
        
        for (int dx = -cellRadius; dx <= cellRadius; dx++) {
            for (int dy = -cellRadius; dy <= cellRadius; dy++) {
                int key = getCellKey(x + dx * cellSize, y + dy * cellSize);
                if (cells.count(key)) {
                    for (auto& item : cells[key]) {
                        result.push_back(item);
                    }
                }
            }
        }
        return result;
    }
};
```

**Game Usage**: Fast combat range queries
```cpp
// In EnemyManager.cpp
SpatialHash<int> enemyGrid;

void EnemyManager::buildSpatialIndex() {
    enemyGrid.clear();
    for (auto& enemy : enemies) {
        if (enemy.isAlive()) {
            enemyGrid.insert(enemy.x, enemy.y, enemy.id);
        }
    }
}

std::vector<int> EnemyManager::findNearbyEnemies(int x, int y, int range) {
    return enemyGrid.query(x, y, range);  // O(k) instead of O(n)!
}
```

**Performance**: O(k) where k = enemies in nearby cells, vs O(n) linear scan

---

## 4. Complexity Analysis Summary

| Algorithm/DS | Insert | Search | Delete | Notes |
|-------------|--------|--------|--------|-------|
| Stack | O(1) | O(n) | O(1)* | *From top only |
| Queue | O(1) | O(n) | O(1)* | *From front only |
| Linked List | O(1) | O(n) | O(n) | Position-based O(1) |
| Hash Table | O(1)† | O(1)† | O(1)† | †Average case |
| Heap | O(log n) | O(n) | O(log n) | Max/min access O(1) |
| Binary Tree | O(log n)‡ | O(log n)‡ | O(log n)‡ | ‡Balanced tree |
| Graph | O(1) | O(V+E) | O(E) | BFS/DFS search |
| A* | - | O(E log V) | - | Pathfinding |
| Dijkstra | - | O(E log V) | - | Shortest paths |
| Spatial Hash | O(1) | O(k) | O(1) | k = nearby items |

---

## 5. System Integration Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              GAME LOOP                                       │
│  ┌─────────────┐   ┌─────────────┐   ┌─────────────┐   ┌─────────────┐     │
│  │   INPUT     │──▶│   UPDATE    │──▶│   RENDER    │──▶│   AUDIO     │     │
│  └─────────────┘   └─────────────┘   └─────────────┘   └─────────────┘     │
└─────────────────────────────────────────────────────────────────────────────┘
         │                   │                   │
         ▼                   ▼                   ▼
┌─────────────┐     ┌─────────────────────────────────────────────────────────┐
│   PLAYER    │     │                    GAME SYSTEMS                          │
│ ┌─────────┐ │     │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐     │
│ │  Stack  │ │     │  │   Dungeon   │  │   Enemies   │  │    Items    │     │
│ │ (history)│ │     │  │ ┌─────────┐│  │ ┌─────────┐ │  │ ┌─────────┐ │     │
│ └─────────┘ │     │  │ │  Graph  ││  │ │  Queue  │ │  │ │HashTable│ │     │
│ ┌─────────┐ │     │  │ │Dijkstra ││  │ │   A*    │ │  │ │  Heap   │ │     │
│ │LinkedList│ │     │  │ │BFS/DFS ││  │ │SpatialH│ │  │ └─────────┘ │     │
│ │(inventory)│ │     │  │ └─────────┘│  │ └─────────┘ │  └─────────────┘     │
│ └─────────┘ │     │  └─────────────┘  └─────────────┘                       │
└─────────────┘     │  ┌─────────────┐  ┌─────────────┐                       │
                    │  │  SkillTree  │  │  PathCache  │                       │
                    │  │ ┌─────────┐ │  │ ┌─────────┐ │                       │
                    │  │ │  Tree   │ │  │ │LRUCache │ │                       │
                    │  │ └─────────┘ │  │ └─────────┘ │                       │
                    │  └─────────────┘  └─────────────┘                       │
                    └─────────────────────────────────────────────────────────┘
```

---

## 6. Conclusion

This project demonstrates practical applications of 15 Data Structures and Algorithms:

### Core DSA (7):
1. **Stack** - Movement undo system
2. **Queue** - Turn-based combat
3. **Linked List** - Dynamic inventory
4. **Hash Table** - O(1) item lookup
5. **Heap** - Loot prioritization
6. **Binary Tree** - Skill dependencies
7. **Graph** - Room connectivity

### Pathfinding (4):
8. **A* Algorithm** - Smart enemy navigation
9. **Dijkstra** - Shortest paths
10. **BFS** - Level-order exploration
11. **DFS** - Deep exploration

### Optimization (4):
12. **Spatial Hash** - O(k) combat queries
13. **Priority Queue** - A* open set
14. **LRU Cache** - Memory management
15. **Object Pool** - Allocation reduction

---

**Report Version**: 3.0  
**Last Updated**: December 2025  
**Build Status**: ✅ Passing
