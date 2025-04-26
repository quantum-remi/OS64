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
    enum
    {
        FONT_PSF1,
        FONT_PSF2
    } font_type;
    union
    {
        struct psf1_header *psf1;
        struct psf2_header *psf2;
    } font_header;
    uint16_t font_width;
    uint16_t font_height;
};

#define PSF1_MAGIC 0x0436
struct psf1_header
{
    uint16_t magic;
    uint8_t mode;
    uint8_t char_size;
};

#define PSF2_MAGIC 0x864ab572
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
