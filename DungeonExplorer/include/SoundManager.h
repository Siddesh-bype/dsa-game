#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <random>
#include <cstdint>

enum class SoundType {
    SquareWave,
    TriangleWave,
    WhiteNoise,
    SineWave
};

class SoundManager {
private:
    // Singleton instance
    static SoundManager* instance;
    
    // Dummy buffer for initialization (SFML 3.0 requirement)
    sf::SoundBuffer dummyBuffer;
    
    // Sound buffers hold the audio data
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    
    // Sounds are the playable instances
    std::vector<std::unique_ptr<sf::Sound>> sounds;
    
    // Volume settings
    float masterVolume;
    bool muted;

    // Private constructor for Singleton
    SoundManager();
    
    // ═══════════════════════════════════════════════════════════════════════
    // AUDIO CONFIGURATION CONSTANTS
    // ═══════════════════════════════════════════════════════════════════════
    static constexpr unsigned int SAMPLE_RATE = 44100;
    static constexpr int SOUND_CHANNELS = 16;
    static constexpr float PI = 3.14159f;
    static constexpr float TWO_PI = 6.28318f;
    static constexpr int MAX_AMPLITUDE = 32767;
    static constexpr int FADE_SAMPLES = 100;
    static constexpr float EXPLOSION_DECAY = 5.0f;
    static constexpr float ENVELOPE_MULT = 10.0f;
    static constexpr float POWERUP_BASE_FREQ = 440.0f;
    static constexpr float POWERUP_DURATION = 0.4f;
    static constexpr float HARMONIC_RATIO = 1.5f;

public:
    static SoundManager& getInstance();
    
    void initialize();
    
    // Play a sound by name
    void playSound(const std::string& name, float pitch = 1.0f, float volume = 100.0f);
    
    // Procedural generation helpers
    void generateSound(const std::string& name, SoundType type, float duration, float frequency, float decay = 0.0f);
    void generateExplosion(const std::string& name, float duration);
    void generatePowerup(const std::string& name);
    void generateHit(const std::string& name);
    void generateJump(const std::string& name);
    
    // Settings
    void setVolume(float volume);
    void toggleMute();
    bool isMuted() const { return muted; }
    
    // Cleanup
    void cleanup();
};
