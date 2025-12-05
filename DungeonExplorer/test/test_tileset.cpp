#include <iostream>
#include <fstream>
#include <cassert>
#include "../include/TilesetReader.h"

// Simple test framework
#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion failed: " << (message) << "\n" \
                      << "File: " << __FILE__ << ", Line: " << __LINE__ << "\n"; \
            std::exit(1); \
        } \
    } while (0)

void create_dummy_tsx(const std::string& filename) {
    std::ofstream file(filename);
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<tileset version=\"1.10\" tiledversion=\"1.10.2\" name=\"dungeon_test\" tilewidth=\"16\" tileheight=\"16\" spacing=\"1\" tilecount=\"100\" columns=\"10\">\n";
    file << " <image source=\"tiles.png\" width=\"170\" height=\"170\"/>\n";
    file << "</tileset>\n";
    file.close();
}

int main() {
    std::cout << "Running TilesetReader Tests..." << std::endl;
    
    std::string filename = "test_tileset.tsx";
    create_dummy_tsx(filename);
    
    TilesetData data = TilesetReader::parseTSX(filename);
    
    ASSERT(data.name == "dungeon_test", "Name mismatch");
    ASSERT(data.tileWidth == 16, "TileWidth mismatch");
    ASSERT(data.tileHeight == 16, "TileHeight mismatch");
    ASSERT(data.spacing == 1, "Spacing mismatch");
    ASSERT(data.columns == 10, "Columns mismatch");
    ASSERT(data.tileCount == 100, "TileCount mismatch");
    ASSERT(data.imageSource == "tiles.png", "ImageSource mismatch");
    ASSERT(data.imageWidth == 170, "ImageWidth mismatch");
    ASSERT(data.imageHeight == 170, "ImageHeight mismatch");
    
    std::cout << "All Tests Passed!" << std::endl;
    
    // Cleanup
    std::remove(filename.c_str());
    
    return 0;
}
