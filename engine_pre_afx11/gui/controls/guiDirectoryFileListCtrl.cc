#include "core/findMatch.h"
#include "gui/controls/guiDirectoryFileListCtrl.h"


IMPLEMENT_CONOBJECT( GuiDirectoryFileListCtrl );

GuiDirectoryFileListCtrl::GuiDirectoryFileListCtrl()
{
}

bool GuiDirectoryFileListCtrl::onWake()
{
   if( !Parent::onWake() )
      return false;

   setCurrentPath( "/", "*.*" );

   return true;
}

void GuiDirectoryFileListCtrl::onMouseDown(const GuiEvent &event)
{
   Parent::onMouseDown( event );

   if( event.mouseClickCount == 2 && isMethod("onDoubleClick") )
      Con::executef(this, 1, "onDoubleClick");

}


void GuiDirectoryFileListCtrl::openDirectory()
{
   Vector<Platform::FileInfo> fileVector;
   Platform::dumpPath( mFilePath, fileVector, 0 );

   // Clear the current file listing
   clearItems();

   // Does this dir have any files?
   if( fileVector.empty() )
      return;

   // If so, iterate through and list them
   Vector<Platform::FileInfo>::iterator i = fileVector.begin();
   for( S32 j=0 ; i != fileVector.end(); i++, j++ )
   {
      if( FindMatch::isMatchMultipleExprs( mFilter, (*i).pFileName,false ) )
         addItem( (*i).pFileName );
   }

   //Done!
}


void GuiDirectoryFileListCtrl::setCurrentFilter( const char* filter )
{
   if( ! filter )
      return;

   mFilter = StringTable->insert( filter );

   // Update our view
   openDirectory();

}

ConsoleMethod( GuiDirectoryFileListCtrl, setFilter, void, 3, 3, "%obj.setFilter([mask space delimited])")
{
   object->setCurrentFilter( argv[2] );
}

bool GuiDirectoryFileListCtrl::setCurrentPath( const char* path, const char* filter )
{
   // Oops, gotta give us a path to work with
   if( !path )
      return false;

   char ExpandedPath[512];
   char FullPath[512];
   dMemset( ExpandedPath, 0, 512 );
   dMemset( FullPath, 0, 512 );

   Con::expandScriptFilename( ExpandedPath, 512, path );

   if( ExpandedPath[0] != '/' )
      dSprintf( FullPath, 512, "%s/%s", Platform::getWorkingDirectory(), ExpandedPath );
   else
      dSprintf( FullPath, 512, "%s%s", Platform::getWorkingDirectory(), ExpandedPath );

   // Platform::isDirectory expects no trailing / so make sure we conform
   if( FullPath[ dStrlen( FullPath ) - 1 ] == '/' )
      FullPath[ dStrlen( FullPath ) - 1 ] = 0x00;

   // A bad path!?  For shame...
   if( !Platform::isDirectory( FullPath ) && !Platform::hasSubDirectory( FullPath ) )
      return false;

   // Store our new info
   mFilePath = StringTable->insert( FullPath );

   if( filter && dStricmp( filter, "" ) )
   mFilter   = StringTable->insert( filter );

   // Update our view
   openDirectory();

   // Peace out!
   return true;
}

ConsoleMethod( GuiDirectoryFileListCtrl, setPath, bool, 3, 4, "setPath(path,filter) - directory to enumerate files from (without trailing slash)" )
{
   return object->setCurrentPath( argv[2], argv[3] );
}


ConsoleMethod( GuiDirectoryFileListCtrl, getSelectedFiles, const char*, 2, 2, "getSelectedFiles () - returns a word separated list of selected file(s)" )
{
   Vector<S32> ItemVector;
   object->getSelectedItems( ItemVector );

   if( ItemVector.empty() )
      return StringTable->insert( "" );

   // Get an adequate buffer
   char itemBuffer[256];
   dMemset( itemBuffer, 0, 256 );

   char* returnBuffer = Con::getReturnBuffer( ItemVector.size() * 64 );
   dMemset( returnBuffer, 0, ItemVector.size() * 64 );

   // Fetch the first entry
   StringTableEntry itemText = object->getItemText( ItemVector[0] );
   if( !itemText )
      return StringTable->lookup("");
   dSprintf( returnBuffer, ItemVector.size() * 64, "%s", itemText );

   // If only one entry, return it.
   if( ItemVector.size() == 1 )
      return returnBuffer;

   // Fetch the remaining entries
   for( S32 i = 1; i < ItemVector.size(); i++ )
   {
      StringTableEntry itemText = object->getItemText( ItemVector[i] );
      if( !itemText )
         continue;

      dMemset( itemBuffer, 0, 256 );
      dSprintf( itemBuffer, 256, " %s", itemText );
      dStrcat( returnBuffer, itemBuffer );
   }

   return returnBuffer;

}

StringTableEntry GuiDirectoryFileListCtrl::getSelectedFileName()
{
   S32 item = getSelectedItem();
   if( item == -1 )
      return StringTable->lookup("");

   StringTableEntry itemText = getItemText( item );
   if( !itemText )
      return StringTable->lookup("");

   return itemText;
}

ConsoleMethod( GuiDirectoryFileListCtrl, getSelectedFile, const char*, 2, 2, "getSelectedFile () - returns the currently selected file name" )
{
   return object->getSelectedFileName();
}

