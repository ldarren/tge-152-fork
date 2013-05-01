
#ifndef _rpgTSCtrl_h
#define _rpgTSCtrl_h


//here resides the RPG's main TS ctrl.. supports navigating, selecting items
//in the world


//Notes

//1) In the default demo you need to remove the Shapename gui etc so that mouse clicks pass thru to this
//control

//2) The datablock needs to be set to something other than GuiContentProfile .. 
//as we need canKeyFocus == true .. MissionEditorProfile will do for now.. unless that gets unwanted
//behavior in the future

//3) The client/guichathud also gives a problem ... setting the main type to NoMouseControl allows click
//thrus... unfortunately it also removes the chat hud


#include "dgl/dgl.h"
#include "game/game.h"
#include "gui/core/guiTSControl.h"
#include "gui/core/guiCanvas.h"


struct gm3DMouseEvent : public GuiEvent
{
   Point3F     vec;
   Point3F     pos;
};

class SceneObject;

struct gm3DMouseCollisionInfo
      {
         SceneObject*     obj;
         Point3F          pos;
         VectorF          normal;
      };




class rpgTSCtrl : public GuiTSCtrl
{
private:
   typedef GuiTSCtrl Parent;

public:
   rpgTSCtrl();
   ~rpgTSCtrl();
   
   bool processCameraQuery(CameraQuery *query);
   void renderWorld(const RectI &updateRect);

   //mouse events
   void onRightMouseDown(const GuiEvent &event);
   void onRightMouseUp(const GuiEvent &event);
   bool onInputEvent(const InputEvent & event);

   //runs selection code
   void onMouseUp(const GuiEvent & event);

   bool onMouseWheelUp(const GuiEvent &event);
   bool onMouseWheelDown(const GuiEvent &event);
   

   void onLoseFirstResponder();


   void onRender(Point2I offset, const RectI &updateRect);

   //most likely moved
   Point3F    smCamPos;
   EulerF     smCamRot;
   MatrixF    smCamMatrix;
   F32        smVisibleDistance;
   gm3DMouseEvent mLastEvent;

   //3D mouse
   void on3DMouseDown(const gm3DMouseEvent & event);
   void make3DMouseEvent(gm3DMouseEvent & gui3DMouseEvent, const GuiEvent & event);

   static void consoleInit();
 
   DECLARE_CONOBJECT(rpgTSCtrl);
};

#endif //_rpgTSCtrl_h
