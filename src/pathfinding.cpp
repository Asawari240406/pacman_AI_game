#include "../include/pathfinding.h"
#include <queue>
#include <map>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <iostream>
#include <functional>

// A* pathfinding algorithm implementation
std::vector<Position> AStar::findPath(const Maze& maze, const Position& start, const Position& goal) {
    // If start or goal is invalid, return empty path
    if (!maze.isValidPosition(start) || !maze.isValidPosition(goal)) {
        return std::vector<Position>();
    }
    
    // If start is goal, return path with just start
    if (start == goal) {
        return std::vector<Position>{start};
    }

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
            
            // Check if this path to neighbor is better
            bool isBetterPath = false;
            if (gScore.find(neighbor) == gScore.end()) {
                // First time visiting this node
                isBetterPath = true;
            } else if (tentativeGScore < gScore[neighbor]) {
                // Found better path to this node
                isBetterPath = true;
            }
            
            if (isBetterPath) {
                // This path is better, record it
                cameFrom[neighbor] = currentPos;
                gScore[neighbor] = tentativeGScore;
                fScore[neighbor] = gScore[neighbor] + heuristic(neighbor, goal);
                
                // Add to open set (we don't check if it's already there, 
                // the priority queue will handle duplicates)
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
    
    // Reconstruct path backwards from goal to start
    while (cameFrom.find(currentPos) != cameFrom.end()) {
        path.push_back(currentPos);
        currentPos = cameFrom.at(currentPos);
    }
    path.push_back(currentPos); // Add start position
    
    // Reverse to get path from start to goal
    std::reverse(path.begin(), path.end());
    return path;
}

// BFS pathfinding algorithm implementation
std::vector<Position> BFS::findPath(const Maze& maze, const Position& start, 
                                  const Position& goal, int maxDepth) {
    // If start or goal is invalid, return empty path
    if (!maze.isValidPosition(start) || !maze.isValidPosition(goal)) {
        return std::vector<Position>();
    }
    
    // If start is goal, return path with just start
    if (start == goal) {
        return std::vector<Position>{start};
    }

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
    cameFrom[start] = start;
    
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
    
    // Reconstruct path backwards from goal to start
    while (cameFrom.find(current) != cameFrom.end() && cameFrom.at(current) != current) {
        path.push_back(current);
        current = cameFrom.at(current);
    }
    path.push_back(current); // Add start position
    
    // Reverse to get path from start to goal
    std::reverse(path.begin(), path.end());
    return path;
}