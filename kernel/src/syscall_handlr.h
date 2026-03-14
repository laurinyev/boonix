#ifndef SYSCALL_HANDLR_H
#define SYSCALL_HANDLR_H

#include "third_party/x86.h"

#define ENOSYS    0x26

#define SYS_READ  0x00
#define SYS_WRITE 0x01
#define SYS_MMAP  0x09
#define SYS_YIELD 0x18
#define SYS_EXIT  0x3c

#define SYS_MAX_CALL SYS_EXIT

void handle_syscall(registers_syscall_t* regs);

#endif //SYSCALL_HANDLR_H
