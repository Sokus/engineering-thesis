#ifndef PI_TIME_H
#define PI_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void InitializeTime();
uint64_t Time_Now();
uint64_t Time_Diff(uint64_t new_ticks, uint64_t old_ticks);
uint64_t Time_Since(uint64_t start_ticks);
uint64_t Time_Laptime(uint64_t *last_time);
double Time_Sec(uint64_t ticks);
uint64_t Time_SecToTicks(double sec);
double Time_Ms(uint64_t ticks);
double Time_Us(uint64_t ticks);
double Time_Ns(uint64_t ticks);
void Time_Sleep(unsigned long ms);

#ifdef __cplusplus
}
#endif

#endif // PI_TIME_H