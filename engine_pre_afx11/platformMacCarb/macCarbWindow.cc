//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformMacCarb/platformMacCarb.h"
#include "platform/platformVideo.h"
#include "platformMacCarb/macCarbOGLVideo.h"
#include "platformMacCarb/macCarbConsole.h"
#include "platform/platformInput.h"
#include "platform/gameInterface.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "platformMacCarb/macCarbEvents.h"
#include "platform/platformThread.h"


//------------------------------------------------------------------------------
#pragma mark ---- PlatState ----
MacCarbPlatState platState;

MacCarbPlatState::MacCarbPlatState()
{
   hDisplay       = NULL;
   appWindow      = NULL;
   
   captureDisplay = true;
   fadeWindows    = true;
   backgrounded   = false;
   minimized      = false;
   
   quit           = false;

   ctx            = NULL;
   headless       = false;

   // start with something reasonable.
   desktopBitsPixel  = 16;
   desktopWidth      = 1024;
   desktopHeight     = 768;

   osVersion = 0;
   
   dStrcpy(appWindowTitle, "Mac Torque Game Engine");
   
   // Semaphore for alerts. We put the app in a modal state by blocking the main 
   // Torque thread until the RAEL thread  allows it to continue. 
   alertSemaphore = Semaphore::createSemaphore(0);
   alertDlg = NULL;
}

#pragma mark ---- Window stuff ----
const U32 kTFullscreenWindowAttrs =  kWindowNoShadowAttribute | kWindowStandardHandlerAttribute;
const U32 kTDefaultWindowAttrs = kWindowStandardDocumentAttributes | kWindowStandardHandlerAttribute;
//------------------------------------------------------------------------------
WindowPtr MacCarbCreateOpenGLWindow( GDHandle hDevice, U32 width, U32 height, bool fullScreen )
{
   WindowPtr w = NULL;

   Rect   rect;
   Rect dRect;

   // device bounds, eg: [top,left , bottom,right] = [0,0 , 768,1024]
   dRect = (**hDevice).gdRect;
   // center the window's rect in the display's rect.
   rect.top = dRect.top + (dRect.bottom - dRect.top - height) / 2;
   rect.left = dRect.left + (dRect.right - dRect.left - width) / 2;
   rect.right = rect.left + width;
   rect.bottom = rect.top + height;
   
   OSStatus err;
   WindowAttributes windAttr = 0L;
   WindowClass windClass = kDocumentWindowClass;

   if (fullScreen)
   {
      windAttr = kTFullscreenWindowAttrs;
      windClass = kAltPlainWindowClass;
//    Overlay windows can be used to make transperant windows,
//    which are good for performing all kinds of cool tricks.
//      windClass = kOverlayWindowClass;
//      windAttr |= kWindowOpaqueForEventsAttribute;
   }
   else
   {
      windAttr = kTDefaultWindowAttrs;
   }
   
   err = CreateNewWindow(windClass, windAttr, &rect, &w);
   AssertISV( err == noErr && w != NULL, "Failed to create a new window.");
   
   // in windowed-fullscreen mode, we set the window's level to be
   // in front of the blanking window
   if (fullScreen)
   { 
     // create a new group if ours doesn't already exist
      if (platState.torqueWindowGroup==NULL)
         CreateWindowGroup(NULL, &platState.torqueWindowGroup);

      // place window in group
      SetWindowGroup(w, platState.torqueWindowGroup);
      
      // set window group level to one higher than blanking window.
      SetWindowGroupLevel(platState.torqueWindowGroup, kTFullscreenWindowLevel);
   }
   
   platState.minimized = false;
   
   RGBColor black;
   dMemset(&black, 0, sizeof(RGBColor));
   SetWindowContentColor( w, &black);
      
   return(w);
}

//------------------------------------------------------------------------------
// Fade a window in, asynchronously.
void MacCarbFadeInWindow( WindowPtr window )
{
   if(!IsValidWindowPtr(window))
      return;
   
   // bump this to the main thread if we're not on the main thread.
   if(Thread::getCurrentThreadId() != platState.firstThreadId)
   {
      MacCarbSendTorqueEventToMain( kEventTorqueFadeInWindow, window );
      return;
   }
   
   // set state on menubar & mouse cursor. 
   if(Video::isFullScreen())
   {
      HideMenuBar();
      MacCarbSetHideCursor(true);
   }
   else
   {
      ShowMenuBar();
   }
   
   SelectWindow(window);

   if(platState.fadeWindows)
   {
      TransitionWindowOptions t;
      dMemset(&t, 0, sizeof(t));
      TransitionWindowWithOptions( window, kWindowFadeTransitionEffect, 
                     kWindowShowTransitionAction, NULL, true, &t);
   }
   else
   {
      ShowWindow(window);
   }
}

//------------------------------------------------------------------------------
// Fade a window out, asynchronously. It will be released when the transition finishes.
void MacCarbFadeAndReleaseWindow( WindowPtr window )
{
   if(!IsValidWindowPtr(window))
      return;

   if(Thread::getCurrentThreadId() != platState.firstThreadId && !platState.quit)
   {
      MacCarbSendTorqueEventToMain( kEventTorqueFadeOutWindow, window );
      return;
   }
   
   if(platState.fadeWindows)
   {
      TransitionWindowOptions t;
      dMemset(&t, 0, sizeof(t));
      TransitionWindowWithOptions( window, kWindowFadeTransitionEffect, 
                     kWindowHideTransitionAction, NULL, false, &t);
   }
   else
   {
      MacCarbSendTorqueEventToMain(kEventTorqueReleaseWindow, window);
   }
}

//------------------------------------------------------------------------------
// Hide or show the menu bar.
void MacCarbShowMenuBar(bool show)
{
   if(Thread::getCurrentThreadId() != platState.firstThreadId && !platState.quit)
   {
      MacCarbSendTorqueEventToMain( kEventTorqueShowMenuBar, (void*)show );
      return;
   }

   if(show)
      ShowMenuBar();
   else
      HideMenuBar();
}

//------------------------------------------------------------------------------
// DGL, the Gui, and TS use this for various purposes.
const Point2I &Platform::getWindowSize()
{
   return platState.windowSize;
}


//------------------------------------------------------------------------------
// save the window size, for DGL's use
void Platform::setWindowSize( U32 newWidth, U32 newHeight )
{
   platState.windowSize.set( newWidth, newHeight );
}


//------------------------------------------------------------------------------
// Issue a minimize event. The standard handler will handle it.
void Platform::minimizeWindow()
{
   HICommand cmd;
   dMemset(&cmd, 0, sizeof(HICommand));
   cmd.commandID = kHICommandMinimizeWindow;
   ProcessHICommand( &cmd );
}

//------------------------------------------------------------------------------
void Platform::setWindowTitle(const char* title )
{
   if(!platState.appWindow)
      return;
      
   // set app window's title
   CFStringRef cfsTitle = CFStringCreateWithCString(NULL, title, kCFStringEncodingUTF8);
   SetWindowTitleWithCFString(platState.appWindow, cfsTitle);
   CFRelease(cfsTitle);
   
   // save title in platstate
   dStrncpy(platState.appWindowTitle, title, getMin((U32)dStrlen(title), sizeof(platState.appWindowTitle)));
}


#pragma mark ---- Init funcs  ----
//------------------------------------------------------------------------------
void Platform::init()
{
   // Set the platform variable for the scripts
   Con::setVariable( "$platform", "macos" );

   MacConsole::create();
   if(gConsole && platState.headless == true)
      gConsole->enable(true);

   Input::init();

   // allow users to specify whether to capture the display or not when going fullscreen
   Con::addVariable("pref::mac::captureDisplay", TypeBool, &platState.captureDisplay);
   Con::addVariable("pref::mac::fadeWindows", TypeBool, &platState.fadeWindows);
      
   // create the opengl display device
   DisplayDevice *dev = NULL;
   Con::printf( "Video Init:" );
   Video::init();
   dev = OpenGLDevice::create();
   if(dev)
      Con::printf( "   Accelerated OpenGL display device detected." );
   else
      Con::printf( "   Accelerated OpenGL display device not detected." );

   // and now we can install the device.
   Video::installDevice(dev);
   Con::printf( "" );
}

//------------------------------------------------------------------------------
void Platform::shutdown()
{
   setWindowLocked( false );
   Video::destroy();
   Input::destroy();
   MacConsole::destroy();
}

//------------------------------------------------------------------------------
// Get the video settings from the prefs.
static void _MacCarbGetInitialRes(U32 &width, U32 &height, U32 &bpp, bool &fullScreen)
{
   const char* resString;
   char *tempBuf, *s;
   
   // cache the desktop size of the selected screen in platState
   Video::getDesktopResolution();
   
   // load pref variables, properly choose windowed / fullscreen  
   fullScreen = Con::getBoolVariable( "$pref::Video::fullScreen" );
   if (fullScreen)
      resString = Con::getVariable( "$pref::Video::resolution" );
   else
      resString = Con::getVariable( "$pref::Video::windowedRes" );

   // dStrtok is destructive, work on a copy...
   tempBuf = new char[dStrlen( resString ) + 1];
   dStrcpy( tempBuf, resString );

   // set window size
   //DAW: Added min size checks for windowSize
   width = dAtoi( dStrtok( tempBuf, " x\0" ) );
   if( ! width > 0 ) width = platState.windowSize.x;

   height = dAtoi( dStrtok( NULL, " x\0") );
   if( ! height > 0 ) height = platState.windowSize.y;

   // bit depth
   if (fullScreen)
   {
      s = dAtoi( dStrtok( NULL, "\0" ) );
      if( ! bpp > 0 ) bpp = 16;
   }
   else
      bpp = platState.desktopBitsPixel;

   delete [] tempBuf;
}

//------------------------------------------------------------------------------
void Platform::initWindow(const Point2I &initialSize, const char *name)
{
   dSprintf(platState.appWindowTitle, sizeof(platState.appWindowTitle), name);

   // init the default window size
   platState.windowSize = initialSize;
   if( ! platState.windowSize.x > 0 ) platState.windowSize.x = 640;
   if( ! platState.windowSize.y > 0 ) platState.windowSize.y = 480;

   DisplayDevice::init();
   
   bool fullScreen;
   U32 width, height, bpp;
   _MacCarbGetInitialRes(width, height, bpp, fullScreen);
  
   // this will create a rendering context & window
   bool ok = Video::setDevice( "OpenGL", width, height, bpp, fullScreen );
   if ( ! ok )
   {
        AssertFatal( false, "Could not find a compatible display device!" );
   }
  
   if (platState.appWindow)
   {
      // install handlers to the given window.
      EventTargetRef winTarg = GetWindowEventTarget(platState.appWindow);
   }
   MacCarbInstallCarbonEventHandlers();
}

#pragma mark ---- Platform utility funcs ----
//--------------------------------------
// Web browser function:
//--------------------------------------
bool Platform::openWebBrowser( const char* webAddress )
{
   OSStatus err;
   CFURLRef url = CFURLCreateWithBytes(NULL,(UInt8*)webAddress,dStrlen(webAddress),kCFStringEncodingASCII,NULL);
   err = LSOpenCFURLRef(url,NULL);
   CFRelease(url);

   // kick out of fullscreen mode, so we can *see* the webpage!
   if(Video::isFullScreen())
      Video::toggleFullScreen();

   return(err==noErr);
}

#pragma mark -
#pragma mark ---- Tests ----

ConsoleFunction(testWindowLevels,void,1,2,"testWindowLevels([lev to set]);")
{
   SInt32 lev;
   Con::printf(" Sheilding window level is %x",CGShieldingWindowLevel());   
   GetWindowGroupLevel(GetWindowGroupOfClass(kUtilityWindowClass),&lev);
   Con::printf("   Utility window level is %x", lev);
   GetWindowGroupLevel(GetWindowGroupOfClass(kUtilityWindowClass),&lev);
   Con::printf("  Floating window level is %x", lev);
   GetWindowGroupLevel(GetWindowGroupOfClass(kAlertWindowClass),&lev);
   Con::printf("     Alert window level is %x", lev);
   
   lev=1;
   if(argc==2)
      lev=dAtoi(argv[1]);
   SetWindowGroupLevel( GetWindowGroupOfClass(kUtilityWindowClass), lev);
   SetWindowGroupLevel( GetWindowGroupOfClass(kFloatingWindowClass), lev);   
   SetWindowGroupLevel( GetWindowGroupOfClass(kAlertWindowClass), lev);
}


ConsoleFunction( testSetWindowTitle, void, 2,4, "")
{
   Platform::setWindowTitle(argv[1]);
}

ConsoleFunction( invertScreenColor, void, 1,1, "")
{
   static bool inverted = false;
   
   CGGammaValue reds[1024];
   CGGammaValue greens[1024];
   CGGammaValue blues[1024];
   U32 numTableEntries;
   
   CGGetDisplayTransferByTable( CGMainDisplayID(), 1024, reds, greens, blues, &numTableEntries);
   
   CGGammaValue newReds[numTableEntries];
   CGGammaValue newGreens[numTableEntries];
   CGGammaValue newBlues[numTableEntries];
   
   for(int i=0; i< numTableEntries; i++)
   {
      newReds[i] = reds[numTableEntries-1-i];
      newGreens[i] = greens[numTableEntries-1-i];
      newBlues[i] = blues[numTableEntries-1-i];      
   }

   CGSetDisplayTransferByTable(CGMainDisplayID(), numTableEntries, newReds, newGreens, newBlues);

}

ConsoleFunction(testAsserts, void, 1,1,"")
{
   AssertFatal(false,"Monsters in my OATMEAL.");
   AssertWarn(false,"Oh sweet mercy, the PAIN... THE PAIN!");
   AssertISV(false,"AAaaah! *GARGLE* *SPUTTER* *WET THUD*");
}



