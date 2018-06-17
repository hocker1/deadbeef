#ifndef __VGA_IMG_H
#define __VGA_IMG_H

#include "ui_vga.h"

// pre-definitions of used structures
typedef struct sVgaImage sVgaImage;
typedef sVgaImage *hsVgaImage;
typedef const sVgaImage *hcsVgaImage;

typedef struct sVgaDraw sVgaDraw;
typedef sVgaDraw *hsVgaDraw;
typedef const sVgaDraw *hcsVgaDraw;

// structure for single image
#define VGA_IMG_FLG_TRANSPARENT     0x01

struct sVgaImage {
    
    unsigned char                   flags;          // VGA_IMG_FLG_*
    vga_color_index_t                 color_trans;    // color for transparency
    
    vga_length_t                    w;              // width
    vga_length_t                    h;              // height
    void                            *data;          // image data
};

// structure describing how to draw (and crop) given image on certain position to another given image
struct sVgaDraw {
    
    vga_position_t                  src_start;      // starting offset in source image data
    vga_position_t                  dst_start;      // starting offset in target image data
    vga_length_t                    line_length;    // how many pixels are copied on every line
    vga_length_t                    src_skip;       // source image's data bytes to skip after single line data copied
    vga_length_t                    dst_skip;       // target image's data bytes to skip after single line data copied
    vga_length_t                    line_count;     // how many lines to be drawn at all
};

// functions
void vga_img_create(hsVgaImage img, vga_length_t w, vga_length_t h, const void *data);
void vga_img_make_transparent(hsVgaImage img, vga_color_index_t color);

//void vga_create_draw_screen(hsVgaDraw draw, hcsVgaImage src, vga_coord_t x, vga_coord_t y);
//void vga_img_draw(hcsVgaImage src, vga_coord_t x, vga_coord_t y);
//void vga_img_draw_by(hcsVgaImage src, hcsVgaDraw draw);

#endif
