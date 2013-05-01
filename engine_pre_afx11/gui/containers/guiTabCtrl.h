//-----------------------------------------------------------------------------
// Gui Tab Ctrl
//
// Contributed by Philip Hardin of Red Brick Games (www.redbrickgames.com)
//-----------------------------------------------------------------------------

#ifndef _GUITABCTRL_H_
#define _GUITABCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif
#ifndef _GTEXMANAGER_H_
#include "dgl/gTexManager.h"
#endif


class GuiTabInfo
{
   public:
   TextureHandle  mBmp;
};



///////////////////////////////////////////////////////////////////////////////
// Tabbed property sheet ctrl.
///////////////////////////////////////////////////////////////////////////////
class GuiTabCtrl : public GuiControl
{
   enum {
      maxTabs = 32
   };
   //---------------------------------------------------------------------------
   // Data Members
   //---------------------------------------------------------------------------
   GuiTabInfo  mTabInfos[maxTabs];
   StringTableEntry  mTabLabelField;
   StringTableEntry  mTabIconField;
   int         mPressedTab;            // which tab is currently depressed by the mouse; -1 if none
   int         mHoveredTab;            // which tab is currently being hovered over by the mouse; -1 if none
   int         mSelectedTab;           // which tab is currently selected (showing its pane)
   Point2I     mTabSize;               // x,y size of tabs. if x is zero, tabs are scaled to width of tabbed ctrl.
   Point2I     mIconSize;              // x,y size of tab icons.  if zeros, tab icons are rendered at their inherent size.
   Point2F     mMargins;               // left & right margins.  Values >=1 are interpreted as pixel widths; values < 1 are interpreted as percentages of the width of the tab ctrl.
   int         mBevel;                 // beveling on the tab corners, in pixels.  0 = square corners.
   Point2I     mLabelOffset;           // vertical offset to the tab icon & label.
public:
   //---------------------------------------------------------------------------
   // Constructor
   //---------------------------------------------------------------------------
   GuiTabCtrl();
   //---------------------------------------------------------------------------
   // Public funcs
   //---------------------------------------------------------------------------
   static void initPersistFields();
   virtual bool onAdd();
   virtual bool onWake();
   virtual void onSleep();
   Point2I getIconSize(int i);
   RectI getTabRect(int i);
   int getTabAt(const Point2I& pt);
   virtual void onMouseDown(const GuiEvent& evt);
   virtual void onMouseUp(const GuiEvent& evt);
   virtual void onMouseMove(const GuiEvent& evt);
   virtual void onMouseLeave(const GuiEvent& evt);
   void setScriptValue(const char* val);
   const char* getScriptValue();
   virtual void onRender(Point2I offset, const RectI& updateRect);
   int getSelectedTab() {return mSelectedTab;}
   void setSelectedTab(int tab);
   GuiControl* getKid(int i);
   int getPressedTab() {return mPressedTab;}
   int getHoveredTab() {return mHoveredTab;}
protected:
   //---------------------------------------------------------------------------
   // Implementation funcs
   //---------------------------------------------------------------------------
   void getIconAndLabelRects(const RectI& r, const Point2I& iconSize, const char* label, RectI& iconRect, RectI& labelRect);
   RectI getTabBitmapRect(int i);
   void drawTab(int i, const ColorI& lite, const ColorI& dark, int thick);
   void drawTabBackground(const RectI& r, const ColorI& fill, const ColorI& lite, const ColorI& dark, int width = 1);
   void drawPane(const RectI& rect, const ColorI& fill, const ColorI& lite, const ColorI& dark, int width = 1);
public:
   typedef GuiControl Parent;
   DECLARE_CONOBJECT(GuiTabCtrl);
};


#endif
