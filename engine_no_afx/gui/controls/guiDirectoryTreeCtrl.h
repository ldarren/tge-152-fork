//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUI_DIRECTORYTREECTRL_H_
#define _GUI_DIRECTORYTREECTRL_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _RESMANAGER_H_
#include "core/resManager.h"
#endif

#ifndef _GUI_TREEVIEWCTRL_H
#include "gui/controls/guiTreeViewCtrl.h"
#endif

class GuiDirectoryTreeCtrl : public GuiTreeViewCtrl
{
private:
   typedef GuiTreeViewCtrl Parent;

   // Utility functions
   void recurseInsert( Item* parent, StringTableEntry path );
   void addPathToTree( StringTableEntry path );

protected:
   StringTableEntry                    mSelPath;

public:
   enum
   {
      Icon_Folder = 1,
      Icon_FolderClosed
   };
   GuiDirectoryTreeCtrl();

   bool onWake();
   bool onAdd();
   bool onVirtualParentExpand(Item *item);
   void onItemSelected( Item *item );
   StringTableEntry getSelectedPath();
   bool setSelectedPath( StringTableEntry path );
   bool buildIconTable(const char * icons);

   // Mod Path Parsing
   StringTableEntry getUnit(const char *string, U32 index, const char *set);
   StringTableEntry getUnits(const char *string, S32 startIndex, S32 endIndex, const char *set);
   U32 getUnitCount(const char *string, const char *set);


   DECLARE_CONOBJECT(GuiDirectoryTreeCtrl);
};

#endif
