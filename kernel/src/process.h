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
    pagemap_t pagemap;
    handle_t handles[MAX_HANDLES]; 
    thread_t main_thread;
} process_t;

extern process_t process_stack[MAX_PROC];
extern uint32_t  process_count;

uint32_t create_process();

#endif //PROCESS_H
