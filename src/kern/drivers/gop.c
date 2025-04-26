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

    const uint16_t *font_magic = (uint16_t *)_psf2_font_start;

    if (*(uint32_t *)_psf2_font_start == PSF2_MAGIC)
    {
        // PSF2 font handling
        ctx->font_type = FONT_PSF2;
        ctx->font_header.psf2 = (struct psf2_header *)_psf2_font_start;
        ctx->font_width = ctx->font_header.psf2->width;
        ctx->font_height = ctx->font_header.psf2->height;
    }
    else if (*font_magic == PSF1_MAGIC)
    {
        // PSF1 font handling
        ctx->font_type = FONT_PSF1;
        ctx->font_header.psf1 = (struct psf1_header *)_psf2_font_start;
        ctx->font_width = 8;
        ctx->font_height = ctx->font_header.psf1->char_size;
    }
    else
    {
        serial_printf("Unknown font format: 0x%04x\n", *font_magic);
        return;
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
    if (ctx->font_type == FONT_PSF2)
    {
        // PSF2 rendering
        struct psf2_header *header = ctx->font_header.psf2;
        const size_t bytes_per_row = (header->width + 7) / 8;
        const uint8_t *glyph = _psf2_font_start + header->headersize + (c * header->bytesperglyph);

        for (size_t y = 0; y < header->height; y++)
        {
            for (size_t byte = 0; byte < bytes_per_row; byte++)
            {
                const uint8_t glyph_byte = glyph[y * bytes_per_row + byte];
                for (size_t bit = 0; bit < 8; bit++)
                {
                    const size_t x = byte * 8 + bit;
                    if (x >= header->width)
                        break;
                    const uint8_t mask = 0x80 >> bit;
                    const uint32_t color = (glyph_byte & mask) ? ctx->default_fg : ctx->default_bg;
                    gop_draw_pixel(ctx, ctx->cursor_x + x, ctx->cursor_y + y, color);
                }
            }
        }
    }
    else
    {
        struct psf1_header *header = ctx->font_header.psf1;

        // Boundary check
        const uint32_t max_glyph = (header->mode & 0x01) ? 512 : 256;
        if (c >= max_glyph)
            c = 0;

        const uint8_t *glyph = (uint8_t *)_psf2_font_start + sizeof(struct psf1_header) + (c * header->char_size);

        // Render each scanline
        for (size_t y = 0; y < header->char_size; y++)
        {
            const uint8_t row = glyph[y];

            // Render 8 pixels per row
            for (size_t bit = 0; bit < 8; bit++)
            {
                const uint8_t mask = 0x80 >> bit;
                const uint32_t color = (row & mask) ? ctx->default_fg : ctx->default_bg;
                gop_draw_pixel(ctx,
                               ctx->cursor_x + bit,
                               ctx->cursor_y + y,
                               color);
            }
        } // Advance cursor
        ctx->cursor_x += ctx->font_width;
        if (ctx->cursor_x + ctx->font_width > ctx->fb->width)
        {
            ctx->cursor_x = 0;
            ctx->cursor_y += ctx->font_height;
        }
    }
}

void gop_draw_string(struct gop_context *ctx, const char *str)
{
    while (*str)
    {
        if (*str == '\n')
        {
            ctx->cursor_x = 0;
            ctx->cursor_y += ctx->font_height;
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
