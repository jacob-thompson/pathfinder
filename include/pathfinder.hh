#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "grid.hh"

class Pathfinder {
private:
    bool error;
    bool running;
public:
    Grid map;
    Input user;
    SDL_Window *win;
    SDL_Renderer *ren;

    // Constructor
    Pathfinder();

    // Check if there was an error initializing
    bool isInitError();

    // Check if the program is running
    bool isRunning();

    // Handles an event
    void handleEvent(SDL_Event e);

    // Updates the user's hovered node
    void updateHoveredNode();

    // Destructor
    ~Pathfinder();
};

#endif // PATHFINDER_H
