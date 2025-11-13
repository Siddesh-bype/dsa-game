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
