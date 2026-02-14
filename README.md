# 🎮 Dungeon Explorer - DSA Game

A roguelike dungeon crawler built with **C++ and SFML 3.0**, demonstrating **15+ Data Structures and Algorithms** in a complete, polished game implementation.

![Build Status](https://img.shields.io/badge/build-passing-brightgreen) ![C++](https://img.shields.io/badge/C++-17-blue) ![SFML](https://img.shields.io/badge/SFML-3.0-orange) ![License](https://img.shields.io/badge/license-Educational-purple)

---

## 📋 Table of Contents

- [Game Overview](#-game-overview)
- [Architecture](#-architecture)
- [Data Structures & Algorithms](#-data-structures--algorithms)
- [Features](#-features)
- [Controls](#-controls)
- [Build Instructions](#-build-instructions)
- [Project Structure](#-project-structure)
- [Technical Details](#-technical-details)

---

## 🎯 Game Overview

Dungeon Explorer is a **turn-based roguelike** where players navigate procedurally generated dungeons across 10 floors, fight enemies with adaptive AI, collect loot, unlock skills, and progress to victory.

### Gameplay Loop

```mermaid
flowchart LR
    A[Enter Floor] --> B[Explore Dungeon]
    B --> C{Enemy?}
    C -->|Yes| D[Combat]
    C -->|No| E[Collect Loot]
    D --> F[Enemy Dies]
    F --> G[Drop Items]
    G --> E
    E --> H{Find Stairs?}
    H -->|No| B
    H -->|Yes| I[Descend]
    I --> A
```

---

## 🏗️ Architecture

### System Architecture

```mermaid
graph TB
    subgraph Core["Core Game Loop"]
        Game[Game.cpp]
        Game --> Player
        Game --> Dungeon
        Game --> EnemyManager
        Game --> UIManager
    end
    
    subgraph DSA["Data Structures"]
        Stack[Stack - Movement History]
        Queue[Queue - Turn Order]
        LinkedList[LinkedList - Inventory]
        HashTable[HashTable - Item Database]
        Heap[Heap - Loot Priority]
        Tree[Binary Tree - Skills]
        Graph[Graph - Room Navigation]
        AStar[A* - Pathfinding]
        SpatialHash[SpatialHash - Combat Queries]
    end
    
    subgraph Systems["Game Systems"]
        Player --> Stack
        Player --> LinkedList
        EnemyManager --> Queue
        EnemyManager --> AStar
        EnemyManager --> SpatialHash
        Dungeon --> Graph
        ItemManager --> HashTable
        SkillTree --> Tree
        LootSystem --> Heap
    end
    
    subgraph Rendering["Rendering & Audio"]
        Renderer[Renderer.cpp]
        UIManager[UIManager.cpp]
        ParticleSystem[ParticleSystem.cpp]
        SoundManager[SoundManager.cpp]
        UIVisualEffects[UIVisualEffects.cpp]
    end
```

### Class Diagram

```mermaid
classDiagram
    class Game {
        -Player player
        -Dungeon dungeon
        -EnemyManager enemies
        -UIManager ui
        +run()
        +handleInput()
        +update()
        +render()
    }
    
    class Player {
        -Stack~Position~ moveHistory
        -LinkedList~ItemNew~ inventory
        -int health, mana, level
        +move()
        +attack()
        +useItem()
        +backtrack()
    }
    
    class Dungeon {
        -Graph~int~ roomGraph
        -vector~Room~ rooms
        +generate()
        +findPath()
        +dijkstra()
    }
    
    class EnemyManager {
        -Queue~Enemy~ turnQueue
        -SpatialHash~int~ enemyGrid
        -AStarPathfinder pathfinder
        +update()
        +findNearbyEnemies()
        +buildSpatialIndex()
    }
    
    class ItemManager {
        -HashTable~string,ItemNew~ items
        -Heap~Loot~ premiumLoot
        +getItem()
        +getRandomDrop()
        +generateLootTable()
    }
    
    class SkillTree {
        -Tree~Skill~ skillTree
        +unlockSkill()
        +getSkillPath()
    }
    
    class SoundManager {
        -unordered_map buffers
        -vector~Sound~ sounds
        +playSound()
        +generateSound()
        +generateExplosion()
    }
    
    class ItemWheel {
        -vector~ItemNew~ items
        -float openProgress
        +open()
        +close()
        +handleClick()
    }
    
    Game --> Player
    Game --> Dungeon
    Game --> EnemyManager
    Game --> ItemManager
    Game --> SkillTree
    Game --> SoundManager
    Game --> ItemWheel
```

---

## 📊 Data Structures & Algorithms

### 15 DSA Implementations

| # | DSA | Location | Purpose | Complexity |
|---|-----|----------|---------|------------|
| 1 | **Stack** | `Stack.h` | Player movement history (backtracking) | O(1) push/pop |
| 2 | **Queue** | `Queue.h` | Enemy turn order management | O(1) enqueue/dequeue |
| 3 | **Linked List** | `LinkedList.h` | Player inventory storage | O(1) insert, O(n) search |
| 4 | **Hash Table** | `HashTable.h` | Item database lookup | O(1) average lookup |
| 5 | **Heap (Max)** | `Heap.h` | Premium loot prioritization | O(log n) insert |
| 6 | **Binary Tree** | `Tree.h` | Skill tree progression | O(log n) traversal |
| 7 | **Graph** | `Graph.h` | Room connectivity | O(V+E) traversal |
| 8 | **Priority Queue** | `PriorityQueue.h` | A* open set management | O(log n) operations |
| 9 | **A* Pathfinding** | `AStar.h` | Enemy smart navigation | O(E log V) |
| 10 | **Spatial Hash** | `SpatialHash.h` | O(k) combat queries | O(1) cell lookup |
| 11 | **Object Pool** | `ObjectPool.h` | Particle memory reuse | O(1) acquire/release |
| 12 | **LRU Cache** | `LRUCache.h` | Asset memory management | O(1) get/put |
| 13 | **Path Cache** | `GameUtils.h` | A* result caching | O(1) cache hit |
| 14 | **Distance Algorithms** | `GameUtils.h` | Manhattan, Euclidean, Chebyshev | O(1) |
| 15 | **Input Debounce** | `GameUtils.h` | Key repeat prevention | O(1) |

### DSA Flow Diagram

```mermaid
flowchart TD
    subgraph Movement["Player Movement"]
        M1[WASD Input] --> M2[Stack.push Position]
        M3[B Key] --> M4[Stack.pop]
        M4 --> M5[Move to Previous]
    end
    
    subgraph Combat["Combat System"]
        C1[Player Attack] --> C2[SpatialHash.query]
        C2 --> C3[Find Nearby Enemies]
        C3 --> C4[Queue Enemy Turns]
        C4 --> C5[Process Turn Order]
    end
    
    subgraph Loot["Loot System"]
        L1[Enemy Dies] --> L2[HashTable.lookup]
        L2 --> L3[Generate Drop]
        L3 --> L4[Heap.insert if Rare]
        L4 --> L5[LinkedList.add to Inventory]
    end
    
    subgraph Navigation["Enemy AI"]
        N1[Enemy Update] --> N2[A* Pathfinding]
        N2 --> N3[Graph.getNeighbors]
        N3 --> N4[PriorityQueue Processing]
        N4 --> N5[Move Towards Player]
    end
```

---

## 🎮 Features

### Core Systems
- ✅ **Procedural Generation**: 10 unique dungeon floors with DungeonLevelManager
- ✅ **Turn-Based Combat**: Strategic enemy encounters with skill hotkeys
- ✅ **Loot System**: 30+ items with rarity tiers and drop tables
- ✅ **Skill Tree**: 20+ skills with unlock dependencies
- ✅ **Achievement System**: 50+ achievements with rewards
- ✅ **Save/Load**: Persistent game state with SaveSystem
- ✅ **Settings Manager**: Volume, graphics quality, and difficulty settings
- ✅ **Shop System**: Buy/sell items with gold economy

### Visual & Audio Features
- ✅ **Particle Effects**: Blood, sparks, magic effects via ObjectPool
- ✅ **Floating Text**: Damage numbers, notifications, status updates
- ✅ **Mini-map**: Real-time dungeon overview with fog of war
- ✅ **DSA Visualization**: Graph paths, stack trails, A* visualization
- ✅ **Item Wheel**: Radial selection UI for quick item access (Tab key)
- ✅ **UI Visual Effects**: Smooth transitions, glow effects, animations
- ✅ **Procedural Audio**: SoundManager with generated sound effects

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| **WASD** | Move player |
| **B** | Backtrack (Stack undo) |
| **Space** | Attack nearest enemy |
| **E** | Interact (pickup, doors, stairs) |
| **I** | Toggle inventory |
| **T** | Toggle skill tree |
| **Tab** | Open item wheel (radial selection) |
| **U** | Use selected item |
| **X** | Drop selected item |
| **1-5** | Activate skills |
| **O** | Unlock next skill (debug) |
| **M** | Toggle mini-map |
| **F3** | Toggle debug overlay |
| **F4** | Toggle DSA visualization |
| **ESC** | Exit game |

---

## 🛠️ Build Instructions

### Prerequisites
- **C++ Compiler**: GCC 13+ or MSVC 2022
- **CMake**: 3.10+
- **SFML**: 3.0.0

### Build Commands

```bash
# Navigate to project directory
cd "path/to/DungeonExplorer"

# Generate build files
cmake -B build -G "Ninja"

# Compile
cmake --build build --config Release

# Run game
./build/Release/DungeonExplorer.exe
```

---

## 📁 Project Structure

```
DungeonExplorer/
├── include/
│   ├── DataStructures/          # 12 DSA implementations
│   │   ├── Stack.h              # LIFO container
│   │   ├── Queue.h              # FIFO container
│   │   ├── LinkedList.h         # Dynamic list
│   │   ├── HashTable.h          # Key-value store
│   │   ├── Heap.h               # Priority heap
│   │   ├── Tree.h               # Binary tree
│   │   ├── Graph.h              # Adjacency list
│   │   ├── PriorityQueue.h      # Min/max heap queue
│   │   ├── AStar.h              # A* pathfinding
│   │   ├── SpatialHash.h        # Spatial partitioning
│   │   ├── ObjectPool.h         # Memory pooling
│   │   └── LRUCache.h           # Cache eviction
│   ├── Game.h                   # Main game loop
│   ├── Player.h                 # Player entity
│   ├── Enemy.h                  # Enemy AI
│   ├── Dungeon.h                # Level generation
│   ├── DungeonLevelManager.h    # Floor progression
│   ├── ItemWheel.h              # Radial item selection UI
│   ├── SoundManager.h           # Procedural audio system
│   ├── SettingsManager.h        # Persistent settings
│   ├── SaveSystem.h             # Game state persistence
│   ├── UIVisualEffects.h        # UI animations & effects
│   ├── GameUtils.h              # Consolidated utilities
│   └── ...                      # (27 header files total)
├── src/                         # Implementation files (25 files)
│   ├── Game.cpp                 # ~50KB main game logic
│   ├── Player.cpp               # Player mechanics
│   ├── Enemy.cpp                # Enemy AI & combat
│   ├── Dungeon.cpp              # Procedural generation
│   ├── SkillTree.cpp            # Skill unlock system
│   ├── UIManager.cpp            # HUD & menus
│   ├── AchievementSystem.cpp    # Achievement tracking
│   ├── ItemWheel.cpp            # Radial menu logic
│   ├── SoundManager.cpp         # Audio generation
│   └── ...
├── assets/
│   ├── data/                    # JSON configurations
│   │   ├── items.json           # 30+ item definitions
│   │   ├── enemies.json         # 8 enemy types
│   │   └── settings.json        # User preferences
│   └── kenney/                  # Tileset sprites (100+ assets)
├── doc/                         # Additional documentation (14 files)
├── test/                        # Unit tests
├── build/                       # Build output
├── CMakeLists.txt               # Build configuration
├── PROJECT_REPORT.md            # Detailed project report
└── README.md                    # This file
```

---

## 🔧 Technical Details

### Memory Management
- **Smart Pointers**: `std::unique_ptr` for ownership
- **Object Pooling**: Particle system reuses objects
- **LRU Cache**: Texture memory management

### Performance Optimizations
- **Spatial Hashing**: O(k) enemy queries vs O(n)
- **Path Caching**: Reuses A* results for common paths
- **Consolidated Utilities**: 22+ redundant includes removed
- **Constexpr Constants**: Magic numbers replaced with compile-time constants

### Audio System
- **Procedural Generation**: SoundManager generates sound effects at runtime
- **Wave Types**: Square, Triangle, Sine, White Noise
- **Dynamic Effects**: Explosions, powerups, hits, jumps

### Settings Persistence
- **JSON-based**: Settings saved to `settings.json`
- **Configurable**: Volume, graphics quality, difficulty, fullscreen

### Code Statistics
| Metric | Count |
|--------|-------|
| Source Files | 25 |
| Header Files | 27 |
| DSA Implementations | 15 |
| Lines of Code | ~18,000 |
| Items | 30+ |
| Skills | 20+ |
| Achievements | 50+ |
| Enemy Types | 8 |
| Dungeon Floors | 10 |

---

## 🧪 Testing

```bash
# Run unit tests
cd build
ctest --output-on-failure
```

Tests cover:
- Data structure operations (Stack, Queue, LinkedList, etc.)
- Pathfinding algorithms (A*, Dijkstra)
- Game state serialization

---

## 📜 License

Educational project for Data Structures and Algorithms demonstration.

---

**Version**: 2.1 | **Updated**: December 2025 | **Build**: ✅ Passing
