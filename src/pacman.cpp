#include "../include/pacman.h"
#include "../include/pathfinding.h"
#include <algorithm>
#include <limits>

// Constructor
Pacman::Pacman(const Position& startPos, bool isAI) 
    : position(startPos), direction(NONE), isAI(isAI), searchDepth(10) {
}

// Destructor
Pacman::~Pacman() {
    // Nothing to clean up
}

// Move pacman
void Pacman::move(const Maze& maze) {
    // Check if we can move in the current direction
    if (direction != NONE && canMove(direction, maze)) {
        position = getNextPosition(direction);
    }
}

// Set direction
void Pacman::setDirection(Direction dir) {
    direction = dir;
}

// Check if pacman can move in a direction
bool Pacman::canMove(Direction dir, const Maze& maze) const {
    Position nextPos = getNextPosition(dir);
    return maze.isValidPosition(nextPos);
}

// Update AI
void Pacman::updateAI(const Maze& maze, const std::vector<Position>& ghostPositions) {
    // Only update if AI is enabled
    if (!isAI) {
        return;
    }
    
    // Find best move
    Direction bestMove = findBestMove(maze, ghostPositions);
    
    // Set direction
    if (bestMove != NONE) {
        direction = bestMove;
    }
}

// Set AI
void Pacman::setAI(bool enabled) {
    isAI = enabled;
}

// Check if AI is enabled
bool Pacman::isAIControlled() const {
    return isAI;
}

// Get position
Position Pacman::getPosition() const {
    return position;
}

// Set position
void Pacman::setPosition(const Position& pos) {
    position = pos;
}

// Get direction
Direction Pacman::getDirection() const {
    return direction;
}

// Get next position based on direction
Position Pacman::getNextPosition(Direction dir) const {
    Position nextPos = position;
    
    switch (dir) {
        case UP:
            nextPos.y--;
            break;
            
        case DOWN:
            nextPos.y++;
            break;
            
        case LEFT:
            nextPos.x--;
            break;
            
        case RIGHT:
            nextPos.x++;
            break;
            
        case NONE:
            break;
    }
    
    return nextPos;
}

// Find best move using AI
Direction Pacman::findBestMove(const Maze& maze, const std::vector<Position>& ghostPositions) {
    // First priority: avoid ghosts if they're close
    int ghostDistance = distanceToNearestGhost(ghostPositions);
    
    if (ghostDistance < 5) {
        return avoidGhosts(maze, ghostPositions);
    }
    
    // Second priority: seek nearest dot
    return seekNearestDot(maze);
}

// Avoid ghosts
Direction Pacman::avoidGhosts(const Maze& maze, const std::vector<Position>& ghostPositions) {
    // Find direction that maximizes distance to nearest ghost
    Direction bestDir = NONE;
    int maxDistance = -1;
    
    // Check each direction
    const Direction dirs[] = {UP, DOWN, LEFT, RIGHT};
    
    for (Direction dir : dirs) {
        // Check if we can move in this direction
        if (!canMove(dir, maze)) {
            continue;
        }
        
        // Get next position
        Position nextPos = getNextPosition(dir);
        
        // Calculate minimum distance to any ghost
        int minDistance = std::numeric_limits<int>::max();
        
        for (const Position& ghostPos : ghostPositions) {
            int distance = std::abs(nextPos.x - ghostPos.x) + std::abs(nextPos.y - ghostPos.y);
            minDistance = std::min(minDistance, distance);
        }
        
        // Update best direction
        if (minDistance > maxDistance) {
            maxDistance = minDistance;
            bestDir = dir;
        }
    }
    
    return bestDir;
}

// Seek nearest dot
Direction Pacman::seekNearestDot(const Maze& maze) {
    // Find nearest dot using BFS
    Position nearestDot(-1, -1);
    int minDistance = std::numeric_limits<int>::max();
    
    // Check each cell in the maze
    for (int y = 0; y < maze.getHeight(); y++) {
        for (int x = 0; x < maze.getWidth(); x++) {
            // Check if cell is a dot
            if (maze.getCell(x, y) == DOT || maze.getCell(x, y) == POWER_PELLET) {
                Position dotPos(x, y);
                
                // Calculate distance
                int distance = std::abs(position.x - x) + std::abs(position.y - y);
                
                // Update nearest dot
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestDot = dotPos;
                }
            }
        }
    }
    
    // If no dot found, return NONE
    if (nearestDot.x == -1) {
        return NONE;
    }
    
    // Find path to nearest dot
    std::vector<Position> path = bfsSearch(maze, position, nearestDot, searchDepth);
    
    // If path is empty or has only one node (current position), return NONE
    if (path.size() <= 1) {
        return NONE;
    }
    
    // Get next position in path
    Position nextPos = path[1];
    
    // Determine direction
    if (nextPos.x < position.x) {
        return LEFT;
    } else if (nextPos.x > position.x) {
        return RIGHT;
    } else if (nextPos.y < position.y) {
        return UP;
    } else if (nextPos.y > position.y) {
        return DOWN;
    }
    
    return NONE;
}

// Calculate distance to nearest ghost
int Pacman::distanceToNearestGhost(const std::vector<Position>& ghostPositions) {
    int minDistance = std::numeric_limits<int>::max();
    
    for (const Position& ghostPos : ghostPositions) {
        int distance = std::abs(position.x - ghostPos.x) + std::abs(position.y - ghostPos.y);
        minDistance = std::min(minDistance, distance);
    }
    
    return minDistance;
}

// BFS search
std::vector<Position> Pacman::bfsSearch(const Maze& maze, const Position& start, 
                                      const Position& target, int maxDepth) {
    return BFS::findPath(maze, start, target, maxDepth);
}