#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>
#include "../include/Dungeon.h"
#include "../include/AssetManager.h"

// Mock AssetManager to avoid loading real assets
// Actually, we can just load the real assets if we run from the right directory
// Or we can rely on the fact that Dungeon might not crash if textures fail to load?
// Dungeon constructor calls loadFromImage.
// Let's try to initialize AssetManager properly.

void testAStar() {
    std::cout << "Testing A* Pathfinding..." << std::endl;
    Dungeon dungeon;
    dungeon.generate(5); // Generate a small dungeon
    
    // Find a walkable start and end
    int startX = -1, startY = -1;
    int endX = -1, endY = -1;
    
    for (int y = 0; y < dungeon.getHeight(); y++) {
        for (int x = 0; x < dungeon.getWidth(); x++) {
            if (dungeon.isWalkable(x, y)) {
                if (startX == -1) {
                    startX = x; startY = y;
                } else if (abs(x - startX) + abs(y - startY) > 5) {
                    endX = x; endY = y;
                    break;
                }
            }
        }
        if (endX != -1) break;
    }
    
    if (startX == -1 || endX == -1) {
        std::cout << "Skipping A* test (could not find start/end points)" << std::endl;
        return;
    }
    
    std::cout << "Start: (" << startX << "," << startY << ") End: (" << endX << "," << endY << ")" << std::endl;
    
    auto nextMove = dungeon.findNextMoveToPlayerAStar(startX, startY, endX, endY);
    std::cout << "Next Move: (" << nextMove.first << "," << nextMove.second << ")" << std::endl;
    
    assert(dungeon.isWalkable(nextMove.first, nextMove.second));
    assert(nextMove.first != startX || nextMove.second != startY); // Should move
    
    std::cout << "A* Test Passed!" << std::endl;
}

void testRandom() {
    std::cout << "Testing Random Movement..." << std::endl;
    Dungeon dungeon;
    dungeon.generate(5);
    
    int startX = -1, startY = -1;
    for (int y = 0; y < dungeon.getHeight(); y++) {
        for (int x = 0; x < dungeon.getWidth(); x++) {
            if (dungeon.isWalkable(x, y)) {
                startX = x; startY = y;
                break;
            }
        }
        if (startX != -1) break;
    }
    
    auto nextMove = dungeon.findNextMoveRandom(startX, startY);
    std::cout << "Random Move from (" << startX << "," << startY << ") to (" << nextMove.first << "," << nextMove.second << ")" << std::endl;
    
    if (nextMove.first != startX || nextMove.second != startY) {
        assert(dungeon.isWalkable(nextMove.first, nextMove.second));
        assert(abs(nextMove.first - startX) + abs(nextMove.second - startY) == 1);
    }
    
    std::cout << "Random Test Passed!" << std::endl;
}

void testFlank() {
    std::cout << "Testing Flank Movement..." << std::endl;
    Dungeon dungeon;
    dungeon.generate(5);
    
    // Mock player and enemy positions
    // We need to find a large enough room to test flanking
    int startX = -1, startY = -1;
    // ... setup ...
    // For now, just basic validity check
    // Since we can't easily guarantee a specific layout, we just check if it returns a valid move
    
     for (int y = 0; y < dungeon.getHeight(); y++) {
        for (int x = 0; x < dungeon.getWidth(); x++) {
            if (dungeon.isWalkable(x, y)) {
                startX = x; startY = y;
                break;
            }
        }
        if (startX != -1) break;
    }
    
    auto nextMove = dungeon.findNextMoveFlank(startX, startY, startX + 3, startY); // Player 3 tiles away
    std::cout << "Flank Move: (" << nextMove.first << "," << nextMove.second << ")" << std::endl;
    
    if (nextMove.first != startX || nextMove.second != startY) {
        assert(dungeon.isWalkable(nextMove.first, nextMove.second));
    }
    
    std::cout << "Flank Test Passed!" << std::endl;
}

int main() {
    // Initialize AssetManager with dummy data if needed, or just rely on it not crashing
    // The Dungeon constructor loads textures. If we don't have a window, it might be fine?
    // SFML Texture loading usually requires an OpenGL context if loading from image? No, loading from file/memory is fine.
    // But we need to make sure assets exist.
    
    // We will run this from the project root so assets/ path is valid.
    try {
        AssetManager::getInstance().loadFromManifest("assets/data/kenney_manifest.json");
    } catch (...) {
        std::cout << "Warning: Failed to load assets. Tests might fail if Dungeon relies on them." << std::endl;
    }

    testAStar();
    testRandom();
    testFlank();
    
    std::cout << "All Logic Tests Passed!" << std::endl;
    return 0;
}
