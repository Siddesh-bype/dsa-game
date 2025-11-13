#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <memory>

enum class AssetPack {
    TinyDungeon,   // Default colorful pack
    OneBitPack     // Monochrome retro pack
};

class AssetManager {
public:
    // Singleton access
    static AssetManager& getInstance();
    
    // Load all assets from manifest
    bool loadFromManifest(const std::string& jsonPath);
    
    // Get texture by key
    sf::Texture* getTexture(const std::string& key);
    
    // Check if texture exists
    bool hasTexture(const std::string& key) const;
    
    // Load individual texture
    bool loadTexture(const std::string& key, const std::string& filePath);
    
    // Asset pack management
    AssetPack getCurrentPack() const { return currentPack; }
    void switchPack(AssetPack pack);
    void togglePack();  // Toggle between TinyDungeon and OneBitPack
    
    // Clear all assets
    void clear();
    
    // Get spritesheet for animations
    sf::Texture* getSpritesheet(const std::string& sheetName);
    // Create an sf::Sprite from a spritesheet by tile index
    // tileIndex: sequential tile number (0-based)
    // columns, tileSize, spacing are used to compute the texture rect
    // Returns true if sprite was created and assigned to outSprite. Returns false if sheet not found.
    bool createSpriteFromSheet(const std::string& sheetName, int tileIndex, sf::Sprite& outSprite, int columns = 12, int tileSize = 16, int spacing = 1);
    
private:
    AssetManager() = default;
    ~AssetManager() = default;
    
    // Prevent copying
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    
    void loadTinyDungeonPack();
    void loadOneBitPack();
    
    AssetPack currentPack = AssetPack::TinyDungeon;
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> spritesheets;
};
