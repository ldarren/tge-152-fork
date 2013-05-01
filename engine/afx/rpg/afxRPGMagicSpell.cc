
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"
#include "afxRPGMagicSpell.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxRPGMagicSpellData

IMPLEMENT_CONSOLETYPE(afxRPGMagicSpellData)
IMPLEMENT_GETDATATYPE(afxRPGMagicSpellData)
IMPLEMENT_SETDATATYPE(afxRPGMagicSpellData)
IMPLEMENT_CO_DATABLOCK_V1(afxRPGMagicSpellData);

afxRPGMagicSpellData::afxRPGMagicSpellData()
{
  // spell parameters
  spell_name = ST_NULLSTRING;
  spell_desc = ST_NULLSTRING;
  spell_target = 0;
  spell_range = 0.0f;
  mana_cost = 0;

  n_reagents = 0;
  for (S32 i = 0; i < MAX_REAGENTS_PER_SPELL; i++)
  {
    reagent_cost[i] = 1;
    reagent_name[i] = ST_NULLSTRING;
  }

  // spell phase timing
  casting_dur = 0.0f;

  // interface elements
  icon_name = ST_NULLSTRING;
  source_pack = ST_NULLSTRING;

  is_placeholder = false;
}

static EnumTable::Enums SpellTargetEnums[] =
{
   { afxRPGMagicSpellData::TARGET_NOTHING,     "nothing"   },
   { afxRPGMagicSpellData::TARGET_SELF,        "self"      },
   { afxRPGMagicSpellData::TARGET_FRIEND,      "friend"    },
   { afxRPGMagicSpellData::TARGET_ENEMY,       "enemy"     },
   { afxRPGMagicSpellData::TARGET_CORPSE,      "corpse"    },
   { afxRPGMagicSpellData::TARGET_AREA,        "area"      },
   { afxRPGMagicSpellData::TARGET_DIRECTION,   "direction" },
};
static EnumTable SpellTargetTable(7, SpellTargetEnums);

#define myOffset(field) Offset(field, afxRPGMagicSpellData)

void afxRPGMagicSpellData::initPersistFields()
{
  Parent::initPersistFields();

  // spell parameters
  addField("name",          TypeString,     myOffset(spell_name));
  addField("desc",          TypeString,     myOffset(spell_desc));
  addField("target",        TypeEnum,       myOffset(spell_target), 1, &SpellTargetTable);
  addField("range",         TypeF32,        myOffset(spell_range));
  addField("manaCost",      TypeS32,        myOffset(mana_cost));
  addField("reagentCost",   TypeS8,         myOffset(reagent_cost), MAX_REAGENTS_PER_SPELL);
  addField("reagentName",   TypeString,     myOffset(reagent_name), MAX_REAGENTS_PER_SPELL);

  // spell phase timing
  addField("castingDur",    TypeF32,        myOffset(casting_dur));

  // interface elements
  addField("iconBitmap",    TypeFilename,   myOffset(icon_name));
  addField("sourcePack",    TypeString,     myOffset(source_pack));
  addField("isPlaceholder", TypeBool,       myOffset(is_placeholder));
}

bool afxRPGMagicSpellData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;
  
  n_reagents = 0;
  for (S32 i = 0; i < MAX_REAGENTS_PER_SPELL && reagent_name[i] != ST_NULLSTRING; i++)
    n_reagents++;
  
  return true;
}

void afxRPGMagicSpellData::packData(BitStream* stream)
{
	Parent::packData(stream);

  stream->write(spell_target);
  stream->write(spell_range);
  stream->write(mana_cost);
  stream->write(n_reagents);
  for (S32 i = 0; i < n_reagents; i++)
  {
    stream->write(reagent_cost[i]);
    stream->writeString(reagent_name[i]);
  }

  stream->write(casting_dur);

  stream->writeString(spell_name);
  stream->writeString(spell_desc);
  stream->writeString(icon_name);
  stream->writeString(source_pack);
  stream->write(is_placeholder);
}

void afxRPGMagicSpellData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  stream->read(&spell_target);
  stream->read(&spell_range);
  stream->read(&mana_cost);
  stream->read(&n_reagents); 
  for (S32 i = 0; i < n_reagents; i++)
  {
    stream->read(&reagent_cost[i]);
    reagent_name[i] = stream->readSTString();
  }

  stream->read(&casting_dur);

  spell_name = stream->readSTString();
  spell_desc = stream->readSTString();
  icon_name = stream->readSTString();
  source_pack = stream->readSTString();
  stream->read(&is_placeholder); 
}

#define NAME_FMT      "<just:left><font:Arial:20><color:FFD200>"
#define TARGET_FMT    "<just:right><font:Arial:20><color:ACACAC>"
#define MANACOST_FMT  "<just:left><font:Arial:16><color:FFFFFF>"
#define RANGE_FMT     "<just:right><font:Arial:16><color:FFFFFF>"
#define CASTLEN_FMT   "<just:left><font:Arial:16><color:FFFFFF>"
#define DESC_FMT      "<just:left><font:Arial:16><color:ACACAC>"
#define SMALL_BR      "<font:Arial:4>\n"
#define BR            "\n"
#define PACK_FMT      "<just:right><font:Arial:14><color:ACACAC>"
#define PACK_NAME_FMT "<color:FFD200>"


char* afxRPGMagicSpellData::fmt_placeholder_desc(char* buffer, int len) const
{
  char pack_str[32]; 
  if (source_pack == ST_NULLSTRING)
    dStrcpy(pack_str, "unknown");
  else
    dSprintf(pack_str, 32, "%s", source_pack);

  dSprintf(buffer, len, 
          NAME_FMT "%s" BR
          SMALL_BR
          DESC_FMT "%s" BR
          SMALL_BR SMALL_BR
          PACK_FMT "source: " PACK_NAME_FMT "%s",
          spell_name, spell_desc, pack_str);

  return buffer;
}

char* afxRPGMagicSpellData::formatDesc(char* buffer, int len) const
{
  if (is_placeholder)
    return fmt_placeholder_desc(buffer, len);

  char target_str[32]; target_str[0] = '\0';
  for (int i = 0; i < SpellTargetTable.size; i++)
  {
    if (SpellTargetTable.table[i].index == spell_target)
    {
      if (spell_target != TARGET_NOTHING)
        dStrcpy(target_str, SpellTargetTable.table[i].label);
      break;
    }
  }

  char range_str[32]; range_str[0] = '\0';
  if (spell_range > 0)
  {
    if (spell_range == ((F32)((S32)spell_range)))
      dSprintf(range_str, 32, "%d meter range", (S32) spell_range);
    else
      dSprintf(range_str, 32, "%.1f meter range", spell_range);
  }

  char casting_str[32];
  if (casting_dur <= 0)
    dStrcpy(casting_str, "instant");
  else
    dSprintf(casting_str, 32, "%.1f sec cast", casting_dur);

  char pack_str[32]; 
  if (source_pack == ST_NULLSTRING)
    dStrcpy(pack_str, "unknown");
  else
    dSprintf(pack_str, 32, "%s", source_pack);

  dSprintf(buffer, len, 
          NAME_FMT "%s" TARGET_FMT "%s" BR
          SMALL_BR
          MANACOST_FMT "%d Mana" RANGE_FMT "%s" BR
          CASTLEN_FMT "%s" BR
          SMALL_BR
          DESC_FMT "%s" BR SMALL_BR
          PACK_FMT "source: " PACK_NAME_FMT "%s",
          spell_name, target_str, 
          mana_cost, range_str, 
          casting_str, 
          spell_desc, pack_str);

  return buffer;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
