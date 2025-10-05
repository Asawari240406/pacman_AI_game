#ifndef PACMAN_H
#define PACMAN_H

#include "maze.h"
#include <queue>

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

class Pacman {
private:
    Position position;
    Direction direction;
    bool isAI;
    int searchDepth; // Depth limit for BFS search
    
    // AI methods
    Direction findBestMove(const Maze& maze, const std::vector<Position>& ghostPositions);
    Direction avoidGhosts(const Maze& maze, const std::vector<Position>& ghostPositions);
    Direction seekNearestDot(const Maze& maze);
    int distanceToNearestGhost(const std::vector<Position>& ghostPositions);
    
    // BFS implementation for pathfinding
    std::vector<Position> bfsSearch(const Maze& maze, const Position& start, 
                                   const Position& target, int maxDepth);

public:
    Pacman(const Position& startPos, bool isAI = true);
    ~Pacman();
    
    // Movement methods
    void move(const Maze& maze);
    void setDirection(Direction dir);
    bool canMove(Direction dir, const Maze& maze) const;
    
    // AI control
    void updateAI(const Maze& maze, const std::vector<Position>& ghostPositions);
    void setAI(bool enabled);
    bool isAIControlled() const;
    
    // Getters and setters
    Position getPosition() const;
    void setPosition(const Position& pos);
    Direction getDirection() const;
    
    // Utility methods
    Position getNextPosition(Direction dir) const;
};

#endif // PACMAN_H