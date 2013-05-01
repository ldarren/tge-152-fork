
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "game/gameBase.h"

#include "afx/afxSpellBook.h"
#include "afx/afxMagicSpell.h"
#include "afx/rpg/afxRPGMagicSpell.h"
#include "afx/ui/afxSpellButton.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxSpellBookData

IMPLEMENT_CONSOLETYPE(afxSpellBookData)
IMPLEMENT_GETDATATYPE(afxSpellBookData)
IMPLEMENT_SETDATATYPE(afxSpellBookData)
IMPLEMENT_CO_DATABLOCK_V1(afxSpellBookData);

afxSpellBookData::afxSpellBookData()
{
  spells_per_page = 12;
  pages_per_book = 12;
  dMemset(spells, 0, sizeof(spells));
  dMemset(rpg_spells, 0, sizeof(rpg_spells));

  // marked true if datablock ids need to
  // be converted into pointers
  do_id_convert = false;
}

#define myOffset(field) Offset(field, afxSpellBookData)

void afxSpellBookData::initPersistFields()
{
  Parent::initPersistFields();

  addField("spellsPerPage",  TypeS8,                myOffset(spells_per_page));
  addField("pagesPerBook",   TypeS8,                myOffset(pages_per_book));
  addField("spells",         TypeGameBaseDataPtr,   myOffset(spells),     MAX_PAGES_PER_BOOK*MAX_SPELLS_PER_PAGE);
  addField("rpgSpells",      TypeGameBaseDataPtr,   myOffset(rpg_spells), MAX_PAGES_PER_BOOK*MAX_SPELLS_PER_PAGE);
}

bool afxSpellBookData::preload(bool server, char errorBuffer[256])
{
  if (!Parent::preload(server, errorBuffer))
    return false;

  // Resolve objects transmitted from server
  if (!server) 
  {
    if (do_id_convert)
    {
      for (S32 i = 0; i < pages_per_book*spells_per_page; i++)
      {
        SimObjectId db_id = (SimObjectId) rpg_spells[i];
        if (db_id != 0)
        {
          // try to convert id to pointer
          if (!Sim::findObject(db_id, rpg_spells[i]))
          {
            Con::errorf(ConsoleLogEntry::General, 
              "afxSpellBookData::preload() -- bad datablockId: 0x%x (afxRPGMagicSpellData)", 
              db_id);
          }
        }
      }
      do_id_convert = false;
    }
  }

  return true;
}

void afxSpellBookData::packData(BitStream* stream)
{
	Parent::packData(stream);

  stream->write(spells_per_page);
  stream->write(pages_per_book);

  for (S32 i = 0; i < pages_per_book*spells_per_page; i++)
    writeDatablockID(stream, rpg_spells[i], packed);
}

void afxSpellBookData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  stream->read(&spells_per_page);
  stream->read(&pages_per_book);

  do_id_convert = true;
  for (S32 i = 0; i < pages_per_book*spells_per_page; i++)
    rpg_spells[i] = (afxRPGMagicSpellData*) readDatablockID(stream);
}

ConsoleMethod(afxSpellBookData, getPageSlotIndex, S32, 3, 3, "getPageSlotIndex(%book_slot)")
{
  int page, slot; dSscanf(argv[2], "%d %d", &page, &slot);
  return object->getPageSlotIndex(page, slot);
}

ConsoleMethod(afxSpellBookData, getCapacity, S32, 2, 2, "getCapacity()")
{
  return object->spells_per_page*object->pages_per_book;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxSpellBook

IMPLEMENT_CO_NETOBJECT_V1(afxSpellBook);

afxSpellBook::afxSpellBook()
{
	mNetFlags.set(Ghostable | ScopeAlways);
	mDataBlock = NULL;
  all_spell_cooldown = 1.0f;
}

afxSpellBook::~afxSpellBook()
{
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

void afxSpellBook::initPersistFields()
{
	Parent::initPersistFields();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

void afxSpellBook::processTick(const Move* m)
{
	Parent::processTick(m);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

void afxSpellBook::advanceTime(F32 dt)
{
  Parent::advanceTime(dt);

  if (all_spell_cooldown < 1.0f)
  {
    all_spell_cooldown += dt/2.0f;
    if (all_spell_cooldown > 1.0f)
      all_spell_cooldown = 1.0f;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

bool afxSpellBook::onNewDataBlock(GameBaseData* dptr)
{
  mDataBlock = dynamic_cast<afxSpellBookData*>(dptr);
  if (!mDataBlock || !Parent::onNewDataBlock(dptr))
    return false;

  scriptOnNewDataBlock();

  return true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

bool afxSpellBook::onAdd()
{
	if (!Parent::onAdd()) 
    return(false);

	return(true);
}

void afxSpellBook::onRemove()
{
	Parent::onRemove();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

U32 afxSpellBook::packUpdate(NetConnection * con, U32 mask, BitStream * stream)
{
  U32 retMask = Parent::packUpdate(con, mask, stream);

  if (stream->writeFlag(mask & InitialUpdateMask))
  {
  }

  // AllSpellCooldown
  if (stream->writeFlag(mask & AllSpellCooldownMask))
  {
  }

	return(retMask);
}

void afxSpellBook::unpackUpdate(NetConnection * con, BitStream * stream)
{
	Parent::unpackUpdate(con, stream);

  // InitialUpdate
  if (stream->readFlag())
  {
  }

  // AllSpellCooldown
  if (stream->readFlag())
  {
    all_spell_cooldown = 0.0f;
  }
}

#define SPELL_DATA_NOT_FOUND "\n<just:center><font:Arial:20><color:FF0000>** Spell data not found **\n\n\n\n"

char* afxSpellBook::formatDesc(char* buffer, int len, S32 page, S32 slot) const
{
  S32 idx = mDataBlock->getPageSlotIndex(page, slot);
  if (idx < 0 || !mDataBlock->rpg_spells[idx])
    return SPELL_DATA_NOT_FOUND;
  
  return mDataBlock->rpg_spells[idx]->formatDesc(buffer, len);
}

const char* afxSpellBook::getSpellIcon(S32 page, S32 slot) const
{
  S32 idx = mDataBlock->getPageSlotIndex(page, slot);
  if (idx < 0 || !mDataBlock->rpg_spells[idx])
    return 0;
  
  return mDataBlock->rpg_spells[idx]->icon_name;
}

bool afxSpellBook::isPlaceholder(S32 page, S32 slot) const
{
  S32 idx = mDataBlock->getPageSlotIndex(page, slot);
  if (idx < 0 || !mDataBlock->rpg_spells[idx])
    return false;
  
  return mDataBlock->rpg_spells[idx]->is_placeholder;
}


afxMagicSpellData*  afxSpellBook::getSpellData(S32 page, S32 slot)
{
  S32 idx = mDataBlock->getPageSlotIndex(page, slot);
  if (idx < 0 || !mDataBlock->spells[idx])
    return 0;

  return mDataBlock->spells[idx];
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

void afxSpellBook::startAllSpellCooldown()
{
  //all_spell_cooldown = 0.0f;
  setMaskBits(AllSpellCooldownMask);
}

F32 afxSpellBook::getCooldownFactor(S32 page, S32 slot)
{
  return all_spell_cooldown;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

ConsoleMethod(afxSpellBook, getPageSlotIndex, S32, 3, 3, "getPageSlotIndex(%book_slot)")
{
  int page, slot; dSscanf(argv[2], "%d %d", &page, &slot);
  return object->getPageSlotIndex(page, slot);
}

ConsoleMethod(afxSpellBook, getSpellData, S32, 3, 3, "getSpellData(%book_slot)")
{
  int page, slot; dSscanf(argv[2], "%d %d", &page, &slot);
  afxMagicSpellData* spell_data = object->getSpellData(page, slot);
  return (spell_data) ? spell_data->getId() : 0;
}

ConsoleMethod(afxSpellBook, startAllSpellCooldown, void, 2, 2, "startAllSpellCooldown()")
{
  object->startAllSpellCooldown();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//



