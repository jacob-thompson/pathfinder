#include "../include/pathfinder.hh"

Pathfinder::Pathfinder() : map(DISPLAY_WIDTH / NODE_SIZE, DISPLAY_HEIGHT / NODE_SIZE)
{
    user.hoveredNode = nullptr;
    user.pos = new SDL_Point();
    error = false;
    running = true;
    selectingDijkstra = false;
    selectingAStar = false;
    selectingDiag = false;
    selectingReset = false;
    selectingRun = false;
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

    if (pathfinderThread.joinable())
        pathfinderThread.join();
}

bool Pathfinder::isInitError()
{
    return error;
}

bool Pathfinder::isRunning()
{
    return running;
}

void Pathfinder::killThread()
{
    {
        std::lock_guard<std::mutex> lock(pathMutex);
        terminateThread = true;
        cv.notify_all();
    }
    pathfinderThread.join();
    terminateThread = false;
}

void Pathfinder::handleEvent(SDL_Event e)
{
    if (e.type == SDL_QUIT) {
        running = false;
    } else if (e.type == SDL_KEYUP) {
        if (!path.empty())
            path.clear();

        if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q) {
            running = false;
        } else if (e.key.keysym.sym == SDLK_a && !pathfinding) {
            if (user.leftClick || user.rightClick)
                return;

            configMenu = false;
            if (pathfinderThread.joinable())
                killThread();
            pathfinderThread = std::thread(&Pathfinder::aStar, this);
        } else if (e.key.keysym.sym == SDLK_c) {
            configMenu = !configMenu;
        } else if (e.key.keysym.sym == SDLK_d && !pathfinding) {
            if (user.leftClick || user.rightClick)
                return;

            configMenu = false;
            if (pathfinderThread.joinable())
                killThread();
            pathfinderThread = std::thread(&Pathfinder::dijkstra, this);
        } else if (e.key.keysym.sym == SDLK_z) {
            map.reset();
        }
    } else if (e.type == SDL_MOUSEMOTION) {
        user.pos->x = e.motion.x;
        user.pos->y = e.motion.y;
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (!path.empty())
            path.clear();

        if (e.button.button == SDL_BUTTON_LEFT) {
            user.leftClick = true;
        } else if (e.button.button == SDL_BUTTON_RIGHT) {
            user.rightClick = true;
        }

        if (configMenu && SDL_PointInRect(user.pos, &map.dijkstraCheckbox)) {
            selectingDijkstra = true;
        } else if (configMenu && SDL_PointInRect(user.pos, &map.aStarCheckbox)) {
            selectingAStar = true;
        } else if (configMenu && SDL_PointInRect(user.pos, &map.diagCheckbox)) {
            selectingDiag = true;
        } else if (configMenu && SDL_PointInRect(user.pos, &map.resetButton)) {
            selectingReset = true;
        } else if (configMenu && SDL_PointInRect(user.pos, &map.runButton)) {
            selectingRun = true;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            user.leftClick = false;
        } else if (e.button.button == SDL_BUTTON_RIGHT) {
            user.rightClick = false;
        }

        if (
            configMenu
            &&
            SDL_PointInRect(user.pos, &map.dijkstraCheckbox)
            &&
            selectingDijkstra
        ) {
            map.dijkstra = true;
            map.aStar = !map.dijkstra;
        } else if (
            configMenu
            &&
            SDL_PointInRect(user.pos, &map.aStarCheckbox)
            &&
            selectingAStar
        ) {
            map.aStar = true;
            map.dijkstra = !map.aStar;
        } else if (
            configMenu
            &&
            SDL_PointInRect(user.pos, &map.diagCheckbox)
            &&
            selectingDiag
        ) {
            map.diagonalTraversal = !map.diagonalTraversal;
        } else if (
            configMenu
            &&
            SDL_PointInRect(user.pos, &map.resetButton)
            &&
            selectingReset
        ) {
            configMenu = false;
            map.reset();
        } else if (
            configMenu
            &&
            SDL_PointInRect(user.pos, &map.runButton)
            &&
            selectingRun
        ) {
            configMenu = false;
            if (map.dijkstra) {
                if (pathfinderThread.joinable())
                    killThread();
                pathfinderThread = std::thread(&Pathfinder::dijkstra, this);
            } else if (map.aStar) {
                if (pathfinderThread.joinable())
                    killThread();
                pathfinderThread = std::thread(&Pathfinder::aStar, this);
            }
        }

        selectingDijkstra = false;
        selectingAStar = false;
        selectingDiag = false;
        selectingReset = false;
        selectingRun = false;
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
        if (user.hoveredNode == map.startNode) {
            map.startNode = nullptr;
        } else if (user.hoveredNode == map.endNode) {
            map.endNode = nullptr;
        }
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
    std::lock_guard<std::mutex> lock(pathMutex); // Ensure thread safety
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
    std::lock_guard<std::mutex> lock(pathMutex); // Ensure thread safety
    Node *lowest = nullptr;
    for (Node *node : unvisited) {
        if (lowest == nullptr || node->g < lowest->g)
            lowest = node;
    }
    return lowest;
}

void Pathfinder::writeNeighborDistances(Node *current)
{
    std::lock_guard<std::mutex> lock(pathMutex); // Ensure thread safety
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (!map.diagonalTraversal && i != 0 && j != 0)
                continue;

            if (i == 0 && j == 0)
                continue;

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
    std::lock_guard<std::mutex> lock(pathMutex); // Ensure thread safety
    Node *current = map.endNode;
    while (current != nullptr) {
        path.insert(current);
        if (current == map.startNode)
            break;
        Node *lowest = nullptr;
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (!map.diagonalTraversal && i != 0 && j != 0)
                    continue;

                if (i == 0 && j == 0)
                    continue;

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

    {
        std::lock_guard<std::mutex> lock(pathMutex); // Ensure thread safety
        path.clear();
    }

    resetUnvisitedNodes();

    pathfinding = true;

    while (!unvisited.empty()) {
        Node *current = getLowestDistanceNode();
        if (current == nullptr || current->g == INT_MAX) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "No path found\n");
            break;
        }

        {
            std::lock_guard<std::mutex> lock(pathMutex); // Ensure thread safety
            unvisited.erase(current);
        }

        if (current == map.endNode)
            break;

        writeNeighborDistances(current);

        // Notify the main loop to proceed to the next step
        {
            std::unique_lock<std::mutex> lock(pathMutex);
            stepCompleted = true;
        }
        cv.notify_one();

        // Wait for the main loop to allow the next step
        std::unique_lock<std::mutex> lock(pathMutex);
        cv.wait_for(lock, std::chrono::milliseconds(1), [this] { return !stepCompleted; });

        if (terminateThread) {
            pathfinding = false;
            return;
        }
    }

    if (map.endNode->g < INT_MAX)
        writePath();

    pathfinding = false;
}

void Pathfinder::aStar()
{
    if (map.startNode == nullptr || map.endNode == nullptr) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "Start and end nodes must be set before running the A* algorithm\n"
        );
        return;
    }

    {
        std::lock_guard<std::mutex> lock(pathMutex); // Ensure thread safety
        path.clear();
    }

    resetUnvisitedNodes();

    pathfinding = true;

    while (!unvisited.empty()) {
        Node *current = getLowestDistanceNode();
        if (current == nullptr || current->g == INT_MAX) {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "No path found\n");
            break;
        }

        {
            std::lock_guard<std::mutex> lock(pathMutex); // Ensure thread safety
            unvisited.erase(current);
        }

        if (current == map.endNode)
            break;

        writeNeighborDistances(current);

        // Notify the main loop to proceed to the next step
        {
            std::unique_lock<std::mutex> lock(pathMutex);
            stepCompleted = true;
        }
        cv.notify_one();

        // Wait for the main loop to allow the next step
        std::unique_lock<std::mutex> lock(pathMutex);
        cv.wait_for(lock, std::chrono::milliseconds(1), [this] { return !stepCompleted; });

        if (terminateThread) {
            pathfinding = false;
            return;
        }
    }

    if (map.endNode->g < INT_MAX)
        writePath();

    pathfinding = false;
}

Pathfinder::~Pathfinder() {
    if (pathfinderThread.joinable())
        pathfinderThread.join();

    delete user.pos;

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
}
