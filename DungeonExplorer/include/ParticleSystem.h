// ParticleSystem.h - Enhanced visual effects system with combat and environmental particles
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "DataStructures/Heap.h"

struct Particle {
    // Particle types for different rendering
    enum class Type {
        Circle,      // Default circular particle
        Sprite,      // Textured particle (future)
        Slash,       // Slash effect (stretched)
        Spark,       // Sharp spark line
        Trail        // Motion trail
    };
    
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;
    float maxLifetime;
    float size;
    int priority;  // For heap ordering (higher = render first)
    
    // Enhanced properties
    Type type = Type::Circle;
    float rotation = 0.0f;
    float rotationSpeed = 0.0f;
    sf::Vector2f scale = {1.0f, 1.0f};
    bool affectedByGravity = true;
    
    Particle(sf::Vector2f pos, sf::Vector2f vel, sf::Color col, 
             float life, float sz, int prio = 0)
        : position(pos), velocity(vel), color(col), 
          lifetime(life), maxLifetime(life), size(sz), priority(prio) {}
    
    bool operator<(const Particle& other) const {
        return priority < other.priority;  // Max heap
    }
};

class ParticleSystem {
public:
    ParticleSystem() = default;
    
    // Original particle emission
    void emitBloodSplatter(sf::Vector2f position, int count = 10);
    void emitSparkles(sf::Vector2f position, sf::Color color, int count = 5);
    void emitFireEffect(sf::Vector2f position, int count = 15);
    void emitIceEffect(sf::Vector2f position, int count = 12);
    void emitDustCloud(sf::Vector2f position, int count = 8);
    void emitExplosion(sf::Vector2f position, int count = 20);
    
    // NEW: Combat effects
    void emitSlashEffect(sf::Vector2f position, float angle, sf::Color color = sf::Color::White);
    void emitImpactSparks(sf::Vector2f position, int count = 15);
    void emitCriticalHit(sf::Vector2f position);
    
    // NEW: Environmental effects
    void emitAmbientDust(sf::Vector2f position, int count = 3);
    void emitTorchFlame(sf::Vector2f position);
    void emitFootstepDust(sf::Vector2f position);
    
    // NEW: Magic effects
    void emitMagicSparkles(sf::Vector2f position, sf::Color color, int count = 20);
    void emitPoisonCloud(sf::Vector2f position, int count = 25);
    
    // Update and render
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    // Control
    void clear() { particles.clear(); }
    size_t getParticleCount() const { return particles.size(); }
    
private:
    std::vector<Particle> particles;
    
    // ═══════════════════════════════════════════════════════════════════════
    // PARTICLE SYSTEM CONSTANTS
    // ═══════════════════════════════════════════════════════════════════════
    
    // Common constants
    static constexpr float TWO_PI = 6.28f;
    static constexpr float ALPHA_MAX = 255.0f;
    static constexpr float GRAVITY_ACCELERATION = 100.0f;
    
    // Slash effect
    static constexpr float SLASH_WIDTH_MULT = 3.0f;
    static constexpr float SLASH_HEIGHT_MULT = 0.5f;
    
    // Spark effect  
    static constexpr float SPARK_WIDTH_MULT = 2.0f;
    static constexpr float SPARK_HEIGHT_MULT = 0.2f;
    
    void addParticle(const Particle& particle);
    float randomFloat(float min, float max);
    
    // Render helpers
    void renderCircleParticle(sf::RenderWindow& window, const Particle& p);
    void renderSlashParticle(sf::RenderWindow& window, const Particle& p);
    void renderSparkParticle(sf::RenderWindow& window, const Particle& p);
    
    // Emit helper
    void emitRadialParticles(sf::Vector2f position, int count, float speedMin, float speedMax,
        sf::Color color, float lifeMin, float lifeMax, float sizeMin, float sizeMax, 
        int priority, float angleMin = 0.f, float angleMax = TWO_PI, sf::Vector2f velocityOffset = {0.f, 0.f});
};
