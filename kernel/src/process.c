#include "process.h"
#include "paging.h"
#include "third_party/x86.h"
#include "printf.h"
#include "pmm.h"

process_t process_stack[MAX_PROC] = {0};
uint32_t  process_count = 0;
uint32_t  current_proc = 0xFFFFFFFF;

#define STACK_SIZE 0x8
#define KERNEL_STACK_SIZE 0x8

uint32_t create_process() {
    uint32_t new_id = process_count++;
    process_t* new_ptr = &process_stack[new_id];

    new_ptr->pagemap = create_pagemap();
    new_ptr->main_thread.regs = (registers_t){0};

    uintptr_t stack_addr = find_avail_blocks(new_ptr->pagemap, STACK_SIZE);
    uintptr_t kernel_stack_addr = find_avail_blocks(new_ptr->pagemap, KERNEL_STACK_SIZE);

    for(uint32_t i = 0; i < STACK_SIZE; i++){
        if(!map(new_ptr->pagemap,stack_addr + i*PAGE_SIZE, (uintptr_t)pmm_alloc(), PAGE_FLAG_P | PAGE_FLAG_W | PAGE_FLAG_U | PAGE_FLAG_NX)) {
            kprintf("Mapping stack failed\n");
            return 0xFFFFFFFF;
        }
    }

    for(uint32_t i = 0; i < KERNEL_STACK_SIZE; i++){
        if(!map(new_ptr->pagemap,kernel_stack_addr + i*PAGE_SIZE, (uintptr_t)pmm_alloc(), PAGE_FLAG_P | PAGE_FLAG_W | PAGE_FLAG_U | PAGE_FLAG_NX)) {
            kprintf("Mapping kernel stack failed\n");
            return 0xFFFFFFFF;
        }
    }

    new_ptr->kernel_stack = kernel_stack_addr + KERNEL_STACK_SIZE*PAGE_SIZE;
    
    new_ptr->main_thread.regs.rsp = (uint64_t)stack_addr + STACK_SIZE*PAGE_SIZE;
    new_ptr->main_thread.regs.rbp = (uint64_t)stack_addr + STACK_SIZE*PAGE_SIZE;
    
    //TODO: add handles

    return new_id;
}

uint32_t switch_to_proc(uint32_t proc_id) {
    if (proc_id >= MAX_PROC){
        return 1; // TODO: error code
    }
    process_t* proc_ptr = &process_stack[proc_id];
    
    if (current_proc != 0xFFFFFFFF){
        process_t* curr = &process_stack[current_proc];
        registers_t* r = &curr->main_thread.regs;

        asm volatile(
            "mov %%rax, %0\n"
            "mov %%rcx, %1\n"
            "mov %%rdx, %2\n"
            "mov %%rbx, %3\n"
            "mov %%rbp, %4\n"
            "mov %%rsi, %5\n"
            "mov %%rdi, %6\n"
            "mov %%r8,  %7\n"
            "mov %%r9,  %8\n"
            "mov %%r10, %9\n"
            "mov %%r11, %10\n"
            "mov %%r12, %11\n"
            "mov %%r13, %12\n"
            "mov %%r14, %13\n"
            "mov %%r15, %14\n"
            "mov %%rsp, %15\n"
            "pushfq\n"
            "pop %16\n"
            : "=m"(r->rax), "=m"(r->rcx), "=m"(r->rdx), "=m"(r->rbx),
              "=m"(r->rbp), "=m"(r->rsi), "=m"(r->rdi),
              "=m"(r->r8), "=m"(r->r9), "=m"(r->r10), "=m"(r->r11),
              "=m"(r->r12), "=m"(r->r13), "=m"(r->r14), "=m"(r->r15),
              "=m"(r->rsp), "=m"(r->rflags)
            :
            : "memory"
        );
        
        curr->main_thread.regs.cs = 0x08; //kernel CS
        curr->main_thread.regs.ss = 0x10; //kernel SS  
        curr->main_thread.regs.rip = (uint64_t)&&return_addr; //GNU EXTENSION: takng the address of a label 
    }

    current_proc = proc_id;
    wrmsr(MSR_IA32_GS_BASE, (uint64_t)proc_ptr);

    registers_t* r = &proc_ptr->main_thread.regs;

    asm volatile(
        "push %0\n"
        "push %1\n"
        "push %2\n"
        "push %3\n"
        "push %4\n"
        "push %5\n"
        "push %6\n"
        "push %7\n"
        "push %8\n"
        "push %9\n"
        "push %10\n"
        "push %11\n"
        "push %12\n"
        "push %13\n"
        "push %14\n"
        
        "pop %%r15\n"
        "pop %%r14\n"
        "pop %%r13\n"
        "pop %%r12\n"
        "pop %%r11\n"
        "pop %%r10\n"
        "pop %%r9 \n"
        "pop %%r8 \n"
        "pop %%rdi\n"
        "pop %%rsi\n"
        "pop %%rbp\n"
        "pop %%rbx\n"
        "pop %%rdx\n"
        "pop %%rcx\n"
        "pop %%rax\n"
        :
        : "r"(r->rax), "r"(r->rcx), "r"(r->rdx), "r"(r->rbx),
          "r"(r->rbp), "r"(r->rsi), "r"(r->rdi),
          "r"(r->r8),  "r"(r->r9),  "r"(r->r10), "r"(r->r11),
          "r"(r->r12), "r"(r->r13), "r"(r->r14), "r"(r->r15)
        : "memory"/*, "rax", "rcx", "rdx",
          "rbx",    "rbp", "rsi", "rdi", 
          "r8" ,    "r9" , "r10", "r11",
          "r12",    "r13", "r14", "r15"*/ // The compiler doesn't have to know...
    );
    
    set_cur_pagemap(proc_ptr->pagemap);

    //NOTE: process may jump to userspace as its first switch 
    asm volatile(
        "mov %0, %%rsp\n"
        "push %1\n"
        "push %2\n"
        "push %3\n"
        "push %4\n"
        "push %5\n"
        "iretq\n"
        :
        : "m"(r->rsp),
          "m"(r->ss),
          "m"(r->rsp),
          "m"(r->rflags),
          "m"(r->cs),
          "m"(r->rip)
        : "memory"
    );

    return_addr: 
    return 0; 
}
