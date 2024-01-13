#include "hints.h"
#include "mtnlog.h"
#include <stdlib.h>

bool hintsCreate(BoardHints *hints, int boardSize)
{
    if (boardSize <= 0) {
        mtnlogMessageTag(MTNLOG_ERROR, "hints", "Invalid board size %d", boardSize);
        return false;
    }

    hints->boardSize = boardSize;

    hints->rows = (int **)malloc(boardSize * sizeof(int *));
    hints->cols = (int **)malloc(boardSize * sizeof(int *));

    if (!hints->rows || !hints->cols) {
        mtnlogMessageTag(MTNLOG_ERROR, "hints", "Failed to create board hints");
        return false;
    }

    for (int i = 0; i < boardSize; ++i) {
        hints->rows[i] = (int *)malloc(MAX_HINTS(boardSize) * sizeof(int));
        hints->cols[i] = (int *)malloc(MAX_HINTS(boardSize) * sizeof(int));
        if (!hints->rows[i] || !hints->cols[i]) {
            mtnlogMessageTag(MTNLOG_ERROR, "hints", "Failed to create board hints");
            return false;
        }
    }

    mtnlogMessageTag(MTNLOG_INFO, "hints", "Created hints for board of size %d", boardSize);
    return true;
}

void hintsDestroy(BoardHints *hints)
{
    if (hints->rows) {
        for (int i = 0; i < hints->boardSize; ++i) {
            free(hints->rows[i]);
        }
        free(hints->rows);
        hints->rows = NULL;
    }

    if (hints->cols) {
        for (int i = 0; i < hints->boardSize; ++i) {
            free(hints->cols[i]);
        }
        free(hints->cols);
        hints->cols = NULL;
    }
}
