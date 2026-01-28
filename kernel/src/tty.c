#include "tty.h"
#include <stdbool.h>

#include "cpu_stuffs.h"

#define COM1 0x3f8

void tty_fb_init(struct limine_framebuffer* fb){
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
    outb(COM1 + 4, 0x0F); //lets just not test it and let it explode if it doesnt work

}
void tty_putc(char c, void* unused){
    outb(COM1,c);
}
