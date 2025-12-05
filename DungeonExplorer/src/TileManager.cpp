#include "TileManager.h"
#include "AssetManager.h"
#include <iostream>
#include <filesystem>

TileManager& TileManager::getInstance() {
    static TileManager instance;
    return instance;
}

bool TileManager::loadTileset(const std::string& tsxPath) {
    currentTileset = TilesetReader::parseTSX(tsxPath);
    
    if (currentTileset.name.empty()) {
        std::cerr << "[TileManager] Failed to parse TSX: " << tsxPath << std::endl;
        return false;
    }
    
    // Resolve image path relative to TSX
    std::filesystem::path tsxDir = std::filesystem::path(tsxPath).parent_path();
    std::filesystem::path imagePath = tsxDir / currentTileset.imageSource;
    
    // Load texture via AssetManager
    std::string textureKey = currentTileset.name + "_texture";
    if (AssetManager::getInstance().loadTexture(textureKey, imagePath.string())) {
        currentTexture = AssetManager::getInstance().getTexture(textureKey);
        std::cout << "[TileManager] Loaded tileset '" << currentTileset.name << "' with texture " << imagePath.string() << std::endl;
        return true;
    }
    
    std::cerr << "[TileManager] Failed to load texture: " << imagePath.string() << std::endl;
    return false;
}

sf::IntRect TileManager::getTileRect(int tileId) const {
    if (currentTileset.columns == 0) return sf::IntRect();
    
    int tx = tileId % currentTileset.columns;
    int ty = tileId / currentTileset.columns;
    
    int x = currentTileset.margin + tx * (currentTileset.tileWidth + currentTileset.spacing);
    int y = currentTileset.margin + ty * (currentTileset.tileHeight + currentTileset.spacing);
    
    return sf::IntRect({x, y}, {currentTileset.tileWidth, currentTileset.tileHeight});
}

sf::Sprite TileManager::getTileSprite(int tileId) {
    if (!currentTexture) {
        static sf::Texture dummyTexture;
        return sf::Sprite(dummyTexture);
    }
    
    sf::Sprite sprite(*currentTexture);
    sprite.setTextureRect(getTileRect(tileId));
    return sprite;
}
