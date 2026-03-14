#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>
#include "third_party/x86.h"
#include "paging.h"

#define MAX_HANDLES 128 
#define MAX_PROC 128 

typedef struct {
    registers_t regs; 
} thread_t;

typedef struct {
    bool exists;
} handle_t;

typedef struct {
    uint64_t kernel_stack;
    uint64_t saved_stack;
    pagemap_t pagemap;
    handle_t handles[MAX_HANDLES]; 
    thread_t main_thread;
} process_t;

extern process_t process_stack[MAX_PROC];
extern uint32_t  process_count;
extern uint32_t  current_proc;

uint32_t create_process();
uint32_t switch_to_proc(uint32_t proc_id);

#endif //PROCESS_H
