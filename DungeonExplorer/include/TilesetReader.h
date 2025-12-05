#pragma once
#include <string>
#include <vector>
#include <map>

struct TilesetData {
    std::string name;
    int tileWidth;
    int tileHeight;
    int spacing;
    int margin;
    int columns;
    int tileCount;
    std::string imageSource;
    int imageWidth;
    int imageHeight;
};

class TilesetReader {
public:
    static TilesetData parseTSX(const std::string& filePath);
    
private:
    static std::string getAttribute(const std::string& line, const std::string& attr);
};
