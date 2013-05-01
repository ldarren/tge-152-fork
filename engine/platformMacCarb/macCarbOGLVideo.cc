//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//
// Portions taken from OpenGL Full Screen.c sample from Apple Computer, Inc.
// (that's where many of the lead helper functions originated from, but code
//  has been significantly changed & revised.)
//-----------------------------------------------------------------------------

#include "platformMacCarb/platformMacCarb.h"
#include "platformMacCarb/platformGL.h"
#include "platformMacCarb/maccarbOGLVideo.h"
#include "console/console.h"
#include "math/mPoint.h"
#include "platform/event.h"
#include "platform/gameInterface.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "core/fileStream.h"
#include "platformMacCarb/macCarbUtil.h"
#include "platformMacCarb/macCarbEvents.h"
#include "dgl/dgl.h"

// TODO: Card Profiling code isn't doing anything.

AGLContext agl_ctx;

//-----------------------------------------------------------------------------------------
// prototypes and globals -- !!!!!!TBD - globals should mostly go away, into platState. 
//-----------------------------------------------------------------------------------------
GLenum dumpAGLDebugStr (void);
GLenum dumpGLDebugStr (void);

void dumpPixelFormatList(AGLPixelFormat fmt);

#pragma mark -

//-----------------------------------------------------------------------------------------
// if error dump agl errors to debugger string, return error
//-----------------------------------------------------------------------------------------
GLenum dumpAGLDebugStr (void)
{
   GLenum err = aglGetError();
   if (err != AGL_NO_ERROR)
      Con::errorf ((char *)aglErrorString(err));
   return err;
}

//-----------------------------------------------------------------------------------------
// if error dump gl errors to debugger string, return error
//-----------------------------------------------------------------------------------------
GLenum dumpGLDebugStr (void)
{
	GLenum err = glGetError();
	if (GL_NO_ERROR != err)
		Con::errorf ((char *)gluErrorString(err));
   return err;
}

#pragma mark -

//-----------------------------------------------------------------------------------------
// Creates a dummy AGL context, so that naughty objects that call OpenGL before the window
//  exists will not crash the game.
//  If for some reason we fail to get a default contet, assert -- something's very wrong.
//-----------------------------------------------------------------------------------------
void initDummyAgl(void)
{
   short               i = 0;
   GLint               attrib[64];
   AGLPixelFormat    fmt;
   AGLContext        ctx;

   // clear the existing agl context
   if(platState.ctx != NULL)
      aglDestroyContext(platState.ctx);
   platState.ctx = NULL;
   agl_ctx = NULL;

   // set up an attribute array for the pixel format.
   attrib [i++] = AGL_RGBA;            // red green blue and alpha
   attrib [i++] = AGL_DOUBLEBUFFER;    // double buffered
   attrib [i++] = AGL_NONE;            // terminate the list.

   // choose a pixel format that works for any current display device, and matches
   // the attributes above as closely as possible.
   fmt = aglChoosePixelFormat(NULL, 0, attrib); 
   dumpAGLDebugStr ();
   AssertFatal(fmt, "Could not find a valid default pixel format in initDummyAgl()");


   dumpPixelFormatList(fmt);
   
   // create an agl context. NULL = don't share resouces with any other context.
   ctx = aglCreateContext (fmt, NULL); 
   dumpAGLDebugStr ();
   AssertFatal(ctx, "Could not create a default agl context in initDummyAgl()");
   
   // make gl calls go to our dummy context
   if (!aglSetCurrentContext (ctx))
   {
      dumpAGLDebugStr ();
      AssertFatal(false,"Could not set a default agl context as the current context.");
   }
   
   // pixel format is no longer needed once the context has been created
   aglDestroyPixelFormat(fmt);

   platState.ctx = ctx;
   agl_ctx = ctx; // maintain up aglMacro.h context

}

GDHandle allDevs[32];
U32      nAllDevs;

AGLPixelFormat findValidPixelFormat(bool fullscreen, U32 bpp, U32 samples, bool recovery = true)
{  
   AssertWarn(bpp==16 || bpp==32 || bpp==0, "An unusual bit depth was requested in findValidPixelFormat(). clamping to 16|32");
   if(bpp)
      bpp = bpp > 16 ? 32 : 16;
   
   AssertWarn(samples <= 6, "An unusual multisample depth was requested in findValidPixelFormat(). clamping to 0...6");
   samples = samples > 6 ? 6 : samples;
   
   // create an agl pixel format
   int            i = 0;
   GLint          attr[64];
   AGLPixelFormat fmt;
   
   // basic hardware accelerated format options
   attr[i++] = AGL_RGBA;         // red green blue and alpha format
   attr[i++] = AGL_DOUBLEBUFFER; // double buffered format
   attr[i++] = AGL_ACCELERATED;  // ask for hardware acceleration
   attr[i++] = AGL_NO_RECOVERY;  // prohibit use of a software rendering fallback
   // request fullscreen capable format if needed
   if(fullscreen)
      attr[i++] = AGL_FULLSCREEN;
   
   if(bpp != 0)
   {
      // native pixel formats are argb 1555 & argb 8888.
      U32 colorbits = 0;
      U32 alphabits = 0;
      if(bpp == 16)
      {
         colorbits = 5;             // ARGB 1555
         alphabits = 1;
      }
      else if(bpp == 32)
         colorbits = alphabits = 8; // ARGB 8888
         
      attr[i++] = AGL_DEPTH_SIZE;
      attr[i++] = bpp;
      attr[i++] = AGL_PIXEL_SIZE;
      attr[i++] = bpp;
      
      attr[i++] = AGL_RED_SIZE;
      attr[i++] = colorbits;
      attr[i++] = AGL_GREEN_SIZE;
      attr[i++] = colorbits;
      attr[i++] = AGL_BLUE_SIZE;
      attr[i++] = colorbits;
      attr[i++] = AGL_ALPHA_SIZE;
      attr[i++] = alphabits;
   }

   if(samples != 0)
   {
      attr[i++] = AGL_SAMPLE_BUFFERS_ARB; // number of multisample buffers
      attr[i++] = 1;                      // currently only 1 is supported
      attr[i++] = AGL_SAMPLES_ARB;        // number of samples per pixel
      attr[i++] = samples + samples % 2;  // must be 2, 4, 6 ...
   }
   
   attr[i++] = AGL_NONE;         // null-terminate the list
   
   fmt = aglChoosePixelFormat(&platState.hDisplay, 1, attr);
   dumpAGLDebugStr();

   if(!fmt && recovery)
   {
      // recovery.
      // first try ignoring samples. then try ignoring the bit depth.
      // we'll try the following order:
      // specified bit depth, specified multisample depth
      // specified bit depth, lower/no multisample
      // any bit depth, specified multisample depth
      // any bit depth, lower/no multisample
      
      // downgrade multisample, trying for a valid format
      for(int fewerSamples = samples - 1; !fmt && fewerSamples >=0; fewerSamples--)
      {
         Con::errorf("error selecting pixel format, trying again with %s, bpp=%i, multisample level=%i",
            fullscreen ? "fullscreen" : "windowed", bpp, fewerSamples);
         fmt = findValidPixelFormat(fullscreen, bpp, fewerSamples, false);
      }

      // allow any bit depth, send in requested multisample level, and ALLOW RECOVERY.
      // if this attempt fails, we'll hit the above 'downgrade multisample' block in the recursion.
      if(!fmt && bpp != 0)
      {
         Con::warnf("error selecting pixel format, trying again with %s, bpp=%i, multisample level=%i",
               fullscreen ? "fullscreen" : "windowed", 0, samples);
         fmt = findValidPixelFormat(fullscreen, 0, samples, true);
      }
   }
   
   return fmt;
}

void dumpPixelFormat(AGLPixelFormat fmt)
{
   Con::printf(" ------------------------------------------------------------");
   Con::printf("Describing pixel format 0x%x", fmt);
   GLint val;
#define DumpAGLPixelFormatAttr(attr) \
   aglDescribePixelFormat(fmt, AGL_##attr, &val); \
   dumpAGLDebugStr(); \
   Con::printf("  %20s %4i", #attr ,val);

   DumpAGLPixelFormatAttr(ALL_RENDERERS);
   DumpAGLPixelFormatAttr(BUFFER_SIZE);
   DumpAGLPixelFormatAttr(LEVEL);
   DumpAGLPixelFormatAttr(RGBA);
   DumpAGLPixelFormatAttr(DOUBLEBUFFER)
   DumpAGLPixelFormatAttr(STEREO);
   DumpAGLPixelFormatAttr(AUX_BUFFERS);
   DumpAGLPixelFormatAttr(GREEN_SIZE);
   DumpAGLPixelFormatAttr(RED_SIZE);   
   DumpAGLPixelFormatAttr(BLUE_SIZE);
   DumpAGLPixelFormatAttr(ALPHA_SIZE);
   DumpAGLPixelFormatAttr(DEPTH_SIZE);
   DumpAGLPixelFormatAttr(STENCIL_SIZE);
   DumpAGLPixelFormatAttr(ACCUM_RED_SIZE);
   DumpAGLPixelFormatAttr(ACCUM_GREEN_SIZE);
   DumpAGLPixelFormatAttr(ACCUM_BLUE_SIZE);
   DumpAGLPixelFormatAttr(ACCUM_ALPHA_SIZE);
   
   DumpAGLPixelFormatAttr(PIXEL_SIZE);
   DumpAGLPixelFormatAttr(MINIMUM_POLICY);
   DumpAGLPixelFormatAttr(MAXIMUM_POLICY);
   DumpAGLPixelFormatAttr(OFFSCREEN);
   DumpAGLPixelFormatAttr(FULLSCREEN);
   DumpAGLPixelFormatAttr(SAMPLE_BUFFERS_ARB);
   DumpAGLPixelFormatAttr(SAMPLES_ARB);
   DumpAGLPixelFormatAttr(AUX_DEPTH_STENCIL);
   DumpAGLPixelFormatAttr(COLOR_FLOAT);
   DumpAGLPixelFormatAttr(MULTISAMPLE);
   DumpAGLPixelFormatAttr(SUPERSAMPLE);
   DumpAGLPixelFormatAttr(SAMPLE_ALPHA);

#undef DumpAGLPixelFormatAttr
   Con::printf(" ------------------------------------------------------------");
}

void dumpPixelFormatList(AGLPixelFormat fmt)
{
   Con::printf("Dumping list of pixel formats:");
   while(fmt!=NULL)
   {
      dumpPixelFormat(fmt);
      fmt = aglNextPixelFormat(fmt);
   }
}

#pragma mark -
//------------------------------------------------------------------------------
OpenGLDevice::OpenGLDevice()
{
   // Set the device name:
   mDeviceName = "OpenGL";

   // macs games are not generally full screen only
   mFullScreenOnly = false;
}

//------------------------------------------------------------------------------
void OpenGLDevice::initDevice()
{
   // pick a monitor to run on
   enumMonitors();

   // choose a monitor at save it in the plat state
   platState.hDisplay = chooseMonitor();
   platState.cgDisplay = MacCarbGetCGDisplayFromQDDisplay(platState.hDisplay);
   
   // figure out & cache what Resolution's this card-monitor combo can support
   enumDisplayModes(platState.cgDisplay);
}

//------------------------------------------------------------------------------
//  Fill Vector<Resolution> mResoultionList with list of supported modes
//------------------------------------------------------------------------------
bool OpenGLDevice::enumDisplayModes(CGDirectDisplayID display)
{
   mResolutionList.clear();
   
   // get the display, and the list of all available modes.
   CFArrayRef modeArray = CGDisplayAvailableModes(display);
   if(!modeArray)
   {
      // we're probably in headless mode. still, best not to leave the list emtpy.
      Resolution headless( 640, 480, 32 );
      mResolutionList.push_back( headless );
      return false;
   }
   
   int len = CFArrayGetCount(modeArray);
   for(int i=0; i < len; i++)
   {
      // get this mode.
      CFNumberRef num;
      int width, height, bpp;
      CFDictionaryRef mode = (CFDictionaryRef) CFArrayGetValueAtIndex(modeArray,i);
      
      // get width
      num = CFDictionaryGetValue( mode, kCGDisplayWidth );
      CFNumberGetValue(num, kCFNumberLongType,&width);
      
      // get height
      num = CFDictionaryGetValue( mode, kCGDisplayHeight );
      CFNumberGetValue(num, kCFNumberLongType,&height);
      
      // get bpp
      num = CFDictionaryGetValue( mode, kCGDisplayBitsPerPixel );
      CFNumberGetValue(num, kCFNumberLongType,&bpp);
      
      // add to the list
      if( bpp != 8 )
      {
         Resolution newRes( width, height, bpp );
         mResolutionList.push_back( newRes );
      }
   }


//   // fill it with some standard sizes for a machine, just to have them available.
//   Point sizes[4] = {{640,480},{800,600},{1024,768},{1280,1024}};
//   for(int i=0; i<4; i++)
//   {
//      // Point is { short v, short h }, so we have to reverse the h and v here, sorry for any confusion.
//      Resolution newRes16( sizes[i].v, sizes[i].h, 16 );
//      Resolution newRes32( sizes[i].v, sizes[i].h, 32 );
//      mResolutionList.push_back( newRes16 );
//      mResolutionList.push_back( newRes32 );
//   }
   
   return true;
}

//------------------------------------------------------------------------------
//  Fill mMonitorList with list of supported modes
//   Guaranteed to include at least the main device.
//------------------------------------------------------------------------------
bool OpenGLDevice::enumMonitors()
{
   // DMGetFirstScreenDevice() et al are deprecated as of 10.4, but we need them
   // for AGL, which operates on GDHandle's for displays.
   // As far as I know, you can get a CGDirectDisplayID from a GDHandle, 
   // but not the other way around.
   mMonitorList.clear();
   GDHandle dev = DMGetFirstScreenDevice( true );
   nAllDevs = 0;
   while( dev )
   {
      Con::printf( "  active displays = 0x%x  CGDisplayID = 0x%x", dev, MacCarbGetCGDisplayFromQDDisplay(dev));
      mMonitorList.push_back(dev);
      allDevs[nAllDevs++] = dev;
      dev = DMGetNextScreenDevice( dev, true);
   }   

   return true;
}

//------------------------------------------------------------------------------
// Chooses a monitor based on $pref, on the results of tors(), & on the
// current window's screen.
//------------------------------------------------------------------------------
GDHandle OpenGLDevice::chooseMonitor()
{
   // TODO: choose monitor based on which one contains most of the window.
   // NOTE: do not call cleanup before calling choose, or we won't have a window to consider.
   AssertFatal(!mMonitorList.empty(), "Cannot choose a monitor if the list is empty!");

   U32 monNum = Con::getIntVariable("$pref::Video::monitorNum", 0);
   if(monNum >= mMonitorList.size())
   {
      Con::errorf("invalid monitor number %i", monNum);
      monNum = 0;
      Con::setIntVariable("$pref::Video::monitorNum", 0);
   }
   Con::printf("using display 0x%x", mMonitorList[monNum]);
   return mMonitorList[monNum];
}

//------------------------------------------------------------------------------
// Activate
//  this is called once, as a result of createCanvas() in scripts.
//  dumps OpenGL driver info for the current screen
//  creates an initial window via setScreenMode
bool OpenGLDevice::activate( U32 width, U32 height, U32 bpp, bool fullScreen )
{
   Con::printf( " OpenGLDevice activating..." );
   
   // Never unload a code module. This makes destroying & recreating contexts faster.
   aglConfigure(AGL_RETAIN_RENDERERS, GL_TRUE);

   // gets opengl rendering capabilities of the screen pointed to by platState.hDisplay
   // sets up dgl with the capabilities info, & reports opengl status.
   getGLCapabilities();

   // Create the window or capture fullscreen
   if(!setScreenMode( width, height, bpp, fullScreen, true, false ))
      return false;

   // set the displayDevice pref to "OpenGL"
   Con::setVariable( "$pref::Video::displayDevice", mDeviceName );   

   // set vertical sync now because it doesnt need setting every time we setScreenMode()
   setVerticalSync( !Con::getBoolVariable( "$pref::Video::disableVerticalSync" ));
   
   return true;
}

//------------------------------------------------------------------------------
// returns TRUE if textures need resurrecting in future...
//------------------------------------------------------------------------------
bool OpenGLDevice::cleanupContextAndWindow()
{
   bool needResurrect = false;
   
   Con::printf( "Cleaning up the display device..." );

   // Delete the rendering context and it's specific data.
   if (platState.ctx)
   {
      // The OpenGL texture handles are specific to each context.
      // We'll need to get new ones for a new context, so kill the texture manager to clear 'em.
      if (!Video::smNeedResurrect) 
      {
         Con::printf( "Killing the texture manager..." );
         Game->textureKill();
         needResurrect = true;
      }
      
      // make the agl context not-current, which stops openGL calls from going anywhere.
      Con::printf( "Clearing the current AGL context..." );
      aglSetCurrentContext(NULL);
#if defined(USE_AGL_MACRO)
      agl_ctx = NULL; // maintain aglMacro.h context
#endif

      // detatch the agl context from it's window
      Con::printf( "Clearing the current drawable..." );
      aglSetDrawable(platState.ctx, NULL);

      // delete the context
      Con::printf( "Deleting the rendering context..." );
      aglDestroyContext(platState.ctx);

      // clear our handle to the context
      platState.ctx = NULL;
   }

   // delete the app window if it exists
   if ( platState.appWindow )
   {
      Con::printf( "Destroying the window..." );
      MacCarbFadeAndReleaseWindow(platState.appWindow);
      platState.appWindow = NULL;
   }
   
   // clear the Resolution state, so setScreenMode() will know not to early-out.
   smCurrentRes = Resolution(0,0,0);
   
   return(needResurrect);
}


//------------------------------------------------------------------------------
void OpenGLDevice::shutdown()
{
   Con::printf( "Shutting down the OpenGL display device..." );

   // clean up the context, the window, and kill the texture manager
   cleanupContextAndWindow();
}


//------------------------------------------------------------------------------
// This is the real workhorse function of the DisplayDevice...
//
bool OpenGLDevice::setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt, bool repaint )
{
   Con::printf(" set screen mode %i x %i x %i, %s, %s, %s",width, height, bpp,
               fullScreen  ? "fullscreen" : "windowed",
               forceIt     ? "force it" : "dont force it",
               repaint     ? "repaint"  : "dont repaint"
               );
               
   // validation, early outs --------------------------------------------------
   // sanity check. some scripts are liable to pass in bad values.
   if(!bpp)
      bpp = platState.desktopBitsPixel;
      
   Resolution newRes = Resolution(width, height, bpp);

   // if no values changing and we're not forcing a change, kick out. prevents thrashing.
   if(!forceIt && smIsFullScreen == fullScreen && smCurrentRes == newRes)
      return(true);

   // we have a new context, this is now safe to do:
   // delete any contexts or windows that exist, and kill the texture manager.
   bool needResurrect = cleanupContextAndWindow();

   Con::printf( ">> Attempting to change display settings to %s %dx%dx%d...", 
         fullScreen?"fullscreen":"windowed", newRes.w, newRes.h, newRes.bpp );

   // monitor selection -------------------------------------------------------
   // set our preferred monitor. default is the screen that has the menu bar.
   GDHandle prevDisplay = platState.hDisplay;
   platState.hDisplay = chooseMonitor();
   platState.cgDisplay = MacCarbGetCGDisplayFromQDDisplay(platState.hDisplay);
   AssertFatal(platState.hDisplay,"We chose a null monitor? Panic!");
   
   // if we're changing screens, we must know what the new one is capable of
   if(prevDisplay != platState.hDisplay)
   {
      enumDisplayModes(platState.cgDisplay);
      getGLCapabilities();
   }
   
   // create an agl rendering context ------------------------------------------
   AGLPixelFormat fmt = NULL;
   AGLContext     ctx = NULL;
   
   // select pixel format. fall back to more generic options until we get something.
   fmt = findValidPixelFormat(fullScreen, bpp, 0);
   AssertFatal(fmt, "We utterly failed to choose a valid AGL pixel format.");
   
   // print out the pixel format list we got.
   //dumpPixelFormatList(fmt);
   
   // create the agl rendering context
   ctx = aglCreateContext(fmt, NULL);
   dumpAGLDebugStr();
   AssertISV( ctx, "We could not create a valid AGL rendering context.");
   if(!ctx)
      return false;

   // format is not needed once we have a context.
   aglDestroyPixelFormat(fmt);
   
   if(fullScreen && platState.captureDisplay)
   {
      // capture main display & go to full screen mode
      // TODO: allow frequency selection?
      aglSetFullScreen(ctx, newRes.w, newRes.h, 0, 0);
      dumpAGLDebugStr();
      Con::printf("set AGL fullscreen");
      MacCarbShowMenuBar(false);
   }
   else
   {
      // create a window, get it's drawable, and attach our context to the window
      bool isFullscreenWindow = (fullScreen && !platState.captureDisplay);
      platState.appWindow = MacCarbCreateOpenGLWindow( platState.hDisplay, newRes.w, newRes.h, isFullscreenWindow );
      if(!platState.appWindow)
      {
         Con::errorf("OpenGLDevice::setScreenMode - Failed to create a new window!");
         return false;
      }
      CGrafPtr drawable = GetWindowPort(platState.appWindow);
      aglSetDrawable(ctx, drawable);
      dumpAGLDebugStr();
      Con::printf("Set up AGL windowed support");
   }
   
   // send opengl commands to this context.
   aglSetCurrentContext(ctx);
   #if defined(USE_AGL_MACRO)
      agl_ctx = ctx; // maintain aglMacro.h context
   #endif
   // save the context
   platState.ctx = ctx;

   // clear out garbage from the gl window.
   glClearColor(0,0,0,1);
   glClear(GL_COLOR_BUFFER_BIT );
   Con::printf("Cleared gl buffers");

   // set opengl options & other options ---------------------------------------
   // ensure data is packed tightly in memory. this defaults to 4.
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   // TODO: set gl arb multisample enable & hint
   dglSetFSAASamples(gFSAASamples);
   // set vertical sync
   setVerticalSync(!Con::getBoolVariable( "$pref::Video::disableVerticalSync" ));

   // set torque variables ----------------------------------------------------
   // save window size for dgl
   Platform::setWindowSize( newRes.w, newRes.h );
   // update smIsFullScreen and pref
   smIsFullScreen = fullScreen;
   Con::setBoolVariable( "$pref::Video::fullScreen", smIsFullScreen );
   // save resolution
   smCurrentRes = newRes;
   // save resolution to prefs
   char buf[32];
   if(fullScreen)
   {
      dSprintf( buf, sizeof(buf), "%d %d %d", newRes.w, newRes.h, newRes.bpp);
      Con::setVariable("$pref::Video::resolution", buf);
   }
   else
   {
      dSprintf( buf, sizeof(buf), "%d %d", newRes.w, newRes.h);
      Con::setVariable("$pref::Video::windowedRes", buf);
   }
   
   // fade the window into existance, asynchronously
   if(platState.appWindow)
      MacCarbFadeInWindow(platState.appWindow);

   
   // begin rendering again ----------------------------------------------------
   if( needResurrect )
   {
      // Reload the textures gl names
      Con::printf( "Resurrecting the texture manager..." );
      Game->textureResurrect();
   }

   // reattach the event handlers to the new window.
   MacCarbRemoveCarbonEventHandlers();
   MacCarbInstallCarbonEventHandlers();

   if( repaint )
      Con::evaluate( "resetCanvas();" );

   return true;
}


//------------------------------------------------------------------------------
void OpenGLDevice::swapBuffers()
{
   if (platState.ctx)
      aglSwapBuffers(platState.ctx); 
#if defined(TORQUE_DEBUG)
   if(gOutlineEnabled)
      glClear(GL_COLOR_BUFFER_BIT);
#endif
}  


//------------------------------------------------------------------------------
const char* OpenGLDevice::getDriverInfo()
{
   // Prepare some driver info for the console:
   const char* vendorString   = (const char*) glGetString( GL_VENDOR );
   const char* rendererString = (const char*) glGetString( GL_RENDERER );
   const char* versionString  = (const char*) glGetString( GL_VERSION );
   const char* extensionsString = (const char*) glGetString( GL_EXTENSIONS );
   
   U32 bufferLen = ( vendorString ? dStrlen( vendorString ) : 0 )
                 + ( rendererString ? dStrlen( rendererString ) : 0 )
                 + ( versionString  ? dStrlen( versionString ) : 0 )
                 + ( extensionsString ? dStrlen( extensionsString ) : 0 )
                 + 4;

   char* returnString = Con::getReturnBuffer( bufferLen );
   dSprintf( returnString, bufferLen, "%s\t%s\t%s\t%s", 
         ( vendorString ? vendorString : "" ),
         ( rendererString ? rendererString : "" ),
         ( versionString ? versionString : "" ),
         ( extensionsString ? extensionsString : "" ) );
   
   return( returnString );   
}

typedef struct MacCarbGamma
{
   F32 r, g, b;
   F32 scale;
};

static MacCarbGamma _MacGamma;
//------------------------------------------------------------------------------
bool OpenGLDevice::getGammaCorrection(F32 &g)
{
   // rgb gamma exponents
   CGGammaValue red,green,blue;
   // rgb min & max gamma. we'll ignore these for the present.
   CGGammaValue rm, rx, gm, gx, bm, bx; 
   // grab the gamma values
   CGDirectDisplayID display = platState.cgDisplay;
   CGGetDisplayTransferByFormula(display, &rm, &rx, &red, &gm, &gx, &green, &bm, &bx, &blue);

   // save the original gamma vals, and the current scale.
   static bool once = true;
   if(once)
   {
      once = false;
      _MacGamma.r = red;
      _MacGamma.g = green;
      _MacGamma.b = blue;
      _MacGamma.scale = 1.0;
   }
   
   g = _MacGamma.scale;
   return true;
}    

//------------------------------------------------------------------------------
bool OpenGLDevice::setGammaCorrection(F32 g)
{
   // revert to default colorsync settings if g approaches 1.0
   F32 epsilon = 0.01f;
   if( mFabs(g - 1.0) <= epsilon )
   {
      CGDisplayRestoreColorSyncSettings();
      return false;
   }
   
   // rgb gamma exponents
   CGGammaValue red,green,blue;
   // rgb min & max gamma. we'll ignore these for the present.
   CGGammaValue rm, rx, gm, gx, bm, bx; 
   CGGetDisplayTransferByFormula(platState.cgDisplay, &rm, &rx, &red, &gm, &gx, &green, &bm, &bx, &blue);
   // scale the original gamma values.
   red   = _MacGamma.r * g;
   green = _MacGamma.g * g;
   blue  = _MacGamma.b * g;
   _MacGamma.scale = g;
   // set the gamma.
   CGSetDisplayTransferByFormula(platState.cgDisplay, rm, rx, red, gm, gx, green, bm, bx, blue);
   
   return true;
}

//------------------------------------------------------------------------------
bool OpenGLDevice::setVerticalSync( bool on )
{
   if (!platState.ctx)
      return false;

   bool ret = aglSetInteger(platState.ctx, AGL_SWAP_INTERVAL, (GLint*)&on);
   return ret;
}

//------------------------------------------------------------------------------
DisplayDevice* OpenGLDevice::create()
{
   // set up a dummy default agl context.
   // this will be replaced later with the window's context,
   // but we need agl_ctx to be valid at all times,
   // since some things try to make gl calls before the device is activated.
   initDummyAgl();

   // create the DisplayDevice
   OpenGLDevice* newOGLDevice = new OpenGLDevice();
   
   // gather monitor & resolution info
   // delegated out to initDevice() in case the display config changes.
   newOGLDevice->initDevice();
   
   return newOGLDevice;
}

#pragma mark -

Resolution Video::getDesktopResolution()
{
   Resolution res;
   Rect r = (**platState.hDisplay).gdRect;
   res.w = r.right - r.left;
   res.h = r.bottom - r.top;
   res.bpp = (**(**platState.hDisplay).gdPMap).pixelSize;
   
   platState.desktopWidth  = res.w;
   platState.desktopHeight = res.h;
   platState.desktopBitsPixel = res.bpp;

   return res;
}

#pragma mark -

ConsoleFunction( dumpDisplayIDs, void, 1,1, "")
{
   Con::errorf("=-== Dumping display ids =-==");
   GDHandle dev = GetMainDevice();
   Con::printf("main display GetMainDevice = 0x%x  CGDisplayID = 0x%x", dev, MacCarbGetCGDisplayFromQDDisplay(dev));

   dev = DMGetFirstScreenDevice( true );
   Con::printf( "first active display = 0x%x  CGDisplayID = 0x%x", dev, MacCarbGetCGDisplayFromQDDisplay(dev));
   while( ( dev = DMGetNextScreenDevice( dev, true)) != NULL)
   {
      Con::printf( "  active displays = 0x%x  CGDisplayID = 0x%x", dev, MacCarbGetCGDisplayFromQDDisplay(dev));
   }   


   CGDirectDisplayID  mainid = CGMainDisplayID();
   Con::printf("main display CGMainDisplayID = 0x%x", mainid);
 
   CGDirectDisplayID lastDisplay, displayArray[64] ;
   CGDisplayCount numDisplays ;   

   CGGetActiveDisplayList( 64, displayArray, &numDisplays );
   for(int i=0; i< numDisplays; i++)
   {
      mainid = displayArray[i];
      Con::printf(" CG active display list CGMainDisplayID = 0x%x", mainid);
     
   }
}

