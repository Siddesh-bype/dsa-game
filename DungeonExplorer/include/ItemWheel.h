#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>
#include "ItemNew.h"

// ═══════════════════════════════════════════════════════════════════════
// ITEM WHEEL UI - Radial selection menu for items
// Opens with Tab key, select with mouse or arrow keys
// ═══════════════════════════════════════════════════════════════════════

class ItemWheel {
public:
    // Configuration
    static constexpr float WHEEL_RADIUS = 120.f;
    static constexpr float ICON_SIZE = 40.f;
    static constexpr float CENTER_RADIUS = 30.f;
    static constexpr float SEGMENT_PADDING = 5.f;
    static constexpr int MAX_ITEMS = 8;
    
private:
    bool visible = false;
    int selectedIndex = -1;
    int hoveredIndex = -1;
    sf::Vector2f centerPosition;
    std::vector<const ItemNew*> items;
    sf::Font* font = nullptr;
    
    // Animation
    float openProgress = 0.f;  // 0 = closed, 1 = fully open
    static constexpr float OPEN_SPEED = 8.f;
    
public:
    ItemWheel() = default;
    
    void setFont(sf::Font* f) { font = f; }
    void setCenter(sf::Vector2f pos) { centerPosition = pos; }
    
    // Control
    void open(sf::Vector2f screenCenter);
    void close();
    bool isOpen() const { return visible; }
    
    // Update items from inventory
    void updateItems(const std::vector<const ItemNew*>& inventoryItems);
    
    // Input
    void handleMouseMove(sf::Vector2f mousePos);
    int handleClick();  // Returns selected item index, or -1 if none
    void handleKeyboard(int direction);  // -1 = left/up, 1 = right/down
    
    // Animation update
    void update(float deltaTime);
    
    // Rendering
    void render(sf::RenderWindow& window);
    
    int getSelectedIndex() const { return selectedIndex; }
    int getHoveredIndex() const { return hoveredIndex; }
};
