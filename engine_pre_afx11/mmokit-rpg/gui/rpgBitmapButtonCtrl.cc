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


//-------------------------------------
//
// Bitmap Button Control
// Set 'bitmap' console field to base name of bitmaps to use.  This control will
// append '_n' for normal
// append '_h' for hilighted
// append '_d' for depressed
//
// if bitmap cannot be found it will use the default bitmap to render.
//
// if the extent is set to (0,0) in the gui editor and appy hit, this control will
// set it's extent to be exactly the size of the normal bitmap (if present)
//


#include "console/console.h"
#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "platform/platformAudio.h"
#include "gui/core/guiCanvas.h"
#include "gui/core/guiDefaultControlRender.h"
#include "rpg/gui/rpgBitmapButtonCtrl.h"

extern F32 gWindowAlphaMod;

IMPLEMENT_CONOBJECT(rpgBitmapButtonCtrl);

//-------------------------------------
rpgBitmapButtonCtrl::rpgBitmapButtonCtrl()
{
   mBitmapName = StringTable->insert("");
   mBounds.extent.set(140, 30);
   mHasStateBitmaps = false;
   mU0 = 0.f;
   mV0 = 0.f;
   mU1 = 1.f;
   mV1 = 1.f;
   mNumber = -1;
}


//-------------------------------------
void rpgBitmapButtonCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("bitmap",          TypeFilename, Offset(mBitmapName,      rpgBitmapButtonCtrl));
   addField("number",          TypeS32,      Offset(mNumber,          rpgBitmapButtonCtrl));
   addField("hasStateBitmaps", TypeBool,     Offset(mHasStateBitmaps, rpgBitmapButtonCtrl));
   addField("u0", TypeF32, Offset(mU0, rpgBitmapButtonCtrl));
   addField("u1", TypeF32, Offset(mU1, rpgBitmapButtonCtrl));
   addField("v0", TypeF32, Offset(mV0, rpgBitmapButtonCtrl));
   addField("v1", TypeF32, Offset(mV1, rpgBitmapButtonCtrl));
}


//-------------------------------------
bool rpgBitmapButtonCtrl::onWake()
{
   if (! Parent::onWake())
      return false;
   
   F32 u0, v0, u1, v1;
   u0 = mU0;
   v0 = mV0;
   u1 = mU1;
   v1 = mV1;
   
   setBitmap(mBitmapName);
   mU0 = u0;
   mV0 = v0;
   mU1 = u1;
   mV1 = v1;
   return true;
}


//-------------------------------------
void rpgBitmapButtonCtrl::onSleep()
{
   mTextureNormal = NULL;
   mTextureHilight = NULL;
   mTextureDepressed = NULL;
   Parent::onSleep();
}


//-------------------------------------

ConsoleMethod( rpgBitmapButtonCtrl, setBitmap, void, 3, 3, "(filepath name)")
{
   object->setBitmapUV(argv[2],0.f,0.f,1.f,1.f);
}

ConsoleMethod( rpgBitmapButtonCtrl, setBitmapUV, void, 7, 7, "(filepath name)")
{
   F32 u0, v0, u1, v1;
   u0 = dAtof(argv[3]);
   v0 = dAtof(argv[4]);
   u1 = dAtof(argv[5]);
   v1 = dAtof(argv[6]);

   object->setBitmapUV(argv[2],u0,v0,u1,v1);
}

//-------------------------------------
void rpgBitmapButtonCtrl::inspectPostApply()
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

//we are clearing and setting bitmaps a lot, and in stock Torque this
//causes a lot of texture locking... so we are splitting this out
void rpgBitmapButtonCtrl::setBitmapPreRender()
{
   if(!isAwake())
      return;

   if (*mBitmapName && mHasStateBitmaps)
   {
      char buffer[1024];
      char *p;
      dStrcpy(buffer, mBitmapName);
      p = buffer + dStrlen(buffer);

      mTextureNormal = TextureHandle(buffer, BitmapTexture, true);
      if (!mTextureNormal)
      {
         dStrcpy(p, "_n");
         mTextureNormal = TextureHandle(buffer, BitmapTexture, true);
      }
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
   }
   else if (*mBitmapName)
   {
      if (!mTextureNormal || (mTextureNormal && mTextureNormal.getName() != mBitmapName))
      {
         mTextureNormal = TextureHandle(mBitmapName, BitmapTexture, true);
         mTextureHilight = mTextureDepressed = mTextureInactive = mTextureNormal;
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


//-------------------------------------
void rpgBitmapButtonCtrl::setBitmap(const char *name)
{
   mBitmapName = StringTable->insert(name);
}

void rpgBitmapButtonCtrl::setBitmapUV(const char *name, F32 U0, F32 V0, F32 U1, F32 V1)
{
   mBitmapName = StringTable->insert(name);
   mU0 = U0;
   mV0 = V0;
   mU1 = U1;
   mV1 = V1;
}


//-------------------------------------
void rpgBitmapButtonCtrl::onRender(Point2I offset, const RectI& updateRect)
{
   enum {
      NORMAL,
      HILIGHT,
      DEPRESSED,
      INACTIVE
   } state = NORMAL;

   if (mActive)
   {
      if (mMouseOver) state = HILIGHT;
      if (mDepressed || mStateOn) state = DEPRESSED;
   }
   else
      state = INACTIVE;
   
   setBitmapPreRender();

   switch (state)
   {
      case NORMAL:      renderButton(mTextureNormal, offset, updateRect); break;
      case HILIGHT:     renderButton(mTextureHilight ? mTextureHilight : mTextureNormal, offset, updateRect); break;
      case DEPRESSED:   renderButton(mTextureDepressed, offset, updateRect); break;
      case INACTIVE:    renderButton(mTextureInactive ? mTextureInactive : mTextureNormal, offset, updateRect); break;
   }
}

//------------------------------------------------------------------------------

void rpgBitmapButtonCtrl::renderButton(TextureHandle &texture, Point2I &offset, const RectI& updateRect)
{
#ifdef DARREN_MMO
   bool highlight = mMouseOver;
   ColorI fontColor = mActive ? (highlight ? mProfile->mFontColorHL : mProfile->mFontColor) : mProfile->mFontColorNA;
   ColorI fillColor = mActive ? (mMouseOver ? mProfile->mFillColorHL : mProfile->mFillColor) : mProfile->mFontColorNA;
   
   if (1)
   {
      ColorI backColor;
      ColorI fillColor;
      ColorI hkColor;

      bool filled = false;
      if (texture || (mButtonText && dStrlen(mButtonText)))
         filled = true;

      fillColor.set(255,255,255,U8(255.f*gWindowAlphaMod));
      
      if (!mStateOn && mMouseOver)
         backColor.set(255,255,255,U8(70.f*gWindowAlphaMod));
      else if (!mStateOn)
         backColor.set(200,200,200,U8(70.f*gWindowAlphaMod));

      if (mStateOn && mMouseOver)
      {
         backColor.set(128,128,128,U8(70.f*gWindowAlphaMod));
      }
      else if (mStateOn)
         backColor.set(64,64,64,U8(70.f*gWindowAlphaMod));

      if (!filled && !mMouseOver)
         backColor.set(32,32,32,U8(70.f*gWindowAlphaMod));
      if (!filled && mMouseOver)
         backColor.set(64,64,64,U8(70.f*gWindowAlphaMod));

      if (mPulseRed)
      {
         F32 s = 191.f+(64.f*sin((Platform::getVirtualMilliseconds()*.0025f)));

         backColor.set(U8(s),0,0,U8(70.f*gWindowAlphaMod));
         fillColor = backColor;
         fillColor.alpha = U8(255.f*gWindowAlphaMod);
      }

      if (mPulseGreen)
      {
         F32 s = 191.f+(64.f*sin((Platform::getVirtualMilliseconds()*.0025f)));

         backColor.set(0,U8(s),0,U8(70.f*gWindowAlphaMod));
         fillColor = backColor;
         fillColor.alpha = U8(255.f*gWindowAlphaMod);
      }

      Point2I off(0,0);
      if (mDepressed)
         off.set(1,1);

      RectI boundsRect(offset+off, mBounds.extent);
      
      if (!mStateOn)
      {
         hkColor.set(10,255,206,U8(255.f*gWindowAlphaMod));
         renderRaisedBox(boundsRect, mProfile,&backColor); 
         if (texture)
         {
            TextureObject* tex = (TextureObject*) texture;
            RectI sr(mU0*tex->texWidth,mV0*tex->texHeight,mU1*tex->texWidth,mV1*tex->texHeight);
            
            backColor.alpha = U8(255.f*gWindowAlphaMod);
            dglSetBitmapModulation( backColor );
            dglDrawBitmapStretchSR(texture, boundsRect,sr);
         }
      }
      else
      {
         hkColor.set(64,154,136,U8(255.f*gWindowAlphaMod));
         renderLoweredBox(boundsRect, mProfile,&backColor); 
         if (texture)
         {
            TextureObject* tex = (TextureObject*) texture;
            RectI sr(mU0*tex->texWidth,mV0*tex->texHeight,mU1*tex->texWidth,mV1*tex->texHeight);
            if (mPulseRed)
            {
               fillColor = backColor;
               fillColor.alpha = U8(255.f*gWindowAlphaMod);
            }
            else if (mPulseGreen)
            {
               fillColor = backColor;
               fillColor.alpha = U8(255.f*gWindowAlphaMod);
            }
            else
            {
               if (mMouseOver)
                  fillColor.set(128,128,128,U8(255.f*gWindowAlphaMod));
               else
                  fillColor.set(80,80,80,U8(255.f*gWindowAlphaMod));
            }
            
            fillColor.alpha=U8(255.f*gWindowAlphaMod);
            dglSetBitmapModulation( fillColor );

            dglDrawBitmapStretchSR(texture, boundsRect,sr);
         }
      }

      if (mHotKey && dStrlen(mHotKey) && dStrcmp(mHotKey,"-1"))
      {
         Point2I textPos = offset;
         textPos.x+=2;
         textPos.y=offset.y+(mBounds.extent.y/2)-7;
         if(mDepressed || !mActive)
            textPos += Point2I(1,1);

         dglSetBitmapModulation( hkColor );
         GuiControlProfile::AlignmentType backup = mProfile->mAlignment;
         mProfile->mAlignment = GuiControlProfile::LeftJustify;
         renderJustifiedText(textPos, mBounds.extent, mHotKey);
         mProfile->mAlignment = backup;
      }
      
      if (mButtonText)
      {
         Point2I btextPos = offset;
         if(mDepressed)
            btextPos += Point2I(1,1);

         dglSetBitmapModulation( fontColor );
   
         renderJustifiedText(btextPos, mBounds.extent, mButtonText);
      }

      if (mNumber!=-1)
      {
         char buff[32];
         dSprintf(buff,32,"%i",mNumber);
         GFont *font = mProfile->mFont;
         S32 textWidth = font->getStrWidth(buff);
         Point2I start;
         Point2I extent = boundsRect.extent;

         start.set( extent.x - textWidth-3, 0 );

         if( textWidth > extent.x )
            start.set( 0, 0 );

         // center the vertical
         start.y = ( extent.y - font->getHeight());

         dSprintf(buff,32,"\3%i",mNumber);
         dglDrawText( font, start + offset, buff, mProfile->mFontColors );
      }
      
      renderChildControls( offset, updateRect);
   }
#endif // DARREN_MMO
}

//------------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(rpgBitmapButtonTextCtrl);

void rpgBitmapButtonTextCtrl::onRender(Point2I offset, const RectI& updateRect)
{
   enum {
      NORMAL,
      HILIGHT,
      DEPRESSED,
      INACTIVE
   } state = NORMAL;

   if (mActive)
   {
      if (mMouseOver) state = HILIGHT;
      if (mDepressed || mStateOn) state = DEPRESSED;
   }
   else
      state = INACTIVE;

   ColorI fontColor = mProfile->mFontColor;

   TextureHandle texture;

   switch (state)
   {
      case NORMAL:
         texture = mTextureNormal;
         fontColor = mProfile->mFontColor;
         break;
      case HILIGHT:
         texture = mTextureHilight;
         fontColor = mProfile->mFontColorHL;
         break;
      case DEPRESSED:
         texture = mTextureDepressed;
         fontColor = mProfile->mFontColorSEL;
         break;
      case INACTIVE:
         texture = mTextureInactive;
         fontColor = mProfile->mFontColorNA;
         if(!texture)
            texture = mTextureNormal;
         break;
   }
   if (texture)
   {
      RectI rect(offset, mBounds.extent);
      dglClearBitmapModulation();
      dglDrawBitmapStretch(texture, rect);

      Point2I textPos = offset;
      if(mDepressed)
         textPos += Point2I(1,1);

      // Make sure we take the profile's textOffset into account.
      textPos += mProfile->mTextOffset;

      dglSetBitmapModulation( fontColor );
      renderJustifiedText(textPos, mBounds.extent, mButtonText);

      renderChildControls( offset, updateRect);
   }
   else
      Parent::onRender(offset, updateRect);
}
