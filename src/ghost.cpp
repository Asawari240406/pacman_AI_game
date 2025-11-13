#include "../include/ghost.h"
#include "../include/pathfinding.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <iostream>

// Constructor
Ghost::Ghost(GhostType type, const Position& startPos)
    : position(startPos), targetPosition(startPos), direction(RIGHT), // Start with a direction
      type(type), state(CHASE), speed(1), frightenedTimer(0) { // Start in CHASE mode
}

// Destructor
Ghost::~Ghost() {
    // Nothing to clean up
}

// Move ghost
void Ghost::move(const Maze& maze) {
    // Only move if we have a direction and can move
    if (direction != NONE) {
        // If current direction is blocked, find a new one
        if (!canMove(direction, maze)) {
            // Find any valid direction
            const Direction dirs[] = {UP, DOWN, LEFT, RIGHT};
            std::vector<Direction> validDirs;
            
            for (Direction dir : dirs) {
                if (canMove(dir, maze)) {
                    validDirs.push_back(dir);
                }
            }
            
            if (!validDirs.empty()) {
                direction = validDirs[rand() % validDirs.size()];
            } else {
                direction = NONE;
                return;
            }
        }
        
        // Move in the current direction
        position = getNextPosition(direction);
    }
}

// Check if ghost can move in a direction
bool Ghost::canMove(Direction dir, const Maze& maze) const {
    Position nextPos = getNextPosition(dir);
    return maze.isValidPosition(nextPos);
}

// Update AI
void Ghost::updateAI(const Maze& maze, const Position& pacmanPos, 
                    const Direction& pacmanDir, const Position& blinkyPos) {
    // Skip if eaten (returning to ghost house)
    if (state == EATEN) {
        // Target ghost house
        targetPosition = maze.getGhostStartPositions()[0];
        
        // If we reached the ghost house, change state to chase
        if (position == targetPosition) {
            state = CHASE;
        }
    } else if (state == FRIGHTENED) {
        // Move randomly when frightened
        const Direction dirs[] = {UP, DOWN, LEFT, RIGHT};
        std::vector<Direction> validDirs;
        
        for (Direction dir : dirs) {
            if (canMove(dir, maze)) {
                validDirs.push_back(dir);
            }
        }
        
        if (!validDirs.empty()) {
            direction = validDirs[rand() % validDirs.size()];
        }
        
        return;
    } else {
        // Calculate target based on ghost type and state
        targetPosition = calculateTarget(maze, pacmanPos, pacmanDir, blinkyPos);
    }
    
    // Find path to target
    std::vector<Position> path = findPath(maze, position, targetPosition);
    
    // If pathfinding fails or path is too short, use simple direction-based movement
    if (path.size() <= 1) {
        // Use simple direction calculation instead of random
        int dx = targetPosition.x - position.x;
        int dy = targetPosition.y - position.y;
        
        // Prefer horizontal or vertical movement based on larger difference
        if (abs(dx) > abs(dy)) {
            if (dx > 0 && canMove(RIGHT, maze)) {
                direction = RIGHT;
            } else if (dx < 0 && canMove(LEFT, maze)) {
                direction = LEFT;
            } else {
                // If preferred direction blocked, try vertical
                if (dy > 0 && canMove(DOWN, maze)) {
                    direction = DOWN;
                } else if (dy < 0 && canMove(UP, maze)) {
                    direction = UP;
                }
            }
        } else {
            if (dy > 0 && canMove(DOWN, maze)) {
                direction = DOWN;
            } else if (dy < 0 && canMove(UP, maze)) {
                direction = UP;
            } else {
                // If preferred direction blocked, try horizontal
                if (dx > 0 && canMove(RIGHT, maze)) {
                    direction = RIGHT;
                } else if (dx < 0 && canMove(LEFT, maze)) {
                    direction = LEFT;
                }
            }
        }
        return;
    }
    
    // Get next position in path (index 1, since index 0 is current position)
    if (path.size() > 1) {
        Position nextPos = path[1];
        
        // Determine direction based on next position
        if (nextPos.x < position.x) {
            direction = LEFT;
        } else if (nextPos.x > position.x) {
            direction = RIGHT;
        } else if (nextPos.y < position.y) {
            direction = UP;
        } else if (nextPos.y > position.y) {
            direction = DOWN;
        }
    }
}

// Set state
void Ghost::setState(GhostState newState) {
    state = newState;
    
    // Reset frightened timer
    if (state == FRIGHTENED) {
        frightenedTimer = 100; // Increased timer for better gameplay
    }
}

// Get state
GhostState Ghost::getState() const {
    return state;
}

// Update frightened timer
void Ghost::updateFrightenedTimer() {
    if (state == FRIGHTENED && frightenedTimer > 0) {
        frightenedTimer--;
        
        // Change back to chase when timer expires
        if (frightenedTimer <= 0) {
            state = CHASE;
        }
    }
}

// Check if frightened
bool Ghost::isFrightened() const {
    return state == FRIGHTENED;
}

// Get position
Position Ghost::getPosition() const {
    return position;
}

// Set position
void Ghost::setPosition(const Position& pos) {
    position = pos;
}

// Get direction
Direction Ghost::getDirection() const {
    return direction;
}

// Set direction
void Ghost::setDirection(Direction dir) {
    direction = dir;
}

// Get type
GhostType Ghost::getType() const {
    return type;
}

// Get next position based on direction
Position Ghost::getNextPosition(Direction dir) const {
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

// Reset position
void Ghost::resetPosition(const Position& startPos) {
    position = startPos;
    direction = RIGHT; // Give a default direction
    state = CHASE;
}

// Calculate target based on ghost type and state
Position Ghost::calculateTarget(const Maze& maze, const Position& pacmanPos, 
                              const Direction& pacmanDir, const Position& blinkyPos) {
    // In scatter mode, target corner
    if (state == SCATTER) {
        // Each ghost targets a different corner
        switch (type) {
            case BLINKY: // Top-right
                return Position(maze.getWidth() - 2, 1);
                
            case PINKY: // Top-left
                return Position(1, 1);
                
            case INKY: // Bottom-right
                return Position(maze.getWidth() - 2, maze.getHeight() - 2);
                
            case CLYDE: // Bottom-left
                return Position(1, maze.getHeight() - 2);
        }
    }
    
    // In chase mode, each ghost has a different targeting strategy
    switch (type) {
        case BLINKY: // Target pacman directly
            return pacmanPos;
            
        case PINKY: { // Target 4 tiles ahead of pacman
            Position target = pacmanPos;
            
            // Calculate position 4 tiles ahead of pacman
            switch (pacmanDir) {
                case UP:
                    target.y -= 4;
                    break;
                    
                case DOWN:
                    target.y += 4;
                    break;
                    
                case LEFT:
                    target.x -= 4;
                    break;
                    
                case RIGHT:
                    target.x += 4;
                    break;
                    
                case NONE:
                    break;
            }
            
            // Ensure target is within maze bounds
            if (target.x < 1) target.x = 1;
            if (target.x >= maze.getWidth() - 1) target.x = maze.getWidth() - 2;
            if (target.y < 1) target.y = 1;
            if (target.y >= maze.getHeight() - 1) target.y = maze.getHeight() - 2;
            
            return target;
        }
            
        case INKY: { // Target based on pacman and blinky positions
            Position target = pacmanPos;
            
            // Calculate position 2 tiles ahead of pacman
            switch (pacmanDir) {
                case UP:
                    target.y -= 2;
                    break;
                    
                case DOWN:
                    target.y += 2;
                    break;
                    
                case LEFT:
                    target.x -= 2;
                    break;
                    
                case RIGHT:
                    target.x += 2;
                    break;
                    
                case NONE:
                    break;
            }
            
            // Calculate vector from blinky to this position
            int dx = target.x - blinkyPos.x;
            int dy = target.y - blinkyPos.y;
            
            // Double the vector
            target.x = blinkyPos.x + dx * 2;
            target.y = blinkyPos.y + dy * 2;
            
            // Ensure target is within maze bounds
            if (target.x < 1) target.x = 1;
            if (target.x >= maze.getWidth() - 1) target.x = maze.getWidth() - 2;
            if (target.y < 1) target.y = 1;
            if (target.y >= maze.getHeight() - 1) target.y = maze.getHeight() - 2;
            
            return target;
        }
            
        case CLYDE: { // Target pacman when far, scatter when close
            int distance = std::abs(position.x - pacmanPos.x) + std::abs(position.y - pacmanPos.y);
            
            if (distance > 8) {
                // Target pacman when far
                return pacmanPos;
            } else {
                // Target bottom-left corner when close
                return Position(1, maze.getHeight() - 2);
            }
        }
    }
    
    // Default to pacman position
    return pacmanPos;
}

// Find path using A*
std::vector<Position> Ghost::findPath(const Maze& maze, const Position& start, 
                                     const Position& target) {
    return AStar::findPath(maze, start, target);
}

// Calculate Manhattan distance heuristic
int Ghost::heuristic(const Position& a, const Position& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

// Get opposite direction
Direction getOppositeDirection(Direction dir) {
    switch (dir) {
        case UP:
            return DOWN;
        case DOWN:
            return UP;
        case LEFT:
            return RIGHT;
        case RIGHT:
            return LEFT;
        default:
            return NONE;
    }
}