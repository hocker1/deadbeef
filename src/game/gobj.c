#include "gobj.h"
#include "anim.h"
#include "utils.h"

bool_t gobj_go_state(hsGameObject obj, int state) {
    
    hcsGameObjectState state_new;
    
    state_new = obj->type->state + state;
    if (obj->state == state_new)
        return FALSE;
    
    (void)anim_go_phase(&obj->anim_state, obj->type->animation, state_new->anim_phase);
    return TRUE;
}
