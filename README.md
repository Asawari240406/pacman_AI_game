# Pac-Man AI Game

A Pac-Man game implementation with AI techniques for both Pac-Man and ghosts, built with C++ and GLUT/OpenGL.

## Features

- Classic Pac-Man gameplay with multiple levels
- AI-controlled ghosts using A* pathfinding algorithm
- Optional AI-controlled Pac-Man using BFS algorithm
- 5 progressively challenging levels
- Power pellets that allow Pac-Man to eat ghosts
- Score tracking and lives system

## AI Techniques

- **Ghosts AI**: Uses A* pathfinding with different targeting strategies:
  - Blinky (Red): Directly targets Pac-Man
  - Pinky (Pink): Targets 4 tiles ahead of Pac-Man
  - Inky (Blue): Uses a combination of Pac-Man and Blinky's positions
  - Clyde (Orange): Targets Pac-Man when far, runs away when close

- **Pac-Man AI**: Uses BFS (Breadth-First Search) for:
  - Avoiding nearby ghosts
  - Finding the nearest dot or power pellet

## Prerequisites

- C++ compiler (MinGW GCC 6.3.0 or later)
- GLUT/OpenGL libraries
- Make utility (for building with the Makefile)

## Building and Running

1. Make sure you have all prerequisites installed
2. Build the project using Make:
   ```
   make
   ```
3. Run the game:
   ```
   make run
   ```
   
   Or directly:
   ```
   bin/pacman
   ```

   or run the game directly:

   ```
   g++ -Iinclude -o bin/pacman src/*.cpp -lfreeglut -lopengl32 -lglu32
   ```

   ```
   bin/pacman
   ```

## Controls

- Arrow keys: Move Pac-Man
- P: Pause game
- R: Restart current level
- A: Toggle AI control for Pac-Man
- ESC: Quit game

## Project Structure

- `include/`: Header files
- `src/`: Source files
- `assets/`: Game resources (fonts, sounds, etc.)
- `bin/`: Compiled executable
- `obj/`: Object files generated during compilation

## Output

Below are some screenshots showing the gameplay and AI in action:

![Gameplay Screenshot 1](assets/screenshots/game1.png)
![Gameplay Screenshot 2](assets/screenshots/game2.png)

## License

This project is created for educational purposes.