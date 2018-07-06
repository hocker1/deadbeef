#ifndef __GOBJ_H
#define __GOBJ_H

#include "utils.h"
#include "types.h"
#include "anim.h"
#include "vga_img.h"

// game object instance's state (standing, walking, jumping...)
typedef struct sGameObjectState {

    animation_index_t               anim_phase; // animation phase to be started when entering this state
    
} sGameObjectState, *hsGameObjectState;
typedef const sGameObjectState *hcsGameObjectState;

// game object type (static data that do not change during play)
typedef struct sGameObjectType {
    
    sPhyVector                      size;       // object dimensions (bounding box)
    sPhyVector                      pivot;      // vector from current position to image origin
    hcsGameObjectState              state;      // array of all object states
    hcsAnimationPhase               animation;  // array of all animations frames
    hcsVgaImage                     image;      // array of all images used in animations
    
} sGameObjectType, *hsGameObjectType;
typedef const sGameObjectType *hcsGameObjectType;

// runtime data of game object instance
typedef struct sGameObject {

    hcsGameObjectType               type;       // object type
    sPhyVector                      position;   // current position
    sPhyVector                      speed;      // current speed
    sAnimationState                 anim_state;
    hsGameObjectState               state;      // current state
    
} sGameObject, *hsGameObject;
typedef const sGameObject *hcsGameObject;

// switch object to given state, return TRUE if changed or FALSE if already was n the state
bool_t gobj_go_state(hsGameObject obj, int state);

#endif
