#include "memstress.h"
#include <stdint.h>
#include <stddef.h>
#include "pmm.h"
#include "printf.h"

static void* pages[MAX_PAGES];
extern uintptr_t hhdm_offset;

static int is_aligned(void* p) {
    return ((uintptr_t)p & (PAGE_SIZE - 1)) == 0;
}

static int overlap_check(size_t count) {
    for (size_t i = 0; i < count; i++) {
        uintptr_t a0 = (uintptr_t)pages[i];
        uintptr_t a1 = a0 + PAGE_SIZE;

        for (size_t j = i + 1; j < count; j++) {
            uintptr_t b0 = (uintptr_t)pages[j];
            uintptr_t b1 = b0 + PAGE_SIZE;

            if (!(a1 <= b0 || b1 <= a0)) {
                kprintf("PMM ERROR: overlap between %zu and %zu\n", i, j);
                return -1;
            }
        }
    }
    return 0;
}

static int pattern_check(uint8_t* p, uint8_t pat) {
    for (size_t i = 0; i < PAGE_SIZE; i++) {
        if (p[i] != pat) {
            kprintf("PMM ERROR: bad memory at %p offset %zu expected %x got %x\n",
                    p, i, pat, p[i]);
            return -1;
        }
    }
    return 0;
}

static void pattern_fill(uint8_t* p, uint8_t pat) {
    for (size_t i = 0; i < PAGE_SIZE; i++) {
        p[i] = pat;
    }
}

void memstress_test(void) {
    size_t count = 0;

    for (; count < MAX_PAGES; count++) {
        void* p = pmm_alloc();
        if (!p) {
            kprintf("PMM TEST: allocated 0x%x pages!\n", count);
            break;
        }

        if (!is_aligned(p)) {
            kprintf("PMM ERROR: unaligned page %p\n", p);
            return;
        }

        pages[count] = p + hhdm_offset;
        pattern_fill(p + hhdm_offset, PAT1);
    }
    
    if (count == MAX_PAGES){
        kprintf("PMM TEST: inital allocation OK\n");
    }

    // Overlap check
    if (overlap_check(count) != 0) return;
    kprintf("PMM TEST: overlap check OK\n");


    for (size_t i = 0; i < count; i++) {
        if (pattern_check(pages[i], PAT1) != 0) return;
    }
    kprintf("PMM TEST: initial pattern OK\n");

    for (size_t i = 0; i < count; i += 2) {
        pmm_free(pages[i]);
        pages[i] = NULL;
    }
    kprintf("PMM TEST: freeing every other page OK\n");

    for (size_t i = 0; i < count; i += 2) {
        void* p = pmm_alloc();
        if (!p) {
            kprintf("PMM ERROR: failed to reallocate page %zu\n", i);
            return;
        }
        pages[i] = p + hhdm_offset;
        pattern_fill(p + hhdm_offset, PAT2);
    }
    kprintf("PMM TEST: reallocate half OK\n");

    for (size_t i = 0; i < count; i++) {
        uint8_t expected = (i % 2 == 0) ? PAT2 : PAT1;
        if (pattern_check(pages[i], expected) != 0) return;
    }
    kprintf("PMM TEST: pattern coexistence OK\n");

    for (size_t i = 0; i < count; i++) {
        if (pages[i]) {
            pmm_free(pages[i]);
            pages[i] = NULL;
        }
    }

    kprintf("PMM TEST: completed\n");
}

