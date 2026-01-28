#include "pmm.h"
#include <stddef.h>
#include "printf.h"
#include "garbage.h"

#define PAGE_SIZE 4096

uint64_t total_mem = 0;
uint64_t used_mem = 0;
uint64_t free_mem = 0;

uint8_t* bitmap_ptr = NULL;
uint64_t bitmap_size = 0;

extern uintptr_t hhdm_offset;

void pmm_init(struct limine_memmap_response* memmap) {
    struct limine_memmap_entry* largest = memmap->entries[0];
    
    for(size_t i = 0; i < memmap->entry_count; i++){
        struct limine_memmap_entry* e = memmap->entries[i];
        kprintf("Memory map entry at 0x%x with size 0x%x and type 0x%x!\n",e->base,e->length,e->type);
        if (e->type == LIMINE_MEMMAP_USABLE){
            total_mem += e->length;
            if (e->length > largest->length){
                largest = e;
            }
        }
    }

    bitmap_size = total_mem / (PAGE_SIZE * 8);

    kprintf("Available memory: %u bytes!\n",total_mem);
    kprintf("Bitmap size: %u bytes!\n",bitmap_size);

    if (largest->length < bitmap_size){
        kprintf("ERROR! largest memory region doesn't fit the memory map!");
        hcf();
    }

    bitmap_ptr = (uint8_t*)(hhdm_offset + largest->base);

    kprintf("Bitmap location: %x\n",bitmap_ptr);
}

void* pmm_alloc() {
    return NULL;
}

void pmm_free(void* ptr) {

}
