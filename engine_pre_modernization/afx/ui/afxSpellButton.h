//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _AFX_SPELL_BUTTON_H_
#define _AFX_SPELL_BUTTON_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifdef AFX_TGE_1_3
#include "gui/guiButtonCtrl.h"
#else
#include "gui/controls/guiButtonCtrl.h"
#endif
#include "dgl/gTexManager.h"

///-------------------------------------
/// Bitmap Button Contrl
/// Set 'bitmap' comsole field to base name of bitmaps to use.  This control will 
/// append '_n' for normal
/// append '_h' for hilighted
/// append '_d' for depressed
///
/// if bitmap cannot be found it will use the default bitmap to render.
///
/// if the extent is set to (0,0) in the gui editor and appy hit, this control will
/// set it's extent to be exactly the size of the normal bitmap (if present)
///

class afxSpellBook;

class afxSpellButton : public GuiButtonCtrl
{
private:
  typedef GuiButtonCtrl Parent;

  enum { NUM_COOLDOWN_FRAMES = 36 };
  
protected:
  static StringTableEntry sUnknownSpellBitmap;
  static StringTableEntry sSpellCooldownBitmaps;

  StringTableEntry  mBitmapName;
  TextureHandle     mTextureNormal;
  TextureHandle     mTextureHilight;
  TextureHandle     mTextureDepressed;
  TextureHandle     mTextureInactive;
  
  afxSpellBook*     spellbook;
  Point2I           book_slot;

  TextureHandle     cooldown_txrs[NUM_COOLDOWN_FRAMES];
  
  void              update_bitmap();
  void              renderButton(TextureHandle &texture, Point2I &offset, const RectI& updateRect);
  
public:   
  /*C*/             afxSpellButton();
  /*D*/             ~afxSpellButton();
  
  void              setBitmap(const char *name, bool placholder=false);
  void              setSpellBook(afxSpellBook*, U8 page);
  void              setPage(U8 page);
  char*             formatDesc(char* buffer, int len) const;
  S32               getSpellDataBlock() const;
  
  virtual bool      onAdd();
  virtual bool      onWake();
  virtual void      onSleep();
  virtual void      inspectPostApply();
  virtual void      onMouseEnter(const GuiEvent &event);
  virtual void      onMouseLeave(const GuiEvent &event);
  virtual void      onRender(Point2I offset, const RectI &updateRect);

  virtual void      onDeleteNotify(SimObject*);
  
  static void       initPersistFields();
  
  DECLARE_CONOBJECT(afxSpellButton);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif //_GUI_BITMAP_BUTTON_CTRL_H
