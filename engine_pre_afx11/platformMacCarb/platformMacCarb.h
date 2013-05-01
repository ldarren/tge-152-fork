//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PLATFORMMACCARB_H_
#define _PLATFORMMACCARB_H_

/// NOTE: Placing system headers before Torque's platform.h will work around the Torque-Redefines-New problems.
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <AGL/agl.h>
#include "platform/platform.h"
#include "math/mMath.h"


class MacCarbPlatState
{
public:
   GDHandle          hDisplay;
   CGDirectDisplayID cgDisplay;
   
   bool              captureDisplay;
   bool              fadeWindows;

   WindowPtr         appWindow;   
   char              appWindowTitle[256];
   WindowGroupRef    torqueWindowGroup;

   bool              quit;
   
   AGLContext        ctx;
   bool              ctxNeedsUpdate;
   bool              headless;

   S32               desktopBitsPixel;
   S32               desktopWidth;
   S32               desktopHeight;
   U32               currentTime;
   
   U32               osVersion;
   
   TSMDocumentID     tsmDoc;
   bool              tsmActive;
   
   U32               firstThreadId;
   U32               torqueThreadId;
   
   void*             alertSemaphore;
   S32               alertHit;
   DialogRef         alertDlg;
   EventQueueRef     mainEventQueue;
   
   MRandomLCG        platRandom;
   
   bool              mouseLocked;
   bool              backgrounded;
   bool              minimized;
   
   S32               sleepTicks;
   S32               lastTimeTick;

   Point2I           windowSize;
   
   U32               appReturn;
   
   U32               argc;
   char**            argv;
   

   
   MacCarbPlatState();
};

/// Global singleton that encapsulates a lot of mac platform state & globals.
extern MacCarbPlatState platState;

/// @name Misc Mac Plat Functions
/// Functions that are used by multiple files in the mac plat, but too trivial
/// to require their own header file.
/// @{
/// Fills gGLState with info about this gl renderer's capabilities.
void getGLCapabilities(void);

/// Creates a new mac window, of a particular size, centered on the screen.
/// If a fullScreen window is requested, then the window is created without
/// decoration, in front of all other normal windows AND BEHIND asian text input methods.
/// This path to a fullScreen window allows asian text input methods to work
/// in full screen mode, because it avoids capturing the display.
WindowPtr MacCarbCreateOpenGLWindow( GDHandle hDevice, U32 width, U32 height, bool fullScreen );

/// Asnychronously fade a window into existence, and set menu bar visibility.
/// The fading can be turned off via the preference $pref::mac::fadeWindows.
/// It also sends itself to the main thread if it is called on any other thread.
void MacCarbFadeInWindow( WindowPtr window );

/// Asnychronously fade a window out of existence. The window will be destroyed
/// when the fade is complete.
/// The fading can be turned off via the preference $pref::mac::fadeWindows.
/// It also sends itself to the main thread if it is called on any other thread.
void MacCarbFadeAndReleaseWindow( WindowPtr window );

/// Manage Mac MenuBar visibility.
/// If show is true the menubar is shown, if false the menubar is hidden.
/// It sends itself to the main thread if it is called on any other thread.
void MacCarbShowMenuBar(bool show);

/// Translates a Mac keycode to a Torque keycode
U8 TranslateOSKeyCode(U8 vcode);
/// @}

/// @name Misc Mac Plat constants
/// @{

/// earlier versions of OSX don't have these convinience macros, so manually stick them here.
#ifndef IntToFixed
#define IntToFixed(a) 	((Fixed)(a) <<16)
#define FixedToInt(a)	((short)(((Fixed)(a) + fixed1/2) >> 16))
#endif

/// window level constants
const U32 kTAlertWindowLevel        = CGShieldingWindowLevel() - 1;
const U32 kTUtilityWindowLevel      = CGShieldingWindowLevel() - 2;
const U32 kTFullscreenWindowLevel   = CGShieldingWindowLevel() - 3;

/// mouse wheel sensitivity factor
const S32 kTMouseWheelMagnificationFactor = 25;

/// @}

#endif //_PLATFORMMACCARB_H_

