#include "args.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int _screenWidth = 800;
static int _screenHeight = 600;
static bool _fullscreen = false;

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
            printf(" --fullscreen - enable fullscreen");
            return ArgParseResult_HelpCommand;
        } else if (strcmp(arg, "--scrWidth") == 0) {
            // screen width
            char *sws = argv[i + 1];
            if (!isNumberStr(sws)) {
                printf("Invalid screen width\n");
                return ArgParseResult_InvalidArgument;
            }
            _screenWidth = atoi(sws);
        } else if (strcmp(arg, "--scrHeight") == 0) {
            // screen height
            char *shs = argv[i + 1];
            if (!isNumberStr(shs)) {
                printf("Invalid screen height\n");
                return ArgParseResult_InvalidArgument;
            }
            _screenHeight = atoi(shs);
        } else if (strcmp(arg, "--fullscreen") == 0) {
            _fullscreen = true;
        }
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

bool argsGetFullscreen(void)
{
    return _fullscreen;
}

void argsCleanup(void)
{
    // (stub)
}
