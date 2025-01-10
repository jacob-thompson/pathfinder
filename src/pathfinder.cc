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

    if (e.type == SDL_KEYUP) {
        if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q)
            running = false;
        if (e.key.keysym.sym == SDLK_z)
            map.reset();
    }

    if (e.type == SDL_MOUSEMOTION) {
        user.pos->x = e.motion.x;
        user.pos->y = e.motion.y;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT)
            user.leftClick = true;
        if (e.button.button == SDL_BUTTON_RIGHT)
            user.rightClick = true;
    }

    if (e.type == SDL_MOUSEBUTTONUP) {
        if (e.button.button == SDL_BUTTON_LEFT)
            user.leftClick = false;
        if (e.button.button == SDL_BUTTON_RIGHT)
            user.rightClick = false;
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

void Pathfinder::modifyHoveredNode(const Uint8 *keys)
{
    if (user.leftClick && keys[SDL_SCANCODE_LSHIFT] && user.hoveredNode != nullptr) {
        bool newStart = true;
        if (map.startNode != nullptr) {
            newStart = map.startNode->x != user.hoveredNode->x || map.startNode->y != user.hoveredNode->y;
            map.startNode->setToDefault();
        }
        map.startNode = user.hoveredNode;
        user.hoveredNode->setToStart();
        if (newStart) {
            SDL_LogInfo(
                SDL_LOG_CATEGORY_APPLICATION,
                "Start node set: (%d, %d)\n",
                map.startNode->x,
                map.startNode->y
            );
        }
    } else if (user.leftClick && keys[SDL_SCANCODE_LCTRL] && user.hoveredNode != nullptr) {
        bool newEnd = true;
        if (map.endNode != nullptr) {
            newEnd = map.endNode->x != user.hoveredNode->x || map.endNode->y != user.hoveredNode->y;
            map.endNode->setToDefault();
        }
        map.endNode = user.hoveredNode;
        user.hoveredNode->setToEnd();
        if (newEnd) {
            SDL_LogInfo(
                SDL_LOG_CATEGORY_APPLICATION,
                "End node set: (%d, %d)\n",
                map.endNode->x,
                map.endNode->y
            );
        }
    } else if (user.leftClick && user.hoveredNode != nullptr) {
        user.hoveredNode->setToWall();
    } else if (user.rightClick && user.hoveredNode != nullptr) {
        user.hoveredNode->setToDefault();
    }
}

Pathfinder::~Pathfinder() {
    delete user.pos;
}
