//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _WEATHER_LIGHTNING_H_
#define _WEATHER_LIGHTNING_H_

#ifndef _NETCONNECTION_H_
#include "sim/netConnection.h"
#endif
#ifndef _GAMEBASE_H_
#include "game/gameBase.h"
#endif

#include "lightingSystem/sgLightManager.h"

class AudioProfile;
class WeatherLightning;

class WeatherLightningStrikeEvent : public NetEvent
{
   typedef NetEvent Parent;

  public:
   enum Constants {
      PositionalBits = 10
   };

   Point2F mStart;
   WeatherLightning* mLightning;

  public:
   WeatherLightningStrikeEvent();
   ~WeatherLightningStrikeEvent();

   void pack(NetConnection*, BitStream*);
   void write(NetConnection*, BitStream*){}
   void unpack(NetConnection*, BitStream*);
   void process(NetConnection*);

   DECLARE_CONOBJECT(WeatherLightningStrikeEvent);
};

class WeatherLightningData : public GameBaseData
{
   typedef GameBaseData Parent;
   
  protected:
   bool onAdd();
  
  public:
   enum {
      MaxSounds = 4,
      MaxStrikeTextures = 6,//8,
      MaxFlashTextures = 4, //6,
      MaxFuzzyTextures = 2, //4,
   };
   
   // primary strike texture
   U32 numStrikes;
   StringTableEntry strikeTextureNames[MaxStrikeTextures];
   TextureHandle strikeTextures[MaxStrikeTextures];
   
   // flash texture
   U32 numFlashes;
   StringTableEntry flashTextureNames[MaxFlashTextures];
   TextureHandle flashTextures[MaxFlashTextures];
   
   // fuzzy/stretch texture
   U32 numFuzzes;
   StringTableEntry fuzzyTextureNames[MaxFuzzyTextures];
   TextureHandle fuzzyTextures[MaxFuzzyTextures];
   
   //strike sound
   S32 strikeSoundId;
   AudioProfile* strikeSound;
   
   // thunder sounds
   U32 numSounds;
   S32 thunderSoundIds[MaxSounds];
   AudioProfile* thunderSounds[MaxSounds];
  
  public:
   WeatherLightningData();
   ~WeatherLightningData();
   
   void packData(BitStream*);
   void unpackData(BitStream*);
   bool preload(bool server, char errorBuffer[256]);

   DECLARE_CONOBJECT(WeatherLightningData);
   static void initPersistFields();
};

struct WeatherLightningBolt
{
   Point3F startPoint;
   Point3F endPoint;
   
   F32 currentAge;
   F32 deathAge;
   F32 strikeTime;
   
   TextureHandle* strikeTexture;
   TextureHandle* flashTexture;
   TextureHandle* fuzzyTexture;
   
   void render(const Point3F &camPos);
};

class WeatherLightning : public GameBase
{
   typedef GameBase Parent;
   
   // datablock
   WeatherLightningData* mDataBlock;
   
   U32 lastThink;
   U32 strikesPerMinute;
   U32 boltDeathAge;
   
   struct SoundEvent {
      S32     soundBlockId;
      MatrixF position;
      U32     time;
   };
   
   // only active on client
   VectorPtr<WeatherLightningBolt*> mActiveBolts;
   Vector<SoundEvent> mSoundEvents;
   
  protected:
   bool onAdd();
   void onRemove();
   bool onNewDataBlock(GameBaseData* dptr);
   
   // rendering
   bool prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
   void renderObject(SceneState *state, SceneRenderImage *image);

   // simulation
   void processTick(const Move *move);
   void advanceTime(F32 dt);
   
   // grab random textures
   TextureHandle* getRandomStrike();
   TextureHandle* getRandomFlash();
   TextureHandle* getRandomFuzzy();
   
   // grab random sounds
   S32 getRandomSound();
   
  public:
   WeatherLightning();
   ~WeatherLightning();
   
   // strike random point within object box
   void strikeRandomPoint();
   
   // receive lightning event and create lightning bolt
   void processEvent(WeatherLightningStrikeEvent*);

   U32  packUpdate  (NetConnection *conn, U32 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn,           BitStream *stream);
   
   DECLARE_CONOBJECT(WeatherLightning);
   static void initPersistFields();
};

#endif // _WEATHER_LIGHTNING_H_
