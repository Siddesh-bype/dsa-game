#include "TilesetReader.h"
#include <fstream>
#include <iostream>
#include <sstream>

std::string TilesetReader::getAttribute(const std::string& line, const std::string& attr) {
    std::string key = attr + "=\"";
    size_t start = line.find(key);
    if (start == std::string::npos) return "";
    
    start += key.length();
    size_t end = line.find("\"", start);
    if (end == std::string::npos) return "";
    
    return line.substr(start, end - start);
}

TilesetData TilesetReader::parseTSX(const std::string& filePath) {
    TilesetData data = {};
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        std::cerr << "[TilesetReader] Error: Could not open file " << filePath << std::endl;
        return data;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Parse <tileset> tag
        if (line.find("<tileset") != std::string::npos) {
            std::string name = getAttribute(line, "name");
            if (!name.empty()) data.name = name;
            
            std::string tw = getAttribute(line, "tilewidth");
            if (!tw.empty()) data.tileWidth = std::stoi(tw);
            
            std::string th = getAttribute(line, "tileheight");
            if (!th.empty()) data.tileHeight = std::stoi(th);
            
            std::string sp = getAttribute(line, "spacing");
            if (!sp.empty()) data.spacing = std::stoi(sp);
            
            std::string mg = getAttribute(line, "margin");
            if (!mg.empty()) data.margin = std::stoi(mg);
            
            std::string cols = getAttribute(line, "columns");
            if (!cols.empty()) data.columns = std::stoi(cols);
            
            std::string tc = getAttribute(line, "tilecount");
            if (!tc.empty()) data.tileCount = std::stoi(tc);
        }
        
        // Parse <image> tag
        if (line.find("<image") != std::string::npos) {
            std::string src = getAttribute(line, "source");
            if (!src.empty()) data.imageSource = src;
            
            std::string w = getAttribute(line, "width");
            if (!w.empty()) data.imageWidth = std::stoi(w);
            
            std::string h = getAttribute(line, "height");
            if (!h.empty()) data.imageHeight = std::stoi(h);
        }
    }
    
    std::cout << "[TilesetReader] Parsed " << filePath << ": " 
              << data.name << " (" << data.tileWidth << "x" << data.tileHeight << ")" << std::endl;
              
    return data;
}
