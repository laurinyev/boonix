#ifndef __TTY_H__
#define __TTY_H__
#include <stddef.h>
#include <limine.h>

void tty_init();
void tty_putc(char c, void* unused);

#endif // __TTY_H__
