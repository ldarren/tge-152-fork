//-----------------------------------------------------------------------------
// Justin DuJardin
// Gui Tab Page Control Class
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "console/console.h"
#include "dgl/dgl.h"
#include "gui/core/guiCanvas.h"
#include "gui/controls/guiTabPageCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/editor/guiEditCtrl.h"

IMPLEMENT_CONOBJECT(GuiTabPageCtrl);

GuiTabPageCtrl::GuiTabPageCtrl(void)
{
   mBounds.extent.set(100, 200);
   mMinSize.set(50, 50);
   dStrcpy(mText,(UTF8*)"TabPage");
   mActive = true;
}

void GuiTabPageCtrl::initPersistFields()
{
   Parent::initPersistFields();
}

bool GuiTabPageCtrl::onWake()
{
   if (! Parent::onWake())
      return false;

   return true;
}

void GuiTabPageCtrl::onSleep()
{
   Parent::onSleep();
}

GuiControl* GuiTabPageCtrl::findHitControl(const Point2I &pt, S32 initialLayer)
{
   return Parent::findHitControl(pt, initialLayer);
}

void GuiTabPageCtrl::onMouseDown(const GuiEvent &event)
{
   setUpdate();
   Point2I localPoint = globalToLocalCoord( event.mousePoint );

   GuiControl *ctrl = findHitControl(localPoint);
   if (ctrl && ctrl != this)
   {
      ctrl->onMouseDown(event);
   }
}

bool GuiTabPageCtrl::onMouseDownEditor(const GuiEvent &event, Point2I offset )
{
   // This shouldn't be called if it's not design time, but check just incase
   if ( GuiControl::smDesignTime )
   {
      GuiEditCtrl* edit = GuiControl::smEditorHandle;
      if( edit )
         edit->select( this );
   }

   return Parent::onMouseDownEditor( event, offset );
}


GuiControl *GuiTabPageCtrl::findNextTabable(GuiControl *curResponder, bool firstCall)
{
   //set the global if this is the first call (directly from the canvas)
   if (firstCall)
   {
      GuiControl::smCurResponder = NULL;
   }

   //if the window does not already contain the first responder, return false
   //ie.  Can't tab into or out of a window
   if (! ControlIsChild(curResponder))
   {
      return NULL;
   }

   //loop through, checking each child to see if it is the one that follows the firstResponder
   GuiControl *tabCtrl = NULL;
   iterator i;
   for (i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      tabCtrl = ctrl->findNextTabable(curResponder, false);
      if (tabCtrl) break;
   }

   //to ensure the tab cycles within the current window...
   if (! tabCtrl)
   {
      tabCtrl = findFirstTabable();
   }

   mFirstResponder = tabCtrl;
   return tabCtrl;
}

GuiControl *GuiTabPageCtrl::findPrevTabable(GuiControl *curResponder, bool firstCall)
{
   if (firstCall)
   {
      GuiControl::smPrevResponder = NULL;
   }

   //if the window does not already contain the first responder, return false
   //ie.  Can't tab into or out of a window
   if (! ControlIsChild(curResponder))
   {
      return NULL;
   }

   //loop through, checking each child to see if it is the one that follows the firstResponder
   GuiControl *tabCtrl = NULL;
   iterator i;
   for (i = begin(); i != end(); i++)
   {
      GuiControl *ctrl = static_cast<GuiControl *>(*i);
      tabCtrl = ctrl->findPrevTabable(curResponder, false);
      if (tabCtrl) break;
   }

   //to ensure the tab cycles within the current window...
   if (! tabCtrl)
   {
      tabCtrl = findLastTabable();
   }

   mFirstResponder = tabCtrl;
   return tabCtrl;
}

bool GuiTabPageCtrl::onKeyDown(const GuiEvent &event)
{

   if ((event.keyCode == KEY_TAB) && (event.modifier & SI_CTRL))
   {
      //find the next sibling window, and select it
      GuiControl *parent = getParent();
      if (parent)
      {
         GuiTabPageCtrl *firstWindow = NULL;
         iterator i;
         for (i = parent->begin(); i != parent->end(); i++)
         {
            GuiTabPageCtrl *ctrl = dynamic_cast<GuiTabPageCtrl *>(*i);
            if (ctrl && ctrl->getTabIndex() == mTabIndex + 1)
            {
               ctrl->selectWindow();
               return true;
            }
            else if (ctrl && ctrl->getTabIndex() == 0)
            {
               firstWindow = ctrl;
            }
         }
         //recycle from the beginning
         if (firstWindow != this)
         {
            firstWindow->selectWindow();
            return true;
         }
      }
   }

   return Parent::onKeyDown(event);
}

void GuiTabPageCtrl::setText(const char *txt)
{
   Parent::setText( txt );

   GuiControl *parent = getParent();
   if( parent )
      parent->setUpdate();
};


void GuiTabPageCtrl::selectWindow(void)
{
   //first make sure this window is the front most of its siblings
   GuiControl *parent = getParent();
   if (parent)
   {
      parent->pushObjectToBack(this);
   }

   //also set the first responder to be the one within this window
   setFirstResponder(mFirstResponder);
}

void GuiTabPageCtrl::onRender(Point2I offset,const RectI &updateRect)
{
   GuiControl::onRender( offset, updateRect );
}
