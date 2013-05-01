//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _THEORATEXTURE_H_
#define _THEORATEXTURE_H_

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

#ifndef _GTEXMANAGER_H_
#include "dgl/gTexManager.h"
#endif

#ifndef _GBITMAP_H_
#include "dgl/gBitmap.h"
#endif

#ifndef _O_THEORA_H_
#include "theora/theora.h"
#endif

#ifndef _vorbis_codec_h_
#include "vorbis/codec.h"
#endif

#ifndef _PLATFORMTHREAD_H_
#include "platform/platformThread.h"
#endif

#ifndef _OGGMIXEDSTREAMSOURCE_H_
#include "audio/oggMixedStreamSource.h"
#endif

#ifndef _VORBISSTREAMSOURCE_H_
#include "audio/vorbisStreamSource.h"
#endif

/// TheoraTexture decodes Ogg Theora files, and their audio.
///
/// TheoraTexture objects can be used similarly to TextureObjects. Just
/// set the video, call play(), and then refresh every frame to get the
/// latest video. Audio happens automagically.
///
/// @note Uses Theora and ogg libraries which are Copyright (C) Xiph.org Foundation
class TheoraTexture
{
private:
	Thread*				mPlayThread;
	
	/// Ogg and codec state for demux/decode.
	ogg_sync_state		mOggSyncState;
	ogg_page			   mOggPage;
	ogg_stream_state	mOggTheoraStream;
	ogg_stream_state	mOggVorbisStream;

	theora_info			mTheoraInfo;
	theora_comment		mTheoraComment;
	theora_state		mTheoraState;
		
	vorbis_info			mVorbisInfo;
	vorbis_comment		mVorbisComment;
	vorbis_dsp_state	mVorbisDspState;
	vorbis_block		mVorbisBlock;

	/// File handle for the theora file.
	Stream*				mTheoraFile;

	volatile bool		mReady;
	volatile bool		mPlaying;

	volatile bool		mHasVorbis;
	OggMixedStreamSource* mVorbisBuffer;
	AUDIOHANDLE			mVorbisHandle;

   volatile F32      mCurrentTime;
   volatile U32      mStartTick;

	void init();

	bool parseHeaders();
	bool createVideoBuffers();
	bool createAudioBuffers(Audio::Description* desc);

   /// We precalculate adjusted YUV values for faster RGB conversion. This
   /// function is responsible for making sure they're present and valid.
	void generateLookupTables();
	void destroyTexture(bool restartOgg = false);
	
	void drawFrame();

	bool readyVideo(const F64 lastFrame, F64 &vBuffTime);
	bool readyAudio();
	
	bool demandOggPage();
	S32  bufferOggPage();
	S32  queueOggPage(ogg_page *page);

	F64 getTheoraTime();

	/// Background playback thread.
	static void playThread( void *udata );
	bool playLoop();

   /// Magical Trevor is responsible for tracking elapsed time based on
   /// the currently playing buffer.
   ///
   /// @note He's ever so clear.
   ///
   /// Basically it takes periodic updates of the currently playing buffer
   /// and figures the time between that buffer and the last tracked
   /// buffer, and tells you how much time passed between those two events.
   /// (time == duration of audio)
   struct MagicalTrevor
   {
      struct BufInf
      {
         BufInf();

         ALuint id;
         F64    time;
         BufInf *next;
      };

      FreeListChunker<BufInf> mBuffPool;
      BufInf *mBufListHead;
      ALint mLastBufferID;
      void *mMutex;

      MagicalTrevor();
      ~MagicalTrevor();

      /// Given current buffer, infer elapsed time since last call.
      const F64 advanceTime(const ALuint buffId);

      /// Note a buffer, and how much time it contains.
      void postBuffer(const ALuint buffId, const F64 duration);

      /// How many items in our list? Used for debugging.
      const U32 getListSize() const;

      /// Clear everything out in preparation for the next playback.
      void reset();
   };

   MagicalTrevor mMagicalTrevor;

public:
   TheoraTexture();
   TheoraTexture(const char *filename, bool play = false, Audio::Description* desc = NULL);
   ~TheoraTexture();

   operator TextureObject*()
   {
      return (TextureObject*)*mTextureHandle;
   }

   bool setFile(const char*, bool play = false, bool restart = false, Audio::Description* desc = NULL);

   const bool isPlaying() const { return mPlaying; }
   const bool isReady() const { return mReady; }
   bool play();
   void stop();
   bool pause();

   /// Update the GL texture from the bitmap the background thread writes to.
   void refresh();
   U32  getGLName();

   const F64 getCurrentTime()
   {
      return getTheoraTime();
   }

   TextureHandle*	mTextureHandle;
};

#endif
