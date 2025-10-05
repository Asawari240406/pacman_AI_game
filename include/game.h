#ifndef GAME_H
#define GAME_H

#include <vector>
#include "maze.h"
#include "pacman.h"
#include "ghost.h"

// Game states
enum GameState {
    GAME_START,
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_OVER,
    GAME_WIN,
    LEVEL_COMPLETE
};

class Game {
private:
    int score;
    int level;
    int maxLevel;
    GameState state;
    Maze* maze;
    Pacman* pacman;
    std::vector<Ghost*> ghosts;
    int dotsRemaining;
    int lives;
    bool gameOver;
    int updateCounter;    // Add this for speed control
    int updateFrequency;  // Add this for speed control

public:
    Game(int maxLevel = 5);
    ~Game();
    
    // Game initialization
    void init();
    void loadLevel(int level);
    
    // Game loop functions
    void update();
    void render();
    void handleInput(int key);
    
    // Game state management
    bool isGameOver() const;
    bool isLevelComplete() const;
    bool advanceToNextLevel();
    void resetLevel();
    
    // Getters and setters
    int getScore() const;
    void addScore(int points);
    int getLevel() const;
    GameState getState() const;
    void setState(GameState newState);
    int getLives() const;
    void decreaseLives();
    
    // Collision detection
    bool checkCollisions();
    bool checkDotCollection();
    bool checkPowerPelletCollection();
    bool checkGhostCollision();
    
    // Render functions
    void renderStartScreen();
    void renderPauseScreen();
    void renderGameOverScreen();
    void renderWinScreen();
    void renderLevelCompleteScreen();
    void renderMaze();
    void renderPacman();
    void renderGhosts();
    void renderHUD();
};

#endif // GAME_H