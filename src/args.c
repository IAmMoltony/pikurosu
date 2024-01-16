#include "args.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int _screenWidth = 800;
static int _screenHeight = 600;
static char *_levelName = NULL;

ArgParseResult argsParse(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];
        if (strcmp(arg, "--help") == 0) {
            // help command
            printf("Pikurosu - a Picross/Nonogram implementation\nUsage: %s [options]\n", argv[0]);
            printf("\nValid options are:\n");
            printf(" --scrWidth [screen width] - set window width\n");
            printf(" --scrHeight [screen height] - set window height\n");
            printf(" --level [level] - what level to load\n");
            return ArgParseResult_HelpCommand;
        } else if (strcmp(arg, "--scrWidth") == 0) {
            // screen width
            // TODO add checking args
            _screenWidth = atoi(argv[i + 1]);
        } else if (strcmp(arg, "--scrHeight") == 0) {
            // screen height
            _screenHeight = atoi(argv[i + 1]);
        } else if (strcmp(arg, "--level") == 0) {
            // level name
            _levelName = (char *)malloc((strlen(argv[i + 1]) + 1) * sizeof(char));
            if (!_levelName) {
                fprintf(stderr, "Failed to allocate buffer for level name\n");
                return ArgParseResult_AllocationError;
            }
            strcpy(_levelName, argv[i + 1]);
        }
    }

    // validate args
    if (_levelName == NULL) {
        fprintf(stderr, "Please specify a level name\nRun '%s --help' for more information\n", argv[0]);
        return ArgParseResult_InvalidArgument;
    }

    return ArgParseResult_OK;
}

int argsGetScreenWidth(void)
{
    return _screenWidth;
}

int argsGetScreenHeight(void)
{
    return _screenHeight;
}

char *argsGetLevelName(void)
{
    return _levelName;
}

void argsCleanup(void)
{
    free(_levelName);
}
