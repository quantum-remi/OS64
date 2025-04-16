#pragma once

#include <stdint.h>
#include <stddef.h>
#include <limine.h>

extern const uint8_t _psf2_font_start[];
extern const uint8_t _psf2_font_end[];
extern const size_t _psf2_font_size;

struct gop_context
{
    struct limine_framebuffer *fb;
    uint32_t default_fg;
    uint32_t default_bg;
    size_t cursor_x;
    size_t cursor_y;
    uint32_t (*rgb_to_native)(struct gop_context *ctx, uint8_t r, uint8_t g, uint8_t b);
};

struct psf2_header
{
    uint32_t magic;
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t numglyph;
    uint32_t bytesperglyph;
    uint32_t height;
    uint32_t width;
};

void gop_init(struct gop_context *ctx);
void gop_clear(struct gop_context *ctx);
void gop_draw_pixel(struct gop_context *ctx, size_t x, size_t y, uint32_t color);
void gop_draw_char(struct gop_context *ctx, uint32_t c);
void gop_draw_string(struct gop_context *ctx, const char *str);
void gop_set_colors(struct gop_context *ctx, uint32_t fg, uint32_t bg);
