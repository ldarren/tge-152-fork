//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PRECIPITATION_H_
#define _PRECIPITATION_H_

#include "game/gameBase.h"
#include "audio/audioDataBlock.h"

//--------------------------------------------------------------------------
/// Precipitation datablock.
class PrecipitationData : public GameBaseData {
   typedef GameBaseData Parent;

  public:
   AudioProfile* soundProfile;
   S32           soundProfileId;    ///< Ambient sound

   StringTableEntry mDropName;      ///< Texture filename for raindrop
   StringTableEntry mSplashName;    ///< Texture filename for splash

   F32 mDropSize;                   ///< Droplet billboard size
   F32 mSplashSize;                 ///< Splash billboard size
   bool mUseTrueBillboards;         ///< True to use true billboards, false for axis-aligned billboards
   S32 mSplashMS;                   ///< How long in milliseconds a splash will last

   PrecipitationData();
   DECLARE_CONOBJECT(PrecipitationData);
   bool onAdd();
   static void  initPersistFields();
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
};
DECLARE_CONSOLETYPE(PrecipitationData)

struct Raindrop
{
   F32 velocity;           ///< How fast the drop is falling downwards
   Point3F position;       ///< Position of the drop
   Point3F renderPosition; ///< Interpolated render-position of the drop
   F32 time;               ///< Time into the turbulence function
   F32 mass;               ///< Mass of drop used for how much turbulence/wind effects the drop

   U32 texCoordIndex;      ///< Which piece of the material will be used

   bool toRender;          ///< Don't want to render all drops, just the ones that pass a few tests
   bool valid;             ///< Drop becomes invalid after hitting something.  Just keep updating
                           ///<   the position of it, but don't render until it hits the bottom
                           ///<   of the renderbox and respawns

   Point3F hitPos;         ///< Point at which the drop will collide with something

   Raindrop *nextSplashDrop; ///< Linked list cruft for easily adding/removing stuff from the splash list
   Raindrop *prevSplashDrop; ///< Same as next but previous!
   SimTime animStartTime;    ///< Animation time tracker
   U32 splashIndex;          ///< Texture index for which frame of the splash to render
   U32 hitType;              ///< What kind of object the drop will hit

   Raindrop* next;         ///< linked list cruft

   Raindrop()
   :  velocity( 0.0f ),
      time( 0.0f ),
      mass( 1.0f ),
      texCoordIndex( 0 ),
      toRender( false ),
      valid( true ),
	  hitPos( 0.0f, 0.0f, 0.0f ),
      nextSplashDrop( NULL ),
      prevSplashDrop( NULL ),
      animStartTime( 0 ),
      splashIndex( 0 ),
      hitType( 0 ),
      next( NULL )
   {
   }
};

//--------------------------------------------------------------------------
class Precipitation : public GameBase
{
  private:

   typedef GameBase Parent;
   PrecipitationData*   mDataBlock;

   const static U32   csmDropsPerSide;       ///< How many drops are on a side of the material texture
   const static F32   csmDropsPerSideF;      ///< How many drops are on a side of the material texture
   const static U32   csmFramesPerSide;      ///< How many frames are on a side of a splash animation
   const static F32   csmFramesPerSideF;     ///< How many frames are on a side of a splash animation
   
   Raindrop *mDropHead;    ///< Drop linked list head
   Raindrop *mSplashHead;  ///< Splash linked list head
   Point2F  *texCoords;    ///< texture coords for rain texture
   Point2F  *splashCoords; ///< texture coordinates for splash texture

   AUDIOHANDLE mAudioHandle;        ///< Ambient sound handle
   TextureHandle mDropHandle;    ///< Texture handle for raindrop
   TextureHandle mSplashHandle;     ///< Texture handle for splash

   //console exposed variables
   S32 mNumDrops;                ///< Number of drops in the scene
   F32 mPercentage;              ///< Server-side set var (NOT exposed to console)
                                 ///< which controls how many drops are present [0,1]

   F32 mMinSpeed;                ///< Minimum downward speed of drops
   F32 mMaxSpeed;                ///< Maximum downward speed of drops

   F32 mMinMass;                 ///< Minimum mass of drops
   F32 mMaxMass;                 ///< Maximum mass of drops

   F32 mBoxWidth;                ///< How far away in the x and y directions drops will render
   F32 mBoxHeight;               ///< How high drops will render

   F32 mMaxTurbulence;           ///< Coefficient to sin/cos for adding turbulence
   F32 mTurbulenceSpeed;         ///< How fast the turbulence wraps in a circle
   bool mUseTurbulence;          ///< Whether to use turbulence or not (MAY EFFECT PERFORMANCE)

   bool mRotateWithCamVel;       ///< Rotate the drops relative to the camera velocity
                                 ///< This is useful for "streak" type drops

   bool mDoCollision;            ///< Whether or not to do collision

   struct
   {
      bool valid;
      U32 startTime;
      U32 totalTime;
      F32 startPct;
      F32 endPct;
   } mStormData;

   //other functions...
   void processTick(const Move*);
   void interpolateTick(F32 delta);

   VectorF getWindVelocity();
   void fillDropList();                      ///< Adds/removes drops from the list to have the right # of drops
   void killDropList();                      ///< Deletes the entire drop list
   void spawnDrop(Raindrop *drop);           ///< Fills drop info with random velocity, x/y positions, and mass
   void spawnNewDrop(Raindrop *drop);        ///< Same as spawnDrop except also does z position
   void findDropCutoff(Raindrop *drop);      ///< Casts a ray to see if/when a drop will collide
   inline void wrapDrop(Raindrop *drop, Box3F &box); ///< Wraps a drop within the specified box

   void createSplash(Raindrop *drop);        ///< Adds a drop to the splash list
   void destroySplash(Raindrop *drop);       ///< Removes a drop from the splash list


  protected:
   bool onAdd();
   void onRemove();

   // Rendering
   bool prepRenderImage(SceneState*, const U32, const U32, const bool);
   void renderObject(SceneState*, SceneRenderImage*);
   void renderPrecip(SceneState *state);
   void renderSplashes(SceneState *state);

  public:

   Precipitation();
   ~Precipitation();
   void inspectPostApply();

   enum
   {
      DataMask       = Parent::NextFreeMask << 0,
      PercentageMask = Parent::NextFreeMask << 1,
      StormMask      = Parent::NextFreeMask << 2,
      NextFreeMask   = Parent::NextFreeMask << 3
   };

   bool onNewDataBlock(GameBaseData* dptr);
   DECLARE_CONOBJECT(Precipitation);
   static void initPersistFields();

   U32  packUpdate(NetConnection*, U32 mask, BitStream* stream);
   void unpackUpdate(NetConnection*, BitStream* stream);

   void setPercentage(F32 pct);
   void modifyStorm(F32 pct, U32 ms);
};

#endif // PRECIPITATION_H_

