#include "../include/game.h"
#include "../include/pathfinding.h"
#include <GL/glut.h>
#include <iostream>
#include <cmath>

// Constructor
Game::Game(int maxLevel) : score(0), level(1), maxLevel(maxLevel), state(GAME_START),
                          maze(nullptr), pacman(nullptr), dotsRemaining(0), 
                          lives(3), gameOver(false), updateCounter(0), updateFrequency(6) {
    // updateFrequency = 6 means update every 6 frames (slower speed)
}

// Destructor
Game::~Game() {
    delete maze;
    delete pacman;
    
    for (auto ghost : ghosts) {
        delete ghost;
    }
    ghosts.clear();
}

// Initialize game
void Game::init() {
    // Load first level
    loadLevel(1);
    
    // Set initial state
    state = GAME_RUNNING;
}

// Load level
void Game::loadLevel(int levelNum) {
    // Clean up previous level
    delete maze;
    delete pacman;
    
    for (auto ghost : ghosts) {
        delete ghost;
    }
    ghosts.clear();
    
    // Set level
    level = levelNum;
    
    // Create maze
    maze = new Maze(level);
    
    // Set dots remaining
    dotsRemaining = maze->getDotCount();
    
    // Create pacman
    pacman = new Pacman(maze->getPacmanStartPosition(), true);
    
    // Create ghosts
    std::vector<Position> ghostPositions = maze->getGhostStartPositions();
    
    // Create different ghost types based on level
    int numGhosts = std::min(4, level + 1); // Increase ghosts with level
    
    for (int i = 0; i < numGhosts && i < ghostPositions.size(); i++) {
        GhostType type = static_cast<GhostType>(i % 4);
        ghosts.push_back(new Ghost(type, ghostPositions[i]));
    }
    
    // Reset update counter when loading new level
    updateCounter = 0;
}

// Update game state
void Game::update() {
    if (state != GAME_RUNNING) {
        return;
    }
    
    // Speed control - only update game logic every N frames
    updateCounter++;
    if (updateCounter % updateFrequency != 0) {
        return; // Skip this update for slower speed
    }
    
    // Get ghost positions for AI
    std::vector<Position> ghostPositions;
    for (auto ghost : ghosts) {
        ghostPositions.push_back(ghost->getPosition());
    }
    
    // Update pacman AI
    pacman->updateAI(*maze, ghostPositions);
    
    // Move pacman
    pacman->move(*maze);
    
    // Check dot collection
    if (checkDotCollection()) {
        // Add score
        addScore(10);
        
        // Decrease dots remaining
        dotsRemaining--;
        
        // Check if level complete
        if (dotsRemaining <= 0) {
            state = LEVEL_COMPLETE;
        }
    }
    
    // Check power pellet collection
    if (checkPowerPelletCollection()) {
        // Add score
        addScore(50);
        
        // Set ghosts to frightened
        for (auto ghost : ghosts) {
            ghost->setState(FRIGHTENED);
        }
    }
    
    // Update ghost AI
    Position blinkyPos = ghosts.size() > 0 ? ghosts[0]->getPosition() : Position(-1, -1);
    
    for (auto ghost : ghosts) {
        ghost->updateAI(*maze, pacman->getPosition(), pacman->getDirection(), blinkyPos);
        ghost->move(*maze);
        
        // Update frightened timer
        if (ghost->isFrightened()) {
            ghost->updateFrightenedTimer();
        }
    }
    
    // Check ghost collision
    if (checkGhostCollision()) {
        // Check if ghost is frightened
        for (auto ghost : ghosts) {
            if (ghost->getPosition() == pacman->getPosition()) {
                if (ghost->isFrightened()) {
                    // Eat ghost
                    ghost->setState(EATEN);
                    ghost->resetPosition(maze->getGhostStartPositions()[0]);
                    
                    // Add score
                    addScore(200);
                } else if (ghost->getState() != EATEN) {
                    // Lose life
                    decreaseLives();
                    
                    // Reset positions
                    pacman->setPosition(maze->getPacmanStartPosition());
                    
                    for (size_t i = 0; i < ghosts.size(); i++) {
                        if (i < maze->getGhostStartPositions().size()) {
                            ghosts[i]->resetPosition(maze->getGhostStartPositions()[i]);
                        }
                    }
                    
                    // Check game over
                    if (lives <= 0) {
                        state = GAME_OVER;
                        gameOver = true;
                    }
                    
                    break;
                }
            }
        }
    }
}

// Render game
void Game::render() {
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Set up view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Render based on game state
    switch (state) {
        case GAME_START:
            // Render start screen
            renderStartScreen();
            break;
            
        case GAME_RUNNING:
            // Render maze
            renderMaze();
            
            // Render pacman
            renderPacman();
            
            // Render ghosts
            renderGhosts();
            
            // Render score and lives
            renderHUD();
            break;
            
        case GAME_PAUSED:
            // Render pause screen
            renderPauseScreen();
            break;
            
        case GAME_OVER:
            // Render game over screen
            renderGameOverScreen();
            break;
            
        case GAME_WIN:
            // Render win screen
            renderWinScreen();
            break;
            
        case LEVEL_COMPLETE:
            // Render level complete screen
            renderLevelCompleteScreen();
            break;
    }
}

// Handle input
void Game::handleInput(int key) {
    // Handle based on game state
    switch (state) {
        case GAME_START:
            // Start game on any key
            state = GAME_RUNNING;
            break;
            
        case GAME_RUNNING:
            // Handle movement keys
            switch (key) {
                case 'w':
                case 'W':
                    pacman->setDirection(UP);
                    break;
                    
                case 's':
                case 'S':
                    pacman->setDirection(DOWN);
                    break;
                    
                case 'a':
                case 'A':
                    pacman->setDirection(LEFT);
                    break;
                    
                case 'd':
                case 'D':
                    pacman->setDirection(RIGHT);
                    break;
                    
                case 'p':
                case 'P':
                    state = GAME_PAUSED;
                    break;
                    
                case ' ':
                    // Toggle AI
                    pacman->setAI(!pacman->isAIControlled());
                    break;
            }
            break;
            
        case GAME_PAUSED:
            // Resume on 'p'
            if (key == 'p' || key == 'P') {
                state = GAME_RUNNING;
            }
            break;
            
        case GAME_OVER:
            // Restart on any key
            init();
            break;
            
        case GAME_WIN:
            // Restart on any key
            init();
            break;
            
        case LEVEL_COMPLETE:
            // Advance to next level on any key
            advanceToNextLevel();
            break;
    }
}

// Check if game is over
bool Game::isGameOver() const {
    return gameOver;
}

// Check if level is complete
bool Game::isLevelComplete() const {
    return dotsRemaining <= 0;
}

// Advance to next level
bool Game::advanceToNextLevel() {
    // Check if we've reached max level
    if (level >= maxLevel) {
        state = GAME_WIN;
        return false;
    }
    
    // Load next level
    loadLevel(level + 1);
    
    // Set state to running
    state = GAME_RUNNING;
    
    return true;
}

// Reset level
void Game::resetLevel() {
    loadLevel(level);
}

// Get score
int Game::getScore() const {
    return score;
}

// Add score
void Game::addScore(int points) {
    score += points;
}

// Get level
int Game::getLevel() const {
    return level;
}

// Get state
GameState Game::getState() const {
    return state;
}

// Set state
void Game::setState(GameState newState) {
    state = newState;
}

// Get lives
int Game::getLives() const {
    return lives;
}

// Decrease lives
void Game::decreaseLives() {
    lives--;
}

// Check collisions
bool Game::checkCollisions() {
    return checkGhostCollision();
}

// Check dot collection
bool Game::checkDotCollection() {
    // Get pacman position
    Position pos = pacman->getPosition();
    
    // Check if dot
    if (maze->getCell(pos) == DOT) {
        // Remove dot
        maze->setCell(pos, EMPTY);
        return true;
    }
    
    return false;
}

// Check power pellet collection
bool Game::checkPowerPelletCollection() {
    // Get pacman position
    Position pos = pacman->getPosition();
    
    // Check if power pellet
    if (maze->getCell(pos) == POWER_PELLET) {
        // Remove power pellet
        maze->setCell(pos, EMPTY);
        return true;
    }
    
    return false;
}

// Check ghost collision
bool Game::checkGhostCollision() {
    // Get pacman position
    Position pos = pacman->getPosition();
    
    // Check if any ghost is at same position
    for (auto ghost : ghosts) {
        if (ghost->getPosition() == pos) {
            return true;
        }
    }
    
    return false;
}

// Render functions
void Game::renderStartScreen() {
    // Placeholder for start screen rendering
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(300, 300);
    const char* text = "Press any key to start";
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void Game::renderPauseScreen() {
    // Placeholder for pause screen rendering
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(300, 300);
    const char* text = "PAUSED - Press P to resume";
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void Game::renderGameOverScreen() {
    // Placeholder for game over screen rendering
    glColor3f(1.0f, 0.0f, 0.0f);
    glRasterPos2i(300, 300);
    const char* text = "GAME OVER - Press any key to restart";
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void Game::renderWinScreen() {
    // Placeholder for win screen rendering
    glColor3f(0.0f, 1.0f, 0.0f);
    glRasterPos2i(300, 300);
    const char* text = "YOU WIN! - Press any key to restart";
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void Game::renderLevelCompleteScreen() {
    // Placeholder for level complete screen rendering
    glColor3f(0.0f, 1.0f, 0.0f);
    glRasterPos2i(300, 300);
    char buffer[100];
    sprintf(buffer, "Level %d Complete! - Press any key to continue", level);
    for (const char* c = buffer; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void Game::renderMaze() {
    // Placeholder for maze rendering
    if (!maze) return;
    
    // Get maze dimensions
    int width = maze->getWidth();
    int height = maze->getHeight();
    
    // Calculate cell size
    float cellWidth = 800.0f / width;
    float cellHeight = 600.0f / height;
    
    // Render each cell
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Get cell type
            CellType cell = maze->getCell(x, y);
            
            // Set color based on cell type
            switch (cell) {
                case WALL:
                    glColor3f(0.0f, 0.0f, 1.0f); // Blue
                    break;
                    
                case PATH:
                    glColor3f(0.0f, 0.0f, 0.0f); // Black
                    break;
                    
                case DOT:
                    glColor3f(0.0f, 0.0f, 0.0f); // Black
                    break;
                    
                case POWER_PELLET:
                    glColor3f(0.0f, 0.0f, 0.0f); // Black
                    break;
                    
                case EMPTY:
                    glColor3f(0.0f, 0.0f, 0.0f); // Black
                    break;
            }
            
            // Draw cell
            glBegin(GL_QUADS);
            glVertex2f(x * cellWidth, y * cellHeight);
            glVertex2f((x + 1) * cellWidth, y * cellHeight);
            glVertex2f((x + 1) * cellWidth, (y + 1) * cellHeight);
            glVertex2f(x * cellWidth, (y + 1) * cellHeight);
            glEnd();
            
            // Draw dot or power pellet
            if (cell == DOT) {
                glColor3f(1.0f, 1.0f, 1.0f); // White
                glPointSize(3.0f);
                glBegin(GL_POINTS);
                glVertex2f(x * cellWidth + cellWidth / 2, y * cellHeight + cellHeight / 2);
                glEnd();
            } else if (cell == POWER_PELLET) {
                glColor3f(1.0f, 1.0f, 1.0f); // White
                glPointSize(8.0f);
                glBegin(GL_POINTS);
                glVertex2f(x * cellWidth + cellWidth / 2, y * cellHeight + cellHeight / 2);
                glEnd();
            }
        }
    }
}

void Game::renderPacman() {
    // Placeholder for pacman rendering
    if (!pacman || !maze) return;
    
    // Get pacman position
    Position pos = pacman->getPosition();
    
    // Calculate cell size
    float cellWidth = 800.0f / maze->getWidth();
    float cellHeight = 600.0f / maze->getHeight();
    
    // Set color
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    
    // Draw pacman
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(pos.x * cellWidth + cellWidth / 2, pos.y * cellHeight + cellHeight / 2);
    
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        float x = pos.x * cellWidth + cellWidth / 2 + cos(angle) * cellWidth / 2 * 0.8f;
        float y = pos.y * cellHeight + cellHeight / 2 + sin(angle) * cellHeight / 2 * 0.8f;
        glVertex2f(x, y);
    }
    
    glEnd();
}

void Game::renderGhosts() {
    // Placeholder for ghost rendering
    if (!maze) return;
    
    // Calculate cell size
    float cellWidth = 800.0f / maze->getWidth();
    float cellHeight = 600.0f / maze->getHeight();
    
    // Render each ghost
    for (auto ghost : ghosts) {
        // Get ghost position
        Position pos = ghost->getPosition();
        
        // Set color based on ghost type
        switch (ghost->getType()) {
            case BLINKY:
                glColor3f(1.0f, 0.0f, 0.0f); // Red
                break;
                
            case PINKY:
                glColor3f(1.0f, 0.75f, 0.8f); // Pink
                break;
                
            case INKY:
                glColor3f(0.0f, 1.0f, 1.0f); // Cyan
                break;
                
            case CLYDE:
                glColor3f(1.0f, 0.5f, 0.0f); // Orange
                break;
        }
        
        // If frightened, set to blue
        if (ghost->isFrightened()) {
            glColor3f(0.0f, 0.0f, 1.0f); // Blue
        }
        
        // If eaten, set to eyes only
        if (ghost->getState() == EATEN) {
            glColor3f(1.0f, 1.0f, 1.0f); // White
        }
        
        // Draw ghost
        glBegin(GL_QUADS);
        glVertex2f(pos.x * cellWidth + cellWidth * 0.2f, pos.y * cellHeight + cellHeight * 0.2f);
        glVertex2f(pos.x * cellWidth + cellWidth * 0.8f, pos.y * cellHeight + cellHeight * 0.2f);
        glVertex2f(pos.x * cellWidth + cellWidth * 0.8f, pos.y * cellHeight + cellHeight * 0.8f);
        glVertex2f(pos.x * cellWidth + cellWidth * 0.2f, pos.y * cellHeight + cellHeight * 0.8f);
        glEnd();
    }
}

void Game::renderHUD() {
    // Placeholder for HUD rendering
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Render score
    glRasterPos2i(10, 20);
    char scoreBuffer[50];
    sprintf(scoreBuffer, "Score: %d", score);
    for (const char* c = scoreBuffer; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    // Render level
    glRasterPos2i(10, 40);
    char levelBuffer[50];
    sprintf(levelBuffer, "Level: %d", level);
    for (const char* c = levelBuffer; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    // Render lives
    glRasterPos2i(10, 60);
    char livesBuffer[50];
    sprintf(livesBuffer, "Lives: %d", lives);
    for (const char* c = livesBuffer; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    // Render AI status
    glRasterPos2i(10, 80);
    const char* aiStatus = pacman->isAIControlled() ? "AI: ON (Space to toggle)" : "AI: OFF (Space to toggle)";
    for (const char* c = aiStatus; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}