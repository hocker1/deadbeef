#include "ui_vga.h"
#include "ui_kbd.h"
#include "vga_img.h"
#include "utils.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>

#define MAP_W                       4
#define MAP_H                       4

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
    
    hsVgaImage                      image;
    char                            block;      // MAP_TILE_BLOCK_*
    
} sMapTile, *hsMapTile;

sVgaImage       tile_image_main;
sVgaImage       tile_image[8];                  // images to be used in map tiles
char            map_dirty[MAP_H][MAP_W];
sMapTile        map_tile[MAP_H][MAP_W] = {
    {
        { &tile_image[0], MAP_TILE_BLOCK_T | MAP_TILE_BLOCK_L },
        { &tile_image[0], MAP_TILE_BLOCK_T },
        { &tile_image[0], MAP_TILE_BLOCK_T },
        { &tile_image[0], MAP_TILE_BLOCK_T | MAP_TILE_BLOCK_R }
    },
    {
        { &tile_image[0], MAP_TILE_BLOCK_B | MAP_TILE_BLOCK_L },
        { &tile_image[0], 0 },
        { &tile_image[0], 0 },
        { &tile_image[0], MAP_TILE_BLOCK_R }
    },
    {
        { &tile_image[1], MAP_TILE_BLOCK_B | MAP_TILE_BLOCK_L },
        { &tile_image[0], MAP_TILE_BLOCK_B },
        { &tile_image[0], MAP_TILE_BLOCK_B },
        { &tile_image[0], MAP_TILE_BLOCK_B | MAP_TILE_BLOCK_R }
    },
    {
        { &tile_image[1], MAP_TILE_BLOCK_B | MAP_TILE_BLOCK_L },
        { &tile_image[1], MAP_TILE_BLOCK_B },
        { &tile_image[1], MAP_TILE_BLOCK_B },
        { &tile_image[1], MAP_TILE_BLOCK_B | MAP_TILE_BLOCK_R }
    }
};

sMapObject      player = { 
    { 
        SPACE_TILE2PHY(0), 
        SPACE_TILE2PHY(0),
        SPACE_TILE2PHY(1), 
        SPACE_TILE2PHY(1) 
    }, 
    0, 
    0, 
    &tile_image[3] 
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
            
        vga_img_draw(map_tile[y][x].image, VGA_XY_TO_POS(SPACE_TILE2SCREEN(x), SPACE_TILE2SCREEN(y)));
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

int main(void) {
    
    fp = fopen("test5.my", "rb");
    err = vga_img_alloc_load(&tile_image_main, fp);
    fclose(fp);
    
    if (err != ERR_OK) {
        
        printf("Error loading image: %d\n", (int)err);
        return -1;
    }

    for (i = 0, j = 0; i < tile_image_main.h; i += 16, j += 1)
        vga_img_create_from(&tile_image[j], &tile_image_main, i, 16);

    vga_open();
    ui_kbd_init();

    map_init();
    map_redraw();
    
    for (;;) {

        if (ui_kbd_pressed(UI_KBD_KEYCODE_CRSR_LEFT)) {
         
            if (player.pd.x > -16)
                player.pd.x -= 2;
        }
        else
        if (ui_kbd_pressed(UI_KBD_KEYCODE_CRSR_RIGHT)) {
            
            if (player.pd.x < 16)
                player.pd.x += 2;
        }
        else {
            
            player.pd.x /= 2;
        }
        
        if (ui_kbd_pressed(UI_KBD_KEYCODE_CRSR_UP)) {
            
            // TODO - tohle neni dobra podminka pro pudu pod nohama :-*
            if (collision & MAP_TILE_BLOCK_B)
                player.pd.y = -24;
        }
        
        if (ui_kbd_pressed(UI_KBD_KEYCODE_Q))
            break;
        
        vga_wait_retrace();
        
        if (player.pd.y < 16)
            player.pd.y += 1;

        collision = adjust_by_static_objects(&player);

        if (player.pd.y != 0 || player.pd.x != 0) {
            
            map_dirty_by_bbox(&player.pbox);
            player.pbox.y += player.pd.y;
            player.pbox.x += player.pd.x;
            map_dirty_by_bbox(&player.pbox);
        }
        
        map_redraw();
    }

    vga_img_destroy(&tile_image_main);

    ui_kbd_close();
    vga_close();
/*
    printf("--\n");
    adjust_by_static_objects(&player);
    player.pbox.y += player.pd.y;
    
    printf("--\n");
    adjust_by_static_objects(&player);
    player.pbox.y += player.pd.y;
*/
    return 0;
}
