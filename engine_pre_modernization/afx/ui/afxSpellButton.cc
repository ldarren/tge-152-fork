
//-----------------------------------------------------------------------------
// Torque Game Engine
// 
// Copyright (c) 2001 GarageGames.Com
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

//-------------------------------------
//
// Bitmap Button Contrl
// Set 'bitmap' comsole field to base name of bitmaps to use.  This control will 
// append '_n' for normal
// append '_h' for hilighted
// append '_d' for depressed
//
// if bitmap cannot be found it will use the default bitmap to render.
//
// if the extent is set to (0,0) in the gui editor and appy hit, this control will
// set it's extent to be exactly the size of the normal bitmap (if present)
//

#include "afx/arcaneFX.h"

#include "console/console.h"
#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "platform/platformAudio.h"
#ifdef AFX_TGE_1_3
#include "gui/guiCanvas.h"
#include "gui/guiDefaultControlRender.h"
#else
#include "gui/core/guiCanvas.h"
#include "gui/core/guiDefaultControlRender.h"
#endif
#include "afx/ui/afxSpellButton.h"
#include "afx/afxSpellBook.h"
#include "afx/afxMagicSpell.h"

#include "game/shapeBase.h"
#include "game/player.h"
#include "game/gameConnection.h"

IMPLEMENT_CONOBJECT(afxSpellButton);

StringTableEntry afxSpellButton::sUnknownSpellBitmap = NULL;
StringTableEntry afxSpellButton::sSpellCooldownBitmaps = NULL;

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

afxSpellButton::afxSpellButton()
{
  if (sUnknownSpellBitmap == NULL)
    sUnknownSpellBitmap = ST_NULLSTRING;
  if (sSpellCooldownBitmaps == NULL)
    sSpellCooldownBitmaps = ST_NULLSTRING;
  mBitmapName = ST_NULLSTRING;
  mBounds.extent.set(140, 30);
  spellbook = NULL;
  book_slot.set(0, 0);
}

afxSpellButton::~afxSpellButton()
{
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

void afxSpellButton::initPersistFields()
{
  Parent::initPersistFields();
  addField("bitmap",      TypeFilename,   Offset(mBitmapName, afxSpellButton));
  addField("book_slot",   TypePoint2I,    Offset(book_slot, afxSpellButton));

  Con::addVariable("pref::afxSpellButton::unknownSpellBitmap", TypeString, &sUnknownSpellBitmap);
  Con::addVariable("pref::afxSpellButton::spellCooldownBitmaps", TypeString, &sSpellCooldownBitmaps);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

bool afxSpellButton::onAdd()
{
  if (!Parent::onAdd())
    return false;

  if (sSpellCooldownBitmaps != NULL)
  {
    char buffer[256];
    for (int i = 0; i < NUM_COOLDOWN_FRAMES; i++)
    {
      dSprintf(buffer, 256, "%s_%.2d", sSpellCooldownBitmaps, i);
      cooldown_txrs[i] = TextureHandle(buffer, BitmapTexture, true);
    }
  }

  return true;
}

bool afxSpellButton::onWake()
{
  if (! Parent::onWake())
    return false;

  setActive(true);

  update_bitmap();

  return true;
}

void afxSpellButton::onSleep()
{
  mTextureNormal = NULL;
  mTextureHilight = NULL;
  mTextureDepressed = NULL;
  Parent::onSleep();
}

void afxSpellButton::onMouseEnter(const GuiEvent &event)
{
  Parent::onMouseEnter(event);
  Con::executef(this, 1, "onMouseEnter");
}

void afxSpellButton::onMouseLeave(const GuiEvent &event)
{
  Parent::onMouseLeave(event);
  Con::executef(this, 1, "onMouseLeave");
}

void afxSpellButton::inspectPostApply()
{
  // if the extent is set to (0,0) in the gui editor and apply hit, this control will
  // set it's extent to be exactly the size of the normal bitmap (if present)
  Parent::inspectPostApply();
  
  if ((mBounds.extent.x == 0) && (mBounds.extent.y == 0) && mTextureNormal)
  {
    TextureObject *texture = (TextureObject *) mTextureNormal;
    mBounds.extent.x = texture->bitmapWidth;
    mBounds.extent.y = texture->bitmapHeight;
  }
}

void afxSpellButton::setBitmap(const char *name, bool placeholder)
{
  mBitmapName = (name) ? StringTable->insert(name) : ST_NULLSTRING;
  if (!isAwake())
    return;
  
  if (mBitmapName != ST_NULLSTRING)
  {
    char buffer[1024];
    char *p;

    if (placeholder)
    {
      dStrcpy(buffer, name);
      p = buffer + dStrlen(buffer);
    
      dStrcpy(p, "_i");
      mTextureInactive = TextureHandle(buffer, BitmapTexture, true);
      mTextureNormal = mTextureInactive;
      mTextureHilight = mTextureInactive;
      mTextureDepressed = mTextureInactive;
      setActive(false);
    }
    else
    {
      dStrcpy(buffer, name);
      p = buffer + dStrlen(buffer);   
      dStrcpy(p, "_n");
      mTextureNormal = TextureHandle(buffer, BitmapTexture, true);
      dStrcpy(p, "_h");
      mTextureHilight = TextureHandle(buffer, BitmapTexture, true);
      if (!mTextureHilight)
        mTextureHilight = mTextureNormal;
      dStrcpy(p, "_d");
      mTextureDepressed = TextureHandle(buffer, BitmapTexture, true);
      if (!mTextureDepressed)
        mTextureDepressed = mTextureHilight;
      dStrcpy(p, "_i");
      mTextureInactive = TextureHandle(buffer, BitmapTexture, true);
      if (!mTextureInactive)
        mTextureInactive = mTextureNormal;
      setActive(true);
    }
  }
  else
  {
    mTextureNormal = NULL;
    mTextureHilight = NULL;
    mTextureDepressed = NULL;
    mTextureInactive = NULL;
  }

  setUpdate();
}   

void afxSpellButton::onRender(Point2I offset, const RectI& updateRect)
{
  enum { NORMAL, HILIGHT, DEPRESSED, INACTIVE } state = NORMAL;

  if (mActive)
  {
    if (mMouseOver) state = HILIGHT;
    if (mDepressed || mStateOn) state = DEPRESSED;
  }
  else
    state = INACTIVE;
  
  switch (state)
  {
  case NORMAL:      renderButton(mTextureNormal, offset, updateRect); break;
  case HILIGHT:     renderButton(mTextureHilight ? mTextureHilight : mTextureNormal, offset, updateRect); break;
  case DEPRESSED:   renderButton(mTextureDepressed, offset, updateRect); break;
  case INACTIVE:    renderButton(mTextureInactive ? mTextureInactive : mTextureNormal, offset, updateRect); break;
  }
}

void afxSpellButton::onDeleteNotify(SimObject* obj)
{
  // Handle Shape Deletion
  afxSpellBook* book = dynamic_cast<afxSpellBook*>(obj);
  if (book != NULL)
  {
    if (book == spellbook)
    {
      spellbook = NULL;
      setBitmap("");
      setVisible(false);
      return;
    }
  }

  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// protected:

void afxSpellButton::renderButton(TextureHandle &texture, Point2I &offset, 
                                  const RectI& updateRect)
{
  static int idx = 0;

  if (texture)
  {
    RectI rect(offset, mBounds.extent);
    dglClearBitmapModulation();
    dglDrawBitmapStretch(texture, rect);

    if (spellbook)
    {
      F32 cooldown = spellbook->getCooldownFactor(book_slot.x, book_slot.y);
      if (cooldown < 1.0f)
      {
        if (cooldown_txrs[(int)(36.0f*cooldown)])
          dglDrawBitmapStretch(cooldown_txrs[(int)(36.0f*cooldown)], rect);
      }
    }

    renderChildControls( offset, updateRect);
  }
  else
    Parent::onRender(offset, updateRect);
}

void afxSpellButton::update_bitmap()
{
  const char* icon_name = 0;

  bool is_placeholder = false; 
  if (spellbook)
  {
    icon_name = spellbook->getSpellIcon(book_slot.x, book_slot.y);
    is_placeholder = spellbook->isPlaceholder(book_slot.x, book_slot.y);
    if (icon_name && icon_name[0] == '\0')
      icon_name = sUnknownSpellBitmap;
  }

  if (icon_name)
  {
    setBitmap(icon_name, is_placeholder);
    setVisible(true);
  }
  else
  {
    setBitmap("");
    setVisible(false);
  }
}

void afxSpellButton::setSpellBook(afxSpellBook* book, U8 page)
{
  book_slot.x = page;

  if (spellbook)
    clearNotify(spellbook);

  spellbook = book;
  update_bitmap();

  if (spellbook)
    deleteNotify(spellbook);
}

void afxSpellButton::setPage(U8 page)
{
  book_slot.x = page;
  update_bitmap();
}

char* afxSpellButton::formatDesc(char* buffer, int len) const
{
  return (spellbook) ? spellbook->formatDesc(buffer, len, book_slot.x, book_slot.y) : (char*)"";
}

S32 afxSpellButton::getSpellDataBlock() const
{
  afxMagicSpellData* spell_data = spellbook->getSpellData(book_slot.x, book_slot.y);
  return (spell_data) ?  spell_data->getId() : -1;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

ConsoleMethod(afxSpellButton, onSpellbookChange, void, 4, 4, "(%spellbook, %page)")
{
  object->setSpellBook(dynamic_cast<afxSpellBook*>(Sim::findObject(argv[2])), dAtoi(argv[3]));
}

ConsoleMethod(afxSpellButton, onTurnPage, void, 3, 3, "(page)")
{
  object->setPage(dAtoi(argv[2]));
}

ConsoleMethod(afxSpellButton, getSpellDescription, const char*, 2, 2, "getSpellDescription()")
{
  char buf[1024];
  return object->formatDesc(buf, 1024);
}

ConsoleMethod(afxSpellButton, getSpellDataBlock, S32, 2, 2, "getSpellDataBlock()")
{
  return object->getSpellDataBlock();
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
