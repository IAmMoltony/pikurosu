#include "game.h"
#include "mtnlog.h"
#include "board.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

#define CELL_SIZE 32
#define BOARD_X 100
#define BOARD_Y 30

static SDL_Window *_window = NULL;
static SDL_Renderer *_rend = NULL;
static bool _running = true;
static int _mouseX = 0;
static int _mouseY = 0;

static Board _board;
static BoardMetadata _boardMeta;

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

    // init board
    boardLoad(&_board, &_boardMeta, "levels/test.pikurosu");
}

static void _update(void)
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
        case SDL_MOUSEMOTION:
            _mouseX = ev.motion.x;
            _mouseY = ev.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
             _mouseX = ev.button.x;
            _mouseY = ev.button.y;
            if (ev.button.button == SDL_BUTTON_LEFT || ev.button.button == SDL_BUTTON_RIGHT) {
                for (int i = 0; i < _board.size; i++) {
                    for (int j = 0; j < _board.size; j++) {
                        int cellX = i * CELL_SIZE + BOARD_X;
                        int cellY = j * CELL_SIZE + BOARD_Y;
                        bool hovering = (_mouseX > cellX && _mouseY > cellY && _mouseX < cellX + CELL_SIZE && _mouseY < cellY + CELL_SIZE);
                        if (hovering) {
                            mtnlogMessageTag(LOG_INFO, "event", "Clicked on cell (%d,%d)", i, j);
                            CellState oldState = boardGetCell(&_board, i, j);

                            Uint8 button = ev.button.button;
                            bool didMove = false;
                            switch (button) {
                            case SDL_BUTTON_LEFT:
                                if (oldState == CellState_Filled) {
                                    boardSetCell(&_board, i, j, CellState_Empty);
                                    didMove = true;
                                } else if (oldState == CellState_Empty) {
                                    boardSetCell(&_board, i, j, CellState_Filled);
                                    didMove = true;
                                }
                                break;
                            case SDL_BUTTON_RIGHT:
                                if (oldState == CellState_Cross) {
                                    boardSetCell(&_board, i, j, CellState_Empty);
                                    didMove = true;
                                } else if (oldState == CellState_Empty) {
                                    boardSetCell(&_board, i, j, CellState_Cross);
                                    didMove = true;
                                }
                                break;
                            }

                            if (didMove && boardIsSolved(&_board)) {
                                mtnlogMessageTag(LOG_INFO, "event", "Board is solved");
                            }
                        }
                    }
                }
            }
            break;
        }
    }
}

static void _renderBoard(void)
{
    for (int i = 0; i < _board.size; i++) {
        for (int j = 0; j < _board.size; j++) {
            int cellX = i * CELL_SIZE + BOARD_X;
            int cellY = j * CELL_SIZE + BOARD_Y;
            SDL_Rect cellRect = {cellX, cellY, CELL_SIZE, CELL_SIZE};
            SDL_Rect cellFilledRect = {cellX + 4, cellY + 4, CELL_SIZE - 8, CELL_SIZE - 8};

            bool hovering = (_mouseX > cellX && _mouseY > cellY && _mouseX < cellX + CELL_SIZE && _mouseY < cellY + CELL_SIZE);

            if (hovering)
                SDL_SetRenderDrawColor(_rend, 230, 230, 230, 255);
            else
                SDL_SetRenderDrawColor(_rend, 128, 128, 128, 255);
            SDL_RenderFillRect(_rend, &cellRect);
            if (hovering)
                SDL_SetRenderDrawColor(_rend, 0, 0, 255, 255);
            else
                SDL_SetRenderDrawColor(_rend, 0, 0, 128, 255);
            SDL_RenderDrawRect(_rend, &cellRect);

            CellState state = boardGetCell(&_board, i, j);
            switch (state) {
            default:
                break;
            case CellState_Filled:
                if (hovering)
                    SDL_SetRenderDrawColor(_rend, 120, 120, 120, 255);
                else
                    SDL_SetRenderDrawColor(_rend, 80, 80, 80, 255);
                SDL_RenderFillRect(_rend, &cellFilledRect);
                break;
            case CellState_Cross:
                if (hovering)
                    SDL_SetRenderDrawColor(_rend, 120, 120, 120, 255);
                else
                    SDL_SetRenderDrawColor(_rend, 80, 80, 80, 255);
                for (int k = -1; k <= 2; k++) {
                    int cx = cellX + k;
                    SDL_RenderDrawLine(_rend, cx + 6, cellY + 6, cx + CELL_SIZE - 8, cellY + CELL_SIZE - 8);
                    SDL_RenderDrawLine(_rend, cx + 6, cellY + CELL_SIZE - 8, cx + CELL_SIZE - 8, cellY + 6);
                }
                break;
            }
        }
    }
}

static void _render(void)
{
    // clear screen
    SDL_SetRenderDrawColor(_rend, 0, 0, 0, 255);
    SDL_RenderClear(_rend);

    _renderBoard();

    // put stuff to screen
    SDL_RenderPresent(_rend);
}

static void _cleanup(void)
{
    mtnlogMessageTag(LOG_INFO, "cleanup", "Cleanup: board");
    boardDestroy(&_board);
    boardMetaDestroy(&_boardMeta);

    mtnlogMessageTag(LOG_INFO, "cleanup", "Cleanup: SDL");
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
