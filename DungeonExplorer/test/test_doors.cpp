#include <iostream>
#include <cassert>
#include <vector>
#include "../include/Dungeon.h"
#include "../include/AssetManager.h"

#include <random>
#include <chrono>

// Define the global RNG needed by Dungeon.cpp
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
thread_local std::mt19937 g_rng(seed);

// Simple test framework
#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion failed: " << (message) << "\n" \
                      << "File: " << __FILE__ << ", Line: " << __LINE__ << "\n"; \
            std::exit(1); \
        } \
    } while (0)

void test_door_generation() {
    std::cout << "Testing Door Generation..." << std::endl;
    Dungeon dungeon;
    int attempts = 0;
    do {
        dungeon.generate(8); // Use more rooms to increase door chance
        attempts++;
    } while (dungeon.getDoors().empty() && attempts < 10);
    
    const auto& doors = dungeon.getDoors();
    ASSERT(!doors.empty(), "Dungeon should have doors after multiple attempts");
    
    for (const auto& door : doors) {
        ASSERT(dungeon.getTile(door.x, door.y) == TileType::Door, "Door position should have Door tile type");
        ASSERT(!door.isOpen, "Doors should be closed initially");
    }
    std::cout << "Passed!" << std::endl;
}

void test_door_opening() {
    std::cout << "Testing Door Opening..." << std::endl;
    Dungeon dungeon;
    int attempts = 0;
    do {
        dungeon.generate(8);
        attempts++;
    } while (dungeon.getDoors().empty() && attempts < 10);
    
    const auto& doors = dungeon.getDoors();
    if (doors.empty()) {
        std::cerr << "Failed to generate dungeon with doors for opening test" << std::endl;
        std::exit(1);
    }
    
    // Pick first door
    const DoorData& door = doors[0];
    int x = door.x;
    int y = door.y;
    
    ASSERT(!door.isOpen, "Door should be closed initially");
    
    // Open door
    dungeon.openDoor(x, y);
    
    const DoorData* updatedDoor = dungeon.getDoorAt(x, y);
    ASSERT(updatedDoor->isOpen, "Door should be open after openDoor()");
    ASSERT(dungeon.getTile(x, y) == TileType::Door, "Tile should remain Door type (visuals handled in render)");
    
    std::cout << "Passed!" << std::endl;
}

void test_key_requirement() {
    std::cout << "Testing Key Requirement Probability..." << std::endl;
    int totalDoors = 0;
    int keyDoors = 0;
    
    // Generate multiple dungeons to get enough sample size
    for (int i = 0; i < 100; i++) {
        Dungeon dungeon;
        dungeon.generate(8);
        const auto& doors = dungeon.getDoors();
        totalDoors += doors.size();
        for (const auto& door : doors) {
            if (door.requiresKey) {
                keyDoors++;
            }
        }
    }
    
    std::cout << "Total Doors: " << totalDoors << ", Key Doors: " << keyDoors << std::endl;
    
    if (totalDoors > 0) {
        float ratio = (float)keyDoors / totalDoors;
        std::cout << "Key Door Ratio: " << ratio << std::endl;
        // Expect roughly 20% (0.2)
        // Allow wide margin due to randomness
        ASSERT(ratio >= 0.05f && ratio <= 0.45f, "Key door ratio should be around 20%");
    }
    
    std::cout << "Passed!" << std::endl;
}

void test_rng() {
    std::cout << "Testing RNG..." << std::endl;
    std::uniform_int_distribution<int> dist(1, 100);
    int hits = 0;
    for(int i=0; i<1000; i++) {
        if(dist(g_rng) <= 20) hits++;
    }
    std::cout << "RNG Test: " << hits << "/1000 (Expected ~200)" << std::endl;
    ASSERT(hits > 150 && hits < 250, "RNG distribution seems off");
}

int main() {
    std::cout << "Running Door System Tests..." << std::endl;
    
    test_rng();
    test_door_generation();
    test_door_opening();
    test_key_requirement();
    
    std::cout << "All Tests Passed!" << std::endl;
    return 0;
}
