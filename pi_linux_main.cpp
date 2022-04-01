#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "pi_helpers.h"

#define MEMORY_BARRIER STATEMENT(__atomic_signal_fence(__ATOMIC_SEQ_CST);)

struct ThreadArg
{
    int thread_index;
};

void *thread_start(void *arg)
{
    ThreadArg *thread_arg = (ThreadArg *)arg;
    printf("Thread %d started.\n", thread_arg->thread_index);
    
    return 0;
}

int main(void)
{
    ThreadArg args[5] = {0};
    pthread_t handles[5] = {0};
    
    
    for(int i = 0; i < 5; i++)
    {
        ThreadArg *thread_arg = args + i;
        thread_arg->thread_index = i;
        pthread_create(handles + i, 0, thread_start, thread_arg);
        pthread_detach(handles[i]);
    }
    
    //sleep(1);
    
    return 0;
}