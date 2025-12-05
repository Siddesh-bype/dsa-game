#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Player;
class Dungeon;
class EnemyManager;

class Renderer {
public:
    struct Light {
        sf::Vector2f position;
        sf::Color color;
        float radius;
        float intensity;
        
        // Flickering properties
        float flickerSpeed = 0.0f;     // 0 = no flicker
        float flickerRange = 0.0f;     // Intensity variance
        float baseIntensity = 1.0f;    // Original intensity
        float flickerOffset = 0.0f;    // Random offset
        
        Light(sf::Vector2f pos, sf::Color col, float r, float i)
            : position(pos), color(col), radius(r), intensity(i), baseIntensity(i) {}
    };

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
    void setAmbientLevel(float level) { ambientLevel = level; }
    
    // Multi-light management
    void addLight(const Light& light);
    void clearLights();
    void updateLights(float totalTime);
    
    void applyLighting(const Player& player);
    bool loadLightingShader();
    
    // NEW: Outline rendering
    void setOutlineEnabled(bool enabled) { outlineEnabled = enabled; }
    bool isOutlineEnabled() const { return outlineEnabled; }
    void setOutlineColor(sf::Color color) { outlineColor = color; }
    void setOutlineThickness(float thickness) { outlineThickness = thickness; }
    
    // NEW: Flash effect
    void applyFlashEffect(sf::Sprite& sprite, sf::Color flashColor, float intensity);
    
    // NEW: Draw sprite with outline
    void drawSpriteWithOutline(const sf::Sprite& sprite, sf::Color outlineColor);
    
    // NEW: Draw sprite with shadow
    void drawSpriteWithShadow(const sf::Sprite& sprite, 
                              sf::Vector2f shadowOffset = {2.0f, 2.0f},
                              sf::Color shadowColor = sf::Color(0, 0, 0, 128));

private:
    sf::RenderWindow* window;
    float tileSize;
    
    sf::View gameView;
    sf::View uiView;
    
    std::unique_ptr<sf::Shader> lightingShader;
    std::unique_ptr<sf::RenderTexture> renderTexture;
    
    bool lightingEnabled;
    float lightRadius;
    float ambientLevel;
    
    std::vector<Light> lights;
    
    // NEW: Outline shader
    std::unique_ptr<sf::Shader> outlineShader;
    bool outlineEnabled = true;
    sf::Color outlineColor = sf::Color::White;
    float outlineThickness = 1.5f;
    
    // NEW: Flash shader
    std::unique_ptr<sf::Shader> flashShader;
    
    // Shader loading
    bool loadOutlineShader();
    bool loadFlashShader();
    
    // ═══════════════════════════════════════════════════════════════════════
    // RENDERER CONFIGURATION CONSTANTS
    // ═══════════════════════════════════════════════════════════════════════
    
    // Default lighting
    static constexpr float DEFAULT_LIGHT_RADIUS = 500.0f;
    static constexpr float DEFAULT_AMBIENT_LEVEL = 0.50f;
    
    // Lighting gradient
    static constexpr int GRADIENT_SEGMENTS = 96;
    static constexpr float OUTER_RADIUS_MULT = 1.3f;
    static constexpr float INNER_RADIUS_MULT = 0.5f;
    
    // Light colors
    static constexpr int WARM_WHITE_R = 255, WARM_WHITE_G = 245, WARM_WHITE_B = 220;
    static constexpr int WARM_GLOW_R = 255, WARM_GLOW_G = 240, WARM_GLOW_B = 200;
    static constexpr int INNER_EDGE_ALPHA = 30;
    static constexpr int DIM_CENTER_ALPHA = 15;
    
    // Math constants
    static constexpr float TWO_PI = 6.28318f;
    
    // Flicker calculation
    static constexpr float FLICKER_SEED_X = 0.1f;
    static constexpr float FLICKER_SEED_Y = 0.2f;
    static constexpr float FLICKER_FREQ_2 = 2.7f;
    static constexpr float FLICKER_FREQ_3 = 5.1f;
    static constexpr float FLICKER_AMP_1 = 0.5f;
    static constexpr float FLICKER_AMP_2 = 0.25f;
    static constexpr float FLICKER_AMP_3 = 0.125f;
    static constexpr float FLICKER_OFFSET = 0.875f;
    static constexpr float FLICKER_DIVISOR = 1.75f;
    
    // Limits
    static constexpr size_t MAX_LIGHTS = 32;
};
