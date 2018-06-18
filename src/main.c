#include "ui_vga.h"
#include "ui_kbd.h"
#include "vga_img.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

keycode_t keycode;
sVgaImage img;
error_t err;

FILE *fp;

int main(void) {

    fp = fopen("test5.my", "rb");
    err = vga_img_create_load(&img, fp);
    fclose(fp);
    
    if (err != ERR_OK) {
        
        printf("Error loading image: %d\n", (int)err);
        return -1;
    }
 
    vga_open();
    ui_kbd_init();

    vga_img_draw(&img, VGA_XY_TO_POS(0, 32));
    vga_img_draw(&img, VGA_XY_TO_POS(16, 32));
    vga_img_draw(&img, VGA_XY_TO_POS(32, 32));
    vga_img_draw(&img, VGA_XY_TO_POS(32, 48));
    vga_img_draw(&img, VGA_XY_TO_POS(48, 48));

    vga_img_destroy(&img);
    
    while ((keycode = ui_kbd_queue_wait()) == UI_KBD_KEYCODE_NONE)
        ;

    ui_kbd_close();
    vga_close();

    return 0;
}
