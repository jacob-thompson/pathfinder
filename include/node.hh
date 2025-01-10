#ifndef NODE_H
#define NODE_H

#include <climits>

#include "SDL.h"

#define NODE_SIZE 20 // width and height of each node

class Node {
public:
    int x, y;
    int g, h, f;
    SDL_Color color;
    SDL_Rect rect;
    bool isWall;
    bool isStart;
    bool isEnd;

    // Constructor
    Node(int x, int y);

    // Set node to wall (non-traversable)
    void setToWall();

    // Set node to default state
    // not wall, not start, not end
    void setToDefault();

    // Set node to start node
    void setToStart();

    // Set node to end node
    void setToEnd();
};

typedef struct {
    SDL_Point *pos;
    Node *hoveredNode;
    bool leftClick;
    bool rightClick;
} Input;

#endif // NODE_H
