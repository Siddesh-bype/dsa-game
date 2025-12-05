#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include "TilesetReader.h"

class TileManager {
public:
    static TileManager& getInstance();
    
    bool loadTileset(const std::string& tsxPath);
    const TilesetData& getTilesetData() const { return currentTileset; }
    
    sf::IntRect getTileRect(int tileId) const;
    sf::Sprite getTileSprite(int tileId);
    
private:
    TileManager() = default;
    TilesetData currentTileset;
    sf::Texture* currentTexture = nullptr;
};
