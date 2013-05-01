//--------------------------------------
// vorbisStreamSource.cc
// implementation of streaming audio source
//
// Kurtis Seebaldt
//--------------------------------------

/*

MMO Kit
-------

- Increased buffer and chunk sizes.
- Improved resource usage.
- Fixed some potential bugs.
- If a source stops now notifies python connection to handle it.

*/

#include "audio/vorbisStreamSource.h"
#include "vorbis/codec.h"
#include "console/console.h"  // <-- MMO Kit

// Begin MMO Kit
#define BUFFERSIZE 262144
#define CHUNKSIZE 8192
/* TGE Original Code
#define BUFFERSIZE 32768
#define CHUNKSIZE 4096
*/
// End MMO Kit

#if defined(TORQUE_BIG_ENDIAN)
#define ENDIAN 1
#else
#define ENDIAN 0
#endif

extern const char * MusicPlayerStreamingHook(const AUDIOHANDLE & handle);

VorbisStreamSource::VorbisStreamSource(const char *filename)
{
   stream = NULL;
   bIsValid = false;
   bBuffersAllocated = false;
   bVorbisFileInitialized = false;
   mBufferList[0] = 0;
   clear();

   mFilename = filename;
   mPosition = Point3F(0.f,0.f,0.f);
   // Begin MMO Kit
   buffersinqueue = 0;
   mNumBuffers = 0;
   // End MMO Kit
}

VorbisStreamSource::~VorbisStreamSource()
{
   if(bReady && bIsValid)
      freeStream();
}

void VorbisStreamSource::clear()
{
   if(stream)
      freeStream();

   mHandle           = NULL_AUDIOHANDLE;
   mSource           = NULL;

   if(mBufferList[0] != 0)
      // Begin MMO Kit
      alDeleteBuffers(mNumBuffers, mBufferList);
      /* TGE Original Code
      alDeleteBuffers(NUMBUFFERS, mBufferList);
      */
      // End MMO Kit
   for(int i = 0; i < NUMBUFFERS; i++)
      mBufferList[i] = 0;

   dMemset(&mDescription, 0, sizeof(Audio::Description));
   mEnvironment = 0;
   mPosition.set(0.f,0.f,0.f);
   mDirection.set(0.f,1.f,0.f);
   mPitch = 1.f;
   mScore = 0.f;
   mCullTime = 0;

   bReady = false;
   bFinishedPlaying = false;
   bIsValid = false;
   bBuffersAllocated = false;
   bVorbisFileInitialized = false;
}

bool VorbisStreamSource::initStream()
{
   vorbis_info *vi;

   ALint error;

   bFinished = false;
   bFinishedPlaying = false;  // <-- MMO Kit

   // JMQ: changed buffer to static and doubled size.  workaround for
   // https://206.163.64.242/mantis/view_bug_page.php?f_id=0000242
   // Begin MMO Kit
   static char data[BUFFERSIZE];
   /* TGE Original Code
   static char data[BUFFERSIZE*2];
   */
   // End MMO Kit

   alSourceStop(mSource);
   alSourcei(mSource, AL_BUFFER, 0);

   stream = ResourceManager->openStream(mFilename);
   if(stream != NULL)
   {
      if(vf.ov_open(stream, NULL, 0) < 0)
      {
         return false;
      }

      bVorbisFileInitialized = true;

      //Read Vorbis File Info
      vi = vf.ov_info(-1);
      freq = vi->rate;

      long samples = (long)vf.ov_pcm_total(-1);

      if(vi->channels == 1)
      {
         format = AL_FORMAT_MONO16;
         DataSize = 2 * samples;
      }
      else
      {
         format = AL_FORMAT_STEREO16;
         DataSize = 4 * samples;
      }
      DataLeft = DataSize;

      // Clear Error Code
      alGetError();
      
      // Begin MMO Kit
      mNumBuffers = DataSize / BUFFERSIZE;
      if(DataSize % BUFFERSIZE)
         mNumBuffers++;
      
      if(mNumBuffers > NUMBUFFERS)
         mNumBuffers = NUMBUFFERS;
      
      alGenBuffers(mNumBuffers, mBufferList);
      /* TGE Original Code
      alGenBuffers(NUMBUFFERS, mBufferList);
      */
      // End MMO Kit
      if ((error = alGetError()) != AL_NO_ERROR)
         return false;

      bBuffersAllocated = true;

      int numBuffers = 0;
      // Begin MMO Kit
      for(int loop = 0; loop < mNumBuffers; loop++)
      /* TGE Original Code
      for(int loop = 0; loop < NUMBUFFERS; loop++)
      */
      // End MMO Kit
      {
         ALuint DataToRead = (DataLeft > BUFFERSIZE) ? BUFFERSIZE : DataLeft;
         if (DataToRead == DataLeft)
            bFinished = AL_TRUE;

         long ret = oggRead(data, BUFFERSIZE, ENDIAN, &current_section);
         if(ret <= 0)
         {
            bFinished = AL_TRUE;
            break;
         }

         DataLeft -= ret;
         alBufferData(mBufferList[loop], format, data, ret, freq);
         ++numBuffers;

         if ((error = alGetError()) != AL_NO_ERROR)
            return false;
         if(bFinished)
            break;
      }

      // Queue the buffers on the source
      // Begin MMO Kit
      alSourceQueueBuffers(mSource, mNumBuffers, mBufferList);
      /* TGE Original Code
      alSourceQueueBuffers(mSource, NUMBUFFERS, mBufferList);
      */
      // End MMO Kit
      if ((error = alGetError()) != AL_NO_ERROR)
         return false;

      alSourcei(mSource, AL_LOOPING, AL_FALSE);
      bReady = true;
   }
   else
   {
      return false;
   }
   bIsValid = true;

   return true;
}

bool VorbisStreamSource::updateBuffers()
{
   ALint         processed;
   ALuint         BufferID;
   ALint         error;
   // JMQ: changed buffer to static and doubled size.  workaround for
   // https://206.163.64.242/mantis/view_bug_page.php?f_id=0000242
   // Begin MMO Kit
   static char data[BUFFERSIZE];
   /* TGE Original Code
   static char data[BUFFERSIZE*2];
   */
   // End MMO Kit

   // don't do anything if stream not loaded properly
   if(!bIsValid)
      return false;
   
   // Begin MMO Kit
   ALint state;
   alGetSourcei(mSource, AL_SOURCE_STATE, &state);

   if (state == AL_STOPPED)
   {
      Con::evaluate("Py::SongStopped();");
      return false;
   }

   // reset AL error code
   alGetError();
   /* TGE Original Code
   if(bFinished && mDescription.mIsLooping)
      resetStream();

   // reset AL error code
   alGetError();

#if 1 //def TORQUE_OS_LINUX
   // JMQ: this doesn't really help on linux.  it may make things worse.
   // if it doesn't help on mac/win either, could disable it.
   ALint state;
   alGetSourcei(mSource, AL_SOURCE_STATE, &state);
   if (state == AL_STOPPED)
   {
      // um, yeah.  you should be playing
      // restart
      alSourcePlay(mSource);
      //#ifdef TORQUE_DEBUG
      //Con::errorf(">><<>><< THE MUSIC STOPPED >><<>><<");
      //#endif
      return true;
   }
#endif
   */
   // End MMO Kit

#ifdef TORQUE_OS_LINUX
   checkPosition();
#endif

   // Get status
   alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);

   // If some buffers have been played, unqueue them and load new audio into them, then add them to the queue
   if (processed > 0)
   {
      while (processed)
      {
         alSourceUnqueueBuffers(mSource, 1, &BufferID);
         if ((error = alGetError()) != AL_NO_ERROR)
            return false;

         if (!bFinished)
         {
            ALuint DataToRead = (DataLeft > BUFFERSIZE) ? BUFFERSIZE : DataLeft;

            if (DataToRead == DataLeft)
               bFinished = AL_TRUE;

            long ret = oggRead(data, BUFFERSIZE, ENDIAN, &current_section);
            if(ret > 0)
            {
               DataLeft -= ret;

               alBufferData(BufferID, format, data, ret, freq);
               if ((error = alGetError()) != AL_NO_ERROR)
                  return false;

               // Queue buffer
               alSourceQueueBuffers(mSource, 1, &BufferID);
               if ((error = alGetError()) != AL_NO_ERROR)
                  return false;
            }

            processed--;

            // Begin MMO Kit
            /* TGE Original Code
            if(bFinished && mDescription.mIsLooping)
            {
               resetStream();
            }
            */
            // End MMO Kit
         }
         else
         {
            buffersinqueue--;
            processed--;

            // Begin MMO Kit
            if (buffersinqueue <= 0)
            /* TGE Original Code
            if (buffersinqueue == 0)
            */
            // End MMO Kit
            {
               bFinishedPlaying = AL_TRUE;
               return AL_FALSE;
            }
         }
      }
   }

   return true;
}

void VorbisStreamSource::freeStream()
{
   bReady = false;

   if(stream != NULL)
      ResourceManager->closeStream(stream);

   stream = NULL;

   if(bBuffersAllocated)
   {
      if(mBufferList[0] != 0)
      // Begin MMO Kit
      {
         alSourceUnqueueBuffers(mSource, mNumBuffers, mBufferList);
         alDeleteBuffers(mNumBuffers, mBufferList);
      }
      /* TGE Original Code
         alDeleteBuffers(NUMBUFFERS, mBufferList);
      */
      // End MMO Kit
      for(int i = 0; i < NUMBUFFERS; i++)
         mBufferList[i] = 0;

      bBuffersAllocated = false;
   }

   if(bVorbisFileInitialized)
   {
      vf.ov_clear();
      bVorbisFileInitialized = false;
   }
}

void VorbisStreamSource::resetStream()
{
   // MusicPlayerStreamingHook allow you to create a handler
   // where you can rotate through streaming files
   // Comment in and provide hook if desired.
   //
   //const char * newFile = MusicPlayerStreamingHook(mHandle);
   //if (newFile)
   //{
   //   setNewFile(newFile);
   //   return;
   //}
   //else
   {
      vf.ov_pcm_seek(0);
      DataLeft = DataSize;
      bFinished = AL_FALSE;
   }
}

void VorbisStreamSource::setNewFile(const char * file)
{
   //---------------------
   // close down old file...
   //---------------------

   if(stream != NULL)
   {
      ResourceManager->closeStream(stream);
      stream = NULL;
   }

   if(bVorbisFileInitialized)
   {
      vf.ov_clear();
      bVorbisFileInitialized = false;
   }

   //---------------------
   // open up new file...
   //---------------------

   mFilename = file;
   stream = ResourceManager->openStream(mFilename);
   if(stream != NULL)
   {
      if(vf.ov_open(stream, NULL, 0) < 0)
      {
         bFinished = AL_TRUE;
         bVorbisFileInitialized = false;
         bIsValid = false;
         return;
      }

      //Read Vorbis File Info
      vorbis_info * vi = vf.ov_info(-1);
      freq = vi->rate;

      long samples = (long)vf.ov_pcm_total(-1);

      if(vi->channels == 1)
      {
         format = AL_FORMAT_MONO16;
         DataSize = 2 * samples;
      }
      else
      {
         format = AL_FORMAT_STEREO16;
         DataSize = 4 * samples;
      }
      DataLeft = DataSize;

      // Clear Error Code
      alGetError();

      bFinished = AL_FALSE;
      bVorbisFileInitialized = true;
      bIsValid = true;
   }
}

// ov_read() only returns a maximum of one page worth of data
// this helper function will repeat the read until buffer is full
long VorbisStreamSource::oggRead(char *buffer,int length,
                                 int bigendianp,int *bitstream)
{
   long bytesRead = 0;
   long totalBytes = 0;
   long offset = 0;
   long bytesToRead = 0;

   while((offset) < length)
   {
      if((length - offset) < CHUNKSIZE)
         bytesToRead = length - offset;
      else
         bytesToRead = CHUNKSIZE;

      bytesRead = vf.ov_read(buffer, bytesToRead, bigendianp, bitstream);
      //#ifdef TORQUE_OS_LINUX
#if 1 // Might fix mac audio issue and possibly others...based on references, this looks like correct action
      // linux ver will hang on exit after a stream loop if we don't
      // do this
      if (bytesRead == OV_HOLE)
         // retry, see:
         // http://www.xiph.org/archives/vorbis-dev/200102/0163.html
         // http://www.mit.edu/afs/sipb/user/xiphmont/ogg-sandbox/vorbis/doc/vorbis-errors.txt
         continue;
#endif

      if(bytesRead <= 0)
         break;
      offset += bytesRead;
      buffer += bytesRead;
   }
   return offset;
}

#ifdef TORQUE_OS_LINUX

// JMQ: OpenAL sometimes replaces the stream source's position with its own
// nifty value, causing the music to pan around the listener.  how nice.
// this function checks to see if the current source position in openal
// is near the initial position, and slams it to the correct value if it
// is wrong.

// This is a bad place to put this, but I don't feel like adding a new
// .cc file.  And since this is an incredibly lame hack to
// workaround a stupid OpenAL bug, I see no point in overengineering it.

void AudioStreamSource::checkPosition()
{
   ALfloat pos[3];
   alGetSourcefv(mSource, AL_POSITION, pos);

   // just compute the difference between the openal pos and the
   // correct pos.  it better be pretty friggin small.
   Point3F openalPos(pos[0], pos[1], pos[2]);
   F32 slopDist = 0.0001f;

   F32 dist = mFabs((openalPos - mPosition).len());
   if (dist > slopDist)
      // slam!
      alSource3f(mSource, AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
}

#endif

F32 VorbisStreamSource::getElapsedTime()
{
   return vf.ov_time_tell();
}

F32 VorbisStreamSource::getTotalTime()
{
   return vf.ov_time_total(-1);
}
