#include "Game.h"
#include <iostream>
#include <exception>
#include <random>

// CHANGE: 2025-11-14 - Global random number generator with thread safety
// This replaces scattered std::srand() calls throughout codebase
thread_local std::mt19937 g_rng(std::random_device{}());

int main() {
    try {
        std::cout << "Starting Dungeon Explorer..." << std::endl;
        std::cout << "[Initialization] Random number generator initialized" << std::endl;
        
        Game game;
        game.run();
        
        std::cout << "Game ended successfully." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "[FATAL ERROR] Unknown exception occurred - terminating" << std::endl;
        return 1;
    }
}
