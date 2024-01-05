#ifndef HINTS_H_
#define HINTS_H_

#include <math.h>
#include <stdbool.h>

#define MAX_HINTS(boardSize) (int)(ceil((boardSize) / 2.0) - 1)

typedef struct s_hints {
    int **rows;
    int **cols;
    int boardSize;
} BoardHints;

bool hintsCreate(BoardHints *hints, int boardSize);
void hintsDestroy(BoardHints *hints);

#endif
