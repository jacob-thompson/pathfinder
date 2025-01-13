#include "../include/grid.hh"

Grid::Grid(int width, int height) : width(width), height(height)
{
    dijkstra = true;
    aStar = false;
    if (TTF_Init() != 0) {
        error = true;
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to initialize SDL_ttf: %s\n",
            TTF_GetError()
        );
    } else {
        error = false;
        SDL_LogInfo(
            SDL_LOG_CATEGORY_APPLICATION,
            "SDL_ttf initialized\n"
        );

        font = TTF_OpenFont("../resources/OpenSans-Regular.ttf", 24);
        if (!font) {
            error = true;
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Failed to load font: %s\n",
                TTF_GetError()
            );
        } else {
            SDL_LogInfo(
                SDL_LOG_CATEGORY_APPLICATION,
                "Font loaded\n"
            );
        }
    }

    configMenuWidth = 200;
    configMenuHeight = 400;

    configMenuRect = {
        (DISPLAY_WIDTH >> 1) - (configMenuWidth >> 1),
        (DISPLAY_HEIGHT >> 1) - (configMenuHeight >> 1),
        configMenuWidth,
        configMenuHeight
    };

    runButton = {
        configMenuRect.x + 10,
        configMenuRect.y + configMenuRect.h - 60,
        configMenuRect.w - 20,
        50
    };

    resetButton = {
        configMenuRect.x + 10,
        configMenuRect.y + configMenuRect.h - 120,
        configMenuRect.w - 20,
        50
    };

    dijkstraCheckbox = {
        configMenuRect.x + 10,
        configMenuRect.y + 10,
        35,
        35
    };

    aStarCheckbox = {
        configMenuRect.x + 10,
        configMenuRect.y + 55,
        35,
        35
    };

    diagCheckbox = {
        configMenuRect.x + 10,
        configMenuRect.y + 100,
        35,
        35
    };

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

bool Grid::isInitError()
{
    return error;
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

void Grid::drawConfigMenu(SDL_Renderer *ren, SDL_Point *pos, bool leftClicking)
{
    // Draw menu background
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderFillRect(ren, &configMenuRect);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderDrawRect(ren, &configMenuRect);

    // Run button
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderDrawRect(ren, &runButton);

    if (SDL_PointInRect(pos, &runButton) && leftClicking) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 150);
        SDL_RenderFillRect(ren, &runButton);
    } else if (SDL_PointInRect(pos, &runButton)) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 75);
        SDL_RenderFillRect(ren, &runButton);
    }

    SDL_Surface *runSurface = TTF_RenderText_Solid(font, "Run", {0, 0, 0, 255});
    SDL_Texture *runTexture = SDL_CreateTextureFromSurface(ren, runSurface);
    SDL_Rect runLabel = {
        runButton.x + (runButton.w >> 1) - (runSurface->w >> 1),
        runButton.y + (runButton.h >> 1) - (runSurface->h >> 1),
        runSurface->w,
        runSurface->h
    };
    SDL_RenderCopy(ren, runTexture, NULL, &runLabel);
    SDL_FreeSurface(runSurface);
    SDL_DestroyTexture(runTexture);

    // Reset button
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderDrawRect(ren, &resetButton);

    if (SDL_PointInRect(pos, &resetButton) && leftClicking) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 150);
        SDL_RenderFillRect(ren, &resetButton);
    } else if (SDL_PointInRect(pos, &resetButton)) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 75);
        SDL_RenderFillRect(ren, &resetButton);
    }

    SDL_Surface *resetSurface = TTF_RenderText_Solid(font, "Reset", {0, 0, 0, 255});
    SDL_Texture *resetTexture = SDL_CreateTextureFromSurface(ren, resetSurface);
    SDL_Rect resetLabel = {
        resetButton.x + (resetButton.w >> 1) - (resetSurface->w >> 1),
        resetButton.y + (resetButton.h >> 1) - (resetSurface->h >> 1),
        resetSurface->w,
        resetSurface->h
    };
    SDL_RenderCopy(ren, resetTexture, NULL, &resetLabel);
    SDL_FreeSurface(resetSurface);
    SDL_DestroyTexture(resetTexture);

    // Dijkstra checkbox
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderDrawRect(ren, &dijkstraCheckbox);

    if (SDL_PointInRect(pos, &dijkstraCheckbox) && leftClicking) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 150);
        SDL_RenderFillRect(ren, &dijkstraCheckbox);
    } else if (SDL_PointInRect(pos, &dijkstraCheckbox)) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 75);
        SDL_RenderFillRect(ren, &dijkstraCheckbox);
    }

    if (dijkstra) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &dijkstraCheckbox);
    }

    SDL_Surface *dijkstraSurface = TTF_RenderText_Solid(font, "Dijkstra", {0, 0, 0, 255});
    SDL_Texture *dijkstraTexture = SDL_CreateTextureFromSurface(ren, dijkstraSurface);
    SDL_Rect dijkstraLabel = {
        dijkstraCheckbox.x + dijkstraCheckbox.w + 10,
        dijkstraCheckbox.y + (dijkstraCheckbox.h >> 1) - (dijkstraSurface->h >> 1),
        dijkstraSurface->w,
        dijkstraSurface->h
    };
    SDL_RenderCopy(ren, dijkstraTexture, NULL, &dijkstraLabel);
    SDL_FreeSurface(dijkstraSurface);
    SDL_DestroyTexture(dijkstraTexture);

    // A* checkbox
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderDrawRect(ren, &aStarCheckbox);

    if (SDL_PointInRect(pos, &aStarCheckbox) && leftClicking) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 150);
        SDL_RenderFillRect(ren, &aStarCheckbox);
    } else if (SDL_PointInRect(pos, &aStarCheckbox)) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 75);
        SDL_RenderFillRect(ren, &aStarCheckbox);
    }

    if (aStar) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &aStarCheckbox);
    }

    SDL_Surface *aStarSurface = TTF_RenderText_Solid(font, "A*", {0, 0, 0, 255});
    SDL_Texture *aStarTexture = SDL_CreateTextureFromSurface(ren, aStarSurface);
    SDL_Rect aStarLabel = {
        aStarCheckbox.x + aStarCheckbox.w + 10,
        aStarCheckbox.y + (aStarCheckbox.h >> 1) - (aStarSurface->h >> 1),
        aStarSurface->w,
        aStarSurface->h
    };
    SDL_RenderCopy(ren, aStarTexture, NULL, &aStarLabel);
    SDL_FreeSurface(aStarSurface);
    SDL_DestroyTexture(aStarTexture);

    // Diagonal traversal checkbox
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderDrawRect(ren, &diagCheckbox);

    if (SDL_PointInRect(pos, &diagCheckbox) && leftClicking) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 150);
        SDL_RenderFillRect(ren, &diagCheckbox);
    } else if (SDL_PointInRect(pos, &diagCheckbox)) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 75);
        SDL_RenderFillRect(ren, &diagCheckbox);
    }

    if (diagonalTraversal) {
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &diagCheckbox);
    }

    SDL_Surface *diagSurface = TTF_RenderText_Solid(font, "Diagonal", {0, 0, 0, 255});
    SDL_Texture *diagTexture = SDL_CreateTextureFromSurface(ren, diagSurface);
    SDL_Rect diagLabel = {
        diagCheckbox.x + diagCheckbox.w + 10,
        diagCheckbox.y + (diagCheckbox.h >> 1) - (diagSurface->h >> 1),
        diagSurface->w,
        diagSurface->h
    };
    SDL_RenderCopy(ren, diagTexture, NULL, &diagLabel);
    SDL_FreeSurface(diagSurface);
    SDL_DestroyTexture(diagTexture);
}

Grid::~Grid()
{
    for (int x = 0; x < width; x++) {
        free(nodes[x]);
    }
    free(nodes);
    TTF_CloseFont(font);
    TTF_Quit();
}
