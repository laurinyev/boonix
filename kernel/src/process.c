#include "process.h"
#include "paging.h"

process_t process_stack[MAX_PROC] = {0};
uint32_t  process_count = 0;

uint32_t create_process() {
    uint32_t new_id = process_count++;

    process_stack[new_id] = (process_t){
        .pagemap = create_pagemap(),
        .handles = {0},
        .main_thread = {0}
    };

    //TODO: add handles

    return new_id;
}
