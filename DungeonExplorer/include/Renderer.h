#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class Player;
class Dungeon;
class EnemyManager;

class Renderer {
private:
    sf::RenderWindow* window;
    float tileSize;
    
    sf::View gameView;
    sf::View uiView;
    
    // Lighting system
    std::unique_ptr<sf::Shader> lightingShader;
    std::unique_ptr<sf::RenderTexture> renderTexture;
    bool lightingEnabled;
    float lightRadius;
    float ambientLevel;

public:
    Renderer(sf::RenderWindow* window, float tileSize = 32.0f);
    
    void setTileSize(float size) { tileSize = size; }
    float getTileSize() const { return tileSize; }
    
    void begin();
    void end();
    
    void renderDungeon(const Dungeon& dungeon, int currentFloor = 1);
    void renderPlayer(const Player& player);
    void renderEnemies(const EnemyManager& enemyManager);
    
    void setGameView();
    void setUIView();
    
    sf::Vector2f worldToScreen(float x, float y) const;
    sf::Vector2f screenToWorld(float x, float y) const;
    
    // Lighting controls
    void toggleLighting();
    bool isLightingEnabled() const { return lightingEnabled; }
    void setLightRadius(float radius) { lightRadius = radius; }
    void setAmbientLevel(float level) { ambientLevel = level; }
    void applyLighting(const Player& player);
    bool loadLightingShader();
};
