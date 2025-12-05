// ParticleSystem.cpp - Implementation of particle effects system
// CHANGE: 2025-12-04 - Refactored to use GameUtils::RandomUtils

#include "ParticleSystem.h"
#include "GameUtils.h"

void ParticleSystem::addParticle(const Particle& particle) {
    particles.push_back(particle);
}

// Use RandomUtils::randomFloat from GameUtils.h instead of local function

// Helper: emit radial particles with common pattern
void ParticleSystem::emitRadialParticles(sf::Vector2f position, int count, 
    float speedMin, float speedMax, sf::Color color, float lifeMin, float lifeMax, 
    float sizeMin, float sizeMax, int priority, float angleMin, float angleMax, sf::Vector2f velocityOffset) {
    for (int i = 0; i < count; ++i) {
        float angle = RandomUtils::randomFloat(angleMin, angleMax);
        float speed = RandomUtils::randomFloat(speedMin, speedMax);
        sf::Vector2f velocity(std::cos(angle) * speed + velocityOffset.x, std::sin(angle) * speed + velocityOffset.y);
        Particle p(position, velocity, color, RandomUtils::randomFloat(lifeMin, lifeMax), RandomUtils::randomFloat(sizeMin, sizeMax), priority);
        addParticle(p);
    }
}

void ParticleSystem::emitBloodSplatter(sf::Vector2f position, int count) {
    emitRadialParticles(position, count, 50.0f, 150.0f, sf::Color(180, 0, 0), 0.3f, 0.8f, 2.0f, 5.0f, 1);
}

void ParticleSystem::emitSparkles(sf::Vector2f position, sf::Color color, int count) {
    emitRadialParticles(position, count, 20.0f, 80.0f, color, 0.5f, 1.2f, 3.0f, 6.0f, 3);
}

void ParticleSystem::emitFireEffect(sf::Vector2f position, int count) {
    for (int i = 0; i < count; ++i) {
        float angle = RandomUtils::randomFloat(-0.5f, 0.5f);
        float speed = RandomUtils::randomFloat(30.0f, 100.0f);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed - 50.0f);
        int greenValue = static_cast<int>(RandomUtils::randomFloat(100.0f, 200.0f));
        Particle p(position, velocity, sf::Color(255, greenValue, 0), RandomUtils::randomFloat(0.4f, 1.0f), RandomUtils::randomFloat(4.0f, 8.0f), 2);
        addParticle(p);
    }
}

void ParticleSystem::emitIceEffect(sf::Vector2f position, int count) {
    emitRadialParticles(position, count, 40.0f, 120.0f, sf::Color(100, 200, 255), 0.5f, 1.5f, 3.0f, 7.0f, 2);
}

void ParticleSystem::emitDustCloud(sf::Vector2f position, int count) {
    emitRadialParticles(position, count, 10.0f, 40.0f, sf::Color(150, 150, 150, 100), 0.6f, 1.2f, 5.0f, 10.0f, 0);
}

void ParticleSystem::emitExplosion(sf::Vector2f position, int count) {
    for (int i = 0; i < count; ++i) {
        float angle = RandomUtils::randomFloat(0, TWO_PI);
        float speed = RandomUtils::randomFloat(100.0f, 250.0f);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        int greenValue = static_cast<int>(RandomUtils::randomFloat(150.0f, 255.0f));
        Particle p(position, velocity, sf::Color(255, greenValue, 0), RandomUtils::randomFloat(0.3f, 0.8f), RandomUtils::randomFloat(6.0f, 12.0f), 4);
        addParticle(p);
    }
}

void ParticleSystem::update(float deltaTime) {
    // Update particles and remove dead ones
    for (auto it = particles.begin(); it != particles.end();) {
        it->lifetime -= deltaTime;
        if (it->lifetime <= 0) {
            it = particles.erase(it);
        } else {
            it->position += it->velocity * deltaTime;
            
            // Apply gravity if enabled
            if (it->affectedByGravity) {
                it->velocity.y += 100.0f * deltaTime;
            }
            
            // Update rotation
            it->rotation += it->rotationSpeed * deltaTime;
            
            ++it;
        }
    }
}

void ParticleSystem::render(sf::RenderWindow& window) {
    // Render particles based on type
    for (const auto& p : particles) {
        switch (p.type) {
            case Particle::Type::Circle:
                renderCircleParticle(window, p);
                break;
            case Particle::Type::Slash:
                renderSlashParticle(window, p);
                break;
            case Particle::Type::Spark:
                renderSparkParticle(window, p);
                break;
            default:
                renderCircleParticle(window, p);
                break;
        }
    }
}

void ParticleSystem::renderCircleParticle(sf::RenderWindow& window, const Particle& p) {
    float alpha = (p.lifetime / p.maxLifetime) * 255.0f;
    sf::Color color = p.color;
    color.a = static_cast<sf::Uint8>(alpha);
    
    sf::CircleShape circle(p.size);
    circle.setPosition(p.position);
    circle.setFillColor(color);
    circle.setOrigin(p.size, p.size);
    circle.setScale(p.scale);
    circle.setRotation(sf::degrees(p.rotation));
    
    window.draw(circle);
}

void ParticleSystem::renderSlashParticle(sf::RenderWindow& window, const Particle& p) {
    float alpha = (p.lifetime / p.maxLifetime) * 255.0f;
    sf::Color color = p.color;
    color.a = static_cast<sf::Uint8>(alpha);
    
    // Slash is a stretched rectangle
    sf::RectangleShape slash(sf::Vector2f(p.size * 3.0f, p.size * 0.5f));
    slash.setPosition(p.position);
    slash.setFillColor(color);
    slash.setOrigin(p.size * 1.5f, p.size * 0.25f);
    slash.setScale(p.scale);
    slash.setRotation(sf::degrees(p.rotation));
    
    window.draw(slash);
}

void ParticleSystem::renderSparkParticle(sf::RenderWindow& window, const Particle& p) {
    float alpha = (p.lifetime / p.maxLifetime) * 255.0f;
    sf::Color color = p.color;
    color.a = static_cast<sf::Uint8>(alpha);
    
    // Spark is a thin line
    sf::RectangleShape spark(sf::Vector2f(p.size * 2.0f, p.size * 0.2f));
    spark.setPosition(p.position);
    spark.setFillColor(color);
    spark.setOrigin(p.size, p.size * 0.1f);
    spark.setScale(p.scale);
    spark.setRotation(sf::degrees(p.rotation));
    
    window.draw(spark);
}

// ========== COMBAT EFFECTS ==========

void ParticleSystem::emitSlashEffect(sf::Vector2f position, float angle, sf::Color color) {
    // Create arc of slash particles
    for (int i = 0; i < 8; ++i) {
        float offsetAngle = angle + RandomUtils::randomFloat(-0.3f, 0.3f);
        float distance = RandomUtils::randomFloat(10.0f, 30.0f);
        sf::Vector2f offset(std::cos(offsetAngle) * distance, std::sin(offsetAngle) * distance);
        
        float speed = RandomUtils::randomFloat(100.0f, 200.0f);
        sf::Vector2f velocity(std::cos(offsetAngle) * speed, std::sin(offsetAngle) * speed);
        
        Particle p(position + offset, velocity, color, 
                   RandomUtils::randomFloat(0.2f, 0.4f), RandomUtils::randomFloat(8.0f, 12.0f), 5);
        p.type = Particle::Type::Slash;
        p.rotation = offsetAngle;
        p.affectedByGravity = false;
        addParticle(p);
    }
}

void ParticleSystem::emitImpactSparks(sf::Vector2f position, int count) {
    for (int i = 0; i < count; ++i) {
        float angle = RandomUtils::randomFloat(0, 6.28f);
        float speed = RandomUtils::randomFloat(150.0f, 300.0f);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        
        // Yellow-orange sparks
        int greenValue = static_cast<int>(RandomUtils::randomFloat(150.0f, 200.0f));
        sf::Color sparkColor(255, greenValue, 0);
        
        Particle p(position, velocity, sparkColor, 
                   RandomUtils::randomFloat(0.15f, 0.35f), RandomUtils::randomFloat(4.0f, 8.0f), 6);
        p.type = Particle::Type::Spark;
        p.rotation = angle;
        p.affectedByGravity = true;
        addParticle(p);
    }
}

void ParticleSystem::emitCriticalHit(sf::Vector2f position) {
    // Large burst with ring expansion
    for (int i = 0; i < 30; ++i) {
        float angle = (i / 30.0f) * 6.28f;
        float speed = RandomUtils::randomFloat(200.0f, 350.0f);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        
        // Bright red-yellow
        sf::Color critColor(255, RandomUtils::randomFloat(100.0f, 200.0f), 0);
        
        Particle p(position, velocity, critColor, 
                   RandomUtils::randomFloat(0.4f, 0.7f), RandomUtils::randomFloat(6.0f, 10.0f), 7);
        p.type = Particle::Type::Spark;
        p.rotation = angle;
        p.affectedByGravity = false;
        addParticle(p);
    }
}

// ========== ENVIRONMENTAL EFFECTS ==========

void ParticleSystem::emitAmbientDust(sf::Vector2f position, int count) {
    for (int i = 0; i < count; ++i) {
        float angle = RandomUtils::randomFloat(0, 6.28f);
        float speed = RandomUtils::randomFloat(5.0f, 15.0f);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed - 10.0f);
        
        // Low opacity dust
        sf::Color dustColor(150, 150, 150, RandomUtils::randomFloat(30.0f, 60.0f));
        
        Particle p(position, velocity, dustColor, 
                   RandomUtils::randomFloat(2.0f, 4.0f), RandomUtils::randomFloat(3.0f, 6.0f), 0);
        p.affectedByGravity = false;
        addParticle(p);
    }
}

void ParticleSystem::emitTorchFlame(sf::Vector2f position) {
    // Continuous flame effect - call every frame
    for (int i = 0; i < 2; ++i) {
        float angle = RandomUtils::randomFloat(-0.3f, 0.3f);
        float speed = RandomUtils::randomFloat(20.0f, 50.0f);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed - 60.0f);
        
        // Flickering fire colors
        int greenValue = static_cast<int>(RandomUtils::randomFloat(100.0f, 200.0f));
        int blueValue = static_cast<int>(RandomUtils::randomFloat(0.0f, 50.0f));
        sf::Color flameColor(255, greenValue, blueValue);
        
        Particle p(position, velocity, flameColor, 
                   RandomUtils::randomFloat(0.3f, 0.6f), RandomUtils::randomFloat(4.0f, 7.0f), 2);
        p.affectedByGravity = false;
        addParticle(p);
    }
}

void ParticleSystem::emitFootstepDust(sf::Vector2f position) {
    for (int i = 0; i < 4; ++i) {
        float angle = RandomUtils::randomFloat(0, 6.28f);
        float speed = RandomUtils::randomFloat(10.0f, 30.0f);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        
        sf::Color dustColor(120, 120, 120, 80);
        
        Particle p(position, velocity, dustColor, 
                   RandomUtils::randomFloat(0.3f, 0.6f), RandomUtils::randomFloat(3.0f, 5.0f), 0);
        p.affectedByGravity = false;
        addParticle(p);
    }
}

// ========== MAGIC EFFECTS ==========

void ParticleSystem::emitMagicSparkles(sf::Vector2f position, sf::Color color, int count) {
    for (int i = 0; i < count; ++i) {
        float angle = RandomUtils::randomFloat(0, 6.28f);
        float speed = RandomUtils::randomFloat(30.0f, 80.0f);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed - 40.0f);
        
        Particle p(position, velocity, color, 
                   RandomUtils::randomFloat(0.8f, 1.5f), RandomUtils::randomFloat(4.0f, 8.0f), 8);
        p.affectedByGravity = false;
        p.rotationSpeed = RandomUtils::randomFloat(-3.0f, 3.0f);
        addParticle(p);
    }
}

void ParticleSystem::emitPoisonCloud(sf::Vector2f position, int count) {
    for (int i = 0; i < count; ++i) {
        float angle = RandomUtils::randomFloat(0, 6.28f);
        float speed = RandomUtils::randomFloat(15.0f, 40.0f);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        
        // Green poison color
        sf::Color poisonColor(50, RandomUtils::randomFloat(180.0f, 255.0f), 50, 150);
        
        Particle p(position, velocity, poisonColor, 
                   RandomUtils::randomFloat(2.0f, 3.5f), RandomUtils::randomFloat(6.0f, 12.0f), 1);
        p.affectedByGravity = false;
        addParticle(p);
    }
}
