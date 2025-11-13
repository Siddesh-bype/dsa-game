# Kenney Tiny Dungeon Sprite Mappings

This document details the corrected tile indexes for the Kenney Tiny Dungeon tilemap.

**Tilemap Specifications:**
- Source: `assets/kenney/kenney_tiny-dungeon/Tilemap/tilemap.png`
- Grid: 12 columns × 11 rows = 132 total tiles
- Tile size: 16×16 pixels
- Spacing: 1 pixel between tiles
- Display scale: 2.0× (32×32 pixels on screen)

## Floor Tiles (Rows 0-2)

### Light Floors (Floors 1-3)
- **Tiles 0-5**: Light brown/dirt floor variants
- Used on dungeon floors 1-3

### Mid-Tone Floors (Floors 4-6)
- **Tiles 6-11**: Grey/stone floor variants
- Used on dungeon floors 4-6

### Dark Floors (Floors 7-10)
- **Tiles 12-17**: Dark stone/dungeon floor variants
- Used on dungeon floors 7-10

## Structural Elements (Rows 1-3)

### Walls
- **Tile 23**: Solid wall tile

### Doors
- **Tile 30**: Closed door

### Stairs
- **Tile 36**: Stairs up (exit)
- **Tile 37**: Stairs down (entrance)

### Props
- **Tile 38**: Treasure chest

## Effects (Row 4)

### Lighting
- **Tile 50**: Glow/light effect (rendered under stairs)

## Player Characters (Rows 5-7, tiles 60-83)

### Player Classes
- **Tile 84**: Warrior/Knight class
- **Tile 85**: Rogue/Thief class
- **Tile 86**: Mage/Wizard class

**Usage in code:**
```cpp
// Player.cpp
if (characterClass == "Warrior") tileIndex = 84;
else if (characterClass == "Rogue") tileIndex = 85;
else if (characterClass == "Mage") tileIndex = 86;
```

## Enemy Characters (Rows 8-10, tiles 96-119)

### Basic Enemies (Early Floors)
- **Tile 112**: Slime (potion-like creature)
- **Tile 104**: Goblin (small humanoid)
- **Tile 105**: Orc (warrior)
- **Tile 106**: Skeleton

### Advanced Enemies (Mid Floors)
- **Tile 100**: Dark Mage
- **Tile 102**: Gargoyle
- **Tile 103**: Minotaur
- **Tile 108**: Shadow/Wraith

### Boss Enemies (Deep Floors)
- **Tile 109**: Vampire
- **Tile 110**: Lich/Necromancer
- **Tile 111**: Dragon
- **Tile 107**: Boss (generic large demon/boss)

**Usage in code:**
```cpp
// Enemy.cpp
if (enemy.name.find("Slime") != std::string::npos) tileIndex = 112;
else if (enemy.name.find("Goblin") != std::string::npos) tileIndex = 104;
else if (enemy.name.find("Orc") != std::string::npos) tileIndex = 105;
// ... etc
```

## Coordinate Calculation

The tile index is converted to sprite coordinates using:

```cpp
int columns = 12;          // 12 columns per row
int tileSizePx = 16;       // 16×16 base tile size
int spacing = 1;           // 1px spacing between tiles

// Calculate row and column
int tx = tileIndex % columns;      // Column (0-11)
int ty = tileIndex / columns;      // Row (0-10)

// Calculate pixel position in spritesheet
int sx = tx * (tileSizePx + spacing);  // X position
int sy = ty * (tileSizePx + spacing);  // Y position

// Create texture rect
sf::IntRect rect({sx, sy}, {tileSizePx, tileSizePx});
```

## Rendering

All sprites are rendered with:
- **Origin**: (8, 8) - center of 16×16 tile for pixel-perfect rotation
- **Scale**: 2.0× (displays as 32×32 pixels)
- **Position offset**: +16px in x and y to compensate for centered origin

### Visual Effects

### Floor Progression
The floor uses an improved pseudo-random variant selector for more natural variety:
```cpp
// Uses multiple prime numbers to avoid repeating patterns
int variant = ((x * 31 + y * 17 + x * y * 3) % 100) % 12;
```

This creates more organic-looking floor patterns instead of repetitive grids.

### Exit Glow Effect
Exit stairs are rendered with a glow effect layered underneath:
1. Draw tile 50 (glow) at position
2. Draw tile 36 (stairs) on top

This creates a visual highlight for the level exit.

## File Locations

### Modified Files
- `src/Dungeon.cpp` - Floor, wall, door, stairs, chest rendering
- `src/Player.cpp` - Player character sprite selection
- `src/Enemy.cpp` - Enemy sprite selection

### Asset Location
- Spritesheet: `assets/kenney/kenney_tiny-dungeon/Tilemap/tilemap.png`
- TSX Definition: `assets/kenney/kenney_tiny-dungeon/Tiled/sampleSheet.tsx`

## Changes Made

### Previous (Incorrect) Mappings
- Floor tiles: 0-29 (incorrect range, wrong progression)
- Walls: tile 12 (too light)
- Stairs: tile 26 (wrong tile)
- Doors: tile 27 (wrong tile)
- Player: tiles 60, 61, 70 (incorrect positions)
- Enemies: tiles 64-76 (scattered, inconsistent)

### Current (Corrected) Mappings
- Floor tiles: 0-17 (proper Kenney floor tiles, 3 theme groups)
- Walls: tile 23 (proper solid wall)
- Stairs up: tile 36
- Stairs down: tile 37
- Doors: tile 30
- Glow: tile 50
- Chests: tile 38
- Player: tiles 84-86 (proper character row)
- Enemies: tiles 96-110 (proper monster rows)

These corrections ensure all sprites match the actual Kenney Tiny Dungeon tilemap layout.
