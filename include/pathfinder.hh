#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "grid.hh"

class Pathfinder {
public:
    Grid map;
    Input user;
    SDL_Window *win;
    SDL_Renderer *ren;
    bool error;

    // Constructor
    Pathfinder();

    // Destructor
    ~Pathfinder();
};

#endif // PATHFINDER_H
