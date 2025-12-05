// SettingsManager.h - Persistent game settings
#pragma once
#include <string>
#include <nlohmann/json.hpp>

class SettingsManager {
public:
    static SettingsManager& getInstance() {
        static SettingsManager instance;
        return instance;
    }
    
    // Settings access
    float getMasterVolume() const { return masterVolume; }
    float getSFXVolume() const { return sfxVolume; }
    float getMusicVolume() const { return musicVolume; }
    int getGraphicsQuality() const { return graphicsQuality; }
    int getDifficulty() const { return difficulty; }
    bool isFullscreen() const { return fullscreen; }
    
    // Settings modification
    void setMasterVolume(float volume);
    void setSFXVolume(float volume);
    void setMusicVolume(float volume);
    void setGraphicsQuality(int quality);  // 0=Low, 1=Medium, 2=High
    void setDifficulty(int diff);  // 0=Easy, 1=Normal, 2=Hard
    void setFullscreen(bool enabled);
    
    // Persistence
    bool loadSettings();
    bool saveSettings();
    void resetToDefaults();
    
private:
    SettingsManager();
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;
    
    // Settings values
    float masterVolume = 1.0f;
    float sfxVolume = 0.8f;
    float musicVolume = 0.6f;
    int graphicsQuality = 1;  // Medium
    int difficulty = 1;  // Normal
    bool fullscreen = false;
    
    const std::string settingsFile = "settings.json";
    
    nlohmann::json serialize() const;
    void deserialize(const nlohmann::json& j);
};
