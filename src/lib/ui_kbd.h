#ifndef __UI_KBD_H
#define __UI_KBD_H

#define UI_KBD_KEYCODE_NONE             0
#define UI_KBD_KEYCODE_Q                16
#define UI_KBD_KEYCODE_W                17
#define UI_KBD_KEYCODE_E                18
#define UI_KBD_KEYCODE_R                19
#define UI_KBD_KEYCODE_T                20
#define UI_KBD_KEYCODE_P                25
#define UI_KBD_KEYCODE_ENTER            28
#define UI_KBD_KEYCODE_A                30
#define UI_KBD_KEYCODE_D                32
#define UI_KBD_KEYCODE_Z                44
#define UI_KBD_KEYCODE_X                45
#define UI_KBD_KEYCODE_C                46
#define UI_KBD_KEYCODE_V                47
#define UI_KBD_KEYCODE_M                50
#define UI_KBD_KEYCODE_CRSR_UP          72
#define UI_KBD_KEYCODE_CRSR_LEFT        75
#define UI_KBD_KEYCODE_CRSR_RIGHT       77
#define UI_KBD_KEYCODE_CRSR_DOWN        80

#define UI_KBD_RELEASE(x)               ((UI_KBD_KEYCODE_ ## x) | 0x80)
#define UI_KBD_PRESS(x)                 (UI_KBD_KEYCODE_ ## x)

typedef unsigned char   keycode_t;
typedef unsigned char   keymap_t[16];

void ui_kbd_init(void);
void ui_kbd_close(void);
char ui_kbd_pressed(keycode_t key);
keycode_t ui_kbd_queue_pop(void);
keycode_t ui_kbd_queue_wait(void);

#endif
