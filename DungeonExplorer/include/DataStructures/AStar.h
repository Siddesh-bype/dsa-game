#pragma once
#include "PriorityQueue.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <functional>
#include <iostream>
#include <algorithm>

// ═══════════════════════════════════════════════════════════════════════════
// A* PATHFINDING ALGORITHM (DSA: Graph Search Algorithm)
// Time Complexity: O(E log V) where E = edges, V = vertices
// Space Complexity: O(V) for the open/closed sets
// Used for: Smart enemy navigation around obstacles
// ═══════════════════════════════════════════════════════════════════════════

struct PathNode {
    int x, y;
    
    PathNode(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator==(const PathNode& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const PathNode& other) const {
        return !(*this == other);
    }
};

// Hash function for PathNode (for unordered_set/map)
struct PathNodeHash {
    size_t operator()(const PathNode& node) const {
        return std::hash<int>()(node.x) ^ (std::hash<int>()(node.y) << 16);
    }
};

class AStarPathfinder {
public:
    using IsWalkableFunc = std::function<bool(int, int)>;
    
private:
    int gridWidth, gridHeight;
    IsWalkableFunc isWalkable;
    
    // Heuristic: Manhattan distance (admissible for grid-based movement)
    float heuristic(const PathNode& a, const PathNode& b) const {
        return static_cast<float>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
    }
    
    // Get valid neighbors (4-directional or 8-directional)
    std::vector<PathNode> getNeighbors(const PathNode& node, bool allowDiagonal = false) const {
        std::vector<PathNode> neighbors;
        
        // 4-directional movement
        const int dx4[] = {0, 1, 0, -1};
        const int dy4[] = {-1, 0, 1, 0};
        
        for (int i = 0; i < 4; ++i) {
            int nx = node.x + dx4[i];
            int ny = node.y + dy4[i];
            
            if (nx >= 0 && nx < gridWidth && ny >= 0 && ny < gridHeight) {
                if (isWalkable(nx, ny)) {
                    neighbors.emplace_back(nx, ny);
                }
            }
        }
        
        // 8-directional movement (diagonals)
        if (allowDiagonal) {
            const int dxDiag[] = {1, 1, -1, -1};
            const int dyDiag[] = {-1, 1, 1, -1};
            
            for (int i = 0; i < 4; ++i) {
                int nx = node.x + dxDiag[i];
                int ny = node.y + dyDiag[i];
                
                if (nx >= 0 && nx < gridWidth && ny >= 0 && ny < gridHeight) {
                    // Only allow diagonal if both adjacent cells are walkable
                    if (isWalkable(nx, ny) && 
                        isWalkable(node.x + dxDiag[i], node.y) && 
                        isWalkable(node.x, node.y + dyDiag[i])) {
                        neighbors.emplace_back(nx, ny);
                    }
                }
            }
        }
        
        return neighbors;
    }

public:
    AStarPathfinder(int width, int height, IsWalkableFunc walkableFunc)
        : gridWidth(width), gridHeight(height), isWalkable(walkableFunc) {}
    
    // ═══════════════════════════════════════════════════════════════════════
    // FIND PATH - A* Algorithm Implementation
    // Returns: Vector of PathNodes from start to goal (empty if no path)
    // ═══════════════════════════════════════════════════════════════════════
    std::vector<PathNode> findPath(const PathNode& start, const PathNode& goal, 
                                    bool allowDiagonal = false, int maxIterations = 1000) {
        // Early exit if start or goal is not walkable
        if (!isWalkable(start.x, start.y) || !isWalkable(goal.x, goal.y)) {
            return {};
        }
        
        // Priority queue: (f-score, node)
        PriorityQueue<PathNode> openSet;
        std::unordered_set<PathNode, PathNodeHash> closedSet;
        std::unordered_map<PathNode, PathNode, PathNodeHash> cameFrom;
        std::unordered_map<PathNode, float, PathNodeHash> gScore;
        std::unordered_map<PathNode, float, PathNodeHash> fScore;
        
        // Initialize start node
        gScore[start] = 0.f;
        fScore[start] = heuristic(start, goal);
        openSet.push(fScore[start], start);
        
        int iterations = 0;
        
        while (!openSet.empty() && iterations < maxIterations) {
            ++iterations;
            
            PathNode current = openSet.pop();
            
            // Goal reached!
            if (current == goal) {
                // Reconstruct path
                std::vector<PathNode> path;
                PathNode node = current;
                while (cameFrom.find(node) != cameFrom.end()) {
                    path.push_back(node);
                    node = cameFrom[node];
                }
                path.push_back(start);
                
                // Reverse to get start -> goal order
                std::reverse(path.begin(), path.end());
                
                std::cout << "[A* DSA] Path found! Length: " << path.size() 
                          << " Iterations: " << iterations << std::endl;
                return path;
            }
            
            closedSet.insert(current);
            
            // Explore neighbors
            for (const PathNode& neighbor : getNeighbors(current, allowDiagonal)) {
                if (closedSet.count(neighbor)) continue;
                
                // Cost to reach neighbor
                float moveCost = (neighbor.x != current.x && neighbor.y != current.y) 
                                 ? 1.414f : 1.f;  // Diagonal = sqrt(2)
                float tentativeG = gScore[current] + moveCost;
                
                // Check if this is a better path
                if (gScore.find(neighbor) == gScore.end() || tentativeG < gScore[neighbor]) {
                    cameFrom[neighbor] = current;
                    gScore[neighbor] = tentativeG;
                    fScore[neighbor] = tentativeG + heuristic(neighbor, goal);
                    
                    if (!openSet.contains(neighbor)) {
                        openSet.push(fScore[neighbor], neighbor);
                    } else {
                        openSet.updatePriority(neighbor, fScore[neighbor]);
                    }
                }
            }
        }
        
        // No path found
        std::cout << "[A* DSA] No path found after " << iterations << " iterations" << std::endl;
        return {};
    }
    
    // Get next step towards goal (for enemy AI)
    PathNode getNextStep(const PathNode& start, const PathNode& goal, bool allowDiagonal = false) {
        std::vector<PathNode> path = findPath(start, goal, allowDiagonal);
        
        if (path.size() > 1) {
            return path[1];  // Return next step (path[0] is current position)
        }
        return start;  // No path or already at goal
    }
    
    void setGridSize(int width, int height) {
        gridWidth = width;
        gridHeight = height;
    }
    
    void setWalkableFunc(IsWalkableFunc func) {
        isWalkable = func;
    }
};
