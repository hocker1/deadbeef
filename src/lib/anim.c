#include "anim.h"
#include "utils.h"

bool_t anim_tick(hsAnimationState s, hcsAnimationPhase first) {
    
    s->ticks--;
    if (s->ticks > 0)
        return FALSE;
    
    s->phase += first[s->phase].next;
    s->ticks = first[s->phase].duration;
    return TRUE;
}

bool_t anim_go_phase(hsAnimationState s, hcsAnimationPhase first, animation_index_t index) {
    
    if (index == s->phase)
        return FALSE;
    
    s->phase = index;
    s->ticks = first[s->phase].duration;
    return TRUE;
}
