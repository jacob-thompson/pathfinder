#include "../include/pathfinder.hh"

Pathfinder::Pathfinder() : map(DISPLAY_WIDTH / 10, DISPLAY_HEIGHT / 10)
{
    user.hoveredNode = nullptr;
    user.pos = new SDL_Point();
    error = false;
    running = true;
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

bool Pathfinder::isInitError()
{
    return error;
}

bool Pathfinder::isRunning()
{
    return running;
}

void Pathfinder::handleEvent(SDL_Event e)
{
    if (e.type == SDL_QUIT)
        running = false;

    if (e.type == SDL_KEYUP)
        if (e.key.keysym.sym == SDLK_ESCAPE)
            running = false;

    if (e.type == SDL_MOUSEMOTION) {
        user.pos->x = e.motion.x;
        user.pos->y = e.motion.y;
    }
}

void Pathfinder::updateHoveredNode()
{
    user.hoveredNode = nullptr;
    for (int x = 0; x < map.width; x++) {
        for (int y = 0; y < map.height; y++) {
            if (SDL_PointInRect(user.pos, &map.nodes[x][y].rect)) {
                user.hoveredNode = &map.nodes[x][y];
                break;
            }
        }
        if (user.hoveredNode != nullptr)
            break;
    }
}

Pathfinder::~Pathfinder() {
    delete user.pos;
}
