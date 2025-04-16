#include "gop.h"

#include "limine.h"
#include "string.h"
#include "serial.h"

#define PSF2_MAGIC 0x864ab572

static uint32_t rgb_to_native(struct gop_context *ctx, uint8_t r, uint8_t g, uint8_t b)
{
    struct limine_framebuffer *fb = ctx->fb;
    return (r << fb->red_mask_shift) |
           (g << fb->green_mask_shift) |
           (b << fb->blue_mask_shift);
}

void gop_init(struct gop_context *ctx)
{
    extern volatile struct limine_framebuffer_request framebuffer_request;
    if (!framebuffer_request.response || framebuffer_request.response->framebuffer_count < 1)
    {
        serial_printf("No framebuffer available\n");
        return;
    }

    ctx->fb = framebuffer_request.response->framebuffers[0];
    ctx->rgb_to_native = rgb_to_native;
    ctx->default_fg = rgb_to_native(ctx, 0xFF, 0xFF, 0xFF);
    ctx->default_bg = rgb_to_native(ctx, 0x00, 0x00, 0x00);
    ctx->cursor_x = 0;
    ctx->cursor_y = 0;

    const struct psf2_header *font_header = (const struct psf2_header *)_psf2_font_start;
    if (font_header->magic != PSF2_MAGIC)
    {
        serial_printf("Invalid PSF2 font\n");
    }
}

void gop_clear(struct gop_context *ctx)
{
    uint32_t *fb_ptr = ctx->fb->address;
    size_t total_pixels = ctx->fb->width * ctx->fb->height;

    for (size_t i = 0; i < total_pixels; i++)
    {
        fb_ptr[i] = ctx->default_bg;
    }
    ctx->cursor_x = 0;
    ctx->cursor_y = 0;
}

void gop_draw_pixel(struct gop_context *ctx, size_t x, size_t y, uint32_t color)
{
    if (x >= ctx->fb->width || y >= ctx->fb->height)
        return;

    uint8_t *fb_ptr = (uint8_t *)ctx->fb->address;
    size_t bytes_per_pixel = (ctx->fb->bpp + 7) / 8;
    size_t offset = y * ctx->fb->pitch + x * bytes_per_pixel;

    fb_ptr[offset + ctx->fb->red_mask_shift / 8] = (color >> 16) & 0xFF;
    fb_ptr[offset + ctx->fb->green_mask_shift / 8] = (color >> 8) & 0xFF;
    fb_ptr[offset + ctx->fb->blue_mask_shift / 8] = color & 0xFF;
    if (bytes_per_pixel == 4)
    {
        fb_ptr[offset + 3] = 0xFF;
    }
}

void gop_draw_char(struct gop_context *ctx, uint32_t c)
{
    const struct psf2_header *header = (const struct psf2_header *)_psf2_font_start;
    if (header->magic != PSF2_MAGIC)
        return;

    size_t bytes_per_row = (header->width + 7) / 8;
    const uint8_t *glyph = _psf2_font_start + header->headersize + (c * header->bytesperglyph);

    for (size_t y = 0; y < header->height; y++)
    {
        uint8_t byte1 = glyph[y * bytes_per_row];
        uint8_t byte2 = glyph[y * bytes_per_row + 1];

        uint16_t row_data = (byte1 << 8) | byte2;

        for (int bit = 15; bit >= 6; bit--)
        {
            size_t x = 15 - bit;
            uint32_t color = (row_data & (1 << bit)) ? ctx->default_fg : ctx->default_bg;
            gop_draw_pixel(ctx, ctx->cursor_x + x, ctx->cursor_y + y, color);
        }
    }

    ctx->cursor_x += header->width;
}

void gop_draw_string(struct gop_context *ctx, const char *str)
{
    while (*str)
    {
        if (*str == '\n')
        {
            ctx->cursor_x = 0;
            ctx->cursor_y += ((struct psf2_header *)_psf2_font_start)->height;
            str++;
            continue;
        }

        gop_draw_char(ctx, (uint32_t)*str);
        str++;
    }
}

void gop_set_colors(struct gop_context *ctx, uint32_t fg, uint32_t bg)
{
    ctx->default_fg = fg;
    ctx->default_bg = bg;
}
