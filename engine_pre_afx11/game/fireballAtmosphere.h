//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _FIREBALLATMOSPHERE_H_
#define _FIREBALLATMOSPHERE_H_

#ifndef _GAMEBASE_H_
#include "game/gameBase.h"
#endif
#ifndef _TSSHAPE_H_
#include "ts/tsShape.h"
#endif

class AudioProfile;
struct DebrisData;

//--------------------------------------------------------------------------
// Fireball Atmosphere Data
//--------------------------------------------------------------------------
class FireballAtmosphereData : public GameBaseData {
   typedef GameBaseData Parent;

  public:
  
   DebrisData* fireball;
   S32 fireballID;

   FireballAtmosphereData();

   bool onAdd();
   static void  initPersistFields();

   void packData(BitStream* stream);
   void unpackData(BitStream* stream);

   DECLARE_CONOBJECT(FireballAtmosphereData);
};


//--------------------------------------------------------------------------
// Fireball atmosphere
//--------------------------------------------------------------------------
class FireballAtmosphere : public GameBase
{
   typedef GameBase Parent;

  private:
   FireballAtmosphereData *mDataBlock;
   AUDIOHANDLE mAudioHandle;
   F32 mTimeSinceLastDrop;

   F32 mDropRadius;
   F32 mDropsPerMinute;
   F32 mMinDropAngle;
   F32 mMaxDropAngle;
   F32 mStartVelocity;
   F32 mDropHeight;
   VectorF mDropDir;

  protected:
   bool onAdd();
   void onRemove();

   void advanceTime(F32 dt);
   void dropNewFireball();

   // Rendering
  protected:
   bool prepRenderImage(SceneState*, const U32, const U32, const bool);
   void renderObject(SceneState*, SceneRenderImage*);
   U32  packUpdate(NetConnection*, U32 mask, BitStream* stream);
   void unpackUpdate(NetConnection*, BitStream* stream);

  public:

   FireballAtmosphere();

   bool onNewDataBlock(GameBaseData* dptr);
   static void initPersistFields();

   DECLARE_CONOBJECT(FireballAtmosphere);
};

#endif // _H_FIREBALL_ATMOSPHERE
