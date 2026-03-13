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
#include "process.h"

extern uintptr_t hhdm_offset;

static gdt_desc_t gdt[] = GDT_PREBAKED; 

void test_proc() {
    asm volatile("syscall":: "a"(0));
    while(1);
}

void spawn_test_proc() {
    uint32_t proc_id  = create_process();
    process_t* proc   = &process_stack[proc_id];
    registers_t* regs = &proc->main_thread.regs;
    
    pagemap_t pm = get_cur_pagemap();
    uintptr_t test_proc_phys = virt_to_phys(pm,(uintptr_t)&test_proc);

    map(proc->pagemap,(uintptr_t)&test_proc,test_proc_phys, PAGE_FLAG_P | PAGE_FLAG_U);

    regs->rip = (uint64_t)&test_proc; 
    regs->cs  = 0x08; 
    regs->ss  = 0x10; 
    switch_to_proc(proc_id); // lets try to just jump to it
}

void kmain(void) {
    tty_init();
    limine_garbage_t garbage = init_limine_garbage(); 

    pmm_init(garbage.memmap);

    // Initialize GDT after the PMM so we can allocate a kernel stack    
    gdtr_t gdtr = {.size    = sizeof(gdt) - 1,
                   .address = gdt};
    lgdt(&gdtr);

    //memstress_test();
    
    //pagemap_t pm = get_cur_pagemap();
    //kprintf("Current pagemap address: 0x%llx\n",pm);
    //uintptr_t addr = virt_to_phys(pm,hhdm_offset + 0x6000);
    //kprintf("Physical address of HHDM + 0x1069: 0x%llx\n",addr);
    //uintptr_t found = find_avail_blocks(pm,1);
    //kprintf("Found available virtual memory: 0x%llx\n",found);

    //if(map(pm,found,addr,0x3)) {
    //    *(uint64_t*)found = 0x69;
    //    kprintf("First long of newly mapped page: 0x%llx\n",*(uint64_t*)found);
    //    kprintf("First long of HHDM + 0x1069: 0x%llx\n",*(uint64_t*)(hhdm_offset + 0x6000));
    //} else {
    //    kprintf("Mapping failed!\n"); 
    //}

    
    syscall_init(); 

    kprintf("Thank you %s <3\n","Nekodev");
    spawn_test_proc();

    hcf();
}
