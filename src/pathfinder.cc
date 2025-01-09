#include "../include/pathfinder.hh"

Pathfinder::Pathfinder() : map(DISPLAY_WIDTH / 10, DISPLAY_HEIGHT / 10) {
    user.hoveredNode = nullptr;
    user.x = 0;
    user.y = 0;
    error = false;
    win = SDL_CreateWindow(
        "pathfinder",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (win == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        error = true;
    }
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        error = true;
    }
    if (SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_SetRenderDrawBlendMode Error: %s\n", SDL_GetError());
        error = true;
    }
}

Pathfinder::~Pathfinder() {}
