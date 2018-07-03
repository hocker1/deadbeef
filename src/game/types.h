#ifndef __TYPES_H
#define __TYPES_H

#include "utils.h"

typedef char tilespace_pos_t;
typedef char tilespace_vec_t;

typedef int screenspace_pos_t;
typedef int screenspace_vec_t;

typedef int physpace_pos_t;
typedef int physpace_vec_t;

#define SPACE_PHY2SCREEN(x)         ((x)>>4)
#define SPACE_SCREEN2PHY(x)         ((x)<<4)
#define SPACE_SCREEN2TILE(x)        ((x)>>4)
#define SPACE_TILE2SCREEN(x)        ((x)<<4)
#define SPACE_PHY2TILE(x)           SPACE_SCREEN2TILE(SPACE_PHY2SCREEN(x))
#define SPACE_TILE2PHY(x)           SPACE_SCREEN2PHY(SPACE_TILE2SCREEN(x))

typedef struct sPhyPosition {
    
    physpace_pos_t          x;
    physpace_pos_t          y;
    
} sPhyPosition, *hsPhyPosition;

typedef struct sPhyRange {
    
    physpace_pos_t          p1;
    physpace_pos_t          p2;
    
} sPhyRange, *hsPhyRange;

typedef struct sPhyVector {
    
    physpace_vec_t          x;
    physpace_vec_t          y;
    
} sPhyVector, *hsPhyVector;

#endif
