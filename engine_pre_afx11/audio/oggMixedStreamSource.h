//--------------------------------------------
// oggMixedStreamSource.h
// header for audio stream dummy class, basicly
// an audio buffer filled remotely
//
//--------------------------------------------

#ifndef _OGGMIXEDSTREAMSOURCE_H_
#define _OGGMIXEDSTREAMSOURCE_H_

#ifndef _AUDIOSTREAMSOURCE_H_
#include "audio/audioStreamSource.h"
#endif

#define BUFFERCNT 48 // Only buffer this much or latency is too high when seeking, etc!

class OggMixedStreamSource: public AudioStreamSource
{
public:
      OggMixedStreamSource(const char *filename);
      virtual ~OggMixedStreamSource();

      virtual bool initStream();
      virtual bool updateBuffers();
      virtual void freeStream();

      bool hasAvailableBuffer();
      ALuint GetAvailableBuffer();
      bool QueueBuffer(ALuint BufferID);
      void PlayStream();

      void empty();

      virtual F32 getElapsedTime()
      {
         return 0.0;
      }

      virtual F32 getTotalTime()
      {
         return 1.0f;
      }

private:
      ALuint mBufferList[BUFFERCNT];
      bool m_fBufferInUse[BUFFERCNT];

      bool bBuffersAllocated;

      void clear();     
};

#endif // _OGGMIXEDSTREAMSOURCE_H_
