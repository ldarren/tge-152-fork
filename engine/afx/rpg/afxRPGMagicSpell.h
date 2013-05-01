
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _AFX_RPG_MAGIC_SPELL_H_
#define _AFX_RPG_MAGIC_SPELL_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "core/tVector.h"
#include "game/gameBase.h"
#include "console/typeValidators.h"

#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/afxMagicMissile.h"

class afxMagicMissileData;
class afxEffectWrapperData;
class SceneObject;

class afxRPGMagicSpellDefs
{
public:
  // Migrate this stuff to RPG Magic-System
  enum {
    TARGET_NOTHING,
    TARGET_SELF,
    TARGET_FRIEND,
    TARGET_ENEMY,
    TARGET_CORPSE,
    TARGET_AREA,
    TARGET_DIRECTION,
  };

  enum {
    MAX_REAGENTS_PER_SPELL = 8,
  };
};

class afxRPGMagicSpellData : public GameBaseData, public afxRPGMagicSpellDefs
{
  typedef GameBaseData Parent;

public:
  F32               casting_dur;
  StringTableEntry  spell_name;
  StringTableEntry  spell_desc;
  S32               spell_target;
  F32               spell_range;
  S32               mana_cost;
  U8                n_reagents;
  S8                reagent_cost[MAX_REAGENTS_PER_SPELL];
  StringTableEntry  reagent_name[MAX_REAGENTS_PER_SPELL];
  StringTableEntry  icon_name;
  StringTableEntry  source_pack;
  bool              is_placeholder;

private:
  char*             fmt_placeholder_desc(char* buffer, int len) const;

public:
  /*C*/             afxRPGMagicSpellData();

  char*             formatDesc(char* buffer, int len) const;
  bool              requiresTarget() { return (spell_target == TARGET_ENEMY || spell_target == TARGET_CORPSE || spell_target == TARGET_FRIEND); }

  virtual bool      onAdd();
  virtual void      packData(BitStream*);
  virtual void      unpackData(BitStream*);

  static void       initPersistFields();

  DECLARE_CONOBJECT(afxRPGMagicSpellData);
};
DECLARE_CONSOLETYPE(afxRPGMagicSpellData);

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_RPG_MAGIC_SPELL_H_
