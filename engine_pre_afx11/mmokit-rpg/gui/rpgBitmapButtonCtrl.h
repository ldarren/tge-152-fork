//-----------------------------------------------------------------------------
// Original guiBitmapButtonCtrl.cc modified for the MMO KIT
//
// Would have been nice to just use a subclass, but in an attempt to increase
// performance on texture handling for these buttons (since textures get changed
// a lot) this option has been ruled out.
//
// - Adds support for subbitmaps.
// - Can render a small number on edge of bitmap.
// - Inherits alpha from parent window.
// - Has a flag if state bitmaps should be used or just one plain.
// - Optimizations to texture handling.
// - Can pulse green or red.
//-----------------------------------------------------------------------------

#ifndef _RPGBITMAPBUTTON_H_
#define _RPGBITMAPBUTTON_H_

#ifndef _GUIBITMAPBUTTON_H_
#include "gui/controls/guiBitmapButtonCtrl.h"
#endif

///-------------------------------------
/// Bitmap Button Control
/// Set 'bitmap' console field to base name of bitmaps to use.  This control will
/// append '_n' for normal
/// append '_h' for hilighted
/// append '_d' for depressed
///
/// if bitmap cannot be found it will use the default bitmap to render.
///
/// if the extent is set to (0,0) in the gui editor and apply hit, this control will
/// set it's extent to be exactly the size of the normal bitmap (if present)
///
class rpgBitmapButtonCtrl : public GuiButtonCtrl
{
private:
   typedef GuiButtonCtrl Parent;

protected:
   bool             mHasStateBitmaps;
   StringTableEntry mBitmapName;
   TextureHandle mTextureNormal;
   TextureHandle mTextureHilight;
   TextureHandle mTextureDepressed;
   TextureHandle mTextureInactive;
   F32           mU0;
   F32           mV0;
   F32           mU1;
   F32           mV1;
   S32           mNumber;

   void renderButton(TextureHandle &texture, Point2I &offset, const RectI& updateRect);

public:
   DECLARE_CONOBJECT(rpgBitmapButtonCtrl);
   rpgBitmapButtonCtrl();

   static void initPersistFields();

   //Parent methods
   bool onWake();
   void onSleep();
   void inspectPostApply();

   void setBitmapPreRender();
   void setBitmap(const char *name);
   void setBitmapUV(const char *name, F32 u0, F32 v0, F32 u1, F32 v1);

   void onRender(Point2I offset, const RectI &updateRect);
};

class rpgBitmapButtonTextCtrl : public rpgBitmapButtonCtrl
{
   typedef rpgBitmapButtonCtrl Parent;
public:
   DECLARE_CONOBJECT(rpgBitmapButtonTextCtrl);
   void onRender(Point2I offset, const RectI &updateRect);
};

#endif //_RPGBITMAPBUTTON_H_
