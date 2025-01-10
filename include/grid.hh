#ifndef GRID_H
#define GRID_H

#include <cstdlib>

#include "node.hh"

#define DISPLAY_WIDTH 800  // window width
#define DISPLAY_HEIGHT 600 // window height

class Grid {
private:
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

    // Constructor
    Grid(int width, int height);

    // Reset the grid to its default state
    void reset();

    // Draw the grid
    void drawGrid(SDL_Renderer *ren);

    // Draw the hovered node
    void drawHovered(SDL_Renderer *ren, Node *node, const Uint8 *keys);

    // Destructor
    ~Grid();
};

#endif // GRID_H
