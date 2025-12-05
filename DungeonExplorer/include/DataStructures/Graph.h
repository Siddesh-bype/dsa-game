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
#include <utility>

template<typename T>
class Graph {
private:
    std::unordered_map<T, std::vector<std::pair<T, int>>> adjacencyList; // node -> [(neighbor, weight)]
    
    void dfsRecursiveHelper(const T& current, std::unordered_set<T>& visited, std::vector<T>& result) const {
        visited.insert(current);
        result.push_back(current);
        
        auto it = adjacencyList.find(current);
        if (it != adjacencyList.end()) {
            for (const auto& [neighbor, weight] : it->second) {
                if (visited.find(neighbor) == visited.end()) {
                    dfsRecursiveHelper(neighbor, visited, result);
                }
            }
        }
    }
    
public:
    // Constructors
    Graph() = default;
    
    // Move constructor
    Graph(Graph&& other) noexcept : adjacencyList(std::move(other.adjacencyList)) {}
    
    // Move assignment operator
    Graph& operator=(Graph&& other) noexcept {
        if (this != &other) {
            adjacencyList = std::move(other.adjacencyList);
        }
        return *this;
    }
    
    // Copy constructor and assignment (default is fine)
    Graph(const Graph&) = default;
    Graph& operator=(const Graph&) = default;
    
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
    
    // Optimized - return const reference instead of copy
    const std::vector<std::pair<T, int>>& getNeighborsWithWeights(const T& vertex) const {
        static const std::vector<std::pair<T, int>> empty;
        auto it = adjacencyList.find(vertex);
        return (it != adjacencyList.end()) ? it->second : empty;
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
    
    bool hasEdge(const T& from, const T& to) const {
        auto it = adjacencyList.find(from);
        if (it != adjacencyList.end()) {
            for (const auto& [neighbor, weight] : it->second) {
                if (neighbor == to) return true;
            }
        }
        return false;
    }
    
    int getEdgeWeight(const T& from, const T& to) const {
        auto it = adjacencyList.find(from);
        if (it != adjacencyList.end()) {
            for (const auto& [neighbor, weight] : it->second) {
                if (neighbor == to) return weight;
            }
        }
        return -1; // Edge not found
    }
    
    bool removeVertex(const T& vertex) {
        if (adjacencyList.find(vertex) == adjacencyList.end()) {
            return false;
        }
        
        // Remove all edges pointing to this vertex
        for (auto& [node, edges] : adjacencyList) {
            edges.erase(
                std::remove_if(edges.begin(), edges.end(),
                    [&vertex](const std::pair<T, int>& p) { return p.first == vertex; }),
                edges.end()
            );
        }
        
        // Remove the vertex itself
        adjacencyList.erase(vertex);
        return true;
    }
    
    bool removeEdge(const T& from, const T& to) {
        auto it = adjacencyList.find(from);
        if (it != adjacencyList.end()) {
            auto& edges = it->second;
            auto edge_it = std::find_if(edges.begin(), edges.end(),
                [&to](const std::pair<T, int>& p) { return p.first == to; });
            
            if (edge_it != edges.end()) {
                edges.erase(edge_it);
                return true;
            }
        }
        return false;
    }
    
    size_t degree(const T& vertex) const {
        auto it = adjacencyList.find(vertex);
        return (it != adjacencyList.end()) ? it->second.size() : 0;
    }
    
    size_t edgeCount() const {
        size_t count = 0;
        for (const auto& [node, edges] : adjacencyList) {
            count += edges.size();
        }
        return count;
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
    
    std::vector<T> dfsRecursive(const T& start) {
        std::cout << "[DSA-Graph] Running recursive DFS from " << start << std::endl;
        std::vector<T> result;
        std::unordered_set<T> visited;
        dfsRecursiveHelper(start, visited, result);
        std::cout << "[DSA-Graph] Recursive DFS visited " << result.size() << " nodes" << std::endl;
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
    
    bool isConnected() const {
        if (adjacencyList.empty()) return true;
        
        // Use BFS to check if all vertices are reachable from the first vertex
        T start = adjacencyList.begin()->first;
        std::unordered_set<T> visited;
        std::queue<T> q;
        
        q.push(start);
        visited.insert(start);
        
        while (!q.empty()) {
            T current = q.front();
            q.pop();
            
            auto it = adjacencyList.find(current);
            if (it != adjacencyList.end()) {
                for (const auto& [neighbor, weight] : it->second) {
                    if (visited.find(neighbor) == visited.end()) {
                        visited.insert(neighbor);
                        q.push(neighbor);
                    }
                }
            }
        }
        
        return visited.size() == adjacencyList.size();
    }
    
    std::vector<T> findPath(const T& from, const T& to) const {
        std::unordered_map<T, T> parent;
        std::unordered_set<T> visited;
        std::queue<T> q;
        
        q.push(from);
        visited.insert(from);
        parent[from] = from;
        
        while (!q.empty()) {
            T current = q.front();
            q.pop();
            
            if (current == to) {
                // Reconstruct path
                std::vector<T> path;
                T node = to;
                while (node != from) {
                    path.push_back(node);
                    node = parent[node];
                }
                path.push_back(from);
                std::reverse(path.begin(), path.end());
                return path;
            }
            
            auto it = adjacencyList.find(current);
            if (it != adjacencyList.end()) {
                for (const auto& [neighbor, weight] : it->second) {
                    if (visited.find(neighbor) == visited.end()) {
                        visited.insert(neighbor);
                        parent[neighbor] = current;
                        q.push(neighbor);
                    }
                }
            }
        }
        
        return {}; // No path found
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
