#include "DSAVisualizer.h"
#include <cmath>
#include <cstdint>
#include <array>

DSAVisualizer::DSAVisualizer() : font(nullptr) {
}

void DSAVisualizer::setFont(const sf::Font* fontPtr) {
    font = fontPtr;
}

void DSAVisualizer::renderStackTrail(sf::RenderWindow& window, const Player& player, float tileSize) {
    Position currentPos = player.getPosition();
    
    for (int i = 0; i < 5; i++) {
        sf::CircleShape footprint(4.f - i * 0.5f);
        float offsetX = (i % 2 == 0) ? -3.f : 3.f;
        float offsetY = -i * 8.f;
        
        footprint.setPosition(sf::Vector2f(
            currentPos.x * tileSize + tileSize * 0.5f + offsetX,
            currentPos.y * tileSize + tileSize * 0.5f + offsetY
        ));
        
        float alpha = 180.f * std::pow(0.65f, static_cast<float>(i));
        footprint.setFillColor(sf::Color(100, 200, 255, static_cast<std::uint8_t>(alpha)));
        window.draw(footprint);
    }
    
    // CHANGE: 2025-11-14 - Add null pointer validation for font before rendering text
    if (font && font->getInfo().family != "") {
        sf::Text stackLabel(*font, "Stack Trail", 10);
        stackLabel.setPosition(sf::Vector2f(10.f, 75.f));
        stackLabel.setFillColor(sf::Color(150, 200, 255));
        window.draw(stackLabel);
    }
}

void DSAVisualizer::renderGraphPaths(sf::RenderWindow& window, const Dungeon& dungeon, float tileSize) {
    const auto& rooms = dungeon.getRooms();
    
    if (rooms.size() < 2) return;
    
    for (size_t i = 0; i < rooms.size() - 1; i++) {
        const auto& room1 = rooms[i];
        const auto& room2 = rooms[i + 1];
        
        sf::Vector2f start(
            (room1.x + room1.width / 2.f) * tileSize,
            (room1.y + room1.height / 2.f) * tileSize
        );
        sf::Vector2f end(
            (room2.x + room2.width / 2.f) * tileSize,
            (room2.y + room2.height / 2.f) * tileSize
        );
        
        // Reduced alpha from 100 to 60 for subtler appearance
        std::array<sf::Vertex, 2> line = {{
            {{start.x, start.y}, sf::Color(80, 200, 80, 60)},
            {{end.x, end.y}, sf::Color(80, 200, 80, 60)}
        }};
        window.draw(line.data(), 2, sf::PrimitiveType::Lines);
        
        sf::CircleShape vertex(3.f);
        vertex.setPosition(sf::Vector2f(start.x - 3.f, start.y - 3.f));
        vertex.setFillColor(sf::Color(80, 200, 80, 80));
        window.draw(vertex);
    }
    
    if (!rooms.empty()) {
        const auto& lastRoom = rooms.back();
        sf::Vector2f lastPos(
            (lastRoom.x + lastRoom.width / 2.f) * tileSize,
            (lastRoom.y + lastRoom.height / 2.f) * tileSize
        );
        sf::CircleShape vertex(3.f);
        vertex.setPosition(sf::Vector2f(lastPos.x - 3.f, lastPos.y - 3.f));
        vertex.setFillColor(sf::Color(80, 200, 80, 80));
        window.draw(vertex);
    }
}

void DSAVisualizer::renderHeapGlow(sf::RenderWindow& window, sf::Vector2f position, int value) {
    float time = static_cast<float>(std::clock()) / CLOCKS_PER_SEC;
    float brightness = 0.5f + 0.5f * std::sin(time * 3.f);
    
    sf::CircleShape glow(15.f);
    glow.setPosition(sf::Vector2f(position.x - 15.f, position.y - 15.f));
    glow.setFillColor(sf::Color(255, 215, 0, static_cast<std::uint8_t>(50 * brightness)));
    glow.setOutlineThickness(2.f);
    glow.setOutlineColor(sf::Color(255, 255, 100, 100));
    window.draw(glow);
    
    for (int i = 0; i < 5; i++) {
        float angle = (time * 2.f + i * 72.f) * 3.14159f / 180.f;
        float radius = 20.f + 5.f * std::sin(time * 4.f + i);
        
        sf::CircleShape sparkle(2.f);
        sparkle.setPosition(sf::Vector2f(
            position.x + std::cos(angle) * radius - 2.f,
            position.y + std::sin(angle) * radius - 2.f
        ));
        sparkle.setFillColor(sf::Color(255, 255, 200, static_cast<std::uint8_t>(200 * brightness)));
        window.draw(sparkle);
    }
    
    // CHANGE: 2025-11-14 - Add null pointer validation for font before rendering
    if (font && font->getInfo().family != "" && value > 0) {
        sf::Text valueText(*font, std::to_string(value), 12);
        valueText.setPosition(sf::Vector2f(position.x - 10.f, position.y - 20.f));
        valueText.setFillColor(sf::Color(255, 255, 255));
        valueText.setOutlineThickness(1.f);
        valueText.setOutlineColor(sf::Color(0, 0, 0));
        window.draw(valueText);
    }
}

void DSAVisualizer::renderLootPopup(sf::RenderWindow& window, const std::string& itemName, 
                                   int value, float x, float y, float time) {
    float alpha = 255.f * (1.f - time);
    
    if (alpha > 0.f && font) {
        sf::RectangleShape popup(sf::Vector2f(120.f, 40.f));
        popup.setPosition(sf::Vector2f(x - 60.f, y - 50.f - time * 20.f));
        popup.setFillColor(sf::Color(255, 215, 0, static_cast<std::uint8_t>(alpha)));
        popup.setOutlineColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(alpha)));
        popup.setOutlineThickness(2.f);
        window.draw(popup);
        
        sf::Text itemText(*font, itemName, 12);
        itemText.setPosition(sf::Vector2f(x - 50.f, y - 45.f - time * 20.f));
        itemText.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(alpha)));
        window.draw(itemText);
        
        sf::Text valueText(*font, "+" + std::to_string(value), 10);
        valueText.setPosition(sf::Vector2f(x - 20.f, y - 28.f - time * 20.f));
        valueText.setFillColor(sf::Color(255, 100, 100, static_cast<std::uint8_t>(alpha)));
        window.draw(valueText);
    }
}
