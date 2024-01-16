#include "util.h"
#include <unistd.h>
#include <errno.h>
#include <time.h>

void sleepMs(int ms)
{
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
}
