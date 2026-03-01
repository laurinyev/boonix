#include "paging.h"
#include "garbage.h"
#include "printf.h"
#include "pmm.h"

extern uintptr_t hhdm_offset;
static uintptr_t get_next_level(uintptr_t cur_addr, uint16_t cur_offset, bool make_new, uint64_t flags) {
    uint64_t* cur_lvl = (uint64_t*)(cur_addr + hhdm_offset); 
    uintptr_t cur_entry = cur_lvl[cur_offset];
    if (make_new && !(cur_entry & 1)) {
        uintptr_t new_page  = (uintptr_t)pmm_alloc();
        if (new_page == 0){
            return 0;
        }
        memset((void*)(new_page + hhdm_offset), 0, 4096);
        cur_entry = (new_page & 0x000ffffffffff000) | flags;
        cur_lvl[cur_offset] = cur_entry; 
    }
    return cur_entry;
}

bool map(pagemap_t pm,uintptr_t virt,uintptr_t phys, uint64_t flags){
    uint16_t off_l4 = (((virt) >> 39) & 0x1ff);
    uint16_t off_l3 = (((virt) >> 30) & 0x1ff);
    uint16_t off_l2 = (((virt) >> 21) & 0x1ff);
    uint16_t off_l1 = (((virt) >> 12) & 0x1ff);
    
    uintptr_t next_level = get_next_level(pm,off_l4,true,flags);
    if(!(next_level & 1)){
        return false;
    }
    next_level &= 0x000ffffffffff000;
    
    next_level = get_next_level(next_level,off_l3,true,flags);
    if(!(next_level & 1)){
        return false;
    }
    next_level &= 0x000ffffffffff000;
    
    next_level = get_next_level(next_level,off_l2,true,flags);
    if(!(next_level & 1)){
        return false;
    }
    next_level &= 0x000ffffffffff000;
   
    *((uint64_t*)(next_level + hhdm_offset) + off_l1) = (phys & 0x000ffffffffff000) | flags;
    asm volatile("invlpg %0"::"m"(pm) : "memory");

    return true;
}

bool unmap(pagemap_t pm, uintptr_t virt){
    uint16_t off_l4 = (((virt) >> 39) & 0x1ff);
    uint16_t off_l3 = (((virt) >> 30) & 0x1ff);
    uint16_t off_l2 = (((virt) >> 21) & 0x1ff);
    uint16_t off_l1 = (((virt) >> 12) & 0x1ff);
    
    uintptr_t next_level = get_next_level(pm,off_l4,false,0);
    if(!(next_level & 1)){
        return false;
    }
    next_level &= 0x000ffffffffff000;
    
    next_level = get_next_level(next_level,off_l3,false,0);
    if(!(next_level & 1)){
        return false;
    }
    next_level &= 0x000ffffffffff000;
    
    next_level = get_next_level(next_level,off_l2,false,0);
    if(!(next_level & 1)){
        return false;
    }
    next_level &= 0x000ffffffffff000;
    
    next_level = get_next_level(next_level, off_l1,false,0);
    if(!(next_level & 1)){
        return false;
    }
    next_level &= 0x000ffffffffff000;
   
    *(uint64_t*)(next_level + hhdm_offset) &= ~1;
    asm volatile("invlpg %0"::"m"(pm) : "memory");
 
    return true;
}

void set_cur_pagemap(pagemap_t pm){
    asm volatile("mov %%cr3, %0"::"r"(pm) : "memory");
}

pagemap_t get_cur_pagemap(){
    pagemap_t page;
    asm volatile("mov %%cr3, %0": "=r"(page) :: "memory");
    return page;
}


uintptr_t virt_to_phys(pagemap_t pm, uintptr_t virt){
    uint16_t off_l4 = (((virt) >> 39) & 0x1ff);
    uint16_t off_l3 = (((virt) >> 30) & 0x1ff);
    uint16_t off_l2 = (((virt) >> 21) & 0x1ff);
    uint16_t off_l1 = (((virt) >> 12) & 0x1ff);
    
    uintptr_t next_level = get_next_level(pm,off_l4,false,0);
    if(!(next_level & 1)){
        return 0;
    }
    next_level &= 0x000ffffffffff000;
    
    next_level = get_next_level(next_level,off_l3,false,0);
    if(!(next_level & 1)){
        return 0;
    }
    next_level &= 0x000ffffffffff000;
    
    next_level = get_next_level(next_level,off_l2,false,0);
    if(!(next_level & 1)){
        return 0;
    }
    next_level &= 0x000ffffffffff000;
    
    next_level = get_next_level(next_level, off_l1,false,0);
    if(!(next_level & 1)){
        return 0;
    }
    next_level &= 0x000ffffffffff000;
    
    return next_level | (virt & 0xfff);
}

//HACK HACK HACK: naive ass implementation
const uintptr_t PAGE_SIZE = 0x1000;
uintptr_t find_avail_blocks(pagemap_t pm, uintptr_t num_pages) {
    if (num_pages == 0)
        return 0;

    uintptr_t run_start = 0;
    uintptr_t run_len   = 0;

    for (uintptr_t p = 0; p + PAGE_SIZE <= hhdm_offset; p += PAGE_SIZE) {
        if (!virt_to_phys(pm, p)) {
            if (run_len == 0)
                run_start = p;

            run_len++;

            if (run_len == num_pages)
                return run_start;
        } else {
            run_len = 0;
        }
    }

    return 0;
}
