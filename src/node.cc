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
    isWall = false;
    isStart = false;
    isEnd = false;
}

void Node::setToWall()
{
    color.r = 0;
    color.g = 0;
    color.b = 0;
    color.a = 255;
    isWall = true;
    isStart = false;
    isEnd = false;
}

void Node::setToDefault()
{
    color.r = 255;
    color.g = 255;
    color.b = 255;
    color.a = 0;
    isWall = false;
    isStart = false;
    isEnd = false;
}

void Node::setToStart()
{
    color.r = 0;
    color.g = 255;
    color.b = 0;
    color.a = 255;
    isWall = false;
    isStart = true;
    isEnd = false;
}

void Node::setToEnd()
{
    color.r = 255;
    color.g = 0;
    color.b = 0;
    color.a = 255;
    isWall = false;
    isStart = false;
    isEnd = true;
}
