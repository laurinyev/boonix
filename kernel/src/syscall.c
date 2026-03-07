#include "syscall.h"

#include "third_party/x86.h"
#include "printf.h"

void syscall(registers_syscall_t* regs){
    kprintf("hello from syscall handler!\n");
}

__attribute__((naked)) void syscall_handler() {
    //TODO: switch stacks
    asm (
        "push %%rcx\n\t"
        "push %%r11\n\t"
    
        "push %%rax\n\t" 
        "push %%rdx\n\t" 
        "push %%rbx\n\t" 
        "push %%rbp\n\t" 
        "push %%rsi\n\t" 
        "push %%rdi\n\t" 
        "push %%r8\n\t" 
        "push %%r9\n\t" 
        "push %%r10\n\t" 
        "push %%r12\n\t" 
        "push %%r13\n\t" 
        "push %%r14\n\t" 
        "push %%r15\n\t" 
  
        "mov %%rsp, %%rdi\n\t"
        "call syscall\n\t"

        "pop %%r15\n\t" 
        "pop %%r14\n\t" 
        "pop %%r13\n\t" 
        "pop %%r12\n\t" 
        "pop %%r10\n\t" 
        "pop %%r9\n\t" 
        "pop %%r8\n\t" 
        "pop %%rdi\n\t" 
        "pop %%rsi\n\t" 
        "pop %%rbp\n\t" 
        "pop %%rbx\n\t" 
        "pop %%rdx\n\t" 
        "pop %%rax\n\t" 

        "popfq\n\t"
        "pop %%rcx\n\t"
        "jmp %%rcx\n\t"
        ::: "memory"
    );
}

void syscall_init(){
    uint64_t efer = rdmsr(MSR_IA32_EFER);
    wrmsr(MSR_IA32_EFER, efer | 1);

    wrmsr(MSR_IA32_LSTAR, (uint64_t)syscall_handler);
    wrmsr(MSR_IA32_STAR, ((uint64_t)0x08 << 48) | ((uint64_t)0x08 << 32));
    wrmsr(MSR_IA32_SFMASK, (1 << 10) | (1 << 9));
}
