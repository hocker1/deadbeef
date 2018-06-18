#include "vga_img.h"
#include <stdlib.h>

error_t vga_img_create_load(hsVgaImage img, FILE *fp) {
    
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

void vga_img_destroy(hsVgaImage img) {
    
    if (img->data != NULL) {
        
        free(img->data);
        img->data = NULL;
    }
}

void vga_img_draw(hsVgaImage img, vga_position_t pos) {
    
    if (img->data == NULL)
        return;
    
    if (img->flags & VGA_IMG_FLG_TRANSPARENT)
        vga_draw_tc(img->data, pos, img->w, img->h, img->transparency);
    else
        vga_draw_tn(img->data, pos, img->w, img->h);
}
