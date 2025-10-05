#include "../include/maze.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

// Constructor
Maze::Maze(int level) : width(0), height(0), dotCount(0), level(level) {
    // Generate maze based on level
    generateMaze(level);
}

// Destructor
Maze::~Maze() {
    // Nothing to clean up
}

// Generate maze based on level
void Maze::generateMaze(int level) {
    // For now, we'll use predefined mazes for each level
    // In a more advanced implementation, we could generate mazes procedurally
    
    // Define maze dimensions based on level
    width = 20 + (level - 1) * 2; // Increase size with level
    height = 15 + (level - 1) * 2;
    
    // Limit size
    if (width > 30) width = 30;
    if (height > 25) height = 25;
    
    // Initialize grid
    grid.resize(height, std::vector<CellType>(width, WALL));
    
    // Create a simple maze layout
    // This is a very basic maze generation - in a real game, you'd want more sophisticated algorithms
    
    // Start with all walls
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            grid[y][x] = WALL;
        }
    }
    
    // Create paths in a grid pattern
    for (int y = 1; y < height - 1; y += 2) {
        for (int x = 1; x < width - 1; x += 2) {
            grid[y][x] = PATH;
            
            // Add some random connections
            if (x + 2 < width - 1 && (rand() % 100) < 70) {
                grid[y][x + 1] = PATH;
            }
            
            if (y + 2 < height - 1 && (rand() % 100) < 70) {
                grid[y + 1][x] = PATH;
            }
        }
    }
    
    // Ensure there's a path around the edges
    for (int y = 1; y < height - 1; y++) {
        grid[y][1] = PATH;
        grid[y][width - 2] = PATH;
    }
    
    for (int x = 1; x < width - 1; x++) {
        grid[1][x] = PATH;
        grid[height - 2][x] = PATH;
    }
    
    // Add dots to all path cells
    dotCount = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x] == PATH) {
                grid[y][x] = DOT;
                dotCount++;
            }
        }
    }
    
    // Add power pellets (one in each corner)
    if (grid[1][1] == DOT) {
        grid[1][1] = POWER_PELLET;
        dotCount--;
    }
    
    if (grid[1][width - 2] == DOT) {
        grid[1][width - 2] = POWER_PELLET;
        dotCount--;
    }
    
    if (grid[height - 2][1] == DOT) {
        grid[height - 2][1] = POWER_PELLET;
        dotCount--;
    }
    
    if (grid[height - 2][width - 2] == DOT) {
        grid[height - 2][width - 2] = POWER_PELLET;
        dotCount--;
    }
    
    // Set pacman start position (center of maze)
    pacmanStartPosition = Position(width / 2, height / 2);
    
    // Make sure pacman start position is a path
    if (grid[pacmanStartPosition.y][pacmanStartPosition.x] == WALL) {
        grid[pacmanStartPosition.y][pacmanStartPosition.x] = EMPTY;
    } else if (grid[pacmanStartPosition.y][pacmanStartPosition.x] == DOT) {
        grid[pacmanStartPosition.y][pacmanStartPosition.x] = EMPTY;
        dotCount--;
    }
    
    // Set ghost start positions (corners)
    ghostStartPositions.clear();
    ghostStartPositions.push_back(Position(1, 1));
    ghostStartPositions.push_back(Position(width - 2, 1));
    ghostStartPositions.push_back(Position(1, height - 2));
    ghostStartPositions.push_back(Position(width - 2, height - 2));
    
    // Make sure ghost start positions are paths
    for (auto& pos : ghostStartPositions) {
        if (grid[pos.y][pos.x] == WALL) {
            grid[pos.y][pos.x] = EMPTY;
        } else if (grid[pos.y][pos.x] == DOT) {
            grid[pos.y][pos.x] = EMPTY;
            dotCount--;
        } else if (grid[pos.y][pos.x] == POWER_PELLET) {
            grid[pos.y][pos.x] = EMPTY;
            dotCount--;
        }
    }
}

// Load maze from file
void Maze::loadMazeFromFile(const std::string& filename) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open maze file: " << filename << std::endl;
        return;
    }
    
    // Read dimensions
    file >> width >> height;
    
    // Initialize grid
    grid.resize(height, std::vector<CellType>(width, WALL));
    
    // Read maze data
    dotCount = 0;
    ghostStartPositions.clear();
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int cellType;
            file >> cellType;
            
            grid[y][x] = static_cast<CellType>(cellType);
            
            // Count dots
            if (grid[y][x] == DOT) {
                dotCount++;
            }
            
            // Set pacman start position
            if (cellType == 5) { // Special value for pacman start
                pacmanStartPosition = Position(x, y);
                grid[y][x] = EMPTY;
            }
            
            // Set ghost start positions
            if (cellType == 6) { // Special value for ghost start
                ghostStartPositions.push_back(Position(x, y));
                grid[y][x] = EMPTY;
            }
        }
    }
    
    file.close();
}

// Get width
int Maze::getWidth() const {
    return width;
}

// Get height
int Maze::getHeight() const {
    return height;
}

// Get cell
CellType Maze::getCell(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return WALL;
    }
    
    return grid[y][x];
}

// Get cell
CellType Maze::getCell(const Position& pos) const {
    return getCell(pos.x, pos.y);
}

// Get pacman start position
Position Maze::getPacmanStartPosition() const {
    return pacmanStartPosition;
}

// Get ghost start positions
std::vector<Position> Maze::getGhostStartPositions() const {
    return ghostStartPositions;
}

// Get dot count
int Maze::getDotCount() const {
    return dotCount;
}

// Get level
int Maze::getLevel() const {
    return level;
}

// Set cell
void Maze::setCell(int x, int y, CellType type) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }
    
    grid[y][x] = type;
}

// Set cell
void Maze::setCell(const Position& pos, CellType type) {
    setCell(pos.x, pos.y, type);
}

// Check if wall
bool Maze::isWall(int x, int y) const {
    return getCell(x, y) == WALL;
}

// Check if wall
bool Maze::isWall(const Position& pos) const {
    return isWall(pos.x, pos.y);
}

// Check if valid position
bool Maze::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height && !isWall(x, y);
}

// Check if valid position
bool Maze::isValidPosition(const Position& pos) const {
    return isValidPosition(pos.x, pos.y);
}

// Get neighbors
std::vector<Position> Maze::getNeighbors(const Position& pos) const {
    std::vector<Position> neighbors;
    
    // Check each direction
    const int dx[] = {0, 1, 0, -1};
    const int dy[] = {-1, 0, 1, 0};
    
    for (int i = 0; i < 4; i++) {
        Position neighbor(pos.x + dx[i], pos.y + dy[i]);
        
        if (isValidPosition(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }
    
    return neighbors;
}