//------------------------------------------------------------------------------
// Gridwalkers
//
// Copyright (c) 2003 Pat Wilson and Flaming Duck Studio
// Portions Copyright (c) 2001 GarageGames.Com
//------------------------------------------------------------------------------

#include "dgl/gDynamicTexture.h"
#include "dgl/gBitmap.h"
#include "gui/core/guiControl.h"
#include "dgl/dgl.h"
#include "util/safeDelete.h"

Vector<DynamicTexture *> DynamicTexture::smRegisteredGuiUpdaters;
Vector<DynamicTexture *> DynamicTexture::smRegisteredScreenUpdaters;
Vector<DynamicTexture *> DynamicTexture::smUpdateAtEndOfFrame;

//--------------------------------------------------------------------------

void dynamicTextureCB(const U32 eventCode, void *userData)
{
   DynamicTexture* ts = reinterpret_cast<DynamicTexture*>(userData);

   if(eventCode==TextureManager::BeginZombification)
   {
      SAFE_DELETE(ts->mTextureHandle);
   }
   else if(eventCode==TextureManager::CacheResurrected)
   {
      GBitmap *bmp = new GBitmap( ts->mSize.x, ts->mSize.y, false, GBitmap::RGBA );
      ts->mTextureHandle = new TextureHandle( NULL, bmp, BitmapKeepTexture, true );
   }
}


//------------------------------------------------------------------------------

DynamicTexture::DynamicTexture()
{
   initDT();
}

//------------------------------------------------------------------------------

DynamicTexture::DynamicTexture( GuiControl * control )
{
   initDT();

   registerForGuiUpdates( control );
}

//------------------------------------------------------------------------------

DynamicTexture::DynamicTexture( const RectI &updateRect )
{
   initDT();
   mUpdateRect = updateRect;

   setUpdateRect( updateRect );
}

//------------------------------------------------------------------------------

DynamicTexture::~DynamicTexture()
{
   TextureManager::unregisterEventCallback(mTexCBHandle);
   SAFE_DELETE( mTextureHandle );
   SAFE_DELETE( mTempDynamicTexture );
}

//------------------------------------------------------------------------------

void DynamicTexture::setEnableRTT( bool enable /* = true */ )
{
   mRTT = enable;

   if( enable && mTempDynamicTexture == NULL )
      mTempDynamicTexture = new DynamicTexture( mUpdateRect );
}

//------------------------------------------------------------------------------

void DynamicTexture::update()
{
   if( !mHasUpdateRect )
      return;

   RectI *rect = &mUpdateRect;

   // Different-rect updating provided it is the same size
   if( 0 )
   {
      /// rect = something differing
      AssertFatal( rect->extent.x <= mUpdateRect.extent.x &&
                   rect->extent.y <= mUpdateRect.extent.x, "Cannot update a DynamicTexture with an update area larger than it's allocated area." );
   }


   // Do the screen copy
   glReadBuffer( GL_BACK );
   glBindTexture( GL_TEXTURE_2D, mTextureHandle->getGLName() );
   glCopyTexSubImage2D( GL_TEXTURE_2D, 0,          // 0 = no mipmaplevels
                        0, 0,                      // X and y offsets, see docs
                        rect->point.x, rect->point.y,
                        rect->extent.x, rect->extent.y );
}

//------------------------------------------------------------------------------

void DynamicTexture::setUpdateRect( const RectI &newRect )
{
   mHasUpdateRect = true;

   // Check to see if this is being called as part of the constructor
   if( mTextureHandle == NULL )
   {
      GBitmap *bmp = new GBitmap( newRect.extent.x, newRect.extent.y, false, GBitmap::RGBA );
      mTextureHandle = new TextureHandle( NULL, bmp, BitmapKeepTexture, true );
      mSize = newRect.extent;
   }

   if( mTempDynamicTexture != NULL )
      mTempDynamicTexture->setUpdateRect( newRect );

   // Flip from upper-left point to lower-left point (for GL)
   RectI updateRect = newRect;
   updateRect.point.y = Platform::getWindowSize().y - updateRect.point.y - updateRect.extent.y;

   if( updateRect.extent == mUpdateRect.extent )
   {
      if( updateRect.point != mUpdateRect.point )
         mUpdateRect.point = updateRect.point;
   }
   else
   {
      TextureObject *to = mTextureHandle->object;
      mUpdateRect = updateRect;

      // If the new texture size can fit in the existing allocated size,
      // then we are golden, otherwise we have to re-create or we'll get
      // hosed.
      if( mTextureHandle->getDownloadedWidth() >= mUpdateRect.extent.x  &&
          mTextureHandle->getDownloadedHeight() >= mUpdateRect.extent.y )
      {

         // We don't have to resize the actual memory (yay)
         to->bitmapWidth = mUpdateRect.extent.x;
         to->bitmapHeight = mUpdateRect.extent.y;
      }
      else
      {
         // Damn, we gotta reallocate
         delete mTextureHandle;
         mTextureHandle = NULL;

         GBitmap *bmp = new GBitmap( updateRect.extent.x, updateRect.extent.y, false, GBitmap::RGBA );
         mTextureHandle = new TextureHandle( NULL, bmp, BitmapKeepTexture, true );
         mSize = updateRect.extent;
      }
   }

   // DON'T USE mUpdateRect here
   // Note that this looks a bit funny, this is because grabbing pixels results in
   // the rows getting flipped, to adjust for this, the texture coordinates must be flipped
   TextureObject *obj = (TextureObject *)mTextureHandle;
   F32 maxX  = F32(newRect.extent.x) / F32(obj->texWidth);
   F32 maxY = F32(newRect.extent.y) / F32(obj->texHeight);
   mTextureCoords = RectF( (F32)newRect.point.x, maxY, maxX, (F32)newRect.point.y );
}

//------------------------------------------------------------------------------

void DynamicTexture::renderGuiControl( GuiControl *ctrl /* = NULL */, bool rttMode /* = false  */ )
{
   if( ctrl == NULL )
   {
      if( mGuiControl == NULL )
         return;
      else
         ctrl = mGuiControl;
   }
   else
      mGuiControl = ctrl;

   setUpdateRect( ctrl->mBounds ); // This CHANGES mUpdateRect

   // If we are simulating rendering to offscreen type thing
   // grab the current contents of the framebuffer and store them
   if( rttMode )
   {
      setEnableRTT( true );
      storePixels();
   }

   // Now render the control and grab the pixels
   dglSetClipRect( ctrl->mBounds );
   //ctrl->preRender();
   ctrl->onRender( ctrl->mBounds.point, ctrl->mBounds );

   update();

   // Now restore the contents of the framebuffer so it doesn't look like we did
   // anything at all (bwahahaha, Trogdor strikes again)
   if( rttMode )
   {
      dglSetClipRect( ctrl->mBounds );
      restorePixels();
   }
}

//------------------------------------------------------------------------------

void DynamicTexture::storePixels()
{
   mTempDynamicTexture->update();
}

//------------------------------------------------------------------------------

void DynamicTexture::restorePixels()
{
   dglClearBitmapModulation();
   dglDrawBitmapStretch( mTempDynamicTexture->getTextureHandle(),  mUpdateRect, GFlip_Y );
}

//------------------------------------------------------------------------------

void DynamicTexture::registerForGuiUpdates( GuiControl *control )
{
   if( control == NULL )
      return;

   unregisterForGuiUpdates();

   // Sign it up in the vector
   smRegisteredGuiUpdaters.push_back( this );

   mGuiControl = control;
}

//------------------------------------------------------------------------------

bool DynamicTexture::unregisterForGuiUpdates()
{
   for( RegisteredUpdateItr i = smRegisteredGuiUpdaters.begin();
        i != smRegisteredGuiUpdaters.end(); i++ )
   {
      if( (*i) == this )
      {
         smRegisteredGuiUpdaters.erase( i );
         return true;
      }
   }

   return false;
}

//------------------------------------------------------------------------------

void DynamicTexture::updateGuiTextures()
{
   for( RegisteredUpdateItr i = smRegisteredGuiUpdaters.begin();
        i != smRegisteredGuiUpdaters.end(); i++ )
   {
      (*i)->renderGuiControl( (*i)->mGuiControl );
   }
}

//------------------------------------------------------------------------------

void DynamicTexture::registerForScreenUpdates()
{
   unregisterForScreenUpdates();

   // Sign it up in the vector
   smRegisteredScreenUpdaters.push_back( this );
}

//------------------------------------------------------------------------------

bool DynamicTexture::unregisterForScreenUpdates()
{
   for( RegisteredUpdateItr i = smRegisteredScreenUpdaters.begin();
      i != smRegisteredScreenUpdaters.end(); i++ )
   {
      if( (*i) == this )
      {
         smRegisteredScreenUpdaters.erase( i );
         return true;
      }
   }

   return false;
}

//------------------------------------------------------------------------------

void DynamicTexture::updateScreenTextures()
{
   for( RegisteredUpdateItr i = smRegisteredScreenUpdaters.begin();
      i != smRegisteredScreenUpdaters.end(); i++ )
   {
      (*i)->update();
   }
}

//------------------------------------------------------------------------------

void DynamicTexture::updateEndOfFrameTextures()
{
   for( RegisteredUpdateItr i = smUpdateAtEndOfFrame.begin();
      i != smUpdateAtEndOfFrame.end(); i++ )
   {
      (*i)->update();
   }

   smUpdateAtEndOfFrame.clear();
}