#ifndef GRID_H
#define GRID_H

#include <cstdlib>
#include <set>

#include "node.hh"

#include "SDL_ttf.h"

#define DISPLAY_WIDTH 800  // window width
#define DISPLAY_HEIGHT 600 // window height

class Grid {
private:
    int configMenuWidth; // width of the configuration menu
    int configMenuHeight; // height of the configuration menu
    SDL_Rect configMenuRect; // rectangle for the configuration menu
    TTF_Font *font;  // font for the text
    bool error; // flag for initialization errors

    // Function to draw background
    void drawBackground(SDL_Renderer *ren);

    // Function to draw grid lines
    void drawGridLines(SDL_Renderer *ren);

    // Function to draw nodes
    void drawNodes(SDL_Renderer *ren);
public:
    int width, height; // number of nodes in the x and y directions
    Node **nodes; // 2D array of nodes
    Node *startNode; // points to the start node
    Node *endNode;   // points to the end node
    SDL_Rect runButton; // rectangle for the run button
    SDL_Rect resetButton; // rectangle for the reset button
    SDL_Rect dijkstraCheckbox; // rectangle for the Dijkstra checkbox
    SDL_Rect aStarCheckbox; // rectangle for the A* checkbox
    SDL_Rect diagCheckbox; // rectangle for the diagonal traversal checkbox
    bool dijkstra; // flag for Dijkstra's algorithm
    bool aStar; // flag for A* algorithm
    bool diagonalTraversal; // flag for diagonal traversal

    // Constructor
    Grid(int width, int height);

    // Check if there was an initialization error
    bool isInitError();

    // Reset the grid to its default state
    void reset();

    // Draw the grid
    void drawGrid(SDL_Renderer *ren);

    // Draw the hovered node
    void drawHovered(
        SDL_Renderer *ren,
        Node *node,
        const Uint8 *keys,
        SDL_Scancode startMod,
        SDL_Scancode endMod);

    // Draw the path
    void drawPath(SDL_Renderer *ren, std::set<Node *> path);

    // Draw the configuration menu
    void drawConfigMenu(SDL_Renderer *ren, SDL_Point *pos, bool leftClicking);

    // Draw the visualization
    void drawVisualization(SDL_Renderer *ren);

    // Destructor
    ~Grid();
};

#endif // GRID_H
