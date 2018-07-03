#ifndef __ANIM_H
#define __ANIM_H

#include "utils.h"

typedef signed int animation_index_t;
typedef unsigned int animation_duration_t;

typedef struct sAnimationPhase {
    
    int                     value;              // animated value
    animation_duration_t    duration;           // duration of this phase in seconds
    animation_index_t       next;               // relative jump to another phase
    
} sAnimationPhase, *hsAnimationPhase;

typedef struct sAnimationState {
    
    animation_index_t       phase;              // current phase's index
    animation_duration_t    ticks;              // remaining ticks to next phase
    
} sAnimationState, *hsAnimationState;

bool_t anim_tick(hsAnimationState s, hsAnimationPhase first);
bool_t anim_go_phase(hsAnimationState s, hsAnimationPhase first, animation_index_t index);

#endif
