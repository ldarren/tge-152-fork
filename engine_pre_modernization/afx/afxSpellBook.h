
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _AFX_SPELL_BOOK_H_
#define _AFX_SPELL_BOOK_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"

class afxSpellBookDefs
{
public:
  enum {
    MAX_SPELLS_PER_PAGE = 12,
    MAX_PAGES_PER_BOOK = 12
  };
};

class afxMagicSpellData;
class afxRPGMagicSpellData;

class afxSpellBookData : public GameBaseData, public afxSpellBookDefs
{
  typedef GameBaseData  Parent;

  bool                  do_id_convert;

public:
  U8                    spells_per_page;
  U8                    pages_per_book;
  afxMagicSpellData*    spells[MAX_PAGES_PER_BOOK*MAX_SPELLS_PER_PAGE];
  afxRPGMagicSpellData* rpg_spells[MAX_PAGES_PER_BOOK*MAX_SPELLS_PER_PAGE];

public:
  /*C*/                 afxSpellBookData();

  virtual void          packData(BitStream*);
  virtual void          unpackData(BitStream*);
  virtual bool          preload(bool server, char errorBuffer[256]);

  bool                  verifyPageSlot(S32 page, S32 slot);
  S32                   getPageSlotIndex(S32 page, S32 slot);

  static void           initPersistFields();

  DECLARE_CONOBJECT(afxSpellBookData);
};

inline bool afxSpellBookData::verifyPageSlot(S32 page, S32 slot)
{
  return (page >= 0 && page < pages_per_book && slot >= 0 && slot < spells_per_page);
}

inline S32 afxSpellBookData::getPageSlotIndex(S32 page, S32 slot)
{
  return (verifyPageSlot(page, slot)) ? page*spells_per_page + slot : -1;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxMagicSpellData;
class afxSpellButton;

class afxSpellBook : public GameBase, public afxSpellBookDefs
{
  typedef GameBase        Parent;

  enum MaskBits 
  {
    AllSpellCooldownMask  = Parent::NextFreeMask << 0,
    NextFreeMask          = Parent::NextFreeMask << 1
  };

private:
  afxSpellBookData*       mDataBlock;
  F32                     all_spell_cooldown;

public:
  /*C*/                   afxSpellBook();
  /*D*/                   ~afxSpellBook();

  virtual bool            onNewDataBlock(GameBaseData* dptr);
  virtual void            processTick(const Move*);
  virtual void            advanceTime(F32 dt);

  virtual bool            onAdd();
  virtual void            onRemove();

  virtual U32             packUpdate(NetConnection*, U32, BitStream*);
  virtual void            unpackUpdate(NetConnection*, BitStream*);

  static void             initPersistFields();

  S32                     getPageSlotIndex(S32 page, S32 slot);
  char*                   formatDesc(char* buffer, int len, S32 page, S32 slot) const;
  const char*             getSpellIcon(S32 page, S32 slot) const;
  bool                    isPlaceholder(S32 page, S32 slot) const;
  afxMagicSpellData*      getSpellData(S32 page, S32 slot);

  void                    startAllSpellCooldown();
  F32                     getCooldownFactor(S32 page, S32 slot);

  DECLARE_CONOBJECT(afxSpellBook);
};

inline S32 afxSpellBook::getPageSlotIndex(S32 page, S32 slot)
{
  return (mDataBlock) ? mDataBlock->getPageSlotIndex(page, slot) : -1;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_SPELL_BOOK_H_
