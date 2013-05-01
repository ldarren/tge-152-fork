/*  mkRenderTexture.h
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */
 
#ifndef _MKRENDERTEXTURE_H_
#define _MKRENDERTEXTURE_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif

class GBitmap;
class _RenderTextureManager;

/// @defgroup rtcreate Render Texture Creation
/// Right, this is going to seem like an odd process for creating RenderTextures, but it makes perfect sense.
/// 
/// First, you must create a RenderTextureFormat with your preferred features.  32 bit color, 24 bit depth, antialiasing with 4 samples
/// etc.  Then you submit this RenderTexture format to _RenderTextureManager::getClosestMatch() which will return a RenderTextureFormat
/// containing the closest set of features to your request that the user's GPU supports.  From here, you can either create a RenderTexture
/// and pass in the best fit format, or choose not to create a RenderTexture if the user's GPU doesn't support some critical feature
/// e.g. you just have to have a  multisampled floating point color buffer.
///
/// Seems like a rather contrived process, no?  Here's the deal.  _RenderTextureManager is meant to handle pbuffers and windows as well
/// and the process described above is essentially that of creating a window in OpenGL.  This process also gives us a chance to
/// generate a format the card can support without you, the programmer, having to manually check features, and also lets you, the programmer,
/// know what you're getting when you create a RenderTexture.  So, if you were to try and create a RenderTexture doing 16x CSAA on a GeForce FX
/// 5200, _RenderTextureManager will kindly inform you that the best you're getting is 4x MSAA.
/// @{

/// RTColorFormat is an enum containing all of the valid color formats for RenderTextures.  At the moment
/// it only contains the standard 16 and 32 bit formats, but it may be later extended to 64 bit and 128 bit
/// floating point, or sRGB formats.
enum RTColorFormat
{
   RGBA0 = 0,
   RGBA8 = 32,
   RGB5_A1 = 16
};

/// RTDepthFormat is an enum containing all of the valid depth formats for RenderTextures.  At the moment
/// it only contains the standard 16, 24, and 32 bit formats (current cards only support 16/24 bit, divided
/// between ATI and nVidia).  It may later be extended to the new 32 bit floating point format supported
/// by the GeForce 8800.
enum RTDepthFormat
{
   Depth0 = 0,
   Depth16 = 16,
   Depth24 = 24,
   Depth32 = 32
};

/// RTAntiAliasing is an enum containg all of the valid antialiasing types for RenderTextures.  At the moment
/// it only contains MSAA and SSAA (Multisampling and supersampling).  It may later be extended to support CSAA
/// (coverage sampling) offered on the GeForce 8800.
enum RTAntiAliasing
{
   None = 0,
   MSAA = 1,
   SSAA = 2
};

/// RTStencilFormat is an enum containg all of the valid stencil formats for RenderTextures.
enum RTStencilFormat
{
   Stencil0 = 0,
   Stencil1 = 1,
   Stencil4 = 4,
   Stencil8 = 8,
   Stencil16 = 16
};

/// The RenderTextureFormat struct contains information on what a RenderTexture should be capable of.
/// What color format (16 bit or 32 (or 64 or 128)) should we use?  What should the depth buffer precision be?
/// Should we multisample?  How many samples?  FBO only, or can we allow a pbuffer fallback?
/// @note If mIsPbuffer is true, creating a RenderTexture from this format will result in pbuffer usage!
struct RenderTextureFormat
{
   RTColorFormat     mColorFormat;
   RTDepthFormat     mDepthFormat;
   RTAntiAliasing    mAntiAliasingFormat;
   RTStencilFormat   mStencilFormat;
   
   U32 mNumAASamples;
   bool mAllowPbuffer;
   bool mIsPbuffer;
   
   bool mMip;
   
   RenderTextureFormat(RTColorFormat _cf, RTDepthFormat _df, RTAntiAliasing _aa, RTStencilFormat _sf, U32 _aas, bool _pb, bool _mip);
   RenderTextureFormat();
};

/// @}

/// This is where render to texture magic happens.
/// In the event that framebuffers are not supported, or
/// the specified RenderTextureFormat cannot be used with FBOs,
/// it falls back to pbuffers.
/// 
/// Anyhow, moving on, this is designed around the immutable object system of OpenGL 3.0
/// Once a RenderTexture is created it may not be modified in any way.
/// This ensures that no one accidentally breaks it and then you've
/// got an incomplete FBO floating around producing corrupt data 
/// and explosions.
///
/// All render textures must be created through the RenderTextureManager
/// Otherwise, the world explodes.
/// Or your application crashes when the resolution is changed
/// but that's just as serious, right?
///
/// As previously stated, once created a RenderTexture cannot be modified.
/// If you find that you need a larger RenderTexture, you must delete the old one
/// and create a new one with your prefered size.
class RenderTexture
{
   friend class _RenderTextureManager;
   
protected:
   RenderTexture();
   virtual ~RenderTexture();
   
   /// Initializes the RenderTexture, and creates internal GL objects
   ///
   /// @param width        The width of the RenderTexture (Pow2 only for now!)
   /// @param height       The height of the RenderTexture (Pow2 only for now!)
   /// @param format       The format we should use.
   virtual void init(U32 width, U32 height, RenderTextureFormat format) = 0;
   
   /// Render to us
   virtual void makeActiveTarget() = 0;
   
   /// Done rendering
   virtual void finish() = 0;
   
public:
   /// Bind the texture to the specified sampler so we can draw using it
   ///
   /// @param samplerNum         glActiveTexture(GL_TEXTURE0 + samplerNum)
   virtual void bindToUse(U32 samplerNum);
   
   /// Returns true if the RenderTexture has been initialized
   virtual bool isInitialized() { return mInit; };
   
   /// Download us to the CPU and store us in a bitmap
   /// Primarily used in normalmap generation by mkInterior.
   virtual GBitmap* download();
   
   /// Copies the backbuffer into the internal gl texture object
   /// !!!WILL ASSERT IF BACKBUFFER IS LARGER THAN TEXTURE!!! WILL NOT RESIZE!!!
   virtual void copyBackBuffer();
   
   
protected:

   /// Deletes all context sensitive information (i.e. all internal GL objects)
   virtual void reset() = 0;
   
   /// Restores all context sensitive information
   virtual void resurrect();
   
   /// Clears everything
   virtual void kill();
   
   bool mInit; ///< If this if false, bad things happen.
   
   bool mMip; ///< Do we want to generate mipmaps?
   
   U32 mTextureId; ///< Our internal texture
   
   U32 mWidth; ///< Width
   
   U32 mHeight; ///< Can you guess what this does?
   
   bool mDepth; ///< Are we rendering depth?
   
   RenderTextureFormat mFormat; ///< Our texture format
   
public:
   U32 getWidth() { return mWidth; };
   U32 getHeight() { return mHeight; };
};

class FBORenderTexture : public RenderTexture
{
   friend class _RenderTextureManager;
      
   FBORenderTexture();
   virtual ~FBORenderTexture();
   
   /// Initializes the RenderTexture, and creates internal GL objects
   ///
   /// @param width        The width of the RenderTexture (Pow2 only for now!)
   /// @param height       The height of the RenderTexture (Pow2 only for now!)
   /// @param format       The format we should use.
   virtual void init(U32 width, U32 height, RenderTextureFormat format);
   
   /// Render to us
   virtual void makeActiveTarget();
   
   /// Done rendering
   virtual void finish();
   
protected:

   /// Deletes all context sensitive information (i.e. all internal GL objects)
   virtual void reset();
   
   U32 mFBO; ///< Framebuffer object
   
   U32 mRB; ///< Renderbuffer (for depth)
};
   
   

class _RenderTextureManager
{
   public:
   
   /// ONLY ONE INSTANCE!!!
   static void create();
   static void destroy();
   
   /// Called before a context switch
   void reset();
   
   /// Called after a context switch
   void resurrect();
   
   /// Death to them all!
   void kill();
   
   /// Render to the specified RenderTexture
   /// if NULL, render to the main window
   void renderToTexture(RenderTexture* tex);
   
   /// Delete the specified RenderTexture
   /// Because we don't love it enough
   void deleteRenderTexture(RenderTexture *tex);
   
   /// Create a new RenderTexture
   ///
   /// @param width        The width of the RenderTexture (Pow2 only for now!)
   /// @param height       The height of the RenderTexture (Pow2 only for now!)
   /// @param format       The format we should use.
   RenderTexture* createRenderTexture(U32 width, U32 height, RenderTextureFormat format);
   
   /// output is the closest possible match to the given RenderTextureFormat which will work on the users GPU.
   void getClosestMatch(RenderTextureFormat &format, RenderTextureFormat &output);
   
   /// Returns the current RenderTexture (NULL if we are rendering to the main window)
   RenderTexture* getCurrentRenderTexture() { return mCurRenderTexture; };
   
   private:
   
   /// Currently active render texture
   RenderTexture *mCurRenderTexture;
   
   _RenderTextureManager();
   ~_RenderTextureManager();
   
   /// All of our registered RenderTextures.
   Vector<RenderTexture*> mRenderTextures;
};

extern _RenderTextureManager *RenderTextureManager;

#endif
   


