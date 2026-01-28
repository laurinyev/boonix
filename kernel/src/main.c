#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include "garbage.h"
#include "cpu_stuffs.h"
#include "tty.h"
#include "pmm.h"
#include "printf.h"

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_req = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_req = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};

//__attribute__((used, section(".limine_requests")))
//static volatile struct limine_framebuffer_request framebuffer_request = {
//    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
//    .revision = 0
//};

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

uintptr_t hhdm_offset = 0;

void kmain(void) {
    load_gdt();
    tty_init();
    
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        kprintf("ERROR! unsupported limine base revision"); 
        hcf();
    }

   //if (framebuffer_request.response == NULL
   //  || framebuffer_request.response->framebuffer_count < 1) {
   //     hcf();
   //}

    if (memmap_req.response == NULL) {
        kprintf("ERROR! no memory map\n");
        hcf(); // no ~~bitches~~ memory map
    }

    if (hhdm_req.response == NULL) {
        kprintf("ERROR! no hhdm offset\n");
        hcf(); // no ~~bitches~~ hhdm offset
    }

    kprintf("HHDM offset: 0x%x\n",hhdm_req.response->offset);

    hhdm_offset = hhdm_req.response->offset;

    pmm_init(memmap_req.response);

    kprintf("Thank you %s <3","NekoDev");

    hcf();
}
