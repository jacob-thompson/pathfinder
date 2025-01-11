#include "../include/pathfinder.hh"

Pathfinder::Pathfinder() : map(DISPLAY_WIDTH / NODE_SIZE, DISPLAY_HEIGHT / NODE_SIZE)
{
    user.hoveredNode = nullptr;
    user.pos = new SDL_Point();
    error = false;
    running = true;
    configMenu = false;
    pathfinding = false;
    win = SDL_CreateWindow(
        "pathfinder",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (map.isInitError())
        error = true;

    if (!win) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        error = true;
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
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
        if (!path.empty())
            path.clear();
        if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q)
            running = false;
        if (e.key.keysym.sym == SDLK_a) {
            if (user.leftClick || user.rightClick)
                return;
            aStar();
        }
        if (e.key.keysym.sym == SDLK_c)
            configMenu = !configMenu;
        if (e.key.keysym.sym == SDLK_d) {
            if (user.leftClick || user.rightClick)
                return;
            dijkstra();
        }
        if (e.key.keysym.sym == SDLK_z)
            map.reset();
    }

    if (e.type == SDL_MOUSEMOTION) {
        user.pos->x = e.motion.x;
        user.pos->y = e.motion.y;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (!path.empty())
            path.clear();
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

        if (configMenu && SDL_PointInRect(user.pos, &map.dijkstraCheckbox)) {
            map.dijkstra = true;
            map.aStar = !map.dijkstra;
        }

        if (configMenu && SDL_PointInRect(user.pos, &map.aStarCheckbox)) {
            map.aStar = true;
            map.dijkstra = !map.aStar;
        }

        if (configMenu && SDL_PointInRect(user.pos, &map.runButton)) {
            configMenu = false;
            if (map.dijkstra)
                dijkstra();
            if (map.aStar)
                aStar();
        }

        if (configMenu && SDL_PointInRect(user.pos, &map.resetButton)) {
            map.reset();
        }
    }
}

void Pathfinder::setHoveredNode()
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
        if (user.hoveredNode == map.startNode) {
            map.startNode = nullptr;
        } else if (user.hoveredNode == map.endNode) {
            map.endNode = nullptr;
        }
        user.hoveredNode->setToDefault();
    }
}

void Pathfinder::resetUnvisitedNodes()
{
    unvisited.clear();
    for (int x = 0; x < map.width; x++) {
        for (int y = 0; y < map.height; y++) {
            if (&map.nodes[x][y] == map.startNode) {
                map.nodes[x][y].g = 0;
            } else {
                map.nodes[x][y].g = INT_MAX;
            }
            unvisited.insert(&map.nodes[x][y]);
        }
    }
}

Node *Pathfinder::getLowestDistanceNode()
{
    Node *lowest = nullptr;
    for (Node *node : unvisited) {
        if (lowest == nullptr || node->g < lowest->g)
            lowest = node;
    }
    return lowest;
}

void Pathfinder::writeNeighborDistances(Node *current)
{
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int x = current->x + i;
            int y = current->y + j;
            if (x < 0 || x >= map.width || y < 0 || y >= map.height)
                continue;
            Node *neighbor = &map.nodes[x][y];
            if (neighbor->isWall)
                continue;
            int newG = current->g + 1;
            neighbor->g = newG < neighbor->g ? newG : neighbor->g;
        }
    }
}

void Pathfinder::writePath()
{
    Node *current = map.endNode;
    while (current != nullptr) {
        path.insert(current);
        if (current == map.startNode)
            break;
        Node *lowest = nullptr;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int x = current->x + i;
                int y = current->y + j;
                if (x < 0 || x >= map.width || y < 0 || y >= map.height )
                    continue;
                Node *neighbor = &map.nodes[x][y];
                if (neighbor->isWall)
                    continue;
                if (lowest == nullptr || neighbor->g < lowest->g)
                    lowest = neighbor;
            }
        }
        current = lowest;
    }
}

void Pathfinder::dijkstra()
{
    if (map.startNode == nullptr || map.endNode == nullptr) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Start and end nodes must be set before running Dijkstra's algorithm\n"
        );
        return;
    }

    path.clear();
    pathfinding = true;

    resetUnvisitedNodes();

    while (!unvisited.empty()) {
        Node *current = getLowestDistanceNode();
        if (current == nullptr || current->g == INT_MAX) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "No path found\n");
            break;
        }

        unvisited.erase(current);

        if (current == map.endNode)
            break;

        writeNeighborDistances(current);
    }


    if (map.endNode->g < INT_MAX)
        writePath();

    pathfinding = false;
}

void Pathfinder::aStar()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "A* algorithm not implemented yet\n");
}

Pathfinder::~Pathfinder() {
    delete user.pos;
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
}
