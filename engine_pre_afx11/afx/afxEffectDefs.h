
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _AFX_EFFECT_DEFS_H_
#define _AFX_EFFECT_DEFS_H_

#include "afx/arcaneFX.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectBASE

class afxEffectDefs
{
public:

  enum
  {
    MAX_EFFECTS_PER_PHRASE  = 1023,
    EFFECTS_PER_PHRASE_BITS = 10
  };

  // component effect types
  enum 
  {      
    UNKNOWN,
    EMITTER,
    EXPLOSION,
    ZODIAC,
    MODEL,
    ANIM_CLIP,
    ANIM_LOCK,
    DAMAGE,
    SOUND,
    LIGHT,
    SCRIPT_EVENT,
    CAMERA_SHAKE,
    DEBRIS,
    STATIC_SHAPE,
    PROJECTILE,
    MACHINE_GUN,
    MOORING,
    TLK_LIGHT,      // requires TLK
    VOLUME_LIGHT,   // requires TLK
    // ADD COMPONENT EFFECT TYPES HERE //
    NUM_COMPONENT_EFFECT_TYPES
  };

  // effect networking
  enum
  {
    SERVER_ONLY       = BIT(0),
    SCOPE_ALWAYS      = BIT(1),
    GHOSTABLE         = BIT(2),
    CLIENT_ONLY       = BIT(3),
    SERVER_AND_CLIENT = BIT(4)
  };
  
  // effect condititons
  enum 
  {
    DISABLED = BIT(0),
    ENABLED = BIT(1),
    FAILING = BIT(2),
    ALIVE = ENABLED,
    DEAD = DISABLED,
    DYING = FAILING,
    //
    IMPACTED_SOMETHING  = BIT(31),
    IMPACTED_TARGET     = BIT(30),
    IMPACTED_PRIMARY    = BIT(29),
    IMPACT_IN_WATER     = BIT(28),
    CASTER_IN_WATER     = BIT(27),
  };

  enum
  {
    REQUIRES_STOP     = BIT(0),
    RUNS_ON_SERVER    = BIT(1),
    RUNS_ON_CLIENT    = BIT(2),
  };

  enum 
  {
    MAX_XFM_MODIFIERS = 32,
    INFINITE_LIFETIME = (24*60*60)
  };

  enum
  {
    POINT_CONSTRAINT,
    OBJECT_CONSTRAINT,
    OBJECT_CONSTRAINT_SANS_OBJ,
    OBJECT_CONSTRAINT_SANS_SHAPE,
  };

  enum
  {
    DIRECT_DAMAGE,
    DAMAGE_OVER_TIME,
    AREA_DAMAGE
  };
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_EFFECT_DEFS_H_
