//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "theoraPlayer.h"
#include "math/mMath.h"
#include "util/safeDelete.h"
#include "platform/profiler.h"

#define THEORA_BUFSIZE 1024
#define VORBIS_BUFSIZE 4096

//-----------------------------------------------------------------------------
TheoraTexture::TheoraTexture()
{
   init();

   //mMutex = Mutex::createMutex();
}
//-----------------------------------------------------------------------------
TheoraTexture::TheoraTexture( const char* szFilename, bool fPlay, Audio::Description *desc )
{
   init();
      
   setFile( szFilename, fPlay, false, desc );

   //mMutex = Mutex::createMutex();
}
//-----------------------------------------------------------------------------
void TheoraTexture::init()
{   
   mPlayThread     = 0;   
   mTheoraFile     = 0;
   mTheoraFileName = 0;
   mSeekFileName   = 0;
   mSavingSeekFile = false;

   mReady          = false;
   mLoop           = false;
   mPlaying        = false;   
   mHasLooped      = false;
   
   mStreamStart    = -1;
   mLastFrameTime  = -1;
   mLastKeyFrame   = -1;
   mLastFrame      = -1;
   mLastPageOffset = -1;
   mCurrentTime    = -1;
   mCurrentVTime   = -1;
   mCurrentFrame   = -1;
   mFrameChanged   = false;
   mSeekTo         = -1;
   mSeekTypeFrame  = false;

   mHasVorbis      = false;
   mVorbisHandle   = NULL;
   mVorbisBuffer   = NULL;
   mPlayThread     = NULL;
   mTheoraFile     = NULL;
   mTextureHandle  = NULL;

   mMagicalTrevor.reset();
}
//-----------------------------------------------------------------------------
TheoraTexture::~TheoraTexture()
{   
   destroyTexture(); 

   //Mutex::destroyMutex( mMutex );

   //mMutex = 0;
}
//-----------------------------------------------------------------------------
// Tears down anything the texture has
void TheoraTexture::destroyTexture( bool restartOgg ) 
{
   mPlaying = false;

   // kill the thread if its playing
   SAFE_DELETE( mPlayThread );

   // kill the sound if its playing
   if( mVorbisHandle )
   {
      alxStop( mVorbisHandle );

      mVorbisHandle = NULL;
      mVorbisBuffer = NULL; // this is already deleted in alxStop
      
      mMagicalTrevor.reset();
   }

   if( mHasVorbis )
   {
      ogg_stream_clear( &mOggVorbisStream );
      
      dMemset( &mOggVorbisStream, 0, sizeof(ogg_stream_state) );

      vorbis_dsp_clear( &mVorbisDspState );

      dMemset( &mVorbisDspState, 0, sizeof(vorbis_dsp_state) );

      vorbis_block_clear( &mVorbisBlock );

      dMemset( &mVorbisBlock, 0, sizeof(vorbis_block) );

      vorbis_comment_clear(&mVorbisComment);
      
      vorbis_info_clear( &mVorbisInfo );

      mHasVorbis = false;

      mMagicalTrevor.reset();
   }

   if( mReady )
   {
      ogg_stream_clear( &mOggTheoraStream );

      theora_clear( &mTheoraState );

      theora_comment_clear( &mTheoraComment );
      
      theora_info_clear( &mTheoraInfo );
      
      ogg_sync_clear( &mOggSyncState );

      saveSeekFile();
   }

   // Close the file if it's open
   if( mTheoraFile )
   {
      ResourceManager->closeStream( mTheoraFile );
      
      mTheoraFile = NULL;
   }

   if( restartOgg )
      return;

   // Set us to a null state.
   mReady = false;

   //SAFE_DELETE(mTextureHandle);
}
//-----------------------------------------------------------------------------
// Takes file name to open, and whether it should autoplay when loaded
bool TheoraTexture::setFile( const char         *filename, 
                             bool                doPlay, 
                             bool                doRestart, 
                             Audio::Description *desc,
                             const char         *seekFile,
                             bool                saveSeek )
{
   if( mPlaying )
      stop();

   if( mReady )
      destroyTexture( doRestart );

   // Seek info
   mSavingSeekFile = saveSeek;
   mSeekFileName   = seekFile ? StringTable->insert( seekFile ) : 0;

   // Initialize vorbis tracker
   mMagicalTrevor.reset();

   // Open the theora file
   mTheoraFile = ResourceManager->openStream( filename );

   if( !mTheoraFile )
   {
      Con::errorf( "TheoraTexture::setFile - Theora file '%s' not found.", filename );

      return false;
   }

   // Load seek file (if specified and NOT recording)
   if( !mSavingSeekFile && seekFile && seekFile[ 0 ] )
   {
      if( !mSeekInfo.loadFromFile( seekFile ) )
         Con::errorf( "TheoraTexture::setFile - Failed to load seek file: %s", seekFile );
   }

   Con::printf( "TheoraTexture - Loading file '%s'", filename );  

   // Start up Ogg stream synchronization layer
   ogg_sync_init( &mOggSyncState );

   // Init supporting Theora structures needed in header parsing
   theora_comment_init( &mTheoraComment );   
   theora_info_init( &mTheoraInfo );

   // Init supporting Vorbis structures needed in header parsing
   vorbis_comment_init( &mVorbisComment );
   vorbis_info_init( &mVorbisInfo );

   if( !parseHeaders() )
   {
      // No theora stream found (must be a vorbis only file?)
      // Clean up all the structs
      theora_comment_clear( &mTheoraComment );
      theora_info_clear( &mTheoraInfo );

      // trash vorbis too, this class isn't for playing lone vorbis streams
      vorbis_info_clear( &mVorbisInfo );
      vorbis_comment_clear( &mVorbisComment );

      Con::errorf( "TheoraTexture::setFile - Failed to parse Ogg headers" );

      return false;
   }

   // If theora stream found, initialize decoders...
   theora_decode_init( &mTheoraState, &mTheoraInfo );

   // This is a work around for a bug in theora when you're using only the
   // decoder (think its fixed in newest theora lib).
   mTheoraState.internal_encode = NULL;

   // Note our state.
   Con::printf( "   Ogg logical stream %x is Theora %dx%d %.02f fps video",
                mOggTheoraStream.serialno,
                mTheoraInfo.width,
                mTheoraInfo.height,
                (F64)mTheoraInfo.fps_numerator / (F64)mTheoraInfo.fps_denominator );

   Con::printf( "      - Frame content is %dx%d with offset (%d,%d).",
                mTheoraInfo.frame_width,
                mTheoraInfo.frame_height,
                mTheoraInfo.offset_x,
                mTheoraInfo.offset_y );
   
   if( mHasVorbis )
   {
      vorbis_synthesis_init( &mVorbisDspState, &mVorbisInfo );
      vorbis_block_init( &mVorbisDspState, &mVorbisBlock );

      Con::printf( "   Ogg logical stream %x is Vorbis %d channel %d Hz audio.",
                   mOggVorbisStream.serialno,
                   mVorbisInfo.channels,
                   mVorbisInfo.rate );

      if( !( mHasVorbis = createAudioBuffers( desc ) ) )
      {
         ogg_stream_clear( &mOggVorbisStream );
         vorbis_block_clear( &mVorbisBlock );
         vorbis_dsp_clear( &mVorbisDspState );
      }
   }

   // Check again because the buffers might fail.
   if( !mHasVorbis )
   {
      // no vorbis stream was found, throw out the vorbis structs
      vorbis_info_clear( &mVorbisInfo );
      vorbis_comment_clear( &mVorbisComment );
   }

   if( !mReady )
   {
      if( !createVideoBuffers() )
      {
         // failed to create buffers, blow everything else up..
         ogg_stream_clear( &mOggTheoraStream );
         theora_clear( &mTheoraState );
         theora_comment_clear( &mTheoraComment );
         theora_info_clear( &mTheoraInfo );
         ogg_sync_clear( &mOggSyncState );

         // And destroy our texture.
         destroyTexture();

         return false;
      }

      mReady = true;
   }

   if( doPlay )
      play();

   return true;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::parseHeaders()
{
   ogg_packet  sOggPacket;
   S32         nTheora = 0;
   S32         nVorbis = 0;
   S32         ret;

   mHasVorbis = false;

   // Parse the headers
   // Find theora and vorbis streams
   // Search pages till we find the headers
   mTheoraFile->setPosition( 0 );

   while( 1 )
   {
      ret = bufferOggPage();

      if( ret == 0 )
         break;

      if( !ogg_sync_pageout( &mOggSyncState, &mOggPage ) )
         break;

      ogg_stream_state testStream;

      if( !ogg_page_bos( &mOggPage ) )
      {
         // This is not an initial header, queue it up
         // Exit stream header finding loop (headers always come before non header stuff)
         queueOggPage( &mOggPage );

         break;
      }

      // Create a temp stream so we can see if its a type we're interested in
      ogg_stream_init( &testStream, ogg_page_serialno( &mOggPage ) );
      ogg_stream_pagein( &testStream, &mOggPage );
      ogg_stream_packetout( &testStream, &sOggPacket );

      // Test if its a theora header
      if( theora_decode_header( &mTheoraInfo, &mTheoraComment, &sOggPacket ) >= 0 )
      {
         // It is theora, copy testStream over to the theora stream
         dMemcpy( &mOggTheoraStream, &testStream, sizeof(testStream) );
         
         nTheora = 1;
      }      
      else // Test if its vorbis
      if( vorbis_synthesis_headerin( &mVorbisInfo, &mVorbisComment, &sOggPacket ) >= 0 )
      {
         // It is vorbis, copy testStream over to the vorbis stream
         dMemcpy( &mOggVorbisStream, &testStream, sizeof(testStream) );
         
         mHasVorbis = true;
         nVorbis    = 1;
      }
      else
      {
         // Some other stream header? unsupported, toss it
         ogg_stream_clear( &testStream );
      }

      // If both vorbis and theora have been found, exit loop
      if( nVorbis && nTheora )
         break;
   }

   if( !nTheora )
   {
      // No theora stream header found
      Con::errorf(" TheoraTexture::parseHeaders - No theora stream headers found." );

      // HAVE to have theora, thats what this class is for. return failure
      return false;
   }

   // We've now identified all the streams. parse (toss) the secondary header packets.
   // It looks like we just have to throw out a few packets from the header page
   // so that they arent mistaken as theora movie data? nothing is done with these things..
   while( ( nTheora < 3 ) || ( nVorbis && nVorbis < 3 ) )
   {
      // Look for further theora headers
      while( ( nTheora < 3 ) && ( ret = ogg_stream_packetout( &mOggTheoraStream, &sOggPacket ) ) )
      {
         if( ret < 0 )
         {
            Con::errorf( "TheoraPlayer::parseHeaders - Error parsing Theora stream headers; corrupt stream? (nothing read?)" );
            
            return false;
         }

         if( theora_decode_header( &mTheoraInfo, &mTheoraComment, &sOggPacket ) )
         {
            Con::errorf( "TheoraPlayer::parseHeaders - Error parsing Theora stream headers; corrupt stream?  (failed to decode)" );

            return false;
         }

         // Sanity around corrupt headers.
         nTheora++;
         
         if( nTheora == 3 )
            break;
      }

      // Look for more vorbis headers
      while( nVorbis     &&
             nVorbis < 3 &&
            ( ret = ogg_stream_packetout( &mOggVorbisStream, &sOggPacket ) ) )
      {
         if( ret < 0 )
         {
            Con::errorf( "Error parsing vorbis stream headers; corrupt stream? (nothing read?)" );

            return false;
         }

         if( vorbis_synthesis_headerin( &mVorbisInfo, &mVorbisComment, &sOggPacket ) )
         {
            Con::errorf( "Error parsing Vorbis stream headers; corrupt stream? (bad synthesis_headerin)" );

            return false;
         }
         
         nVorbis++;
         
         if( nVorbis == 3 )
            break;
      }

      // The header pages/packets will arrive before anything else we
      // care about, or the stream is not obeying spec
      // continue searching the next page for the headers

      // put the next page into the theora stream
      if( ogg_sync_pageout( &mOggSyncState, &mOggPage ) > 0 )
      {
         queueOggPage( &mOggPage );
      }
      else
      {
         // if there are no more pages, buffer another one
         const S32 ret = bufferOggPage();

         // if there is nothing left to buffer..
         if( ret == 0 )
         {
            Con::errorf( "TheoraTexture::parseHeaders - End of file while searching for codec headers." );

            return false;
         }
      }
   }
   
   return true;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::createVideoBuffers()
{
   // Set up our texture bitmap
   const GBitmap *bmp = new GBitmap( getMax( (U32)mTheoraInfo.frame_width, (U32)mTheoraInfo.width ),
                                     getMax( (U32)mTheoraInfo.frame_height, (U32)mTheoraInfo.height ),
                                     false, 
                                     GBitmap::RGB );
   // Set up our texture
   mTextureHandle = new TextureHandle( NULL, bmp, true );

   // Generate yuv conversion lookup tables
   generateLookupTables();

   return true;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::createAudioBuffers( Audio::Description* desc )
{
   // Just to be sure, clear out Trevor.
   mMagicalTrevor.reset();

   // If they didnt pass a description...
   if( !desc )
   {
      // ...fill a default
      static Audio::Description sDesc;
      
      desc = &sDesc;

      sDesc.mReferenceDistance  = 1.0f;
      sDesc.mMaxDistance        = 100.0f;
      sDesc.mConeInsideAngle    = 360;
      sDesc.mConeOutsideAngle   = 360;
      sDesc.mConeOutsideVolume  = 1.0f;
      sDesc.mConeVector.set(0, 0, 1);
      sDesc.mEnvironmentLevel   = 0.f;
      sDesc.mLoopCount          = -1;
      sDesc.mMinLoopGap         = 0;
      sDesc.mMaxLoopGap         = 0;

      sDesc.mIs3D        = false;
      sDesc.mVolume      = 1.0f;
      sDesc.mIsLooping   = false;
      sDesc.mType        = 1;
      sDesc.mIsStreaming = true;
   }

   // Create an audio handle to use
   mVorbisHandle = alxCreateSource( desc, "oggMixedStream" );

   if( !mVorbisHandle )
   {
      Con::errorf( "Could not alxCreateSource for oggMixedStream.\n" );

      return false;
   }

   // Get a pointer for it
   mVorbisBuffer = dynamic_cast<OggMixedStreamSource*>( alxFindAudioStreamSource( mVorbisHandle ) );

   if( !mVorbisBuffer )
   {
      alxStop( mVorbisHandle ); // not sure how alxStop would find it if i couldnt..

      Con::errorf( "Could not find oggMixedStreamSource ptr." );

      return false;
   }

   return true;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::play()
{
   if( mReady && !mPlaying )
      mPlayThread = new Thread( (ThreadRunFunction)playThread, (S32)this, 1 );

   return mPlayThread;
}
//-----------------------------------------------------------------------------
void TheoraTexture::stop()
{
   mPlaying = false;

   if( mPlayThread )      
      SAFE_DELETE( mPlayThread );
}
//-----------------------------------------------------------------------------
void TheoraTexture::playThread( void *udata )
{
   TheoraTexture* pThis = (TheoraTexture*)udata;
   
   pThis->playLoop();
}
//-----------------------------------------------------------------------------
bool TheoraTexture::playLoop()
{
   bool moreVideo = true;
   bool moreAudio = mHasVorbis;

   // timing variables
   F64 dVBuffTime = 0;   
   mLastFrameTime = 0;
   mPlaying       = true;
   mCurrentTime   = 0.f;
   mStartTick     = Platform::getRealMilliseconds();
   mStreamStart   = mTheoraFile->getPosition();
   
   bool isAudioActive   = false;
   S64  lastActualFrame = 0;
   
   while( mPlaying && ( mPlayThread && mPlayThread->isAlive() ) )
   {  
      ogg_packet packet;
      
      bool afterSeek = false;

      // Do we need to seek?
      if( !mSavingSeekFile && mSeekTo != -1 )
      {
         // Seek
         doSeek( &packet );

         // Make sure we don't totally lose it 
         dVBuffTime = mCurrentTime;
         
         // Tell the world we just did a seek
         afterSeek = true;
      }
      
      // keep our own copy as mFrameChanged might be reset at any time in another frame
      bool frameChanged = false; 
      
      if( !afterSeek && moreVideo )
         moreVideo = readyVideo( mLastFrameTime, &packet, dVBuffTime, frameChanged );

      if( mHasLooped )
         moreAudio = mHasVorbis;

      if( !afterSeek && moreAudio )
         moreAudio = readyAudio();      

      mCurrentVTime = dVBuffTime;

      if( !moreVideo && !moreAudio )
         break; // If we have no more audio to buffer, and no more video frames to display, we are done

      // If we haven't started yet, start it! :)
      if( !isAudioActive && mHasVorbis )
      {
         alxPlay( mVorbisHandle );

         isAudioActive = true;
      }

      // If we're set for the next frame, sleep
      // Make sure we never sleep for longer than 50 ms which can
      // happen if we just did a seek and we need to catch up.
      U32 safety  = 0;      
      F64 curTime = getTheoraTime();
      F64 t       = dVBuffTime - curTime;
            
      while( t > 0.f && safety < 100 && mPlaying )
      {
         ++safety;
         
         Platform::sleep( 1 );
         
         t = dVBuffTime - getTheoraTime();
      }
            
      mCurrentVTime = dVBuffTime;

      // Time to draw the frame
      if( frameChanged )
         drawFrame();

      // Keep track of the last frame time
      mLastFrameTime = getTheoraTime();
   }

   if( isAudioActive && mHasVorbis )
   {
      alxStop( mVorbisHandle );
      
      mMagicalTrevor.reset();
   }

   mPlaying = false;
   mVorbisHandle = NULL;
   mVorbisBuffer = NULL;   
   mHasLooped = true; // DARREN MOD, force Theora to call onMovieLoop
   
   return false;
}
//-----------------------------------------------------------------------------
S32 TheoraTexture::decodeVideoFrame( ogg_packet *packet, S32 &frame, F64 &time )
{
   PROFILE_START( TheoraTexture_decodeVideoFrame );

   // Read a packet from the sync layer
   int flag = ogg_stream_packetout( &mOggTheoraStream, packet );

   // -1 -> gap in data or sync error 
   //  0 -> need more data
   //  1 -> success
   if( flag > 0 )
   {
      // Submit the packet for decoding
      theora_decode_packetin( &mTheoraState, packet );

      // Get the current time and frame in the theora stream
      time  = theora_granule_time( &mTheoraState, mTheoraState.granulepos );
      frame = theora_granule_frame( &mTheoraState, mTheoraState.granulepos );

      if( time < 0 )
         flag = -1;
   }

   PROFILE_END();
         
   return flag;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::checkFrameChange( ogg_packet *packet, S32 frame, F64 time )
{  
   PROFILE_START( TheoraTexture_checkFrameChange );

   bool r;

   // Did we change frames?
   if( frame != mLastFrame )
   {
      mLastFrame    = mCurrentFrame;
      mCurrentFrame = frame;
      mFrameChanged = true; // threaded flag, reset externally, do not use internally!
      
      // Keyframe?
      if( theora_packet_iskeyframe( packet ) )
         mLastKeyFrame = frame;

      if( mSavingSeekFile )
      {               
         S32 page = ogg_page_pageno( &mOggPage );         
                  
         mSeekInfo.addEntry(  frame, 
                              mLastKeyFrame,
                              time, 
                              page, 
                              mLastPageOffset,
                              mTheoraState.granulepos );
      }

      r = true;
   }
   else
      r = false;

   PROFILE_END();

   return r;
}
//-----------------------------------------------------------------------------
// Ready a single frame (not a late one either)
bool TheoraTexture::readyVideo( const F64     lastFrameTime, 
                                ogg_packet   *packet, 
                                F64          &time, 
                                bool         &frameChanged )
{
   PROFILE_START( TheoraTexture_readyVideo );
       
   //MutexHandle mutexHandle;

   //mutexHandle.lock( mMutex );
   
   bool r = true;

   while( 1 )
   {
      // Decode a frame if we can
      S32 frame = -1;     
      S32 flag  = decodeVideoFrame( packet, frame, time );
               
      // Got the frame?
      if( flag > 0 )
      {
         // Mark the frame change (if any) and collect frame data
         frameChanged = checkFrameChange( packet, frame, time );

         // Check if this frame time has not passed yet.
         // If the frame is late we need to decode additional
         // ones and keep looping, since theora at this stage
         // needs to decode all frames.
         // Display at least one frame per second, regardless
         const F64 now = getTheoraTime();

         // Got a good frame, not late, ready to break out?
         if( time - now >= 0.0 || now - lastFrameTime >= 1.0 )
            break;
         
         // else frame is dropped (its behind), look again
      }
      else 
      {
         // Get another page
         if( !demandOggPage( &mOggPage ) ) 
         {
            // End of file, bug out
            if( mLoop )
            {
               setTheoraTime( 0 );

               mHasLooped = true;
            }         
            else
               r = false;
            
            break;
         }
      }      
   }
   
   PROFILE_END();

   return r;
}
//-----------------------------------------------------------------------------
S32 TheoraTexture::nextOggPage( ogg_page *page )
{
   // We're looking for the start of the next page
   S32 offset  = 0;
   S32 skipped = 0;

   while( true )
   {
      // Find the next page boundary     
      S32 bytes = ogg_sync_pageseek( &mOggSyncState, page );

      if( bytes < 0 )         // Skipped bytes?      
      {
         skipped -= bytes;    // bytes < 0 so its really offset += abs( bytes );               
         offset  -= bytes;    // Need to move beyond skipped bytes
      }
      else
         if( bytes == 0 )        // Need more data?
         {
            // Go buffer it...
            S32 r = bufferOggPage();

            // If no more bytes we hit the end of the file, ciao...
            if( r <= 0 )
               return -1;
         }
         else
         {
            // Got a page - return the offset at the page beginning,
            // advance the internal offset past the page end
            offset += bytes;

            // Is this the stream we were interested in?
            if( ogg_page_serialno( page ) && mOggTheoraStream.serialno )
               mLastPageOffset = mTheoraFile->getPosition() - offset + skipped;

            break;            
         }     
   }

   return offset;
}
//-----------------------------------------------------------------------------
// Seeking needs a seek index file to work.
// A seek file encodes all page boundaries, key frames, frames, times, file offsets etc so
// that we can seek very fast. As this is generally for games we can preprocess all media
// to create the seek files and dont have to do the extra work to get generic seeking etc.
// TheoraTexture can generate a seek file if mSeekFile is set to a filename and saveSeek is true.
//
bool TheoraTexture::doSeek( ogg_packet *packet )
{
   // Clear the current ogg buffers, find the start of the page containing the
   // time/frame we wish to seek to, and fill the buffers again.
   
   // Do we have seek info?
   const SeekData *sd = 0; 

   if( mSeekTypeFrame )
      sd = mSeekInfo.findFrame( (S32)mSeekTo );
   else   
      sd = mSeekInfo.findTime( mSeekTo );

   // Save actual seek time/frame
   F64 actualSeekTo     = mSeekTo;
   S32 findThisKeyFrame = 0;

   // Reset so we don't try and reseek while we're in here eh!
   mSeekTo = -1;

   // We want a key frame
   if( sd )
   {
      actualSeekTo = mSeekTypeFrame ? sd->frame : sd->timeMark;

      if( sd->frame != sd->lastKeyFrame )
      {
         findThisKeyFrame = sd->lastKeyFrame;

         sd = mSeekInfo.findFrame( sd->lastKeyFrame );
      }
   }

   // If we had a key frame entry, seek the file to before that offset, otherwise
   // just start at the beginning of the data stream.
   S32 offset     = sd ? getMax( sd->fileOffset - ( THEORA_BUFSIZE * 4 ), 0 ) : mStreamStart;
   S32 targetPage = sd ? sd->page : 0;

   // Reset streams and sync
   ogg_sync_reset( &mOggSyncState );
   ogg_stream_reset( &mOggTheoraStream );
   ogg_stream_reset( &mOggVorbisStream );
   
   // Reset theora decoder
   theora_decode_init( &mTheoraState, &mTheoraInfo );
   
   // Clear out any pending audio buffers
   mVorbisBuffer->empty();

   //alxStop( mVorbisHandle );

   // Reset vorbis
   vorbis_synthesis_restart( &mVorbisDspState );

   // Keep Magical trevor magical...
   mMagicalTrevor.reset();

   // Seek to file offset
   mTheoraFile->setPosition( offset );

   bool foundKeyFrame  = false;

   // So large old stack buffer, pffft...
   S16 samples[ VORBIS_BUFSIZE ]; // This should be large enough

   while( 1 )
   {  
      // Seek to next page (we moved to a position before it)      
      if( !demandOggPage( &mOggPage  ) )
         break;

      // Get some info
      int pageno = ogg_page_pageno( &mOggPage );

      // Granular search for page number first
      //if( pageno < targetPage )
        // continue;
            
      // Now search the packets until we find the keyframe we need to start at
      bool needData = false;

      while( !needData )
      {     
         S32 frame = -1;
         F64 time  = -1;         
         S32 count = 0;

         // Process audio, any samples?
         while( !( count = decodeAudioSamples( samples, VORBIS_BUFSIZE ) ) )            
         {
            if( !nextAudioPacket( packet ) ) // No more samples, read the next packet
            {
               needData = true;              // Please sir, my I have some more...

               break;                        // Let video have its go...
            }
         }       

         // Now video, read a packet from sync layer
         if( decodeVideoFrame( packet, frame, time ) > 0 )
         {              
            // Have we hit our keyframe (any before it actually)?
            if( !foundKeyFrame )
               if( theora_packet_iskeyframe( packet ) ) //&& frame >= findThisKeyFrame )
                  foundKeyFrame = true;
            
            // We have to restart decoding at a keyframe or we get gunk until we hit a keyframe
            if( foundKeyFrame )
            {               
               // Ok, so we've started decoding from the keyframe, we have to unpack
               // everything from here until we get to our desired time/frame.
               drawFrame( true );

               // Have we hit our actual seekpoint?
               bool done = mSeekTypeFrame ? ( frame >= (S32)actualSeekTo ) : ( time >= actualSeekTo );
                              
               if( done )
               {                           
                  // Mark seek point as current so normal decoding can continue
                  mCurrentTime   = time;
                  mLastFrameTime = mCurrentTime;
                  mStartTick     = Platform::getRealMilliseconds() - ( time * 1000 ) - 1000;

                  return true;
               }  
            }           
         }
         else                  
            needData = true; // Break out to page loop         
      }
   }

   return false;
}
//-----------------------------------------------------------------------------
// 6K!! memory needed to speed up theora player. Small price to pay!
static S32 sAdjCrr[256];
static S32 sAdjCrg[256];
static S32 sAdjCbg[256];
static S32 sAdjCbb[256];
static S32 sAdjY[256];
static U8  sClampBuff[1024];
static U8* sClamp = sClampBuff + 384;
//-----------------------------------------------------------------------------
// Precalculate adjusted YUV values for faster RGB conversion
void TheoraTexture::generateLookupTables()
{
   static bool fGenerated = false;
      
   for( S32 i = 0; i < 256; i++ )
   {
      sAdjCrr[i] = (409 * (i - 128) + 128) >> 8;
      sAdjCrg[i] = (208 * (i - 128) + 128) >> 8;
      sAdjCbg[i] = (100 * (i - 128) + 128) >> 8;
      sAdjCbb[i] = (516 * (i - 128) + 128) >> 8;
      sAdjY[i]   = (298 * (i - 16)) >> 8;
   }

   // And setup LUT clamp range
   for( S32 i = -384; i < 640; i++ )   
      sClamp[i] = mClamp( i, 0, 0xFF );   
}
//-----------------------------------------------------------------------------
void TheoraTexture::drawFrame( bool decodeOnly )
{
   yuv_buffer yuv;

   // Decode a frame! (into yuv)
   theora_decode_YUVout( &mTheoraState, &yuv );

   if( decodeOnly )
      return;

   // Get destination buffer (and 1 row offset)
   GBitmap *bmp = mTextureHandle->getBitmap();
   U8* dst0     = bmp->getAddress(0, 0);
   U8 *dst1     = dst0 + bmp->getWidth() * bmp->bytesPerPixel;

   // Find picture offset
   const S32 pictOffset = yuv.y_stride * mTheoraInfo.offset_y + mTheoraInfo.offset_x;

   const U8 *pY0, *pY1, *pU, *pV;

   for( S32 y = 0; y < yuv.y_height; y += 2 )
   {
      // set pointers into yuv buffers (2 lines for y)
      pY0 = yuv.y + pictOffset + y * (yuv.y_stride);
      pY1 = yuv.y + pictOffset + (y | 1) * (yuv.y_stride);
      pU = yuv.u + ((pictOffset + y * (yuv.uv_stride)) >> 1);
      pV = yuv.v + ((pictOffset + y * (yuv.uv_stride)) >> 1);

      for(S32 x = 0; x < yuv.y_width; x += 2)
      {
         // convert a 2x2 block over

         // speed up G conversion a very very small amount ;)
         const S32 G = sAdjCrg[*pV] + sAdjCbg[*pU];

         // pixel 0x0
         *dst0++ = sClamp[sAdjY[*pY0] + sAdjCrr[*pV]];
         *dst0++ = sClamp[sAdjY[*pY0] - G];
         *dst0++ = sClamp[sAdjY[*pY0++] + sAdjCbb[*pU]];

         // pixel 1x0
         *dst0++ = sClamp[sAdjY[*pY0] + sAdjCrr[*pV]];
         *dst0++ = sClamp[sAdjY[*pY0] - G];
         *dst0++ = sClamp[sAdjY[*pY0++] + sAdjCbb[*pU]];

         // pixel 0x1
         *dst1++ = sClamp[sAdjY[*pY1] + sAdjCrr[*pV]];
         *dst1++ = sClamp[sAdjY[*pY1] - G];
         *dst1++ = sClamp[sAdjY[*pY1++] + sAdjCbb[*pU]];

         // pixel 1x1
         *dst1++ = sClamp[sAdjY[*pY1] + sAdjCrr[*pV++]];
         *dst1++ = sClamp[sAdjY[*pY1] - G];
         *dst1++ = sClamp[sAdjY[*pY1++] + sAdjCbb[*pU++]];
      }

      // shift the destination pointers a row (loop incs 2 at a time)
      dst0  = dst1;
      dst1 += bmp->getWidth() * bmp->bytesPerPixel;
   }
}
//-----------------------------------------------------------------------------
F64 TheoraTexture::getTheoraTime()
{
   if( mHasVorbis && mVorbisBuffer )
   {
      // We have audio, so synch to audio track.
      ALint buf = -1;

      alGetSourcei( mVorbisBuffer->mSource, AL_BUFFER, &buf );

      mCurrentTime += mMagicalTrevor.advanceTime( buf );

      return mCurrentTime;
   }
   else
   {
      // We have no audio, just synch to start time.
      return (F64)0.001 * (F64)( Platform::getRealMilliseconds() - mStartTick );
   }
}
//-----------------------------------------------------------------------------
bool TheoraTexture::setTheoraTime( F64 time )
{  
   // Just mark that we want to seek, actual seek is done in thread loop
   mSeekTo        = time < 0 ? 0 : time;
   mSeekTypeFrame = false;
   
   return true;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::setTheoraFrame( S32 frame )
{
   // Just mark that we want to seek, actual seek is done in thread loop
   mSeekTo        = frame < 0 ? 0 : frame;
   mSeekTypeFrame = true;
   
   return true;
}
//-----------------------------------------------------------------------------
// Function does whatever it can get pages into streams
bool TheoraTexture::demandOggPage( ogg_page *page )
{
   if( nextOggPage( page ) > 0 )
   {
      queueOggPage( page );

      return true;
   }

   return false;
}
//-----------------------------------------------------------------------------
// Grabs some more compressed bitstream and syncs it for page extraction
S32 TheoraTexture::bufferOggPage()
{
   // Hit the end of the stream?
   if( mTheoraFile->getStatus() != Stream::Ok )
      return -1;
         
   // Get a buffer
   char *buffer = ogg_sync_buffer( &mOggSyncState, THEORA_BUFSIZE );
      
   // Save position so we can calc how many bytes were actually read
   S32 bytes = mTheoraFile->getPosition();

   // Pull some data into the buffer
   mTheoraFile->read( THEORA_BUFSIZE, buffer );
   
   // Find out how much was read
   bytes = mTheoraFile->getPosition() - bytes;

   // Give it to ogg and tell it how many bytes
   ogg_sync_wrote( &mOggSyncState, bytes );
   
   return bytes;
}
//-----------------------------------------------------------------------------
// Try and put the page into the theora and vorbis streams,
S32 TheoraTexture::queueOggPage( ogg_page *page )
{
   // Lets not be lazy - queue the correct stream by serialno
   int ps = ogg_page_serialno( page );

   if( ps == mOggTheoraStream.serialno )   
      return ogg_stream_pagein( &mOggTheoraStream, page );
   else
   if( mHasVorbis && ps == mOggVorbisStream.serialno )
      return ogg_stream_pagein( &mOggVorbisStream, page );

   return -1;
}
//-----------------------------------------------------------------------------
// Returns a handle for OpenGL calls
U32 TheoraTexture::getGLName()
{
   if( mTextureHandle )
   {
      mTextureHandle->refresh();

      return mTextureHandle->getGLName();
   }
   return 0;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::nextAudioPacket( ogg_packet *packet )
{         
   // Is there a pending packet to decode?
   if( ogg_stream_packetout( &mOggVorbisStream, packet ) > 0 )
   {
      // Did we hit end of stream?
      //if( sOggPacket.e_o_s )
      // break;

      if( vorbis_synthesis( &mVorbisBlock, packet ) == 0 )         
         vorbis_synthesis_blockin( &mVorbisDspState, &mVorbisBlock );

      return true;
   }

   return false;
}
//-----------------------------------------------------------------------------
S32 TheoraTexture::decodeAudioSamples( S16 *outbuf, S32 size )
{  
   float **pcm = 0;
   S32 count   = 0;
   S32 samples = vorbis_synthesis_pcmout( &mVorbisDspState, &pcm );

   if( samples > 0 )
   {  
      S32 smax = getMin( samples, size );

      // Found samples to buffer, convert! 
      for( S32 i = 0; i < smax; i++ )
      {
         for( int j = 0; j < mVorbisInfo.channels; j++ )
         {
            int val = (int)( pcm[j][i] * 32767.f );
            
            if( val > 32767 )
               val = 32767;

            if( val < -32768 )
               val = -32768;

         #if defined(TORQUE_OS_MAC) && !defined(TORQUE_BIG_ENDIAN)
            outbuf[ count++ ] = ( ( val << 8 ) & 0xFF00 ) | ( ( val >> 8 ) & 0x00FF );
         #else
            outbuf[ count++ ] = val;
         #endif
         }
      }   

      // Tell vorbis how many samples we actually consumed
      vorbis_synthesis_read( &mVorbisDspState, samples );
   }      
   
   return count;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::postNextAudioBuffer( S16 *samples, S32 count )
{
   // Get a real buffer
   ALuint bufferId = mVorbisBuffer->GetAvailableBuffer();

   if( !bufferId ) // Buffered all that it can fit
      return false;

   // Note the time for synchronization by Magical Trevor.
   const F64 newTimeSlice = F64( count * 0.5 ) / F64( mVorbisInfo.rate );

   // Add buffer and time
   mMagicalTrevor.postBuffer( bufferId, newTimeSlice );

   // By this point the buffer should be filled (or as close as its gonna get)
   // ... Queue buffer
   alBufferData(  bufferId,
                  ( mVorbisInfo.channels == 1 ) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
                  samples,
                  count * 2,
                  mVorbisInfo.rate );

   return mVorbisBuffer->QueueBuffer( bufferId );
}
//-----------------------------------------------------------------------------
// Buffers up as much audio as it can fit into OggMixedStream audiostream thing
bool TheoraTexture::readyAudio( bool decodeOnly )
{
   ogg_packet sOggPacket;
      
   // The memory manager doesn't seem to be working with multiple threads,
   // so large old stack buffer, pffft...
   S16 samples[ VORBIS_BUFSIZE ]; // this should be large enough

   //MutexHandle mutexHandle;

   //mutexHandle.lock( mMutex );
   PROFILE_START( TheoraTexture_readyAudio );

   // If i don't have a buffer to put samples into..
   while( mVorbisBuffer->hasAvailableBuffer() )
   {  
      // If the buffer is ready, fill it!
      S32 count = 0;

      // Try and get some samples      
      while( !( count = decodeAudioSamples( samples, VORBIS_BUFSIZE ) ) )
      {
         // No more samples, read the next packet            
         if( !nextAudioPacket( &sOggPacket ) )
         {
            // We need more data; suck in another page
            if( !demandOggPage( &mOggPage ) )
            {
               PROFILE_END();

               return false;   // end of file
            }
         }         
      }

      // If we only wanted to decode, don't post audio buffer
      if( !decodeOnly )      
         if( !postNextAudioBuffer( samples, count ) )
            break;
   }

   PROFILE_END();

   return true;
}
//-----------------------------------------------------------------------------
// Copies the newest texture data to openGL video memory
void TheoraTexture::refresh()
{
   if( mTextureHandle )
      mTextureHandle->refresh();
}
//-----------------------------------------------------------------------------
void TheoraTexture::saveSeekFile()
{
   // If we were recording a seek file, save it out
   if( mSavingSeekFile )
   {
      if( mSeekFileName && mSeekFileName[ 0 ] )
      {
         MutexHandle mutexHandle;

         mutexHandle.lock( mMutex );

         if( !mSeekInfo.saveToFile( mSeekFileName ) )
            Con::errorf( "TheoraTexture::destroyTexture - failed to save seek data to file: %s", mSeekFileName );
      }
      else
         Con::errorf( "TheoraTexture::destroyTexture - flagged to save seek data but no seek file specified!" );
   }
}

//============================================================================================
//                                       SEEK INFO
//============================================================================================
TheoraTexture::SeekInfo::SeekInfo() : mStream( 0 ), mStreamOwned( false )
{
}
//-----------------------------------------------------------------------------
TheoraTexture::SeekInfo::~SeekInfo()
{
   if( mStream )
   {
      if( mStreamOwned )      
         ResourceManager->closeStream( mStream );      
   
      mStream = 0;
   }
}
//-----------------------------------------------------------------------------
void TheoraTexture::SeekInfo::clear()
{
   mDataList.clear();
}
//-----------------------------------------------------------------------------
bool TheoraTexture::SeekInfo::saveToFile( const char *fname )
{
   FileStream fs;
    
   if( ResourceManager->openFileForWrite( fs, fname ) )
   {      
      for( S32 i = 0; i < mDataList.size(); i++ )
         writeEntry( &fs, &mDataList[ i ] );

      ResourceManager->closeStream( &fs );

      return true;
   }

   return false;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::SeekInfo::loadFromFile( const char *fname )
{
   Stream *fs = ResourceManager->openStream( fname );

   if( fs )
   {
      mDataList.clear();

      SeekData e;

      while( readEntry( fs, &e ) )
         mDataList.push_back( e );

      ResourceManager->closeStream( fs );

      return mDataList.size() > 0;
   }
   
   return false;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::SeekInfo::addEntry( S32 frame, S32 lastKeyFrame, F32 timeMark, S32 page, S32 offset, S64 granpos )
{
   // Make sure we don't add anything we've done already
   S32 max = mDataList.size() > 0 ? mDataList.last().frame : -1;
   
   if( frame <= max )
      return false;

   // Add it
   mDataList.increment();
   
   mDataList.last().frame        = frame;
   mDataList.last().lastKeyFrame = lastKeyFrame;
   mDataList.last().timeMark     = timeMark;
   mDataList.last().fileOffset   = offset;   
   mDataList.last().page         = page;  
   mDataList.last().granulePos   = granpos;

   if( mStream )
      writeEntry( mStream, &mDataList.last() );

   return true;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::SeekInfo::streamTo( const char *fname )
{
   mStream      = ResourceManager->openStream( fname );
   mStreamOwned = true;

   return mStream != 0;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::SeekInfo::streamTo( Stream *stream )
{
   mStream      = stream;
   mStreamOwned = false;

   return mStream != 0;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::SeekInfo::readEntry( Stream *s, TheoraTexture::SeekData *entry )
{
   if( s )
   {
      char buf[ 1024 ];

      s->readLine( (U8*)buf, 1024 );

      if( dStrlen( buf ) > 0 )
      {          
         dSscanf( buf, "%d %d %f %d %d %lld", &entry->frame, &entry->lastKeyFrame, &entry->timeMark, &entry->page, &entry->fileOffset, &entry->granulePos );
         
         return true;
      }
   }

   return false;
}
//-----------------------------------------------------------------------------
bool TheoraTexture::SeekInfo::writeEntry( Stream *s, TheoraTexture::SeekData *entry )
{
   if( s )
   {
      char buf[ 1024 ];

      dSprintf( buf, 1024, "%d %d %f %d %d %lld", entry->frame, entry->lastKeyFrame, entry->timeMark, entry->page, entry->fileOffset, entry->granulePos );

      s->writeLine( (U8*)buf );
   }

   return false;
}
//-----------------------------------------------------------------------------
const TheoraTexture::SeekData* TheoraTexture::SeekInfo::findFrame( S32 frame ) const
{
   const SeekData *prev = mDataList.size() > 0 ? &mDataList[ 0 ] : 0;

   for( S32 i = 0; i < mDataList.size(); i++ )
   {
      // This entry just on or after frame?
      const SeekData *d = &mDataList[ i ];

      if( d->frame == frame ) // If exactly on the frame return this entry
         return d;

      if( d->frame > frame )  // if first after return previous (which had to be just before frame)
         return prev;

      prev = d;
   }
      
   return 0;
}
//-----------------------------------------------------------------------------
const TheoraTexture::SeekData* TheoraTexture::SeekInfo::findTime( F32 time ) const
{
   // Guess an entry (Normalized time better?)
   //S32 index = bisectSearchTime( time, 0, mDataList.size() - 1 );

   const SeekData *prev = mDataList.size() > 0 ? &mDataList[ 0 ] : 0;

   for( S32 i = 0; i < mDataList.size(); i++ )
   {
      // This entry just on or after frame?
      const SeekData *d = &mDataList[ i ];

      if( d->timeMark == time ) // If exactly on the time return this entry
         return d;

      if( d->timeMark > time )  // if first after return previous (which had to be just before frame)
         return prev;

      prev = d;
   }
      
   return 0;
}
//-----------------------------------------------------------------------------
S32 TheoraTexture::SeekInfo::bisectSearchGranulePos( S64 granulepos, S32 low, S32 high )
{  
   if( low > high )
      return 0;

   S32 mid = ( high + low ) / 2;

   S64 gp = mDataList[ mid ].granulePos;

   // Did we find it?
   if( gp == granulepos )
      return mid;

   // Bisect
   if( gp > granulepos )
      return bisectSearchGranulePos( granulepos, low, mid - 1 );
   else    
      return bisectSearchGranulePos( granulepos, mid + 1, high );
} 

//=============================================================================
//                               MAGICAL TREVOR
//=============================================================================
TheoraTexture::MagicalTrevor::BufInf::BufInf()
{
   id    = 0;
   time  = 0;
   next  = NULL;
}
//-----------------------------------------------------------------------------
TheoraTexture::MagicalTrevor::MagicalTrevor()
{
   mBufListHead   = NULL;
   mLastBufferID  = -1;

   mMutex = Mutex::createMutex();
}
//-----------------------------------------------------------------------------
TheoraTexture::MagicalTrevor::~MagicalTrevor()
{
   // Cheat on freeing, since BufInf has no destructor.
   mBuffPool.freeBlocks();

   Mutex::destroyMutex( mMutex );
}
//-----------------------------------------------------------------------------
const F64 TheoraTexture::MagicalTrevor::advanceTime( const ALuint buffId )
{
   MutexHandle handle;
   
   handle.lock( mMutex );

   // We basically find the last entry on the list that references
   // this buffId, then count how much time it + all its followers
   // contains, and return that amount. Then the list is truncated.

   // Skip if we just saw this one... we'd better not go
   // through all the buffers in one advanceTime call, that would
   // confuse the hell out of this code.
   if( mLastBufferID == buffId )
      return 0.f;

   mLastBufferID = buffId;

   // Ok, find last occurence of buffId.
   BufInf **walk          = &mBufListHead;
   BufInf **lastOccurence = NULL;

   while( *walk )
   {
      if( (*walk)->id == buffId )
      {
         lastOccurence = walk;

         break;
      }

      walk = &(*walk)->next;
   }

   if( lastOccurence == NULL )
   {
      //Con::warnf( "MagicalTrevor::advancetime - no last occurrence for buffer %d found!", buffId );

      return 0.f;
   }

   // We've got the last occurrence, sum the time and truncate the list.
   F64 timeSum = 0.f;

   walk = lastOccurence;

   while( *walk )
   {
      timeSum += (*walk)->time;

      // Blast it and advance.
      BufInf *del = *walk;
      *walk       = (*walk)->next;
      
      mBuffPool.free(del);
   }

   return timeSum;
}
//-----------------------------------------------------------------------------
void TheoraTexture::MagicalTrevor::postBuffer( const ALuint buffId, const F64 duration )
{
   MutexHandle handle;
   
   handle.lock( mMutex );

   // Stick the buffer at the front of the queue...
   BufInf *walk = mBuffPool.alloc();
   walk->id     = buffId;
   walk->time   = duration;

   // Link it in.
   walk->next   = mBufListHead;
   mBufListHead = walk;
}
//-----------------------------------------------------------------------------
const U32 TheoraTexture::MagicalTrevor::getListSize() const
{
   MutexHandle handle;
   
   handle.lock( mMutex );

   U32 size = 0;

   // Ok, find last occurence of buffId.
   const BufInf *walk = mBufListHead;
   
   while( walk )
   {
      ++size;

      walk = walk->next;
   }

   return size;
}
//-----------------------------------------------------------------------------
void TheoraTexture::MagicalTrevor::reset()
{
   MutexHandle handle;
   
   handle.lock( mMutex );

   // Since we're mostly touched by the thread, let's make this a mutex
   // operation.

   mBuffPool.freeBlocks();
   
   mBufListHead  = NULL;
   mLastBufferID = -1;
}
//-----------------------------------------------------------------------------