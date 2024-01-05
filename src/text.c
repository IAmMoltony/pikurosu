#include "text.h"
#include "mtnlog.h"
#include <SDL2/SDL_ttf.h>

bool textInit(void)
{
    if (TTF_Init() < 0) {
        mtnlogMessageTag(LOG_ERROR, "text", "Failed to init SDL_ttf: %s", TTF_GetError());
        return false;
    }

    mtnlogMessageTag(LOG_INFO, "text", "Text renderer initialized");

    return true;
}
