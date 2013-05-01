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
   const char*    mFilename;        ///< Currently playing theora file
   const char*    mSeekFileName;    ///< Seek index file
   TheoraTexture  mTheoraTexture;   ///< Theora stream gets unpacked and decoded to a texture 
   bool           mSaveSeekInfo;    ///< Write out seek info? (Note won't be able to seek during a recording)
   bool           mLoop;            ///< Loop to start automatically when playback gets to last frame
   bool           mStopOnSleep;     ///< If true, stop video playback when the control goes to sleep.   
   bool           mDone;            ///< Are we done with playback?   
   ColorI         mBackgroundColor; ///< Our background color.

  public:
   DECLARE_CONOBJECT(GuiTheoraCtrl);

   GuiTheoraCtrl();
   ~GuiTheoraCtrl();
   
   // DARREN MOD: allow movie skip when esc press
   virtual bool onKeyDown(const GuiEvent &event);

   static void initPersistFields();
   static bool setLoopProtected( void *obj, const char *data )
   {
      static_cast<GuiTheoraCtrl*>( obj )->setLoop( dAtob( data ) );

      return true;
   }

   ///
   void inspectPostApply();
   
   /// Set the theora file to play
   bool setFile( const char *filename, const char *seekFile = 0, bool saveSeek = false );

   /// Start or continue playback
   void play();

   /// Stop playback
   void stop();

   /// Set whether to loop the playback when we get to the last frame
   void setLoop( bool flag )
   {
      mTheoraTexture.setLoop( flag );
   }

   /// Overrides
   bool onWake();
   void onSleep();
   void onRender(Point2I offset, const RectI &updateRect);

   /// Get the current time in seconds
   F32 getCurrentTime()
   {
      return mTheoraTexture.getCurrentTime();
   }
   
   /// Set the current time to start playback at in seconds
   bool setCurrentTime( F64 time )
   {
      return mTheoraTexture.setCurrentTime( time );
   }

   /// Get the current time in seconds
   F32 getCurrentVTime() const
   {
      return mTheoraTexture.getCurrentVTime();
   }

   /// Get current frame
   S32 getCurrentFrame() const
   {
      return mTheoraTexture.getCurrentFrame();
   }

   /// Set current frame
   bool setCurrentFrame( S32 frame ) 
   {
      return mTheoraTexture.setCurrentFrame( frame );
   }
};

#endif
