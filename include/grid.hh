#ifndef GRID_H
#define GRID_H

#include <cstdlib>

#include "node.hh"

#define DISPLAY_WIDTH 800  // Define display width, adjust as necessary
#define DISPLAY_HEIGHT 600 // Define display height, adjust as necessary

class Grid {
public:
    int width, height;
    Node **nodes;

    // Constructor
    Grid(int width, int height);

    // Function to draw grid lines
    void drawGridLines(SDL_Renderer *ren);

    // Function to draw nodes
    void drawNodes(SDL_Renderer *ren);

    // Function to draw the entire grid
    void draw(SDL_Renderer *ren);

    // Destructor
    ~Grid();
};

#endif // GRID_H
