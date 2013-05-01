//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

/// To target MacOS 10.2, you need to modify some global project build settings.
/// The default is to target 10.3, as that provides the best feature set and behavior in torque.
/// MACOSX_DEPLOYMENT_TARGET_ppc = 10.2
/// SDKROOT_ppc = /Developer/SDKs/MacOSX10.2.8.sdk

/// for more info on cross development with xcode, see:
/// http://developer.apple.com/documentation/DeveloperTools/Conceptual/cross_development/UniversalBinaries/chapter_4_section_1.html
#include <AvailabilityMacros.h>
#if MAC_OS_X_VERSION_MIN_REQUIRED < 1030


#define NO_REDEFINE_10_2_COMPAT
#include "platformMacCarb/platformMacCarb.h"
#include "platformMacCarb/macCarbEvents.h"
#include "platform/platformThread.h"

S32 (* TransitionWindowWithOptions_ptr) (void*, U32, U32, const void*, bool, void*) = NULL;
   
bool (* CGCursorIsVisible_ptr) ()      = NULL;
U32 (* CGDisplayHideCursor_ptr) (U32)  = NULL;
U32 (* CGDisplayShowCursor_ptr) (U32)  = NULL;


// Replaces the transitions under 10.2. 
// Our implementation does not perform transitions, it just sends the events
// that torque expects.
S32 TransitionWindowWithOptions_compat( void* theWind, U32, U32 transition, 
   const void*, bool, TransitionWindowOptions*)
{
   if(transition == kWindowHideTransitionAction)
   {
      HideWindow(theWind);
      MacCarbSendTorqueEventToMain(kEventTorqueReleaseWindow, theWind);
   }
   else
   {
      ShowWindow(theWind);
   }
}

static bool sgCursorHidden = false;
bool CGCursorIsVisible_compat()
{
   return sgCursorHidden;
}

U32 CGDisplayHideCursor_compat(U32)
{
   AssertFatal(Thread::getCurrentThreadId() == platState.firstThreadId, 
         "Cannot call Mac Carbon toolbox routines from any thread but the first thread.");
   HideCursor();
   sgCursorHidden = true;
   return 0;
}

U32 CGDisplayShowCursor_compat(U32)
{
   AssertFatal(Thread::getCurrentThreadId() == platState.firstThreadId, 
         "Cannot call Mac Carbon toolbox routines from any thread but the first thread.");
   ShowCursor();
   sgCursorHidden = false;
   return 0;
}

void MacCarbInit1020CompatInit()
{
#define INIT_COMPAT_FUNC( a ) \
         a##_ptr = a;         \
         if( a == NULL )      \
         a##_ptr = a##_compat

   INIT_COMPAT_FUNC(CGCursorIsVisible);
   INIT_COMPAT_FUNC(CGDisplayHideCursor);
   INIT_COMPAT_FUNC(CGDisplayShowCursor);
   INIT_COMPAT_FUNC(TransitionWindowWithOptions);   
#undef INIT_COMPAT_FUNC
}


#endif //MAC_OS_X_VERSION_MIN_REQUIRED < 1030
