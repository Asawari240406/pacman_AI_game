#ifndef GHOST_H
#define GHOST_H

#include "maze.h"
#include "pacman.h"  // Include pacman.h for Direction enum
#include <vector>
#include <queue>
#include <map>

enum GhostType {
    BLINKY, // Red ghost - directly targets Pac-Man
    PINKY,  // Pink ghost - targets 4 tiles ahead of Pac-Man
    INKY,   // Blue ghost - uses combination of Blinky and Pac-Man positions
    CLYDE   // Orange ghost - targets Pac-Man when far, runs away when close
};

enum GhostState {
    CHASE,      // Actively pursuing Pac-Man
    SCATTER,    // Moving to a corner of the maze
    FRIGHTENED, // Running away from Pac-Man (after power pellet)
    EATEN       // Returning to ghost house after being eaten
};

class Ghost {
private:
    Position position;
    Position targetPosition;
    Direction direction;
    GhostType type;
    GhostState state;
    int speed;
    int frightenedTimer;
    
    // A* pathfinding implementation
    std::vector<Position> findPath(const Maze& maze, const Position& start, 
                                  const Position& target);
    int heuristic(const Position& a, const Position& b);
    
    // Ghost behavior methods
    Position calculateTarget(const Maze& maze, const Position& pacmanPos, 
                            const Direction& pacmanDir, const Position& blinkyPos);

public:
    Ghost(GhostType type, const Position& startPos);
    ~Ghost();
    
    // Movement methods
    void move(const Maze& maze);
    bool canMove(Direction dir, const Maze& maze) const;
    
    // AI methods
    void updateAI(const Maze& maze, const Position& pacmanPos, 
                 const Direction& pacmanDir, const Position& blinkyPos);
    
    // State management
    void setState(GhostState newState);
    GhostState getState() const;
    void updateFrightenedTimer();
    bool isFrightened() const;
    
    // Getters and setters
    Position getPosition() const;
    void setPosition(const Position& pos);
    Direction getDirection() const;
    void setDirection(Direction dir);
    GhostType getType() const;
    
    // Utility methods
    Position getNextPosition(Direction dir) const;
    void resetPosition(const Position& startPos);
};

// Helper function to get opposite direction
Direction getOppositeDirection(Direction dir);

#endif // GHOST_H