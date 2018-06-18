#ifndef __UI_VGA_H
#define __UI_VGA_H

#include "utils.h"

// integer types
typedef unsigned short vga_length_t;                // length along single axis
typedef signed short vga_coord_t;                   // coordinate or offset along single axis
typedef unsigned short vga_position_t;              // offset from the start of image data
typedef unsigned char vga_color_index_t;            // index to color palette

// general macros
#define VGA_SCREEN_W                320
#define VGA_SCREEN_H                200
#define VGA_XY_TO_POS(x, y)         (((y) << 8) + ((y) << 6) + (x))
#define VGA_YX_TO_POS(y, x)         (((y) << 8) + ((y) << 6) + (x))

// functions
void vga_open(void);
void vga_close(void);
void vga_pal_write(vga_color_index_t color, char r, char g, char b);
void vga_draw_tn(const void *data, vga_position_t pos, vga_length_t w, vga_length_t h);
void vga_draw_tc(const void *data, vga_position_t pos, vga_length_t w, vga_length_t h, vga_color_index_t color);

#endif
