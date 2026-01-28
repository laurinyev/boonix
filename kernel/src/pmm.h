#ifndef __PMM_H__
#define __PMM_H__
#include <limine.h>

void pmm_init(struct limine_memmap_response* memap);

void* pmm_alloc();
void pmm_free(void* ptr);

#endif // __PMM_H__
