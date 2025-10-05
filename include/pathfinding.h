#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "maze.h"
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <functional>

// Node structure for A* algorithm
struct Node {
    int fScore;
    Position pos;
    
    Node(int f, Position p) : fScore(f), pos(p) {}
    
    // For priority queue ordering
    bool operator>(const Node& other) const {
        return fScore > other.fScore;
    }
};

// A* pathfinding algorithm
class AStar {
public:
    // Find path from start to goal using A* algorithm
    static std::vector<Position> findPath(const Maze& maze, const Position& start, const Position& goal);
    
private:
    // Manhattan distance heuristic
    static int heuristic(const Position& a, const Position& b) {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }
    
    // Reconstruct path from came_from map
    static std::vector<Position> reconstructPath(const std::map<Position, Position>& cameFrom, const Position& current);
};

// BFS pathfinding algorithm with depth limit
class BFS {
public:
    // Find path from start to goal using BFS algorithm with depth limit
    static std::vector<Position> findPath(const Maze& maze, const Position& start, 
                                         const Position& goal, int maxDepth = -1);
    
private:
    // Reconstruct path from came_from map
    static std::vector<Position> reconstructPath(const std::map<Position, Position>& cameFrom, Position current);
};

#endif // PATHFINDING_H