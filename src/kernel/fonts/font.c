#include "font.h"

const struct psf2_header *font_get_header(void)
{
    return (const struct psf2_header *)_psf2_font_start;
}

const uint8_t *font_get_glyph(uint32_t character)
{
    const struct psf2_header *header = font_get_header();
    if (header->magic != PSF2_MAGIC)
        return NULL;

    if (character < header->numglyph)
    {
        return _psf2_font_start + header->headersize +
               (character * header->bytesperglyph);
    }

    return NULL;
}
