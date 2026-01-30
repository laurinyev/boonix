#include "pmm.h"
#include <stddef.h>
#include "printf.h"
#include "garbage.h"

#define PAGE_SIZE 0x1000
uint64_t usable_mem_start = 0;
uint64_t total_range = 0;
uint64_t total_mem = 0;
uint64_t used_mem = 0;
uint64_t free_mem = 0;

uint64_t lowest_free_page = 0;

uint8_t* bitmap_ptr = NULL;
uint64_t bitmap_size = 0;

extern uintptr_t hhdm_offset;

static void mark_page(uintptr_t page) {
    uintptr_t offset = page / 8;
    uint8_t   bit    = page % 8;
    if (offset <= bitmap_size){
        bitmap_ptr[offset] = bitmap_ptr[offset] | (1 << bit); 
    }
}

static void unmark_page(uintptr_t page) {
    uintptr_t offset = page / 8;
    uint8_t   bit    = page % 8;
    if (offset <= bitmap_size){
        bitmap_ptr[offset] = bitmap_ptr[offset] & ~(1 << bit); 
    }
}

static void mark_region(uintptr_t start, uintptr_t size) {
    uintptr_t first = start;
    uintptr_t last  = start + size - 1;

    uintptr_t first_byte = first >> 3;
    uintptr_t last_byte  = last  >> 3;

    uint8_t first_mask = 0xFFu << (first & 7);
    uint8_t last_mask  = 0xFFu >> (7 - (last & 7));

    if (first_byte == last_byte) {
        bitmap_ptr[first_byte] |= (first_mask & last_mask);
        return;
    }

    bitmap_ptr[first_byte] |= first_mask;
    bitmap_ptr[last_byte]  |= last_mask;

    if (first_byte + 1 < last_byte) {
        memset(bitmap_ptr + first_byte + 1, 0xFF, last_byte - first_byte - 1);
    }
}

static void unmark_region(uintptr_t start, uintptr_t size) {
    uintptr_t first = start;
    uintptr_t last  = start + size - 1;

    uintptr_t first_byte = first >> 3;
    uintptr_t last_byte  = last  >> 3;

    uint8_t first_mask = 0xFFu << (first & 7);
    uint8_t last_mask  = 0xFFu >> (7 - (last & 7));

    if (first_byte == last_byte) {
        bitmap_ptr[first_byte] &= ~(first_mask & last_mask);
        return;
    }

    bitmap_ptr[first_byte] &= ~first_mask;
    bitmap_ptr[last_byte]  &= ~last_mask;

    if (first_byte + 1 < last_byte) {
        memset(bitmap_ptr + first_byte + 1, 0xFF, last_byte - first_byte - 1);
    }
}

#define PURE(n) (uintptr_t)(((uintptr_t)n - usable_mem_start)/PAGE_SIZE)

void pmm_init(struct limine_memmap_response* memmap) {
    struct limine_memmap_entry* largest = memmap->entries[0];
    
    uint64_t highest_end = 0;
    for(size_t i = 0; i < memmap->entry_count; i++){
        struct limine_memmap_entry* e = memmap->entries[i];
        //kprintf("Memory map entry at 0x%x with size 0x%x and type 0x%x!\n",e->base,e->length,e->type);
        if (e->type == LIMINE_MEMMAP_USABLE){
            total_mem += e->length;
            if (e->length > largest->length){
                largest = e;
            }
            if (e->base < usable_mem_start) {
                usable_mem_start = e->base;
            }
            if (e->base + e->length > highest_end){
                highest_end = e->base + e->length;
            }
        }
    }
    total_range = highest_end - usable_mem_start;
    bitmap_size = total_range / (PAGE_SIZE * 8);
    total_mem -= bitmap_size;
    lowest_free_page = usable_mem_start;
    used_mem = 0;
    free_mem = total_mem;

    if (largest->length < bitmap_size){
        kprintf("ERROR! largest memory region doesn't fit the memory map!");
        hcf();
    }

    bitmap_ptr = (uint8_t*)(hhdm_offset + largest->base);

    memset(bitmap_ptr, 0xFF, bitmap_size);

    for(size_t i = 0; i < memmap->entry_count; i++){
        struct limine_memmap_entry* e = memmap->entries[i];
        if (e->type == LIMINE_MEMMAP_USABLE && e->base >= usable_mem_start && e->base + e->length <= usable_mem_start + total_range) {
            unmark_region(PURE(e->base),e->length/PAGE_SIZE);
        }
    }
    
    mark_region(PURE(largest->base),bitmap_size);
    mark_page(0); // 0 is for error lul

    kprintf("Available memory: %u bytes!\n",total_mem);
}

void* pmm_alloc() {
    for(uintptr_t c = lowest_free_page; c - usable_mem_start <= total_range; c+=PAGE_SIZE){
        uintptr_t c_pure = PURE(c);
        uintptr_t offset = c_pure / 8;
        uint8_t   bit    = c_pure % 8;
        uint8_t   byte   = bitmap_ptr[offset];
        //kprintf("c = 1x%llx; c_pure = 0x%x; byte = 0x%x; bit = 0x%x\n",c,c_pure,byte,bit);
        if (byte == 0xFF) {
            c+= PAGE_SIZE * ((8-bit)-1);
            continue;
        }
        //kprintf("E 0x%llx\n",(byte & (1 << bit)));
        if ((byte & (1 << bit)) == 0) {
            mark_page(c_pure);
            lowest_free_page = c + PAGE_SIZE;
            used_mem += PAGE_SIZE;
            free_mem -= PAGE_SIZE;
            return (void*)c;
        }
    }
    kprintf("Failed to allocate!(lowest_free_page=0x%llx; usable_mem_start=0x%llx; total_range=0x%llx)\n",lowest_free_page,usable_mem_start,total_range);
    return NULL;
}

void pmm_free(void* ptr) {
    if ((uintptr_t)ptr > hhdm_offset){
        ptr -= hhdm_offset;
    }
    
    if (lowest_free_page > (uintptr_t)ptr) {
        lowest_free_page = (uint64_t)ptr;
    }

    free_mem += PAGE_SIZE;
    used_mem -= PAGE_SIZE;

    if (used_mem > total_mem){
        kprintf("PMM: something horrible happened(used memory underflow)");
    }

    unmark_page((uintptr_t)ptr/PAGE_SIZE);
}
