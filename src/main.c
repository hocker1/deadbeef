#include "ui_vga.h"
#include "ui_kbd.h"
#include "vga_img.h"
#include "utils.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>

#define MAP_W                       20
#define MAP_H                       10

#define TILE_SCREEN_W               16
#define TILE_SCREEN_H               16

#define for_each_map_tile(x, y)     for (y = 0; y < MAP_H; y++) for (x = 0; x < MAP_W; x++)

#define MAP_TILE_BLOCK_T            0x01        // tile may not be leaved by object in the direction upwards
#define MAP_TILE_BLOCK_R            0x02        // tile may not be leaved by object in the direction right
#define MAP_TILE_BLOCK_B            0x04        // tile may not be leaved by object in the direction downwards
#define MAP_TILE_BLOCK_L            0x08        // tile may not be leaved by object in the direction left

// box that represents space occupied by an object in physical space
typedef struct sPhyBox {

    physpace_pos_t                  x;
    physpace_pos_t                  y;
    physpace_pos_t                  w;
    physpace_pos_t                  h;

} sPhyBox, *hsPhyBox;

typedef struct sMapObject {

    sPhyBox                         pbox;       // object's position box
    sPhyVector                      pd;         // current speed
    hsVgaImage                      image;      // object image

} sMapObject, *hsMapObject;

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
    { {__,X___L}, {__,X____}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__}, {TT,X____}, {__,X___L}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,XT___}, {__,X____}, {__,X____}, {__,X____}, {__,X_R__} },
    { {__,X__BL}, {__,X__B_}, {_9,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,XT_B_}, {__,X__B_}, {Bo,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X__B_}, {__,X_RB_} },
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

sMapObject      player = {
    { 
        SPACE_TILE2PHY(0), 
        SPACE_TILE2PHY(0),
        SPACE_TILE2PHY(1), 
        SPACE_TILE2PHY(1) 
    }, 
    { 0, 0 },
    NULL
};

void map_init(void) {
    
    tilespace_pos_t x, y;
    
    for_each_map_tile(x, y)
        map_dirty[y][x] = 1;
}

void map_redraw(void) {
    
    tilespace_pos_t x, y;
    
    for_each_map_tile(x, y) {
        
        if (!map_dirty[y][x])
            continue;
            
        vga_img_draw(img_tiles_parts + map_tile[y][x].image, VGA_XY_TO_POS(SPACE_TILE2SCREEN(x), SPACE_TILE2SCREEN(y)));
        map_dirty[y][x] = 0;
    }
    
    vga_img_draw(player.image, VGA_XY_TO_POS(SPACE_PHY2SCREEN(player.pbox.x), SPACE_PHY2SCREEN(player.pbox.y)));
}

void map_dirty_by_bbox(hsPhyBox b) {
    
    map_dirty[SPACE_PHY2TILE(b->y)][SPACE_PHY2TILE(b->x)] = 1;
    map_dirty[SPACE_PHY2TILE(b->y + b->h - 1)][SPACE_PHY2TILE(b->x)] = 1;
    map_dirty[SPACE_PHY2TILE(b->y)][SPACE_PHY2TILE(b->x + b->w - 1)] = 1;
    map_dirty[SPACE_PHY2TILE(b->y + b->h - 1)][SPACE_PHY2TILE(b->x + b->w - 1)] = 1;
}

char adjust_by_static_objects(hsMapObject obj) {
    
    char collision = 0;
    physpace_pos_t pold, pnew;
    hsMapTile t1, t2;

    // handle vertical part of the movement
    if (obj->pd.y > 0) {
        
        pold = obj->pbox.y + obj->pbox.h - 1;
        pnew = SPACE_PHY2TILE(pold + obj->pd.y);

        if (SPACE_PHY2TILE(pold) != pnew) {
            
            t1 = &map_tile[SPACE_PHY2TILE(pold)][SPACE_PHY2TILE(obj->pbox.x)];
            t2 = &map_tile[SPACE_PHY2TILE(pold)][SPACE_PHY2TILE(obj->pbox.x + obj->pbox.w - 1)];
            
            if ((t1->block | t2->block) & MAP_TILE_BLOCK_B) {
                
                obj->pd.y = SPACE_TILE2PHY(pnew) - pold - 1;
                collision |= MAP_TILE_BLOCK_B;
            }
        }
    }
    else
    if (obj->pd.y < 0) {
        
        pold = SPACE_PHY2TILE(obj->pbox.y);
        pnew = obj->pbox.y + obj->pd.y;

        if (pold != SPACE_PHY2TILE(pnew)) {
            
            t1 = &map_tile[pold][SPACE_PHY2TILE(obj->pbox.x)];
            t2 = &map_tile[pold][SPACE_PHY2TILE(obj->pbox.x + obj->pbox.w - 1)];
            
            if ((t1->block | t2->block) & MAP_TILE_BLOCK_T) {
                
                obj->pd.y = SPACE_TILE2PHY(pold) - obj->pbox.y;
                collision |= MAP_TILE_BLOCK_T;
            }
        }
    }

    // handle horizontal part of the movement
    if (obj->pd.x > 0) {

        pold = obj->pbox.x + obj->pbox.w - 1;
        pnew = SPACE_PHY2TILE(pold + obj->pd.x);

        if (SPACE_PHY2TILE(pold) != pnew) {
            
            t1 = &map_tile[SPACE_PHY2TILE(obj->pbox.y)][SPACE_PHY2TILE(pold)];
            t2 = &map_tile[SPACE_PHY2TILE(obj->pbox.y + obj->pbox.h - 1)][SPACE_PHY2TILE(pold)];
            
            if ((t1->block | t2->block) & MAP_TILE_BLOCK_R) {
                
                obj->pd.x = SPACE_TILE2PHY(pnew) - pold - 1;
                collision |= MAP_TILE_BLOCK_R;
            }
        }
    }
    else
    if (obj->pd.x < 0) {
        
        pold = SPACE_PHY2TILE(obj->pbox.x);
        pnew = obj->pbox.x + obj->pd.x;

        if (pold != SPACE_PHY2TILE(pnew)) {
            
            t1 = &map_tile[SPACE_PHY2TILE(obj->pbox.y)][pold];
            t2 = &map_tile[SPACE_PHY2TILE(obj->pbox.y + obj->pbox.h - 1)][pold];
            
            if ((t1->block | t2->block) & MAP_TILE_BLOCK_L) {
                
                obj->pd.x = SPACE_TILE2PHY(pold) - obj->pbox.x;
                collision |= MAP_TILE_BLOCK_L;
            }
        }
    }
    
    return collision;
}

unsigned int i, j, x, y;
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

            player.pd.x -= (collision & MAP_TILE_BLOCK_B) ? PLAYER_HSPD_WALK_ACC : PLAYER_HSPD_JUMP_ACC;
            if (player.pd.x < -PLAYER_HSPD_WALK_MAX)
                player.pd.x = -PLAYER_HSPD_WALK_MAX;
        }
        else
        if (ui_kbd_pressed(UI_KBD_KEYCODE_CRSR_RIGHT) && !ui_kbd_pressed(UI_KBD_KEYCODE_CRSR_LEFT)) {

            player.pd.x += (collision & MAP_TILE_BLOCK_B) ? PLAYER_HSPD_WALK_ACC : PLAYER_HSPD_JUMP_ACC;
            if (player.pd.x > PLAYER_HSPD_WALK_MAX)
                player.pd.x = PLAYER_HSPD_WALK_MAX;
        }
        else {

            player.pd.x /= 2;
        }

        // actions on single keypresses
        while ((keycode = ui_kbd_queue_pop()) != UI_KBD_KEYCODE_NONE) {
            
            // jumping - only with ground under feet
            if (keycode == UI_KBD_PRESS(CRSR_UP)) {

                if (collision & MAP_TILE_BLOCK_B)
                    player.pd.y = -PLAYER_VSPD_JUMP_ACC;
            }
        }

        if (ui_kbd_pressed(UI_KBD_KEYCODE_Q))
            break;
        
        vga_wait_retrace();
        
        if (player.pd.y < PLAYER_VSPD_FALL_MAX)
            player.pd.y += PLAYER_VSPD_FALL_ACC;

        collision = adjust_by_static_objects(&player);

        if (player.pd.y != 0 || player.pd.x != 0) {
            
            map_dirty_by_bbox(&player.pbox);
            player.pbox.y += player.pd.y;
            player.pbox.x += player.pd.x;
            map_dirty_by_bbox(&player.pbox);
        }

        player.image = (collision & MAP_TILE_BLOCK_B) ? img_deadbeef_parts : img_deadbeef_parts + 1;
        
        map_redraw();
    }

    free(img_deadbeef_parts);
    vga_img_destroy(&img_tiles);
    vga_img_destroy(&img_deadbeef);

    ui_kbd_close();
    vga_close();
    return 0;
}
