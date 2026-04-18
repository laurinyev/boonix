#include "syscall_handlr.h" 
#include "process.h"
#include "printf.h"

typedef uint64_t(*syscall_handler_t)(registers_syscall_t* regs);

uint64_t sys_read (registers_syscall_t* args){
    kprintf("sys_read: unimplemented\n");
    return -ENOSYS; 
}

uint64_t sys_write(registers_syscall_t* args){
    kprintf("sys_write: unimplemented\n");
    return -ENOSYS; 
}

uint64_t sys_mmap (registers_syscall_t* args){
    kprintf("sys_mmap: unimplemented\n");
    return -ENOSYS; 
}

//single tasking approximation of yield behaviour + starting the first proc from kernelspace
uint64_t sys_yield(registers_syscall_t* args){
    if (current_proc == 0xFFFFFFFF){
        switch_to_proc(process_count - 1, true);
    } else {
        switch_to_proc(current_proc, false);
    }
    return 0; 
}

uint64_t sys_exit (registers_syscall_t* args){
    kprintf("sys_exit: unimplemented\n");
    return -ENOSYS; 
}

syscall_handler_t handlers[] = {
    [SYS_READ ] = sys_read, 
    [SYS_WRITE] = sys_write, 
    [SYS_MMAP ] = sys_mmap, 
    [SYS_YIELD] = sys_yield, 
    [SYS_EXIT ] = sys_exit, 
};

void handle_syscall(registers_syscall_t* regs){
    //kprintf("default syscall handler, RAX=0x%x\n",regs->rax);
    if (regs->rax > SYS_MAX_CALL || !handlers[regs->rax]){
        regs->rax = -ENOSYS;
    } else {
        regs->rax = handlers[regs->rax](regs);
    }
}
