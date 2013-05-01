#include "gui/shiny/guiEffectCanvas.h"
#include "dgl/dgl.h"
#include "util/safeDelete.h"

IMPLEMENT_CONOBJECT(GuiEffectCanvas);

//------------------------------------------------------------------------------

// An effect
namespace EffectCanvasEffects
{

void FN_CDECL spiralInitFn( const int x, const int y, const Point2I &resolution,
                           const F32 maxX, const F32 maxY, Point2F *outVec )
{
   outVec->x += 0.01;

   //outVec->x -= 0.5f;
   //outVec->y -= 0.5f;

   //F32 d = mSqrt( ( x - resolution.x / 2.f ) * ( x - resolution.x / 2.f ) +
   //   ( y - resolution.y / 2.f ) * ( y - resolution.y / 2.f ) );
   //F32 c = mCos( d * M_PI / 64.f ) * 0.5f;
   //F32 s = mSin( d * M_PI / 64.f ) * 0.5f;
   //outVec->x = c * outVec->x - s * outVec->y;
   //outVec->y = s * outVec->x + c * outVec->y;

   //outVec->x += 0.5f;
   //outVec->y += 0.5f;
}

};

//------------------------------------------------------------------------------

ConsoleFunction( createEffectCanvas, bool, 2, 2, "(string windowTitle)"
                "Create the game window/canvas, with the specified window title.")
{
   AssertISV(!Canvas, "createEffectCanvas: canvas has already been instantiated");

#if !defined(TORQUE_OS_MAC) // macs can only run one instance in general.
#if !defined(TORQUE_DEBUG) && !defined(INTERNAL_RELEASE)
   if(!Platform::excludeOtherInstances("TorqueTest"))
      return false;
#endif
#endif
   Platform::initWindow(Point2I(800, 600), argv[1]);

   // create the canvas, and add it to the manager
   Canvas = new GuiEffectCanvas();
   Canvas->registerObject("Canvas"); // automatically adds to GuiGroup
   return true;
}

//------------------------------------------------------------------------------

GuiEffectCanvas::GuiEffectCanvas()
{
   mStartEffect = false;
   mEffectInProgress = false;
   mVisualizeField = false;
   mUpdateFeedbackTexture = false;

   mVectorField = new VectorField( Point2I( 16, 12 ) );

   mClearColor.set( 1.f, 0.f, 0.f, 1.0f );

   mLastSize = mBounds.extent;
}

//------------------------------------------------------------------------------

GuiEffectCanvas::~GuiEffectCanvas()
{
   SAFE_DELETE( mVectorField );
}

//------------------------------------------------------------------------------

void GuiEffectCanvas::renderFrame( bool preRenderOnly, bool bufferSwap /* = true */ )
{
   // With this canvas, always re-draw the whole thing if an effect is in progress
   if( mEffectInProgress )
   {
      resetUpdateRegions();
   }

   // Render normally.
   Parent::renderFrame( preRenderOnly, false );

   // Check for resize (renderFrame does this)
   if( Platform::getWindowSize() != mLastSize )
   {
      canvasResized();
      mLastSize = Platform::getWindowSize();
   }

   // Check to see if the effect should be started
   if( mStartEffect )
   {
      // Sweet, don't do this again until next time
      mStartEffect = false;

      // Grab the frame
      mFeedbackTexture.update();

      // And we are in business!
      mEffectInProgress = true;
   }

   // Check to see if we are going
   if( mEffectInProgress )
   {
      // Do some vooodooo!
      glDisable( GL_LIGHTING );
      glEnable( GL_TEXTURE_2D );
      glEnable( GL_BLEND );

      // Bind the feedback texture
      glBindTexture( GL_TEXTURE_2D, mFeedbackTexture.getTextureHandle().getGLName() );

      // Set up some tex parameters
      glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
      glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (F32 *)&mClearColor );

      // Set up the blend parameters
      glBlendColorEXT( mClearColor.red, mClearColor.green, mClearColor.blue, mClearColor.alpha );
      glBlendFunc( GL_CONSTANT_COLOR_EXT, GL_ZERO );

      glGetFloatv( GL_BLEND_COLOR_EXT, (F32 *)&mClearColor );

      // Render the current field
      mVectorField->renderField( true );

      glDisable( GL_TEXTURE_2D );
   }

   // Check to see if we should update the feedback texture
   if( mUpdateFeedbackTexture )
   {
      mFeedbackTexture.update();
      mUpdateFeedbackTexture = false;
   }

   // Debug visualization
   if( mVisualizeField )
      mVectorField->visualizeField();

   // Now swap the buffers
   swapBuffers();
}

//------------------------------------------------------------------------------

void GuiEffectCanvas::processTick()
{
   if( mEffectInProgress )
   {
      // Check to see if we are done
      if( mEffectTickCount++ > 100 )
         mEffectInProgress = false;
      else
         mUpdateFeedbackTexture = true;
   }
}

//------------------------------------------------------------------------------

void GuiEffectCanvas::canvasResized()
{
   // Do NOT call parent here
   mFeedbackTexture.setUpdateRect( mBounds );

   TextureObject *obj = (TextureObject *)mFeedbackTexture.getTextureHandle();
   F32 maxX  = F32(obj->bitmapWidth) / F32(obj->texWidth);
   F32 maxY = F32(obj->bitmapHeight) / F32(obj->texHeight);

   mVectorField->initVectorField( maxX, maxY, VectorField::Flip_None, &EffectCanvasEffects::spiralInitFn );
}

//------------------------------------------------------------------------------

void GuiEffectCanvas::setContentControl(GuiControl *gui)
{
   Parent::setContentControl( gui );
   //mStartEffect = true;
   mEffectTickCount = 0;
}

//------------------------------------------------------------------------------