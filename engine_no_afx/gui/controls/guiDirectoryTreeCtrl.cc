//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/controls/guiDirectoryTreeCtrl.h"

IMPLEMENT_CONOBJECT(GuiDirectoryTreeCtrl);

GuiDirectoryTreeCtrl::GuiDirectoryTreeCtrl(): GuiTreeViewCtrl()
{
   // Parent configuration
   mBounds.set( 0,0,200,100 );
   mDestroyOnSleep = false;
   mSupportMouseDragging = false;
   mMultipleSelections = false;

   mSelPath = StringTable->insert("");
}

bool GuiDirectoryTreeCtrl::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   // Specify our icons
   buildIconTable( NULL );

   return true;
}


bool GuiDirectoryTreeCtrl::onWake()
{
   if( !Parent::onWake() )
      return false;

   // Kill off any existing items
   destroyTree();

   // Here we're going to grab our system volumes from the platform layer and create them as roots
   //
   // Note : that we're passing a 1 as the last parameter to Platform::dumpDirectories, which tells it
   // how deep to dump in recursion.  This is an optimization to keep from dumping the whole file system
   // to the tree.  The tree will dump more paths as necessary when the virtual parents are expanded,
   // much as windows does.

   ResourceManager->initExcludedDirectories();

   StringTableEntry RootPath = ResourceManager->getModPaths();
   //getUnit(argv[1], dAtoi(argv[2]), " \t\n");
   S32 modCount = getUnitCount( RootPath, ";" );
   for( S32 i = 0; i < modCount; i++ )
   {
      // Compose full mod path location, and dump the path to our vector
      StringTableEntry currentMod = getUnit( RootPath, i, ";" );
      char fullModPath [512];
      dMemset( fullModPath, 0, 512 );
      dSprintf( fullModPath, 512, "%s/%s/", Platform::getWorkingDirectory(), currentMod );

      Vector<StringTableEntry> pathVec;
      Platform::dumpDirectories( fullModPath, pathVec, 0, true);
      if( ! pathVec.empty() )
      {
         // Iterate through the returned paths and add them to the tree
         Vector<StringTableEntry>::iterator j = pathVec.begin();
         for( ; j != pathVec.end(); j++ )
         {
            char fullModPathSub [512];
            dMemset( fullModPathSub, 0, 512 );
            dSprintf( fullModPathSub, 512, "%s/%s", currentMod, (*j) );
            addPathToTree( fullModPathSub );
         }
      }
      else
         addPathToTree( fullModPath );
   }

   // Success!
   return true;
}

bool GuiDirectoryTreeCtrl::onVirtualParentExpand(Item *item)
{
   if( !item || !item->isExpanded() )
      return true;

   StringTableEntry pathToExpand = item->getValue();

   if( !pathToExpand )
   {
      Con::errorf("GuiDirectoryTreeCtrl::onVirtualParentExpand - Unable to retrieve item value!");
      return false;
   }

   Vector<StringTableEntry> pathVec;
   Platform::dumpDirectories( pathToExpand, pathVec, 0, true );
   if( ! pathVec.empty() )
   {
      // Iterate through the returned paths and add them to the tree
      Vector<StringTableEntry>::iterator i = pathVec.begin();
      for( ; i != pathVec.end(); i++ )
         recurseInsert(item, (*i) );

      item->setExpanded( true );
   }

   item->setVirtualParent( false );

   // Update our tree view
   buildVisibleTree();

   return true;

}



bool GuiDirectoryTreeCtrl::buildIconTable(const char * icons)
{
   // Icons should be designated by the bitmap/png file names (minus the file extensions)
   // and separated by colons (:).
   if (!icons)
      icons = StringTable->insert("common/ui/folder:common/ui/folder:common/ui/folder_closed");

   return Parent::buildIconTable( icons );
}

void GuiDirectoryTreeCtrl::addPathToTree( StringTableEntry path )
{
   if( !path )
   {
      Con::errorf("GuiDirectoryTreeCtrl::addPathToTree - Invalid Path!");
      return;
   }

   // Identify which root (volume) this path belongs to (if any)
   S32 root = getFirstRootItem();
   StringTableEntry ourPath = &path[ dStrcspn( path, "//" ) + 1];
   StringTableEntry ourRoot = getUnit( path, 0, "//" );
   // There are no current roots, we can safely create one
   if( root == 0 )
   {
      recurseInsert( NULL, path );
   }
   else
   {
      while( root != 0 )
      {
         if( dStrcmp( getItemValue( root ), ourRoot ) == 0 )
         {
            recurseInsert( getItem( root ), ourPath );
            break;
         }
         root = this->getNextSiblingItem( root );
      }
      // We found none so we'll create one
      if ( root == 0 )
      {
         recurseInsert( NULL, path );
      }
   }
}

void GuiDirectoryTreeCtrl::onItemSelected( Item *item )
{
   Con::executef( this, 2, "onSelectPath", avar("%s",item->getValue()) );

   mSelPath = StringTable->insert( item->getValue() );

   if( Platform::hasSubDirectory( item->getValue() ) )
      item->setVirtualParent( true );
}

void GuiDirectoryTreeCtrl::recurseInsert( Item* parent, StringTableEntry path )
{
   if( !path )
      return;

   char szPathCopy [ 1024 ];
   dMemset( szPathCopy, 0, 1024 );
   dStrcpy( szPathCopy, path );

   // Jump over the first character if it's a root /
   char *curPos = szPathCopy;
   if( *curPos == '/' )
      curPos++;

   char *delim = dStrchr( curPos, '/' );
   if ( delim )
   {
      // terminate our / and then move our pointer to the next character (rest of the path)
      *delim = 0x00;
      delim++;
   }
   S32 itemIndex = 0;
   // only insert blindly if we have no root
   if( !parent )
   {
      itemIndex = insertItem( 0, curPos, curPos );
      getItem( itemIndex )->setNormalImage( Icon_FolderClosed );
      getItem( itemIndex )->setExpandedImage( Icon_Folder );
   }
   else
   {
      Item *item = parent;

      char *szValue = new char[ 1024 ];
      dMemset( szValue, 0, 1024 );
      dSprintf( szValue, 1024, "%s/%s", parent->getValue(), curPos );
      Item *exists = item->findChildByValue( szValue );
      if( !exists && dStrcmp( curPos, "" ) != 0 )
      {
         // Since we're adding a child this parent can't be a virtual parent, so clear that flag
         item->setVirtualParent( false );

         itemIndex = insertItem( item->getID(), curPos);

         getItem( itemIndex )->setValue( szValue );
         getItem( itemIndex )->setNormalImage( Icon_FolderClosed );
         getItem( itemIndex )->setExpandedImage( Icon_Folder );

      }
      else
      {
         delete []szValue;
         itemIndex = ( item != NULL ) ? ( ( exists != NULL ) ? exists->getID() : -1 ) : -1;
      }
   }

   // since we're only dealing with volumes and directories, all end nodes will be virtual parents
   // so if we are at the bottom of the rabbit hole, set the item to be a virtual parent
   Item* item = getItem( itemIndex );
   if( delim )
   {
      if( ( dStrcmp( delim, "" ) == 0 ) && item )
      {
         item->setExpanded( false );
         if( parent && Platform::hasSubDirectory( item->getValue() ) )
            item->setVirtualParent( true );
      }
   }
   else
   {
      if( item )
      {
         item->setExpanded( false );
         if( parent &&  Platform::hasSubDirectory( item->getValue() ) )
            item->setVirtualParent( true );
      }
   }

   // Down the rabbit hole we go
   recurseInsert( getItem( itemIndex ), delim );

}


StringTableEntry GuiDirectoryTreeCtrl::getUnit(const char *string, U32 index, const char *set)
{
   U32 sz;
   while(index--)
   {
      if(!*string)
         return "";
      sz = dStrcspn(string, set);
      if (string[sz] == 0)
         return "";
      string += (sz + 1);
   }
   sz = dStrcspn(string, set);
   if (sz == 0)
      return "";
   char *ret = Con::getReturnBuffer(sz+1);
   dStrncpy(ret, string, sz);
   ret[sz] = '\0';
   return ret;
}
StringTableEntry GuiDirectoryTreeCtrl::getUnits(const char *string, S32 startIndex, S32 endIndex, const char *set)
{
   S32 sz;
   S32 index = startIndex;
   while(index--)
   {
      if(!*string)
         return "";
      sz = dStrcspn(string, set);
      if (string[sz] == 0)
         return "";
      string += (sz + 1);
   }
   const char *startString = string;
   while(startIndex <= endIndex--)
   {
      sz = dStrcspn(string, set);
      string += sz;
      if (*string == 0)
         break;
      string++;
   }
   if(!*string)
      string++;
   U32 totalSize = (U32(string - startString));
   char *ret = Con::getReturnBuffer(totalSize);
   dStrncpy(ret, startString, totalSize - 1);
   ret[totalSize-1] = '\0';
   return ret;
}

U32 GuiDirectoryTreeCtrl::getUnitCount(const char *string, const char *set)
{
   U32 count = 0;
   U8 last = 0;
   while(*string)
   {
      last = *string++;

      for(U32 i =0; set[i]; i++)
      {
         if(last == set[i])
         {
            count++;
            last = 0;
            break;
         }
      }
   }
   if(last)
      count++;
   return count;
}


ConsoleMethod( GuiDirectoryTreeCtrl, getSelectedPath, const char*, 2,2, "getSelectedPath() - returns the currently selected path in the tree")
{
   return object->getSelectedPath();
}

StringTableEntry GuiDirectoryTreeCtrl::getSelectedPath()
{
   return mSelPath;
}

ConsoleMethod( GuiDirectoryTreeCtrl, setSelectedPath, bool, 3, 3, "setSelectedPath(path) - expands the tree to the specified path")
{
   return object->setSelectedPath( argv[2] );
}

bool GuiDirectoryTreeCtrl::setSelectedPath( StringTableEntry path )
{
   if( !path )
      return false;

   // Since we only list one deep on paths, we need to add the path to the tree just incase it isn't already indexed in the tree
   // or else we wouldn't be able to select a path we hadn't previously browsed to. :)
   if( Platform::isDirectory( path ) )
      addPathToTree( path );

   // see if we have a child that matches what we want
   for(U32 i = 0; i < mItems.size(); i++)
   {
      if( dStricmp( mItems[i]->getValue(), path ) == 0 )
      {
         Item* item = mItems[i];
         AssertFatal(item,"GuiDirectoryTreeCtrl::setSelectedPath - Item Index Bad, Fatal Mistake!!!");
         item->setExpanded( true );
         clearSelection();
         setItemSelected( item->getID(), true );
         // make sure all of it's parents are expanded
         S32 parent = getParentItem( item->getID() );
         while( parent != 0 )
         {
            setItemExpanded( parent, true );
            parent = getParentItem( parent );
         }
         // Rebuild our tree just incase we've oops'd
         buildVisibleTree();
         scrollVisible( item );
      }
   }
   return false;
}