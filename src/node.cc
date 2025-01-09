#include "../include/node.hh"

Node::Node(int x, int y) : x(x), y(y)
{
    g = INT_MAX;
    h = INT_MAX;
    f = INT_MAX;
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 0;
    rect.x = x * NODE_SIZE + 1;
    rect.y = y * NODE_SIZE + 1;
    rect.w = NODE_SIZE - 1;
    rect.h = NODE_SIZE - 1;
}
