#include <time.h>
#include "md_utils.h"


int str_to_ptime(sim_time_t* ptm, const char* buf, int len)
{
    // 14:50:50.500
    ptm->tm_hour = atoi(buf);
    ptm->tm_min = atoi(buf + 3);
    ptm->tm_sec = atoi(buf + 6);
    // ptm->ms = atoi(buf + 9);
    return 0;
}

int int_to_ptime(sim_time_t* ptm, int time, int have_millisec)
{
    int hhmmss = time;
    if (have_millisec)
        hhmmss /= 1000;

    ptm->tm_hour = time / 10000;
    ptm->tm_min = (time / 100) % 100;
    ptm->tm_sec = time % 100;
    return 0;
}
