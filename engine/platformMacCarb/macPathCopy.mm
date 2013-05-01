//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#import <Cocoa/Cocoa.h>

#include "platform/platform.h"
#include "console/console.h"

bool dPathCopy(const char* source, const char* dest, bool nooverwrite)
{
   NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
   NSFileManager *manager = [NSFileManager defaultManager];
   
   NSString *nsource = [manager stringWithFileSystemRepresentation:source length:dStrlen(source)];
   NSString *ndest   = [manager stringWithFileSystemRepresentation:dest length:dStrlen(dest)];
   
   if(! [manager fileExistsAtPath:nsource])
   {
      Con::errorf("dPathCopy: no file exists at %s",source);
      return false;
   }
   
   if( [manager fileExistsAtPath:ndest] )
   {
      if(nooverwrite)
      {
         Con::errorf("dPathCopy: file already exists at %s",dest);
         return false;
      }
      Con::warnf("Deleting files at path: %s", dest);
      bool deleted = [manager removeFileAtPath:ndest handler:nil];
      if(!deleted)
      {
         Con::errorf("Copy failed! Could not delete files at path: %s", dest);
         return false;
      }
   }
   
   bool ret = [manager copyPath:nsource toPath:ndest handler:nil];
   [pool release];
   
   return ret;
   
}

ConsoleFunction( dPathCopy, void, 3,3, "2 args.")
{
   dPathCopy(argv[1], argv[2]);
}
