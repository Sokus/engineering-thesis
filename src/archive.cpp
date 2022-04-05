// THIS CODE IS KEPT ONLY FOR FUTURE REFERENCE

#if 0

struct WorkQueueEntry
{
    char *string_to_print;
};

global uint32_t volatile global_entry_completion_count;
global uint32_t volatile global_next_entry_to_do;
global uint32_t volatile global_entry_count;
WorkQueueEntry global_entries[256];

#define MEMORY_BARRIER STATEMENT(__atomic_signal_fence(__ATOMIC_SEQ_CST);)

internal void PushString(sem_t *semaphore_handle, char *str)
{
    ASSERT(global_entry_count < ARRAY_COUNT(global_entries));
    
    WorkQueueEntry *entry = global_entries + global_entry_count;
    entry->string_to_print = str;
    
    MEMORY_BARRIER;
    
    ++global_entry_count;
    
    sem_post(semaphore_handle);
}

struct ThreadInfo
{
    sem_t* semaphore_handle;
    int logical_index;
};

void *thread_start(void *arg)
{
    ThreadInfo *thread_info = (ThreadInfo *)arg;
    
    for(;;)
    {
        if(global_next_entry_to_do < global_entry_count)
        {
            int entry_index = __atomic_fetch_add(&global_next_entry_to_do, 1, __ATOMIC_SEQ_CST);
            //MEMORY_BARRIER;
            WorkQueueEntry *entry = global_entries + entry_index;
            
            char buffer[256];
            sprintf(buffer, "Thread %i: %s\n", thread_info->logical_index, entry->string_to_print);
            printf("%s", buffer);
            
            __atomic_add_fetch(&global_entry_completion_count, 1, __ATOMIC_SEQ_CST);
        }
        else
        {
            sem_wait(thread_info->semaphore_handle);
        }
    }
    
    exit(0);
}

int main(void)
{
    ThreadInfo thread_info[8];
    
    uint32_t initial_count = 0;
    sem_t semaphore_handle;
    sem_init(&semaphore_handle, 0, initial_count);
    
    for(uint32_t thread_index = 0;
        thread_index < ARRAY_COUNT(thread_info);
        thread_index++)
    {
        ThreadInfo *info = thread_info + thread_index;
        info->semaphore_handle = &semaphore_handle;
        info->logical_index = thread_index;
        
        pthread_t thread_handle;
        pthread_create(&thread_handle, 0, thread_start, info);
        pthread_detach(thread_handle);
    }
    
    PushString(&semaphore_handle, "String A0");
    PushString(&semaphore_handle, "String A1");
    PushString(&semaphore_handle, "String A2");
    PushString(&semaphore_handle, "String A3");
    PushString(&semaphore_handle, "String A4");
    PushString(&semaphore_handle, "String A5");
    PushString(&semaphore_handle, "String A6");
    PushString(&semaphore_handle, "String A7");
    PushString(&semaphore_handle, "String A8");
    PushString(&semaphore_handle, "String A9");
    
    sleep(2);
    
    PushString(&semaphore_handle, "String B0");
    PushString(&semaphore_handle, "String B1");
    PushString(&semaphore_handle, "String B2");
    PushString(&semaphore_handle, "String B3");
    PushString(&semaphore_handle, "String B4");
    PushString(&semaphore_handle, "String B5");
    PushString(&semaphore_handle, "String B6");
    PushString(&semaphore_handle, "String B7");
    PushString(&semaphore_handle, "String B8");
    PushString(&semaphore_handle, "String B9");
    
    sleep(2);
    
    return 0;
}
#endif