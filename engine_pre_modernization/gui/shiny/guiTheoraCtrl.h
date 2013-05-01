//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUITHEORACTRL_H_
#define _GUITHEORACTRL_H_

#ifndef _THEORATEXTURE_H_
#include "core/theoraPlayer.h"
#endif

/// Play back a Theora video file.
class GuiTheoraCtrl : public GuiControl
{
  private:
   typedef GuiControl Parent;

  protected:
   const char* mFilename;
   TheoraTexture mTheoraTexture;

  public:
   DECLARE_CONOBJECT(GuiTheoraCtrl);

   GuiTheoraCtrl();
   ~GuiTheoraCtrl();

   static void initPersistFields();

   void inspectPostApply();
   void setFile(const char *filename);
   void stop();

   /// If true, stop video playback when the control goes to sleep.
   bool mStopOnSleep;

   /// Are we done with playback?
   bool mDone;

   /// Our background color.
   ColorI mBackgroundColor;

   bool onWake();
   void onSleep();
   void onRender(Point2I offset, const RectI &updateRect);

   F32 getCurrentTime()
   {
      return mTheoraTexture.getCurrentTime();
   }
};

#endif
