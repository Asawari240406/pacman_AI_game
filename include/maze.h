#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <string>

// Cell types in the maze
enum CellType {
    WALL,
    PATH,
    DOT,
    POWER_PELLET,
    EMPTY
};

// Position structure for coordinates
struct Position {
    int x;
    int y;
    
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
    
    // Add comparison operators for std::map
    bool operator<(const Position& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

class Maze {
private:
    int width;
    int height;
    std::vector<std::vector<CellType>> grid;
    std::vector<Position> ghostStartPositions;
    Position pacmanStartPosition;
    int dotCount;
    int level;

public:
    Maze(int level = 1);
    ~Maze();
    
    // Maze generation and loading
    void generateMaze(int level);
    void loadMazeFromFile(const std::string& filename);
    
    // Getters
    int getWidth() const;
    int getHeight() const;
    CellType getCell(int x, int y) const;
    CellType getCell(const Position& pos) const;
    Position getPacmanStartPosition() const;
    std::vector<Position> getGhostStartPositions() const;
    int getDotCount() const;
    int getLevel() const;
    
    // Setters
    void setCell(int x, int y, CellType type);
    void setCell(const Position& pos, CellType type);
    
    // Utility functions
    bool isWall(int x, int y) const;
    bool isWall(const Position& pos) const;
    bool isValidPosition(int x, int y) const;
    bool isValidPosition(const Position& pos) const;
    std::vector<Position> getNeighbors(const Position& pos) const;
};

#endif // MAZE_H