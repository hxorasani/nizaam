#ifndef _nuk_canvas_h
#define _nuk_canvas_h

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_BUTTON_TRIGGER_ON_RELEASE
#include "../include/nuklear.min.h"

struct nk_canvas {
    struct nk_command_buffer *painter;
    struct nk_vec2 item_spacing;
    struct nk_vec2 panel_padding;
    struct nk_style_item window_background;
};

struct nk_canvas prime_canvas;

nk_bool
canvas_begin(struct nk_context *, const char *, struct nk_canvas *, nk_flags,
    int, int, int, int, struct nk_color);
void canvas_end(struct nk_context *, struct nk_canvas *);

#endif