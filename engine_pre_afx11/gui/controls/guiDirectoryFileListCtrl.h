#ifndef _GUI_DIRECTORYFILELISTCTRL_H_
#define _GUI_DIRECTORYFILELISTCTRL_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _GUI_LISTBOXCTRL_H_
#include "gui/controls/guiListBoxCtrl.h"
#endif

class GuiDirectoryFileListCtrl : public GuiListBoxCtrl
{
private:
   typedef GuiListBoxCtrl Parent;
protected:
   StringTableEntry mFilePath;
   StringTableEntry mFilter;

   void openDirectory();
public:
   GuiDirectoryFileListCtrl();
   DECLARE_CONOBJECT(GuiDirectoryFileListCtrl);

   /// Set the current path to grab files from
   bool setCurrentPath( const char* path, const char* filter );
   void setCurrentFilter( const char* filter );

   /// Get the currently selected file's name
   StringTableEntry getSelectedFileName();


   virtual void onMouseDown(const GuiEvent &event);


   bool onWake();
};


#endif
