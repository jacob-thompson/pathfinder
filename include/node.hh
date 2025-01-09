#ifndef NODE_H
#define NODE_H

#include <climits>

#include "SDL.h"

#define NODE_SIZE 20

class Node {
public:
    int x, y;
    int g, h, f;
    SDL_Color color;
    SDL_Rect rect;

    // Constructor
    Node(int x, int y);
};

typedef struct {
    SDL_Point *pos;
    Node *hoveredNode;
} Input;

#endif // NODE_H
