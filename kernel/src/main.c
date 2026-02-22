#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include "paging.h"
#include "garbage.h"
#include "limine_garbage.h"
#include "cpu_stuffs.h"
#include "tty.h"
#include "pmm.h"
#include "printf.h"
#include "memstress.h"

extern uintptr_t hhdm_offset;

void kmain(void) {
    load_gdt();
    tty_init();
    limine_garbage_t garbage = init_limine_garbage(); 

    pmm_init(garbage.memmap);

    pagemap_t pm = get_cur_pagemap();

    kprintf("Current pagemap address: 0x%llx\n",pm);
    kprintf("Physical address of HHDM + 0x1069: 0x%llx\n",virt_to_phys(pm,hhdm_offset + 0x1069));

    //memstress_test();

    kprintf("Thank you %s <3","Nekodev");

    hcf();
}
