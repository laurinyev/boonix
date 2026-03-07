#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include "paging.h"
#include "garbage.h"
#include "limine_garbage.h"
#include "third_party/x86.h"
#include "syscall.h"
#include "tty.h"
#include "pmm.h"
#include "printf.h"
#include "memstress.h"

extern uintptr_t hhdm_offset;

static gdt_desc_t gdt[] = GDT_PREBAKED; 

void kmain(void) {
    //pretty easy huh?  
    gdtr_t gdtr = {.size    = sizeof(gdt) - 1,
                   .address = gdt};
    lgdt(&gdtr);

    tty_init();
    limine_garbage_t garbage = init_limine_garbage(); 

    pmm_init(garbage.memmap);

    pagemap_t pm = get_cur_pagemap();

    kprintf("Current pagemap address: 0x%llx\n",pm);
    uintptr_t addr = virt_to_phys(pm,hhdm_offset + 0x6000);
    kprintf("Physical address of HHDM + 0x1069: 0x%llx\n",addr);
    uintptr_t found = find_avail_blocks(pm,1);
    kprintf("Found available virtual memory: 0x%llx\n",found);

    syscall_init(); 
    asm volatile("syscall");

    if(map(pm,found,addr,0x3)) {
        *(uint64_t*)found = 0x69;
        kprintf("First long of newly mapped page: 0x%llx\n",*(uint64_t*)found);
        kprintf("First long of HHDM + 0x1069: 0x%llx\n",*(uint64_t*)(hhdm_offset + 0x6000));
    } else {
        kprintf("Mapping failed!\n"); 
    }

    //memstress_test();

    kprintf("Thank you %s <3","Nekodev");

    hcf();
}
