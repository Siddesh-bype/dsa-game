// CHANGE: 2025-12-06 - Animation system implementation
// Supports frame-based animation from Aseprite-exported sprite sheets

#include "Animation.h"
#include <iostream>

// Static empty string for when no animation is playing
static const std::string EMPTY_ANIM_NAME = "";

AnimatedSprite::AnimatedSprite()
    : sprite(nullptr), texture(nullptr), frameWidth(16), frameHeight(16), framesPerRow(1),
      currentFrame(0), timer(0.f), playing(false), finished(false),
      currentAnimation(nullptr) {
}

void AnimatedSprite::setTexture(sf::Texture* tex, int fw, int fh) {
    texture = tex;
    frameWidth = fw;
    frameHeight = fh;
    
    if (texture) {
        // SFML 3.x: Create sprite with texture reference
        sprite = std::make_unique<sf::Sprite>(*texture);
        framesPerRow = texture->getSize().x / frameWidth;
        if (framesPerRow < 1) framesPerRow = 1;
        updateTextureRect();
    }
}

void AnimatedSprite::addAnimation(const Animation& anim) {
    animations[anim.name] = anim;
}

void AnimatedSprite::play(const std::string& animName) {
    auto it = animations.find(animName);
    if (it == animations.end()) {
        std::cerr << "[AnimatedSprite] Animation not found: " << animName << std::endl;
        return;
    }
    
    // Don't restart if already playing same animation
    if (currentAnimation && currentAnimation->name == animName && playing) {
        return;
    }
    
    currentAnimation = &it->second;
    currentFrame = currentAnimation->startFrame;
    timer = 0.f;
    playing = true;
    finished = false;
    updateTextureRect();
}

void AnimatedSprite::stop() {
    playing = false;
}

void AnimatedSprite::reset() {
    if (currentAnimation) {
        currentFrame = currentAnimation->startFrame;
    } else {
        currentFrame = 0;
    }
    timer = 0.f;
    finished = false;
    updateTextureRect();
}

void AnimatedSprite::update(float deltaTime) {
    if (!playing || !currentAnimation || finished) return;
    
    timer += deltaTime;
    
    if (timer >= currentAnimation->frameTime) {
        timer -= currentAnimation->frameTime;
        currentFrame++;
        
        int lastFrame = currentAnimation->startFrame + currentAnimation->frameCount - 1;
        
        if (currentFrame > lastFrame) {
            if (currentAnimation->loop) {
                currentFrame = currentAnimation->startFrame;
            } else {
                currentFrame = lastFrame;
                finished = true;
                playing = false;
            }
        }
        
        updateTextureRect();
    }
}

void AnimatedSprite::updateTextureRect() {
    if (!texture || !sprite) return;
    
    // Calculate frame position in sprite sheet (horizontal strip layout)
    int frameX = (currentFrame % framesPerRow) * frameWidth;
    int frameY = (currentFrame / framesPerRow) * frameHeight;
    
    sprite->setTextureRect(sf::IntRect({frameX, frameY}, {frameWidth, frameHeight}));
}

const std::string& AnimatedSprite::getCurrentAnimationName() const {
    if (currentAnimation) {
        return currentAnimation->name;
    }
    return EMPTY_ANIM_NAME;
}
