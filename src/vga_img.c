#include "vga_img.h"
#include <stdlib.h>

error_t vga_img_alloc_load(hsVgaImage img, FILE *fp) {
    
    sVgaImgHeader hdr;
    
    if (fread(&hdr, sizeof(hdr), 1, fp) != 1)
        return ERR_FAILURE;

    if (hdr.w > 320 || hdr.h > 200)
        return ERR_OUT_OF_BOUNDS;
    
    img->w = hdr.w;
    img->h = hdr.h;
    img->flags = 0;
    img->transparency = hdr.transparency;
    
    if (hdr.flags & VGA_IMG_HDR_TRANSPARENT)
        img->flags |= VGA_IMG_FLG_TRANSPARENT;
            
    if ((img->data = malloc(hdr.w * hdr.h)) == NULL)
        return ERR_OUT_OF_MEMORY;
    
    if (fread(img->data, hdr.w * hdr.h, 1, fp) != 1) {
        
        free(img->data);
        return ERR_FAILURE;
    }
    
    return ERR_OK;
}

void vga_img_free(hsVgaImage img) {
    
    if (img->data != NULL) {
        
        free(img->data);
        img->data = NULL;
    }
}

error_t vga_img_create_raw(hsVgaImage img, void *data, vga_length_t w, vga_length_t h) {
    
    img->flags = 0;
    img->transparency = 0;
    img->w = w;
    img->h = h;
    img->data = data;
    return ERR_OK;
}

error_t vga_img_create_from(hsVgaImage img, hsVgaImage src, vga_length_t offset, vga_length_t h) {
    
    img->flags = src->flags;
    img->transparency = src->transparency;
    img->w = src->w;
    img->h = h;
    img->data = (char*)src->data + offset * src->w;
    return ERR_OK;
}

void vga_img_destroy(hsVgaImage img) {
    
    img->data = NULL;
}

void vga_img_set_transparent(hsVgaImage img, vga_color_index_t color) {

    img->flags |= VGA_IMG_FLG_TRANSPARENT;
    img->transparency = color;
}

void vga_img_draw(hsVgaImage img, vga_position_t pos) {
    
    if (img->data == NULL)
        return;
    
    if (img->flags & VGA_IMG_FLG_TRANSPARENT)
        vga_draw_tc(img->data, pos, img->w, img->h, img->transparency);
    else
        vga_draw_tn(img->data, pos, img->w, img->h);
}
