//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

/*

MMO Kit
-------



*/

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//    Changes:
//        obj-select -- object selection functionality
//         anim-clip -- sequence selection by afx effects
//       player-look -- modified player head and arm control
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _RPG_PLAYER_H_
#define _RPG_PLAYER_H_

#ifndef _SHAPEBASE_H_
#include "game/shapeBase.h"
#endif
#ifndef _BOXCONVEX_H_
#include "collision/boxConvex.h"
#endif

// Begin MMO Kit
#define RPG_ENCOUNTER_PVE 0
#define RPG_ENCOUNTER_RVR 1
#define RPG_ENCOUNTER_GVG 2
#define RPG_ENCOUNTER_PVP 3
// End MMO Kit

class ParticleEmitter;
class ParticleEmitterData;
class DecalData;
class SplashData;

//----------------------------------------------------------------------------

struct rpgPlayerData: public ShapeBaseData {
   typedef ShapeBaseData Parent;
   enum Constants {
      RecoverDelayBits = 7,
      JumpDelayBits = 7,
      NumSpineNodes = 6,
      ImpactBits = 3,
      NUM_SPLASH_EMITTERS = 3,
      BUBBLE_EMITTER = 2,
   };
   bool renderFirstPerson;    ///< Render the player shape in first person

   F32 pickupRadius;          ///< Radius around player for items (on server)
   F32 maxTimeScale;          ///< Max timeScale for action animations

// Begin MMO Kit
   F32 radius;
   F32 scale;
// End MMO Kit

   F32 minLookAngle;          ///< Lowest angle (radians) the player can look
   F32 maxLookAngle;          ///< Highest angle (radians) the player can look
   F32 maxFreelookAngle;      ///< Max left/right angle the player can look

   /// @name Physics constants
   /// @{

   F32 runForce;                   ///< Force used to accelerate player
   F32 runEnergyDrain;             ///< Energy drain/tick
   F32 minRunEnergy;               ///< Minimum energy required to run
   F32 maxForwardSpeed;            ///< Maximum forward speed when running
   F32 maxBackwardSpeed;           ///< Maximum backward speed when running
   F32 maxSideSpeed;               ///< Maximum side speed when running
   F32 maxUnderwaterForwardSpeed;  ///< Maximum underwater forward speed when running
   F32 maxUnderwaterBackwardSpeed; ///< Maximum underwater backward speed when running
   F32 maxUnderwaterSideSpeed;     ///< Maximum underwater side speed when running

   F32 maxStepHeight;         ///< Maximum height the player can step up
   F32 runSurfaceAngle;       ///< Maximum angle from vertical in degrees the player can run up

   F32 horizMaxSpeed;         ///< Max speed attainable in the horizontal
   F32 horizResistSpeed;      ///< Speed at which resistence will take place
   F32 horizResistFactor;     ///< Factor of resistence once horizResistSpeed has been reached

   F32 upMaxSpeed;            ///< Max vertical speed attainable
   F32 upResistSpeed;         ///< Speed at which resistence will take place
   F32 upResistFactor;        ///< Factor of resistence once upResistSpeed has been reached

   S32 recoverDelay;          ///< # tick
   F32 recoverRunForceScale;  ///< RunForce multiplier in recover state

   F32 jumpForce;             ///< Force exherted per jump
   F32 jumpEnergyDrain;       ///< Energy drained per jump
   F32 minJumpEnergy;         ///< Minimum energy required to jump
   F32 minJumpSpeed;          ///< Minimum speed needed to jump
   F32 maxJumpSpeed;          ///< Maximum speed before the player can no longer jump
   F32 jumpSurfaceAngle;      ///< Angle from vertical in degrees where the player can jump
   S32 jumpDelay;             ///< Delay time in ticks between jumps
   /// @}

/* Original TGE Code:
   /// @name Hitboxes
   /// @{

   F32 boxHeadPercentage;
   F32 boxTorsoPercentage;

   S32 boxHeadLeftPercentage;
   S32 boxHeadRightPercentage;
   S32 boxHeadBackPercentage;
   S32 boxHeadFrontPercentage;
   /// @}
*/

   F32 minImpactSpeed;        ///< Minimum impact speed required to apply fall damage

/* Original TGE Code:
   F32 decalOffset;
*/

   F32 groundImpactMinSpeed;      ///< Minimum impact speed required to apply fall damage with the ground
   VectorF groundImpactShakeFreq; ///< Frequency in each direction for the camera to shake
   VectorF groundImpactShakeAmp;  ///< How much to shake
   F32 groundImpactShakeDuration; ///< How long to shake
   F32 groundImpactShakeFalloff;  ///< How fast the shake disapates

   /// Zounds!
   enum Sounds {
      FootSoft,
      FootHard,
      FootMetal,
      FootSnow,
      FootShallowSplash,
      FootWading,
      FootUnderWater,
      FootBubbles,
      MoveBubbles,
      WaterBreath,
      ImpactSoft,
      ImpactHard,
      ImpactMetal,
      ImpactSnow,
      ImpactWaterEasy,
      ImpactWaterMedium,
      ImpactWaterHard,
      ExitWater,
      MaxSounds
   };
   AudioProfile* sound[MaxSounds];

   Point3F boxSize;           ///< Width, depth, height

   /// Animation and other data intialized in onAdd
   struct ActionAnimationDef {
      const char* name;       ///< Sequence name
      struct Vector {
         F32 x,y,z;
      } dir;                  ///< Default direction
   };
   struct ActionAnimation {
      const char* name;       ///< Sequence name
      S32      sequence;      ///< Sequence index
      VectorF  dir;           ///< Dir of animation ground transform
      F32      speed;         ///< Speed in m/s
      bool     velocityScale; ///< Scale animation by velocity
      bool     death;         ///< Are we dying?
   };
/* Original TGE Code:
   enum {
      // *** WARNING ***
      // These enum values are used to index the ActionAnimationList
      // array instantiated in player.cc
      // The first five are selected in the move state based on velocity
      RootAnim,
      RunForwardAnim,
      BackBackwardAnim,
      SideLeftAnim,

      // These are set explicitly based on player actions
      FallAnim,
      JumpAnim,
      StandJumpAnim,
      LandAnim,

      //
      NumMoveActionAnims = SideLeftAnim + 1,
      NumTableActionAnims = LandAnim + 1,
      NumExtraActionAnims = 512,
      NumActionAnims = NumTableActionAnims + NumExtraActionAnims,
      ActionAnimBits = 9,
      NullAnimation = (1 << ActionAnimBits) - 1
   };

   static ActionAnimationDef ActionAnimationList[NumTableActionAnims];
   ActionAnimation actionList[NumActionAnims];
   U32 actionCount;
   U32 lookAction;
   S32 spineNode[NumSpineNodes];
*/
   S32 pickupDelta;           ///< Base off of pcikupRadius
   F32 runSurfaceCos;         ///< Angle from vertical in cos(runSurfaceAngle)
   F32 jumpSurfaceCos;        ///< Angle from vertical in cos(jumpSurfaceAngle)

   enum Impacts {
      ImpactNone,
      ImpactNormal,
   };

/* Original TGE Code:
   enum Recoil {
      LightRecoil,
      MediumRecoil,
      HeavyRecoil,
      NumRecoilSequences
   };
   S32 recoilSequence[NumRecoilSequences];
*/

   /// @name Particles
   /// All of the data relating to environmental effects
   /// @{

/* Original TGE Code:
   ParticleEmitterData * footPuffEmitter;
   S32 footPuffID;
   S32 footPuffNumParts;
   F32 footPuffRadius;

   DecalData* decalData;
   S32 decalID;

   ParticleEmitterData * dustEmitter;
   S32 dustID;
*/

   SplashData* splash;
   S32 splashId;
   F32 splashVelocity;
   F32 splashAngle;
   F32 splashFreqMod;
   F32 splashVelEpsilon;
   F32 bubbleEmitTime;

   F32 medSplashSoundVel;
   F32 hardSplashSoundVel;
   F32 exitSplashSoundVel;
   F32 footSplashHeight;

   ParticleEmitterData* splashEmitterList[NUM_SPLASH_EMITTERS];
   S32 splashEmitterIDList[NUM_SPLASH_EMITTERS];
   /// @}

   //
   DECLARE_CONOBJECT(rpgPlayerData);
   rpgPlayerData();
   bool preload(bool server, char errorBuffer[256]);
   void getGroundInfo(TSShapeInstance*,TSThread*,ActionAnimation*);
   bool isTableSequence(S32 seq);
   bool isJumpAction(U32 action);

   static void initPersistFields();
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
};


//----------------------------------------------------------------------------

class rpgPlayer: public ShapeBase
{
   typedef ShapeBase Parent;
protected:
   /// Bit masks for different types of events
   enum MaskBits {
      ActionMask   = Parent::NextFreeMask << 0,
      MoveMask     = Parent::NextFreeMask << 1,
      ImpactMask   = Parent::NextFreeMask << 2,
      // AFX CODE BLOCK (player-look) <<
      LookOverrideMask = Parent::NextFreeMask << 3,
      RpgMask      = Parent::NextFreeMask << 4, // <-- MMO Kit
      NextFreeMask = Parent::NextFreeMask << 5
      /* ORIGINAL CODE
      NextFreeMask = Parent::NextFreeMask << 3
       */
      // AFX CODE BLOCK (player-look) >>
   };

   struct Range {
      Range(F32 _min,F32 _max) {
         min = _min;
         max = _max;
         delta = _max - _min;
      };
      F32 min,max;
      F32 delta;
   };

   ParticleEmitter *mSplashEmitter[rpgPlayerData::NUM_SPLASH_EMITTERS];
   F32 mBubbleEmitterTime;

   /// Client interpolation/warp data
   struct StateDelta {
      Move move;                    ///< Last move from server
      F32 dt;                       ///< Last interpolation time
      /// @name Interpolation data
     /// @{

      Point3F pos;
      Point3F rot;
      Point3F head;
      VectorF posVec;
      VectorF rotVec;
      VectorF headVec;
     /// @}

     /// @name Warp data
     /// @{

      S32 warpTicks;
      Point3F warpOffset;
      Point3F rotOffset;
     /// @}
   };
   StateDelta delta;                ///< Used for interpolation on the client.  @see StateDelta
   S32 mPredictionCount;            ///< Number of ticks to predict

   // Current pos, vel etc.
   Point3F mHead;                   ///< Head rotation, uses only x & z
   Point3F mRot;                    ///< Body rotation, uses only z
// Begin MMO Kit
/* Original TGE Code:
   VectorF mVelocity;
*/
   Point3F mAnchorPoint;            ///< Pos compression anchor
// End MMO Kit
   static F32 mGravity;             ///< Gravity
   S32 mImpactSound;

   S32 mMountPending;               ///< mMountPending suppresses tickDelay countdown so players will sit until
                                    ///< their mount, or another animation, comes through (or 13 seconds elapses).

   /// Main player state
   enum ActionState {
      NullState,
      MoveState,
      RecoverState,
      NumStateBits = 3
   };
   ActionState mState;              ///< What is the player doing? @see ActionState
   bool mFalling;                   ///< Falling in mid-air?
   S32  mJumpDelay;                 ///< Delay till next jump
   S32  mContactTimer;              ///< Ticks since last contact

   Point3F mJumpSurfaceNormal;      ///< Normal of the surface the player last jumped on
// Begin MMO Kit
   Point3F mContactNormal;          ///< Normal of the surface the player last jumped on
   bool mRunSurface;                ///< Can the player run on this surface?
// End MMO Kit

   U32 mJumpSurfaceLastContact;     ///< How long it's been since the player landed (ticks)
   F32  mWeaponBackFraction;        ///< Amount to slide the weapon back (if it's up against something)

   AUDIOHANDLE mMoveBubbleHandle;   ///< Audio handle for moving bubbles
   AUDIOHANDLE mWaterBreathHandle;  ///< Audio handle for underwater breath

   SimObjectPtr<ShapeBase> mControlObject; ///< Controlling object

/* Original TGE Code:
   /// @name Animation threads & data
   /// @{

   struct ActionAnimation {
      U32 action;
      TSThread* thread;
      S32 delayTicks;               // before picking another.
      bool forward;
      bool firstPerson;
      bool waitForEnd;
      bool holdAtEnd;
      bool animateOnServer;
      bool atEnd;
   } mActionAnimation;

   struct ArmAnimation {
      U32 action;
      TSThread* thread;
   } mArmAnimation;
   TSThread* mArmThread;

   TSThread* mHeadVThread;
   TSThread* mHeadHThread;
   TSThread* mRecoilThread;
   static Range mArmRange;
   static Range mHeadVRange;
   static Range mHeadHRange;
   /// @}
*/

   bool mInMissionArea;          ///< Are we in the mission area?
   //
   U32 mRecoverTicks;            ///< same as recoverTicks in the player datablock
   U32 mReversePending;

   bool inLiquid;                ///< Are we in liquid?
   //
   rpgPlayerData* mDataBlock;       ///< MMmmmmm...datablock...

   Point3F mLastPos;             ///< Holds the last position for physics updates
   Point3F mLastWaterPos;        ///< Same as mLastPos, but for water

   struct ContactInfo {
      bool     contacted, jump, run;
      VectorF  contactNormal;
      void clear()   {contacted=jump=run=false; contactNormal.set(1,1,1);}
      ContactInfo()  {clear();}
   } mContactInfo;

   struct Death {
      F32      lastPos;
      Point3F  posAdd;
      VectorF  rotate;
      VectorF  curNormal;
      F32      curSink;
      void     clear()           {dMemset(this, 0, sizeof(*this)); initFall();}
      VectorF  getPosAdd()       {VectorF ret(posAdd); posAdd.set(0,0,0); return ret;}
      bool     haveVelocity()    {return posAdd.x != 0 || posAdd.y != 0;}
      void     initFall()        {curNormal.set(0,0,1); curSink = 0;}
      Death()                    {clear();}
      MatrixF* fallToGround(F32 adjust, const Point3F& pos, F32 zrot, F32 boxRad);
   } mDeath;

   // New collision
  public:
   OrthoBoxConvex mConvex;
   Box3F          mWorkingQueryBox;

// Begin MMO Kit
   U32 mCollisionMoveMask;
   U32 mServerCollisionContactMask;
   U32 mClientCollisionContactMask;

   //rpg stuff

   F32  mMoveModifier;
   F32  mScaleModifier;

   void setMoveModifier(F32 mod);
   void setScaleModifier(F32 mod);
   //should probably be in shapebase, though I am setting this up for players now
   //and don't want to artificially limit scale limits on other things derivedfrom shapebase
   void setScale(const VectorF & scale);
// End MMO Kit

  protected:
   void setState(ActionState state, U32 ticks=0);
   void updateState();

   ///Update the movement
   void updateMove(const Move *move);
   ///Interpolate movement
   bool updatePos(const F32 travelTime = TickSec);
/* Original TGE Code:
   ///Update head animation
   void updateLookAnimation();
   ///Update other animations
   void updateAnimation(F32 dt);
   void updateAnimationTree(bool firstPerson);
*/
   bool step(Point3F *pos,F32 *maxStep,F32 time);
   ///See if the player is still in the mission area
   bool checkMissionArea(); // <-- MMO kit
/* Original TGE Code:
   void checkMissionArea();
*/

/* Original TGE Code:
   virtual bool setArmThread(U32 action);
   virtual void setActionThread(U32 action,bool forward,bool hold = false,bool wait = false,bool fsp = false, bool forceSet = false);
   virtual void updateActionThread();
   virtual void pickActionAnimation();
*/
   void onUnmount(ShapeBase* obj,S32 node);

   void setPosition(const Point3F& pos,const Point3F& viewRot);
   void setRenderPosition(const Point3F& pos,const Point3F& viewRot,F32 dt=-1);
   void findContact(bool* run,bool* jump,VectorF* contactNormal);
   //virtual void onImageRecoil(U32 imageSlot,ShapeBaseImageData::StateData::RecoilState); // <-- OneST8: compile fix
   virtual void updateDamageLevel();
   virtual void updateDamageState();
   /// Set which client is controlling this player
   void setControllingClient(GameConnection* client);

   void calcClassRenderData();
   /// Render the mounted image, usually the weapon
   void renderMountedImage(SceneState* state, ShapeImageRenderImage* image);
   /// Render the player
   void renderImage(SceneState *state, SceneRenderImage *image);
   /// Play a footstep sound
   void playFootstepSound(bool triggeredLeft, S32 sound);
   /// Play an impact sound
   void playImpactSound();

   /// Are we in the process of dying?
/* Original TGE Code:
   bool inDeathAnim();
*/
   F32  deathDelta(Point3F &delta);
   void updateDeathOffsets();
   bool inSittingAnim();

   /// @name Water
   /// @{

   void updateSplash();                             ///< Update the splash effect
   void updateFroth( F32 dt );                      ///< Update any froth
   void updateWaterSounds( F32 dt );                ///< Update water sounds
   bool pointInWater( Point3F &point );             ///< Tests to see if a point is in water
   void createSplash( Point3F &pos, F32 speed );    ///< Creates a splash
   bool collidingWithWater( Point3F &waterHeight ); ///< Are we collising with water?
   /// @}

public:
   DECLARE_CONOBJECT(rpgPlayer);

   rpgPlayer();
   ~rpgPlayer();
   static void consoleInit();

   /// @name Transforms
   /// Transforms are all in object space
   /// @{

   void setTransform(const MatrixF &mat);
   void getEyeTransform(MatrixF* mat);
   void getRenderEyeTransform(MatrixF* mat);
   void getCameraParameters(F32 *min, F32 *max, Point3F *offset, MatrixF *rot);
   void getMuzzleTransform(U32 imageSlot,MatrixF* mat);
   void getRenderMuzzleTransform(U32 imageSlot,MatrixF* mat);
   /// @}

   Point3F getVelocity() const;
   void setVelocity(const VectorF& vel);
   /// Apply an impulse at the given point, with magnitude/direction of vec
   void applyImpulse(const Point3F& pos,const VectorF& vec);
   /// Get the rotation of the player
   const Point3F& getRotation() { return mRot; }
   /// Get the rotation of the head of the player
   const Point3F& getHeadRotation() { return mHead; }
   void getDamageLocation(const Point3F& in_rPos, const char *&out_rpVert, const char *&out_rpQuad);

   bool  canJump();                                         ///< Can the player jump?
   bool  haveContact()     {return !mContactTimer;}         ///< Is it in contact with something
   S32 getContactTimer()   {return mContactTimer;} // <-- MMO Kit
   void  getMuzzlePointAI(U32 imageSlot, Point3F* point);
   /// duh
   float getMaxForwardVelocity() { return (mDataBlock != NULL ? mDataBlock->maxForwardSpeed : 0); }

   virtual bool    isDisplacable() const;
   virtual Point3F getMomentum() const;
   virtual void    setMomentum(const Point3F &momentum);
   virtual F32     getMass() const;
   virtual bool    displaceObject(const Point3F& displaceVector);
   virtual bool    getAIMove(Move*);

   bool checkDismountPosition(const MatrixF& oldPos, const MatrixF& newPos);  ///< Is it safe to dismount here?

   //
   bool onAdd();
   void onRemove();
   bool onNewDataBlock(GameBaseData* dptr);
   void onScaleChanged();
   Box3F mScaledBox;

   // Animation
   const char* getStateName();
/* Original TGE Code:
   bool setActionThread(const char* sequence,bool hold,bool wait,bool fsp = false);
   bool setArmThread(const char* sequence);
*/

   // Object control
   void setControlObject(ShapeBase *obj);
   ShapeBase* getControlObject();

   //
   void updateWorkingCollisionSet();
   void processTick(const Move *move);
   void interpolateTick(F32 delta);
   void advanceTime(F32 dt);
   bool castRay(const Point3F &start, const Point3F &end, RayInfo* info);
   bool buildPolyList(AbstractPolyList* polyList, const Box3F &box, const SphereF &sphere);
   void buildConvex(const Box3F& box, Convex* convex);
   bool isControlObject();

   void onCameraScopeQuery(NetConnection *cr, CameraScopeQuery *);
   void writePacketData(GameConnection *conn, BitStream *stream);
   void readPacketData (GameConnection *conn, BitStream *stream);
   U32  packUpdate  (NetConnection *conn, U32 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn,           BitStream *stream);

// Begin MMO Kit

public:

   //skinning

   ///
   enum SkinType {
      SkinHead,
      SkinArms,
      SkinLegs,
      SkinFeet,
      SkinHands,
      SkinBody,
      SkinSingle, //for monsters with one texture
      SkinMax
   };

   bool             mSkinsDirty; //server side only
   StringTableEntry mSkins[SkinMax];

   void setSkin(SkinType skType, StringTableEntry texture);

   bool canSwim();
   bool mSwimming;

   LightInfo            mLight;
//   virtual void registerLights(LightManager * lightManager, bool lightingScene);

   void setLightRadius(F32 r);

   //flying
   F32 mFlyingMod;
   void setFlyingMod(F32 mod);

   F32  mUnstickTimer;
   void stickHack(bool moves);

   bool mZombie;
   bool mSimZombie;
   bool mCanKite;
   bool mPlayerPet;

   F32  mAggroRange;

   static void initPersistFields();


   S32 mWanderGroup;
   S32 mUpdatePosCounter;
   MatrixF mWarpTransform;

   //hack
   bool mCheckPushed;

   S32 castSpell(const char* spellname, rpgPlayer* tgt=NULL);

   S8  mEncounterSetting;
   void setEncounterSetting(S8 encounterSetting);
   S16 mPrimaryLevel;
   S32 mAllianceLeader;
   void setAllianceLeader(S32 leader);
   StringTableEntry mGuildName;
   void setGuildName(StringTableEntry guild);

   bool allowHarmful(const rpgPlayer *opponent);
// End MMO Kit


   // AFX CODE BLOCK (misc) <<
private:
   static void  afx_consoleInit();
   void         afx_init();
   U32          afx_packUpdate(NetConnection*, U32 mask, BitStream*, U32 retMask);
   void         afx_unpackUpdate(NetConnection*, BitStream*);
   // AFX CODE BLOCK (misc) >>

   // AFX CODE BLOCK (obj-select) <<
private:
   static bool  sCorpsesHiddenFromRayCast;
   // AFX CODE BLOCK (obj-select) >>

   // AFX CODE BLOCK (anim-clip) <<
protected:
   enum { 
      ANIM_OVERRIDDEN     = BIT(0),
      BLOCK_USER_CONTROL  = BIT(1),
   };
   struct BlendThread
   {
      TSThread* thread;
      U32       tag;
   };
   Vector<BlendThread> blend_clips;
   static U32   unique_anim_tag_counter;
   U8           anim_clip_flags;
   U32          last_anim_tag;
   U32          last_anim_lock_tag;
public:
   void         restoreAnimation(U32 tag, bool is_death_anim);
   U32          getAnimationID(const char* name);
   F32          getAnimationDuration(U32 anim_id);
   U32          playAnimation(U32 anim_id, F32 pos, F32 rate, F32 trans, bool hold, bool wait);
   F32          getAnimationDuration(const char* name);
   U32          playAnimation(const char* name, F32 pos, F32 rate, F32 trans, bool hold, bool wait, bool is_death_anim);
   void         unlockAnimation(U32 tag, bool force=false);
   U32          lockAnimation();
   bool         isAnimationLocked() const { return ((anim_clip_flags & BLOCK_USER_CONTROL) != 0); }
   bool         isBlendAnimation(const char* name);
   // AFX CODE BLOCK (anim-clip) >>

   // AFX CODE BLOCK (player-look) <<
protected:
   bool         overrideLookAnimation;
   F32          armLookOverridePos;
   F32          headVLookOverridePos;
   F32          headHLookOverridePos;
public:
   void         setLookAnimationOverride(bool flag);
   // AFX CODE BLOCK (player-look) >>
};


#endif
