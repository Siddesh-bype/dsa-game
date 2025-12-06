// CHANGE: 2025-12-06 - Item Wheel UI implementation
// Radial selection menu for quick item access

#include "ItemWheel.h"
#include "AssetManager.h"
#include <cmath>
#include <iostream>

static constexpr float PI = 3.14159265359f;

void ItemWheel::open(sf::Vector2f screenCenter) {
    visible = true;
    centerPosition = screenCenter;
    hoveredIndex = 0;
    openProgress = 0.f;
}

void ItemWheel::close() {
    visible = false;
    selectedIndex = -1;
    hoveredIndex = -1;
}

void ItemWheel::updateItems(const std::vector<const ItemNew*>& inventoryItems) {
    items.clear();
    for (size_t i = 0; i < inventoryItems.size() && i < MAX_ITEMS; i++) {
        items.push_back(inventoryItems[i]);
    }
}

void ItemWheel::handleMouseMove(sf::Vector2f mousePos) {
    if (!visible || items.empty()) return;
    
    // Calculate angle from center to mouse
    sf::Vector2f delta = mousePos - centerPosition;
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    
    if (distance < CENTER_RADIUS) {
        hoveredIndex = -1;  // Center = cancel
        return;
    }
    
    // Calculate angle (0 = right, counterclockwise)
    float angle = std::atan2(delta.y, delta.x);
    if (angle < 0) angle += 2.f * PI;
    
    // Convert angle to item index
    float segmentAngle = 2.f * PI / items.size();
    // Offset to center first item at top
    float offsetAngle = angle + (segmentAngle / 2.f) + (PI / 2.f);
    if (offsetAngle > 2.f * PI) offsetAngle -= 2.f * PI;
    
    hoveredIndex = static_cast<int>(offsetAngle / segmentAngle) % items.size();
}

int ItemWheel::handleClick() {
    if (!visible) return -1;
    
    if (hoveredIndex >= 0 && hoveredIndex < static_cast<int>(items.size())) {
        selectedIndex = hoveredIndex;
        return selectedIndex;
    }
    return -1;
}

void ItemWheel::handleKeyboard(int direction) {
    if (!visible || items.empty()) return;
    
    if (hoveredIndex < 0) hoveredIndex = 0;
    
    hoveredIndex += direction;
    if (hoveredIndex < 0) hoveredIndex = items.size() - 1;
    if (hoveredIndex >= static_cast<int>(items.size())) hoveredIndex = 0;
}

void ItemWheel::update(float deltaTime) {
    if (visible && openProgress < 1.f) {
        openProgress += OPEN_SPEED * deltaTime;
        if (openProgress > 1.f) openProgress = 1.f;
    } else if (!visible && openProgress > 0.f) {
        openProgress -= OPEN_SPEED * deltaTime;
        if (openProgress < 0.f) openProgress = 0.f;
    }
}

void ItemWheel::render(sf::RenderWindow& window) {
    if (openProgress <= 0.01f) return;
    
    float animatedRadius = WHEEL_RADIUS * openProgress;
    float alpha = openProgress * 255.f;
    
    // Background circle (semi-transparent)
    sf::CircleShape bgCircle(animatedRadius);
    bgCircle.setOrigin({animatedRadius, animatedRadius});
    bgCircle.setPosition(centerPosition);
    bgCircle.setFillColor(sf::Color(20, 20, 30, static_cast<uint8_t>(alpha * 0.85f)));
    bgCircle.setOutlineThickness(3.f);
    bgCircle.setOutlineColor(sf::Color(100, 100, 120, static_cast<uint8_t>(alpha)));
    window.draw(bgCircle);
    
    // Center circle (cancel zone)
    sf::CircleShape centerCircle(CENTER_RADIUS * openProgress);
    centerCircle.setOrigin({CENTER_RADIUS * openProgress, CENTER_RADIUS * openProgress});
    centerCircle.setPosition(centerPosition);
    centerCircle.setFillColor(sf::Color(40, 40, 50, static_cast<uint8_t>(alpha * 0.9f)));
    centerCircle.setOutlineThickness(2.f);
    centerCircle.setOutlineColor(sf::Color(80, 80, 100, static_cast<uint8_t>(alpha)));
    window.draw(centerCircle);
    
    if (items.empty()) return;
    
    float segmentAngle = 2.f * PI / items.size();
    float segmentRadius = (animatedRadius - CENTER_RADIUS * openProgress) / 2.f + CENTER_RADIUS * openProgress;
    
    for (size_t i = 0; i < items.size(); i++) {
        // Angle for this item (starting from top, going clockwise)
        float angle = -PI / 2.f + i * segmentAngle;
        
        // Item position
        float itemX = centerPosition.x + std::cos(angle) * segmentRadius;
        float itemY = centerPosition.y + std::sin(angle) * segmentRadius;
        
        // Highlight if hovered
        bool isHovered = (static_cast<int>(i) == hoveredIndex);
        float iconScale = isHovered ? 1.2f : 1.0f;
        
        // Draw slot background
        sf::CircleShape slotBg(ICON_SIZE / 2.f * iconScale);
        slotBg.setOrigin({ICON_SIZE / 2.f * iconScale, ICON_SIZE / 2.f * iconScale});
        slotBg.setPosition({itemX, itemY});
        slotBg.setFillColor(isHovered ? sf::Color(80, 80, 100, static_cast<uint8_t>(alpha)) 
                                      : sf::Color(50, 50, 60, static_cast<uint8_t>(alpha)));
        slotBg.setOutlineThickness(2.f);
        slotBg.setOutlineColor(isHovered ? sf::Color(255, 215, 0, static_cast<uint8_t>(alpha))
                                         : sf::Color(100, 100, 120, static_cast<uint8_t>(alpha)));
        window.draw(slotBg);
        
        // Draw item icon
        const ItemNew* item = items[i];
        if (item && !item->id.empty()) {
            sf::Texture* tex = AssetManager::getInstance().getTexture(item->id);
            if (tex) {
                sf::Sprite itemSprite(*tex);
                sf::Vector2u texSize = tex->getSize();
                float scale = (ICON_SIZE * 0.7f * iconScale) / std::max(texSize.x, texSize.y);
                itemSprite.setOrigin({texSize.x / 2.f, texSize.y / 2.f});
                itemSprite.setPosition({itemX, itemY});
                itemSprite.setScale({scale, scale});
                itemSprite.setColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha)));
                window.draw(itemSprite);
            }
        }
        
        // Draw hotkey number
        if (font) {
            sf::Text keyText(*font, std::to_string(i + 1), 12);
            keyText.setPosition({itemX - ICON_SIZE / 2.f, itemY - ICON_SIZE / 2.f - 5.f});
            keyText.setFillColor(sf::Color(255, 255, 100, static_cast<uint8_t>(alpha)));
            keyText.setOutlineThickness(1.f);
            keyText.setOutlineColor(sf::Color(0, 0, 0, static_cast<uint8_t>(alpha)));
            window.draw(keyText);
            
            // Stack count
            if (item && item->stackCount > 1) {
                sf::Text stackText(*font, "x" + std::to_string(item->stackCount), 10);
                sf::FloatRect bounds = stackText.getLocalBounds();
                stackText.setPosition({itemX + ICON_SIZE / 4.f - bounds.size.x / 2.f, 
                                       itemY + ICON_SIZE / 4.f});
                stackText.setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha)));
                stackText.setOutlineThickness(1.f);
                stackText.setOutlineColor(sf::Color(0, 0, 0, static_cast<uint8_t>(alpha)));
                window.draw(stackText);
            }
        }
    }
    
    // Draw "Tab" hint in center
    if (font) {
        sf::Text hint(*font, "TAB", 14);
        sf::FloatRect hintBounds = hint.getLocalBounds();
        hint.setOrigin({hintBounds.size.x / 2.f, hintBounds.size.y / 2.f});
        hint.setPosition(centerPosition);
        hint.setFillColor(sf::Color(150, 150, 170, static_cast<uint8_t>(alpha * 0.8f)));
        window.draw(hint);
    }
}
