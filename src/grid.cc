#include "../include/grid.hh"

Grid::Grid(int width, int height) : width(width), height(height)
{
    startNode = nullptr;
    endNode = nullptr;
    nodes = (Node **)malloc(width * sizeof(Node *));
    for (int x = 0; x < width; x++) {
        nodes[x] = (Node *)malloc(height * sizeof(Node));
        for (int y = 0; y < height; y++) {
            nodes[x][y] = Node(x, y);
        }
    }
}

void Grid::reset()
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            nodes[x][y].setToDefault();

    startNode = nullptr;
    endNode = nullptr;
}

void Grid::drawBackground(SDL_Renderer *ren)
{
    SDL_SetRenderDrawColor(ren, 175, 175, 175, 255);
    SDL_RenderClear(ren);
}

void Grid::drawGridLines(SDL_Renderer *ren)
{
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
            SDL_RenderDrawLine(ren, x * NODE_SIZE, 0, x * NODE_SIZE, DISPLAY_HEIGHT);
            SDL_RenderDrawLine(ren, 0, y * NODE_SIZE, DISPLAY_WIDTH, y * NODE_SIZE);
        }
    }
}

void Grid::drawNodes(SDL_Renderer *ren)
{
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            SDL_SetRenderDrawColor(
                ren,
                nodes[x][y].color.r,
                nodes[x][y].color.g,
                nodes[x][y].color.b,
                nodes[x][y].color.a
            );
            SDL_RenderFillRect(ren, &nodes[x][y].rect);
        }
    }
}

void Grid::drawGrid(SDL_Renderer *ren)
{
    drawBackground(ren);
    drawGridLines(ren);
    drawNodes(ren);
}

void Grid::drawHovered(SDL_Renderer *ren, Node *node, const Uint8 *keys)
{
    if (keys[SDL_SCANCODE_LSHIFT])
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 75);
    else if (keys[SDL_SCANCODE_LCTRL])
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 75);
    else
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 75);

    if (node != nullptr)
        SDL_RenderFillRect(ren, &node->rect);
}

void Grid::drawPath(SDL_Renderer *ren, std::set<Node *> path)
{
    for (Node *node : path) {
        SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
        SDL_RenderFillRect(ren, &node->rect);
    }
}

Grid::~Grid()
{
    for (int x = 0; x < width; x++) {
        free(nodes[x]);
    }
    free(nodes);
}
