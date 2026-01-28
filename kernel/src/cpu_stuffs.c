#include "cpu_stuffs.h"
#include <stdint.h>

static uint64_t gdt[7] = {
    0x0000000000000000, // 0x00 null
    0x00af9b000000ffff, // 0x08 64-bit kernel code
    0x00af93000000ffff, // 0x10 64-bit kernel data
    0x00aff3000000ffff, // 0x18 64-bit user data
    0x00affb000000ffff, // 0x20 64-bit user code
    0x0020890000000000, // 0x28 low IST
    0x0000000000000000  // 0x30 high ist (null?)
};

typedef struct {
    uint16_t size;
    uint64_t address;
} __attribute__((packed)) gdtr_t;

void load_gdt(){
    gdtr_t gdtr = {.size    = sizeof(gdt) - 1,
                   .address = (uintptr_t)gdt};

    asm volatile("lgdt (%%rax)" : : "a"(&gdtr) : "memory");
    asm volatile("ltr %%ax" : : "a"(0x28));
    asm volatile("swapgs;"
                 "mov $0, %%ax;"
                 "mov %%ax, %%gs;"
                 "mov %%ax, %%fs;"
                 "swapgs;"
                 "mov $0x10, %%ax;"
                 "mov %%ax, %%ds;"
                 "mov %%ax, %%es;"
                 "mov %%ax, %%ss;"
                 "pushq $0x8;"
                 "pushq $.reload;"
                 "retfq;"
                 ".reload:"
                 :
                 :
                 : "ax");
}

uint8_t inb(uint16_t addr){
    uint8_t val = 0;
    asm volatile("inb %1,%0" : "=a"(val) : "dN"(addr));
    return val;
}
void outb(uint16_t addr, uint8_t val){
    asm volatile("outb %0,%1" :: "a"(val),"dN"(addr));
}
