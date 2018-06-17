#include "vga_img.h"
/*
void vga_img_create(hsVgaImage img, vga_length_t w, vga_length_t h, const void *data) {

    img->flags = 0;
    img->color_trans = 0;
    img->w = w;
    img->h = h;
    img->data = data;
}

void vga_img_make_transparent(hsVgaImage img, vga_color_index_t color) {
    
    img->flags |= VGA_IMG_FLG_TRANSPARENT;
    img->color_trans = color;
}

void vga_create_draw_screen(hsVgaDraw draw, hcsVgaImage src, vga_coord_t x, vga_coord_t y) {
    
    // image completely on-screen?
    if (x >= 0 && (x + src->w <= VGA_SCREEN_W) && y >= 0 && (y + src->h <= VGA_SCREEN_H)) {
        
        draw->src_start     = 0;
        draw->dst_start     = VGA_SCREEN_W * y + x;
        draw->line_length   = src->w;
        draw->src_skip      = 0;
        draw->dst_skip      = VGA_SCREEN_W - src->w;
        draw->line_count    = src->h;
        return;
    }

    // image completely out of screen?
    if ((x >= VGA_SCREEN_W) || (y >= VGA_SCREEN_H) || ((x + src->w) < 0) || ((y + src->h) < 0)) {
        
        draw->line_count = 0;
        return;
    }

    // image cropped?
    // TODO - do not draw image for now
    draw->line_count = 0;
}

void vga_img_draw_by(hcsVgaImage src, hcsVgaDraw draw) {
    
    // TODO - handle transparency
    
    vga_position_t pdst, psrc;
    vga_length_t line, offset;

    psrc = draw->src_start;
    pdst = draw->dst_start;
    line = draw->line_count;
    
    while (line--) {
        
        for (offset = 0; offset < draw->line_length; offset++)
            vga[pdst++] = ((char*)src->data)[psrc++];
            
        pdst += draw->dst_skip;
        psrc += draw->src_skip;
    }
}

static_assert(offsetof(sVgaImage, w) == 2);
static_assert(offsetof(sVgaImage, h) == 4);
static_assert(offsetof(sVgaImage, data) == 6);
#pragma argsused
// TODO - works just for images with height > 0 and height < 255
void vga_img_draw(hcsVgaImage src, vga_coord_t x, vga_coord_t y) { asm {
    
    push ES

    // destination address (320 * y + x) is put to ES:DI
    mov BX, 0A000h
    mov ES, BX
    mov AX, y
    shl AX, 6
    mov DI, AX
    shl AX, 2
    add DI, AX
    add DI, x

    // get some info from the image header
    mov SI, [src]               // SI is now address where "src" points to
    mov AX, [SI+2]              // image width
    mov BL, [SI+4]              // image height (just the lo byte)
    mov DX, 320
    sub DX, AX                  //  pixels between image width and 320

    // source data address
    mov SI, [SI+6]
    cld
    
    } _nextline: asm {

    mov CX, AX
    rep movsb
    add DI, DX
    dec BL
    jnz _nextline
    
    } _end: asm {
        
    pop ES
}}
*/
