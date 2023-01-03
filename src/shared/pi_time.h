#ifndef PI_TIME_H
#define PI_TIME_H

#include <stdint.h>

namespace Time {

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