#include "pi_time.h"
#include "macros.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // #ifndef WIN32_LEAN_AND_MEAN
#else // #ifdef _WIN32
#include <time.h>
#include <unistd.h>
#endif // #ifdef _WIN32

struct TimeState
{
    bool initialized;
    bool sleep_granular;
#ifdef _WIN32
    LARGE_INTEGER freq;
    LARGE_INTEGER start;
#else
    uint64_t start;
#endif
} time_state = {0};

void InitializeTime()
{
    time_state.initialized = true;
#ifdef _WIN32
    QueryPerformanceCounter(&time_state.start);
    QueryPerformanceFrequency(&time_state.freq);
    UINT desired_scheduler_ms = 1;
    MMRESULT bprc = timeBeginPeriod(desired_scheduler_ms);
    time_state.sleep_granular = (bprc == TIMERR_NOERROR);
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    time_state.start = (uint64_t)ts.tv_sec*1000000000 + (uint64_t)ts.tv_nsec;
    time_state.sleep_granular = true;
#endif
}

#if defined(_WIN32)
static int64_t int64_muldiv(int64_t value, int64_t numer, int64_t denom)
{
    int64_t q = value / denom;
    int64_t r = value % denom;
    return q * numer + r * numer / denom;
}
#endif

uint64_t Time_Now()
{
    ASSERT(time_state.initialized);
    uint64_t now;
#ifdef _WIN32
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    now = (uint64_t)int64_muldiv(counter.QuadPart - time_state.start.QuadPart, 1000000000, time_state.freq.QuadPart);
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    now = ((uint64_t)ts.tv_sec * 1000000000 + (uint64_t)ts.tv_nsec) - time_state.start;
#endif
    return now;
}

/*
uint64_t Time_Diff(uint64_t new_ticks, uint64_t old_ticks)
{
    return (new_ticks > old_ticks ? new_ticks - old_ticks : 1);
}
*/
uint64_t Time_Diff(uint64_t new_ticks, uint64_t old_ticks)
{
    uint64_t result = 0;
    if (new_ticks >= old_ticks)
        result = new_ticks - old_ticks;
    else
        result = UINT64_MAX - old_ticks + new_ticks;
    return result;
}

uint64_t Time_Since(uint64_t start_ticks)
{
    return Time_Diff(Time_Now(), start_ticks);
}

uint64_t Time_Laptime(uint64_t *last_time)
{
    ASSERT(last_time);
    uint64_t dt = 0;
    uint64_t now = Time_Now();
    if(*last_time != 0)
        dt = Time_Diff(now, *last_time);
    *last_time = now;
    return dt;
}

double Time_Sec(uint64_t ticks)
{
    return (double)ticks / 1000000000.0;
}

uint64_t Time_SecToTicks(double sec)
{
    return (uint64_t)sec * 1000000000;
}

double Time_Ms(uint64_t ticks)
{
    return (double)ticks / 1000000.0;
}

double Time_Us(uint64_t ticks)
{
    return (double)ticks / 1000.0;
}

double Time_Ns(uint64_t ticks)
{
    return (double)ticks;
}

void Time_Sleep(unsigned long ms)
{
    #if defined(_WIN32)
        Sleep(ms);
    #else
        struct timespec rem;
        struct timespec req;
        req.tv_sec = (int)(ms / 1000);
        req.tv_nsec = (ms % 1000) * 1000000;
        nanosleep(&req , &rem);
    #endif
}