#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "ItemNew.h"

class InventoryRenderer {
public:
    InventoryRenderer();
    
    // Render a single item slot with icon and border
    void renderItemSlot(sf::RenderWindow& window, const ItemNew& item, sf::Vector2f position, bool selected, bool hovered);
    
    // Render item icon (loads from AssetManager or uses fallback)
    void renderItemIcon(sf::RenderWindow& window, const std::string& iconPath, sf::Vector2f position, sf::Vector2f size);
    
    // Render detailed tooltip for an item
    void renderTooltip(sf::RenderWindow& window, const ItemNew& item, sf::Vector2f mousePos, const sf::Font& font);
    
    // Render arrows showing LinkedList connections
    void renderLinkedListArrow(sf::RenderWindow& window, sf::Vector2f startPos, sf::Vector2f endPos);
    
    // Get rarity color for item
    sf::Color getRarityColor(int rarity) const;
    
private:
    // ═══════════════════════════════════════════════════════════════════════
    // CONFIGURATION CONSTANTS
    // ═══════════════════════════════════════════════════════════════════════
    
    // Slot dimensions
    static constexpr float SLOT_SIZE = 64.0f;
    static constexpr float SLOT_PADDING = 8.0f;
    static constexpr float ICON_SIZE = 48.0f;
    
    // Border thickness
    static constexpr float BORDER_SELECTED = 3.0f;
    static constexpr float BORDER_HOVERED = 2.0f;
    static constexpr float BORDER_NORMAL = 1.5f;
    static constexpr float BORDER_TOOLTIP = 2.0f;
    
    // Background colors
    static constexpr int BG_SLOT_R = 40;
    static constexpr int BG_SLOT_G = 40;
    static constexpr int BG_SLOT_B = 45;
    static constexpr int BG_SLOT_A = 200;
    
    static constexpr int BG_TOOLTIP_R = 20;
    static constexpr int BG_TOOLTIP_G = 20;
    static constexpr int BG_TOOLTIP_B = 25;
    static constexpr int BG_TOOLTIP_A = 240;
    
    // Tooltip settings
    static constexpr float TOOLTIP_PADDING = 10.0f;
    static constexpr float TOOLTIP_OFFSET = 15.0f;
    static constexpr unsigned int TOOLTIP_FONT_SIZE = 14;
    
    // Arrow settings
    static constexpr float ARROW_SIZE = 8.0f;
};
