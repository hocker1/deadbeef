#include "ui_vga.h"
#include "ui_kbd.h"
#include "vga_img.h"
#include "utils.h"
#include "types.h"
#include "anim.h"
#include "gobj.h"
#include <stdio.h>
#include <stdlib.h>

#define MAP_W                       20
#define MAP_H                       10

#define for_each_map_tile(x, y)     for (y = 0; y < MAP_H; y++) for (x = 0; x < MAP_W; x++)

#define MAP_TILE_BLOCK_T            0x01        // tile may not be leaved by object in the direction upwards
#define MAP_TILE_BLOCK_R            0x02        // tile may not be leaved by object in the direction right
#define MAP_TILE_BLOCK_B            0x04        // tile may not be leaved by object in the direction downwards
#define MAP_TILE_BLOCK_L            0x08        // tile may not be leaved by object in the direction left

typedef struct sMapTile {
    
    unsigned char                   image;      // index to tile images
    char                            block;      // MAP_TILE_BLOCK_*
    
} sMapTile, *hsMapTile;

sVgaImage       img_deadbeef;
hsVgaImage      img_deadbeef_parts;

sVgaImage       img_tiles;
hsVgaImage      img_tiles_parts;

char            map_dirty[MAP_H][MAP_W];

#define X____   0
#define XT___   (MAP_TILE_BLOCK_T)
#define X_R__   (MAP_TILE_BLOCK_R)
#define X__B_   (MAP_TILE_BLOCK_B)
#define X___L   (MAP_TILE_BLOCK_L)
#define XTR__   (MAP_TILE_BLOCK_T | MAP_TILE_BLOCK_R)
#define XT_B_   (MAP_TILE_BLOCK_T | MAP_TILE_BLOCK_B)
#define XT__L   (MAP_TILE_BLOCK_T | MAP_TILE_BLOCK_L)
#define X_RB_   (MAP_TILE_BLOCK_R | MAP_TILE_BLOCK_B)
#define X_R_L   (MAP_TILE_BLOCK_R | MAP_TILE_BLOCK_L)
#define X__BL   (MAP_TILE_BLOCK_B | MAP_TILE_BLOCK_L)
#define XTRB_   (MAP_TILE_BLOCK_T | MAP_TILE_BLOCK_R | MAP_TILE_BLOCK_B)
#define XTR_L   (MAP_TILE_BLOCK_T | MAP_TILE_BLOCK_R | MAP_TILE_BLOCK_L)
#define XT_BL   (MAP_TILE_BLOCK_T | MAP_TILE_BLOCK_B | MAP_TILE_BLOCK_L)
#define X_RBL   (MAP_TILE_BLOCK_R | MAP_TILE_BLOCK_B | MAP_TILE_BLOCK_L)

#define __      (0)
#define TT      (1)
#define _9      (2)
#define Bo      (3)
#define oo      (4)

sMapTile        map_tile[MAP_H][MAP_W] = {
    { {__,XT__L}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XTR__} },
    { {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X___L}, {__,X____}, {__,X__B_}, {__,X____}, {__,X____}, {_9,X__B_}, {__,X__B_}, {__,X__B_}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X___L}, {__,X_R__}, {TT,X____}, {__,X___L}, {__,X_R__}, {TT,X____}, {TT,X____}, {TT,X____}, {__,X___L}, {Bo,X__B_}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X___L}, {__,X____}, {__,XT___}, {__,X____}, {__,X____}, {__,XT___}, {__,XT___}, {__,XT___}, {__,X_R__}, {TT,X____}, {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X__B_}, {__,XT_B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {_9,X__B_}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X__B_}, {__,X_R__}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {oo,X____}, {TT,X____}, {TT,X____}, {__,X___L}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__}, {TT,X____}, {__,X___L}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,X____}, {__,X____}, {__,X____}, {__,XT___}, {__,X____}, {__,X____}, {__,X____}, {__,X_RB_} },
    { {__,X__BL}, {__,X__B_}, {_9,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,XT_B_}, {__,X__B_}, {Bo,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X_RB_}, {TT,X_RB_} },
    { {TT,X___L}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X_R__} },
/*
    { {__,XT__L}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XTR__} },
    { {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X__BL}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X_RB_} },
    { {TT,X___L}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X____}, {TT,X_R__} },
*/ 
};

const sAnimationPhase player_anim[] = {
    { 0,    300,    1 },            // stand calm
    { 2,    10,     1 },            // stand calm, wink
    { 0,    180,    1 },            // stand calm
    { 2,    10,     1 },            // stand calm, wink
    { 0,    10,     1 },            // stand calm
    { 2,    10,     -5 },           // stand calm, wink
    { 1,    1,      0 }             // jump / fall
};

const sGameObjectState player_states[] = {
    { 0 },                          // stand
    { 0 },                          // go
    { 6 }                           // jump / fall
};
        
sGameObjectType ot_player = {
    { SPACE_TILE2PHY(1) - SPACE_SCREEN2PHY(8), SPACE_TILE2PHY(1) }, 
    { -SPACE_SCREEN2PHY(4), 0 },
    player_states,
    player_anim,
    NULL
};

sGameObject player = {
    &ot_player,
    { SPACE_TILE2PHY(1), SPACE_TILE2PHY(0) },
    { 0, 0 },
    { 0, 1 }
};

void map_init(void) {
    
    tilespace_pos_t x, y;
    
    for_each_map_tile(x, y)
        map_dirty[y][x] = 1;
}

void map_redraw(void) {
    
    tilespace_pos_t x, y;
    hcsGameObjectType ot;
    
    for_each_map_tile(x, y) {
        
        if (!map_dirty[y][x])
            continue;
            
        vga_img_draw(img_tiles_parts + map_tile[y][x].image, VGA_XY_TO_POS(SPACE_TILE2SCREEN(x), SPACE_TILE2SCREEN(y)));
        map_dirty[y][x] = 0;
    }
    
    ot = player.type;
    vga_img_draw(ot->image + (ot->animation + player.anim_state.phase)->value, VGA_XY_TO_POS(SPACE_PHY2SCREEN(player.position.x + ot->pivot.x), SPACE_PHY2SCREEN(player.position.y + ot->pivot.y)));
}

void map_dirty_by_object(hsGameObject obj) {

    hcsGameObjectType ot;
    hcsVgaImage img;
    physpace_pos_t x, y, w, h;
    
    ot = obj->type;
    img = ot->image + (ot->animation + obj->anim_state.phase)->value;
    
    x = obj->position.x + ot->pivot.x;
    y = obj->position.y + ot->pivot.y;
    w = SPACE_SCREEN2PHY(img->w) - 1;
    h = SPACE_SCREEN2PHY(img->h) - 1;
    
    map_dirty[SPACE_PHY2TILE(y)][SPACE_PHY2TILE(x)] = 1;
    map_dirty[SPACE_PHY2TILE(y)][SPACE_PHY2TILE(x + w)] = 1;
    map_dirty[SPACE_PHY2TILE(y + h)][SPACE_PHY2TILE(x)] = 1;
    map_dirty[SPACE_PHY2TILE(y + h)][SPACE_PHY2TILE(x + w)] = 1;
}

char adjust_by_static_objects(hsGameObject obj) {
    
    char collision = 0;
    physpace_pos_t pold, pnew;
    hsMapTile t1, t2;
    hcsGameObjectType ot;

    ot = obj->type;
    
    // handle vertical part of the movement
    if (obj->speed.y > 0) {
        
        pold = obj->position.y + ot->size.y - 1;
        pnew = SPACE_PHY2TILE(pold + obj->speed.y);

        if (SPACE_PHY2TILE(pold) != pnew) {
            
            t1 = &map_tile[SPACE_PHY2TILE(pold)][SPACE_PHY2TILE(obj->position.x)];
            t2 = &map_tile[SPACE_PHY2TILE(pold)][SPACE_PHY2TILE(obj->position.x + ot->size.x - 1)];
            
            if ((t1->block | t2->block) & MAP_TILE_BLOCK_B) {
                
                obj->speed.y = SPACE_TILE2PHY(pnew) - pold - 1;
                collision |= MAP_TILE_BLOCK_B;
            }
        }
    }
    else
    if (obj->speed.y < 0) {
        
        pold = SPACE_PHY2TILE(obj->position.y);
        pnew = obj->position.y + obj->speed.y;

        if (pold != SPACE_PHY2TILE(pnew)) {
            
            t1 = &map_tile[pold][SPACE_PHY2TILE(obj->position.x)];
            t2 = &map_tile[pold][SPACE_PHY2TILE(obj->position.x + ot->size.x - 1)];
            
            if ((t1->block | t2->block) & MAP_TILE_BLOCK_T) {
                
                obj->speed.y = SPACE_TILE2PHY(pold) - obj->position.y;
                collision |= MAP_TILE_BLOCK_T;
            }
        }
    }

    // handle horizontal part of the movement
    if (obj->speed.x > 0) {

        pold = obj->position.x + ot->size.x - 1;
        pnew = SPACE_PHY2TILE(pold + obj->speed.x);

        if (SPACE_PHY2TILE(pold) != pnew) {
            
            t1 = &map_tile[SPACE_PHY2TILE(obj->position.y)][SPACE_PHY2TILE(pold)];
            t2 = &map_tile[SPACE_PHY2TILE(obj->position.y + ot->size.y - 1)][SPACE_PHY2TILE(pold)];
            
            if ((t1->block | t2->block) & MAP_TILE_BLOCK_R) {
                
                obj->speed.x = SPACE_TILE2PHY(pnew) - pold - 1;
                collision |= MAP_TILE_BLOCK_R;
            }
        }
    }
    else
    if (obj->speed.x < 0) {
        
        pold = SPACE_PHY2TILE(obj->position.x);
        pnew = obj->position.x + obj->speed.x;

        if (pold != SPACE_PHY2TILE(pnew)) {
            
            t1 = &map_tile[SPACE_PHY2TILE(obj->position.y)][pold];
            t2 = &map_tile[SPACE_PHY2TILE(obj->position.y + ot->size.y - 1)][pold];
            
            if ((t1->block | t2->block) & MAP_TILE_BLOCK_L) {
                
                obj->speed.x = SPACE_TILE2PHY(pold) - obj->position.x;
                collision |= MAP_TILE_BLOCK_L;
            }
        }
    }
    
    return collision;
}

unsigned int i, j;
FILE *fp;
error_t err;
keycode_t keycode;
char collision;

// movement speed constants
#define PLAYER_HSPD_WALK_MAX            16
#define PLAYER_HSPD_WALK_ACC            2
#define PLAYER_HSPD_JUMP_ACC            1
#define PLAYER_VSPD_FALL_MAX            24
#define PLAYER_VSPD_FALL_ACC            1
#define PLAYER_VSPD_JUMP_ACC            33

int main(void) {

    fp = fopen("tiles.my", "rb");
    err = vga_img_alloc_load(&img_tiles, fp);
    fclose(fp);
    
    if (err != ERR_OK) {
        
        printf("Error loading image (tiles): %d\n", (int)err);
        return -1;
    }

    fp = fopen("deadbeef.my", "rb");
    err = vga_img_alloc_load(&img_deadbeef, fp);
    fclose(fp);
    
    if (err != ERR_OK) {
        
        printf("Error loading image (deadbeef): %d\n", (int)err);
        return -1;
    }

    if (!(img_deadbeef_parts = (hsVgaImage)malloc(sizeof(sVgaImage) * (img_deadbeef.h / 16)))) {

        printf("Error allocating image frames.\n");
        return -1;
    }
    
    vga_img_set_transparent(&img_deadbeef, 5);
    for (i = 0; i < (img_deadbeef.h / 16); i++)
        vga_img_create_from(img_deadbeef_parts + i, &img_deadbeef, 16 * i, 16);

    ot_player.image = img_deadbeef_parts;
    
    if (!(img_tiles_parts = (hsVgaImage)malloc(sizeof(sVgaImage) * (img_tiles.h / 16)))) {

        printf("Error allocating image frames.\n");
        return -1;
    }
    
    for (i = 0; i < (img_tiles.h / 16); i++)
        vga_img_create_from(img_tiles_parts + i, &img_tiles, 16 * i, 16);

    vga_open();
    ui_kbd_init();

    map_init();

    for (;;) {

        // walking left and right
        if (ui_kbd_pressed(UI_KBD_KEYCODE_CRSR_LEFT) && !ui_kbd_pressed(UI_KBD_KEYCODE_CRSR_RIGHT)) {

            player.speed.x -= (collision & MAP_TILE_BLOCK_B) ? PLAYER_HSPD_WALK_ACC : PLAYER_HSPD_JUMP_ACC;
            if (player.speed.x < -PLAYER_HSPD_WALK_MAX)
                player.speed.x = -PLAYER_HSPD_WALK_MAX;
        }
        else
        if (ui_kbd_pressed(UI_KBD_KEYCODE_CRSR_RIGHT) && !ui_kbd_pressed(UI_KBD_KEYCODE_CRSR_LEFT)) {

            player.speed.x += (collision & MAP_TILE_BLOCK_B) ? PLAYER_HSPD_WALK_ACC : PLAYER_HSPD_JUMP_ACC;
            if (player.speed.x > PLAYER_HSPD_WALK_MAX)
                player.speed.x = PLAYER_HSPD_WALK_MAX;
        }
        else {

            player.speed.x /= 2;
        }

        // actions on single keypresses
        while ((keycode = ui_kbd_queue_pop()) != UI_KBD_KEYCODE_NONE) {
            
            // jumping - only with ground under feet
            if (keycode == UI_KBD_PRESS(CRSR_UP)) {

                if (collision & MAP_TILE_BLOCK_B)
                    player.speed.y = -PLAYER_VSPD_JUMP_ACC;
            }
        }

        if (ui_kbd_pressed(UI_KBD_KEYCODE_Q))
            break;
        
        if (player.speed.y < PLAYER_VSPD_FALL_MAX)
            player.speed.y += PLAYER_VSPD_FALL_ACC;

        collision = adjust_by_static_objects(&player);

        if (player.speed.y != 0 || player.speed.x != 0) {
            
            map_dirty_by_object(&player);
            player.position.y += player.speed.y;
            player.position.x += player.speed.x;
            map_dirty_by_object(&player);
        }
        
        // determine and update player state
        if (collision & MAP_TILE_BLOCK_B) {

            // standing on ground, maybe walking
            gobj_go_state(&player, player.speed.x ? 1 : 0);
        }
        else {

            // flying up or down
            gobj_go_state(&player, 2);
        }

        // perform player animation tick
        if (anim_tick(&player.anim_state, player.type->animation) == TRUE)
            map_dirty_by_object(&player);
        
        // do the drawing...
        vga_wait_retrace();
        map_redraw();
    }

    free(img_deadbeef_parts);
    vga_img_destroy(&img_tiles);
    vga_img_destroy(&img_deadbeef);

    ui_kbd_close();
    vga_close();

    return 0;
}
