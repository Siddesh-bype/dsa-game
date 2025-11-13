// CHANGE: 2025-11-10 — TASK B & H: Enhanced asset loading with validation
// - Added INFO/ERROR logging for all asset loads
// - Improved error messages with full paths
// - Added missing asset detection

#include "AssetManager.h"
#include <iostream>
#include <fstream>

// Note: JSON parsing would normally use a library like nlohmann/json
// For now, we'll implement manual loading and add JSON support later

AssetManager& AssetManager::getInstance() {
    static AssetManager instance;
    return instance;
}

bool AssetManager::loadTexture(const std::string& key, const std::string& filePath) {
    auto texture = std::make_unique<sf::Texture>();
    
    if (!texture->loadFromFile(filePath)) {
        std::cerr << "[ERROR] Missing asset '" << key << "' -> " << filePath << std::endl;
        return false;
    }
    
    texture->setSmooth(false);  // Pixel-perfect rendering
    textures[key] = std::move(texture);
    std::cout << "[INFO] Loaded asset '" << key << "' -> " << filePath << std::endl;
    return true;
}

sf::Texture* AssetManager::getTexture(const std::string& key) {
    auto it = textures.find(key);
    if (it != textures.end()) {
        return it->second.get();
    }
    
    std::cerr << "[AssetManager] Texture not found: " << key << std::endl;
    return nullptr;
}

bool AssetManager::hasTexture(const std::string& key) const {
    return textures.find(key) != textures.end();
}

void AssetManager::clear() {
    textures.clear();
    spritesheets.clear();
    std::cout << "[AssetManager] Cleared all assets" << std::endl;
}

sf::Texture* AssetManager::getSpritesheet(const std::string& sheetName) {
    auto it = spritesheets.find(sheetName);
    if (it != spritesheets.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool AssetManager::createSpriteFromSheet(const std::string& sheetName, int tileIndex, sf::Sprite& outSprite, int columns, int tileSize, int spacing) {
    sf::Texture* sheet = getSpritesheet(sheetName);
    if (!sheet) {
        std::cerr << "[AssetManager] Spritesheet not found: " << sheetName << std::endl;
        return false;
    }

    // Compute tile position with 0-based tileIndex
    int tx = tileIndex % columns;
    int ty = tileIndex / columns;
    int x = tx * (tileSize + spacing);
    int y = ty * (tileSize + spacing);

    sf::IntRect rect({x, y}, {tileSize, tileSize});
    outSprite = sf::Sprite(*sheet, rect);
    return true;
}

bool AssetManager::loadFromManifest(const std::string& jsonPath) {
    std::cout << "[AssetManager] Loading DebtsInTheDepths assets..." << std::endl;
    
    // ═══════════════════════════════════════════════════════════════════════
    // DEBTS IN THE DEPTHS ASSET PACK
    // Location: assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/
    // Using PNG sprites (animated GIFs available for future use)
    // ═══════════════════════════════════════════════════════════════════════
    
    // 🟫 FLOOR & ENVIRONMENT TILES (Using GIFs for animated tiles)
    loadTexture("floor", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Environment/sprBrick.gif");
    loadTexture("floor_variant_1", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Environment/sprBrimstone.gif");
    loadTexture("floor_variant_2", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Environment/sprRock.gif");
    loadTexture("floor_variant_3", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Environment/sprBrick.gif");
    loadTexture("floor_variant_4", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Environment/sprBrimstone.gif");
    
    // 🧱 STRUCTURAL ELEMENTS - Using different prop for walls
    loadTexture("wall", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Environment/sprRock.gif");
    
    // 🪜 STAIRS - Using Kenney Tiny Dungeon
    loadTexture("stairs_up", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0014.png");
    loadTexture("stairs_down", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0040.png");
    
    // 🚪 DOORS - Using Kenney Tiny Dungeon for better door sprites
    loadTexture("door_closed", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0045.png");
    loadTexture("door_open", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0000.png");
    
    // 👤 PLAYER CHARACTER - WIZARD (Using GIF for animation)
    loadTexture("player_warrior", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprWizard.gif");
    loadTexture("player_rogue", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprWizard.gif");
    loadTexture("player_mage", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprWizard.gif");
    
    // 👹 ENEMY MONSTERS - Individual sprites (Using GIF for animation)
    // Basic Enemies
    loadTexture("goblin", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprGoblin1.gif");
    loadTexture("slime", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprBogslium1.gif");
    loadTexture("orc", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprOrcArcher1.gif");
    loadTexture("skeleton", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprSkeleton1.gif");
    loadTexture("demon", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprGhost1.gif");
    
    // Advanced Enemies
    loadTexture("wraith", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprGhost2.gif");
    loadTexture("vampire", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprBatilisk1.gif");
    loadTexture("lich", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprLizardMonk1.gif");
    loadTexture("dragon", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprDragon.gif");
    loadTexture("necromancer", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprLizardMonk2.gif");
    loadTexture("dark_mage", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprLizardMonk3.gif");
    loadTexture("gargoyle", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprBatilisk2.gif");
    loadTexture("minotaur", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Characters/sprMinotaur1.gif");
    
    // 🎒 ITEMS & COLLECTIBLES - Using Kenney Tiny Dungeon
    loadTexture("potion_red", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0090.png");
    loadTexture("potion_blue", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0091.png");
    
    // Collectibles - Kenney Tiny Dungeon
    loadTexture("coin", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0088.png");
    loadTexture("chest", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0050.png");
    loadTexture("key", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0060.png");
    
    // ⚔️ WEAPONS & EFFECTS - Using Kenney Tiny Dungeon for weapons
    loadTexture("sword_iron", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0092.png");
    loadTexture("sword_flame", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0093.png");
    loadTexture("shield", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0094.png");
    
    // ✨ ADDITIONAL EFFECTS - For combat and visual enhancement
    loadTexture("effect_attack_swing", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Effects/sprAttackSwing.png");
    loadTexture("effect_attack_large", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Effects/sprAttackLarge.png");
    loadTexture("effect_sparkle", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Effects/sprSparkle.png");
    loadTexture("effect_explosion", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Effects/sprExplosion.png");
    loadTexture("effect_fire_explosion", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Effects/sprFireExplosion.png");
    loadTexture("effect_magic_explosion", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Effects/sprMagicExplosion.png");
    loadTexture("effect_arrow", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Effects/sprArrow.png");
    loadTexture("effect_acid", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Effects/sprAcidProjectile.png");
    loadTexture("effect_ghost_orb", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/Effects/sprGhostOrb.png");
    
    // � BUTTONS - Kenney UI
    loadTexture("button_square_blue", "assets/kenney/kenney_ui-pack-rpg-expansion/PNG/buttonSquare_blue.png");
    loadTexture("button_long_blue", "assets/kenney/kenney_ui-pack-rpg-expansion/PNG/buttonLong_blue.png");
    
    // �💗 UI ICONS - DebtsInTheDepths
    loadTexture("ui_heart", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/UI/sprHeart.png");
    loadTexture("ui_gold", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/UI/sprGoldIcon.png");
    loadTexture("ui_cursor", "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/UI/sprCursor.png");
    
    // ═══════════════════════════════════════════════════════════════════════
    // 📊 SPRITESHEETS - Large combined tilesets
    // These are used by Dungeon.cpp, Player.cpp, Enemy.cpp
    // ═══════════════════════════════════════════════════════════════════════
    
    // 🎮 TINY DUNGEON SPRITESHEET - Main game tileset (THIS IS THE ONE USED!)
    // File: tilemap.png (203x186 pixels, 12 columns x 11 rows = 132 tiles)
    // Tile size: 16x16 pixels with 1px spacing
    // Contains: floors, walls, stairs, doors, characters, enemies, items
    auto tinyDungeonSheet = std::make_unique<sf::Texture>();
    if (tinyDungeonSheet->loadFromFile("assets/kenney/kenney_tiny-dungeon/Tilemap/tilemap.png")) {
        tinyDungeonSheet->setSmooth(false);  // Keep pixels sharp
        spritesheets["tiny_dungeon"] = std::move(tinyDungeonSheet);
        std::cout << "[AssetManager] Loaded Tiny Dungeon spritesheet" << std::endl;
    }
    
    // 🎯 ROGUELIKE SPRITESHEET - Alternative larger tileset (not currently used)
    // File: roguelikeSheet_transparent.png
    // Contains more detailed sprites but different layout
    auto roguelikeSheet = std::make_unique<sf::Texture>();
    if (roguelikeSheet->loadFromFile("assets/kenney/kenney_roguelike-rpg-pack/Spritesheet/roguelikeSheet_transparent.png")) {
        roguelikeSheet->setSmooth(false);
        spritesheets["roguelike"] = std::move(roguelikeSheet);
        std::cout << "[AssetManager] Loaded Roguelike spritesheet" << std::endl;
    }
    
    std::cout << "[AssetManager] Loaded " << textures.size() << " textures and " 
              << spritesheets.size() << " spritesheets" << std::endl;
    
    return true;
}

void AssetManager::loadTinyDungeonPack() {
    std::cout << "[AssetManager] Loading Tiny Dungeon pack (colorful)..." << std::endl;
    
    // Clear existing textures
    textures.clear();
    
    // Tiny Dungeon tiles (16x16 base size)
    loadTexture("floor", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0001.png");
    loadTexture("floor_variant_1", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0002.png");
    loadTexture("floor_variant_2", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0003.png");
    loadTexture("floor_variant_3", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0004.png");
    loadTexture("floor_variant_4", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0005.png");
    loadTexture("wall", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0023.png");
    loadTexture("door_closed", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0017.png");
    loadTexture("door_open", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0018.png");
    loadTexture("stairs_down", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0046.png");
    loadTexture("stairs_up", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0047.png");
    
    // Characters
    loadTexture("player_warrior", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0084.png");
    loadTexture("player_rogue", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0085.png");
    loadTexture("player_mage", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0086.png");
    
    // Enemies
    loadTexture("goblin", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0108.png");
    loadTexture("orc", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0109.png");
    loadTexture("skeleton", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0110.png");
    loadTexture("demon", "assets/kenney/kenney_tiny-dungeon/Tiles/tile_0111.png");
    
    std::cout << "[AssetManager] Tiny Dungeon pack loaded with " << textures.size() << " textures" << std::endl;
}

void AssetManager::loadOneBitPack() {
    std::cout << "[AssetManager] Loading 1-Bit Pack (monochrome)..." << std::endl;
    
    // Clear existing textures
    textures.clear();
    
    // 1-Bit Pack tiles (16x16 monochrome)
    loadTexture("floor", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0000.png");
    loadTexture("floor_variant_1", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0001.png");
    loadTexture("floor_variant_2", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0002.png");
    loadTexture("floor_variant_3", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0003.png");
    loadTexture("floor_variant_4", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0004.png");
    loadTexture("wall", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0028.png");
    loadTexture("door_closed", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0064.png");
    loadTexture("door_open", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0065.png");
    loadTexture("stairs_down", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0046.png");
    loadTexture("stairs_up", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0047.png");
    
    // Characters (using generic sprites from 1-bit pack)
    loadTexture("player_warrior", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0432.png");
    loadTexture("player_rogue", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0433.png");
    loadTexture("player_mage", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0434.png");
    
    // Enemies (using creature tiles)
    loadTexture("goblin", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0451.png");
    loadTexture("orc", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0452.png");
    loadTexture("skeleton", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0453.png");
    loadTexture("demon", "assets/kenney/kenney_1-bit-pack/Tilesheet/tile_0454.png");
    
    std::cout << "[AssetManager] 1-Bit Pack loaded with " << textures.size() << " textures" << std::endl;
}

void AssetManager::switchPack(AssetPack pack) {
    if (pack == currentPack) {
        std::cout << "[AssetManager] Already using " << (pack == AssetPack::TinyDungeon ? "Tiny Dungeon" : "1-Bit") << " pack" << std::endl;
        return;
    }
    
    currentPack = pack;
    
    if (pack == AssetPack::TinyDungeon) {
        loadTinyDungeonPack();
    } else {
        loadOneBitPack();
    }
    
    std::cout << "[AssetManager] Switched to " << (pack == AssetPack::TinyDungeon ? "Tiny Dungeon (colorful)" : "1-Bit (monochrome)") << " pack" << std::endl;
}

void AssetManager::togglePack() {
    if (currentPack == AssetPack::TinyDungeon) {
        switchPack(AssetPack::OneBitPack);
    } else {
        switchPack(AssetPack::TinyDungeon);
    }
}
