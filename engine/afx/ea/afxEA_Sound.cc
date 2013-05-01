
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"

#include "audio/audio.h"

#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Sound 

class afxEA_Sound : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  AudioProfile* sound_data;
  AUDIOHANDLE   sound_handle;

public:
  /*C*/         afxEA_Sound();
  /*D*/         ~afxEA_Sound();

  virtual void  ea_set_datablock(SimDataBlock*);
  virtual bool  ea_start();
  virtual bool  ea_update(F32 dt);
  virtual void  ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_Sound::afxEA_Sound()
{
  sound_data = 0;
  sound_handle = NULL_AUDIOHANDLE;
}

afxEA_Sound::~afxEA_Sound()
{
  sound_handle = NULL_AUDIOHANDLE;
}

void afxEA_Sound::ea_set_datablock(SimDataBlock* db)
{
  sound_data = dynamic_cast<AudioProfile*>(db);
}

bool afxEA_Sound::ea_start()
{
  if (!sound_data)
  {
    Con::errorf("afxEA_Sound::ea_start() -- missing or incompatible datablock.");
    return false;
  }
  return true;
}

bool afxEA_Sound::ea_update(F32 dt)
{
  if (sound_handle == NULL_AUDIOHANDLE)
  {
    // ADJUST STARTING POINT BASED ON ELAPSED?
    sound_handle = alxPlay(sound_data, &updated_xfm, 0);
  }

  if (sound_handle != NULL_AUDIOHANDLE)
  {
    alxSourceMatrixF(sound_handle, &updated_xfm);
    alxSourcef(sound_handle, AL_GAIN_LINEAR, updated_scale.x*fade_value);
  }

  return true;
}

void afxEA_Sound::ea_finish(bool was_stopped)
{
  if (sound_handle != NULL_AUDIOHANDLE)
  {
    alxStop(sound_handle);
    sound_handle = NULL_AUDIOHANDLE;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_SoundDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_SoundDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }
  virtual void  prepEffect(afxEffectWrapperData*) const;

  virtual afxEffectWrapper* create() const { return new afxEA_Sound; }
};

afxEA_SoundDesc afxEA_SoundDesc::desc;

bool afxEA_SoundDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(AudioProfile) == typeid(*db));
}

bool afxEA_SoundDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  AudioProfile* ap = (AudioProfile*) ew->effect_data;
  AudioDescription* ad = ap->mDescriptionObject;
  return (ad && ad->mDescription.mIsLooping) ? (ew->timing.lifetime < 0) : false;
}

extern ALuint alxGetWaveLen(ALuint buffer);

void afxEA_SoundDesc::prepEffect(afxEffectWrapperData* ew) const 
{ 
  if (ew->timing.lifetime < 0)
  {
    AudioProfile* snd = (AudioProfile*) ew->effect_data;
    if (snd->mDescriptionObject && !snd->mDescriptionObject->mDescription.mIsLooping)
    {
      static bool test_for_audio = true;
      static bool can_get_audio_len = false;

      if (test_for_audio)
      {
        can_get_audio_len = (ResourceManager->getCreateFunction(".ogg") != NULL);
        test_for_audio = false;
      }

      if (can_get_audio_len)
      {
        Resource<AudioBuffer> mBuffer = AudioBuffer::find(snd->mFilename);
        if (bool(mBuffer))
        {
          ew->timing.lifetime = 0.001f*alxGetWaveLen(mBuffer->getALBuffer());
          //Con::printf("SFX (%s) duration=%g", snd->mFilename, timing.lifetime);
        }
      }
      else
      {
        ew->timing.lifetime = 0;
        Con::printf("afxEA_SoundDesc::prepEffect() -- cannot get audio length from file, (%s).", snd->mFilename);
      }
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//