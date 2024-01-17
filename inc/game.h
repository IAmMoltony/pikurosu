#ifndef GAME_H_
#define GAME_H_

typedef enum e_gamestate {
    GameState_Game,
    GameState_LevelSelect
} GameState;

void gameRun(int argc, char **argv);

#endif
