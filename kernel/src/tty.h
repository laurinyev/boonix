#ifndef __TTY_H__
#define __TTY_H__
#include <stddef.h>
#include <limine.h>

void tty_fb_init(struct limine_framebuffer* fb);
void tty_putc(char c, void* unused);

#endif // __TTY_H__
