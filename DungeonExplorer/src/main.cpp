// main.cpp - Dungeon Explorer Entry Point
// CHANGE: 2025-11-14 - Global thread-safe random number generator
// CHANGE: 2025-12-07 - Removed debug output, improved structure

#include "Game.h"
#include <cstdlib>
#include <exception>
#include <random>

// ═══════════════════════════════════════════════════════════════════════════
// GLOBAL RANDOM NUMBER GENERATOR (Thread-safe Mersenne Twister)
// Replaces scattered std::srand() calls throughout codebase
// ═══════════════════════════════════════════════════════════════════════════
thread_local std::mt19937 g_rng(std::random_device{}());

namespace {
    constexpr int EXIT_SUCCESS_CODE = 0;
    constexpr int EXIT_FAILURE_CODE = 1;
}

int main() {
    try {
        Game game;
        game.run();
        return EXIT_SUCCESS_CODE;
        
    } catch (const std::exception& e) {
        // Log fatal errors to stderr (keeping for crash diagnostics)
        std::fprintf(stderr, "[FATAL ERROR] %s\n", e.what());
        return EXIT_FAILURE_CODE;
        
    } catch (...) {
        std::fprintf(stderr, "[FATAL ERROR] Unknown exception occurred\n");
        return EXIT_FAILURE_CODE;
    }
}
