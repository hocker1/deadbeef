#include "ui_vga.h"
#include "ui_kbd.h"
#include <stdio.h>
#include <alloc.h>

keycode_t keycode;

FILE *fp;

struct sMyHeader {
    
    unsigned short mark;
    unsigned short width;
    unsigned short height;
    unsigned short reserved;
    
} hdr;

char *data;
size_t size;

int main(void) {

    fp = fopen("test.my", "rb");
    fread(&hdr, sizeof(hdr), 1, fp);
  
    size = hdr.width * hdr.height;
    if (!(data = (char*)malloc(size))) {
        
        fclose(fp);
        printf("Cannot allocate.\n");
        return -1;
    }

    fread(data, size, 1, fp);
    fclose(fp);
    
    vga_open();
    ui_kbd_init();

    vga_draw_tn(data, VGA_XY_TO_POS(0, 0), hdr.width, hdr.height);
    free(data);
    
    while ((keycode = ui_kbd_queue_wait()) == UI_KBD_KEYCODE_NONE)
        ;

    ui_kbd_close();
    vga_close();

    return 0;
}
