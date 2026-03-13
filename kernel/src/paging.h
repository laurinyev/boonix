#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define PAGE_SIZE 0x1000

#define PAGE_FLAG_P  (uint64_t)1 << 0
#define PAGE_FLAG_W  (uint64_t)1 << 1
#define PAGE_FLAG_U  (uint64_t)1 << 2
#define PAGE_FLAG_NX (uint64_t)1 << 63

typedef uintptr_t pagemap_t;

bool map(pagemap_t pm,uintptr_t virt,uintptr_t phys, uint64_t flags);
bool unmap(pagemap_t pm, uintptr_t virt);

void set_cur_pagemap(pagemap_t pm);
pagemap_t get_cur_pagemap();

pagemap_t create_pagemap();

uintptr_t virt_to_phys(pagemap_t pm, uintptr_t virt);
uintptr_t find_avail_blocks(pagemap_t pm, uintptr_t num_pages);

#endif //PAGING_H
