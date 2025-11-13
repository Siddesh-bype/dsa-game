# 🧩 Dungeon Explorer – 2D C++ Game Using DSA, SFML, and TGUI

A fully functional 2D tile-based adventure game demonstrating core Data Structures and Algorithms through gameplay mechanics.

![Version](https://img.shields.io/badge/version-1.0.0-blue)
![C++](https://img.shields.io/badge/C++-17-00599C?logo=cplusplus)
![SFML](https://img.shields.io/badge/SFML-2.6-8CC445)
![TGUI](https://img.shields.io/badge/TGUI-1.0-orange)

## 🎯 Overview

Dungeon Explorer is an educational game that demonstrates various data structures and algorithms in action. Each gameplay system is built around a specific DSA concept, making it perfect for learning while playing.

## 🧱 Features & DSA Integration

| Gameplay Element | Data Structure | Purpose |
|-----------------|----------------|---------|
| **Dungeon Map** | Graph + Array | Room layout and pathfinding |
| **Player Movement** | Stack | Backtracking path history |
| **Enemies** | Queue | Turn-based attack system |
| **Loot System** | Heap (Priority Queue) | Ranking best items |
| **Inventory** | Linked List | Dynamic item management |
| **Skill Tree** | Binary Tree | Upgrade path system |
| **Item Lookup** | Hash Table | Fast item search |
| **Pathfinding** | BFS/DFS/Dijkstra | Map exploration |

## 🧠 Algorithms Demonstrated

- **BFS (Breadth-First Search)**: Discover all connected rooms
- **DFS (Depth-First Search)**: Explore unexplored areas
- **Dijkstra's Algorithm**: Find shortest path to treasure
- **Heap Sort**: Order loot by value
- **Tree Traversal**: Navigate skill unlocks

## ⚙️ Technologies

- **Language**: C++17
- **Graphics**: SFML 2.6
- **UI System**: TGUI 1.0
- **Build System**: CMake 3.16+
- **Platform**: Windows, Linux, macOS

## 📁 Project Structure

```
DungeonExplorer/
├── assets/
│   ├── data/           # JSON configuration files
│   ├── tiles/          # Tile graphics (add your own)
│   ├── characters/     # Character sprites (add your own)
│   ├── ui/             # UI elements (add your own)
│   ├── sounds/         # Sound effects (add your own)
│   └── fonts/          # Fonts (add your own)
├── include/
│   ├── DataStructures/ # DSA implementations
│   │   ├── Stack.h
│   │   ├── Queue.h
│   │   ├── LinkedList.h
│   │   ├── Heap.h
│   │   ├── Tree.h
│   │   ├── Graph.h
│   │   └── HashTable.h
│   ├── Game.h
│   ├── Player.h
│   ├── Dungeon.h
│   ├── Enemy.h
│   ├── SkillTree.h
│   ├── UIManager.h
│   └── Renderer.h
├── src/
│   ├── main.cpp
│   └── [implementation files]
└── CMakeLists.txt
```

## 🚀 Building the Project

### Prerequisites

1. **C++ Compiler** with C++17 support
   - Windows: Visual Studio 2019+ or MinGW-w64
   - Linux: GCC 7+ or Clang 5+
   - macOS: Xcode 10+ or Clang

2. **CMake** 3.16 or higher
   - Download from: https://cmake.org/download/

3. **SFML 2.6**
   - Windows: Download from https://www.sfml-dev.org/download.php
   - Linux: `sudo apt-get install libsfml-dev`
   - macOS: `brew install sfml`

4. **TGUI 1.0+**
   - Download from: https://tgui.eu/download/
   - Or build from source: https://github.com/texus/TGUI

### Windows Build Instructions

#### Option 1: Using Visual Studio

```powershell
# Navigate to project directory
cd "C:\Users\sidde\OneDrive\Desktop\FSD\dsa game\DungeonExplorer"

# Create build directory
mkdir build
cd build

# Generate Visual Studio solution
cmake .. -G "Visual Studio 16 2019" -A x64

# Open the solution
start DungeonExplorer.sln

# Or build from command line
cmake --build . --config Release
```

#### Option 2: Using MinGW

```powershell
# Create build directory
mkdir build
cd build

# Generate Makefiles
cmake .. -G "MinGW Makefiles"

# Build
cmake --build .
```

### Linux Build Instructions

```bash
# Navigate to project directory
cd ~/DungeonExplorer

# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake libsfml-dev

# Install TGUI (if not available via package manager)
# Follow instructions at: https://tgui.eu/tutorials/1.0/linux/

# Create build directory
mkdir build && cd build

# Generate and build
cmake ..
make -j$(nproc)

# Run the game
./DungeonExplorer
```

### macOS Build Instructions

```bash
# Install dependencies
brew install cmake sfml tgui

# Navigate to project directory
cd ~/DungeonExplorer

# Create build directory
mkdir build && cd build

# Generate and build
cmake ..
make -j$(sysctl -n hw.ncpu)

# Run the game
./DungeonExplorer
```

## 🎮 Controls

| Key | Action |
|-----|--------|
| **W/↑** | Move Up |
| **A/←** | Move Left |
| **S/↓** | Move Down |
| **D/→** | Move Right |
| **B** | Backtrack (Stack) |
| **I** | Open Inventory |
| **K** | Open Skill Tree |
| **ESC** | Return to Menu |

## 📝 Console Logs

The game logs all DSA operations to the console for educational purposes:

```
[DSA-Stack] Pushed element. Stack size: 5
[DSA-Queue] Enqueued element. Queue size: 3
[DSA-Graph] Running BFS from 0
[DSA-Heap] Inserted element. Heap size: 8
[DSA-LinkedList] Appended element. List size: 4
[DSA-Tree] Level-order traversal
[DSA-HashTable] Key found
```

## 🎨 Adding Assets

To enhance the visual experience, add your own assets:

### Recommended Sources:

1. **Tilesets**: 
   - [0x72's Dungeon Tileset](https://0x72.itch.io/dungeontileset-ii)
   - [Kenney's Asset Packs](https://kenney.nl/)

2. **Character Sprites**:
   - [0x72's Tiny Dungeon](https://0x72.itch.io/dungeontileset-ii)

3. **UI Elements**:
   - [Kenney UI Pack](https://kenney.nl/assets/ui-pack)

4. **Fonts**:
   - [Press Start 2P](https://fonts.google.com/specimen/Press+Start+2P)

5. **Sounds**:
   - [Kenney Sound Pack](https://kenney.nl/assets/rpg-audio)
   - [OpenGameArt](https://opengameart.org/)

### Asset Placement:

```
assets/
├── tiles/      → Place .png tile images here
├── characters/ → Place .png sprite sheets here
├── ui/         → Place .png UI elements here
├── sounds/     → Place .wav or .ogg files here
└── fonts/      → Place .ttf font files here
```

## 🧪 Testing DSA Features

### 1. Stack (Player Movement)
- Move around the dungeon
- Press **B** to backtrack through your path
- Watch console logs show stack operations

### 2. Queue (Enemy Turns)
- Enemies attack in turn-based order
- Console shows queue enqueue/dequeue operations

### 3. Linked List (Inventory)
- Collect items as you explore
- Press **I** to view inventory
- Items stored in linked list structure

### 4. Heap (Loot System)
- Defeat enemies to get loot
- Best items extracted first (max heap)

### 5. Binary Tree (Skill Tree)
- Press **K** to view skill tree
- Unlock skills in hierarchical order
- Tree traversal demonstrated

### 6. Graph (Dungeon Layout)
- Rooms connected as graph nodes
- BFS, DFS, and Dijkstra demonstrated at startup

### 7. Hash Table (Item Database)
- Fast item lookup by name
- Console shows hash operations

## 🔧 Troubleshooting

### SFML Not Found
```bash
# Set SFML_DIR environment variable
export SFML_DIR=/path/to/SFML-2.6.0/lib/cmake/SFML
```

### TGUI Not Found
```bash
# Set TGUI_DIR environment variable
export TGUI_DIR=/path/to/TGUI-1.0/lib/cmake/TGUI
```

### CMake Error: C++17 Not Supported
- Ensure you have a modern compiler (GCC 7+, Clang 5+, MSVC 2017+)

### Missing DLLs (Windows)
- Copy SFML and TGUI DLLs to the same directory as the executable
- Usually found in `SFML-2.6.0/bin/` and `TGUI-1.0/bin/`

## 📚 Learning Resources

### Data Structures:
- [Stack](include/DataStructures/Stack.h) - LIFO structure for backtracking
- [Queue](include/DataStructures/Queue.h) - FIFO structure for turn order
- [Linked List](include/DataStructures/LinkedList.h) - Dynamic list for inventory
- [Heap](include/DataStructures/Heap.h) - Priority queue for loot
- [Binary Tree](include/DataStructures/Tree.h) - Hierarchical skill structure
- [Graph](include/DataStructures/Graph.h) - Room connections and pathfinding
- [Hash Table](include/DataStructures/HashTable.h) - Fast item lookup

### Algorithms:
- BFS: `Dungeon::visualizeBFS()`
- DFS: `Dungeon::visualizeDFS()`
- Dijkstra: `Dungeon::visualizeDijkstra()`

## 🤝 Contributing

This is an educational project. Feel free to:
- Add new DSA features
- Enhance graphics and UI
- Add more gameplay mechanics
- Improve documentation

## 📄 License

This project is created for educational purposes. Feel free to use and modify for learning.

## 🎓 Educational Value

This project demonstrates:
- ✅ Practical application of data structures
- ✅ Algorithm visualization
- ✅ Object-oriented design in C++
- ✅ Game architecture patterns
- ✅ SFML graphics programming
- ✅ GUI development with TGUI
- ✅ CMake build systems

## 📞 Support

For questions or issues:
1. Check the console logs for DSA operation details
2. Review the code comments in header files
3. Ensure all dependencies are correctly installed

---

**Built with ❤️ for learning Data Structures and Algorithms**

*Dungeon Explorer - Where Learning Meets Adventure!* 🎮✨
