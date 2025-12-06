// SettingsManager.cpp - Game settings persistence and management
#include "SettingsManager.h"
#include <fstream>
#include <algorithm>

// ═══════════════════════════════════════════════════════════════════════════
// CONFIGURATION CONSTANTS
// ═══════════════════════════════════════════════════════════════════════════

namespace {
    // Volume defaults
    constexpr float DEFAULT_MASTER_VOLUME = 1.0f;
    constexpr float DEFAULT_SFX_VOLUME = 0.8f;
    constexpr float DEFAULT_MUSIC_VOLUME = 0.6f;
    
    // Quality defaults
    constexpr int DEFAULT_GRAPHICS_QUALITY = 1;  // 0=Low, 1=Medium, 2=High
    constexpr int DEFAULT_DIFFICULTY = 1;        // 0=Easy, 1=Normal, 2=Hard
    
    // Bounds
    constexpr float VOLUME_MIN = 0.0f;
    constexpr float VOLUME_MAX = 1.0f;
    constexpr int QUALITY_MIN = 0;
    constexpr int QUALITY_MAX = 2;
}

// ═══════════════════════════════════════════════════════════════════════════
// CONSTRUCTOR
// ═══════════════════════════════════════════════════════════════════════════

SettingsManager::SettingsManager() {
    if (!loadSettings()) {
        resetToDefaults();
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// SETTERS WITH VALIDATION
// ═══════════════════════════════════════════════════════════════════════════

void SettingsManager::setMasterVolume(float volume) {
    masterVolume = std::clamp(volume, VOLUME_MIN, VOLUME_MAX);
}

void SettingsManager::setSFXVolume(float volume) {
    sfxVolume = std::clamp(volume, VOLUME_MIN, VOLUME_MAX);
}

void SettingsManager::setMusicVolume(float volume) {
    musicVolume = std::clamp(volume, VOLUME_MIN, VOLUME_MAX);
}

void SettingsManager::setGraphicsQuality(int quality) {
    graphicsQuality = std::clamp(quality, QUALITY_MIN, QUALITY_MAX);
}

void SettingsManager::setDifficulty(int diff) {
    difficulty = std::clamp(diff, QUALITY_MIN, QUALITY_MAX);
}

void SettingsManager::setFullscreen(bool enabled) {
    fullscreen = enabled;
}

// ═══════════════════════════════════════════════════════════════════════════
// PERSISTENCE
// ═══════════════════════════════════════════════════════════════════════════

bool SettingsManager::loadSettings() {
    std::ifstream file(settingsFile);
    if (!file.is_open()) return false;

    try {
        nlohmann::json j;
        file >> j;
        deserialize(j);
        return true;
    } catch (...) {
        return false;
    }
}

bool SettingsManager::saveSettings() {
    std::ofstream file(settingsFile);
    if (!file.is_open()) return false;

    const nlohmann::json j = serialize();
    file << j.dump(4);
    return true;
}

void SettingsManager::resetToDefaults() {
    masterVolume = DEFAULT_MASTER_VOLUME;
    sfxVolume = DEFAULT_SFX_VOLUME;
    musicVolume = DEFAULT_MUSIC_VOLUME;
    graphicsQuality = DEFAULT_GRAPHICS_QUALITY;
    difficulty = DEFAULT_DIFFICULTY;
    fullscreen = false;
}

// ═══════════════════════════════════════════════════════════════════════════
// SERIALIZATION
// ═══════════════════════════════════════════════════════════════════════════

nlohmann::json SettingsManager::serialize() const {
    nlohmann::json j;
    j["masterVolume"] = masterVolume;
    j["sfxVolume"] = sfxVolume;
    j["musicVolume"] = musicVolume;
    j["graphicsQuality"] = graphicsQuality;
    j["difficulty"] = difficulty;
    j["fullscreen"] = fullscreen;
    return j;
}

void SettingsManager::deserialize(const nlohmann::json& j) {
    if (j.contains("masterVolume")) masterVolume = j["masterVolume"];
    if (j.contains("sfxVolume")) sfxVolume = j["sfxVolume"];
    if (j.contains("musicVolume")) musicVolume = j["musicVolume"];
    if (j.contains("graphicsQuality")) graphicsQuality = j["graphicsQuality"];
    if (j.contains("difficulty")) difficulty = j["difficulty"];
    if (j.contains("fullscreen")) fullscreen = j["fullscreen"];
}

