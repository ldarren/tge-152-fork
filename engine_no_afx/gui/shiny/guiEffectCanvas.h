#ifndef _GUIEFFECTCANVAS_H_
#define _GUIEFFECTCANVAS_H_

#include "gui/core/guiCanvas.h"
#include "core/iTickable.h"
#include "dgl/gVectorField.h"
#include "dgl/gDynamicTexture.h"

class GuiEffectCanvas : public GuiCanvas, public virtual ITickable
{
   typedef GuiCanvas Parent;

protected:
   bool mEffectInProgress;
   bool mVisualizeField;
   bool mUpdateFeedbackTexture;
   bool mStartEffect;

   U32 mEffectTickCount;

   VectorField *mVectorField;
   DynamicTexture mFeedbackTexture;
   ColorF mClearColor;

   Point2I mLastSize;

public:
   DECLARE_CONOBJECT(GuiEffectCanvas);

   GuiEffectCanvas();
   ~GuiEffectCanvas();

   // Change rendering to support the effects
   virtual void renderFrame( bool preRenderOnly, bool bufferSwap = true );

   // To adjust the vector field
   virtual void canvasResized();

   // This will start the effect!
   virtual void setContentControl(GuiControl *gui);

   // ITickable stuff
   virtual void processTick();

   virtual void interpolateTick( F32 delta ) {};
   virtual void advanceTime( F32 timeDelta ) {};
};

#endif