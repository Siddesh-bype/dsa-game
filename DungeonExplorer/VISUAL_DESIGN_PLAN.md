# 🎨 VISUAL DESIGN PLAN - Dungeon Explorer (Pure SFML 3.x)

**Date:** November 10, 2025  
**Status:** Build Complete - Now Adding Visual Polish  
**Tech Stack:** C++17, SFML 3.0.2, Pure SFML Rendering (No TGUI)

---

## 🎯 Design Philosophy

- **Art Style:** 16-bit pixel-art dungeon aesthetic
- **Layout:** Top-down 2D grid (32×32px tiles)
- **Tone:** Dark fantasy with torch lighting
- **UI:** Pure SFML shapes, text, and sprites
- **DSA Visualization:** Real-time overlay showing data structure operations

---

## ✅ Already Implemented

- ✅ Window creation (800×600)
- ✅ Grid-based dungeon rendering
- ✅ Player (green square) rendering
- ✅ Enemy (red squares) rendering
- ✅ Health bars above enemies
- ✅ Basic UI shapes (health bar background/foreground)
- ✅ All 7 DSA structures functional with console logging

---

## 🧱 PHASE 1: Enhanced Tile Rendering

### Current State
```cpp
// Simple colored rectangles for tiles
sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));
tile.setFillColor(sf::Color::Black); // Empty
```

### Enhancement Plan

**1.1 Textured Tiles** (Optional - can use colored tiles with borders)
```cpp
// Add tile borders for clarity
tile.setOutlineThickness(1.0f);
tile.setOutlineColor(sf::Color(40, 40, 40));

// Different shades for different tile types
TileType::Wall     -> sf::Color(64, 64, 64)     // Dark gray
TileType::Floor    -> sf::Color(40, 35, 30)      // Brown stone
TileType::Door     -> sf::Color(150, 120, 60)    // Gold trim
TileType::Treasure -> sf::Color(200, 200, 50)    // Yellow glow
```

**1.2 Room Highlighting**
- Visited rooms: Light green overlay (sf::Color(50, 100, 50, 100))
- Current room: Blue glow outline
- Unexplored: Dark overlay (sf::Color(0, 0, 0, 150))

**Files to Modify:**
- `src/Dungeon.cpp` - Enhanced render() method
- Add tile borders and room overlays

---

## 🧩 PHASE 2: Enhanced Entity Sprites

### Current State
```cpp
// Simple colored squares
sf::RectangleShape playerShape(sf::Vector2f(tileSize * 0.8f, tileSize * 0.8f));
playerShape.setFillColor(sf::Color::Green);
```

### Enhancement Plan

**2.1 Player Visual Upgrades**
```cpp
// Add circular shape option
sf::CircleShape playerCircle(tileSize * 0.4f);
playerCircle.setFillColor(sf::Color(0, 200, 255));  // Cyan
playerCircle.setOutlineThickness(2.0f);
playerCircle.setOutlineColor(sf::Color::White);

// Add directional indicator (triangle)
sf::ConvexShape directionArrow;
// Points toward last movement direction
```

**2.2 Enemy Visual Upgrades**
```cpp
// Different enemy types with different colors
Goblin: sf::Color(200, 50, 50)   // Red
Orc:    sf::Color(150, 30, 30)   // Dark red
Boss:   sf::Color(255, 0, 0)     // Bright red with glow

// Add pulsing animation for alive enemies
float pulse = 0.8f + 0.2f * sin(time * 3.0f);
enemyShape.setScale(pulse, pulse);
```

**2.3 Loot Indicators**
```cpp
// Gold coin sprite (yellow circle with sparkle)
sf::CircleShape coin(8.0f);
coin.setFillColor(sf::Color(255, 215, 0));
coin.setOutlineColor(sf::Color(255, 255, 100));
```

**Files to Modify:**
- `src/Player.cpp` - Enhanced render()
- `src/Enemy.cpp` - Enhanced render()
- `src/Dungeon.cpp` - Add loot rendering

---

## 🧠 PHASE 3: Lighting & Visual Effects

### 3.1 Radial Light Around Player
```cpp
// Create gradient circle for torch effect
sf::CircleShape light(150.0f, 50);
light.setFillColor(sf::Color(255, 200, 100, 80));  // Warm glow
light.setPosition(playerPos - sf::Vector2f(150, 150));

// Optional: SFML shader for smooth falloff
sf::Shader lightShader;
lightShader.loadFromMemory(R"(
    uniform vec2 lightPos;
    uniform float radius;
    void main() {
        vec2 pos = gl_FragCoord.xy;
        float dist = length(pos - lightPos);
        float brightness = 1.0 - smoothstep(0.0, radius, dist);
        gl_FragColor = vec4(vec3(brightness), 1.0);
    }
)", sf::Shader::Fragment);
```

### 3.2 Shadow Overlay for Unexplored Areas
```cpp
// Dim tiles not visited by BFS/DFS
if (!visited[roomId]) {
    sf::RectangleShape shadow(tileSize);
    shadow.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(shadow);
}
```

**Files to Create:**
- `assets/shaders/light.frag` - Fragment shader for lighting
- `include/LightingSystem.h` - Lighting manager
- `src/LightingSystem.cpp` - Implementation

---

## 🧱 PHASE 4: HUD & UI Panels (Pure SFML)

### 4.1 Top HUD Bar
```cpp
// Background panel
sf::RectangleShape hudBg({800.f, 60.f});
hudBg.setPosition(0, 0);
hudBg.setFillColor(sf::Color(20, 20, 30, 200));

// Health bar (already exists - enhance)
sf::Text hpText(font);
hpText.setString("HP: " + std::to_string(health) + "/" + std::to_string(maxHealth));
hpText.setCharacterSize(16);
hpText.setPosition(10, 10);

// XP bar
sf::RectangleShape xpBar({200.f, 10.f});
xpBar.setFillColor(sf::Color(100, 100, 255));

// Level indicator
sf::Text levelText(font);
levelText.setString("LVL " + std::to_string(level));
```

### 4.2 Mini-Map (Bottom Right)
```cpp
// Small representation of dungeon graph
sf::RectangleShape miniMapBg({150.f, 150.f});
miniMapBg.setPosition(630, 430);
miniMapBg.setFillColor(sf::Color(10, 10, 20, 180));

// Draw rooms as small squares
for (Room& room : rooms) {
    sf::RectangleShape miniRoom({8.f, 8.f});
    miniRoom.setPosition(mapPos);
    miniRoom.setFillColor(visited ? sf::Color::Green : sf::Color::Gray);
}

// Draw edges as lines
sf::Vertex line[] = {
    sf::Vertex(room1Pos, sf::Color(100, 100, 100)),
    sf::Vertex(room2Pos, sf::Color(100, 100, 100))
};
window.draw(line, 2, sf::Lines);
```

### 4.3 Inventory Panel (Linked List Display)
```cpp
// Toggle with 'I' key
if (inventoryVisible) {
    sf::RectangleShape invBg({300.f, 400.f});
    invBg.setPosition(250, 100);
    invBg.setFillColor(sf::Color(30, 30, 40, 230));
    
    // Title
    sf::Text title(font);
    title.setString("=== INVENTORY ===");
    title.setPosition(280, 110);
    
    // List items from LinkedList
    float yOffset = 150.f;
    player.getInventory().traverse([&](const Item& item) {
        sf::Text itemText(font);
        itemText.setString("- " + item.name + " ($" + std::to_string(item.value) + ")");
        itemText.setPosition(270, yOffset);
        window.draw(itemText);
        yOffset += 25.f;
    });
}
```

### 4.4 Skill Tree Panel (Binary Tree Display)
```cpp
// Toggle with 'K' key
if (skillTreeVisible) {
    sf::RectangleShape treeBg({500.f, 450.f});
    treeBg.setPosition(150, 75);
    treeBg.setFillColor(sf::Color(25, 30, 40, 240));
    
    // Draw tree nodes
    drawTreeNode(rootSkill, 400, 120, 80, 60);
}

void drawTreeNode(Node* node, float x, float y, float offsetX, float offsetY) {
    // Node circle
    sf::CircleShape nodeCircle(25.f);
    nodeCircle.setPosition(x - 25, y - 25);
    nodeCircle.setFillColor(node->unlocked ? sf::Color::Green : sf::Color(80, 80, 80));
    
    // Skill name
    sf::Text skillName(font);
    skillName.setString(node->data.name);
    skillName.setCharacterSize(12);
    
    // Recursive draw children with connecting lines
    if (node->left) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(x, y), sf::Color::White),
            sf::Vertex(sf::Vector2f(x - offsetX, y + offsetY), sf::Color::White)
        };
        window.draw(line, 2, sf::Lines);
        drawTreeNode(node->left, x - offsetX, y + offsetY, offsetX/2, offsetY);
    }
    if (node->right) {
        // Similar for right child
    }
}
```

**Files to Modify:**
- `include/UIManager.h` - Add panel rendering methods
- `src/UIManager.cpp` - Implement panel drawing
- `src/Game.cpp` - Toggle panels with keys (I, K, M)

---

## 🧩 PHASE 5: DSA Visualization Overlays

### 5.1 Stack Trail (Player Path)
```cpp
// Render fading footsteps from stack history
const auto& pathHistory = player.getPathHistory();
float alpha = 255.f;
for (Position pos : pathHistory) {
    sf::CircleShape footprint(4.f);
    footprint.setPosition(pos.x * tileSize, pos.y * tileSize);
    footprint.setFillColor(sf::Color(100, 200, 255, alpha));
    window.draw(footprint);
    alpha *= 0.85f;  // Fade older positions
}
```

### 5.2 Queue Indicator (Combat Order)
```cpp
// Show next 3 enemies in turn order
sf::RectangleShape queuePanel({180.f, 80.f});
queuePanel.setPosition(10, 520);
queuePanel.setFillColor(sf::Color(40, 20, 20, 200));

sf::Text queueTitle(font);
queueTitle.setString("Turn Order:");
queueTitle.setPosition(15, 525);

// Draw enemy icons in queue order
for (int i = 0; i < 3; i++) {
    sf::CircleShape enemyIcon(12.f);
    enemyIcon.setPosition(20 + i * 50, 550);
    enemyIcon.setFillColor(sf::Color::Red);
    window.draw(enemyIcon);
}
```

### 5.3 Graph Visualization (BFS/DFS Paths)
```cpp
// When visualizing algorithms, draw colored lines
if (visualizingBFS) {
    for (auto& edge : bfsPath) {
        sf::Vertex line[] = {
            sf::Vertex(edge.start, sf::Color::Green),
            sf::Vertex(edge.end, sf::Color::Green)
        };
        window.draw(line, 2, sf::Lines);
    }
}

// Dijkstra shortest path in blue
if (visualizingDijkstra) {
    for (auto& edge : shortestPath) {
        sf::Vertex line[] = {
            sf::Vertex(edge.start, sf::Color(100, 150, 255)),
            sf::Vertex(edge.end, sf::Color(100, 150, 255))
        };
        window.draw(line, 2, sf::Lines);
    }
}
```

### 5.4 Heap Loot Popup
```cpp
// When collecting loot, show animated popup
if (lootCollected) {
    float popupY = 300 - (time * 50);  // Rise upward
    float popupAlpha = 255 - (time * 200);  // Fade out
    
    sf::Text lootText(font);
    lootText.setString("+ " + itemName + " (+" + std::to_string(value) + " gold)");
    lootText.setPosition(350, popupY);
    lootText.setFillColor(sf::Color(255, 215, 0, popupAlpha));
    lootText.setOutlineColor(sf::Color(0, 0, 0, popupAlpha));
    lootText.setOutlineThickness(2);
    window.draw(lootText);
}
```

**Files to Create:**
- `include/DSAVisualizer.h` - DSA overlay renderer
- `src/DSAVisualizer.cpp` - Implementation

---

## 🧱 PHASE 6: Main Menu Screen

### 6.1 Title Screen
```cpp
void renderMainMenu(sf::RenderWindow& window) {
    // Background (dark with gradient effect)
    sf::RectangleShape bg({800.f, 600.f});
    bg.setFillColor(sf::Color(10, 10, 15));
    window.draw(bg);
    
    // Title text
    sf::Text title(font);
    title.setString("DUNGEON EXPLORER");
    title.setCharacterSize(48);
    title.setPosition(150, 100);
    title.setFillColor(sf::Color(200, 200, 255));
    title.setOutlineColor(sf::Color(50, 50, 150));
    title.setOutlineThickness(3);
    window.draw(title);
    
    // Subtitle
    sf::Text subtitle(font);
    subtitle.setString("A Data Structures Adventure");
    subtitle.setCharacterSize(16);
    subtitle.setPosition(250, 160);
    subtitle.setFillColor(sf::Color(150, 150, 200));
    window.draw(subtitle);
    
    // Menu options
    drawMenuButton(window, "START GAME", 350, 250, selectedOption == 0);
    drawMenuButton(window, "CONTROLS", 350, 310, selectedOption == 1);
    drawMenuButton(window, "EXIT", 350, 370, selectedOption == 2);
    
    // Instructions
    sf::Text controls(font);
    controls.setString("Use Arrow Keys to Navigate | Enter to Select");
    controls.setCharacterSize(12);
    controls.setPosition(200, 500);
    controls.setFillColor(sf::Color(100, 100, 120));
    window.draw(controls);
}

void drawMenuButton(sf::RenderWindow& window, const std::string& text, 
                   float x, float y, bool selected) {
    sf::RectangleShape button({200.f, 40.f});
    button.setPosition(x, y);
    button.setFillColor(selected ? sf::Color(80, 80, 120) : sf::Color(50, 50, 70));
    button.setOutlineColor(sf::Color(100, 100, 150));
    button.setOutlineThickness(2);
    window.draw(button);
    
    sf::Text buttonText(font);
    buttonText.setString(text);
    buttonText.setCharacterSize(18);
    buttonText.setPosition(x + 20, y + 10);
    buttonText.setFillColor(selected ? sf::Color::White : sf::Color(180, 180, 200));
    window.draw(buttonText);
}
```

**Files to Modify:**
- `include/Game.h` - Add MainMenu state
- `src/Game.cpp` - Implement menu rendering

---

## 🧠 PHASE 7: Animations & Polish

### 7.1 Smooth Movement
```cpp
// Interpolate between grid positions
sf::Vector2f currentPos = {player.x * tileSize, player.y * tileSize};
sf::Vector2f targetPos = {player.targetX * tileSize, player.targetY * tileSize};
sf::Vector2f renderPos = lerp(currentPos, targetPos, movementProgress);

playerShape.setPosition(renderPos);
```

### 7.2 Attack Flash
```cpp
if (attacking) {
    enemyShape.setFillColor(sf::Color(255, 255, 255));  // Flash white
    attackTimer -= deltaTime;
    if (attackTimer <= 0) attacking = false;
}
```

### 7.3 Particle System (Simple)
```cpp
struct Particle {
    sf::Vector2f pos;
    sf::Vector2f velocity;
    float lifetime;
    sf::Color color;
};

// On loot pickup, spawn particles
for (int i = 0; i < 10; i++) {
    Particle p;
    p.pos = lootPos;
    p.velocity = {randomFloat(-50, 50), randomFloat(-100, -50)};
    p.lifetime = 1.0f;
    p.color = sf::Color(255, 215, 0);
    particles.push_back(p);
}

// Update and render
for (auto& p : particles) {
    p.pos += p.velocity * deltaTime;
    p.lifetime -= deltaTime;
    
    sf::CircleShape particle(2.f);
    particle.setPosition(p.pos);
    particle.setFillColor(sf::Color(p.color.r, p.color.g, p.color.b, p.lifetime * 255));
    window.draw(particle);
}
```

---

## ✅ Implementation Priority

**HIGH PRIORITY (Visual Clarity):**
1. ✅ Enhanced tile borders and colors
2. ✅ Better player/enemy sprites (circles with outlines)
3. ✅ HUD with health/XP/level display
4. ✅ Mini-map overlay

**MEDIUM PRIORITY (Polish):**
5. Inventory panel (toggle with I)
6. Skill tree panel (toggle with K)
7. Stack trail visualization
8. Queue turn order display

**LOW PRIORITY (Nice to Have):**
9. Lighting effects / shaders
10. Particle system
11. Smooth movement interpolation
12. Main menu screen

---

## 📋 Asset Recommendations

**Free Asset Sources:**
- **Tileset:** [0x72 Dungeon Tileset](https://0x72.itch.io/dungeontileset-ii)
- **Font:** [Press Start 2P](https://fonts.google.com/specimen/Press+Start+2P) (free pixel font)
- **Sprites:** Use simple SFML shapes first, add sprites later
- **UI:** Pure SFML rectangles, circles, and text

**Optional (If time permits):**
- Load actual tile textures from PNG
- Add sprite sheets for animated characters
- Include background music (using sf::Music)

---

## 🚀 Next Steps

1. Choose priority level (High/Medium/Low)
2. I'll implement the selected features
3. Test visual improvements
4. Iterate based on feedback

**Ready to implement?** Which phase should we start with?
