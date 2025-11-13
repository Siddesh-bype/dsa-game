#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <iostream>
#include <limits>
#include <algorithm>
#include <functional>

template<typename T>
class Graph {
private:
    std::unordered_map<T, std::vector<std::pair<T, int>>> adjacencyList; // node -> [(neighbor, weight)]
    
public:
    Graph() = default;
    
    void addVertex(const T& vertex) {
        if (adjacencyList.find(vertex) == adjacencyList.end()) {
            adjacencyList[vertex] = std::vector<std::pair<T, int>>();
            std::cout << "[DSA-Graph] Added vertex. Total vertices: " << adjacencyList.size() << std::endl;
        }
    }
    
    void addEdge(const T& from, const T& to, int weight = 1) {
        addVertex(from);
        addVertex(to);
        adjacencyList[from].push_back({to, weight});
        std::cout << "[DSA-Graph] Added edge from " << from << " to " << to << " (weight: " << weight << ")" << std::endl;
    }
    
    void addBidirectionalEdge(const T& v1, const T& v2, int weight = 1) {
        addEdge(v1, v2, weight);
        addEdge(v2, v1, weight);
    }
    
    std::vector<T> getNeighbors(const T& vertex) const {
        std::vector<T> neighbors;
        auto it = adjacencyList.find(vertex);
        if (it != adjacencyList.end()) {
            for (const auto& pair : it->second) {
                neighbors.push_back(pair.first);
            }
        }
        return neighbors;
    }
    
    std::vector<T> bfs(const T& start) {
        std::cout << "[DSA-Graph] Running BFS from " << start << std::endl;
        std::vector<T> result;
        std::unordered_set<T> visited;
        std::queue<T> q;
        
        q.push(start);
        visited.insert(start);
        
        while (!q.empty()) {
            T current = q.front();
            q.pop();
            result.push_back(current);
            
            for (const auto& neighbor : getNeighbors(current)) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    q.push(neighbor);
                }
            }
        }
        
        std::cout << "[DSA-Graph] BFS visited " << result.size() << " nodes" << std::endl;
        return result;
    }
    
    std::vector<T> dfs(const T& start) {
        std::cout << "[DSA-Graph] Running DFS from " << start << std::endl;
        std::vector<T> result;
        std::unordered_set<T> visited;
        std::stack<T> s;
        
        s.push(start);
        
        while (!s.empty()) {
            T current = s.top();
            s.pop();
            
            if (visited.find(current) == visited.end()) {
                visited.insert(current);
                result.push_back(current);
                
                auto neighbors = getNeighbors(current);
                for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                    if (visited.find(*it) == visited.end()) {
                        s.push(*it);
                    }
                }
            }
        }
        
        std::cout << "[DSA-Graph] DFS visited " << result.size() << " nodes" << std::endl;
        return result;
    }
    
    std::unordered_map<T, int> dijkstra(const T& start) {
        std::cout << "[DSA-Graph] Running Dijkstra from " << start << std::endl;
        std::unordered_map<T, int> distances;
        
        for (const auto& pair : adjacencyList) {
            distances[pair.first] = std::numeric_limits<int>::max();
        }
        distances[start] = 0;
        
        using PQElement = std::pair<int, T>; // (distance, vertex)
        std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;
        pq.push({0, start});
        
        while (!pq.empty()) {
            auto [dist, current] = pq.top();
            pq.pop();
            
            if (dist > distances[current]) continue;
            
            auto it = adjacencyList.find(current);
            if (it != adjacencyList.end()) {
                for (const auto& [neighbor, weight] : it->second) {
                    int newDist = dist + weight;
                    if (newDist < distances[neighbor]) {
                        distances[neighbor] = newDist;
                        pq.push({newDist, neighbor});
                    }
                }
            }
        }
        
        std::cout << "[DSA-Graph] Dijkstra computed distances to " << distances.size() << " nodes" << std::endl;
        return distances;
    }
    
    bool hasVertex(const T& vertex) const {
        return adjacencyList.find(vertex) != adjacencyList.end();
    }
    
    size_t vertexCount() const {
        return adjacencyList.size();
    }
    
    void clear() {
        adjacencyList.clear();
        std::cout << "[DSA-Graph] Cleared graph" << std::endl;
    }
};
