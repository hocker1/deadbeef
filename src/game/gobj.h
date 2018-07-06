#ifndef __GOBJ_H
#define __GOBJ_H

#include "types.h"
#include "anim.h"

// game object type (static data that do not change during play)
typedef struct sGameObjectType {
    
    sPhyVector                      size;       // object dimensions (bounding box)
    sPhyVector                      pivot;      // vector from current position to image origin
    hsAnimationPhase                animation;  // array of all animations frames
    hsVgaImage                      image;      // array of all images used in animations
    
} sGameObjectType, *hsGameObjectType;
typedef const sGameObjectType *hcsGameObjectType;

// runtime data of game object instance
typedef struct sGameObject {

    hcsGameObjectType               type;       // object type
    sPhyVector                      position;   // current position
    sPhyVector                      speed;      // current speed
    sAnimationState                 anim_state;
    
} sGameObject, *hsGameObject;
typedef const sGameObject *hcsGameObject;

#endif
