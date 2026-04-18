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
    new_ptr->regs = (registers_t){0};

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
    
    new_ptr->regs.rsp = (uint64_t)stack_addr + STACK_SIZE*PAGE_SIZE;
    new_ptr->regs.rbp = (uint64_t)stack_addr + STACK_SIZE*PAGE_SIZE;
    
    //TODO: add handles

    return new_id;
}

void switch_curr_proc(uint32_t proc_id) {
    process_t* proc_ptr = &process_stack[proc_id];
    
    current_proc = proc_id;
    wrmsr(MSR_IA32_GS_BASE, (uint64_t)proc_ptr);
    set_cur_pagemap(proc_ptr->pagemap);
}

__attribute__((naked)) uint32_t switch_to_proc(uint32_t proc_id,bool first_switch) {
    asm volatile(
        "cmpq $0x00, %%rsi\n"
        "jne .post_save\n"
        "pop %%gs:0x98\n" // the return address
        "mov %%r15, %%gs:0x10\n"
        "mov %%r14, %%gs:0x18\n"
        "mov %%r13, %%gs:0x20\n"
        "mov %%r12, %%gs:0x28\n"
        "mov %%r11, %%gs:0x30\n"
        "mov %%r10, %%gs:0x38\n"
        "mov %%r9 , %%gs:0x40\n"
        "mov %%r8 , %%gs:0x48\n"
        "mov %%rdi, %%gs:0x50\n"
        "mov %%rsi, %%gs:0x58\n"
        "mov %%rbp, %%gs:0x60\n"
        "mov %%rbx, %%gs:0x68\n"
        "mov %%rdx, %%gs:0x70\n"
        "mov %%rcx, %%gs:0x78\n"
        "mov %%rax, %%gs:0x80\n"
        "mov %%rsp, %%gs:0xB0\n"
        ".post_save:\n"

        "pushfq\n"
        "pop %%gs:0xA8\n" // GS + 0xA8 = rflags
        // mov %rdi, %rdi <-- unneeded lmao
        "call switch_curr_proc\n"

        "mov %%gs:0x10, %%r15\n"
        "mov %%gs:0x18, %%r14\n"
        "mov %%gs:0x20, %%r13\n"
        "mov %%gs:0x28, %%r12\n"
        "mov %%gs:0x30, %%r11\n"
        "mov %%gs:0x38, %%r10\n"
        "mov %%gs:0x40, %%r9 \n"
        "mov %%gs:0x48, %%r8 \n"
        "mov %%gs:0x50, %%rdi\n"
        "mov %%gs:0x58, %%rsi\n"
        "mov %%gs:0x60, %%rbp\n"
        "mov %%gs:0x68, %%rbx\n"
        "mov %%gs:0x70, %%rdx\n"
        "mov %%gs:0x78, %%rcx\n"
        "mov %%gs:0x80, %%rax\n"
        "mov %%gs:0xB0, %%rsp\n"
        
        // a first switch is a special case where iretq needs extra arguments
        "cmpq $0x18, %%gs:0xA0\n" // 0x18 = user code segment, GS + 0xA0 = code segment
        "jne .not_first_sw\n"
        
        "push %%gs:0xB8\n" // GS + 0xB8 = stack segment
        "movq $0x10, %%gs:0xB8\n" // all subsequents switches will be from kernel mode
        "push %%gs:0xB0\n" // GS + 0xB0 = stack pointer
        "push %%gs:0xA8\n" // GS + 0xA8 = rflags
        "push %%gs:0xA0\n" // GS + 0xA0 = code segment
        "movq $0x08, %%gs:0xA0\n" // all subsequents switches will be from kernel mode
        "push %%gs:98\n" // GS + 0x98 = RIP
        "jmp .ret_instr\n"
        
        ".not_first_sw:\n"
        "push %%gs:0xA8\n" // GS + 0xA8 = rflags
        "push %%gs:0xA0\n" // GS + 0xA0 = code segment
        "movq $0x08, %%gs:0xA0\n" // all subsequents switches will be from kernel mode
        "push %%gs:0x98\n" // GS + 0x98 = RIP

        ".ret_instr:\n"
        "iretq" 
        :
        : 
        : "memory"
    );
}
