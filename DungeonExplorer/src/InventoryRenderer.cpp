#include "InventoryRenderer.h"
#include "AssetManager.h"
#include <iostream>
#include <sstream>

InventoryRenderer::InventoryRenderer() {
}

void InventoryRenderer::renderItemSlot(sf::RenderWindow& window, const ItemNew& item,
                                       sf::Vector2f position, bool selected, bool hovered) {
    // Background slot
    sf::RectangleShape slotBg({SLOT_SIZE, SLOT_SIZE});
    slotBg.setPosition(position);
    slotBg.setFillColor(sf::Color(BG_SLOT_R, BG_SLOT_G, BG_SLOT_B, BG_SLOT_A));
    
    // Border color based on state
    if (selected) {
        slotBg.setOutlineColor(sf::Color(255, 215, 0)); // Gold
        slotBg.setOutlineThickness(BORDER_SELECTED);
    } else if (hovered) {
        slotBg.setOutlineColor(sf::Color(150, 150, 150)); // Gray
        slotBg.setOutlineThickness(BORDER_HOVERED);
    } else {
        slotBg.setOutlineColor(getRarityColor(item.rarity));
        slotBg.setOutlineThickness(BORDER_NORMAL);
    }
    
    window.draw(slotBg);
    
    // Render item icon - use item.id as key (ItemManager registers textures by ID)
    sf::Vector2f iconPos = position + sf::Vector2f(SLOT_PADDING, SLOT_PADDING);
    renderItemIcon(window, item.id, iconPos, {ICON_SIZE, ICON_SIZE});
}

void InventoryRenderer::renderItemIcon(sf::RenderWindow& window, const std::string& itemId,
                                       sf::Vector2f position, sf::Vector2f size) {
    // Try to load icon from AssetManager using item ID as key
    sf::Texture* iconTexture = AssetManager::getInstance().getTexture(itemId);
    
    if (iconTexture) {
        sf::Sprite iconSprite(*iconTexture);
        iconSprite.setPosition(position);
        
        // Scale to fit icon size
        sf::Vector2u texSize = iconTexture->getSize();
        float scaleX = size.x / texSize.x;
        float scaleY = size.y / texSize.y;
        float scale = std::min(scaleX, scaleY);
        iconSprite.setScale({scale, scale});
        
        window.draw(iconSprite);
    } else {
        // Fallback: colored rectangle
        sf::RectangleShape fallback(size);
        fallback.setPosition(position);
        fallback.setFillColor(sf::Color(100, 100, 120));
        fallback.setOutlineColor(sf::Color(150, 150, 170));
        fallback.setOutlineThickness(1.0f);
        window.draw(fallback);
    }
}

void InventoryRenderer::renderTooltip(sf::RenderWindow& window, const ItemNew& item,
                                      sf::Vector2f mousePos, const sf::Font& font) {
    // Build tooltip text
    std::stringstream ss;
    ss << item.name << "\n";
    ss << "Type: " << item.type << "\n";
    ss << "Rarity: " << item.rarity << "\n";
    
    // Check action parameters for damage/healing
    if (item.action.kind == "heal" && item.action.params.contains("amount")) {
        ss << "Healing: " << item.action.params["amount"] << "\n";
    }
    if (item.action.kind == "equip" && item.action.params.contains("attack_bonus")) {
        ss << "Attack: +" << item.action.params["attack_bonus"] << "\n";
    }
    
    ss << "Value: " << item.value << " gold\n";
    
    if (item.cooldown > 0) {
        ss << "Cooldown: " << item.cooldown << "s\n";
    }
    
    // Create text object
    sf::Text tooltipText(font, ss.str(), TOOLTIP_FONT_SIZE);
    tooltipText.setFillColor(sf::Color::White);
    
    // Calculate tooltip size
    sf::FloatRect textBounds = tooltipText.getLocalBounds();
    float tooltipWidth = textBounds.size.x + (TOOLTIP_PADDING * 2.0f);
    float tooltipHeight = textBounds.size.y + (TOOLTIP_PADDING * 2.0f);
    
    // Position tooltip near mouse, adjust if off-screen
    sf::Vector2f tooltipPos = mousePos + sf::Vector2f(TOOLTIP_OFFSET, TOOLTIP_OFFSET);
    sf::Vector2u windowSize = window.getSize();
    
    if (tooltipPos.x + tooltipWidth > windowSize.x) {
        tooltipPos.x = mousePos.x - tooltipWidth - TOOLTIP_OFFSET;
    }
    if (tooltipPos.y + tooltipHeight > windowSize.y) {
        tooltipPos.y = mousePos.y - tooltipHeight - TOOLTIP_OFFSET;
    }
    
    // Background
    sf::RectangleShape tooltipBg({tooltipWidth, tooltipHeight});
    tooltipBg.setPosition(tooltipPos);
    tooltipBg.setFillColor(sf::Color(BG_TOOLTIP_R, BG_TOOLTIP_G, BG_TOOLTIP_B, BG_TOOLTIP_A));
    tooltipBg.setOutlineColor(getRarityColor(item.rarity));
    tooltipBg.setOutlineThickness(BORDER_TOOLTIP);
    
    window.draw(tooltipBg);
    
    // Text
    tooltipText.setPosition(tooltipPos + sf::Vector2f(TOOLTIP_PADDING, TOOLTIP_PADDING));
    window.draw(tooltipText);
}

void InventoryRenderer::renderLinkedListArrow(sf::RenderWindow& window,
                                              sf::Vector2f startPos, sf::Vector2f endPos) {
    // Draw line
    sf::Vertex line[] = {
        sf::Vertex{{startPos}, sf::Color(150, 150, 150)},
        sf::Vertex{{endPos}, sf::Color(150, 150, 150)}
    };
    window.draw(line, 2, sf::PrimitiveType::Lines);
    
    // Draw arrowhead
    sf::Vector2f direction = endPos - startPos;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction /= length;
        
        sf::Vector2f perpendicular(-direction.y, direction.x);
        
        sf::ConvexShape arrowhead(3);
        arrowhead.setPoint(0, endPos);
        arrowhead.setPoint(1, endPos - direction * ARROW_SIZE + perpendicular * (ARROW_SIZE * 0.5f));
        arrowhead.setPoint(2, endPos - direction * ARROW_SIZE - perpendicular * (ARROW_SIZE * 0.5f));
        arrowhead.setFillColor(sf::Color(150, 150, 150));
        
        window.draw(arrowhead);
    }
}

sf::Color InventoryRenderer::getRarityColor(int rarity) const {
    switch (rarity) {
        case 1: return sf::Color(150, 150, 150);  // Common - Gray
        case 2: return sf::Color(50, 200, 50);    // Uncommon - Green
        case 3: return sf::Color(50, 150, 255);   // Rare - Blue
        case 4: return sf::Color(200, 50, 200);   // Epic - Purple
        case 5: return sf::Color(255, 165, 0);    // Legendary - Orange
        default: return sf::Color(100, 100, 100); // Unknown - Dark Gray
    }
}
