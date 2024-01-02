#ifndef BOARD_H_
#define BOARD_H_

typedef enum e_cell_state {
    CellState_Empty,
    CellState_Filled,
    CellState_Cross,
} CellState;

typedef struct s_board {
    CellState *cells;
    int size;
} Board;

void boardCreate(Board *board, int size);
CellState boardGetCell(Board *board, int x, int y);
void boardSetCell(Board *board, int x, int y, CellState state);
void boardDestroy(Board *board);

#endif
