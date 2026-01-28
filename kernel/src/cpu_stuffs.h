#ifndef __CPU_STUFFS_H__
#define __CPU_STUFFS_H__

#include <stdint.h>

//Load the (useless) Garbage Descriptor Table
void load_gdt();

uint8_t inb(uint16_t addr);
void outb(uint16_t addr, uint8_t val);

#endif // __CPU_STUFFS_H__
