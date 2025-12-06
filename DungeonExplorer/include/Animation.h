#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <memory>

// ═══════════════════════════════════════════════════════════════════════
// ANIMATION SYSTEM - Frame-based sprite animation from sprite sheets
// Supports Aseprite-exported horizontal sprite strips
// ═══════════════════════════════════════════════════════════════════════

struct Animation {
    std::string name;       // Animation identifier (e.g., "idle", "walk", "attack")
    int startFrame;         // First frame index in sprite sheet
    int frameCount;         // Total frames in this animation
    float frameTime;        // Seconds per frame (e.g., 0.1f = 10 FPS)
    bool loop;              // Should animation loop when finished?
    
    Animation() : startFrame(0), frameCount(1), frameTime(0.1f), loop(true) {}
    
    Animation(const std::string& n, int start, int count, float time, bool l = true)
        : name(n), startFrame(start), frameCount(count), frameTime(time), loop(l) {}
};

class AnimatedSprite {
private:
    std::unique_ptr<sf::Sprite> sprite;  // SFML 3.x requires texture at construction
    sf::Texture* texture;
    
    int frameWidth;         // Width of single frame in pixels
    int frameHeight;        // Height of single frame in pixels
    int framesPerRow;       // Frames per row in sprite sheet
    
    int currentFrame;       // Current frame index
    float timer;            // Time accumulator
    bool playing;           // Is animation playing?
    bool finished;          // Has non-looping animation finished?
    
    std::unordered_map<std::string, Animation> animations;
    Animation* currentAnimation;
    
    void updateTextureRect();
    
public:
    AnimatedSprite();
    
    // Setup
    void setTexture(sf::Texture* tex, int fw, int fh);
    void addAnimation(const Animation& anim);
    
    // Control
    void play(const std::string& animName);
    void stop();
    void reset();
    
    // Update (call each frame with deltaTime)
    void update(float deltaTime);
    
    // Rendering
    sf::Sprite* getSprite() { return sprite.get(); }
    const sf::Sprite* getSprite() const { return sprite.get(); }
    bool hasSprite() const { return sprite != nullptr; }
    
    // State
    bool isPlaying() const { return playing; }
    bool isFinished() const { return finished; }
    int getCurrentFrame() const { return currentFrame; }
    const std::string& getCurrentAnimationName() const;
    
    // Transform shortcuts (check hasSprite() before calling)
    void setPosition(float x, float y) { if (sprite) sprite->setPosition({x, y}); }
    void setPosition(sf::Vector2f pos) { if (sprite) sprite->setPosition(pos); }
    void setScale(float x, float y) { if (sprite) sprite->setScale({x, y}); }
    void setOrigin(float x, float y) { if (sprite) sprite->setOrigin({x, y}); }
    void setColor(sf::Color color) { if (sprite) sprite->setColor(color); }
};
