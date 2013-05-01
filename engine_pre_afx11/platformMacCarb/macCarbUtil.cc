//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformMacCarb/platformMacCarb.h"
#include "macCarbUtil.h"
//#include <CoreServices/CoreServices.h>
#include "console/console.h"

//-------------------------------------------------------------------------------
// Function name:  LoadPrivateFrameworksBundle
// Summary:        Looks in the application's Frameworks folder for a framework,
//                  and loads it if it finds it.
//                  Adpoted from a forum post by David 'FenrirWolf' Grace.
//-------------------------------------------------------------------------------
bool LoadPrivateFrameworkBundle(CFStringRef framework, CFBundleRef *bundlePtr) {
	// Looks in the application bundle for a framework, and loads it if it finds it.
	
	CFURLRef   privFrameworksURL, bundleURL;
	if(!( privFrameworksURL = CFBundleCopyPrivateFrameworksURL( CFBundleGetMainBundle() )) ) {
		return false;
	}
   
	
	if(!( bundleURL = CFURLCreateCopyAppendingPathComponent(NULL, privFrameworksURL, framework, false)) ) {
		CFRelease(privFrameworksURL);
		return false;
	}
	
   *bundlePtr = CFBundleCreate(NULL, bundleURL);
	if (*bundlePtr && CFBundleLoadExecutable( *bundlePtr ) ) {
		//found it under the apps private framework
		CFRelease(privFrameworksURL);
		CFRelease(bundleURL);
		return true;
	}
   
   CFRelease(privFrameworksURL);
   CFRelease(bundleURL);
   if(*bundlePtr != NULL)
      CFRelease(*bundlePtr);
   return false;
}


//-------------------------------------------------------------------------------
// Function name:  LoadFrameworksBundle
// Summary:        Looks for a framework first in the app then the system,
//                 and loads it if it finds it.
//                 Adopted from Apple "CallMachOFramework" sample application
//-------------------------------------------------------------------------------
bool LoadFrameworkBundle(CFStringRef framework, CFBundleRef *bundlePtr)
{
	const int   numLocs = 5;
   short       vols[numLocs] =        {kOnAppropriateDisk,
                                       kOnAppropriateDisk,
                                       kOnAppropriateDisk,
                                       kLocalDomain,
                                       kLocalDomain};
                                       
	OSType		folderType[numLocs] =  {kPrivateFrameworksFolderType,
                                       kFrameworksFolderType,
                                       kApplicationSupportFolderType,
                                       kFrameworksFolderType,
                                       kDomainLibraryFolderType};
	OSStatus    err;
	FSRef       frameworksFolderRef;
	CFURLRef    baseURL;
	CFURLRef    bundleURL;
   
	if (bundlePtr==NULL)
      return false;
   
	*bundlePtr = NULL;
   
   // Look in the application's Frameworks folder before looking in the system
	if( LoadPrivateFrameworkBundle(framework, bundlePtr) )
      return true; // Yay!  We found it, so return true...
                   // Otherwise, fall through to Torques usual bundle load logic
   
   
	baseURL = NULL;
	bundleURL = NULL;
   
	for (int i = 0; i<numLocs; i++)
	{
	   err = FSFindFolder(vols[i], folderType[i], true, &frameworksFolderRef);
	   if (err == noErr) {
	      baseURL = CFURLCreateFromFSRef(kCFAllocatorSystemDefault, &frameworksFolderRef);
	      if (baseURL == NULL) {
	         err = coreFoundationUnknownErr;
	      }
	   }
	   if (err == noErr) {
	      bundleURL = CFURLCreateCopyAppendingPathComponent(kCFAllocatorSystemDefault, baseURL, framework, false);
	      if (bundleURL == NULL) {
	         err = coreFoundationUnknownErr;
	      }
	   }
	   if (err == noErr) {
	      *bundlePtr = CFBundleCreate(kCFAllocatorSystemDefault, bundleURL);
	      if (*bundlePtr == NULL) {
	         err = coreFoundationUnknownErr;
	      }
	   }
	   if (err == noErr) {
	      if ( ! CFBundleLoadExecutable( *bundlePtr ) ) {
	         err = coreFoundationUnknownErr;
	      }
	      else // GOT IT!
	      	break;
	   }
	}
	
   // Clean up.
   if (err != noErr && *bundlePtr != NULL)
   {
      CFRelease(*bundlePtr);
      *bundlePtr = NULL;
   }
   if (bundleURL != NULL)
      CFRelease(bundleURL);
   if (baseURL != NULL)
      CFRelease(baseURL);
   
   return err == noErr;
}

//-----------------------------------------------------------------------------
// Converts a QuickDraw displayID to a Core Graphics displayID.
// Different Mac APIs need different displayID types. The conversion is trivial
// on 10.3+, but ugly on 10.2, so we wrap it here.
//-----------------------------------------------------------------------------
CGDirectDisplayID MacCarbGetCGDisplayFromQDDisplay(GDHandle hDisplay)
{
   // 10.2 doesn't have QDGetCGDirectDisplayID, so we roll our own.
   //  this is adapted from http://developer.apple.com/samplecode/glut/listing116.html
#if defined(TORQUE_MAC_HAS_QDGETCGDIRECTDISPLAYID)
   CGDirectDisplayID display = QDGetCGDirectDisplayID(hDisplay);
#else
   Rect qdRect = (**hDisplay).gdRect;
   CGRect cgRect = CGRectMake(qdRect.left, qdRect.top, qdRect.right - qdRect.left, qdRect.bottom - qdRect.top);
   CGDisplayCount nDisplays;
   CGDirectDisplayID displays[32];
   CGGetDisplaysWithRect(cgRect, 32, displays, &nDisplays);
   // TODO: this may not work well when video mirroring is on. check it & see.
   CGDirectDisplayID display = displays[0];
#endif
   return display;
}

