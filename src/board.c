#include "board.h"
#include "mtnlog.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

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

void boardLoad(Board *board, BoardMetadata *boardMeta, const char *name)
{
    mtnlogMessageTag(LOG_INFO, "board", "Loading board from '%s'", name);
    boardLoadMeta(boardMeta, &board->size, name);
    boardLoadSolution(board, name);
    boardCreate(board, board->size);
}

void boardLoadMeta(BoardMetadata *meta, int *size, const char *name) {
    mtnlogMessageTag(LOG_INFO, "board", "Loading board metadata from '%s'", name);

    char *line = NULL;
    FILE *fp;
    size_t len = 0;
    ssize_t read;

    fp = fopen(name, "r");
    if (!fp) {
        mtnlogMessageTag(LOG_ERROR, "board", "Failed to open board file '%s': %s", name, strerror(errno));
        return;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        if (strncmp(line, "nm ", 3) == 0) {
            // name
            line[read - 1] = '\0'; // remove newline
            mtnlogMessageTag(LOG_INFO, "board", "Found name: '%s'", line + 3);
            meta->name = strdup(line + 3);
            if (!meta->name) {
                mtnlogMessageTag(LOG_ERROR, "board", "Failed to allocate buffer for board name");
                return;
            }
            continue;
        }

        if (strncmp(line, "au ", 3) == 0) {
            // author
            line[read - 1] = '\0'; // remove newline
            mtnlogMessageTag(LOG_INFO, "board", "Found author: '%s'", line + 3);
            meta->author = strdup(line + 3);
            if (!meta->author) {
                mtnlogMessageTag(LOG_ERROR, "board", "Failed to allocate buffer for board author");
                return;
            }
            continue;
        }

        if (strncmp(line, "sz ", 3) == 0) {
            // board size
            line[read - 1] = '\0'; // remove newline
            mtnlogMessageTag(LOG_INFO, "board", "Found size: '%s'", line + 3);
            *size = atoi(line + 3);
            continue;
        }

        if (strcmp(line, "s\n") == 0) {
            mtnlogMessageTag(LOG_INFO, "board", "Solution section found, stopping finding meta");
            break;
        }

        mtnlogMessageTag(LOG_WARNING, "board", "Invalid metadata in '%s': %s", name, line);
    }

    free(line);
    fclose(fp);
}

void boardLoadSolution(Board *board, const char *name)
{
    mtnlogMessageTag(LOG_INFO, "board", "Loading solution from file '%s'", name);
    board->solved = (CellState *)malloc(sizeof(CellState) * board->size * board->size);
    if (!board->solved) {
        mtnlogMessageTag(LOG_ERROR, "board", "Failed to allocate solution buffer");
        return;
    }

    FILE *fp = fopen(name, "r");
    if (!fp) {
        mtnlogMessageTag(LOG_ERROR, "board", "Failed to open file '%s': %s", name, strerror(errno));
        return;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    bool doRead = false;
    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (strcmp(line, "s\n") == 0) {
            mtnlogMessageTag(LOG_INFO, "board", "Found solution section. Starting getting solution.");
            doRead = true;
            continue;
        }

        if (doRead) {
            for (int j = 0; j < board->size; j++) {
                char ch = line[j];
                CellState st;
                switch (ch) {
                case '#':
                    st = CellState_Filled;
                    break;
                case '_':
                    st = CellState_Empty;
                    break;
                default:
                    mtnlogMessageTag(LOG_WARNING, "board", "Found unknown cell char '%c' in file '%s', assuming empty", ch, name);
                    st = CellState_Empty;
                    break;
                }

                board->solved[j + i * board->size] = st;
            }
            i++;
        }
    }
    fclose(fp);
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
    free(board->solved);
}

void boardMetaDestroy(BoardMetadata *board)
{
    free(board->name);
    free(board->author);
}

static bool _compareStatesForSolve(CellState s1, CellState s2)
{
    if (s1 == s2)
        return true;
    if (s1 == CellState_Empty && s2 == CellState_Cross)
        return true;
    if (s1 == CellState_Cross && s2 == CellState_Empty)
        return true;
    return false;
}

bool boardIsSolved(Board *board)
{
    bool diff = false;
    for (int i = 0; i < board->size; i++) {
        for (int j = 0; j < board->size; j++) {
            CellState a = board->cells[i + j * board->size];
            CellState b = board->solved[i + j * board->size];
            if (!_compareStatesForSolve(a, b)) {
                diff = true;
                break;
            }
        }
        if (diff)
            break;
    }

    return !diff;
}
