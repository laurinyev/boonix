#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include "garbage.h"
#include "limine_garbage.h"
#include "cpu_stuffs.h"
#include "tty.h"
#include "pmm.h"
#include "printf.h"
#include "memstress.h"


void kmain(void) {
    load_gdt();
    tty_init();
    limine_garbage_t garbage = init_limine_garbage(); 

    pmm_init(garbage.memmap);

    memstress_test();

    kprintf("Thank you %s <3","Nekodev");

    hcf();
}
