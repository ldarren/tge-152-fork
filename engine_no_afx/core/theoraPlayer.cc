//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "theoraPlayer.h"
#include "math/mMath.h"
#include "util/safeDelete.h"
#include "platform/profiler.h"

//-----------------------------------------------------------------------------

TheoraTexture::MagicalTrevor::BufInf::BufInf()
{
   id=0;
   time=0;
   next=NULL;
}

TheoraTexture::MagicalTrevor::MagicalTrevor()
{
   mBufListHead = NULL;
   mLastBufferID = -1;

   mMutex = Mutex::createMutex();
}

TheoraTexture::MagicalTrevor::~MagicalTrevor()
{
   // Cheat on freeing, since BufInf has no destructor.
   mBuffPool.freeBlocks();

   Mutex::destroyMutex(mMutex);
}

const F64 TheoraTexture::MagicalTrevor::advanceTime(const ALuint buffId)
{
   MutexHandle handle;
   handle.lock(mMutex);

   // We basically find the last entry on the list that references
   // this buffId, then count how much time it + all its followers
   // contains, and return that amount. Then the list is truncated.

   // Skip if we just saw this one... we'd better not go
   // through all the buffers in one advanceTime call, that would
   // confuse the hell out of this code.
   if(mLastBufferID == buffId)
      return 0.f;

   mLastBufferID = buffId;

   // Ok, find last occurence of buffId.
   BufInf **walk = &mBufListHead;

   BufInf **lastOccurence = NULL;

   while(*walk)
   {
      if((*walk)->id == buffId)
         lastOccurence = walk;

      walk = &(*walk)->next;
   }

   if(lastOccurence == NULL)
   {
      Con::warnf("MagicalTrevor::advancetime - no last occurrence for buffer %d found!", buffId);
      return 0.f;
   }

   // We've got the last occurrence, sum the time and truncate the list.
   F64 timeSum = 0.f;

   walk = lastOccurence;

   while(*walk)
   {
      timeSum += (*walk)->time;

      // Blast it and advance.
      BufInf *del = *walk;
      *walk = (*walk)->next;
      mBuffPool.free(del);
   }

   return timeSum;
}

void TheoraTexture::MagicalTrevor::postBuffer(const ALuint buffId, const F64 duration)
{
   MutexHandle handle;
   handle.lock(mMutex);

   // Stick the buffer at the front of the queue...
   BufInf *walk = mBuffPool.alloc();
   walk->id     = buffId;
   walk->time   = duration;

   // Link it in.
   walk->next = mBufListHead;
   mBufListHead = walk;

}

const U32 TheoraTexture::MagicalTrevor::getListSize() const
{
   MutexHandle handle;
   handle.lock(mMutex);

   U32 size=0;

   // Ok, find last occurence of buffId.
   const BufInf *walk = mBufListHead;
   while(walk)
   {
      size++;
      walk = walk->next;
   }

   return size;
}

void TheoraTexture::MagicalTrevor::reset()
{
   MutexHandle handle;
   handle.lock(mMutex);

   // Since we're mostly touched by the thread, let's make this a mutex
   // operation.

   mBuffPool.freeBlocks();
   mBufListHead = NULL;
   mLastBufferID = -1;
}

//-----------------------------------------------------------------------------

TheoraTexture::TheoraTexture()
{
   init();
}

TheoraTexture::TheoraTexture(const char* szFilename, bool fPlay, Audio::Description* desc)
{
   init();
   setFile(szFilename, fPlay, false, desc);
}

void TheoraTexture::init()
{
   mReady = false;
   mPlaying = false;
   mHasVorbis = false;
   mVorbisHandle = NULL;
   mVorbisBuffer = NULL;
   mPlayThread = NULL;
   mTheoraFile = NULL;
   mTextureHandle = NULL;
   mMagicalTrevor.reset();
}

TheoraTexture::~TheoraTexture()
{
   destroyTexture();
}

// tears down anything the texture has
void TheoraTexture::destroyTexture(bool restartOgg)
{
   mPlaying = false;

   // kill the thread if its playing
   SAFE_DELETE(mPlayThread);

   // kill the sound if its playing
   if(mVorbisHandle)
   {
      alxStop(mVorbisHandle);
      mVorbisHandle = NULL;
      mVorbisBuffer = NULL; // this is already deleted in alxStop
      mMagicalTrevor.reset();
   }

   if(mHasVorbis)
   {
      ogg_stream_clear(&mOggVorbisStream);
      dMemset(&mOggVorbisStream, 0, sizeof(ogg_stream_state));

      vorbis_dsp_clear(&mVorbisDspState);
      dMemset(&mVorbisDspState, 0, sizeof(vorbis_dsp_state));

      vorbis_block_clear(&mVorbisBlock);
      dMemset(&mVorbisBlock, 0, sizeof(vorbis_block));

      vorbis_comment_clear(&mVorbisComment);
      vorbis_info_clear(&mVorbisInfo);

      mHasVorbis = false;
      mMagicalTrevor.reset();
   }

   if(mReady)
   {
      ogg_stream_clear(&mOggTheoraStream);
      theora_clear(&mTheoraState);
      theora_comment_clear(&mTheoraComment);
      theora_info_clear(&mTheoraInfo);
      ogg_sync_clear(&mOggSyncState);
   }

   // close the file if it's open
   if(mTheoraFile)
   {
      ResourceManager->closeStream(mTheoraFile);
      mTheoraFile = NULL;
   }

   if(restartOgg)
      return;

   // Set us to a null state.
   mReady = false;

   //SAFE_DELETE(mTextureHandle);
}


// Takes file name to open, and whether it should autoplay when loaded
bool TheoraTexture::setFile(const char* filename, bool doPlay, bool doRestart, Audio::Description* desc)
{
   if(mPlaying)
      stop();

   if(mReady)
      destroyTexture(doRestart);

   // open the theora file
   mTheoraFile = ResourceManager->openStream(filename);

   mMagicalTrevor.reset();

   if(!mTheoraFile)
   {
      Con::errorf("TheoraTexture::setFile - Theora file '%s' not found.", filename);
      return false;
   }

   Con::printf("TheoraTexture - Loading file '%s'", filename);

   // start up Ogg stream synchronization layer
   ogg_sync_init(&mOggSyncState);

   // init supporting Theora structures needed in header parsing
   theora_comment_init(&mTheoraComment);
   theora_info_init(&mTheoraInfo);

   // init supporting Vorbis structures needed in header parsing
   vorbis_comment_init(&mVorbisComment);
   vorbis_info_init(&mVorbisInfo);

   if(!parseHeaders())
   {
      // No theora stream found (must be a vorbis only file?)
      // Clean up all the structs
      theora_comment_clear(&mTheoraComment);
      theora_info_clear(&mTheoraInfo);

      // trash vorbis too, this class isn't for playing lone vorbis streams
      vorbis_info_clear(&mVorbisInfo);
      vorbis_comment_clear(&mVorbisComment);

      Con::errorf("TheoraTexture::setFile - Failed to parse Ogg headers");
      return false;
   }

   // If theora stream found, initialize decoders...
   theora_decode_init(&mTheoraState, &mTheoraInfo);

   // This is a work around for a bug in theora when you're using only the
   // decoder (think its fixed in newest theora lib).
   mTheoraState.internal_encode = NULL;

   // Note our state.
   Con::printf("   Ogg logical stream %x is Theora %dx%d %.02f fps video",
            mOggTheoraStream.serialno,
            mTheoraInfo.width,
            mTheoraInfo.height,
            (F64)mTheoraInfo.fps_numerator / (F64)mTheoraInfo.fps_denominator);

   Con::printf("      - Frame content is %dx%d with offset (%d,%d).",
            mTheoraInfo.frame_width,
            mTheoraInfo.frame_height,
            mTheoraInfo.offset_x,
            mTheoraInfo.offset_y);

   if(mHasVorbis)
   {
      vorbis_synthesis_init(&mVorbisDspState, &mVorbisInfo);
      vorbis_block_init(&mVorbisDspState, &mVorbisBlock);
      Con::printf("   Ogg logical stream %x is Vorbis %d channel %d Hz audio.",
               mOggVorbisStream.serialno,
               mVorbisInfo.channels,
               mVorbisInfo.rate);

      if(!(mHasVorbis = createAudioBuffers(desc)))
      {
         ogg_stream_clear(&mOggVorbisStream);
         vorbis_block_clear(&mVorbisBlock);
         vorbis_dsp_clear(&mVorbisDspState);
      }
   }

   // Check again because the buffers might fail.
   if(!mHasVorbis)
   {
      // no vorbis stream was found, throw out the vorbis structs
      vorbis_info_clear(&mVorbisInfo);
      vorbis_comment_clear(&mVorbisComment);
   }

   if(!mReady)
   {
      if(!createVideoBuffers())
      {
         // failed to create buffers, blow everything else up..
         ogg_stream_clear(&mOggTheoraStream);
         theora_clear(&mTheoraState);
         theora_comment_clear(&mTheoraComment);
         theora_info_clear(&mTheoraInfo);
         ogg_sync_clear(&mOggSyncState);

         // And destroy our texture.
         destroyTexture();

         return false;
      }

      mReady = true;
   }

   if(doPlay)
      play();

   return true;
}

bool TheoraTexture::parseHeaders()
{
   ogg_packet   sOggPacket;
   S32         nTheora = 0;
   S32         nVorbis = 0;
   S32         ret;

   mHasVorbis = false;

   // Parse the headers
   // find theora and vorbis streams
   // search pages till you find the headers
   mTheoraFile->setPosition(0);

   while(1)
   {
      ret = bufferOggPage();
      if(ret == 0)
         break;

      if(!ogg_sync_pageout(&mOggSyncState, &mOggPage))
         break;

      ogg_stream_state testStream;

      if(!ogg_page_bos(&mOggPage))
      {
         // this is not an initial header, queue it up
         // exit stream header finding loop (headers always come before non header stuff)
         queueOggPage(&mOggPage);
         break;
      }

      // create a stream
      ogg_stream_init(&testStream, ogg_page_serialno(&mOggPage));
      ogg_stream_pagein(&testStream, &mOggPage);
      ogg_stream_packetout(&testStream, &sOggPacket);

      // test if its a theora header
      if(theora_decode_header(&mTheoraInfo, &mTheoraComment, &sOggPacket) >= 0)
      {
         // it is theora, copy testStream over to the theora stream
         dMemcpy(&mOggTheoraStream, &testStream, sizeof(testStream));
         nTheora = 1;
      }
      // test if its vorbis
      else if(vorbis_synthesis_headerin(&mVorbisInfo, &mVorbisComment, &sOggPacket) >= 0)
      {
         // it is vorbis, copy testStream over to the vorbis stream
         dMemcpy(&mOggVorbisStream, &testStream, sizeof(testStream));
         mHasVorbis = true;
         nVorbis = 1;
      }
      else
      {
         // some other stream header? unsupported, toss it
         ogg_stream_clear(&testStream);
      }

      // if both vorbis and theora have been found, exit loop
      if(nVorbis && nTheora)
         break;
   }

   if(!nTheora)
   {
      // no theora stream header found
      Con::errorf("TheoraTexture::parseHeaders - No theora stream headers found.");

      // HAVE to have theora, thats what this class is for. return failure
      return false;
   }

   // we've now identified all the streams. parse (toss) the secondary header packets.
   // it looks like we just have to throw out a few packets from the header page
   // so that they arent mistaken as theora movie data? nothing is done with these things..
   while((nTheora < 3) ||
        (nVorbis && nVorbis < 3))
   {
      // look for further theora headers

      while((nTheora < 3) &&
           (ret = ogg_stream_packetout(&mOggTheoraStream, &sOggPacket)))
      {
         if(ret < 0)
         {
            Con::errorf("TheoraPlayer::parseHeaders - Error parsing Theora stream headers; corrupt stream? (nothing read?)");
            return false;
         }

         if(theora_decode_header(&mTheoraInfo, &mTheoraComment, &sOggPacket))
         {
            Con::errorf("TheoraPlayer::parseHeaders - Error parsing Theora stream headers; corrupt stream?  (failed to decode)");
            return false;
         }

         // Sanity around corrupt headers.
         nTheora++;
         if(nTheora == 3)
            break;
      }

      // look for more vorbis headers
      while((nVorbis) &&
           (nVorbis < 3) &&
           (ret = ogg_stream_packetout(&mOggVorbisStream, &sOggPacket)))
      {
         if(ret < 0)
         {
            Con::errorf("Error parsing vorbis stream headers; corrupt stream? (nothing read?)");
            return false;
         }
         if(vorbis_synthesis_headerin(&mVorbisInfo, &mVorbisComment, &sOggPacket))
         {
            Con::errorf("Error parsing Vorbis stream headers; corrupt stream? (bad synthesis_headerin)");
            return false;
         }
         nVorbis++;
         if(nVorbis == 3)
            break;
      }

      // The header pages/packets will arrive before anything else we
      // care about, or the stream is not obeying spec
      // continue searching the next page for the headers

      // put the next page into the theora stream
      if(ogg_sync_pageout(&mOggSyncState, &mOggPage) > 0)
      {
         queueOggPage(&mOggPage);
      }
      else
      {
         // if there are no more pages, buffer another one
         const S32 ret = bufferOggPage();

         // if there is nothing left to buffer..
         if(ret == 0)
         {
            Con::errorf("TheoraTexture::parseHeaders - End of file while searching for codec headers.");
            return false;
         }
      }
   }
   return true;
}

bool TheoraTexture::createVideoBuffers()
{
   // Set up our texture.
   const GBitmap *bmp = new GBitmap(
                        getMax((U32)mTheoraInfo.frame_width, (U32)mTheoraInfo.width),
                        getMax((U32)mTheoraInfo.frame_height, (U32)mTheoraInfo.height),
                        false, GBitmap::RGB);

   mTextureHandle = new TextureHandle(NULL, bmp, true);

   // generate yuv conversion lookup tables
   generateLookupTables();
   return true;
}

bool TheoraTexture::createAudioBuffers(Audio::Description* desc)
{
   // Just to be sure, clear out Trevor.
   mMagicalTrevor.reset();

   // if they didnt pass a description...
   if(!desc)
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

      sDesc.mIs3D = false;
      sDesc.mVolume = 1.0f;
      sDesc.mIsLooping = false;
      sDesc.mType = 1;
      sDesc.mIsStreaming = true;
   }

   // create an audio handle to use
   mVorbisHandle = alxCreateSource(desc, "oggMixedStream");
   if(!mVorbisHandle)
   {
      Con::errorf("Could not alxCreateSource for oggMixedStream.\n");
      return false;
   }

   // get a pointer for it
   mVorbisBuffer = dynamic_cast<OggMixedStreamSource*>(alxFindAudioStreamSource(mVorbisHandle));
   if(!mVorbisBuffer)
   {
      alxStop(mVorbisHandle); // not sure how alxStop would find it if i couldnt..
      Con::errorf("Could not find oggMixedStreamSource ptr.");
      return false;
   }

   return true;
}

// 6K!! memory needed to speed up theora player. Small price to pay!
static S32 sAdjCrr[256];
static S32 sAdjCrg[256];
static S32 sAdjCbg[256];
static S32 sAdjCbb[256];
static S32 sAdjY[256];
static U8  sClampBuff[1024];
static U8* sClamp = sClampBuff + 384;

// precalculate adjusted YUV values for faster RGB conversion
void TheoraTexture::generateLookupTables()
{
   static bool fGenerated = false;
   S32 i;

   for(i = 0; i < 256; i++)
   {
      sAdjCrr[i] = (409 * (i - 128) + 128) >> 8;
      sAdjCrg[i] = (208 * (i - 128) + 128) >> 8;
      sAdjCbg[i] = (100 * (i - 128) + 128) >> 8;
      sAdjCbb[i] = (516 * (i - 128) + 128) >> 8;
      sAdjY[i] = (298 * (i - 16)) >> 8;
   }

   // and setup LUT clamp range
   for(i = -384; i < 640; i++)
   {
      sClamp[i] = mClamp(i, 0, 0xFF);
   }
}

void TheoraTexture::drawFrame()
{
   yuv_buffer yuv;

   // decode a frame! (into yuv)
   theora_decode_YUVout(&mTheoraState, &yuv);

   // get destination buffer (and 1 row offset)
   GBitmap *bmp = mTextureHandle->getBitmap();
   U8* dst0 = bmp->getAddress(0, 0);
   U8 *dst1 = dst0 + bmp->getWidth() * bmp->bytesPerPixel;

   // find picture offset
   const S32 pictOffset  = yuv.y_stride * mTheoraInfo.offset_y + mTheoraInfo.offset_x;

   const U8 *pY0, *pY1, *pU, *pV;

   for(S32 y = 0; y < yuv.y_height; y += 2)
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
      dst0 = dst1;
      dst1 += bmp->getWidth() * bmp->bytesPerPixel;
   }
}

bool TheoraTexture::play()
{
   if(mReady && !mPlaying)
      mPlayThread = new Thread((ThreadRunFunction)playThread, (S32) this, 1);

   return mPlayThread;
}

void TheoraTexture::stop()
{
   mPlaying = false;

   if(mPlayThread)
   {
      SAFE_DELETE(mPlayThread);
   }
}

void TheoraTexture::playThread( void *udata )
{
   TheoraTexture* pThis = (TheoraTexture *)udata;
   pThis->playLoop();
}

bool TheoraTexture::playLoop()
{
   bool fMoreVideo = true;
   bool fMoreAudio = mHasVorbis;

   // timing variables
   F64 dVBuffTime = 0;
   F64 dLastFrame = 0;

   mPlaying = true;
   mCurrentTime = 0.f;
   mStartTick = Platform::getRealMilliseconds();

   bool isAudioActive = false;

   while(mPlaying)
   {
      if(fMoreAudio)
         fMoreAudio = readyAudio();

      if(fMoreVideo)
         fMoreVideo = readyVideo(dLastFrame, dVBuffTime);

      if(!fMoreVideo && !fMoreAudio)
         break;   // if we have no more audio to buffer, and no more video frames to display, we are done

      // if we haven't started yet, start it! :)
      if(!isAudioActive && mHasVorbis)
      {
         alxPlay(mVorbisHandle);
         isAudioActive = true;
      }

      // if we're set for the next frame, sleep
      /*S32 t = (int)((double) 1000 * (dVBuffTime - getTheoraTime()));
      if(t>0)
         Platform::sleep(t); */
      U32 safety = 0;
      while((dVBuffTime - getTheoraTime()) >= 0.f && safety < 500)
      {
         safety++;
         Platform::sleep(2);
      }

      // time to draw the frame!
      drawFrame();

      // keep track of the last frame time
      dLastFrame = getTheoraTime();
   }

   if(mHasVorbis)
   {
      alxStop(mVorbisHandle);
      mMagicalTrevor.reset();
   }

   mPlaying = false;
   mVorbisHandle = NULL;
   mVorbisBuffer = NULL;

   return false;
}

// ready a single frame (not a late one either)
bool TheoraTexture::readyVideo(F64 dLastFrame, F64& dVBuffTime)
{
   ogg_packet sOggPacket;

   while(1)
   {
      PROFILE_START(TheoraTexture_readyVideo);

      // get a video packet
      if(ogg_stream_packetout(&mOggTheoraStream, &sOggPacket) > 0)
      {
         theora_decode_packetin(&mTheoraState, &sOggPacket);

         dVBuffTime = theora_granule_time(&mTheoraState, mTheoraState.granulepos);

         // check if this frame time has not passed yet.
         // If the frame is late we need to decode additional
         // ones and keep looping, since theora at this stage
         // needs to decode all frames
         const F64 dNow = getTheoraTime();

         if((dVBuffTime - dNow) >= 0.0)
         {
            PROFILE_END();
            return true; // got a good frame, not late, ready to break out
         }
         else if((dNow - dLastFrame) >= 1.0)
         {
            PROFILE_END();
            return true; // display at least one frame per second, regardless
         }

         // else frame is dropped (its behind), look again
      }
      else   // get another page
      {
         if(!demandOggPage()) // end of file
         {
            PROFILE_END();
            return false;
         }
      }

      //else we got a page, try and get a frame out of it
      PROFILE_END();
   }
}


// buffers up as much audio as it can fit into OggMixedStream audiostream thing
bool TheoraTexture::readyAudio()
{
#define BUFFERSIZE 8192

   ogg_packet sOggPacket;
   ALuint   bufferId = 0;
   S32 ret, i, count;
   float **pcm;

   // i was making buffers to fit the exact size
   // but the memory manager doesn't seem to be working
   // with multiple threads..

   S16 samples[BUFFERSIZE]; // this should be large enough

   // If i don't have a buffer to put samples into..
   while(1)
   {
      PROFILE_START(TheoraTexture_readyAudio);

      bufferId = mVorbisBuffer->GetAvailableBuffer();
      if(!bufferId) // buffered all that it cant fit
      {
         PROFILE_END();
         return true;
      }

      // Skip if we're ahead of the video...

      // if the buffer is ready now
      // fill it!

      while(!(ret = vorbis_synthesis_pcmout(&mVorbisDspState, &pcm)))
      {
         // no pending audio; is there a pending packet to decode?
         if(ogg_stream_packetout(&mOggVorbisStream, &sOggPacket) > 0)
         {
            if(vorbis_synthesis(&mVorbisBlock, &sOggPacket) == 0) // test for success!
               vorbis_synthesis_blockin(&mVorbisDspState, &mVorbisBlock);
         }
         else   // we need more data; break out to suck in another page
         {
            if(!demandOggPage())
            {
               PROFILE_END();
               return false;   // end of file
            }
         }
      }

      // found samples to buffer!
      for(count = i = 0; i < ret && i < (BUFFERSIZE * mVorbisInfo.channels); i++)
      {
         for(int j = 0; j < mVorbisInfo.channels; j++)
         {
            int val = (int)(pcm[j][i] * 32767.f);
            if(val > 32767)
               val = 32767;
            if(val < -32768)
               val = -32768;

#if defined(TORQUE_OS_MAC) && !defined(TORQUE_BIG_ENDIAN)
            samples[count++] = ((val << 8) & 0xFF00) | ((val >> 8) & 0x00FF);
#else
            samples[count++] = val;
#endif
         }
      }

      // bump up my buffering position
      vorbis_synthesis_read(&mVorbisDspState, i);

      // by this point the buffer should be filled (or as close as its gonna get)
      // ... Queue buffer
      alBufferData(  bufferId,
                     (mVorbisInfo.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
                     samples,
                     count * 2,
                     mVorbisInfo.rate);

      // Note the time for synchronization by magical trevor.
      const F64 newTimeSlice = F64(ret) / F64(mVorbisInfo.rate);

      mMagicalTrevor.postBuffer(bufferId, newTimeSlice);

      mVorbisBuffer->QueueBuffer(bufferId);

      PROFILE_END();
   }
}

F64 TheoraTexture::getTheoraTime()
{
   if(mHasVorbis)
   {
      // We have audio, so synch to audio track.
      ALint buf=-1;
      alGetSourcei(mVorbisBuffer->mSource, AL_BUFFER, &buf);
      mCurrentTime += mMagicalTrevor.advanceTime(buf);
      return mCurrentTime;
   }
   else
   {
      // We have no audio, just synch to start time.
      return (double)0.001 * (double)(Platform::getRealMilliseconds() - mStartTick);
   }
}


// helpers

// function does whatever it can get pages into streams
bool TheoraTexture::demandOggPage()
{
   while(1)
   {
      if(ogg_sync_pageout(&mOggSyncState,&mOggPage) > 0)
      {
         // found a page, queue it to its stream
         queueOggPage(&mOggPage);
         return true;
      }

      if(bufferOggPage() == 0)
      {
         // Ogg buffering stopped, end of file reached
         return false;
      }
   }
}

// grabs some more compressed bitstream and syncs it for page extraction
S32 TheoraTexture::bufferOggPage()
{
   char *buffer = ogg_sync_buffer(&mOggSyncState, 4096);

   // this is a bit of a hack, i guess, i dont know how else to do it
   // you dont want to send extra data to ogg_sync or it will try and
   // pull it out like its real data. thats no good
   // grab current position
   U32 bytes = mTheoraFile->getPosition();

   mTheoraFile->read(4096, buffer);

   // find out how much was read
   bytes = mTheoraFile->getPosition() - bytes;

   // give it to ogg and tell it how many bytes
   ogg_sync_wrote(&mOggSyncState, bytes);

   return bytes;
}

// try and put the page into the theora and vorbis streams,
// they wont accept pages that arent for them
S32 TheoraTexture::queueOggPage(ogg_page *page)
{
   ogg_stream_pagein(&mOggTheoraStream, page);
   if(mHasVorbis)
      ogg_stream_pagein(&mOggVorbisStream, page);
   return 0;
}

// returns a handle for OpenGL calls
U32 TheoraTexture::getGLName()
{
   if(mTextureHandle)
   {
      mTextureHandle->refresh();
      return mTextureHandle->getGLName();
   }
   return 0;
}

// copies the newest texture data to openGL video memory
void TheoraTexture::refresh()
{
   if(mTextureHandle)
      mTextureHandle->refresh();
}
