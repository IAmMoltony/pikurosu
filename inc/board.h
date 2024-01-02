#ifndef BOARD_H_
#define BOARD_H_

#include <stdbool.h>

typedef enum e_cell_state {
    CellState_Empty,
    CellState_Filled,
    CellState_Cross,
} CellState;

typedef struct s_board {
    CellState *cells;
    CellState *solved;
    int size;
} Board;

typedef struct s_board_meta {
    char *name;
    char *author;
} BoardMetadata;

void boardCreate(Board *board, int size);
void boardLoad(Board *board, BoardMetadata *boardMeta, const char *name);
void boardLoadMeta(BoardMetadata *meta, int *size, const char *name);
void boardLoadSolution(Board *board, const char *name);

CellState boardGetCell(Board *board, int x, int y);
void boardSetCell(Board *board, int x, int y, CellState state);

void boardDestroy(Board *board);
void boardMetaDestroy(BoardMetadata *board);

bool boardIsSolved(Board *board);

#endif
