# DSA Feature Report - Dungeon Explorer

## Project Overview
**Name**: Dungeon Explorer  
**Language**: C++17  
**Graphics**: SFML 2.6  
**UI**: TGUI 1.0  
**Build System**: CMake  

---

## Data Structures Implementation

### 1. Stack (Player Movement History)
**File**: `include/DataStructures/Stack.h`

**Purpose**: Store player's movement history for backtracking

**Implementation**:
- Template-based stack using `std::vector`
- LIFO (Last In, First Out) operations
- Operations: `push()`, `pop()`, `top()`, `isEmpty()`

**Usage in Game**:
- Location: `Player.cpp`
- Each movement pushes current position onto stack
- Press 'B' key to backtrack (pop from stack)
- Console logs: `[DSA-Stack] Pushed element. Stack size: X`

**Code Example**:
```cpp
void Player::moveTo(const Position& pos) {
    pathHistory.push(position);  // Save current position
    position = pos;
}

void Player::backtrack() {
    if (pathHistory.size() > 1) {
        pathHistory.pop();  // Remove current
        position = pathHistory.top();  // Go to previous
    }
}
```

---

### 2. Queue (Enemy Turn System)
**File**: `include/DataStructures/Queue.h`

**Purpose**: Manage turn-based combat order

**Implementation**:
- Template-based queue using `std::deque`
- FIFO (First In, First Out) operations
- Operations: `enqueue()`, `dequeue()`, `front()`, `isEmpty()`

**Usage in Game**:
- Location: `Enemy.cpp`
- Enemies enqueued in initiative order
- Each turn dequeues next attacker
- Console logs: `[DSA-Queue] Enqueued element. Queue size: X`

**Code Example**:
```cpp
void EnemyManager::processNextTurn() {
    if (!turnQueue.isEmpty()) {
        EnemyData* enemy = turnQueue.front();
        turnQueue.dequeue();
        // Process enemy action
        turnQueue.enqueue(enemy);  // Re-queue for next round
    }
}
```

---

### 3. Linked List (Inventory System)
**File**: `include/DataStructures/LinkedList.h`

**Purpose**: Dynamic item storage without fixed size

**Implementation**:
- Singly linked list with smart pointers
- Node-based structure
- Operations: `insert()`, `append()`, `remove()`, `traverse()`

**Usage in Game**:
- Location: `Player.cpp`
- Items added to inventory dynamically
- No size limit (unlike arrays)
- Console logs: `[DSA-LinkedList] Appended element. List size: X`

**Code Example**:
```cpp
void Player::addItem(const Item& item) {
    inventory.append(item);  // Add to end of list
}

// Display all items
inventory.traverse([](const Item& item) {
    std::cout << "  - " << item.name << std::endl;
});
```

---

### 4. Heap (Priority Queue for Loot)
**File**: `include/DataStructures/Heap.h`

**Purpose**: Extract highest-value items first

**Implementation**:
- Max heap using array representation
- Binary heap properties maintained
- Operations: `insert()`, `extractTop()`, `heapifyUp()`, `heapifyDown()`

**Usage in Game**:
- Location: `Game.cpp`
- Loot items stored by value
- Best loot extracted first
- Console logs: `[DSA-Heap] Inserted element. Heap size: X`

**Code Example**:
```cpp
Heap<Item, std::greater<int>> lootHeap;  // Max heap

lootHeap.insert(Item("Gold Coin", "treasure", 0, 100));
lootHeap.insert(Item("Rusty Dagger", "weapon", 5, 10));

Item bestLoot = lootHeap.extractTop();  // Gets Gold Coin first
```

---

### 5. Binary Tree (Skill Tree)
**File**: `include/DataStructures/Tree.h`

**Purpose**: Hierarchical skill progression system

**Implementation**:
- Binary tree with left/right children
- Each node represents a skill
- Supports multiple traversal methods

**Usage in Game**:
- Location: `SkillTree.cpp`
- Root skill always unlocked
- Left/right branches for different paths
- Console logs: `[DSA-Tree] Level-order traversal`

**Code Example**:
```cpp
tree.setRoot(Skill("Basic Combat", "Foundation", 0));
auto left = tree.insertLeft(root, Skill("Power Strike", "Offense", 1));
tree.insertLeft(left, Skill("Berserker", "High damage", 2));

// Traverse all skills
tree.levelOrder([](const Skill& skill) {
    std::cout << skill.name << std::endl;
});
```

---

### 6. Graph (Dungeon Layout)
**File**: `include/DataStructures/Graph.h`

**Purpose**: Represent room connections and pathfinding

**Implementation**:
- Adjacency list representation
- Weighted edges for distances
- Supports directed and undirected edges

**Usage in Game**:
- Location: `Dungeon.cpp`
- Each room is a vertex
- Corridors are edges with weights
- Console logs: `[DSA-Graph] Added vertex. Total vertices: X`

**Code Example**:
```cpp
Graph<int> roomGraph;
roomGraph.addVertex(0);  // Room 0
roomGraph.addVertex(1);  // Room 1
roomGraph.addBidirectionalEdge(0, 1, 5);  // Distance 5

auto reachable = roomGraph.bfs(0);  // Find all reachable rooms
```

---

### 7. Hash Table (Item Database)
**File**: `include/DataStructures/HashTable.h`

**Purpose**: Fast item lookup by name/ID

**Implementation**:
- Uses `std::unordered_map`
- O(1) average lookup time
- Key-value pairs for items

**Usage in Game**:
- Location: `Game.cpp`
- Store all game items
- Quick search by item name
- Console logs: `[DSA-HashTable] Key found`

**Code Example**:
```cpp
HashTable<std::string, Item> itemDB;
itemDB.insert("sword", Item("Iron Sword", "weapon", 15, 50));

auto item = itemDB.get("sword");  // Fast lookup
if (item) {
    std::cout << "Found: " << item->name << std::endl;
}
```

---

## Algorithms Implementation

### 1. Breadth-First Search (BFS)
**File**: `include/DataStructures/Graph.h`

**Purpose**: Explore all connected rooms level by level

**Implementation**:
```cpp
std::vector<T> Graph::bfs(const T& start) {
    std::vector<T> result;
    std::unordered_set<T> visited;
    std::queue<T> q;
    
    q.push(start);
    visited.insert(start);
    
    while (!q.empty()) {
        T current = q.front();
        q.pop();
        result.push_back(current);
        
        for (const auto& neighbor : getNeighbors(current)) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                q.push(neighbor);
            }
        }
    }
    return result;
}
```

**Usage**: Find all rooms reachable from current position

**Time Complexity**: O(V + E) where V = vertices, E = edges

---

### 2. Depth-First Search (DFS)
**File**: `include/DataStructures/Graph.h`

**Purpose**: Explore dungeon by going as deep as possible first

**Implementation**:
```cpp
std::vector<T> Graph::dfs(const T& start) {
    std::vector<T> result;
    std::unordered_set<T> visited;
    std::stack<T> s;
    
    s.push(start);
    
    while (!s.empty()) {
        T current = s.top();
        s.pop();
        
        if (visited.find(current) == visited.end()) {
            visited.insert(current);
            result.push_back(current);
            
            for (const auto& neighbor : getNeighbors(current)) {
                if (visited.find(neighbor) == visited.end()) {
                    s.push(neighbor);
                }
            }
        }
    }
    return result;
}
```

**Usage**: Explore unknown areas systematically

**Time Complexity**: O(V + E)

---

### 3. Dijkstra's Algorithm
**File**: `include/DataStructures/Graph.h`

**Purpose**: Find shortest path to treasure/exit

**Implementation**:
```cpp
std::unordered_map<T, int> Graph::dijkstra(const T& start) {
    std::unordered_map<T, int> distances;
    
    for (const auto& pair : adjacencyList) {
        distances[pair.first] = INT_MAX;
    }
    distances[start] = 0;
    
    std::priority_queue<PQElement, 
                       std::vector<PQElement>, 
                       std::greater<PQElement>> pq;
    pq.push({0, start});
    
    while (!pq.empty()) {
        auto [dist, current] = pq.top();
        pq.pop();
        
        if (dist > distances[current]) continue;
        
        for (const auto& [neighbor, weight] : adjacencyList[current]) {
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

**Usage**: Calculate shortest path to all rooms

**Time Complexity**: O((V + E) log V)

---

## Performance Analysis

| Data Structure | Operation | Time Complexity | Space Complexity |
|----------------|-----------|-----------------|------------------|
| Stack | Push/Pop | O(1) | O(n) |
| Queue | Enqueue/Dequeue | O(1) | O(n) |
| Linked List | Insert/Delete | O(1) at head, O(n) search | O(n) |
| Heap | Insert/Extract | O(log n) | O(n) |
| Binary Tree | Insert/Search | O(log n) balanced | O(n) |
| Graph | Add Vertex/Edge | O(1) | O(V + E) |
| Hash Table | Insert/Search | O(1) average | O(n) |

| Algorithm | Time Complexity | Space Complexity |
|-----------|-----------------|------------------|
| BFS | O(V + E) | O(V) |
| DFS | O(V + E) | O(V) |
| Dijkstra | O((V + E) log V) | O(V) |

---

## Console Output Examples

When running the game, you'll see:

```
========================================
   DUNGEON EXPLORER - DSA Game
========================================

[Dungeon] Generating dungeon with 8 rooms...
[DSA-Graph] Added vertex. Total vertices: 1
[DSA-Graph] Added vertex. Total vertices: 2
[DSA-Graph] Added edge from 0 to 1 (weight: 5)

[Player] Initialized at position (2, 2)
[DSA-Stack] Pushed element. Stack size: 1

[Game] Creating Item Database (Hash Table)...
[DSA-HashTable] Inserted key. Table size: 1
[DSA-HashTable] Key found

[SkillTree] Initializing skill tree...
[DSA-Tree] Set root node
[DSA-Tree] Inserted left child
[DSA-Tree] Level-order traversal

[EnemyManager] Spawned Goblin at (5, 3) with 50 HP
[DSA-Queue] Enqueued element. Queue size: 1

[Dungeon] Visualizing BFS from room 0
[DSA-Graph] Running BFS from 0
[DSA-Graph] BFS visited 8 nodes

[Game] Initialization complete!
```

---

## Educational Value

This project demonstrates:

1. ✅ **Practical DSA Application**: Real-world use cases for each structure
2. ✅ **Algorithm Visualization**: See BFS/DFS/Dijkstra in action
3. ✅ **Performance Understanding**: Experience O(1) vs O(log n) vs O(n) operations
4. ✅ **Memory Management**: Smart pointers and proper cleanup
5. ✅ **Template Programming**: Generic data structures
6. ✅ **Object-Oriented Design**: Separation of concerns
7. ✅ **Game Architecture**: MVC-like pattern

---

## Testing Each Feature

1. **Stack**: Move around (WASD), then press B repeatedly to backtrack
2. **Queue**: Watch enemy turns in console
3. **Linked List**: Press I to view inventory
4. **Heap**: Defeat enemies to see loot ordering
5. **Tree**: Press K to view skill tree structure
6. **Graph**: Watch BFS/DFS/Dijkstra at startup
7. **Hash Table**: Item lookups logged in console

---

## Conclusion

Dungeon Explorer successfully integrates 7 major data structures and 3 pathfinding algorithms into a playable game experience. Each DSA concept is not just implemented but actively used in gameplay, with console logging to demonstrate operations in real-time.

This makes it an excellent educational tool for understanding how data structures and algorithms work in practical applications.
