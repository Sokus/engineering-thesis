#ifndef PI_TIME_H
#define PI_TIME_H

#include <stdint.h>

#if defined(_WIN32)
    #include <windows.h>
#endif

namespace Time {

struct State
{
    bool initialized;
    bool sleep_granular;
    #if defined(_WIN32)
        LARGE_INTEGER freq;
        LARGE_INTEGER start;
    #else
        uint64_t start;
    #endif
};

void Setup();
uint64_t Now();
uint64_t Diff(uint64_t new_ticks, uint64_t old_ticks);
uint64_t Since(uint64_t start_ticks);
uint64_t Laptime(uint64_t* last_time);
double Sec(uint64_t ticks);
double Ms(uint64_t ticks);
double Us(uint64_t ticks);
double Ns(uint64_t ticks);
void SleepMs(unsigned long ms);

} // namespace Time

#endif // PI_TIME_H