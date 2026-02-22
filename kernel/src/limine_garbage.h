#ifndef LIMINE_GARBAGE_H
#define LIMINE_GARBAGE_H
#include <limine.h>

typedef struct {
    struct limine_memmap_response* memmap; 
} limine_garbage_t;

limine_garbage_t init_limine_garbage();

#endif //LIMINE_GARBAGE_H
