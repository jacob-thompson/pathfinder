#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "grid.hh"

class Pathfinder {
private:
    bool error;
    bool running;
    bool selectingDijkstra;
    bool selectingAStar;
    bool selectingDiag;
    bool selectingReset;
    bool selectingRun;

    // Compute the path using Dijkstra's algorithm
    void dijkstra();

    // Compute the path using A* algorithm
    void aStar();

    // Reset the unvisited nodes
    void resetUnvisitedNodes();

    // Get the node with the lowest distance
    Node *getLowestDistanceNode();

    // Write the distances to the neighbors of the current node
    void writeNeighborDistances(Node *current);

    // Get computed path
    void writePath();

public:
    Grid map;
    Input user;
    SDL_Window *win;
    SDL_Renderer *ren;
    std::set<Node *> unvisited;
    std::set<Node *> path;
    bool configMenu;
    bool pathfinding;

    // Constructor
    Pathfinder();

    // Check if there was an error initializing
    bool isInitError();

    // Check if the program is running
    bool isRunning();

    // Handles an event
    void handleEvent(SDL_Event e);

    // Updates the user's hovered node
    void setHoveredNode();

    // Modify the state of the hovered node based on input
    void modifyHoveredNode(const Uint8 *keys);

    // Destructor
    ~Pathfinder();
};

#endif // PATHFINDER_H
