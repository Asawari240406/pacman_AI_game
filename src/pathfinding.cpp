#include "../include/pathfinding.h"
#include <queue>
#include <map>
#include <algorithm>
#include <cmath>
#include <unordered_map>

// A* pathfinding algorithm implementation
std::vector<Position> AStar::findPath(const Maze& maze, const Position& start, const Position& goal) {
    // Priority queue for open set
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    
    // Maps for tracking
    std::map<Position, int> gScore;  // Cost from start to node
    std::map<Position, int> fScore;  // Estimated total cost
    std::map<Position, Position> cameFrom;  // Path tracking
    
    // Initialize start node
    gScore[start] = 0;
    fScore[start] = heuristic(start, goal);
    openSet.push(Node(fScore[start], start));
    
    while (!openSet.empty()) {
        // Get node with lowest f-score
        Node current = openSet.top();
        openSet.pop();
        
        Position currentPos = current.pos;
        
        // Check if we reached the goal
        if (currentPos == goal) {
            return reconstructPath(cameFrom, currentPos);
        }
        
        // Check neighbors
        std::vector<Position> neighbors = maze.getNeighbors(currentPos);
        for (const Position& neighbor : neighbors) {
            // Calculate tentative g-score
            int tentativeGScore = gScore[currentPos] + 1;
            
            // If neighbor not visited or has better g-score
            if (gScore.find(neighbor) == gScore.end() || tentativeGScore < gScore[neighbor]) {
                // Update path
                cameFrom[neighbor] = currentPos;
                gScore[neighbor] = tentativeGScore;
                fScore[neighbor] = gScore[neighbor] + heuristic(neighbor, goal);
                
                // Add to open set
                openSet.push(Node(fScore[neighbor], neighbor));
            }
        }
    }
    
    // No path found
    return std::vector<Position>();
}

// Reconstruct path from cameFrom map
std::vector<Position> AStar::reconstructPath(const std::map<Position, Position>& cameFrom, const Position& current) {
    std::vector<Position> path;
    Position currentPos = current;
    path.push_back(currentPos);
    
    while (cameFrom.find(currentPos) != cameFrom.end()) {
        currentPos = cameFrom.at(currentPos);
        path.push_back(currentPos);
    }
    
    // Reverse to get path from start to goal
    std::reverse(path.begin(), path.end());
    return path;
}

// BFS pathfinding algorithm implementation
std::vector<Position> BFS::findPath(const Maze& maze, const Position& start, 
                                  const Position& goal, int maxDepth) {
    // Queue for BFS
    std::queue<Position> queue;
    
    // Maps for tracking
    std::map<Position, bool> visited;
    std::map<Position, Position> cameFrom;
    std::map<Position, int> depth;
    
    // Initialize
    queue.push(start);
    visited[start] = true;
    depth[start] = 0;
    
    while (!queue.empty()) {
        Position current = queue.front();
        queue.pop();
        
        // Check if we reached the goal
        if (current == goal) {
            return reconstructPath(cameFrom, current);
        }
        
        // Check depth limit
        if (maxDepth > 0 && depth[current] >= maxDepth) {
            continue;
        }
        
        // Check neighbors
        std::vector<Position> neighbors = maze.getNeighbors(current);
        for (const Position& neighbor : neighbors) {
            // Skip visited nodes
            if (visited.find(neighbor) != visited.end()) {
                continue;
            }
            
            // Mark as visited
            visited[neighbor] = true;
            cameFrom[neighbor] = current;
            depth[neighbor] = depth[current] + 1;
            
            // Add to queue
            queue.push(neighbor);
        }
    }
    
    // No path found
    return std::vector<Position>();
}

// Reconstruct path from cameFrom map
std::vector<Position> BFS::reconstructPath(const std::map<Position, Position>& cameFrom, Position current) {
    std::vector<Position> path;
    path.push_back(current);
    
    while (cameFrom.find(current) != cameFrom.end()) {
        current = cameFrom.at(current);
        path.push_back(current);
    }
    
    // Reverse to get path from start to goal
    std::reverse(path.begin(), path.end());
    return path;
}