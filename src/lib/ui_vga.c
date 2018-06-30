#include "ui_vga.h"
#include "utils.h"

#if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
#define PTRS_DATA_FAR
#endif

static unsigned char last_mode;

void vga_open(void) { asm {

    // get current video mode
    mov AH, 0fh
    int 10h
    mov last_mode, AL

    // turn on mode 13 (320x200x256)
    mov AX, 0013h
    int 10h
}}

void vga_close(void) { asm {

    // restore original graphics mode
    mov AH, 0
    mov AL, last_mode    
    int 10h
}}

// TODO - asm
void vga_wait_retrace(void) {

    while((inp(0x03da) & 0x08))
        ;
    while(!(inp(0x03da) & 0x08))
        ;
}

void vga_pal_write(vga_color_index_t color, char r, char g, char b) { asm {
    
    mov DX, 03c8h
    mov AL, color
    out DX, AL

    mov DX, 03c9h
    mov AL, r
    out DX, AL
    mov AL, g
    out DX, AL
    mov AL, b
    out DX, AL
}}

// TODO - works just for images with height > 0 and height < 255
void vga_draw_tn(const void *data, vga_position_t pos, vga_length_t w, vga_length_t h) { asm {

    push ES
#ifdef PTRS_DATA_FAR
    push DS
#endif
    
    // destination address (320 * y + x) is put to ES:DI
    mov BX, 0A000h
    mov ES, BX
    mov DI, pos

    // source data address
#ifdef PTRS_DATA_FAR
    lds SI, data
#else
    mov SI, data
#endif

    // screen pixels to be skipped after every line
    mov DX, 320
    sub DX, w

    // reset counters
    cld
    mov BX, h
    
    } _nextline: asm {

    // draw one line
    mov CX, w
    rep movsb
    add DI, DX
    dec BL
    jnz _nextline
    
    } _end: asm {
        
#ifdef PTRS_DATA_FAR
    pop DS
#endif
    pop ES
}}

// TODO - works just for images with height > 0 and height < 255
void vga_draw_tc(const void *data, vga_position_t pos, vga_length_t w, vga_length_t h, vga_color_index_t color) { asm {

    push ES
#ifdef PTRS_DATA_FAR
    push DS
#endif

    // destination address (320 * y + x) is put to ES:DI
    mov BX, 0A000h
    mov ES, BX
    mov DI, pos

    // source data address
#ifdef PTRS_DATA_FAR
    lds SI, data
#else
    mov SI, data
#endif

    // screen pixels to be skipped after every line
    mov DX, 320
    sub DX, w

    // reset counters
    cld
    mov BX, h
    
    } 
    _nextline: asm {

    // draw one line
    mov CX, w

    } 
    _nextpix: asm {
    
    lodsb                   // load image pixel and increment source pointer
    cmp AL, color           // compare it to transparency color
    jz _skip                // if not transparent ...
    mov [ES:DI], AL         // ... put the pixel to screen
    
    }
    _skip: asm {
    
    inc DI
    loopnz _nextpix

    add DI, DX
    dec BL
    jnz _nextline
    
#ifdef PTRS_DATA_FAR
    pop DS
#endif
    pop ES
}}
