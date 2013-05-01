//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _MACCARBOGLVIDEO_H_
#define _MACCARBOGLVIDEO_H_

#include "platform/platformVideo.h"

class OpenGLDevice : public DisplayDevice
{
private:

   /// Handles to all the devices ( monitors ) attached to the system.
   Vector<GDHandle> mMonitorList;
   /// Gamma value
   F32 mGamma;
   
   
   /// Fills mResolutionList with a list of valid resolutions for a particular screen
   bool enumDisplayModes(CGDirectDisplayID hDevice);
   /// Fills mMonitorList with all available monitors
   bool enumMonitors();
   /// Chooses a monitor based on $pref::the results of enumMontors()
   GDHandle chooseMonitor();
   /// Cleans up the opengl context, and destroys the rendering window
   bool cleanupContextAndWindow();

public:
   OpenGLDevice();
   static DisplayDevice* create();

   /// The following are inherited from DisplayDevice
   void initDevice();
   bool activate( U32 width, U32 height, U32 bpp, bool fullScreen );
   
   void shutdown();
   
   bool setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt = false, bool repaint = true );
   void swapBuffers();
   
   const char* getDriverInfo();
   bool getGammaCorrection(F32 &g);
   bool setGammaCorrection(F32 g);
   bool setVerticalSync( bool on );
};

#endif // _MACCARBOGLVIDEO_H_
