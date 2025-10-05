#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glut.h>
#include <iostream>
#include <ctime>
#include "../include/game.h"

// Global game instance
Game* game = nullptr;

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Function prototypes
void display();
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void timer(int value);
void init();

// Main function
int main(int argc, char** argv) {
    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Pac-Man AI Game");
    
    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(50, timer, 0); // Changed from 100 to 50 for smoother rendering
    
    // Initialize game
    init();
    
    // Start main loop
    glutMainLoop();
    
    // Clean up
    delete game;
    
    return 0;
}

// Initialize game
void init() {
    // Set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Create game instance
    game = new Game();
    game->init();
}

// Display callback
void display() {
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Render game
    if (game) {
        game->render();
    }
    
    // Swap buffers
    glutSwapBuffers();
}

// Reshape callback
void reshape(int width, int height) {
    // Set viewport
    glViewport(0, 0, width, height);
    
    // Set projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    
    // Set model view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Keyboard callback
void keyboard(unsigned char key, int x, int y) {
    // Handle keyboard input
    if (game) {
        game->handleInput(key);
    }
    
    // Quit on ESC
    if (key == 27) {
        delete game;
        exit(0);
    }
    
    // Restart on 'r'
    if (key == 'r' || key == 'R') {
        delete game;
        game = new Game();
        game->init();
    }
    
    // Speed control keys
    if (key == '+' || key == '=') {
        // Speed up - you can implement this if needed
    }
    if (key == '-' || key == '_') {
        // Slow down - you can implement this if needed
    }
    
    // Redisplay
    glutPostRedisplay();
}

// Special keys callback
void specialKeys(int key, int x, int y) {
    // Handle arrow keys
    Direction dir = NONE;
    
    switch (key) {
        case GLUT_KEY_UP:
            dir = UP;
            break;
        case GLUT_KEY_DOWN:
            dir = DOWN;
            break;
        case GLUT_KEY_LEFT:
            dir = LEFT;
            break;
        case GLUT_KEY_RIGHT:
            dir = RIGHT;
            break;
    }
    
    // Update pacman direction
    if (game && dir != NONE) {
        game->handleInput(' '); // This will be handled by the game class
    }
    
    // Redisplay
    glutPostRedisplay();
}

// Timer callback
void timer(int value) {
    // Update game
    if (game) {
        game->update();
    }
    
    // Redisplay
    glutPostRedisplay();
    
    // Register next timer - keep it at 50ms for smooth rendering
    glutTimerFunc(50, timer, 0);
}