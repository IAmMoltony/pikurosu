#include "util.h"
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#ifdef WIN32
#include <windows.h>
#endif

void sleepMs(int ms)
{
#ifdef WIN32
    Sleep(ms);
#else
    // Code stolen from https://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds
    struct timespec ts;
    int res;

    if (ms < 0)
        return;

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);
#endif
}

bool isNumberStr(const char *str)
{
    for (int i = 0; str[i]; i++)
        if (!isdigit(str[i]))
            return false;
    return true;
}
