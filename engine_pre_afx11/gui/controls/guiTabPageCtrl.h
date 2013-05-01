//-----------------------------------------------------------------------------
// Justin DuJardin
// Gui Tab Page Control Class
//-----------------------------------------------------------------------------

#ifndef _GUITABPAGECTRL_H_
#define _GUITABPAGECTRL_H_

#ifndef _GUITEXTCTRL_H_
#include "gui/controls/guiTextCtrl.h"
#endif

class GuiTabPageCtrl : public GuiTextCtrl
{
   private:
      typedef GuiTextCtrl Parent;

      Point2I        mMinSize;
      S32            mTabIndex;

   public:
      GuiTabPageCtrl();
      DECLARE_CONOBJECT(GuiTabPageCtrl);
      static void initPersistFields();

      bool onWake();    ///< The page awakens (becomes active)!
      void onSleep();   ///< The page sleeps (zzzzZZ - becomes inactive)

      GuiControl* findHitControl(const Point2I &pt, S32 initialLayer = -1); ///< Find which control is hit by the mouse starting at a specified layer

      void onMouseDown(const GuiEvent &event);  ///< Called when a mouseDown event occurs
      bool onMouseDownEditor(const GuiEvent &event, Point2I offset );   ///< Called when a mouseDown event occurs and the GUI editor is active

      S32 getTabIndex(void) { return mTabIndex; }  ///< Get the tab index of this control

      //only cycle tabs through the current window, so overwrite the method
      GuiControl* findNextTabable(GuiControl *curResponder, bool firstCall = true);
      GuiControl* findPrevTabable(GuiControl *curResponder, bool firstCall = true);

      bool onKeyDown(const GuiEvent &event); ///< Called when a keyDown event occurs

      void selectWindow(void);               ///< Select this window

      virtual void setText(const char *txt = NULL); ///< Override setText function to signal parent we need to update.

      void onRender(Point2I offset, const RectI &updateRect);  ///< Called when it's time to render this page to the scene
};

#endif //_GUI_WINDOW_CTRL_H
