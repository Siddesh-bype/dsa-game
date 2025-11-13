#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Player.h"
#include "Dungeon.h"

class DSAVisualizer {
private:
    const sf::Font* font;
    
public:
    DSAVisualizer();
    
    void setFont(const sf::Font* fontPtr);
    
    // Stack visualization - player path trail
    void renderStackTrail(sf::RenderWindow& window, const Player& player, float tileSize);
    
    // Graph visualization - room connections
    void renderGraphPaths(sf::RenderWindow& window, const Dungeon& dungeon, float tileSize);
    
    // Heap visualization - loot sparkles
    void renderHeapGlow(sf::RenderWindow& window, sf::Vector2f position, int value);
    
    // Loot popup animation
    void renderLootPopup(sf::RenderWindow& window, const std::string& itemName, 
                        int value, float x, float y, float time);
};
