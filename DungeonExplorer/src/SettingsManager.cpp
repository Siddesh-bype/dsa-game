// SettingsManager.cpp - Implementation of SettingsManager
#include "SettingsManager.h"
#include <fstream>
#include <iostream>

SettingsManager::SettingsManager() {
    if (!loadSettings()) {
        resetToDefaults();
    }
}

void SettingsManager::setMasterVolume(float volume) {
    masterVolume = std::max(0.0f, std::min(1.0f, volume));
}

void SettingsManager::setSFXVolume(float volume) {
    sfxVolume = std::max(0.0f, std::min(1.0f, volume));
}

void SettingsManager::setMusicVolume(float volume) {
    musicVolume = std::max(0.0f, std::min(1.0f, volume));
}

void SettingsManager::setGraphicsQuality(int quality) {
    graphicsQuality = std::max(0, std::min(2, quality));
}

void SettingsManager::setDifficulty(int diff) {
    difficulty = std::max(0, std::min(2, diff));
}

void SettingsManager::setFullscreen(bool enabled) {
    fullscreen = enabled;
}

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

    nlohmann::json j = serialize();
    file << j.dump(4);
    return true;
}

void SettingsManager::resetToDefaults() {
    masterVolume = 1.0f;
    sfxVolume = 0.8f;
    musicVolume = 0.6f;
    graphicsQuality = 1;
    difficulty = 1;
    fullscreen = false;
}

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
