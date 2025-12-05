// CHANGE: 2025-11-10 - Loot entity for items on the ground
// Represents an item that can be picked up by the player

#pragma once
#include "ItemNew.h"
#include <SFML/Graphics.hpp>

// In-game loot entity sitting on a tile
class Loot {
private:
    ItemNew item;           // The item this loot represents
    sf::Vector2i tilePos;   // Position on the dungeon grid
    float bobOffset;        // For visual bobbing animation
    float bobTimer;
    
    // ═══════════════════════════════════════════════════════════════════════
    // LOOT CONFIGURATION CONSTANTS
    // ═══════════════════════════════════════════════════════════════════════
    
    // Animation settings
    static constexpr float BOB_SPEED = 3.0f;
    static constexpr float BOB_AMPLITUDE = 4.0f;  // Pixels
    static constexpr float INITIAL_BOB_OFFSET = 0.0f;
    static constexpr float INITIAL_BOB_TIMER = 0.0f;
    
    // Rendering settings
    static constexpr float FALLBACK_CIRCLE_SIZE = 0.3f;  // Multiplier of tile size
    static constexpr float FALLBACK_CIRCLE_OFFSET = 0.2f;
    static constexpr float OUTLINE_THICKNESS = 2.0f;
    
    // Glow effect settings
    static constexpr int RARE_ITEM_THRESHOLD = 3;  // Rarity >= 3 gets glow
    static constexpr float GLOW_SIZE = 0.5f;  // Multiplier of tile size
    static constexpr float GLOW_OFFSET = 0.1f;
    static constexpr int GLOW_ALPHA = 50;  // Semi-transparent
    
    // Pickup range
    static constexpr int PICKUP_RANGE = 1;  // Adjacent tiles
    
public:
    Loot() : tilePos(0, 0), bobOffset(INITIAL_BOB_OFFSET), bobTimer(INITIAL_BOB_TIMER) {}
    
    Loot(const ItemNew& lootItem, int x, int y) 
        : item(lootItem), tilePos(x, y), bobOffset(INITIAL_BOB_OFFSET), bobTimer(INITIAL_BOB_TIMER) {}
    
    Loot(const ItemNew& lootItem, const sf::Vector2i& pos)
        : item(lootItem), tilePos(pos), bobOffset(INITIAL_BOB_OFFSET), bobTimer(INITIAL_BOB_TIMER) {}
    
    // Getters
    const ItemNew& getItem() const { return item; }
    sf::Vector2i getPosition() const { return tilePos; }
    int getX() const { return tilePos.x; }
    int getY() const { return tilePos.y; }
    
    // Update animation
    void update(float deltaTime) {
        bobTimer += deltaTime * BOB_SPEED;
        bobOffset = std::sin(bobTimer) * BOB_AMPLITUDE;
    }
    
    // Render the loot on the ground
    void render(sf::RenderWindow& window, float tileSize, const sf::Texture* iconTexture = nullptr) const {
        sf::Vector2f worldPos(tilePos.x * tileSize, tilePos.y * tileSize);
        worldPos.y += bobOffset;  // Apply bobbing effect
        
        if (iconTexture) {
            // Draw item icon
            sf::Sprite sprite(*iconTexture);
            sprite.setPosition(worldPos);
            
            // Scale to fit tile (icons might be different sizes)
            sf::Vector2u texSize = iconTexture->getSize();
            float scale = tileSize / std::max(texSize.x, texSize.y);
            sprite.setScale(sf::Vector2f(scale, scale));
            
            window.draw(sprite);
        } else {
            // Fallback: draw colored circle based on rarity
            sf::CircleShape circle(tileSize * FALLBACK_CIRCLE_SIZE);
            circle.setPosition(worldPos + sf::Vector2f(tileSize * FALLBACK_CIRCLE_OFFSET, tileSize * FALLBACK_CIRCLE_OFFSET));
            circle.setFillColor(item.getRarityColor());
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(OUTLINE_THICKNESS);
            window.draw(circle);
        }
        
        // Draw glow effect for rare items
        if (item.rarity >= RARE_ITEM_THRESHOLD) {
            sf::CircleShape glow(tileSize * GLOW_SIZE);
            glow.setPosition(worldPos - sf::Vector2f(tileSize * GLOW_OFFSET, tileSize * GLOW_OFFSET));
            sf::Color glowColor = item.getRarityColor();
            glowColor.a = GLOW_ALPHA;  // Semi-transparent
            glow.setFillColor(glowColor);
            window.draw(glow);
        }
    }
    
    // Check if player is adjacent to this loot
    bool isAdjacentTo(int px, int py) const {
        int dx = std::abs(tilePos.x - px);
        int dy = std::abs(tilePos.y - py);
        return (dx <= PICKUP_RANGE && dy <= PICKUP_RANGE);
    }
};
