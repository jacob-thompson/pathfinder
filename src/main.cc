#include "../include/pathfinder.hh"

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SDL Initialized\n");
    }

    Pathfinder pf = Pathfinder();
    if (pf.isInitError()) {
        return 1; // error message is already logged
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Pathfinder initialized\n");
    }

    SDL_Event event;
    while (pf.isRunning()) {
        while (SDL_PollEvent(&event))
            pf.handleEvent(event);

        // input
        const Uint8 *keys = SDL_GetKeyboardState(nullptr);
        pf.setHoveredNode();
        pf.modifyHoveredNode(keys);

        /*
        if (pf.user.hoveredNode != nullptr)
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Hovered node: (%d, %d)\n", pf.user.hoveredNode->x, pf.user.hoveredNode->y);

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Mouse pos: (%d, %d)\n", pf.user.pos->x, pf.user.pos->y);
        */

        // draw
        pf.map.drawGrid(pf.ren);
        pf.map.drawHovered(pf.ren, pf.user.hoveredNode, keys);
        if (!pf.path.empty())
            pf.map.drawPath(pf.ren, pf.path);

        // render
        SDL_RenderPresent(pf.ren);
    }

    return 0;
}
