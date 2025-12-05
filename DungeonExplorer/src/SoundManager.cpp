#include "SoundManager.h"
#include <iostream>
#include <algorithm>

SoundManager* SoundManager::instance = nullptr;

SoundManager::SoundManager() : masterVolume(50.0f), muted(false) {
    // Reserve some sound channels
    // SFML 3.0 requires a buffer for Sound constructor
    // We use a dummy buffer for initialization
    std::vector<std::int16_t> dummySamples = {0};
    // SFML 3.0: loadFromSamples requires channel map
    std::vector<sf::SoundChannel> channels = {sf::SoundChannel::Mono};
    dummyBuffer.loadFromSamples(dummySamples.data(), 1, 1, 44100, channels);

    for (int i = 0; i < SOUND_CHANNELS; i++) {
        sounds.push_back(std::make_unique<sf::Sound>(dummyBuffer));
    }
}

SoundManager& SoundManager::getInstance() {
    if (!instance) {
        instance = new SoundManager();
    }
    return *instance;
}

void SoundManager::initialize() {
    std::cout << "[SoundManager] Generating procedural retro sounds..." << std::endl;
    
    // 1. Footstep (Short noise burst)
    generateSound("step", SoundType::WhiteNoise, 0.05f, 0.0f, 0.8f);
    
    // 2. Attack Swing (Triangle wave slide)
    generateSound("swing", SoundType::TriangleWave, 0.15f, 300.0f, 0.5f);
    
    // 3. Hit/Damage (Explosion-like noise)
    generateHit("hit");
    
    // 4. Pickup (High pitch square wave)
    generatePowerup("pickup");
    
    // 5. UI Hover (Short high blip)
    generateSound("ui_hover", SoundType::SineWave, 0.05f, 800.0f, 0.1f);
    
    // 6. UI Click (Lower blip)
    generateSound("ui_click", SoundType::SquareWave, 0.1f, 400.0f, 0.2f);
    
    // 7. Level Up (Arpeggio-like sequence - simplified as a long powerup for now)
    generatePowerup("levelup"); 
    
    // 8. Door Open (Low rumble)
    generateSound("door", SoundType::WhiteNoise, 0.3f, 0.0f, 0.5f);
    
    // 9. Explosion (Longer noise)
    generateExplosion("explosion", 0.5f);
    
    std::cout << "[SoundManager] Audio initialized!" << std::endl;
}

void SoundManager::playSound(const std::string& name, float pitch, float volume) {
    if (muted) return;
    
    auto it = soundBuffers.find(name);
    if (it == soundBuffers.end()) {
        // std::cerr << "[SoundManager] Sound not found: " << name << std::endl;
        return;
    }
    
    // Find a free sound channel
    for (auto& sound : sounds) {
        // SFML 3.0: Status enum is scoped
        if (sound->getStatus() != sf::Sound::Status::Playing) {
            sound->setBuffer(it->second);
            sound->setPitch(pitch);
            sound->setVolume(std::min(100.0f, volume * (masterVolume / 100.0f)));
            sound->play();
            return;
        }
    }
    
    // If all channels busy, steal the oldest one (first one)
    if (!sounds.empty()) {
        sounds[0]->setBuffer(it->second);
        sounds[0]->setPitch(pitch);
        sounds[0]->setVolume(std::min(100.0f, volume * (masterVolume / 100.0f)));
        sounds[0]->play();
    }
}

void SoundManager::generateSound(const std::string& name, SoundType type, float duration, float frequency, float decay) {
    unsigned int sampleRate = 44100;
    unsigned int sampleCount = static_cast<unsigned int>(sampleRate * duration);
    std::vector<std::int16_t> samples(sampleCount);
    
    for (unsigned int i = 0; i < sampleCount; i++) {
        float time = static_cast<float>(i) / sampleRate;
        float value = 0.0f;
        
        switch (type) {
            case SoundType::SineWave:
                value = std::sin(TWO_PI * frequency * time);
                break;
            case SoundType::SquareWave:
                value = (std::sin(TWO_PI * frequency * time) > 0) ? 1.0f : -1.0f;
                break;
            case SoundType::TriangleWave:
                value = 2.0f * std::abs(2.0f * (time * frequency - std::floor(time * frequency + 0.5f))) - 1.0f;
                break;
            case SoundType::WhiteNoise:
                value = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f;
                break;
        }
        
        // Apply decay envelope
        float envelope = 1.0f;
        if (decay > 0.0f) {
            envelope = std::exp(-decay * time * ENVELOPE_MULT);
        }
        
        // Fade out at end to prevent clicking
        if (i > sampleCount - FADE_SAMPLES) {
            envelope *= static_cast<float>(sampleCount - i) / static_cast<float>(FADE_SAMPLES);
        }
        
        samples[i] = static_cast<std::int16_t>(value * MAX_AMPLITUDE * envelope);
    }
    
    sf::SoundBuffer buffer;
    std::vector<sf::SoundChannel> channels = {sf::SoundChannel::Mono};
    if (buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate, channels)) {
        soundBuffers[name] = buffer;
    }
}

void SoundManager::generateExplosion(const std::string& name, float duration) {
    unsigned int sampleRate = 44100;
    unsigned int sampleCount = static_cast<unsigned int>(sampleRate * duration);
    std::vector<std::int16_t> samples(sampleCount);
    
    for (unsigned int i = 0; i < sampleCount; i++) {
        float time = static_cast<float>(i) / sampleRate;
        
        // Noise with exponential decay
        float value = (static_cast<float>(std::rand()) / RAND_MAX) * 2.0f - 1.0f;
        float envelope = std::exp(-EXPLOSION_DECAY * time);
        
        samples[i] = static_cast<std::int16_t>(value * MAX_AMPLITUDE * envelope);
    }
    
    sf::SoundBuffer buffer;
    std::vector<sf::SoundChannel> channels = {sf::SoundChannel::Mono};
    if (buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate, channels)) {
        soundBuffers[name] = buffer;
    }
}

void SoundManager::generateHit(const std::string& name) {
    // Short low frequency noise/crunch
    generateSound(name, SoundType::WhiteNoise, 0.1f, 0.0f, 0.8f);
    
    // Post-process: Low pass filter effect (simple moving average)
    if (soundBuffers.find(name) != soundBuffers.end()) {
        // This is a simplified "crunch" generation
        // In a real synth we'd do more, but white noise with decay is decent for 8-bit hit
    }
}

void SoundManager::generatePowerup(const std::string& name) {
    // Arpeggio / Slide up
    unsigned int sampleRate = SAMPLE_RATE;
    float duration = POWERUP_DURATION;
    unsigned int sampleCount = static_cast<unsigned int>(sampleRate * duration);
    std::vector<std::int16_t> samples(sampleCount);
    
    for (unsigned int i = 0; i < sampleCount; i++) {
        float time = static_cast<float>(i) / sampleRate;
        
        // Frequency slide from 440Hz to 880Hz
        float frequency = POWERUP_BASE_FREQ + (POWERUP_BASE_FREQ * (time / duration));
        
        // Square wave
        float value = (std::sin(TWO_PI * frequency * time) > 0) ? 0.5f : -0.5f;
        
        // Add a second harmonic
        float value2 = (std::sin(TWO_PI * (frequency * HARMONIC_RATIO) * time) > 0) ? 0.5f : -0.5f;
        
        samples[i] = static_cast<std::int16_t>((value + value2) * 0.5f * MAX_AMPLITUDE);
    }
    
    sf::SoundBuffer buffer;
    std::vector<sf::SoundChannel> channels = {sf::SoundChannel::Mono};
    if (buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate, channels)) {
        soundBuffers[name] = buffer;
    }
}

void SoundManager::setVolume(float volume) {
    masterVolume = std::max(0.0f, std::min(100.0f, volume));
}

void SoundManager::toggleMute() {
    muted = !muted;
    if (muted) {
        for (auto& sound : sounds) {
            sound->stop();
        }
    }
}

void SoundManager::cleanup() {
    sounds.clear();
    soundBuffers.clear();
    delete instance;
    instance = nullptr;
}
