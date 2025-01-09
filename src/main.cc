#include "../include/pathfinder.hh"

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    Pathfinder pf = Pathfinder();
    if (pf.error) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error initializing pathfinder\n");
        return 1;
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Pathfinder initialized\n");
    }

    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
            }
            if (e.type == SDL_MOUSEMOTION) {
                pf.user.pos->x = e.motion.x;
                pf.user.pos->y = e.motion.y;
            }
        }

        // input handling
        const Uint8 *keys = SDL_GetKeyboardState(nullptr);
        pf.getHoveredNode();

        /*
        if (pf.user.hoveredNode != nullptr)
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Hovered node: (%d, %d)\n", pf.user.hoveredNode->x, pf.user.hoveredNode->y);

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Mouse pos: (%d, %d)\n", pf.user.pos->x, pf.user.pos->y);
        */

        SDL_SetRenderDrawColor(pf.ren, 175, 175, 175, 255);
        SDL_RenderClear(pf.ren);

        pf.map.draw(pf.ren);

        if (pf.user.hoveredNode != nullptr) {
                if (keys[SDL_SCANCODE_LSHIFT])
                    SDL_SetRenderDrawColor(pf.ren, 0, 255, 0, 75);
                else if (keys[SDL_SCANCODE_LCTRL])
                    SDL_SetRenderDrawColor(pf.ren, 255, 0, 0, 75);
                else
                    SDL_SetRenderDrawColor(pf.ren, 0, 0, 0, 75);
            SDL_RenderFillRect(pf.ren, &pf.user.hoveredNode->rect);
        }

        SDL_RenderPresent(pf.ren);
    }

    return 0;
}
