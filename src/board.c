#include "board.h"
#include "mtnlog.h"
#include <stdlib.h>

void boardCreate(Board *board, int size)
{
    board->size = size;
    board->cells = (CellState *)malloc(sizeof(CellState) * size * size);
    if (!board->cells) {
        mtnlogMessageTag(LOG_ERROR, "board", "Failed to allocate memory for board cells");
        return;
    }
    
    for (int i = 0; i < size * size; i++) {
        board->cells[i] = CellState_Empty;
    }
    mtnlogMessageTag(LOG_INFO, "board", "Created board with size of %d", size);
}

CellState boardGetCell(Board *board, int x, int y)
{
    return board->cells[x + y * board->size];
}

void boardSetCell(Board *board, int x, int y, CellState state)
{
    board->cells[x + y * board->size] = state;
}

void boardDestroy(Board *board)
{
    free(board->cells);
}
