#ifndef GRID_H
#define GRID_H

#include <cstdlib>

#include "node.hh"

#define DISPLAY_WIDTH 800  // Define display width, adjust as necessary
#define DISPLAY_HEIGHT 600 // Define display height, adjust as necessary

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

    // Constructor
    Grid(int width, int height);

    // Function to draw the entire grid
    void drawGrid(SDL_Renderer *ren);

    // Function to draw hovered node
    void drawHovered(SDL_Renderer *ren, Node *node, const Uint8 *keys);

    // Destructor
    ~Grid();
};

#endif // GRID_H
