#include "pi_time.h"
#include "macros.h"

#include <stdint.h>
#include <string.h>

#if defined(_WIN32)
    #include <windows.h>
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
#else
    #include <time.h>
    #include <unistd.h>
#endif

namespace Time {

static State state;

void Setup()
{
    memset(&state, 0x00, sizeof(State));
    state.initialized = true;
    #if defined(_WIN32)
        QueryPerformanceCounter(&state.start);
        QueryPerformanceFrequency(&state.freq);
        UINT desired_scheduler_ms = 1;
        MMRESULT bprc = timeBeginPeriod(desired_scheduler_ms);
        state.sleep_granular = (bprc == TIMERR_NOERROR);
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        state.start = (uint64_t)ts.tv_sec*1000000000 + (uint64_t)tv.tv_nsec;
        state.sleep_granular = true;
    #endif
}

#if defined(_WIN32)
    int64_t int64_muldiv(int64_t value, int64_t numer, int64_t denom)
    {
        int64_t q = value / denom;
        int64_t r = value % denom;
        return q * numer + r * numer / denom;
    }
#endif

uint64_t Now()
{
    ASSERT(state.initialized);
    uint64_t now;
    #if defined(_WIN32)
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        now = (uint64_t)int64_muldiv(counter.QuadPart - state.start.QuadPart, 1000000000, state.freq.QuadPart);
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        now = ((uint64_t)ts.tv_sec*1000000000 + (uint64_t)ts.tv_nsec) - _stm.start;
    #endif
    return now;
}

uint64_t Diff(uint64_t new_ticks, uint64_t old_ticks)
{
    return (new_ticks > old_ticks ? new_ticks - old_ticks : 1);
}

uint64_t Since(uint64_t start_ticks)
{
    return Diff(Now(), start_ticks);
}

uint64_t Laptime(uint64_t* last_time)
{
    ASSERT(last_time);
    uint64_t dt = 0;
    uint64_t now = Now();
    if(*last_time != 0)
        dt = Diff(now, *last_time);
    *last_time = now;
    return dt;
}

double Sec(uint64_t ticks)
{
    return (double)ticks / 1000000000.0;
}

double Ms(uint64_t ticks)
{
    return (double)ticks / 1000000.0;
}

double Us(uint64_t ticks)
{
    return (double)ticks / 1000.0;
}

double Ns(uint64_t ticks)
{
    return (double)ticks;
}

void SleepMs(unsigned long ms)
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

} // namespace Time