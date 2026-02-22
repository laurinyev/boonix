#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uintptr_t pagemap_t;

bool map(pagemap_t pm,uintptr_t virt,uintptr_t phys, uint64_t flags);
bool unmap(pagemap_t pm, uintptr_t virt);

void set_cur_pagemap(pagemap_t pm);
pagemap_t get_cur_pagemap();

uintptr_t virt_to_phys(pagemap_t pm, uintptr_t virt);
uintptr_t find_avail_blocks(pagemap_t pm, uintptr_t num_pages);

#endif //PAGING_H
