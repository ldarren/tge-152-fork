//------------------------------------------------------------------------------
// Gui Transition Control
//------------------------------------------------------------------------------

#ifndef _GUI_VECTORFIELDCTRL_H_
#define _GUI_VECTORFIELDCTRL_H_

#include "gui/shiny/guiTickCtrl.h"
#include "dgl/gVectorField.h"
#include "dgl/gDynamicTexture.h"

class GuiVectorFieldCtrl : public GuiTickCtrl
{
   typedef GuiTickCtrl Parent;

private:
   VectorField *mVectorField;
   DynamicTexture mFeedbackTexture;
   ColorF mClearColor;
   TextureHandle *mTestTexture;

protected:
   // No interpolation on this
   virtual void interpolateTick( F32 delta ) {};

   // Don't care if time passes
   virtual void advanceTime( F32 timeDelta ) {};

   // I do care about this
   virtual void processTick();
   
   // do allocation and init of members here.
   virtual bool onAdd(void);
   // do deallocation of members here.
   virtual void onRemove(void);

public:
   GuiVectorFieldCtrl();
   ~GuiVectorFieldCtrl();

   virtual void resize( const Point2I &newPosition, const Point2I &newExtent );
   virtual void onRender( Point2I offset, const RectI &updateRect );

   DECLARE_CONOBJECT( GuiVectorFieldCtrl );

};

#endif