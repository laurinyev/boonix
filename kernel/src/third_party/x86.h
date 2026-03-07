#ifndef __X86_H
#define __X86_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//===================== PORT IO ======================

inline uint8_t inb(uint16_t addr){
    uint8_t val = 0;
    asm volatile("inb %1,%0" : "=a"(val) : "dN"(addr));
    return val;
}
inline void outb(uint16_t addr, uint8_t val){
    asm volatile("outb %0,%1" :: "a"(val),"dN"(addr));
}

inline uint16_t inw(uint16_t addr){
    uint8_t val = 0;
    asm volatile("inw %1,%0" : "=a"(val) : "dN"(addr));
    return val;
}
inline void outw(uint16_t addr, uint16_t val){
    asm volatile("outw %0,%1" :: "a"(val),"dN"(addr));
}

inline uint32_t inl(uint16_t addr){
    uint8_t val = 0;
    asm volatile("inl %1,%0" : "=a"(val) : "dN"(addr));
    return val;
}
inline void outl(uint16_t addr, uint32_t val){
    asm volatile("outw %0,%1" :: "a"(val),"dN"(addr));
}

//======================= MSR ========================
#define MSR_IA32_EFER   0xC0000080
#define MSR_IA32_STAR   0xC0000081
#define MSR_IA32_LSTAR  0xC0000082
#define MSR_IA32_CSTAR  0xC0000083
#define MSR_IA32_SFMASK 0xC0000084
#define MSR_IA32_FS_BASE 0xC0000100
#define MSR_IA32_GS_BASE 0xC0000101
#define MSR_IA32_KERNEL_GS_BASE 0xC0000102

static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    asm volatile("rdmsr": "=a"(low), "=d"(high): "c"(msr));
    return ((uint64_t)high << 32) | low;
}

static inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t low = (uint32_t)value;
    uint32_t high = (uint32_t)(value >> 32);
    asm volatile("wrmsr":: "c"(msr), "a"(low), "d"(high));
}

//==================== INTERRUPTS ====================

typedef struct {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rax;

    uint64_t intno;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed)) registers_t;

typedef struct {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t rdx;
    uint64_t rax;

    uint64_t rflags;
    uint64_t rip;
} __attribute__((packed)) registers_syscall_t;

inline void lidt(void* idtr) {
    asm volatile("lidt (%0)":: "r"(idtr));
}

//==================== SEGMENTATION ==================
typedef uint64_t gdt_desc_t;

typedef struct {
    uint16_t size;
    gdt_desc_t* address;
} __attribute__((packed)) gdtr_t;

#define GDT_CODE64 0x00af9b000000ffff
#define GDT_DATA64 0x00af93000000ffff
#define GDT_CODE64U 0x00aff3000000ffff
#define GDT_DATA64U 0x00affb000000ffff

//Offsets: 
// * kernel code: 0x8
// * kernel data: 0x10
// * user   code: 0x18
// * user   data: 0x20
#define GDT_PREBAKED {0, GDT_CODE64, GDT_DATA64, GDT_CODE64U, GDT_DATA64U }

inline void lgdt(gdtr_t* gdtr) {
    asm volatile(
        "lgdt (%0)\n"
        "pushq $0x08\n"
        "lea 1f(%%rip), %%rax\n"
        "pushq %%rax\n"
        "lretq\n"
        "1:\n"
        "movw $0x10, %%ax\n"
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
        "movw %%ax, %%ss\n"
        :
        : "r"(gdtr)
        : "rax"
    );
}

inline void ltr(uint16_t tss_selector) {
    asm volatile("ltr %0":: "r"(tss_selector));
}

//=================== VIRTUAL MEMORY =================

inline void load_cr3(uint64_t cr3) {
    asm volatile("mov %0, %%cr3":: "r"(cr3));
}

inline void invlpg(uint64_t addr) {
    asm volatile("invlpg (%0)":: "r"(addr) : "memory");
}

//=================== CPU CONTROL ====================
inline void sti(void) {
    asm volatile("sti");
}

inline void cli(void) {
    asm volatile("cli");
}

inline void hlt(void) {
    asm volatile("hlt");
}

inline void jmp(void* addr) {
    asm volatile("jmp *%0":: "r"(addr));
}

#endif // __X86_H
