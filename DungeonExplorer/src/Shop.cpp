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
        addItem(itemMgr.getItemById("potion"), PRICE_POTION);
    }
    if (itemMgr.hasItem("elixir")) {
        addItem(itemMgr.getItemById("elixir"), PRICE_ELIXIR);
    }
    if (itemMgr.hasItem("potion_mega")) {
        addItem(itemMgr.getItemById("potion_mega"), PRICE_MEGA_POTION);
    }
    if (itemMgr.hasItem("sword_iron")) {
        addItem(itemMgr.getItemById("sword_iron"), PRICE_SWORD_IRON);
    }
    if (itemMgr.hasItem("shield_wood")) {
        addItem(itemMgr.getItemById("shield_wood"), PRICE_SHIELD_WOOD);
    }
    if (itemMgr.hasItem("bomb")) {
        addItem(itemMgr.getItemById("bomb"), PRICE_BOMB);
    }
    if (itemMgr.hasItem("smoke_bomb")) {
        addItem(itemMgr.getItemById("smoke_bomb"), PRICE_SMOKE_BOMB);
    }
    if (itemMgr.hasItem("map_fragment")) {
        addItem(itemMgr.getItemById("map_fragment"), PRICE_MAP_FRAGMENT);
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
        player->addItem(shopItem.item);
        
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
    overlay.setFillColor(sf::Color(0, 0, 0, OVERLAY_ALPHA));
    window.draw(overlay);
    
    // Shop panel
    float panelX = (window.getSize().x - PANEL_WIDTH) / 2.f;
    float panelY = (window.getSize().y - PANEL_HEIGHT) / 2.f;
    
    sf::RectangleShape panel(sf::Vector2f(PANEL_WIDTH, PANEL_HEIGHT));
    panel.setPosition(sf::Vector2f(panelX, panelY));
    panel.setFillColor(sf::Color(PANEL_BG_R, PANEL_BG_G, PANEL_BG_B, PANEL_BG_A));
    panel.setOutlineColor(sf::Color(BORDER_R, BORDER_G, BORDER_B));
    panel.setOutlineThickness(PANEL_OUTLINE);
    window.draw(panel);
    
    // Title
    sf::Text title(font, "SHOP", TITLE_FONT_SIZE);
    title.setPosition(sf::Vector2f(panelX + PADDING, panelY + TITLE_Y));
    title.setFillColor(sf::Color(GOLD_R, GOLD_G, GOLD_B));
    title.setOutlineThickness(2.f);
    title.setOutlineColor(sf::Color::Black);
    window.draw(title);
    
    // Instructions
    sf::Text instructions(font, "Arrow Keys: Select | Enter: Buy | P/Esc: Close", INSTR_FONT_SIZE);
    instructions.setPosition(sf::Vector2f(panelX + PADDING, panelY + INSTR_Y));
    instructions.setFillColor(sf::Color(180, 180, 180));
    window.draw(instructions);
    
    // Item list
    float itemY = panelY + LIST_START_Y;
    
    for (size_t i = 0; i < shopInventory.size(); ++i) {
        const ShopItem& shopItem = shopInventory[i];
        bool isSelected = (static_cast<int>(i) == selectedIndex);
        
        // Selection highlight
        if (isSelected) {
            sf::RectangleShape highlight(sf::Vector2f(PANEL_WIDTH - 2*PADDING, ITEM_HEIGHT - 5.f));
            highlight.setPosition(sf::Vector2f(panelX + PADDING, itemY));
            highlight.setFillColor(sf::Color(HIGHLIGHT_R, HIGHLIGHT_G, HIGHLIGHT_B, HIGHLIGHT_A));
            highlight.setOutlineColor(sf::Color(GOLD_R, GOLD_G, GOLD_B));
            highlight.setOutlineThickness(2.f);
            window.draw(highlight);
        }
        
        // Item name
        sf::Text itemName(font, shopItem.item.name, ITEM_FONT_SIZE);
        itemName.setPosition(sf::Vector2f(panelX + PADDING + 10.f, itemY + 5.f));
        itemName.setFillColor(shopItem.item.getRarityColor());
        itemName.setOutlineThickness(1.f);
        itemName.setOutlineColor(sf::Color::Black);
        window.draw(itemName);
        
        // Item type and rarity
        std::string typeText = shopItem.item.type + " (" + shopItem.item.getRarityName() + ")";
        sf::Text itemDesc(font, typeText, TYPE_FONT_SIZE);
        itemDesc.setPosition(sf::Vector2f(panelX + PADDING + 10.f, itemY + 22.f));
        itemDesc.setFillColor(sf::Color(150, 150, 150));
        window.draw(itemDesc);
        
        // Price
        sf::Text priceText(font, std::to_string(shopItem.price) + " G", ITEM_FONT_SIZE);
        priceText.setPosition(sf::Vector2f(panelX + PANEL_WIDTH - 100.f, itemY + 8.f));
        priceText.setFillColor(sf::Color(GOLD_R, GOLD_G, GOLD_B));
        priceText.setOutlineThickness(1.f);
        priceText.setOutlineColor(sf::Color::Black);
        window.draw(priceText);
        
        itemY += ITEM_HEIGHT;
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
