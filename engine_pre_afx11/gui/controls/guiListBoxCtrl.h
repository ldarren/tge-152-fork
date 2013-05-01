//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _GUI_LISTBOXCTRL_H_
#define _GUI_LISTBOXCTRL_H_

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _DGL_H_
#include "dgl/dgl.h"
#endif

#ifndef _H_GUIDEFAULTCONTROLRENDER_
#include "gui/core/guiDefaultControlRender.h"
#endif

#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif


class GuiListBoxCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;
public:

   GuiListBoxCtrl();
   ~GuiListBoxCtrl();
   DECLARE_CONOBJECT(GuiListBoxCtrl);

   struct LBItem
   {
      StringTableEntry  itemText;
      bool              isSelected;      
      void*             itemData;
      ColorF            color;
      bool              hasColor;
   };

   VectorPtr<LBItem*>   mItems;
   VectorPtr<LBItem*>   mSelectedItems;
   bool                 mMultipleSelections;
   Point2I              mItemSize;
   bool                 mFitParentWidth;
   LBItem*              mLastClickItem;

   // Persistence
   static void       initPersistFields();   

   // Item Accessors
   S32               getItemCount();
   S32               getSelCount();
   S32               getSelectedItem();
   void              getSelectedItems( Vector<S32> &Items );
   S32               getItemIndex( LBItem *item );
   StringTableEntry  getItemText( S32 index );
   
   void              setCurSel( S32 index );
   void              setCurSelRange( S32 start, S32 stop );
   void              setItemText( S32 index, StringTableEntry text );

   S32               addItem( StringTableEntry text, void *itemData = NULL );
   S32               addItemWithColor( StringTableEntry text, ColorF color = ColorF(-1, -1, -1), void *itemData = NULL);
   S32               insertItem( S32 index, StringTableEntry text, void *itemData = NULL );
   S32               insertItemWithColor( S32 index, StringTableEntry text, ColorF color = ColorF(-1, -1, -1), void *itemData = NULL);
   S32               findItemText( StringTableEntry text, bool caseSensitive = false );

   void              setItemColor(S32 index, ColorF color);
   void              clearItemColor(S32 index);

   void              deleteItem( S32 index );
   void              clearItems();
   void              clearSelection();
   void              removeSelection( LBItem *item, S32 index );
   void              removeSelection( S32 index );
   void              addSelection( LBItem *item, S32 index );
   void              addSelection( S32 index );
   inline void       setMultipleSelection( bool allowMultipleSelect = true ) { mMultipleSelections = allowMultipleSelect; };

   // Sizing
   void              updateSize();
   virtual void      parentResized(const Point2I &oldParentExtent, const Point2I &newParentExtent);
   virtual bool      onWake();

   // Rendering
   virtual void      onRender( Point2I offset, const RectI &updateRect );
   virtual void      onRenderItem( RectI itemRect, LBItem *item );
   void              drawBox( const Point2I &box, S32 size, ColorI &outlineColor, ColorI &boxColor );

   // Selections
   virtual void      onMouseDown( const GuiEvent &event );

   // String Utility
   static U32        getStringElementCount( const char *string );
   static const char* getStringElement( const char* inString, const U32 index );
   

};

#endif