#include "game.h"
#include "mtnlog.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

static SDL_Window *_window = NULL;
static SDL_Renderer *_rend = NULL;
static bool _running = true;

static void _init(int argc, char **argv)
{
    mtnlogInit(LOG_INFO, "pikurosu.log");

    // print args
    mtnlogMessageTag(LOG_INFO, "init", "%d arguments", argc);
    for (int i = 0; i < argc; i++)
        mtnlogMessageTag(LOG_INFO, "init", "Argument #%d: %s", i + 1, argv[i]);

    // init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        mtnlogMessageTag(LOG_ERROR, "init", "Failed to init SDL2: %s", SDL_GetError());
        return;
    }
    mtnlogMessageTag(LOG_INFO, "init", "SDL initialized");

    // init window
    _window = SDL_CreateWindow("Pikurosu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!_window) {
        mtnlogMessageTag(LOG_ERROR, "init", "Failed to create window: %s", SDL_GetError());
        return;
    }

    // init renderer
    _rend = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!_rend) {
        mtnlogMessageTag(LOG_ERROR, "init", "Failed to create renderer: %s", SDL_GetError());
        return;
    }
}

static void _update()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_KEYDOWN:
            if (ev.key.keysym.sym == SDLK_ESCAPE) {
                mtnlogMessageTag(LOG_INFO, "event", "Pressed escape, exiting");
                _running = false;
            }
            break;
        case SDL_QUIT:
            mtnlogMessageTag(LOG_INFO, "event", "Quit event, exiting");
            _running = false;
            break;
        }
    }
}

static void _render()
{
    SDL_SetRenderDrawColor(_rend, 0, 0, 0, 255);
    SDL_RenderClear(_rend);
    SDL_RenderPresent(_rend);
}

static void _cleanup()
{
    SDL_DestroyRenderer(_rend);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void gameRun(int argc, char **argv)
{
    _init(argc, argv);
    while (_running) {
        _update();
        _render();
    }
    _cleanup();
}
