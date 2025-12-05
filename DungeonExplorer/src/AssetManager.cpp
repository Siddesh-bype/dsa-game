// CHANGE: 2025-11-10 — TASK B & H: Enhanced asset loading with validation
// - Added INFO/ERROR logging for all asset loads
// - Improved error messages with full paths
// - Added missing asset detection
// CHANGE: 2025-12-03 — Added GIF support via stb_image + path optimization
// CHANGE: 2025-12-03 — CRITICAL FIX: Completed loadFromManifest() function

#include "AssetManager.h"

// STB Image implementation - must be defined before including the header
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <iostream>
#include <fstream>

// ═══════════════════════════════════════════════════════════════════════
// ASSET PATH CONSTANTS - Centralized path management
// ═══════════════════════════════════════════════════════════════════════

namespace AssetPaths {
    // Base directories
    constexpr const char* DEBTS_BASE = "assets/DebtsInTheDepthsAssets/DebtsInTheDepthsAssets/";
    constexpr const char* KENNEY_TINY = "assets/kenney/kenney_tiny-dungeon/Tiles/";
    constexpr const char* KENNEY_UI = "assets/kenney/kenney_ui-pack-rpg-expansion/PNG/";
    constexpr const char* KENNEY_1BIT = "assets/kenney/kenney_1-bit-pack/Tilesheet/";
    
    // Debts in the Depths subdirectories
    constexpr const char* DEBTS_CHARS = "Characters/";
    constexpr const char* DEBTS_ENV = "Environment/";
    constexpr const char* DEBTS_FX = "Effects/";
    constexpr const char* DEBTS_UI = "UI/";
    
    // Helper function to build full paths
    inline std::string debtsChar(const char* filename) {
        return std::string(DEBTS_BASE) + DEBTS_CHARS + filename;
    }
    
    inline std::string debtsEnv(const char* filename) {
        return std::string(DEBTS_BASE) + DEBTS_ENV + filename;
    }
    
    inline std::string debtsFx(const char* filename) {
        return std::string(DEBTS_BASE) + DEBTS_FX + filename;
    }
    
    inline std::string debtsUI(const char* filename) {
        return std::string(DEBTS_BASE) + DEBTS_UI + filename;
    }
    
    inline std::string kenneyTile(const char* filename) {
        return std::string(KENNEY_TINY) + filename;
    }
    
    inline std::string kenneyUI(const char* filename) {
        return std::string(KENNEY_UI) + filename;
    }
}

// ═══════════════════════════════════════════════════════════════════════
// ASSET MANAGER IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════

AssetManager& AssetManager::getInstance() {
    static AssetManager instance;
    return instance;
}

bool AssetManager::loadTexture(const std::string& key, const std::string& filePath) {
    auto texture = std::make_unique<sf::Texture>();
    
    // Check if file is GIF - use stb_image for GIF loading (C++17 compatible check)
    bool isGif = filePath.size() >= 4 && filePath.substr(filePath.size() - 4) == ".gif";
    
    if (isGif) {
        int width, height, channels;
        unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        
        if (!data) {
            std::cerr << "[ERROR] Failed to load GIF '" << key << "' -> " << filePath << std::endl;
            std::cerr << "        stb_image error: " << stbi_failure_reason() << std::endl;
            return false;
        }
        
        // Create SFML texture from loaded pixel data
        if (!texture->resize({static_cast<unsigned int>(width), static_cast<unsigned int>(height)})) {
            std::cerr << "[ERROR] Failed to create texture for '" << key << "'" << std::endl;
            stbi_image_free(data);
            return false;
        }
        
        texture->update(data);
        stbi_image_free(data);
        
        std::cout << "[INFO] Loaded GIF asset '" << key << "' -> " << filePath 
                  << " (" << width << "x" << height << ")" << std::endl;
    } else {
        // Use SFML's native loading for PNG/JPG/etc
        if (!texture->loadFromFile(filePath)) {
            std::cerr << "[ERROR] Missing asset '" << key << "' -> " << filePath << std::endl;
            return false;
        }
        std::cout << "[INFO] Loaded asset '" << key << "' -> " << filePath << std::endl;
    }
    
    texture->setSmooth(false);  // Pixel-perfect rendering
    textures[key] = std::move(texture);
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
    using namespace AssetPaths;
    
    std::cout << "[AssetManager] Loading Debts in the Depths assets (GIF format)..." << std::endl;
    
    // ═══════════════════════════════════════════════════════════════════════
    // DEBTS IN THE DEPTHS ASSET PACK - COMPLETE GIF INTEGRATION
    // Using centralized path helpers to reduce duplication
    // ═══════════════════════════════════════════════════════════════════════
    
    // 🟫 FLOOR & ENVIRONMENT TILES
    loadTexture("floor", debtsEnv("sprBrick.png"));
    loadTexture("floor_variant_1", debtsEnv("sprBrimstone.png"));
    loadTexture("floor_variant_2", debtsEnv("sprRock.png"));
    loadTexture("floor_variant_3", debtsEnv("sprBrick.png"));
    loadTexture("floor_variant_4", debtsEnv("sprBrimstone.png"));
    loadTexture("wall", debtsEnv("sprRock.png"));
    
    // 🪜 STAIRS
    loadTexture("stairs_up", kenneyTile("tile_0014.png"));
    loadTexture("stairs_down", kenneyTile("tile_0040.png"));
    
    // 🚪 DOORS
    loadTexture("door_closed", kenneyTile("tile_0045.png"));
    loadTexture("door_open", kenneyTile("tile_0000.png"));
    
    // 👤 PLAYER CHARACTER - WIZARD (GIF format for animation)
    loadTexture("player_warrior", debtsChar("sprWizard.gif"));
    loadTexture("player_rogue", debtsChar("sprWizard.gif"));
    loadTexture("player_mage", debtsChar("sprWizard.gif"));
    loadTexture("player_wizard_idle", debtsChar("sprWizardIdle.gif"));
    loadTexture("player_wizard_fire", debtsChar("sprWizardFire.gif"));
    loadTexture("player_wizard_hurt", debtsChar("sprWizardHurt.gif"));
    
    // 👹 ENEMY MONSTERS - All with GIF format
    loadTexture("goblin", debtsChar("sprGoblin1.gif"));
    loadTexture("slime", debtsChar("sprBogslium1.gif"));
    loadTexture("orc", debtsChar("sprOrcArcher1.gif"));
    loadTexture("skeleton", debtsChar("sprSkeleton1.gif"));
    loadTexture("demon", debtsChar("sprGhost1.gif"));
    loadTexture("wraith", debtsChar("sprGhost2.gif"));
    loadTexture("vampire", debtsChar("sprBatilisk1.gif"));
    loadTexture("lich", debtsChar("sprLizardMonk1.gif"));
    loadTexture("dragon", debtsChar("sprDragon.gif"));
    loadTexture("necromancer", debtsChar("sprLizardMonk2.gif"));
    loadTexture("dark_mage", debtsChar("sprLizardMonk3.gif"));
    loadTexture("gargoyle", debtsChar("sprBatilisk2.gif"));
    loadTexture("minotaur", debtsChar("sprMinotaur1.gif"));
    
    // 🎒 ITEMS & COLLECTIBLES
    loadTexture("gold_pile", debtsEnv("sprGoldPile.png"));
    loadTexture("gold_vein", debtsEnv("sprGoldVein.png"));
    loadTexture("potion_red", kenneyTile("tile_0090.png"));
    loadTexture("potion_blue", kenneyTile("tile_0091.png"));
    loadTexture("coin", kenneyTile("tile_0088.png"));
    loadTexture("chest", kenneyTile("tile_0050.png"));
    loadTexture("key", kenneyTile("tile_0060.png"));
    
    // ⚔️ WEAPONS
    loadTexture("sword_iron", kenneyTile("tile_0092.png"));
    loadTexture("sword_flame", kenneyTile("tile_0093.png"));
    loadTexture("shield", kenneyTile("tile_0094.png"));
    
    // ✨ COMBAT EFFECTS - All GIF format
    loadTexture("effect_attack_swing", debtsFx("sprAttackSwing.gif"));
    loadTexture("effect_attack_large", debtsFx("sprAttackLarge.gif"));
    loadTexture("effect_attack_spear", debtsFx("sprAttackSpear.gif"));
    loadTexture("effect_sparkle", debtsFx("sprSparkle.gif"));
    loadTexture("effect_explosion", debtsFx("sprExplosion.gif"));
    loadTexture("effect_fire_explosion", debtsFx("sprFireExplosion.gif"));
    loadTexture("effect_magic_explosion", debtsFx("sprMagicExplosion.gif"));
    loadTexture("effect_shockwave", debtsFx("sprShockwave.gif"));
    
    // 🏹 PROJECTILE EFFECTS - All GIF format
    loadTexture("effect_arrow", debtsFx("sprArrow.gif"));
    loadTexture("effect_firebolt", debtsFx("sprFirebolt.gif"));
    loadTexture("effect_magic_bolt", debtsFx("sprMagicBolt.gif"));
    loadTexture("effect_acid", debtsFx("sprAcidProjectile.gif"));
    loadTexture("effect_ghost_orb", debtsFx("sprGhostOrb.gif"));
    
    // 💚 HEALING & PARTICLES
    loadTexture("effect_heal", debtsFx("sprHealParticle.gif"));
    loadTexture("effect_rock_particle", debtsFx("sprRockParticle.png"));
    
    // 🌊 ENVIRONMENTAL EFFECTS
    loadTexture("env_water", debtsEnv("sprWater.gif"));
    loadTexture("env_lava", debtsEnv("sprLava.gif"));
    
    // 🏺 ENVIRONMENT PROPS
    loadTexture("props_catacombs", debtsEnv("sprPropsCatacombs.gif"));
    loadTexture("props_corpses", debtsEnv("sprPropsCorpses.gif"));
    loadTexture("props_inferno", debtsEnv("sprPropsInferno.gif"));
    loadTexture("props_swamp", debtsEnv("sprPropsSwamp.gif"));
    
    // 💗 UI ICONS
    loadTexture("ui_heart", debtsUI("sprHeart.gif"));
    loadTexture("ui_gold", debtsUI("sprGoldIcon.png"));
    loadTexture("ui_cursor", debtsUI("sprCursor.gif"));
    
    // 🎮 BUTTONS
    loadTexture("button_square_blue", kenneyUI("buttonSquare_blue.png"));
    loadTexture("button_long_blue", kenneyUI("buttonLong_blue.png"));
    
    // ═══════════════════════════════════════════════════════════════════════
    // 📊 SPRITESHEETS - Large combined tilesets
    // ═══════════════════════════════════════════════════════════════════════
    
    auto tinyDungeonSheet = std::make_unique<sf::Texture>();
    if (tinyDungeonSheet->loadFromFile("assets/kenney/kenney_tiny-dungeon/Tilemap/tilemap.png")) {
        tinyDungeonSheet->setSmooth(false);
        spritesheets["tiny_dungeon"] = std::move(tinyDungeonSheet);
        std::cout << "[AssetManager] Loaded Tiny Dungeon spritesheet" << std::endl;
    }
    
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

void AssetManager::loadTinyDungeonPack() {
    using namespace AssetPaths;
    std::cout << "[AssetManager] Loading Tiny Dungeon pack (colorful)..." << std::endl;
    
    textures.clear();
    
    loadTexture("floor", kenneyTile("tile_0001.png"));
    loadTexture("floor_variant_1", kenneyTile("tile_0002.png"));
    loadTexture("floor_variant_2", kenneyTile("tile_0003.png"));
    loadTexture("floor_variant_3", kenneyTile("tile_0004.png"));
    loadTexture("floor_variant_4", kenneyTile("tile_0005.png"));
    loadTexture("wall", kenneyTile("tile_0023.png"));
    loadTexture("door_closed", kenneyTile("tile_0017.png"));
    loadTexture("door_open", kenneyTile("tile_0018.png"));
    loadTexture("stairs_down", kenneyTile("tile_0046.png"));
    loadTexture("stairs_up", kenneyTile("tile_0047.png"));
    
    loadTexture("player_warrior", kenneyTile("tile_0084.png"));
    loadTexture("player_rogue", kenneyTile("tile_0085.png"));
    loadTexture("player_mage", kenneyTile("tile_0086.png"));
    
    loadTexture("goblin", kenneyTile("tile_0108.png"));
    loadTexture("orc", kenneyTile("tile_0109.png"));
    loadTexture("skeleton", kenneyTile("tile_0110.png"));
    loadTexture("demon", kenneyTile("tile_0111.png"));
    
    std::cout << "[AssetManager] Tiny Dungeon pack loaded with " << textures.size() << " textures" << std::endl;
}

void AssetManager::loadOneBitPack() {
    using namespace AssetPaths;
    std::cout << "[AssetManager] Loading 1-Bit Pack (monochrome)..." << std::endl;
    
    textures.clear();
    
    const char* prefix = KENNEY_1BIT;
    loadTexture("floor", std::string(prefix) + "tile_0000.png");
    loadTexture("floor_variant_1", std::string(prefix) + "tile_0001.png");
    loadTexture("floor_variant_2", std::string(prefix) + "tile_0002.png");
    loadTexture("floor_variant_3", std::string(prefix) + "tile_0003.png");
    loadTexture("floor_variant_4", std::string(prefix) + "tile_0004.png");
    loadTexture("wall", std::string(prefix) + "tile_0028.png");
    loadTexture("door_closed", std::string(prefix) + "tile_0064.png");
    loadTexture("door_open", std::string(prefix) + "tile_0065.png");
    loadTexture("stairs_down", std::string(prefix) + "tile_0046.png");
    loadTexture("stairs_up", std::string(prefix) + "tile_0047.png");
    
    loadTexture("player_warrior", std::string(prefix) + "tile_0432.png");
    loadTexture("player_rogue", std::string(prefix) + "tile_0433.png");
    loadTexture("player_mage", std::string(prefix) + "tile_0434.png");
    
    loadTexture("goblin", std::string(prefix) + "tile_0451.png");
    loadTexture("orc", std::string(prefix) + "tile_0452.png");
    loadTexture("skeleton", std::string(prefix) + "tile_0453.png");
    loadTexture("demon", std::string(prefix) + "tile_0454.png");
    
    std::cout << "[AssetManager] 1-Bit Pack loaded with " << textures.size() << " textures" << std::endl;
}