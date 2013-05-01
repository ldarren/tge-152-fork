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
   void*             mMutex;

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
   StringTableEntry  mTheoraFileName;
   StringTableEntry  mSeekFileName;
   bool              mSavingSeekFile;
	   
	volatile bool		      mHasVorbis;
	OggMixedStreamSource*   mVorbisBuffer;
	AUDIOHANDLE			      mVorbisHandle;

   volatile bool		mReady;           ///< Texture ready for rendering?
	volatile bool     mLoop;            ///< Loop to start when we get to end?
   volatile bool		mPlaying;         ///< Are we currently playing?
   volatile bool     mHasLooped;       ///< Flag set if the movie hit the end and restarted

   volatile F64      mCurrentTime;     ///< Current playback time
   volatile F64      mCurrentVTime;    ///< Current video playback time
   volatile S32      mCurrentFrame;    ///< Current playback frame
   volatile S32      mLastKeyFrame;    ///< Last key frame before current frame
   volatile S32      mLastFrame;
   volatile bool     mFrameChanged;    ///< Has the frame changed since last call?
   volatile S32      mLastPageOffset;  ///< Offset into file stream when last page was read
   volatile U32      mStartTick;       ///< Start of playback
   volatile F64      mLastFrameTime;   ///< Time at which last frame was decoded
   volatile S32      mStreamStart;     ///< Start of data after headers
         
   F64               mSeekTo;          ///< We pass a frame or time to seek to for the play thread
   bool              mSeekTypeFrame;   ///< Did we pass a fram or a time to seek to?

   /// Clear out all vars 
	void init();

   /// Read theora and vorbis headers from ogg stream
	bool parseHeaders();

   /// Create bitmap and texture
	bool createVideoBuffers();
   
   /// Create buffers for audio and initialize
	bool createAudioBuffers(Audio::Description* desc);

   /// We precalculate adjusted YUV values for faster RGB conversion. This
   /// function is responsible for making sure they're present and valid.
	void generateLookupTables();

   /// Clear out all buffers, decoders, everything...
	void destroyTexture( bool restartOgg = false );
	
	/// Decode one frame into our texture
   void drawFrame( bool decodeOnly = false );
   
   /// Set up a frame of video
   bool readyVideo( const F64 lastFrame, ogg_packet *packet, F64 &vBuffTime, bool &frameChanged );

   /// Decode one frame of video
   S32 decodeVideoFrame( ogg_packet *packet, S32 &frame, F64 &time );

   /// Check the frame values for the current packet
   /// @returns   true if frame changed, else false
   bool checkFrameChange( ogg_packet *packet, S32 frame, F64 time );

   /// Buffer samples of audio for playback
   bool readyAudio( bool decodeOnly = false );

   /// Read the next vorbis audio packet from the ogg stream
   bool nextAudioPacket( ogg_packet *packet );
   
   /// Decode the buffered samples (if any)
   S32 decodeAudioSamples( S16 *outbuf, S32 size );

   /// Post sample buffer to audio system
   bool postNextAudioBuffer( S16 *samples, S32 count );
	
	/// Read a page from the sync layer and submit to decoder
   bool demandOggPage( ogg_page *page );
   
   /// Read data from stream and submit to sync layer
	S32  bufferOggPage();	
   
   /// Submit next page to either theora or vorbis streams
   S32  queueOggPage( ogg_page *page );

   /// Find the next Ogg page in the file stream
   S32 nextOggPage( ogg_page *page );

   /// Find the prev Ogg page in the file stream
   S32  prevOggPage( ogg_page *page );
   
   /// Get the playback time in seconds
	F64  getTheoraTime();
   
   /// Set the time to start playback from
   bool setTheoraTime( F64 time );

   /// Set the frame to start playback from
   bool setTheoraFrame( S32 frame );

   /// If there was a seek request carry it out (within worker thread)
   bool doSeek( ogg_packet *packet );

   /// Save out the seek file (if needed)
   void saveSeekFile();

	/// Background playback thread.
	static void playThread( void *udata );

   /// Our main decoder loop (Seperate thread)
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

   /// Entry to store seek data
   struct SeekData
   {
      S32   frame;         ///< Frame number at this position
      S32   lastKeyFrame;  ///< Last key frame before this frame, if lastKeyFrame == frame this frame is a key frame
      F32   timeMark;      ///< Time in seconds at this position
      S32   fileOffset;    ///< Offset of file stream to begin looking for the page that contains this frame
      S32   page;          ///< Number of the ogg page that contains this frame
      S64   granulePos;    ///< Theora granulepos at this position
   };

   /// SeekInfo provides a way to create, load and store a seek index file
   /// It stores frame data that can be used to seek very quickly to a 
   /// correct frame or time in an associated theora stream.
   class SeekInfo
   {
      protected:
         Vector<SeekData>  mDataList;
         Stream*				mStream;
         bool              mStreamOwned;

      protected:
         /// Read an entry from a stream (space seperated single text line)
         bool  readEntry( Stream *s, SeekData *entry );         
         /// Write an entry to a stream (space seperated single text line)
         bool  writeEntry( Stream *s, SeekData *entry );         
         /// Do a bisect search for a specific time instead of scanning from start
         S32   bisectSearchTime( F32 stime, S32 low, S32 high );
         /// Do a bisect search for a specific frame instead of scanning from start
         S32   bisectSearchFrame( S32 frame, S32 low, S32 high );
         /// Do a bisect search for a specific granule pos instead of scanning from start
         S32   bisectSearchGranulePos( S64 granulepos, S32 low, S32 high );

      public:
         /// Default constructor
         SeekInfo();
         /// Destructor
         ~SeekInfo();

         /// Clear out all entries
         void             clear();
         /// Open file and write entries to stream as they are added
         bool             streamTo( const char *fname );
         /// Write entries to stream as they are added
         bool             streamTo( Stream *mStream );
         /// Save entries to file (one entry per line)
         bool             saveToFile( const char *fname );
         /// Load entries from file
         bool             loadFromFile( const char *fname );
         /// Add a new entry
         bool             addEntry( S32 frame, S32 lastKeyFrame, F32 timeMark, S32 page, S32 offset, S64 granpos );
         /// Find the closest entry by granule pos
         const SeekData*  findGranulePos( S64 granulePos );
         /// Find the entry with a frame closest to frame
         const SeekData*  findFrame( S32 frame ) const;
         /// Find the entry with a time closest to time
         const SeekData*  findTime( F32 time ) const;
   };

   /// Store our seek info
   SeekInfo mSeekInfo;

public:
   TheoraTexture();
   TheoraTexture(const char *filename, bool play = false, Audio::Description* desc = NULL);
   ~TheoraTexture();

   operator TextureObject*()
   {
      return (TextureObject*)*mTextureHandle;
   }

   /// Set the theora file to play
   /// If a seek file is provided it will load seek data from 
   /// that file to speed up seeking to a frame or time.
   /// Note that if saveSeek is specified no seeking will be allowed until
   /// the playhead has traversed the entire movie as it will be recording
   /// seek data to file.
   bool setFile( const char         *file,               ///< theora file to play
                 bool                play     = false,   ///< start playing immediately
                 bool                restart  = false,   ///< this is a restart call, it was playing before
                 Audio::Description *desc     = 0,       ///< 
                 const char         *seekFile = 0,       ///< seek file name (for seeking or writing to)
                 bool                saveSeek = false ); ///< save seek info to seekFile when done
   
   /// Is the video playing?
   const bool isPlaying() const { return mPlaying; }

   /// Is the video texture ready for use?
   const bool isReady() const { return mReady; }
   
   /// Play or continue from the current position
   bool play();

   /// Stop playback and reset state
   void stop();
   
   /// Pause playback and keep the current state
   bool pause();

   /// Are we set to loop to first frame when playhead gets to last frame?
   bool getLoop() const { return mLoop; }

   /// Should we loop video?
   void setLoop( bool flag ) { mLoop = flag; }

   /// Has the movie looped?
   bool hasLooped() const { return mHasLooped; }

   /// Reset the loop flag - this must be done from outside the class!
   void resetLoopFlag() { mHasLooped = false; }

   /// Update the GL texture from the bitmap the background thread writes to.
   void refresh();

   /// Get the GL texture handle
   U32  getGLName();

   /// Get the current playback time in seconds
   const F64 getCurrentTime()
   {
      return getTheoraTime();
   }

   /// Set the current time in seconds to start playing at
   bool setCurrentTime( F64 time )
   {
      return setTheoraTime( time );
   }

   F64 getCurrentVTime() const
   {
      return mCurrentVTime;
   };

   //////////////////////////////////////////////////////////////////////////

   /// Get the current playback frame
   S32 getCurrentFrame() const 
   {
      return mCurrentFrame;
   }

   /// Check if the frame has changed since last call
   bool frameHasChanged() const
   {
      return mFrameChanged;
   }

   /// Reset frame change flag
   void resetFrameChangedFlag()
   {
      mFrameChanged = false;
   }

   /// Set the current playback frame
   bool setCurrentFrame( S32 frame )
   {
      return setTheoraFrame( frame );
   }      
            
   /// Our texture
   TextureHandle*	mTextureHandle;
};

#endif
