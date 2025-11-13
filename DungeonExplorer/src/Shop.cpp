#include "Shop.h"
#include "ItemManager.h"
#include <iostream>

Shop::Shop() : isOpen(false), selectedIndex(0), fontLoaded(false) {
}

void Shop::initialize() {
    // Load font
    if (font.openFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        fontLoaded = true;
        std::cout << "[Shop] Font loaded successfully" << std::endl;
    } else {
        std::cerr << "[Shop] Failed to load font!" << std::endl;
    }
    
    // Add items to shop with prices (higher than item value)
    ItemManager& itemMgr = ItemManager::getInstance();
    
    if (itemMgr.hasItem("potion")) {
        addItem(itemMgr.getItemById("potion"), 75);
    }
    if (itemMgr.hasItem("elixir")) {
        addItem(itemMgr.getItemById("elixir"), 200);
    }
    if (itemMgr.hasItem("potion_mega")) {
        addItem(itemMgr.getItemById("potion_mega"), 150);
    }
    if (itemMgr.hasItem("sword_iron")) {
        addItem(itemMgr.getItemById("sword_iron"), 100);
    }
    if (itemMgr.hasItem("shield_wood")) {
        addItem(itemMgr.getItemById("shield_wood"), 50);
    }
    if (itemMgr.hasItem("bomb")) {
        addItem(itemMgr.getItemById("bomb"), 120);
    }
    if (itemMgr.hasItem("smoke_bomb")) {
        addItem(itemMgr.getItemById("smoke_bomb"), 60);
    }
    if (itemMgr.hasItem("map_fragment")) {
        addItem(itemMgr.getItemById("map_fragment"), 40);
    }
    
    std::cout << "[Shop] Initialized with " << shopInventory.size() << " items" << std::endl;
}

void Shop::addItem(const ItemNew& item, int price) {
    shopInventory.emplace_back(item, price);
}

void Shop::open() {
    isOpen = true;
    selectedIndex = 0;
    std::cout << "[Shop] Opened" << std::endl;
}

void Shop::close() {
    isOpen = false;
    std::cout << "[Shop] Closed" << std::endl;
}

void Shop::toggle() {
    if (isOpen) {
        close();
    } else {
        open();
    }
}

bool Shop::purchaseItem(int index, Player* player) {
    if (index < 0 || index >= static_cast<int>(shopInventory.size())) {
        return false;
    }
    
    const ShopItem& shopItem = shopInventory[index];
    
    if (player->spendGold(shopItem.price)) {
        // Add item to player inventory (new system)
        player->addItemNew(shopItem.item);
        
        std::cout << "[Shop] Purchased " << shopItem.item.name << " for " << shopItem.price << " gold" << std::endl;
        return true;
    }
    
    return false;
}

void Shop::handleInput(sf::Keyboard::Key key, Player* player) {
    if (!isOpen) return;
    
    switch (key) {
        case sf::Keyboard::Key::Up:
        case sf::Keyboard::Key::W:
            moveSelectionUp();
            break;
            
        case sf::Keyboard::Key::Down:
        case sf::Keyboard::Key::S:
            moveSelectionDown();
            break;
            
        case sf::Keyboard::Key::Enter:
        case sf::Keyboard::Key::Space:
            purchaseItem(selectedIndex, player);
            break;
            
        case sf::Keyboard::Key::Escape:
        case sf::Keyboard::Key::P:
            close();
            break;
            
        default:
            break;
    }
}

void Shop::render(sf::RenderWindow& window) {
    if (!isOpen || !fontLoaded) return;
    
    // Dark overlay
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);
    
    // Shop panel
    float panelWidth = 500.f;
    float panelHeight = 450.f;
    float panelX = (window.getSize().x - panelWidth) / 2.f;
    float panelY = (window.getSize().y - panelHeight) / 2.f;
    
    sf::RectangleShape panel(sf::Vector2f(panelWidth, panelHeight));
    panel.setPosition(sf::Vector2f(panelX, panelY));
    panel.setFillColor(sf::Color(40, 30, 20, 250));
    panel.setOutlineColor(sf::Color(200, 150, 100));
    panel.setOutlineThickness(3.f);
    window.draw(panel);
    
    // Title
    sf::Text title(font, "SHOP", 28);
    title.setPosition(sf::Vector2f(panelX + 20.f, panelY + 15.f));
    title.setFillColor(sf::Color(255, 215, 0));
    title.setOutlineThickness(2.f);
    title.setOutlineColor(sf::Color::Black);
    window.draw(title);
    
    // Instructions
    sf::Text instructions(font, "Arrow Keys: Select | Enter: Buy | P/Esc: Close", 12);
    instructions.setPosition(sf::Vector2f(panelX + 20.f, panelY + 50.f));
    instructions.setFillColor(sf::Color(180, 180, 180));
    window.draw(instructions);
    
    // Item list
    float itemY = panelY + 90.f;
    float itemHeight = 35.f;
    
    for (size_t i = 0; i < shopInventory.size(); ++i) {
        const ShopItem& shopItem = shopInventory[i];
        bool isSelected = (static_cast<int>(i) == selectedIndex);
        
        // Selection highlight
        if (isSelected) {
            sf::RectangleShape highlight(sf::Vector2f(panelWidth - 40.f, itemHeight - 5.f));
            highlight.setPosition(sf::Vector2f(panelX + 20.f, itemY));
            highlight.setFillColor(sf::Color(80, 60, 40, 200));
            highlight.setOutlineColor(sf::Color(255, 215, 0));
            highlight.setOutlineThickness(2.f);
            window.draw(highlight);
        }
        
        // Item name
        sf::Text itemName(font, shopItem.item.name, 16);
        itemName.setPosition(sf::Vector2f(panelX + 30.f, itemY + 5.f));
        itemName.setFillColor(shopItem.item.getRarityColor());
        itemName.setOutlineThickness(1.f);
        itemName.setOutlineColor(sf::Color::Black);
        window.draw(itemName);
        
        // Item type and rarity
        std::string typeText = shopItem.item.type + " (" + shopItem.item.getRarityName() + ")";
        sf::Text itemDesc(font, typeText, 11);
        itemDesc.setPosition(sf::Vector2f(panelX + 30.f, itemY + 22.f));
        itemDesc.setFillColor(sf::Color(150, 150, 150));
        window.draw(itemDesc);
        
        // Price
        sf::Text priceText(font, std::to_string(shopItem.price) + " G", 16);
        priceText.setPosition(sf::Vector2f(panelX + panelWidth - 100.f, itemY + 8.f));
        priceText.setFillColor(sf::Color(255, 215, 0));
        priceText.setOutlineThickness(1.f);
        priceText.setOutlineColor(sf::Color::Black);
        window.draw(priceText);
        
        itemY += itemHeight;
    }
}

void Shop::moveSelectionUp() {
    selectedIndex--;
    if (selectedIndex < 0) {
        selectedIndex = static_cast<int>(shopInventory.size()) - 1;
    }
}

void Shop::moveSelectionDown() {
    selectedIndex++;
    if (selectedIndex >= static_cast<int>(shopInventory.size())) {
        selectedIndex = 0;
    }
}
