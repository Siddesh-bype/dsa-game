#include "Game.h"
#include <iostream>
#include <exception>

int main() {
    try {
        std::cout << "Starting Dungeon Explorer..." << std::endl;
        
        Game game;
        game.run();
        
        std::cout << "Game ended successfully." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
