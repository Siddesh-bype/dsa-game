#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "ItemNew.h"
#include "Player.h"

struct ShopItem {
    ItemNew item;
    int price;
    
    ShopItem(const ItemNew& i, int p) : item(i), price(p) {}
};

class Shop {
private:
    std::vector<ShopItem> shopInventory;
    bool isOpen;
    int selectedIndex;
    sf::Font font;
    bool fontLoaded;
    
    // ═══════════════════════════════════════════════════════════════════════
    // SHOP CONFIGURATION CONSTANTS
    // ═══════════════════════════════════════════════════════════════════════
    
    // Item prices
    static constexpr int PRICE_POTION = 75;
    static constexpr int PRICE_ELIXIR = 200;
    static constexpr int PRICE_MEGA_POTION = 150;
    static constexpr int PRICE_SWORD_IRON = 100;
    static constexpr int PRICE_SHIELD_WOOD = 50;
    static constexpr int PRICE_BOMB = 120;
    static constexpr int PRICE_SMOKE_BOMB = 60;
    static constexpr int PRICE_MAP_FRAGMENT = 40;
    
    // UI dimensions
    static constexpr float PANEL_WIDTH = 500.0f;
    static constexpr float PANEL_HEIGHT = 450.0f;
    static constexpr float PANEL_OUTLINE = 3.0f;
    static constexpr float ITEM_HEIGHT = 35.0f;
    static constexpr float PADDING = 20.0f;
    static constexpr float TITLE_Y = 15.0f;
    static constexpr float INSTR_Y = 50.0f;
    static constexpr float LIST_START_Y = 90.0f;
    
    // Font sizes
    static constexpr int TITLE_FONT_SIZE = 28;
    static constexpr int INSTR_FONT_SIZE = 12;
    static constexpr int ITEM_FONT_SIZE = 16;
    static constexpr int TYPE_FONT_SIZE = 11;
    
    // Colors
    static constexpr int OVERLAY_ALPHA = 180;
    static constexpr int PANEL_BG_R = 40, PANEL_BG_G = 30, PANEL_BG_B = 20, PANEL_BG_A = 250;
    static constexpr int BORDER_R = 200, BORDER_G = 150, BORDER_B = 100;
    static constexpr int GOLD_R = 255, GOLD_G = 215, GOLD_B = 0;
    static constexpr int HIGHLIGHT_R = 80, HIGHLIGHT_G = 60, HIGHLIGHT_B = 40, HIGHLIGHT_A = 200;
    
public:
    Shop();
    
    void initialize();
    void addItem(const ItemNew& item, int price);
    void open();
    void close();
    void toggle();
    
    bool isShopOpen() const { return isOpen; }
    
    // Returns true if purchase was successful
    bool purchaseItem(int index, Player* player);
    
    void handleInput(sf::Keyboard::Key key, Player* player);
    void render(sf::RenderWindow& window);
    
    void moveSelectionUp();
    void moveSelectionDown();
};
