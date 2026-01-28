#include "tty.h"
#include <stdbool.h>

#include <third_party/flanterm/src/flanterm.h>
#include <third_party/flanterm/src/flanterm_backends/fb.h>

struct flanterm_context* ft_ctx = NULL;


void tty_fb_init(struct limine_framebuffer* fb){
    ft_ctx = flanterm_fb_init(
        NULL,
        NULL,
        fb->address, fb->width, fb->height, fb->pitch,
        fb->red_mask_size, fb->red_mask_shift,
        fb->blue_mask_size, fb->green_mask_shift,
        fb->green_mask_size, fb->blue_mask_shift,
        NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, 0, 0, 1,
        0, 0,
        0,0
    );
}
void tty_write(char* buf, size_t len){
    if (ft_ctx) {
        flanterm_write(ft_ctx, buf, len);
    }
}