#pragma once

#include <stdint.h>
#include <stddef.h>

extern const uint8_t _psf2_font_start[];
extern const uint8_t _psf2_font_end[];
extern const size_t _psf2_font_size;

struct psf2_header {
    uint32_t magic;
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t numglyph;
    uint32_t bytesperglyph;
    uint32_t height;
    uint32_t width;
};

#define PSF2_MAGIC 0x864ab572

// PSF2 flags
#define PSF2_HAS_UNICODE_TABLE 0x01

const struct psf2_header *font_get_header(void);
const uint8_t *font_get_glyph(uint32_t character);
