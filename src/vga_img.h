#ifndef __VGA_IMG_H
#define __VGA_IMG_H

#include "ui_vga.h"
#include "utils.h"
#include <stdio.h>

// bitmap image flags
#define VGA_IMG_FLG_VALID           0x01
#define VGA_IMG_FLG_TRANSPARENT     0x02

// bitmap image
typedef struct sVgaImage {
    
    unsigned char       flags;              // VGA_IMG_FLG_*
    vga_color_index_t   transparency;       // index of transparency color for VGA_IMG_FLG_TRANSPARENT
    vga_length_t        w;                  // image width in pixels
    vga_length_t        h;                  // image height in pixels
    void                *data;              // image data
    
} sVgaImage, *hsVgaImage;
typedef const sVgaImage *hcsVgaImage;

// image file flags
#define VGA_IMG_HDR_TRANSPARENT     0x01

// image file header
typedef struct sVgaImgHeader {
    
    unsigned short      mark;
    unsigned short      w;
    unsigned short      h;
    unsigned char       flags;
    unsigned char       transparency;
    
} sVgaImgHeader, *hsVgaImgHeader;
typedef const sVgaImgHeader *hcsVgaImgHeader;

// functions
error_t vga_img_create_load(hsVgaImage img, FILE *fp);
void vga_img_destroy(hsVgaImage img);
void vga_img_draw(hsVgaImage img, vga_position_t pos);

#endif
