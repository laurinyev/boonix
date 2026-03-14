#include "syscall.h" 
#include "syscall_handlr.h"
#include "third_party/x86.h"

void syscall(registers_syscall_t* regs){
    //TODO: syscall redirecting
    handle_syscall(regs); 
}

__attribute__((naked)) void syscall_handler() {
    //TODO: switch stacks
    asm (
        "push %%rcx\n"
        "push %%r11\n"
    
        "push %%rax\n" 
        "push %%rdx\n" 
        "push %%rbx\n" 
        "push %%rbp\n" 
        "push %%rsi\n" 
        "push %%rdi\n" 
        "push %%r8\n" 
        "push %%r9\n" 
        "push %%r10\n" 
        "push %%r12\n" 
        "push %%r13\n" 
        "push %%r14\n" 
        "push %%r15\n" 
  
        "mov %%rsp, %%rdi\n"
        "call syscall\n"

        "pop %%r15\n" 
        "pop %%r14\n" 
        "pop %%r13\n" 
        "pop %%r12\n" 
        "pop %%r10\n" 
        "pop %%r9\n" 
        "pop %%r8\n" 
        "pop %%rdi\n" 
        "pop %%rsi\n" 
        "pop %%rbp\n" 
        "pop %%rbx\n" 
        "pop %%rdx\n" 
        "pop %%rax\n" 

        "popfq\n"
        "pop %%rcx\n"
        "jmp %%rcx\n"
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
