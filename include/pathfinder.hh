#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <condition_variable>
#include <mutex>
#include <thread>

#include "grid.hh"

#define DELAY 5

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

    // Calculate the heuristic value (H) used in A* algorithm
    void calculateH(Node *current, Node *end);

    // Reset the unvisited nodes
    void resetUnvisitedNodes();

    // Get the node with the lowest G Score (distance from start)
    Node *getLowestGScoreNode();

    // Get the node with the lowest F Score (G + H)
    Node *getLowestFScoreNode();

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
    std::thread pathfinderThread; // Thread for pathfinding
    std::mutex pathMutex;       // Mutex to protect shared resources
    std::condition_variable cv; // Condition variable for synchronization
    bool stepCompleted = false; // Flag to indicate if a step is completed
    bool terminateThread = false; // Flag to indicate if the thread should terminate


    // Constructor
    Pathfinder();

    // Check if there was an error initializing
    bool isInitError();

    // Check if the program is running
    bool isRunning();

    // Kill the pathfinder thread
    void killThread();

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
