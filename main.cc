#include <cstdlib>
#include <cstring>
#include <iostream>
#include <SDL2/SDL.h>

#include "game.hh"

const uint32_t fps = 60;
const uint32_t ms_per_frame = 1000 / fps;

void usage(void)
{
    std::cerr << "usage: breakout <print_fps=bool>\n";
    exit(1);
}

int main(int argc, char** argv)
{
    if (argc != 2)
        usage();
    bool print_fps = strcmp(argv[1], "-print_fps=true") == 0 ? true : false;

    SDL_SetEventFilter(
            [](void*, SDL_Event* event) -> int
            {
                switch (event->type) {
                case SDL_QUIT:
                    return 1;
                case SDL_KEYDOWN:
                    return 1;
                default:
                    return 0;
                }
            }, nullptr);

    Game     game(print_fps);
    bool     quit        = false;
    uint32_t current_fps = fps;
    while (!quit) {
        uint32_t start_time = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                {
                    int pressed_key = event.key.keysym.scancode;
                    switch (pressed_key) {
                    case SDL_SCANCODE_LEFT:
                        game.update_x(Game::LEFT);
                        break;
                    case SDL_SCANCODE_RIGHT:
                        game.update_x(Game::RIGHT);
                        break;
                    case SDL_SCANCODE_Q:
                        quit = true;
                        break;
                    case SDL_SCANCODE_P:
                        game.toggle_pause();
                        break;
                    default:
                        // do nothing
                        break;
                    }
                }
                break;
            default:
                // do nothing
                break;
            }
        }
        game.update();
        game.update_current_fps(current_fps);
        game.render();

        uint32_t end_time = SDL_GetTicks();
        uint32_t delta = end_time - start_time;
        current_fps = 1000 / (delta > 0 ? delta : 1);
        if (delta < ms_per_frame)
            SDL_Delay(ms_per_frame - delta);
    }

    return 0;
}
