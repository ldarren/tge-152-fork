//------------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.Com, Inc.
//------------------------------------------------------------------------------

#ifndef _GDYNAMICTEXTURE_H_
#define _GDYNAMICTEXTURE_H_

#include "dgl/gTexManager.h"
#include "math/mRect.h"
#include "core/tVector.h"

// Forward definitions
class GuiControl;

extern void dynamicTextureCB(const U32 eventCode, void *userData);

class DynamicTexture
{
   friend void dynamicTextureCB(const U32 eventCode, void *userData);
private:
   TextureHandle *mTextureHandle;   ///< TextureHandle
   Point2I mSize;
   S32 mTexCBHandle;
   RectI mUpdateRect;               ///< When you call update() this is the rect, in screen coordinates, that gets grabbed
   RectF mTextureCoords;            ///< This is the texturecoord information you need to know to create texturecoords
                                    ///< if you are not using dglDrawBitmap
   bool mRTT;                       ///< Supports RTT emulation for Gui @see renderGuiControl
   bool mHasUpdateRect;             ///< Lets this have a default constructor

   GuiControl *mGuiControl;         ///< If the object is registered as wanting a Gui update every frame, this is
                                    ///< the variable that contains the Gui control it will render

   DynamicTexture *mTempDynamicTexture; ///< For RTT emulation

   typedef Vector<DynamicTexture *>::iterator RegisteredUpdateItr;
   static Vector<DynamicTexture *> smRegisteredGuiUpdaters; ///< This is a vector of DynamicTexture objects that
                                                            ///< will be updated every frame with their mGuiControl
                                                            ///< rendered to them.
   static Vector<DynamicTexture *> smRegisteredScreenUpdaters;

   static Vector<DynamicTexture *> smUpdateAtEndOfFrame;

   /// Helper function for the constructors
   void initDT();

public:
   /// Default Constructor
   DynamicTexture();

   /// Constructor
   ///
   /// @param  updateRect    The screen-rect this will grab from the color-buffer when update() is called
   DynamicTexture( const RectI &updateRect );

   /// Gui updater Constructor
   ///
   /// @param  control       Gui control to render to this texture every frame
   DynamicTexture( GuiControl *control );

   /// Destructor
   ~DynamicTexture();

   /// Grabs the rectangle defined by mUpdateRect from the color-buffer
   /// and sticks it in the texture
   void update();

   /// @defgroup dyntex_guiupdate Automatic Gui Updates
   /// This collection of functions is used to make textures which automatically
   /// update every frame based on the content of a GuiControl
   /// @{

   /// This registers this DynamicTexture to be updated every frame with the contents
   /// of a GuiControl
   ///
   /// @param  control     Gui control to render to this texture every frame
   void registerForGuiUpdates( GuiControl *control );

   /// This will unregister this DynamicTexture from per-frame Gui updates
   ///
   /// @returns True if it removed this from updating
   bool unregisterForGuiUpdates();

   /// This method is called in GuiCanvas before the screen-rendering takes place
   /// to update all of the registered GUI update textures
   static void updateGuiTextures();

   /// @}

   /// @defgroup dyntex_screenupdate Automatic Screen Updates
   /// This collection of functions is used to make tuextures which automatically
   /// update every frame based on a absolute set of screen coordinates
   /// @{

   /// This will register this DynamicTexture to be updated every frame
   void registerForScreenUpdates();

   /// This will unregister this control from screen updates
   ///
   /// @returns True if it removed this from updating
   bool unregisterForScreenUpdates();

   /// This method is called in GuiCanvas right before the buffers are swapped
   static void updateScreenTextures();

   /// This method just registers this texture to be updated at the end of the
   /// current frame
   void updateAtEndOfFrame();

   /// This method will update all the textures that asked to be rendered at the
   /// end of the current frame
   static void updateEndOfFrameTextures();

   /// @}

   /// This lets you change the rect that this texture grabs from the screen
   /// This is not a cheap operation, you don't want to do it unless
   /// you have a good reason to.
   ///
   /// @param   newUpdatePoint   The new rect, in screen coordinates, to use for the update rect
   void setUpdateRect( const RectI &newRect );

   /// @defgroup dyntex_rtt Render To Texture Simulation
   /// Render To Texture is really great because it allows you to draw to offscreen
   /// buffers and not muck around with copying pixels etc. Unfortunatly there is
   /// no way to do this cross-platform (that I know of) in OpenGL 1.5 so this
   /// is the work around. When the time comes to draw something to a texture
   /// we will copy the current contents of that area of the color buffer into
   /// a temporary DynamicTexture, draw (to that area), grab those pixels, then
   /// restore the contents of the color buffer to what it was before we drew
   /// to it. This method is NOT necesasary for every application of DynamicTexture.
   /// In fact, most applications will not need it at all.
   /// @{

   /// Manually enable or disable RTT mode
   void setEnableRTT( bool enable = true );

   /// Stores the pixels defined by the updateRect into a temporary
   /// DynamicTexture object so that they can be restored at a later
   /// time.
   void storePixels();

   /// Restores the pixels in the temporary DynamicTexture object
   /// by doing a dglDrawBitmap with them and drawing them, stretched
   /// to the updateRect.
   void restorePixels();

   /// @}

   /// Returns the texture handle for this so it can be used to render
   ///
   /// @return Texture handle for the dynamic texture
   TextureHandle getTextureHandle() const;

   /// Returns a const pointer to the texture coordinate information
   ///
   /// @return Texture coordinate information
   const RectF * const texCoordInfo() const;


   /// Returns the update rect of this dynamic texture
   ///
   /// @return Update rect for the dynamic texture
   const RectI * const getUpdateRect() const;


   /// Returns the GuiControl this texture is using, if any
   GuiControl *getGuiControl() const;

   /// Renders a GuiControl, and it's children, to the texture
   /// This has two different behaviors depending on if RTT emulation
   /// is specified as enabled.
   ///
   /// In OpenGL, rendering to an off screen
   /// buffer is a platform-specific operation and I don't have a Mac
   /// to make sure this works and what have you, so what happens is,
   /// if RTT mode is turned on, the area that will be used to draw
   /// the texture will be saved, with the temp DyanmicTexture object
   /// then the rendering will take place, it will grab the texture,
   /// then restore the edited area of the color-buffer.
   ///
   /// If RTT is turned off, then it will draw the control on the screen,
   /// grab it, and leave it. This is not as useless as it seems, this is
   /// not at all a bad way to do things, just do it all in the GuiCanvas
   /// before it begins rendering the content control
   /// @see GuiCanvas::renderFrame
   ///
   /// @param   ctrl   GuiControl to render
   void renderGuiControl( GuiControl *ctrl = NULL, bool rttMode = false );
};


//------------------------------------------------------------------------------

inline TextureHandle DynamicTexture::getTextureHandle() const
{
   return *mTextureHandle;
}

inline const RectF * const DynamicTexture::texCoordInfo() const
{
   return &mTextureCoords;
}

inline GuiControl *DynamicTexture::getGuiControl() const
{
   return mGuiControl;
}

inline const RectI * const DynamicTexture::getUpdateRect() const
{
   return &mUpdateRect;
}

inline void DynamicTexture::updateAtEndOfFrame()
{
   smUpdateAtEndOfFrame.push_back( this );
}

inline void DynamicTexture::initDT()
{
   mTexCBHandle = TextureManager::registerEventCallback(dynamicTextureCB, this);
   mTextureHandle = NULL;
   mRTT = false;
   mHasUpdateRect = false;
   mGuiControl = NULL;
   mTempDynamicTexture = NULL;
}

//------------------------------------------------------------------------------

#endif