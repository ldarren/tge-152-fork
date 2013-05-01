//-----------------------------------------------------------------------------
// Gui Tab Ctrl
//
// Contributed by Philip Hardin of Red Brick Games (www.redbrickgames.com)
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleTypes.h"
#include "dgl/dgl.h"
#include "guiTabCtrl.h"



////////////////////////////////////////////////////////////////////////////////
// 
// GuiTabCtrl
// 
////////////////////////////////////////////////////////////////////////////////

GuiTabCtrl::GuiTabCtrl() :
   mPressedTab(-1),
   mHoveredTab(-1),
   mSelectedTab(0),
   mTabSize(0,23),
   mIconSize(0,0),
   mMargins(3,3),
   mBevel(3),
   mLabelOffset(0,0)
{
   mActive = true;
}


//==============================================================================
// Public funcs
//==============================================================================

void GuiTabCtrl::initPersistFields()
{
   GuiControl::initPersistFields();
   addField("selectedTab", TypeS32, Offset(mSelectedTab, GuiTabCtrl));
   addField("tabSize", TypePoint2I, Offset(mTabSize, GuiTabCtrl));
   addField("iconSize", TypePoint2I, Offset(mIconSize, GuiTabCtrl));
   addField("margins", TypePoint2F, Offset(mMargins, GuiTabCtrl));
   addField("bevel", TypeS32, Offset(mBevel, GuiTabCtrl));
   addField("labelOffset", TypePoint2I, Offset(mLabelOffset, GuiTabCtrl));
}


bool GuiTabCtrl::onAdd()
{
   if (!GuiControl::onAdd())
      return false;
   mTabLabelField = StringTable->insert("tabLabel");
   mTabIconField = StringTable->insert("tabIcon");
   return true;
}


bool GuiTabCtrl::onWake()
{
   if (!GuiControl::onWake())
      return false;

   // construct tab bitmap array.
   if (mProfile->mTextureHandle)
      mProfile->constructBitmapArray();

   // get label icons.
   for (int i = 0; i < size(); i++){
      const char* s = getKid(i)->getDataField(mTabIconField,NULL);
      if (s)
         mTabInfos[i].mBmp.set(s);
   }
   
   if (mSelectedTab < 0 || mSelectedTab >= size())
      mSelectedTab = 0;
   setSelectedTab(mSelectedTab);

   return true;
}


void GuiTabCtrl::onSleep()
{
   GuiControl::onSleep();
   // free tab icons while we sleep.
   for (int i = 0; i < size(); i++)
      mTabInfos[i].mBmp = NULL;
}


// returns 0,0 if tab 'i' has no icon.
Point2I GuiTabCtrl::getIconSize(int i)
{
   TextureHandle bmp = mTabInfos[i].mBmp;
   if (mIconSize.x == 0 && mIconSize.y == 0 && bmp)
      return Point2I(bmp.getWidth(),bmp.getHeight());
   return mIconSize;
}


// Returns the rect for the i'th tab.
RectI GuiTabCtrl::getTabRect(int i)
{
   Point2I sz = mTabSize;
   int left =  mMargins.x < 1 ? ((int)(mMargins.x * mBounds.extent.x + 0.5)) : mMargins.x;
   int right = mMargins.y < 1 ? ((int)(mMargins.y * mBounds.extent.x + 0.5)) : mMargins.y;
   if (sz.x == 0) {
      sz.x = (mBounds.extent.x - left - right);
      if (size() > 1)
         sz.x /= size();
   }
   return RectI(localToGlobalCoord(Point2I(left + i * sz.x, 0)), sz);
}


// Returns the # of the tab that contains 'pt', or -1 if no tab contains 'pt'.
int GuiTabCtrl::getTabAt(const Point2I& pt)
{
   for (int i = 0; i < size(); i++)
      if (getTabRect(i).pointInRect(pt))
         return i;
   return -1;
}


void GuiTabCtrl::onMouseDown(const GuiEvent& evt)
{
   if (!mActive)
      return;
   if (mProfile->mCanKeyFocus)
      setFirstResponder();
   mPressedTab = getTabAt(evt.mousePoint);
   if (mPressedTab == -1)
      return;
   
   if (mProfile->mSoundButtonDown) {
      AUDIOHANDLE handle = alxCreateSource(mProfile->mSoundButtonDown);
      alxPlay(handle);
   }
   
   mouseLock();
   setUpdate();
}


void GuiTabCtrl::onMouseUp(const GuiEvent& evt)
{
   if (!mActive)
      return;
   
   mouseUnlock();
   setUpdate();
   
   //if the mouse-up occurs over the tab that was moused-down'ed, make that the active tab.
   if (mPressedTab != -1 && mPressedTab == getTabAt(evt.mousePoint)) {
      setSelectedTab(mPressedTab);
   }
   
   mPressedTab = -1;
}


void GuiTabCtrl::onMouseMove(const GuiEvent& evt)
{
   if (!mActive)
      return;
   int hover = getTabAt(evt.mousePoint);
   if (hover != mHoveredTab) {
      setUpdate();
      if (hover != -1 && !isMouseLocked() && mProfile->mSoundButtonOver) {
         AUDIOHANDLE handle = alxCreateSource(mProfile->mSoundButtonOver);
         alxPlay(handle);
      }
      mHoveredTab = hover;
   }
}


void GuiTabCtrl::onMouseLeave(const GuiEvent& evt)
{
   mHoveredTab = -1;
}


void GuiTabCtrl::setScriptValue(const char* val)
{
   setSelectedTab(dAtoi(val));
}


const char* GuiTabCtrl::getScriptValue()
{
   char* ret = Con::getReturnBuffer(9);
   dSprintf(ret, 9, "%d", mSelectedTab);
   return ret;
}


void GuiTabCtrl::onRender(Point2I offset, const RectI& updateRect)
{
   ColorI lite = mProfile->mBorderColorHL;
   ColorI dark = mProfile->mBorderColor;
   int thick = mProfile->mBorderThickness;
   
   // draw pane border.
   RectI r(offset, mBounds.extent);
   int h = mTabSize.y;
   r.point.y += h;
   r.extent.y -= h;
   drawPane(r,mProfile->mFillColor,lite,dark,thick);
   
   // draw tabs.
   for (int i = 0; i < size(); i++)
      drawTab(i,lite,dark,thick);
   
   dglSetBitmapModulation(ColorI(0,0,0));
   renderChildControls(offset, updateRect);
}


void GuiTabCtrl::setSelectedTab(int tab)
{
   if (tab < 0 || tab >= size())
      return;
   for (int i = 0; i < size(); i++)
      getKid(i)->setVisible(false);
   getKid(tab)->setVisible(true);
   mSelectedTab = tab;
}


GuiControl* GuiTabCtrl::getKid(int i)
{
   return (GuiControl*) (*this)[i];
}


//==============================================================================
// Implementation funcs
//==============================================================================

// get icon and label positions.
void GuiTabCtrl::getIconAndLabelRects(const RectI& r, const Point2I& iconSize, const char* label, RectI& iconRect, RectI& labelRect)
{
   GFont* font = mProfile->mFont;
   iconRect.extent = iconSize;
   labelRect.extent = Point2I(font->getStrWidth(label),font->getHeight());
   int gap = iconRect.extent.x && labelRect.extent.x ? 1 : 0;
   S32 width = iconRect.extent.x + gap + labelRect.extent.x;
   
   // align the horizontal.
   S32 startX = 0;
   // If the text is longer then the box size, force Left Justify.
   if (width <= r.extent.x) {
      if (mProfile->mAlignment == GuiControlProfile::RightJustify)
         startX += r.extent.x - width;
      else if (mProfile->mAlignment == GuiControlProfile::CenterJustify)
         startX += (r.extent.x - width) / 2;
   }
   
   iconRect.point = r.point + Point2I(startX, (r.extent.y - iconRect.extent.y) / 2);
   labelRect.point = r.point + Point2I(startX + iconRect.extent.x + gap, (r.extent.y - labelRect.extent.y) / 2);
}


// get the bitmap subregion for tab 'tab'.
RectI GuiTabCtrl::getTabBitmapRect(int tab)
{
   // You can supply up to 6 bitmaps to represent the states of a tab.
   // The 6 bitmaps and the corresponding tab states are:
   //
   // 0 = tab unselected
   // 1 = tab   selected
   // 2 = tab unselected + hovered
   // 3 = tab   selected + hovered
   // 4 = tab unselected + hovered + pressed
   // 5 = tab   selected + hovered + pressed
   //
   // If you supply fewer than 6 bitmaps, the tab ctrl will be smart about
   // choosing reasonable-looking substitutes if necessary.

   int numBmps = mProfile->mBitmapArrayRects.size();
   int idx = 0;
   if (tab == mSelectedTab && idx + 1 < numBmps)
      idx++;
   if (tab == mHoveredTab && idx + 2 < numBmps)
      idx += 2;
   if (tab == mPressedTab && idx + 2 < numBmps)
      idx += 2;
   return mProfile->mBitmapArrayRects[idx];
}


void GuiTabCtrl::drawTab(int i, const ColorI& lite, const ColorI& dark, int thick)
{
   // determine fill color.
   ColorI fill = mProfile->mFillColorNA;
   if (i == mSelectedTab)
      fill = mProfile->mFillColor;
   else if (i == mHoveredTab)
      fill = mProfile->mFillColorHL;
   
   // tab raised or lowered?
   bool isPressed = i == mHoveredTab && i == mPressedTab;
   
   RectI r = getTabRect(i);
   
   // draw tab.
   dglClearBitmapModulation();
   if (mProfile->mTextureHandle) {
      RectI r2 = r;
      r2.extent.y += thick; // this makes the bitmap overlap the top of the border around the pane.
      dglDrawBitmapStretchSR(mProfile->mTextureHandle,r2,getTabBitmapRect(i));
   }
   else {
      // get pts.
      RectI r2 = r;
      r2.point.x += 1;
      r2.extent.x -= 1;
      drawTabBackground(r2, fill, isPressed ? dark : lite, isPressed ? lite : dark, thick);
   }
   
   // figure out icon/label rects.
   if (isPressed)
      r.point += Point2I(2,2);
   r.point += mLabelOffset;
   RectI iconRect;
   RectI labelRect;
   const char* label = getKid(i)->getDataField(mTabLabelField,NULL);
   getIconAndLabelRects(r,getIconSize(i),label,iconRect,labelRect);
   
   // draw icon.
   GuiTabInfo& info = mTabInfos[i];
   if (info.mBmp)
      dglDrawBitmapStretch(info.mBmp,iconRect);
   
   // get label color.
   ColorI c = mProfile->mFontColor;
   if (!mActive)
      c = mProfile->mFontColorNA;
   else if (i == mSelectedTab)
      c = mProfile->mFontColorHL;
   
   // draw label.
   dglSetBitmapModulation(c);
   dglDrawText(mProfile->mFont, labelRect.point, label, mProfile->mFontColors);
   dglClearBitmapModulation();
}


void GuiTabCtrl::drawTabBackground(const RectI& r, const ColorI& fill, const ColorI& lite, const ColorI& dark, int width)
{
   const int numPts = 6;
   Point2I pts[numPts];
   pts[0] = r.point + Point2I(0,r.extent.y);
   pts[1] = r.point + Point2I(0,mBevel);
   pts[2] = r.point + Point2I(mBevel,0);
   pts[3] = r.point + Point2I(r.extent.x-mBevel,0);
   pts[4] = r.point + Point2I(r.extent.x,mBevel);
   pts[5] = r.point + r.extent;
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);
   
   // draw interior.
   glColor4ubv(fill);
   glBegin(GL_TRIANGLE_FAN);
   for (int i = 0; i < numPts; i++)
      glVertex2i(pts[i].x,pts[i].y);
   glEnd();
   
   // draw border.
   while (width) {
      glBegin(GL_LINES);
      for (int i = 1; i < numPts; i++){
         // figure out what color to use for this border segment
         ColorI c = lite;
         Point2I delta = pts[i] - pts[i-1];
         int d = delta.x - delta.y;
         if (d < 0)
            c = dark;
         else if (d == 0)
            c.interpolate(lite,dark,0.5);
         glColor4ubv(c);
   
         glVertex2i(pts[i-1].x,pts[i-1].y);
         glVertex2i(pts[i].x,pts[i].y);
      }
      glEnd();
   
      pts[0].x++;
      pts[1].x++;
      pts[2].y++;
      pts[3].y++;
      pts[4].x--;
      pts[5].x--;
   
      width--;
   }
}


void GuiTabCtrl::drawPane(const RectI& rect, const ColorI& fill, const ColorI& lite, const ColorI& dark, int width)
{
   dglDrawRectFill(rect, fill);
   
   int l = rect.point.x;
   int t = rect.point.y;
   int r = l + rect.extent.x - 1;
   int b = t + rect.extent.y - 1;
   RectI selTabRect = getTabRect(mSelectedTab);
   int m1 = selTabRect.point.x + width;
   int m2 = m1 + selTabRect.extent.x + 1 - width;
   
   while (width) {
      dglDrawLine(l, t, m1, t, lite);
      dglDrawLine(m2, t, r, t, lite);
      dglDrawLine(l, t, l, b, lite);
      dglDrawLine(l + 1, b, r, b, dark);
      dglDrawLine(r, t + 1, r, b - 1, dark);
   
      l++;
      r--;
      t++;
      b--;
   
      width--;
   }
}


IMPLEMENT_CONOBJECT(GuiTabCtrl);

ConsoleMethod(GuiTabCtrl, setSelectedTab, void, 3, 3, "(some args...)\n")
{
   int tab = dAtoi(argv[2]);
   object->setSelectedTab(tab);
}
