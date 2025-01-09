#include "../include/grid.hh"

Grid::Grid(int width, int height) : width(width), height(height) {
    nodes = (Node **)malloc(width * sizeof(Node *));
    for (int x = 0; x < width; x++) {
        nodes[x] = (Node *)malloc(height * sizeof(Node));
        for (int y = 0; y < height; y++) {
            nodes[x][y] = Node(x, y);
        }
    }
}

void Grid::drawGridLines(SDL_Renderer *ren) {
    int nodeSize = DISPLAY_WIDTH / width;
    for (int x = 1; x < width; x++) {
        for (int y = 1; y < height; y++) {
            SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
            SDL_RenderDrawLine(ren, x * nodeSize, 0, x * nodeSize, DISPLAY_HEIGHT);
            SDL_RenderDrawLine(ren, 0, y * nodeSize, DISPLAY_WIDTH, y * nodeSize);
        }
    }
}

void Grid::drawNodes(SDL_Renderer *ren) {
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

void Grid::draw(SDL_Renderer *ren) {
    drawGridLines(ren);
    drawNodes(ren);
}

Grid::~Grid() {
    for (int x = 0; x < width; x++) {
        free(nodes[x]);
    }
    free(nodes);
}
