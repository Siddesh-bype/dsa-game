---
description: # PROJECT RULES: DUNGEON EXPLORER (DSA C++ ENGINE)
---

## 1. Core Tech Stack & Standards
- **Language:** C++17 standard.
- **Engine:** SFML 3.0.0 (Note: Syntax differs from SFML 2.x. Use SFML 3 naming conventions, e.g., camelCase for methods).
- **Build System:** CMake + Ninja.
- **Memory Management:** STRICT use of `std::unique_ptr` for ownership. No raw `new`/`delete`.

## 2. The "Custom DSA" Prime Directive
**CRITICAL:** This is a Data Structures demonstration project.
- **Do NOT use:** `std::stack`, `std::queue`, `std::list`, `std::unordered_map` (for game logic), or `std::priority_queue`.
- **MUST USE:** The custom implementations located in `include/DataStructures/`:
  - `Stack.h` (Movement/Backtracking)
  - `Queue.h` (Turn Order)
  - `LinkedList.h` (Inventory)
  - `HashTable.h` (Item DB)
  - `Heap.h` / `PriorityQueue.h` (Loot/Pathfinding)
  - `SpatialHash.h` (Combat/Collision)
- *Exception:* You may use `std::vector` for contiguous memory buffers or backend implementation of the custom structures.

## 3. Architecture & Patterns
- **Managers:** Logic resides in Managers (`EnemyManager`, `ItemManager`), not in the entities themselves.
- **Entities:** `Player` and `Enemy` are data-heavy classes manipulated by Managers.
- **Optimization:** 
  - Always use `SpatialHash` for finding neighbors (query O(k)), never iterate all entities (O(n)).
  - Use `ObjectPool` for particle effects or projectiles.

## 4. Coding Style (Google C++ Style Derivative)
- **Headers:** `.h` files in `include/`, `.cpp` files in `src/`.
- **Naming:** 
  - Classes: `PascalCase`
  - Variables/Functions: `camelCase`
  - Member variables: `m_variableName` (optional but preferred for clarity).
- **Const Correctness:** Mark all "getter" functions and non-modifying methods as `const`.

## 5. Implementation Guidelines
- **Pathfinding:** Always use `AStar.h`. When requesting path logic, verify if `PathCache` (LRU) can be used first.
- **Loot:** When generating items, check `HashTable` for the definition and push to `LinkedList` (Inventory).
- **Combat:**
  1. Detect input.
  2. Query `SpatialHash` for targets.
  3. Push events to `Queue` (Turn system).

## 6. Build & Test Commands
- **Configure:** `cmake -B build -G "Ninja"`
- **Build:** `cmake --build build --config Release`
- **Run:** `./build/Release/DungeonExplorer.exe`