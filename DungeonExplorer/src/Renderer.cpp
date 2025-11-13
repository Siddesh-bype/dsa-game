#include "Renderer.h"
#include "Player.h"
#include "Dungeon.h"
#include "Enemy.h"
#include <iostream>
#include <cmath>

Renderer::Renderer(sf::RenderWindow* window, float tileSize)
    : window(window), tileSize(tileSize), 
      lightingEnabled(false),  // TEMPORARILY DISABLED to test if lighting is causing black areas
      lightRadius(500.0f),  // Much larger radius to cover 800px width
      ambientLevel(0.50f) {  // 50% ambient - dark areas clearly visible
    
    gameView = window->getDefaultView();
    uiView = window->getDefaultView();
    
    // Initialize render texture for lighting
    renderTexture = std::make_unique<sf::RenderTexture>();
    auto windowSize = window->getSize();
    if (!renderTexture->resize(windowSize)) {
        std::cerr << "[Renderer] Failed to create render texture!" << std::endl;
    }
    
    // Load lighting shader
    loadLightingShader();
}

void Renderer::begin() {
    window->clear(sf::Color::Black);
}

void Renderer::end() {
    window->display();
}

void Renderer::renderDungeon(const Dungeon& dungeon, int currentFloor) {
    setGameView();
    dungeon.render(*window, tileSize, currentFloor);
}

void Renderer::renderPlayer(const Player& player) {
    setGameView();
    player.render(*window, tileSize);
}

void Renderer::renderEnemies(const EnemyManager& enemyManager) {
    setGameView();
    enemyManager.render(*window, tileSize);
}

void Renderer::setGameView() {
    window->setView(gameView);
}

void Renderer::setUIView() {
    window->setView(uiView);
}

sf::Vector2f Renderer::worldToScreen(float x, float y) const {
    return sf::Vector2f(x * tileSize, y * tileSize);
}

sf::Vector2f Renderer::screenToWorld(float x, float y) const {
    return sf::Vector2f(x / tileSize, y / tileSize);
}

bool Renderer::loadLightingShader() {
    lightingShader = std::make_unique<sf::Shader>();
    
    if (!sf::Shader::isAvailable()) {
        std::cerr << "[Renderer] Shaders not available on this system!" << std::endl;
        lightingEnabled = false;
        return false;
    }
    
    // Try to load shader files
    if (!lightingShader->loadFromFile("assets/shaders/lighting.vert", "assets/shaders/lighting.frag")) {
        std::cerr << "[Renderer] Failed to load lighting shader!" << std::endl;
        lightingEnabled = false;
        return false;
    }
    
    std::cout << "[Renderer] Lighting shader loaded successfully!" << std::endl;
    return true;
}

void Renderer::toggleLighting() {
    if (lightingShader) {
        lightingEnabled = !lightingEnabled;
        std::cout << "[Renderer] Lighting " << (lightingEnabled ? "ENABLED" : "DISABLED") << std::endl;
    }
}

void Renderer::applyLighting(const Player& player) {
    if (!lightingEnabled) {
        return;
    }
    
    // Get player position in screen coordinates
    Position playerPos = player.getPosition();
    sf::Vector2f screenPos = worldToScreen(static_cast<float>(playerPos.x), static_cast<float>(playerPos.y));
    
    // Center the light on the player sprite
    screenPos.x += tileSize / 2.0f;
    screenPos.y += tileSize / 2.0f;
    
    // If shader is available, use shader-based lighting
    if (lightingShader) {
        // Set shader uniforms
        lightingShader->setUniform("playerPosition", screenPos);
        lightingShader->setUniform("lightRadius", lightRadius);
        lightingShader->setUniform("ambientLevel", ambientLevel);
        
        // Create fullscreen quad to apply shader
        sf::RectangleShape overlay(sf::Vector2f(window->getSize()));
        overlay.setPosition(sf::Vector2f(0.f, 0.f));
        overlay.setFillColor(sf::Color::White);
        
        // Draw with shader
        window->draw(overlay, lightingShader.get());
    } else {
        // Fallback: Enhanced radial gradient with softer falloff
        const int segments = 96;  // More segments for smoother gradient
        const float outerRadius = lightRadius * 1.3f;  // Extended radius for softer edge
        
        // Create dark overlay for entire screen
        sf::RectangleShape darkOverlay(sf::Vector2f(window->getSize()));
        darkOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<uint8_t>(255 * (1.0f - ambientLevel))));
        window->draw(darkOverlay, sf::BlendMultiply);
        
        // Create multi-layer light for ultra-soft falloff
        // Layer 1: Bright inner circle
        sf::VertexArray innerLight(sf::PrimitiveType::TriangleFan, segments + 2);
        innerLight[0].position = screenPos;
        innerLight[0].color = sf::Color(255, 245, 220, 0);  // Warm white center
        
        float innerRadius = lightRadius * 0.5f;
        for (int i = 0; i <= segments; ++i) {
            float angle = (i / static_cast<float>(segments)) * 2.0f * 3.14159f;
            float x = screenPos.x + std::cos(angle) * innerRadius;
            float y = screenPos.y + std::sin(angle) * innerRadius;
            
            innerLight[i + 1].position = sf::Vector2f(x, y);
            innerLight[i + 1].color = sf::Color(255, 240, 200, 30);  // Soft edge
        }
        window->draw(innerLight, sf::BlendAdd);
        
        // Layer 2: Extended outer glow
        sf::VertexArray outerGlow(sf::PrimitiveType::TriangleFan, segments + 2);
        outerGlow[0].position = screenPos;
        outerGlow[0].color = sf::Color(255, 240, 200, 15);  // Dim center
        
        for (int i = 0; i <= segments; ++i) {
            float angle = (i / static_cast<float>(segments)) * 2.0f * 3.14159f;
            float x = screenPos.x + std::cos(angle) * outerRadius;
            float y = screenPos.y + std::sin(angle) * outerRadius;
            
            outerGlow[i + 1].position = sf::Vector2f(x, y);
            outerGlow[i + 1].color = sf::Color(0, 0, 0, static_cast<uint8_t>(255 * (1.0f - ambientLevel)));
        }
        window->draw(outerGlow, sf::BlendAdd);
    }
}
