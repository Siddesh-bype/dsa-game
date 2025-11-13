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
    
public:
    Loot() : tilePos(0, 0), bobOffset(0.0f), bobTimer(0.0f) {}
    
    Loot(const ItemNew& lootItem, int x, int y) 
        : item(lootItem), tilePos(x, y), bobOffset(0.0f), bobTimer(0.0f) {}
    
    Loot(const ItemNew& lootItem, const sf::Vector2i& pos)
        : item(lootItem), tilePos(pos), bobOffset(0.0f), bobTimer(0.0f) {}
    
    // Getters
    const ItemNew& getItem() const { return item; }
    sf::Vector2i getPosition() const { return tilePos; }
    int getX() const { return tilePos.x; }
    int getY() const { return tilePos.y; }
    
    // Update animation
    void update(float deltaTime) {
        bobTimer += deltaTime * 3.0f;  // Bob speed
        bobOffset = std::sin(bobTimer) * 4.0f;  // Bob up/down 4 pixels
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
            sf::CircleShape circle(tileSize * 0.3f);
            circle.setPosition(worldPos + sf::Vector2f(tileSize * 0.2f, tileSize * 0.2f));
            circle.setFillColor(item.getRarityColor());
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(2.0f);
            window.draw(circle);
        }
        
        // Draw glow effect for rare items
        if (item.rarity >= 3) {
            sf::CircleShape glow(tileSize * 0.5f);
            glow.setPosition(worldPos - sf::Vector2f(tileSize * 0.1f, tileSize * 0.1f));
            sf::Color glowColor = item.getRarityColor();
            glowColor.a = 50;  // Semi-transparent
            glow.setFillColor(glowColor);
            window.draw(glow);
        }
    }
    
    // Check if player is adjacent to this loot
    bool isAdjacentTo(int px, int py) const {
        int dx = std::abs(tilePos.x - px);
        int dy = std::abs(tilePos.y - py);
        return (dx <= 1 && dy <= 1);
    }
};
