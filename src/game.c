#include "game.h"
#include "mtnlog.h"
#include "board.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#define CELL_SIZE 32
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

static SDL_Window *_window = NULL;
static SDL_Renderer *_rend = NULL;
static bool _running = true;
static int _mouseX = 0;
static int _mouseY = 0;
static GameState _gState = GameState_Game;

static Board _board;
static BoardMetadata _boardMeta;
static bool _boardSolved = false;
static int _boardX = 100;
static int _boardY = 30;
static int _time = 0;
static bool _incTime = true;
static pthread_t _incTimeThread;
static bool _incTaskRunning = true;

static void *_timeIncrementTask(void *arg)
{
    (void)arg;
    while (true) {
        if (_incTime)
            _time++;
        usleep(1000);
        if (!_incTaskRunning)
            break;
    }
    return NULL;
}

static void _loadBoard(const char *name)
{
    boardLoad(&_board, &_boardMeta, name);
    _boardX = SCREEN_WIDTH / 2 - (_board.size * CELL_SIZE / 2);
    _boardY = SCREEN_HEIGHT / 2 - (_board.size * CELL_SIZE / 2);
}

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
    _window = SDL_CreateWindow("Pikurosu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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
    _loadBoard("levels/test.pikurosu");

    // start time increment task
    int incTaskCode = pthread_create(&_incTimeThread, NULL, _timeIncrementTask, NULL);
    if (incTaskCode != 0) {
        mtnlogMessageTag(LOG_ERROR, "init", "Failed to create time increment thread (error %d)", incTaskCode);
        return;
    }
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

            switch (_gState) {
            case GameState_Game:
                if (ev.button.button == SDL_BUTTON_LEFT || ev.button.button == SDL_BUTTON_RIGHT) {
                    for (int i = 0; i < _board.size; i++) {
                        for (int j = 0; j < _board.size; j++) {
                            int cellX = i * CELL_SIZE + _boardX;
                            int cellY = j * CELL_SIZE + _boardY;
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
                                    _boardSolved = true;
                                    _incTime = false;
                                    mtnlogMessageTag(LOG_INFO, "event", "Solve time: %d ms (%.2f s)", _time, (float)_time / 1000);
                                }
                            }
                        }
                    }
                }
                break;
            }
            break;
        }
    }
}

static void _renderBoard(void)
{
    for (int i = 0; i < _board.size; i++) {
        for (int j = 0; j < _board.size; j++) {
            int cellX = i * CELL_SIZE + _boardX;
            int cellY = j * CELL_SIZE + _boardY;
            SDL_Rect cellRect = {cellX, cellY, CELL_SIZE, CELL_SIZE};
            SDL_Rect cellFilledRect = {cellX + 4, cellY + 4, CELL_SIZE - 8, CELL_SIZE - 8};

            bool hovering = (_mouseX > cellX && _mouseY > cellY && _mouseX < cellX + CELL_SIZE && _mouseY < cellY + CELL_SIZE);
            bool hoveringOnlyX = (_mouseX > cellX && _mouseX < cellX + CELL_SIZE);
            bool hoveringOnlyY = (_mouseY > cellY && _mouseY < cellY + CELL_SIZE);
            bool mouseInBoard = (_mouseX > _boardX && _mouseY > _boardY && _mouseX < _boardX + _board.size * CELL_SIZE && _mouseY < _boardY + _board.size * CELL_SIZE);

            if (hovering)
                SDL_SetRenderDrawColor(_rend, 230, 230, 230, 255);
            else if ((hoveringOnlyX || hoveringOnlyY) && mouseInBoard)
                SDL_SetRenderDrawColor(_rend, 160, 160, 160, 255);
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

    switch (_gState) {
    case GameState_Game:
        _renderBoard();
        break;
    }

    // put stuff to screen
    SDL_RenderPresent(_rend);
}

static void _cleanup(void)
{
    mtnlogMessageTag(LOG_INFO, "cleanup", "Cleanup: board");
    boardDestroy(&_board);
    boardMetaDestroy(&_boardMeta);

    mtnlogMessageTag(LOG_INFO, "cleanup", "Cleanup: stopping threads");
    _incTaskRunning = false;
    pthread_join(_incTimeThread, NULL);

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
