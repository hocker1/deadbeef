#include "config.h"
#include "ui_kbd.h"
#include <dos.h>

#if UI_KBD_EVENT_QUEUE == 0

    #define QUEUE_ADD(c)        { }
    #define QUEUE_REMOVE()      { }
    #define QUEUE_CLEAR()       { }
    #define QUEUE_PEEK()        UI_KBD_KEYCODE_NONE

#elif UI_KBD_EVENT_QUEUE == 1

    static keycode_t queue;
    
    #define QUEUE_ADD(c)        { if (queue == UI_KBD_KEYCODE_NONE) { queue = (c); } }
    #define QUEUE_REMOVE()      { queue = UI_KBD_KEYCODE_NONE; }
    #define QUEUE_CLEAR()       { queue = UI_KBD_KEYCODE_NONE; }
    #define QUEUE_PEEK()        queue

#elif UI_KBD_EVENT_QUEUE > 1

    #error Full UI_KBD_EVENT_QUEUE not yet implemented.

#else
    
    #error Specify UI_KBD_EVENT_QUEUE length in config.h

#endif

#define CODE_TO_INDEX(c)        (((c) >> 3) & 15)
#define CODE_TO_MASK(c)         (1 << ((c) & 7))

static keymap_t pressed;

static void interrupt (*irq_kbd_old)();

static void interrupt irq_kbd() {

    keycode_t code;
    unsigned char index;
    unsigned int mask;

    // get the keycode
    code = inportb(0x60);
    
    // ack the interrupt
    asm {
        in  AL, 61h   
        or  AL, 82h
        out 61h, AL   
        and AL, 7fh   
        out 61h, AL   
        mov AL, 20h   
        out 20h, AL   
    }

    // coordinates to map of pressed keys
    index = CODE_TO_INDEX(code);
    mask = CODE_TO_MASK(code);
    
    if (code & 0x80) {

        // key was released, check if it was pressed
        if (pressed[index] & mask) {

            QUEUE_ADD(code);
            pressed[index] &= ~mask;    
        }
    }
    else {

        // key was pressed, check if it was released
        if (!(pressed[index] & mask)) {

            QUEUE_ADD(code);
            pressed[index] |= mask;
        }
    }
}

void ui_kbd_init() {

    QUEUE_CLEAR();
    irq_kbd_old = getvect(0x09);
    setvect(0x09, irq_kbd);
}

void ui_kbd_close() {

    setvect(0x09, irq_kbd_old);
}

char ui_kbd_pressed(keycode_t key) {

    return pressed[CODE_TO_INDEX(key)] & CODE_TO_MASK(key);
}

keycode_t ui_kbd_queue_pop() {

    keycode_t result = QUEUE_PEEK();
    QUEUE_REMOVE();
    return result;
}

keycode_t ui_kbd_queue_wait() {

    keycode_t result = ui_kbd_queue_pop();

    if (result != UI_KBD_KEYCODE_NONE)
        return result;
        
    asm { hlt }
    return ui_kbd_queue_pop();
}
