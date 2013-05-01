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

#include "rpg/game/rpgPlayer.h"

#include "platform/platform.h"
#include "platform/profiler.h"
#include "dgl/dgl.h"
#include "dgl/materialPropertyMap.h"
#include "math/mMath.h"
#include "core/stringTable.h"
#include "core/bitStream.h"
#include "core/dnet.h"
#include "core/resManager.h"
#include "console/simBase.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "collision/extrudedPolyList.h"
#include "collision/clippedPolyList.h"
#include "collision/earlyOutPolyList.h"
#include "sim/decalManager.h"
#include "ts/tsShapeInstance.h"
#include "audio/audioDataBlock.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/detailManager.h"
#include "terrain/terrData.h"
#include "terrain/terrRender.h"
#include "terrain/waterBlock.h"
#include "game/game.h"
#include "game/moveManager.h"
#include "game/gameConnection.h"
#include "game/trigger.h"
#include "game/physicalZone.h"
#include "game/item.h"
#include "game/shadow.h"
#include "game/missionArea.h"
#include "game/fx/particleEngine.h"
#include "game/fx/splash.h"
#include "game/fx/cameraFXMgr.h"

// Begin MMO Kit
#include "interior/interior.h"
#include "afx/afxMagicSpell.h"
// End MMO Kit

//----------------------------------------------------------------------------
// Amount we try to stay out of walls by...
static F32 sWeaponPushBack = 0.03f;

// Amount of time if takes to transition to a new action sequence.
/* Original TGE Code:
static F32 sAnimationTransitionTime = 0.25f;
static bool sUseAnimationTransitions = true;
*/
static F32 sLandReverseScale = 0.25f;
static F32 sStandingJumpSpeed = 2.0f;
static F32 sJumpingThreshold = 4.0f;
static F32 sSlowStandThreshSquared = 1.69f;
static S32 sRenderMyPlayer = true;
static S32 sRenderMyItems = true;

// Chooses new action animations every n ticks.
static const F32 sNewAnimationTickTime = 4.0f;
static const F32 sMountPendingTickWait = (13.0f * 32.0f);

// Number of ticks before we pick non-contact animations
static const S32 sContactTickTime = 30;

// Downward velocity at which we consider the player falling
static const F32 sFallingThreshold = -10.0f;

// Movement constants
static F32 sVerticalStepDot = 0.173f;   // 80
static F32 sMinFaceDistance = 0.01f;
static F32 sTractionDistance = 0.03f;
static F32 sNormalElasticity = 0.01f;
static U32 sMoveRetryCount = 5;

// Client prediction
// Begin MMO Kit
/* Original TGE Code:
static F32 sMinWarpTicks = 0.5f;        // Fraction of tick at which instant warp occures
static S32 sMaxWarpTicks = 3;          // Max warp duration in ticks
static S32 sMaxPredictionTicks = 30;   // Number of ticks to predict
*/
static F32 sMinWarpTicks = 0.1f;        // Fraction of tick at which instant warp occures
static S32 sMaxWarpTicks = 8;          // Max warp duration in ticks
static S32 sMaxPredictionTicks = 32;   // Number of ticks to predict
// End MMO Kit

// Anchor point compression
const F32 sAnchorMaxDistance = 32.0f;

/* Original TGE Code:
//
static U32 sCollisionMoveMask = (TerrainObjectType      | InteriorObjectType   |
                                 WaterObjectType        | PlayerObjectType     |
                                 StaticShapeObjectType  | VehicleObjectType    |
                                 PhysicalZoneObjectType | StaticTSObjectType);

static U32 sServerCollisionContactMask = (sCollisionMoveMask |
                                          (ItemObjectType    |
                                           TriggerObjectType |
                                           CorpseObjectType));

static U32 sClientCollisionContactMask = sCollisionMoveMask | PhysicalZoneObjectType;
*/

// Begin MMO Kit
//extern MissionArea* gClientMissionArea;
extern bool gGamePaused;
// End MMO Kit

enum PlayerConstants {
   JumpSkipContactsMax = 8
};

//----------------------------------------------------------------------------
// rpgPlayer shape animation sequences:

/* Original TGE Code:
// look     Used to contol the upper body arm motion.  Must animate
//          vertically +-80 deg.
rpgPlayer::Range rpgPlayer::mArmRange(mDegToRad(-80.0f),mDegToRad(+80.0f));

// head     Used to control the direction the head is looking.  Must
//          animated vertically +-80 deg .
rpgPlayer::Range rpgPlayer::mHeadVRange(mDegToRad(-80.0f),mDegToRad(+80.0f));

// Action Animations:
rpgPlayerData::ActionAnimationDef rpgPlayerData::ActionAnimationList[NumTableActionAnims] =
{
   // *** WARNING ***
   // This array is indexed useing the enum values defined in player.h

   // Root is the default animation
   { "root" },       // RootAnim,

   // These are selected in the move state based on velocity
   { "run",  { 0.0f, 1.0f, 0.0f } },       // RunForwardAnim,
   { "back", { 0.0f, -1.0f, 0.0f } },       // BackBackwardAnim
   { "side", { -1.0f, 0.0f, 0.0f } },       // SideLeftAnim,

   // These are set explicitly based on player actions
   { "fall" },       // FallAnim
   { "jump" },       // JumpAnim
   { "standjump" },  // StandJumpAnim
   { "land" },       // LandAnim
};
*/

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(rpgPlayerData);

rpgPlayerData::rpgPlayerData()
{
   shadowEnable = true;
   shadowCanMove = true;
   shadowCanAnimate = true;

// Begin MMO Kit
   radius = 3;
   scale = 1;
// End MMO Kit

   renderFirstPerson = true;
   pickupRadius = 0.0f;
   minLookAngle = -1.4f;
   maxLookAngle = 1.4f;
   maxFreelookAngle = 3.0f;
   maxTimeScale = 1.5f;

   mass = 9.0f;
   maxEnergy =  60.0f;

   runForce = 40.0f * 9.0f;
   runEnergyDrain = 0.0f;
   minRunEnergy = 0.0f;
   maxForwardSpeed = 10.0f;
   maxBackwardSpeed = 10.0f;
   maxSideSpeed = 10.0f;
   maxUnderwaterForwardSpeed = 10.0f;
   maxUnderwaterBackwardSpeed = 10.0f;
   maxUnderwaterSideSpeed = 10.0f;

// Begin MMO Kit
/* Original TGE Code:
   maxStepHeight = 1.0f;
   runSurfaceAngle = 80.0f;
*/
   maxStepHeight = 0.5f;
   runSurfaceAngle = 60.0f;
// End MMO Kit

   recoverDelay = 30;
   recoverRunForceScale = 1.0f;

   jumpForce = 75.0f;
   jumpEnergyDrain = 0.0f;
   minJumpEnergy = 0.0f;
   jumpSurfaceAngle = 78.0f;
   jumpDelay = 30;
   minJumpSpeed = 500.0f;
   maxJumpSpeed = 2.0f * minJumpSpeed;

   horizMaxSpeed = 80.0f;
   horizResistSpeed = 38.0f;
   horizResistFactor = 1.0f;

   upMaxSpeed = 80.0f;
   upResistSpeed = 38.0f;
   upResistFactor = 1.0f;

   minImpactSpeed = 25.0f;

/* Original TGE Code:
   decalData      = NULL;
   decalID        = 0;
   decalOffset      = 0.0f;

   lookAction = 0;
*/

   // size of bounding box
   boxSize.set(1.0f, 1.0f, 2.3f );

/* Original TGE Code:
   // location of head, torso, legs
   boxHeadPercentage = 0.85f;
   boxTorsoPercentage = 0.55f;

   // damage locations
   boxHeadLeftPercentage  = 0;
   boxHeadRightPercentage = 1;
   boxHeadBackPercentage  = 0;
   boxHeadFrontPercentage = 1;
*/

   for (S32 i = 0; i < MaxSounds; i++)
      sound[i] = NULL;

/* Original TGE Code:
   footPuffEmitter = NULL;
   footPuffID = 0;
   footPuffNumParts = 15;
   footPuffRadius = .25f;

   dustEmitter = NULL;
   dustID = 0;
*/

   splash = NULL;
   splashId = 0;
   splashVelocity = 1.0f;
   splashAngle = 45.0f;
   splashFreqMod = 300.0f;
   splashVelEpsilon = 0.25f;
   bubbleEmitTime = 0.4f;

   medSplashSoundVel = 2.0f;
   hardSplashSoundVel = 3.0f;
   exitSplashSoundVel = 2.0f;
   footSplashHeight = 0.1f;

   dMemset( splashEmitterList, 0, sizeof( splashEmitterList ) );
   dMemset( splashEmitterIDList, 0, sizeof( splashEmitterIDList ) );

   genericShadowLevel = Player_GenericShadowLevel;
   noShadowLevel = Player_NoShadowLevel;

   groundImpactMinSpeed = 10.0f;
   groundImpactShakeFreq.set( 10.0f, 10.0f, 10.0f );
   groundImpactShakeAmp.set( 20.0f, 20.0f, 20.0f );
   groundImpactShakeDuration = 1.0f;
   groundImpactShakeFalloff = 10.0f;
}

bool rpgPlayerData::preload(bool server, char errorBuffer[256])
{
   if(!Parent::preload(server, errorBuffer))
      return false;

   // Resolve objects transmitted from server
   if (!server) {
      for (S32 i = 0; i < MaxSounds; i++)
         if (sound[i])
            Sim::findObject(SimObjectId(sound[i]),sound[i]);
   }

   //
   runSurfaceCos = mCos(mDegToRad(runSurfaceAngle));
   jumpSurfaceCos = mCos(mDegToRad(jumpSurfaceAngle));
   if (minJumpEnergy < jumpEnergyDrain)
      minJumpEnergy = jumpEnergyDrain;

   // Validate some of the data
   if (recoverDelay > (1 << RecoverDelayBits) - 1) {
      recoverDelay = (1 << RecoverDelayBits) - 1;
      Con::printf("rpgPlayerData:: Recover delay exceeds range (0-%d)",recoverDelay);
   }
   if (jumpDelay > (1 << JumpDelayBits) - 1) {
      jumpDelay = (1 << JumpDelayBits) - 1;
      Con::printf("rpgPlayerData:: Jump delay exceeds range (0-%d)",jumpDelay);
   }

   // Go ahead a pre-load the player shape
   TSShapeInstance* si = new TSShapeInstance(shape, false);
/* Original TGE Code:
   TSThread* thread = si->addThread();

   // Extract ground transform velocity from animations
   // Get the named ones first so they can be indexed directly.
   ActionAnimation *dp = &actionList[0];
   for (int i = 0; i < NumTableActionAnims; i++,dp++)
   {
      ActionAnimationDef *sp = &ActionAnimationList[i];
      dp->name          = sp->name;
      dp->dir.set(sp->dir.x,sp->dir.y,sp->dir.z);
      dp->sequence      = shape->findSequence(sp->name);
      dp->velocityScale = true;
      dp->death         = false;
      if (dp->sequence != -1)
         getGroundInfo(si,thread,dp);

      AssertWarn(dp->sequence != -1, avar("rpgPlayerData::preload - Unable to find named animation sequence '%s'!", sp->name));
   }
   for (int b = 0; b < shape->sequences.size(); b++)
   {
      if (!isTableSequence(b))
      {
         dp->sequence      = b;
         dp->name          = shape->getName(shape->sequences[b].nameIndex);
         dp->velocityScale = false;
         getGroundInfo(si,thread,dp++);
      }
   }
   actionCount = dp - actionList;
   AssertFatal(actionCount <= NumActionAnims, "Too many action animations!");
   delete si;

   // Resolve lookAction index
   dp = &actionList[0];
   const char *lookName = StringTable->insert("look");
   for (int c = 0; c < actionCount; c++,dp++)
      if (dp->name == lookName)
         lookAction = c;

   // Resolve spine
   spineNode[0] = shape->findNode("Bip01 Pelvis");
   spineNode[1] = shape->findNode("Bip01 Spine");
   spineNode[2] = shape->findNode("Bip01 Spine1");
   spineNode[3] = shape->findNode("Bip01 Spine2");
   spineNode[4] = shape->findNode("Bip01 Neck");
   spineNode[5] = shape->findNode("Bip01 Head");

   // Recoil animations
   recoilSequence[0] = shape->findSequence("light_recoil");
   recoilSequence[1] = shape->findSequence("medium_recoil");
   recoilSequence[2] = shape->findSequence("heavy_recoil");
*/

   // Lookup shadow node (shadow center moves in synch with this node)
// Begin MMO Kit
/* Original TGE Code:
   shadowNode = spineNode[0];
*/
   shadowNode = 0;
// End MMO Kit

   // Convert pickupRadius to a delta of boundingBox
   F32 dr = (boxSize.x > boxSize.y)? boxSize.x: boxSize.y;
   if (pickupRadius < dr)
      pickupRadius = dr;
   else
      if (pickupRadius > 2.0f * dr)
         pickupRadius = 2.0f * dr;
   pickupDelta = (S32)(pickupRadius - dr);

   // Validate jump speed
   if (maxJumpSpeed <= minJumpSpeed)
      maxJumpSpeed = minJumpSpeed + 0.1f;

   // Load up all the emitters
/* Original TGE Code:
   if (!footPuffEmitter && footPuffID != 0)
      if (!Sim::findObject(footPuffID, footPuffEmitter))
         Con::errorf(ConsoleLogEntry::General, "rpgPlayerData::preload - Invalid packet, bad datablockId(footPuffEmitter): 0x%x", footPuffID);

   if (!decalData && decalID != 0 )
      if (!Sim::findObject(decalID, decalData))
         Con::errorf(ConsoleLogEntry::General, "rpgPlayerData::preload - Invalid packet, bad datablockId(decalData): 0x%x", decalID);

   if (!dustEmitter && dustID != 0 )
      if (!Sim::findObject(dustID, dustEmitter))
         Con::errorf(ConsoleLogEntry::General, "rpgPlayerData::preload - Invalid packet, bad datablockId(dustEmitter): 0x%x", dustID);
*/

   for (int i=0; i<NUM_SPLASH_EMITTERS; i++)
      if( !splashEmitterList[i] && splashEmitterIDList[i] != 0 )
         if( Sim::findObject( splashEmitterIDList[i], splashEmitterList[i] ) == false)
            Con::errorf(ConsoleLogEntry::General, "rpgPlayerData::onAdd - Invalid packet, bad datablockId(particle emitter): 0x%x", splashEmitterIDList[i]);

   return true;
}

void rpgPlayerData::getGroundInfo(TSShapeInstance* si, TSThread* thread,ActionAnimation *dp)
{
   dp->death = !dStrnicmp(dp->name, "death", 5);
   if (dp->death)
   {
      // Death animations use roll frame-to-frame changes in ground transform into position
      dp->speed = 0.0f;
      dp->dir.set(0.0f, 0.0f, 0.0f);
   }
   else
   {
      VectorF save = dp->dir;
      si->setSequence(thread,dp->sequence,0);
      si->animate();
      si->advanceTime(1);
      si->animateGround();
      si->getGroundTransform().getColumn(3,&dp->dir);
      if ((dp->speed = dp->dir.len()) < 0.01f)
      {
         // No ground displacement... In this case we'll use the
         // default table entry, if there is one.
         if (save.len() > 0.01f)
         {
            dp->dir = save;
            dp->speed = 1.0f;
            dp->velocityScale = false;
         }
         else
            dp->speed = 0.0f;
      }
      else
         dp->dir *= 1.0f / dp->speed;
   }
}

/* Original TGE Code:
bool rpgPlayerData::isTableSequence(S32 seq)
{
   // The sequences from the table must already have
   // been loaded for this to work.
   for (int i = 0; i < NumTableActionAnims; i++)
      if (actionList[i].sequence == seq)
         return true;
   return false;
}

bool rpgPlayerData::isJumpAction(U32 action)
{
   return (action == JumpAnim || action == StandJumpAnim);
}
*/

void rpgPlayerData::initPersistFields()
{
   Parent::initPersistFields();

   addField("renderFirstPerson", TypeBool, Offset(renderFirstPerson, rpgPlayerData));
   addField("pickupRadius", TypeF32, Offset(pickupRadius, rpgPlayerData));

// Begin MMO Kit
   addField("radius", TypeF32, Offset(radius, rpgPlayerData));
   addField("scale", TypeF32, Offset(scale, rpgPlayerData));
// End MMO Kit

   addField("minLookAngle", TypeF32, Offset(minLookAngle, rpgPlayerData));
   addField("maxLookAngle", TypeF32, Offset(maxLookAngle, rpgPlayerData));
   addField("maxFreelookAngle", TypeF32, Offset(maxFreelookAngle, rpgPlayerData));

   addField("maxTimeScale", TypeF32, Offset(maxTimeScale, rpgPlayerData));

   addField("maxStepHeight", TypeF32, Offset(maxStepHeight, rpgPlayerData));
   addField("runForce", TypeF32, Offset(runForce, rpgPlayerData));
   addField("runEnergyDrain", TypeF32, Offset(runEnergyDrain, rpgPlayerData));
   addField("minRunEnergy", TypeF32, Offset(minRunEnergy, rpgPlayerData));
   addField("maxForwardSpeed", TypeF32, Offset(maxForwardSpeed, rpgPlayerData));
   addField("maxBackwardSpeed", TypeF32, Offset(maxBackwardSpeed, rpgPlayerData));
   addField("maxSideSpeed", TypeF32, Offset(maxSideSpeed, rpgPlayerData));
   addField("maxUnderwaterForwardSpeed", TypeF32, Offset(maxUnderwaterForwardSpeed, rpgPlayerData));
   addField("maxUnderwaterBackwardSpeed", TypeF32, Offset(maxUnderwaterBackwardSpeed, rpgPlayerData));
   addField("maxUnderwaterSideSpeed", TypeF32, Offset(maxUnderwaterSideSpeed, rpgPlayerData));
   addField("runSurfaceAngle", TypeF32, Offset(runSurfaceAngle, rpgPlayerData));
   addField("minImpactSpeed", TypeF32, Offset(minImpactSpeed, rpgPlayerData));

   addField("recoverDelay", TypeS32, Offset(recoverDelay, rpgPlayerData));
   addField("recoverRunForceScale", TypeF32, Offset(recoverRunForceScale, rpgPlayerData));

   addField("jumpForce", TypeF32, Offset(jumpForce, rpgPlayerData));
   addField("jumpEnergyDrain", TypeF32, Offset(jumpEnergyDrain, rpgPlayerData));
   addField("minJumpEnergy", TypeF32, Offset(minJumpEnergy, rpgPlayerData));
   addField("minJumpSpeed", TypeF32, Offset(minJumpSpeed, rpgPlayerData));
   addField("maxJumpSpeed", TypeF32, Offset(maxJumpSpeed, rpgPlayerData));
   addField("jumpSurfaceAngle", TypeF32, Offset(jumpSurfaceAngle, rpgPlayerData));
   addField("jumpDelay", TypeS32, Offset(jumpDelay, rpgPlayerData));

/* Original TGE Code:
   addField("boundingBox", TypePoint3F, Offset(boxSize, rpgPlayerData));
   addField("boxHeadPercentage", TypeF32, Offset(boxHeadPercentage, rpgPlayerData));
   addField("boxTorsoPercentage", TypeF32, Offset(boxTorsoPercentage, rpgPlayerData));
   addField("boxHeadLeftPercentage", TypeS32, Offset(boxHeadLeftPercentage, rpgPlayerData));
   addField("boxHeadRightPercentage", TypeS32, Offset(boxHeadRightPercentage, rpgPlayerData));
   addField("boxHeadBackPercentage", TypeS32, Offset(boxHeadBackPercentage, rpgPlayerData));
   addField("boxHeadFrontPercentage", TypeS32, Offset(boxHeadFrontPercentage, rpgPlayerData));
*/

   addField("horizMaxSpeed", TypeF32, Offset(horizMaxSpeed, rpgPlayerData));
   addField("horizResistSpeed", TypeF32, Offset(horizResistSpeed, rpgPlayerData));
   addField("horizResistFactor", TypeF32, Offset(horizResistFactor, rpgPlayerData));

   addField("upMaxSpeed", TypeF32, Offset(upMaxSpeed, rpgPlayerData));
   addField("upResistSpeed", TypeF32, Offset(upResistSpeed, rpgPlayerData));
   addField("upResistFactor", TypeF32, Offset(upResistFactor, rpgPlayerData));

/* Original TGE Code:
   addField("decalData",         TypeDecalDataPtr, Offset(decalData, rpgPlayerData));
   addField("decalOffset",TypeF32, Offset(decalOffset, rpgPlayerData));

   addField("footPuffEmitter",   TypeParticleEmitterDataPtr,   Offset(footPuffEmitter,    rpgPlayerData));
   addField("footPuffNumParts",  TypeS32,                      Offset(footPuffNumParts,   rpgPlayerData));
   addField("footPuffRadius",    TypeF32,                      Offset(footPuffRadius,     rpgPlayerData));
   addField("dustEmitter",       TypeParticleEmitterDataPtr,   Offset(dustEmitter,        rpgPlayerData));
*/

   addField("FootSoftSound",       TypeAudioProfilePtr, Offset(sound[FootSoft],          rpgPlayerData));
   addField("FootHardSound",       TypeAudioProfilePtr, Offset(sound[FootHard],          rpgPlayerData));
   addField("FootMetalSound",      TypeAudioProfilePtr, Offset(sound[FootMetal],         rpgPlayerData));
   addField("FootSnowSound",       TypeAudioProfilePtr, Offset(sound[FootSnow],          rpgPlayerData));
   addField("FootShallowSound",    TypeAudioProfilePtr, Offset(sound[FootShallowSplash], rpgPlayerData));
   addField("FootWadingSound",     TypeAudioProfilePtr, Offset(sound[FootWading],        rpgPlayerData));
   addField("FootUnderwaterSound", TypeAudioProfilePtr, Offset(sound[FootUnderWater],    rpgPlayerData));
   addField("FootBubblesSound",    TypeAudioProfilePtr, Offset(sound[FootBubbles],       rpgPlayerData));
   addField("movingBubblesSound",   TypeAudioProfilePtr, Offset(sound[MoveBubbles],        rpgPlayerData));
   addField("waterBreathSound",     TypeAudioProfilePtr, Offset(sound[WaterBreath],        rpgPlayerData));

   addField("impactSoftSound",   TypeAudioProfilePtr, Offset(sound[ImpactSoft],  rpgPlayerData));
   addField("impactHardSound",   TypeAudioProfilePtr, Offset(sound[ImpactHard],  rpgPlayerData));
   addField("impactMetalSound",  TypeAudioProfilePtr, Offset(sound[ImpactMetal], rpgPlayerData));
   addField("impactSnowSound",   TypeAudioProfilePtr, Offset(sound[ImpactSnow],  rpgPlayerData));

   addField("mediumSplashSoundVelocity", TypeF32,     Offset(medSplashSoundVel,  rpgPlayerData));
   addField("hardSplashSoundVelocity",   TypeF32,     Offset(hardSplashSoundVel,  rpgPlayerData));
   addField("exitSplashSoundVelocity",   TypeF32,     Offset(exitSplashSoundVel,  rpgPlayerData));

   addField("impactWaterEasy",   TypeAudioProfilePtr, Offset(sound[ImpactWaterEasy],   rpgPlayerData));
   addField("impactWaterMedium", TypeAudioProfilePtr, Offset(sound[ImpactWaterMedium], rpgPlayerData));
   addField("impactWaterHard",   TypeAudioProfilePtr, Offset(sound[ImpactWaterHard],   rpgPlayerData));
   addField("exitingWater",      TypeAudioProfilePtr, Offset(sound[ExitWater],         rpgPlayerData));

   addField("splash",         TypeSplashDataPtr,      Offset(splash,          rpgPlayerData));
   addField("splashVelocity", TypeF32,                Offset(splashVelocity,  rpgPlayerData));
   addField("splashAngle",    TypeF32,                Offset(splashAngle,     rpgPlayerData));
   addField("splashFreqMod",  TypeF32,                Offset(splashFreqMod,   rpgPlayerData));
   addField("splashVelEpsilon", TypeF32,              Offset(splashVelEpsilon, rpgPlayerData));
   addField("bubbleEmitTime", TypeF32,                Offset(bubbleEmitTime,  rpgPlayerData));
   addField("splashEmitter",  TypeParticleEmitterDataPtr,   Offset(splashEmitterList,   rpgPlayerData), NUM_SPLASH_EMITTERS);
   addField("footstepSplashHeight",      TypeF32,     Offset(footSplashHeight,  rpgPlayerData));

   addField("groundImpactMinSpeed",       TypeF32,       Offset(groundImpactMinSpeed,        rpgPlayerData));
   addField("groundImpactShakeFreq",      TypePoint3F,   Offset(groundImpactShakeFreq,       rpgPlayerData));
   addField("groundImpactShakeAmp",       TypePoint3F,   Offset(groundImpactShakeAmp,        rpgPlayerData));
   addField("groundImpactShakeDuration",  TypeF32,       Offset(groundImpactShakeDuration,   rpgPlayerData));
   addField("groundImpactShakeFalloff",   TypeF32,       Offset(groundImpactShakeFalloff,    rpgPlayerData));
}

void rpgPlayerData::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->writeFlag(renderFirstPerson);

// Begin MMO Kit
   stream->write(radius);
   stream->write(scale);
// End MMO Kit

   stream->write(minLookAngle);
   stream->write(maxLookAngle);
   stream->write(maxFreelookAngle);
   stream->write(maxTimeScale);

   stream->write(maxStepHeight);

   stream->write(runForce);
   stream->write(runEnergyDrain);
   stream->write(minRunEnergy);
   stream->write(maxForwardSpeed);
   stream->write(maxBackwardSpeed);
   stream->write(maxSideSpeed);
   stream->write(maxUnderwaterForwardSpeed);
   stream->write(maxUnderwaterBackwardSpeed);
   stream->write(maxUnderwaterSideSpeed);
   stream->write(runSurfaceAngle);

   stream->write(recoverDelay);
   stream->write(recoverRunForceScale);

   stream->write(jumpForce);
   stream->write(jumpEnergyDrain);
   stream->write(minJumpEnergy);
   stream->write(minJumpSpeed);
   stream->write(maxJumpSpeed);
   stream->write(jumpSurfaceAngle);
   stream->writeInt(jumpDelay,JumpDelayBits);

   stream->write(horizMaxSpeed);
   stream->write(horizResistSpeed);
   stream->write(horizResistFactor);

   stream->write(upMaxSpeed);
   stream->write(upResistSpeed);
   stream->write(upResistFactor);

   stream->write(splashVelocity);
   stream->write(splashAngle);
   stream->write(splashFreqMod);
   stream->write(splashVelEpsilon);
   stream->write(bubbleEmitTime);

   stream->write(medSplashSoundVel);
   stream->write(hardSplashSoundVel);
   stream->write(exitSplashSoundVel);
   stream->write(footSplashHeight);
   // Don't need damage scale on the client
   stream->write(minImpactSpeed);

/* Original TGE Code:
   S32 i;
   for ( i = 0; i < MaxSounds; i++)
      if (stream->writeFlag(sound[i]))
         stream->writeRangedU32(packed? SimObjectId(sound[i]):
                                sound[i]->getId(),DataBlockObjectIdFirst,DataBlockObjectIdLast);
*/

   stream->write(boxSize.x);
   stream->write(boxSize.y);
   stream->write(boxSize.z);

/* Original TGE Code:
   if( stream->writeFlag( footPuffEmitter ) )
   {
      stream->writeRangedU32( footPuffEmitter->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
   }

   stream->write( footPuffNumParts );
   stream->write( footPuffRadius );

   if( stream->writeFlag( decalData ) )
   {
      stream->writeRangedU32( decalData->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
   }
   stream->write(decalOffset);

   if( stream->writeFlag( dustEmitter ) )
   {
      stream->writeRangedU32( dustEmitter->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
   }
*/

   if (stream->writeFlag( splash ))
   {
      stream->writeRangedU32(splash->getId(), DataBlockObjectIdFirst, DataBlockObjectIdLast);
   }

   U32 i;
   for( i=0; i<NUM_SPLASH_EMITTERS; i++ )
   {
      if( stream->writeFlag( splashEmitterList[i] != NULL ) )
      {
         stream->writeRangedU32( splashEmitterList[i]->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
      }
   }

   stream->write(groundImpactMinSpeed);
   stream->write(groundImpactShakeFreq.x);
   stream->write(groundImpactShakeFreq.y);
   stream->write(groundImpactShakeFreq.z);
   stream->write(groundImpactShakeAmp.x);
   stream->write(groundImpactShakeAmp.y);
   stream->write(groundImpactShakeAmp.z);
   stream->write(groundImpactShakeDuration);
   stream->write(groundImpactShakeFalloff);
}

void rpgPlayerData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   renderFirstPerson = stream->readFlag();

// Begin MMO Kit
   stream->read(&radius);
   stream->read(&scale);
// End MMO Kit

   stream->read(&minLookAngle);
   stream->read(&maxLookAngle);
   stream->read(&maxFreelookAngle);
   stream->read(&maxTimeScale);

   stream->read(&maxStepHeight);

   stream->read(&runForce);
   stream->read(&runEnergyDrain);
   stream->read(&minRunEnergy);
   stream->read(&maxForwardSpeed);
   stream->read(&maxBackwardSpeed);
   stream->read(&maxSideSpeed);
   stream->read(&maxUnderwaterForwardSpeed);
   stream->read(&maxUnderwaterBackwardSpeed);
   stream->read(&maxUnderwaterSideSpeed);
   stream->read(&runSurfaceAngle);

   stream->read(&recoverDelay);
   stream->read(&recoverRunForceScale);

   stream->read(&jumpForce);
   stream->read(&jumpEnergyDrain);
   stream->read(&minJumpEnergy);
   stream->read(&minJumpSpeed);
   stream->read(&maxJumpSpeed);
   stream->read(&jumpSurfaceAngle);
   jumpDelay = stream->readInt(JumpDelayBits);

   stream->read(&horizMaxSpeed);
   stream->read(&horizResistSpeed);
   stream->read(&horizResistFactor);

   stream->read(&upMaxSpeed);
   stream->read(&upResistSpeed);
   stream->read(&upResistFactor);

   stream->read(&splashVelocity);
   stream->read(&splashAngle);
   stream->read(&splashFreqMod);
   stream->read(&splashVelEpsilon);
   stream->read(&bubbleEmitTime);

   stream->read(&medSplashSoundVel);
   stream->read(&hardSplashSoundVel);
   stream->read(&exitSplashSoundVel);
   stream->read(&footSplashHeight);

   stream->read(&minImpactSpeed);

/* Original TGE Code:
   S32 i;
   for (i = 0; i < MaxSounds; i++) {
      sound[i] = NULL;
      if (stream->readFlag())
         sound[i] = (AudioProfile*)stream->readRangedU32(DataBlockObjectIdFirst,
                                                         DataBlockObjectIdLast);
   }
*/

   stream->read(&boxSize.x);
   stream->read(&boxSize.y);
   stream->read(&boxSize.z);

/* Original TGE Code:
   if( stream->readFlag() )
   {
      footPuffID = (S32) stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   }

   stream->read(&footPuffNumParts);
   stream->read(&footPuffRadius);

   if( stream->readFlag() )
   {
      decalID = (S32) stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   }
   stream->read(&decalOffset);

   if( stream->readFlag() )
   {
      dustID = (S32) stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   }
*/
   
   if (stream->readFlag())
   {
      splashId = stream->readRangedU32( DataBlockObjectIdFirst, DataBlockObjectIdLast );
   }

   U32 i;
   for( i=0; i<NUM_SPLASH_EMITTERS; i++ )
   {
      if( stream->readFlag() )
      {
         splashEmitterIDList[i] = stream->readRangedU32( DataBlockObjectIdFirst, DataBlockObjectIdLast );
      }
   }

   stream->read(&groundImpactMinSpeed);
   stream->read(&groundImpactShakeFreq.x);
   stream->read(&groundImpactShakeFreq.y);
   stream->read(&groundImpactShakeFreq.z);
   stream->read(&groundImpactShakeAmp.x);
   stream->read(&groundImpactShakeAmp.y);
   stream->read(&groundImpactShakeAmp.z);
   stream->read(&groundImpactShakeDuration);
   stream->read(&groundImpactShakeFalloff);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(rpgPlayer);
F32 rpgPlayer::mGravity = -20.0f;


//----------------------------------------------------------------------------

rpgPlayer::rpgPlayer()
{
#ifdef DARREN_MMO

   mNetFlags.set(Ghostable); // <-- MMO Kit
   mTypeMask |= PlayerObjectType;
   delta.pos.set(0.0f,0.0f,100.0f);
   delta.rot.set(0.0f,0.0f,0.0f);
   delta.head.set(0.0f,0.0f,0.0f);
   delta.rotOffset.set(0.0f,0.0f,0.0f);
   delta.warpOffset.set(0.0f,0.0f,0.0f);
   delta.posVec.set(0.0f,0.0f,0.0f);
   delta.rotVec.set(0.0f,0.0f,0.0f);
   delta.headVec.set(0.0f,0.0f,0.0f);
   delta.warpTicks = 0;
   delta.dt = 1.0f;
   delta.move = NullMove;
   mPredictionCount = sMaxPredictionTicks;
   mObjToWorld.setColumn(3,delta.pos);
   mRot = delta.rot;
   mHead = delta.head;
   mVelocity.set(0.0f, 0.0f, 0.0f);
   mDataBlock = 0;
/* Original TGE Code:
   mHeadHThread = mHeadVThread = mRecoilThread = 0;
   mArmAnimation.action = rpgPlayerData::NullAnimation;
   mArmAnimation.thread = 0;
   mActionAnimation.action = rpgPlayerData::NullAnimation;
   mActionAnimation.thread = 0;
   mActionAnimation.delayTicks = 0;
   mActionAnimation.forward = true;
   mActionAnimation.firstPerson = false;
   mActionAnimation.waitForEnd = false;
   mActionAnimation.holdAtEnd = false;
   mActionAnimation.animateOnServer = false;
   mActionAnimation.atEnd = false;
*/
   mState = MoveState;
   mFalling = false;
// Begin MMO Kit
/* Original TGE Code:
   mContactTimer = 0;
*/
   mContactTimer = 1;
// End MMO Kit
   mJumpDelay = 0;
   mJumpSurfaceLastContact = 0;
   mJumpSurfaceNormal.set(0.0f, 0.0f, 1.0f);
   mControlObject = 0;
   dMemset( mSplashEmitter, 0, sizeof( mSplashEmitter ) );

   mImpactSound = 0;
   mRecoverTicks = 0;
   mReversePending = 0;

   mLastPos.set( 0.0f, 0.0f, 0.0f );

   mMoveBubbleHandle = 0;
   mWaterBreathHandle = 0;
   inLiquid = false;

   mConvex.init(this);
   mWorkingQueryBox.min.set(-1e9f, -1e9f, -1e9f);
   mWorkingQueryBox.max.set(-1e9f, -1e9f, -1e9f);

   mWeaponBackFraction = 0.0f;

   mInMissionArea = true;

   mBubbleEmitterTime = 10.0f;
   mLastWaterPos.set( 0.0f, 0.0f, 0.0f );

   mMountPending = 0;

// Begin MMO Kit
   mCollisionMoveMask = (TerrainObjectType      | InteriorObjectType   |
                         //WaterObjectType        |
                         StaticShapeObjectType  | VehicleObjectType    |
                         PhysicalZoneObjectType | StaticTSObjectType);

   mServerCollisionContactMask = (mCollisionMoveMask |
                                  (ItemObjectType    |
                                   TriggerObjectType |
                                   CorpseObjectType));

   mClientCollisionContactMask = mCollisionMoveMask | PhysicalZoneObjectType;

   mMoveModifier = 1.0f;
   mScaleModifier = 1.0f;

   U32 i;
   for (i=0;i<SkinMax;i++)
      mSkins[i]=StringTable->insert("");

   mSwimming = false;
   mLight.mRadius = 0.f;

   mFlyingMod = 0.0;

   mUnstickTimer = 10.f;

   mZombie = false;
   mSimZombie = false;

   mWanderGroup = 0;
   mUpdatePosCounter = 0;

   mCanKite = false;

   mRunSurface = false;
   mPlayerPet = false;
   mAggroRange = 100.f;
   mRealm = 0;

   mEncounterSetting = RPG_ENCOUNTER_PVE;  // default to PVE
   mPrimaryLevel = 1;
   mAllianceLeader = 0;  // default to none
   mGuildName = StringTable->insert("");
// End MMO Kit

   // AFX CODE BLOCK (misc) <<
   afx_init();
   // AFX CODE BLOCK (misc) <<
#endif // DARREN_MMO
}

rpgPlayer::~rpgPlayer()
{
}

// Begin MMO Kit
void rpgPlayer::initPersistFields()
{
   Parent::initPersistFields();
#ifdef DARREN_MMO

   addField("role",              TypeString,    Offset(mRole, rpgPlayer)             );
   addField("wanderGroup",       TypeS32,       Offset(mWanderGroup, rpgPlayer)      );
   addField("canKite",           TypeBool,      Offset(mCanKite, rpgPlayer)          );
   addField("playerPet",         TypeBool,      Offset(mPlayerPet, rpgPlayer)        );
   addField("realm",             TypeS32,       Offset(mRealm, rpgPlayer)            );
   addField("aggroRange",        TypeF32,       Offset(mAggroRange, rpgPlayer)       );
   addField("waterCoverage",     TypeF32,       Offset(mWaterCoverage, rpgPlayer)    );
   addField("twoHanded",         TypeBool,      Offset(mTwoHanded, rpgPlayer)        );
   addField("encounterSetting",  TypeS32,       Offset(mEncounterSetting, rpgPlayer) );
   addField("primaryLevel",      TypeS32,       Offset(mPrimaryLevel, rpgPlayer)     );
   addField("allianceLeader",    TypeS32,       Offset(mAllianceLeader, rpgPlayer)   );
   addField("guildName",         TypeString,    Offset(mGuildName, rpgPlayer)        );
#endif //DARREN_MMO
}
// End MMO Kit

//----------------------------------------------------------------------------

bool rpgPlayer::onAdd()
{
/* Original TGE Code:
   ActionAnimation serverAnim = mActionAnimation;
*/
   if(!Parent::onAdd() || !mDataBlock)
      return false;

   mWorkingQueryBox.min.set(-1e9f, -1e9f, -1e9f);
   mWorkingQueryBox.max.set(-1e9f, -1e9f, -1e9f);

   addToScene();

   // Make sure any state and animation passed from the server
   // in the initial update is set correctly.
   ActionState state = mState;
   mState = NullState;
   setState(state);
/* Original TGE Code:
   if (serverAnim.action != rpgPlayerData::NullAnimation)
   {
      setActionThread(serverAnim.action, true, serverAnim.holdAtEnd, true, false, true);
      if (serverAnim.atEnd)
      {
         mShapeInstance->clearTransition(mActionAnimation.thread);
         mShapeInstance->setPos(mActionAnimation.thread,
                                mActionAnimation.forward? 1: 0);
         if (inDeathAnim())
            mDeath.lastPos = 1.0f;
      }

      // We have to leave them sitting for a while since mounts don't come through right
      // away (and sometimes not for a while).  Still going to let this time out because
      // I'm not sure if we're guaranteed another anim will come through and cancel.
      if (!isServerObject() && inSittingAnim())
         mMountPending = (S32) sMountPendingTickWait;
      else
         mMountPending = 0;
   }
   if (mArmAnimation.action != rpgPlayerData::NullAnimation)
      setArmThread(mArmAnimation.action);
*/

   //
   if (isServerObject())
   {
      scriptOnAdd();
   }
   else
   {
      U32 i;
      for( i=0; i<rpgPlayerData::NUM_SPLASH_EMITTERS; i++ )
      {
         mSplashEmitter[i] = new ParticleEmitter;
         mSplashEmitter[i]->onNewDataBlock( mDataBlock->splashEmitterList[i] );
         if( !mSplashEmitter[i]->registerObject() )
         {
            Con::warnf( ConsoleLogEntry::General, "Could not register dust emitter for class: %s", mDataBlock->getName() );
            delete mSplashEmitter[i];
            mSplashEmitter[i] = NULL;
         }
      }
      mLastWaterPos = getPosition();

      // clear out all camera effects
      gCamFXMgr.clear();
   }

   Sim::getLightSet()->addObject(this); // <-- MMO Kit

   return true;
}

void rpgPlayer::onRemove()
{

// Begin MMO Kit
   if (isServerObject())
      Con::evaluatef("Py::OnPlayerDeleted(%i);",mId);
// End MMO Kit

   setControlObject(0);
   scriptOnRemove();
   removeFromScene();

   U32 i;
   for( i=0; i<rpgPlayerData::NUM_SPLASH_EMITTERS; i++ )
   {
      if( mSplashEmitter[i] )
      {
         mSplashEmitter[i]->deleteWhenEmpty();
         mSplashEmitter[i] = NULL;
      }
   }

   mWorkingQueryBox.min.set(-1e9f, -1e9f, -1e9f);
   mWorkingQueryBox.max.set(-1e9f, -1e9f, -1e9f);

   Sim::getLightSet()->removeObject(this); // <-- MMO Kit

   Parent::onRemove();
}

void rpgPlayer::onScaleChanged()
{
/* Original TGE Code:
   const Point3F& scale = getScale();
   mScaledBox = mObjBox;
*/

// Begin MMO Kit
   Point3F scale(1.f,1.f,1.f);
   mScaledBox = mObjBox;

   //JMR, this solves many scale related problems, like stepping
   mScaledBox.max.z = 2.0f;
// End MMO Kit

   mScaledBox.min.convolve( scale );
   mScaledBox.max.convolve( scale );
}


//----------------------------------------------------------------------------

bool rpgPlayer::onNewDataBlock(GameBaseData* dptr)
{
   rpgPlayerData* prevData = mDataBlock;
   mDataBlock = dynamic_cast<rpgPlayerData*>(dptr);
/* Original TGE Code:
   if (!mDataBlock || !Parent::onNewDataBlock(dptr))
      return false;

   // Initialize arm thread, preserve arm sequence from last datablock.
   // Arm animation can be from last datablock, or sent from the server.
   U32 prevAction = mArmAnimation.action;
   mArmAnimation.action = rpgPlayerData::NullAnimation;
   if (mDataBlock->lookAction) {
      mArmAnimation.thread = mShapeInstance->addThread();
      mShapeInstance->setTimeScale(mArmAnimation.thread,0);
      if (prevData) {
         if (prevAction != prevData->lookAction && prevAction != rpgPlayerData::NullAnimation)
            setArmThread(prevData->actionList[prevAction].name);
         prevAction = rpgPlayerData::NullAnimation;
      }
      if (mArmAnimation.action == rpgPlayerData::NullAnimation) {
         mArmAnimation.action = (prevAction != rpgPlayerData::NullAnimation)?
            prevAction: mDataBlock->lookAction;
         mShapeInstance->setSequence(mArmAnimation.thread,
           mDataBlock->actionList[mArmAnimation.action].sequence,0);
      }
   }
   else
      mArmAnimation.thread = 0;

   // Initialize head look thread
   TSShape const* shape = mShapeInstance->getShape();
   S32 headSeq = shape->findSequence("head");
   if (headSeq != -1) {
      mHeadVThread = mShapeInstance->addThread();
      mShapeInstance->setSequence(mHeadVThread,headSeq,0);
      mShapeInstance->setTimeScale(mHeadVThread,0);
   }
   else
      mHeadVThread = 0;

   headSeq = shape->findSequence("headside");
   if (headSeq != -1) {
      mHeadHThread = mShapeInstance->addThread();
      mShapeInstance->setSequence(mHeadHThread,headSeq,0);
      mShapeInstance->setTimeScale(mHeadHThread,0);
   }
   else
      mHeadHThread = 0;

   // Recoil thread. The server player does not play this animation.
   mRecoilThread = 0;
   if (isGhost())
      for (U32 s = 0; s < rpgPlayerData::NumRecoilSequences; s++)
         if (mDataBlock->recoilSequence[s] != -1) {
            mRecoilThread = mShapeInstance->addThread();
            mShapeInstance->setSequence(mRecoilThread,mDataBlock->recoilSequence[s],0);
            mShapeInstance->setTimeScale(mRecoilThread,0);
         }

   // Initialize the primary thread, the actual sequence is
   // set later depending on player actions.
   mActionAnimation.action = rpgPlayerData::NullAnimation;
   mActionAnimation.thread = mShapeInstance->addThread();
   updateAnimationTree(!isGhost());

   mObjBox.max.x = mDataBlock->boxSize.x * 0.5f;
   mObjBox.max.y = mDataBlock->boxSize.y * 0.5f;
   mObjBox.max.z = mDataBlock->boxSize.z;
   mObjBox.min.x = -mObjBox.max.x;
   mObjBox.min.y = -mObjBox.max.y;
   mObjBox.min.z = 0.0f;
*/

// Begin MMO Kit
   //if (!mDataBlock)
   //   Sim::findObject("TempDummyPlayerData",mDataBlock);

   AssertISV(dptr,"Missing rpgPlayer dptr Datablock!");
   AssertISV(mDataBlock,"Missing rpgPlayer Datablock!");

   if (!mDataBlock || !Parent::onNewDataBlock(dptr))
      return false;

   Point3F scale = getScale();

   mObjBox.min= Point3F(-1,-1,0);////mDataBlock->shape->bounds; argh the bounds are off!
   mObjBox.max= Point3F(1,1,2.5);////mDataBlock->shape->bounds; argh the bounds are off!
   mObjBox.min*=scale;
   mObjBox.max*=scale;
// End MMO Kit

   // Setup the box for our convex object...
   mObjBox.getCenter(&mConvex.mCenter);
   mConvex.mSize.x = mObjBox.len_x() / 2.0f;
   mConvex.mSize.y = mObjBox.len_y() / 2.0f;
   mConvex.mSize.z = mObjBox.len_z() / 2.0f;

   // Initialize our scaled attributes as well
/* Original TGE Code:
   onScaleChanged();
*/

   scriptOnNewDataBlock();
   return true;
}


//----------------------------------------------------------------------------

void rpgPlayer::setControllingClient(GameConnection* client)
{
   Parent::setControllingClient(client);
   if (mControlObject)
      mControlObject->setControllingClient(client);
}

void rpgPlayer::setControlObject(ShapeBase* obj)
{
   if (mControlObject) {
      mControlObject->setControllingObject(0);
      mControlObject->setControllingClient(0);
   }
   if (obj == this || obj == 0)
      mControlObject = 0;
   else {
      if (ShapeBase* coo = obj->getControllingObject())
         coo->setControlObject(0);
      if (GameConnection* con = obj->getControllingClient())
         con->setControlObject(0);

      mControlObject = obj;
      mControlObject->setControllingObject(this);
      mControlObject->setControllingClient(getControllingClient());
   }
}

void rpgPlayer::onCameraScopeQuery(NetConnection *connection, CameraScopeQuery *query)
{
   // First, we are certainly in scope, and whatever we're riding is too...
   if(mControlObject.isNull() || mControlObject == mMount.object)
      Parent::onCameraScopeQuery(connection, query);
   else
   {
      connection->objectInScope(this);
      if (isMounted())
         connection->objectInScope(mMount.object);
      mControlObject->onCameraScopeQuery(connection, query);
   }
}

ShapeBase* rpgPlayer::getControlObject()
{
   return mControlObject;
}


//-----------------------------------------------------------------------------

void rpgPlayer::processTick(const Move* move)
{
   PROFILE_START(Player_ProcessTick);

// Begin MMO Kit
   bool grosshack = false;

   if (mTypeMask & AIObjectType && isServerObject() && mZombie)
   {
      SimGroup* pClientGroup = Sim::getClientGroup();

      bool gotone = false;

      for (SimGroup::iterator itr = pClientGroup->begin(); itr != pClientGroup->end(); itr++)
      {
         GameConnection* nc = dynamic_cast<GameConnection*>(*itr);

         //if (nc->getGhostIndex(this)==-1)
         //   continue;

         if (nc && nc->getControlObject())
         {
            ShapeBase* sb = nc->getControlObject();

            if (sb->getTypeMask() & AIObjectType)
               continue;

            F32 len = (sb->getPosition()-getPosition()).lenSquared();

            F32 radius = getWorldSphere().radius;
            if (radius < 1.5f)
               radius = 1.5f;

            F32 sdist = 150.f*(radius/2.25);
            if (sdist<mAggroRange)
               sdist = mAggroRange;

            if (len <= sdist*sdist+15.f)
            {
               gotone = true;
               break;
            }

         }
         else
            continue;

        }

      if (!gotone)
      {
         mSimZombie = true;
         grosshack=true;
         if (mWanderGroup<=0 || !pClientGroup->size())// ||!Con::getBoolVariable( "Server::Dedicated" )) //we still update move of wandering on dedicated
         {
            delta.warpTicks = 0;
            PROFILE_END();
            return;

         }
      }
   }

   if (!grosshack)
      mSimZombie = false;
// End MMO Kit

   // If we're not being controlled by a client, let the
   // AI sub-module get a chance at producing a move.
   Move aiMove;
// Begin MMO Kit
/* Original TGE Code:
   if (!move && isServerObject() && getAIMove(&aiMove))
      move = &aiMove;
*/
   if (!move && isServerObject() && getAIMove(&aiMove))
   {
      move = &aiMove;
   }
   else
   {
      if (move)
      {
#ifdef DARREN_MMO
		  Point3F pos(move->ax,move->ay,move->az);
         Point3F rot(0,0,move->arz);
         setPosition(pos,rot);
         mHead = Point3F(move->hx,0,move->hz);
#endif //DARREN_MMO
	  }
   }
// End MMO Kit

   // Manage the control object and filter moves for the player
   Move pMove,cMove;
   if (mControlObject) {
      if (!move)
         mControlObject->processTick(0);
      else {
         pMove = NullMove;
         cMove = *move;
         if (isMounted()) {
            // Filter Jump trigger if mounted
            pMove.trigger[2] = move->trigger[2];
            cMove.trigger[2] = false;
         }
         if (move->freeLook) {
            // Filter yaw/picth/roll when freelooking.
            pMove.yaw = move->yaw;
            pMove.pitch = move->pitch;
            pMove.roll = move->roll;
            pMove.freeLook = true;
            cMove.freeLook = false;
            cMove.yaw = cMove.pitch = cMove.roll = 0.0f;
         }
// Begin MMO Kit
         //mControlObject->processTick((mDamageState == Enabled)? &cMove: &NullMove);
         mControlObject->processTick( &cMove );
// End MMO Kit
         move = &pMove;
      }
   }

   Parent::processTick(move);
   // Warp to catch up to server
   if (delta.warpTicks > 0) {
      delta.warpTicks--;

      // Set new pos.
      getTransform().getColumn(3,&delta.pos);
      delta.pos += delta.warpOffset;
      delta.rot += delta.rotOffset;
      setPosition(delta.pos,delta.rot);
      setRenderPosition(delta.pos,delta.rot);
/* Original TGE Code:
      updateDeathOffsets();
      updateLookAnimation();
*/

      // Backstepping
      delta.posVec.x = -delta.warpOffset.x;
      delta.posVec.y = -delta.warpOffset.y;
      delta.posVec.z = -delta.warpOffset.z;
      delta.rotVec.x = -delta.rotOffset.x;
      delta.rotVec.y = -delta.rotOffset.y;
      delta.rotVec.z = -delta.rotOffset.z;
   }
   else {
      // If there is no move, the player is either an
      // unattached player on the server, or a player's
      // client ghost.
      if (!move) {
         if (isGhost()) {
            // If we haven't run out of prediction time,
            // predict using the last known move.
            if (mPredictionCount-- <= 0)
            {
               PROFILE_END();
               return;
            }
            move = &delta.move;
         }
         else
            move = &NullMove;
      }
/* Original TGE Code:
      if (!isGhost())
         updateAnimation(TickSec);
*/

      PROFILE_START(Player_PhysicsSection);
      if(isServerObject() || (didRenderLastRender() || getControllingClient()))
      {
// Begin MMO Kit
/* Original TGE Code:
         updateWorkingCollisionSet();

         updateState();
         updateMove(move);
         updateLookAnimation();
         updateDeathOffsets();
         updatePos();
*/
      mUpdatePosCounter++;
      if ((mTypeMask&AIObjectType)
           && !( move->x    || move->y   || move->z     ||
                 move->roll || move->yaw || move->pitch ||
                 mContactTimer )
           && mUpdatePosCounter < 50)
      {
#ifdef DARREN_MMO
         mVelocity.set(0,0,0);
#endif // DARREN_MMO
         PROFILE_END();
         PROFILE_END();
         return;
      }

      mUpdatePosCounter=0;

      //updateWorkingCollisionSet();
      updateState();
      updateMove(move);

      updatePos();
// End MMO Kit
      }
      PROFILE_END();

/* Original TGE Code:
      if (!isGhost())
      {
         // Animations are advanced based on frame rate on the
         // client and must be ticked on the server.
         updateActionThread();
         updateAnimationTree(true);
      }
*/
   }
   PROFILE_END();
}

void rpgPlayer::interpolateTick(F32 dt)
{
   if (mControlObject)
      mControlObject->interpolateTick(dt);

   // Client side interpolation
   Parent::interpolateTick(dt);
   if(dt != 0.0f)
   {
      Point3F pos = delta.pos + delta.posVec * dt;
      Point3F rot = delta.rot + delta.rotVec * dt;

      mHead = delta.head + delta.headVec * dt;
      setRenderPosition(pos,rot,dt);

      // apply camera effects - is this the best place? - bramage
      GameConnection* connection = GameConnection::getConnectionToServer();
      if( connection->isFirstPerson() )
      {
         ShapeBase *obj = connection->getControlObject();
         if( obj == this )
         {
            MatrixF curTrans = getRenderTransform();
            curTrans.mul( gCamFXMgr.getTrans() );
            Parent::setRenderTransform( curTrans );
         }
      }

   }
   else
   {
      mHead = delta.head;
      setRenderPosition(delta.pos, delta.rot, 0);
   }
/* Original TGE Code:
   updateLookAnimation();
*/
   delta.dt = dt;
}

void rpgPlayer::advanceTime(F32 dt)
{
   // Client side animations
   Parent::advanceTime(dt);
/* Original TGE Code:
   updateActionThread();
   updateAnimation(dt);
*/
   updateSplash();
   updateFroth(dt);
   updateWaterSounds(dt);

   mLastPos = getPosition();

   if (mImpactSound)
      playImpactSound();

   // update camera effects.  Definitely need to find better place for this - bramage
   if( isControlObject() )
   {
      if( mDamageState == Disabled || mDamageState == Destroyed )
      {
         // clear out all camera effects being applied to player if dead
         gCamFXMgr.clear();
      }
      gCamFXMgr.update( dt );
   }
}

bool rpgPlayer::getAIMove(Move* move)
{
   return false;
}


//----------------------------------------------------------------------------

void rpgPlayer::setState(ActionState state, U32 recoverTicks)
{
   if (state != mState) {
      // Skip initialization if there is no manager, the state
      // will get reset when the object is added to a manager.
      if (isProperlyAdded()) {
         switch (state) {
            case RecoverState: {
               mRecoverTicks = recoverTicks;
               mReversePending = U32(F32(mRecoverTicks) / sLandReverseScale);
/* Original TGE Code:
               setActionThread(rpgPlayerData::LandAnim, true, false, true, true);
*/
               break;
            }
         }
      }

      mState = state;
   }
}

void rpgPlayer::updateState()
{
#ifdef DARREN_MMO
   switch (mState)
   {
      case RecoverState:
         if (mRecoverTicks-- == 0)
         {
            if (mReversePending)
            {
               // this serves and counter, and direction state
               mRecoverTicks = mReversePending;
/* Original TGE Code:
               mActionAnimation.forward = false;

               S32 seq = mDataBlock->actionList[mActionAnimation.action].sequence;
               F32 pos = mShapeInstance->getPos(mActionAnimation.thread);

               mShapeInstance->setTimeScale(mActionAnimation.thread, -sLandReverseScale);
               mShapeInstance->transitionToSequence(mActionAnimation.thread,
                                                    seq, pos, sAnimationTransitionTime, true);
*/
               mReversePending = 0;
            }
            else
            {
               setState(MoveState);
            }
         }        // Stand back up slowly only if not moving much-
         else if (!mReversePending && mVelocity.lenSquared() > sSlowStandThreshSquared)
         {
/* Original TGE Code:
            mActionAnimation.waitForEnd = false;
*/
            setState(MoveState);
         }
         break;
   }
#endif // DARREN_MMO
}

const char* rpgPlayer::getStateName()
{
   if (mDamageState != Enabled)
      return "Dead";
   if (isMounted())
      return "Mounted";
   if (mState == RecoverState)
      return "Recover";
   return "Move";
}

void rpgPlayer::getDamageLocation(const Point3F& in_rPos, const char *&out_rpVert, const char *&out_rpQuad)
{
/* Original TGE Code:
   Point3F newPoint;
   mWorldToObj.mulP(in_rPos, &newPoint);

   F32 zHeight = mDataBlock->boxSize.z;
   F32 zTorso  = mDataBlock->boxTorsoPercentage;
   F32 zHead   = mDataBlock->boxHeadPercentage;

   zTorso *= zHeight;
   zHead  *= zHeight;

   if (newPoint.z <= zTorso)
      out_rpVert = "legs";
   else if (newPoint.z <= zHead)
      out_rpVert = "torso";
   else
      out_rpVert = "head";

   if(dStrcmp(out_rpVert, "head") != 0)
   {
      if (newPoint.y >= 0.0f)
      {
         if (newPoint.x <= 0.0f)
            out_rpQuad = "front_left";
         else
            out_rpQuad = "front_right";
      }
      else
      {
         if (newPoint.x <= 0.0f)
            out_rpQuad = "back_left";
         else
            out_rpQuad = "back_right";
      }
   }
   else
   {
      F32 backToFront = mDataBlock->boxSize.x;
      F32 leftToRight = mDataBlock->boxSize.y;

      F32 backPoint  = backToFront * (mDataBlock->boxHeadBackPercentage  - 0.5f);
      F32 frontPoint = backToFront * (mDataBlock->boxHeadFrontPercentage - 0.5f);
      F32 leftPoint  = leftToRight * (mDataBlock->boxHeadLeftPercentage  - 0.5f);
      F32 rightPoint = leftToRight * (mDataBlock->boxHeadRightPercentage - 0.5f);

      S32 index = 0;
      if (newPoint.y < backPoint)
         index += 0;
      else if (newPoint.y <= frontPoint)
         index += 3;
      else
         index += 6;

      if (newPoint.x < leftPoint)
         index += 0;
      else if (newPoint.x <= rightPoint)
         index += 1;
      else
         index += 2;

      switch (index)
      {
         case 0:
         out_rpQuad = "left_back";
         break;

         case 1: out_rpQuad = "middle_back"; break;
         case 2: out_rpQuad = "right_back"; break;
         case 3: out_rpQuad = "left_middle";   break;
         case 4: out_rpQuad = "middle_middle"; break;
         case 5: out_rpQuad = "right_middle"; break;
         case 6: out_rpQuad = "left_front";   break;
         case 7: out_rpQuad = "middle_front"; break;
         case 8: out_rpQuad = "right_front"; break;

         default:
            AssertFatal(0, "Bad non-tant index");
      };
   }
*/
// Begin MMO Kit
   out_rpVert = "torso";
   out_rpQuad = "back_left";
// End MMO Kit
}

//----------------------------------------------------------------------------

void rpgPlayer::updateMove(const Move* move)
{
#ifdef DARREN_MMO
	delta.move = *move;

// Begin MMO Kit
   F32 flyingMod = mFlyingMod;
   F32 moveMod = mMoveModifier;
   bool inStrat=false;

   mCheckPushed =false;
   if (!(mTypeMask&AIObjectType))
   {
      
   
      if (!isGhost())  {
         // Collisions are only queued on the server and can be
         // generated by either updateMove or updatePos
         //notifyCollision();

         // Do mission area callbacks on the server as well
         inStrat = checkMissionArea();
      }
      else
      {
         Point3F pos;

         
         
         if (gClientMissionArea)
         {
            inStrat = checkMissionArea();
            //getTransform().getColumn(3, &pos);
            //F32 strat = gClientMissionArea->getStratosphere();

            //if (strat != -1 && pos.z>=strat)
            //   inStrat = true;

         }

      }
   }
   bool outside =false;
   for (int i=0;i<getNumCurrZones();i++)
      if (getCurrZone(i)==0)
      {
         outside = true;
         break;
      }

   if (!outside || inStrat || mCheckPushed)
   {
      if (moveMod > 1.f)
         moveMod = 1.f;
      flyingMod = 0.f;
   }




   mCameraZoom+=move->zoom*4.0f;
   if (mCameraZoom<0.3)
   {
      if (!Con::getBoolVariable("$pref::firstPerson"))
         Con::setBoolVariable("$pref::firstPerson",true);
         
      mCameraZoom=0.3;
   }

   if (mCameraZoom>5.0)
      mCameraZoom=5.0;

   if (move->zoom > 0.f && Con::getBoolVariable("$pref::firstPerson"))
   {
      mCameraZoom=.3;
      Con::setBoolVariable("$pref::firstPerson",false);
   }
// End MMO Kit

      
   // Trigger images
   if (mDamageState == Enabled) {
      setImageTriggerState(0,move->trigger[0]);
      setImageTriggerState(1,move->trigger[1]);
   }

   // Update current orientation
   if (mDamageState == Enabled) {
      F32 prevZRot = mRot.z;
      delta.headVec = mHead;

      F32 p = move->pitch;
      if (p > M_PI_F)
         p -= M_2PI_F;
      mHead.x = mClampF(mHead.x + p,mDataBlock->minLookAngle,
                        mDataBlock->maxLookAngle);

      F32 y = move->yaw;
      if (y > M_PI_F)
         y -= M_2PI_F;

      GameConnection* con = getControllingClient();
      if (move->freeLook && ((isMounted() && getMountNode() == 0) || (con && !con->isFirstPerson())))
      {
         mHead.z = mClampF(mHead.z + y,
                           -mDataBlock->maxFreelookAngle,
                           mDataBlock->maxFreelookAngle);
      }
      else
      {
         mRot.z += y;
         // Rotate the head back to the front, center horizontal
         // as well if we're controlling another object.
         mHead.z *= 0.5f;
         if (mControlObject)
            mHead.x *= 0.5f;
      }

      // constrain the range of mRot.z

// Begin MMO Kit
      //this is a hack, the zone server was hanging inside updateMove
      //it might be here... we'll see
      if (mFabs(mRot.z)>M_2PI*10.f)
         mRot.z = 0.0f;
// End MMO Kit

      while (mRot.z < 0.0f)
         mRot.z += M_2PI_F;
      while (mRot.z > M_2PI_F)
         mRot.z -= M_2PI_F;

      delta.rot = mRot;
      delta.rotVec.x = delta.rotVec.y = 0.0f;
      delta.rotVec.z = prevZRot - mRot.z;
      if (delta.rotVec.z > M_PI_F)
         delta.rotVec.z -= M_2PI_F;
      else if (delta.rotVec.z < -M_PI_F)
         delta.rotVec.z += M_2PI_F;

      delta.head = mHead;
      delta.headVec -= mHead;
   }

// Begin MMO Kit
   // Flying 
   MatrixF zRot,xRot,superRot;
   zRot.set(EulerF(0.0f, 0.0f, mRot.z));
   xRot.set(EulerF(mHead.x, 0.0f, 0.0f));
   if (flyingMod || mSwimming)
      superRot.mul(zRot, xRot);
   else
      superRot = zRot;

   // End Flying

//   MatrixF zRot;
//   zRot.set(EulerF(0.0f, 0.0f, mRot.z));
// End MMO Kit

   // Desired move direction & speed
   VectorF moveVec;
   F32 moveSpeed;
   // AFX CODE BLOCK (anim-clip) <<
   // If BLOCK_USER_CONTROL is set in anim_clip_flags, the user won't be able to
   // resume control over the player character. This generally happens for
   // short periods of time synchronized with script driven animation at places
   // where it makes sense that user motion is prohibited, such as when the 
   // player is lifted off the ground or knocked down.
   if (mState == MoveState && mDamageState == Enabled && !isAnimationLocked())
   /* ORIGINAL CODE
   if (mState == MoveState && mDamageState == Enabled)
    */
   // AFX CODE BLOCK (anim-clip) >>
   {
      zRot.getColumn(0,&moveVec);
      moveVec *= move->x;
      VectorF tv;
// Begin MMO Kit
      superRot.getColumn(1,&tv); // One Line for flight
      //zRot.getColumn(1,&tv);
// End MMO Kit
      moveVec += tv * move->y;

      // Clamp water movement
      if (move->y > 0.0f)
      {
// Begin MMO Kit
/* Original TGE Code:
         if( mWaterCoverage >= 0.9f )
            moveSpeed = getMax(mDataBlock->maxUnderwaterForwardSpeed * move->y,
                               mDataBlock->maxUnderwaterSideSpeed * mFabs(move->x));
         else
            moveSpeed = getMax(mDataBlock->maxForwardSpeed * move->y,
                               mDataBlock->maxSideSpeed * mFabs(move->x));
*/
            moveSpeed = getMax(mDataBlock->maxForwardSpeed*moveMod * move->y,
                               mDataBlock->maxSideSpeed*moveMod * mFabs(move->x));
// End MMO Kit
      }
      else
      {
// Begin MMO Kit
/* Original TGE Code:
         if( mWaterCoverage >= 0.9f )
            moveSpeed = getMax(mDataBlock->maxUnderwaterBackwardSpeed * mFabs(move->y),
                               mDataBlock->maxUnderwaterSideSpeed * mFabs(move->x));
         else
            moveSpeed = getMax(mDataBlock->maxBackwardSpeed * mFabs(move->y),
                               mDataBlock->maxSideSpeed * mFabs(move->x));
*/
         moveSpeed = getMax(mDataBlock->maxBackwardSpeed*moveMod * mFabs(move->y),
                            mDataBlock->maxSideSpeed*moveMod * mFabs(move->x));
// End MMO Kit
      }

/* Original TGE Code:
      // Cancel any script driven animations if we are going to move.
      if (moveVec.x + moveVec.y + moveVec.z != 0.0f &&
          (mActionAnimation.action >= rpgPlayerData::NumTableActionAnims
               || mActionAnimation.action == rpgPlayerData::LandAnim))
         mActionAnimation.action = rpgPlayerData::NullAnimation;
*/
   }
   else
   {
      moveVec.set(0.0f, 0.0f, 0.0f);
      moveSpeed = 0.0f;
   }

   // Acceleration due to gravity
   VectorF acc(0.0f, 0.0f, mGravity * mGravityMod * TickSec);

// Begin MMO Kit
   if (flyingMod)
      acc.set(0.0f,0.0f,mGravity * mGravityMod*.025f * TickSec*.5f);
   if (mSwimming)
      acc.set(0.0f,0.0f,0.0f);
// End MMO Kit

   // Determine ground contact normal. Only look for contacts if
   // we can move.
   VectorF contactNormal;
   bool jumpSurface = false, runSurface = false;

// Begin MMO Kit
   jumpSurface = runSurface = mRunSurface;
   contactNormal = mContactNormal;
   //if (!isMounted())
   //   findContact(&runSurface,&jumpSurface,&contactNormal);
// End MMO Kit

   if (jumpSurface)
      mJumpSurfaceNormal = contactNormal;

   // Acceleration on run surface
   if (runSurface) {
      mContactTimer = 0;

      // Remove acc into contact surface (should only be gravity)
      // Clear out floating point acc errors, this will allow
      // the player to "rest" on the ground.
      F32 vd = -mDot(acc,contactNormal);
      if (vd > 0.0f) {
         VectorF dv = contactNormal * (vd + 0.002f);
         acc += dv;
         if (acc.len() < 0.0001f)
            acc.set(0.0f, 0.0f, 0.0f);
      }

      // Force a 0 move if there is no energy, and only drain
      // move energy if we're moving.
      VectorF pv;
      if (mEnergy >= mDataBlock->minRunEnergy) {
         if (moveSpeed)
            mEnergy -= mDataBlock->runEnergyDrain;
         pv = moveVec;
      }
      else
         pv.set(0.0f, 0.0f, 0.0f);

      // Adjust the players's requested dir. to be parallel
      // to the contact surface.
      F32 pvl = pv.len();
      if (pvl) {
         VectorF nn;
         mCross(pv,VectorF(0.0f, 0.0f, 1.0f),&nn);
         nn *= 1.0f / pvl;
         VectorF cv = contactNormal;
         cv -= nn * mDot(nn,cv);
         pv -= cv * mDot(pv,cv);
         pvl = pv.len();
      }

      // Convert to acceleration
      if (pvl)
         pv *= moveSpeed / pvl;
      VectorF runAcc = pv - (mVelocity + acc);
      F32 runSpeed = runAcc.len();

      // Clamp acceleratin, player also accelerates faster when
      // in his hard landing recover state.
      F32 maxAcc = (mDataBlock->runForce / mMass) * TickSec;
      if (mState == RecoverState)
         maxAcc *= mDataBlock->recoverRunForceScale;
      if (runSpeed > maxAcc)
         runAcc *= maxAcc / runSpeed;
      acc += runAcc;

/* Original TGE Code:
      // If we are running on the ground, then we're not jumping
      if (mDataBlock->isJumpAction(mActionAnimation.action))
         mActionAnimation.action = rpgPlayerData::NullAnimation;
*/
   }
   else
// Begin MMO Kit
   {
      mContactTimer++;
      if (mContactTimer > 100)
         mContactTimer = 100;
   }

   if (flyingMod)
   {
      F32 impulse = mDataBlock->runForce / mMass; // The impulse that is going to be applied

      // Get the move vector
      VectorF pv = moveVec;
      F32 pvl = pv.len();

      if (pvl)
      {
         pv *= moveSpeed / pvl;

         // Initialize flyAcc
         VectorF flyAcc = pv - ( mVelocity + acc);

         //Apply the impulse to vector
         flyAcc *= impulse;
         //      flyAcc.x *= impulse;
         //      flyAcc.y *= impulse;
         //      flyAcc.z *= impulse;

         // Get ready to limit the acceleration
         F32 flySpeed = flyAcc.len();
         F32 maxAcc = impulse * TickSec*.5f*moveMod;

         // Limit the acceleration
         if (flySpeed > maxAcc)
            flyAcc *= maxAcc / flySpeed;

         // Apply the flying acceleration to the total acceleration.
         acc += flyAcc;
      }
      else
      {
         if (!runSurface)
         {
            mVelocity[0]*=TickSec*.5f;
            mVelocity[1]*=TickSec*.5f;
            if (mVelocity[2]>0.f)
               mVelocity[2]*=TickSec*.5f;


         }
         
      }
   }

   // End Flying 
// End MMO Kit

   // Acceleration from Jumping
   // AFX CODE BLOCK (anim-clip) <<
   // While BLOCK_USER_CONTROL is set in anim_clip_flags, the user won't be able to
   // make the player character jump.
   if (move->trigger[2] && !isMounted() && canJump() && !isAnimationLocked())
   /* ORIGINAL CODE
   if (move->trigger[2] && !isMounted() && canJump())
    */
   // AFX CODE BLOCK (anim-clip) >>
   {
      // Scale the jump impulse base on maxJumpSpeed
      F32 zSpeedScale = mVelocity.z;
      if (zSpeedScale <= mDataBlock->maxJumpSpeed)
      {
         zSpeedScale = (zSpeedScale <= mDataBlock->minJumpSpeed)? 1.0f :
            1.0f - (zSpeedScale - mDataBlock->minJumpSpeed) /
            (mDataBlock->maxJumpSpeed - mDataBlock->minJumpSpeed);

         // Desired jump direction
         VectorF pv = moveVec;
         F32 len = pv.len();
         if (len > 0.0f)
            pv *= 1.0f / len;

         // We want to scale the jump size by the player size, somewhat
         // in reduced ratio so a smaller player can jump higher in
         // proportion to his size, than a larger player.
         F32 scaleZ = (getScale().z * 0.25f) + 0.75f;

         // If we are facing into the surface jump up, otherwise
         // jump away from surface.
         F32 dot = mDot(pv,mJumpSurfaceNormal);
         F32 impulse = mDataBlock->jumpForce / mMass;
         if (dot <= 0.0f)
            acc.z += mJumpSurfaceNormal.z * scaleZ * impulse * zSpeedScale;
         else
         {
            acc.x += pv.x * impulse * dot;
            acc.y += pv.y * impulse * dot;
            acc.z += mJumpSurfaceNormal.z * scaleZ * impulse * zSpeedScale;
         }

         mJumpDelay = mDataBlock->jumpDelay;
         mEnergy -= mDataBlock->jumpEnergyDrain;

/* Original TGE Code:
         setActionThread((mVelocity.len() < 0.5f) ?
            rpgPlayerData::StandJumpAnim : rpgPlayerData::JumpAnim, true, false, true);
*/
         mJumpSurfaceLastContact = JumpSkipContactsMax;
      }
   }
   else
      if (jumpSurface) {
         if (mJumpDelay > 0)
            mJumpDelay--;
         mJumpSurfaceLastContact = 0;
      }
      else
         mJumpSurfaceLastContact++;

// Begin MMO Kit
   // Acceleration from Swimming
   // I don't understand physics, nor 3d math. Forgive me if this
   // looks horrid. It seems to work fairly well though, so I'll
   // be using it for now.   
     if (!isMounted() && canSwim() &&!mCheckPushed)    
     {   
        mSwimming = true;  // Not actually used, but perhaps good to have

        F32 impulse = mDataBlock->runForce / mMass; // The impulse that is going to be applied

        // Get the move vector
        VectorF pv = moveVec;
        F32 pvl = pv.len();

        if (pvl)
        {
           pv *= moveSpeed / pvl;

           // Initialize flyAcc
           VectorF flyAcc = pv - ( mVelocity + acc);

           //Apply the impulse to vector
           flyAcc *= impulse*2.f;
           //      flyAcc.x *= impulse;
           //      flyAcc.y *= impulse;
           //      flyAcc.z *= impulse;

           // Get ready to limit the acceleration
           F32 flySpeed = flyAcc.len();
           F32 maxAcc = impulse * TickSec*moveMod*.5f;

           // Limit the acceleration
           if (flySpeed > maxAcc)
              flyAcc *= maxAcc / flySpeed;

           // Apply the flying acceleration to the total acceleration.
           acc += flyAcc;
        }
        else
        {
           if (!runSurface)
           {
              mVelocity[0]*=TickSec*.5f;
              mVelocity[1]*=TickSec*.5f;
              if (mVelocity[2]>0.f)
                 mVelocity[2]*=TickSec*.5f;


           }

        }

     } else {
        mSwimming = false;
     }
// End MMO Kit
      
   // Add in force from physical zones...
   acc += (mAppliedForce / mMass) * TickSec;

   // Adjust velocity with all the move & gravity acceleration
   // TG: I forgot why doesn't the TickSec multiply happen here...
// Begin MMO Kit
/* Original TGE Code:
   mVelocity += acc;
*/
   if (!mCheckPushed)
      mVelocity += acc;
// End MMO Kit

   // apply horizontal air resistance

   F32 hvel = mSqrt(mVelocity.x * mVelocity.x + mVelocity.y * mVelocity.y);

   if(hvel > mDataBlock->horizResistSpeed)
   {
      F32 speedCap = hvel;
      if(speedCap > mDataBlock->horizMaxSpeed)
         speedCap = mDataBlock->horizMaxSpeed;
      speedCap -= mDataBlock->horizResistFactor * TickSec * (speedCap - mDataBlock->horizResistSpeed);
      F32 scale = speedCap / hvel;
      mVelocity.x *= scale;
      mVelocity.y *= scale;
   }
   if(mVelocity.z > mDataBlock->upResistSpeed)
   {
      if(mVelocity.z > mDataBlock->upMaxSpeed)
         mVelocity.z = mDataBlock->upMaxSpeed;
      mVelocity.z -= mDataBlock->upResistFactor * TickSec * (mVelocity.z - mDataBlock->upResistSpeed);
   }

// Begin MMO Kit
   if(mVelocity.z < -70.0f)
      mVelocity.z = -70.0f;
// End MMO Kit
   
   // Container buoyancy & drag
   if (mBuoyancy != 0.0f)
   {     // Applying buoyancy when standing still causing some jitters-
      if (mBuoyancy > 1.0f || !mVelocity.isZero() || !runSurface)
         mVelocity.z -= mBuoyancy * mGravity * mGravityMod * TickSec;
   }
   mVelocity   -= mVelocity * mDrag * TickSec;

   // If we are not touching anything and have sufficient -z vel,
   // we are falling.
   if (runSurface)
      mFalling = false;
   else {
      VectorF vel;
      mWorldToObj.mulV(mVelocity,&vel);
      mFalling = vel.z < sFallingThreshold;
   }

   if (!isGhost()) {
      // Vehicle Dismount
      if(move->trigger[2] && isMounted())
         Con::executef(mDataBlock,2,"doDismount",scriptThis());

      if(!inLiquid && mWaterCoverage != 0.0f) {
         Con::executef(mDataBlock,4,"onEnterLiquid",scriptThis(), Con::getFloatArg(mWaterCoverage), Con::getIntArg(mLiquidType));
         inLiquid = true;
      }
      else if(inLiquid && mWaterCoverage == 0.0f) {
         Con::executef(mDataBlock,3,"onLeaveLiquid",scriptThis(), Con::getIntArg(mLiquidType));
         inLiquid = false;
      }
   }
   else {
      if(!inLiquid && mWaterCoverage >= 1.0f) {

         inLiquid = true;
      }
      else if(inLiquid && mWaterCoverage < 0.8f) {
         if(getVelocity().len() >= mDataBlock->exitSplashSoundVel && !isMounted())
            alxPlay(mDataBlock->sound[rpgPlayerData::ExitWater], &getTransform());
         inLiquid = false;
      }
   }

// Begin MMO Kit
   if (gGamePaused)
      mVelocity.set(0.0f,0.0f,0.0f);
// End MMO Kit
#endif // DARREN_MMO
}


//----------------------------------------------------------------------------

bool rpgPlayer::checkDismountPosition(const MatrixF& oldMat, const MatrixF& mat)
{
   AssertFatal(getContainer() != NULL, "Error, must have a container!");
   AssertFatal(getObjectMount() != NULL, "Error, must be mounted!");

   Point3F pos;
   Point3F oldPos;
   mat.getColumn(3, &pos);
   oldMat.getColumn(3, &oldPos);
   RayInfo info;
   disableCollision();
   getObjectMount()->disableCollision();
   if (getContainer()->castRay(oldPos, pos, mCollisionMoveMask, &info))
   {
      enableCollision();
      getObjectMount()->enableCollision();
      return false;
   }

   Box3F wBox = mObjBox;
   wBox.min += pos;
   wBox.max += pos;

   EarlyOutPolyList polyList;
   polyList.mNormal.set(0.0f, 0.0f, 0.0f);
   polyList.mPlaneList.clear();
   polyList.mPlaneList.setSize(6);
   polyList.mPlaneList[0].set(wBox.min,VectorF(-1.0f, 0.0f, 0.0f));
   polyList.mPlaneList[1].set(wBox.max,VectorF(0.0f, 1.0f, 0.0f));
   polyList.mPlaneList[2].set(wBox.max,VectorF(1.0f, 0.0f, 0.0f));
   polyList.mPlaneList[3].set(wBox.min,VectorF(0.0f, -1.0f, 0.0f));
   polyList.mPlaneList[4].set(wBox.min,VectorF(0.0f, 0.0f, -1.0f));
   polyList.mPlaneList[5].set(wBox.max,VectorF(0.0f, 0.0f, 1.0f));

   if (getContainer()->buildPolyList(wBox, mCollisionMoveMask, &polyList))
   {
      enableCollision();
      getObjectMount()->enableCollision();
      return false;
   }

   enableCollision();
   getObjectMount()->enableCollision();
   return true;
}


//----------------------------------------------------------------------------

bool rpgPlayer::canJump()
{
   return mState == MoveState && mDamageState == Enabled && !isMounted() && !mJumpDelay && mEnergy >= mDataBlock->minJumpEnergy && mJumpSurfaceLastContact < JumpSkipContactsMax;
}

// Begin MMO Kit
bool rpgPlayer::canSwim()
{  
   return mState == MoveState && mDamageState == Enabled && !isMounted() && mWaterCoverage >= 0.7f;
}
// End MMO Kit

//----------------------------------------------------------------------------

void rpgPlayer::updateDamageLevel()
{
   if (!isGhost())
      setDamageState((mDamage >= mDataBlock->maxDamage)? Disabled: Enabled);
   if (mDamageThread)
      mShapeInstance->setPos(mDamageThread, mDamage / mDataBlock->destroyedLevel);
}

void rpgPlayer::updateDamageState()
{
   // Become a corpse when we're disabled (dead).
   if (mDamageState == Enabled) {
      mTypeMask &= ~CorpseObjectType;
      mTypeMask |= PlayerObjectType;
   }
   else {
      mTypeMask &= ~PlayerObjectType;
      mTypeMask |= CorpseObjectType;
   }

   Parent::updateDamageState();
}


//----------------------------------------------------------------------------

/* Original TGE Code:
void rpgPlayer::updateLookAnimation()
{
   // AFX CODE BLOCK (player-look) <<
   // If the preference setting overrideLookAnimation is true, the player's
   // arm and head no longer animate according to the view direction. They
   // are instead given fixed positions.
   if (overrideLookAnimation)
   {
     if (mArmAnimation.thread) 
       mShapeInstance->setPos(mArmAnimation.thread, armLookOverridePos);
     if (mHeadVThread) 
       mShapeInstance->setPos(mHeadVThread, headVLookOverridePos);
     if (mHeadHThread) 
       mShapeInstance->setPos(mHeadHThread, headHLookOverridePos);
     return;
   }
   // AFX CODE BLOCK (player-look) >>

   // Adjust look pos.  This assumes that the animations match
   // the min and max look angles provided in the datablock.
   if (mArmAnimation.thread) {
      // TG: Adjust arm position to avoid collision.
      F32 tp = mControlObject? 0.5f :
         (mHead.x - mArmRange.min) / mArmRange.delta;
      mShapeInstance->setPos(mArmAnimation.thread,mClampF(tp, 0.0f, 1.0f));
   }
   if (mHeadVThread) {
      F32 tp = (mHead.x - mHeadVRange.min) / mHeadVRange.delta;
      mShapeInstance->setPos(mHeadVThread,mClampF(tp, 0.0f, 1.0f));
   }
   if (mHeadHThread) {
      F32 dt = 2.0f * mDataBlock->maxLookAngle;
      F32 tp = (mHead.z + mDataBlock->maxLookAngle) / dt;
      mShapeInstance->setPos(mHeadHThread,mClampF(tp, 0.0f, 1.0f));
   }
}
*/


//----------------------------------------------------------------------------
// Methods to get delta (as amount to affect velocity by)

/* Original TGE Code:
bool rpgPlayer::inDeathAnim()
{
   if (mActionAnimation.thread && mActionAnimation.action >= 0)
      if (mActionAnimation.action < mDataBlock->actionCount)
         return mDataBlock->actionList[mActionAnimation.action].death;

   return false;
}

// Get change from mLastDeathPos - return current pos.  Assumes we're in death anim.
F32 rpgPlayer::deathDelta(Point3F & delta)
{
   // Get ground delta from the last time we offset this.
   MatrixF  mat;
   F32 pos = mShapeInstance->getPos(mActionAnimation.thread);
   mShapeInstance->deltaGround1(mActionAnimation.thread, mDeath.lastPos, pos, mat);
   mat.getColumn(3, & delta);
   return pos;
}
*/

// Called before updatePos() to prepare it's needed change to velocity, which
// must roll over.  Should be updated on tick, this is where we remember last
// position of animation that was used to roll into velocity.
void rpgPlayer::updateDeathOffsets()
{
/* Original TGE Code:
   if (inDeathAnim())
      // Get ground delta from the last time we offset this.
      mDeath.lastPos = deathDelta(mDeath.posAdd);
   else
*/
      mDeath.clear();
}


//----------------------------------------------------------------------------

static const U32 sPlayerConformMask =  InteriorObjectType|StaticShapeObjectType
                                       |StaticObjectType|TerrainObjectType;

static void accel(F32& from, F32 to, F32 rate)
{
   if (from < to)
      from = getMin(from += rate, to);
   else
      from = getMax(from -= rate, to);
}

/* Original TGE Code:
// if (dt == -1)
//    normal tick, so we advance.
// else
//    interpolate with dt as % of tick, don't advance
//
MatrixF * rpgPlayer::Death::fallToGround(F32 dt, const Point3F& loc, F32 curZ, F32 boxRad)
{
   static const F32 sConformCheckDown = 4.0f;
   RayInfo     coll;
   bool        conformToStairs = false;
   Point3F     pos(loc.x, loc.y, loc.z + 0.1f);
   Point3F     below(pos.x, pos.y, loc.z - sConformCheckDown);
   MatrixF  *  retVal = NULL;

   PROFILE_START(ConformToGround);

   if (gClientContainer.castRay(pos, below, sPlayerConformMask, &coll))
   {
      F32      adjust, height = (loc.z - coll.point.z), sink = curSink;
      VectorF  desNormal = coll.normal;
      VectorF  normal = curNormal;

      // dt >= 0 means we're interpolating and don't accel the numbers
      if (dt >= 0.0f)
         adjust = dt * TickSec;
      else
         adjust = TickSec;

      // Try to get them to conform to stairs by doing several LOS calls.  We do this if
      // normal is within about 5 deg. of vertical.
      if (desNormal.z > 0.995f)
      {
         Point3F  corners[3], downpts[3];
         S32      c;

         for (c = 0; c < 3; c++) {    // Build 3 corners to cast down from-
            corners[c].set(loc.x - boxRad, loc.y - boxRad, loc.z + 1.0f);
            if (c)      // add (0,boxWidth) and (boxWidth,0)
               corners[c][c - 1] += (boxRad * 2.0f);
            downpts[c].set(corners[c].x, corners[c].y, loc.z - sConformCheckDown);
         }

         // Do the three casts-
         for (c = 0; c < 3; c++)
            if (gClientContainer.castRay(corners[c], downpts[c], sPlayerConformMask, &coll))
               downpts[c] = coll.point;
            else
               break;

         // Do the math if everything hit below-
         if (c == 3) {
            mCross(downpts[1] - downpts[0], downpts[2] - downpts[1], &desNormal);
            AssertFatal(desNormal.z > 0, "Abnormality in rpgPlayer::Death::fallToGround()");
			   downpts[2] = downpts[2] - downpts[1];
			   downpts[1] = downpts[1] - downpts[0];
            desNormal.normalize();
            conformToStairs = true;
         }
      }

      // Move normal in direction we want-
      F32   * cur = normal, * des = desNormal;
      for (S32 i = 0; i < 3; i++)
         accel(*cur++, *des++, adjust * 0.25f);

      if (mFabs(height) < 2.2f && !normal.isZero() && desNormal.z > 0.01f)
      {
         VectorF  upY(0.0f, 1.0f, 0.0f), ahead;
         VectorF  sideVec;
         MatrixF  mat(true);

         normal.normalize();
         mat.set(EulerF (0.0f, 0.0f, curZ));
         mat.mulV(upY, & ahead);
	      mCross(ahead, normal, &sideVec);
         sideVec.normalize();
         mCross(normal, sideVec, &ahead);

         static MatrixF resMat(true);
         resMat.setColumn(0, sideVec);
         resMat.setColumn(1, ahead);
         resMat.setColumn(2, normal);

         // Adjust Z down to account for box offset on slope.  Figure out how
         // much we want to sink, and gradually accel to this amount.  Don't do if
         // we're conforming to stairs though
         F32   xy = mSqrt(desNormal.x * desNormal.x + desNormal.y * desNormal.y);
         F32   desiredSink = (boxRad * xy / desNormal.z);
         if (conformToStairs)
            desiredSink *= 0.5f;

         accel(sink, desiredSink, adjust * 0.15f);

         Point3F  position(pos);
         position.z -= sink;
         resMat.setColumn(3, position);

         if (dt < 0.0f)
         {  // we're moving, so update normal and sink amount
            curNormal = normal;
            curSink = sink;
         }

         retVal = &resMat;
      }
   }
   PROFILE_END();
   return retVal;
}
*/

//-------------------------------------------------------------------------------------

/* Original TGE Code:
// This is called ::onAdd() to see if we're in a sitting animation.  These then
// can use a longer tick delay for the mount to get across.
bool rpgPlayer::inSittingAnim()
{
   U32   action = mActionAnimation.action;
   if (mActionAnimation.thread && action < mDataBlock->actionCount) {
      const char * name = mDataBlock->actionList[action].name;
      if (!dStricmp(name, "Sitting") || !dStricmp(name, "Scoutroot"))
         return true;
   }
   return false;
}
*/


//----------------------------------------------------------------------------

/* Original TGE Code:
bool rpgPlayer::setArmThread(const char* sequence)
{
   // The arm sequence must be in the action list.
   for (U32 i = 1; i < mDataBlock->actionCount; i++)
      if (!dStricmp(mDataBlock->actionList[i].name,sequence))
         return setArmThread(i);
   return false;
}

bool rpgPlayer::setArmThread(U32 action)
{
   rpgPlayerData::ActionAnimation &anim = mDataBlock->actionList[action];
   if (anim.sequence != -1 &&
      anim.sequence != mShapeInstance->getSequence(mArmAnimation.thread))
   {
      mShapeInstance->setSequence(mArmAnimation.thread,anim.sequence,0);
      mArmAnimation.action = action;
      setMaskBits(ActionMask);
      return true;
   }
   return false;
}
*/

//----------------------------------------------------------------------------

/* Original TGE Code:
bool rpgPlayer::setActionThread(const char* sequence,bool hold,bool wait,bool fsp)
{
   // AFX CODE BLOCK (anim-clip) <<
   if (anim_clip_flags & ANIM_OVERRIDDEN)
     return false;
   // AFX CODE BLOCK (anim-clip) >>

   for (U32 i = 1; i < mDataBlock->actionCount; i++)
   {
      rpgPlayerData::ActionAnimation &anim = mDataBlock->actionList[i];
      if (!dStricmp(anim.name,sequence))
      {
         setActionThread(i,true,hold,wait,fsp);
         setMaskBits(ActionMask);
         return true;
      }
   }
   return false;
}

void rpgPlayer::setActionThread(U32 action,bool forward,bool hold,bool wait,bool fsp, bool forceSet)
{
   if (mActionAnimation.action == action && !forceSet)
      return;

   if (action >= rpgPlayerData::NumActionAnims)
   {
      Con::errorf("rpgPlayer::setActionThread(%d): rpgPlayer action out of range", action);
      return;
   }

   rpgPlayerData::ActionAnimation &anim = mDataBlock->actionList[action];
   if (anim.sequence != -1)
   {
      mActionAnimation.action          = action;
      mActionAnimation.forward         = forward;
      mActionAnimation.firstPerson     = fsp;
      mActionAnimation.holdAtEnd       = hold;
      mActionAnimation.waitForEnd      = hold? true: wait;
      mActionAnimation.animateOnServer = fsp;
      mActionAnimation.atEnd           = false;
      mActionAnimation.delayTicks      = (S32)sNewAnimationTickTime;
      mActionAnimation.atEnd           = false;

      if (sUseAnimationTransitions && (isGhost()/ * || mActionAnimation.animateOnServer* /))
      {
         // The transition code needs the timeScale to be set in the
         // right direction to know which way to go.
         F32   transTime = sAnimationTransitionTime;
         if (mDataBlock && mDataBlock->isJumpAction(action))
            transTime = 0.15f;

         mShapeInstance->setTimeScale(mActionAnimation.thread,
            mActionAnimation.forward? 1: -1);
         mShapeInstance->transitionToSequence(mActionAnimation.thread,anim.sequence,
            mActionAnimation.forward? 0: 1, transTime, true);
      }
      else
         mShapeInstance->setSequence(mActionAnimation.thread,anim.sequence,
            mActionAnimation.forward? 0: 1);
   }
}

void rpgPlayer::updateActionThread()
{
   PROFILE_START(UpdateActionThread);

   // Select an action animation sequence, this assumes that
   // this function is called once per tick.

   // This is annoying - BJG
   //AssertWarn(mActionAnimation.action != rpgPlayerData::NullAnimation, "Attempting to advance position of NULL animation thread.");

   if(mActionAnimation.action != rpgPlayerData::NullAnimation)
      if (mActionAnimation.forward)
         mActionAnimation.atEnd = mShapeInstance->getPos(mActionAnimation.thread) == 1;
      else
         mActionAnimation.atEnd = mShapeInstance->getPos(mActionAnimation.thread) == 0;

   // Only need to deal with triggers on the client
   if (isGhost())  {
      bool triggeredLeft = false;
      bool triggeredRight = false;
      F32 offset = 0.0f;
      if(mShapeInstance->getTriggerState(1)) {
         triggeredLeft = true;
         offset = -mDataBlock->decalOffset;
      }
      else if(mShapeInstance->getTriggerState(2)) {
         triggeredRight = true;
         offset = mDataBlock->decalOffset;
      }

      if (triggeredLeft || triggeredRight)
      {
         Point3F rot, pos;
         static RayInfo rInfo;
         MatrixF mat = getRenderTransform();
         mat.getColumn(1, &rot);
         mat.mulP(Point3F(offset,0.0f,0.0f), &pos);
         if (gClientContainer.castRay(Point3F(pos.x, pos.y, pos.z + 0.01f),
            Point3F(pos.x, pos.y, pos.z - 2.0f ),
            TerrainObjectType | InteriorObjectType | VehicleObjectType, &rInfo))
         {
            S32 sound = -1;
            // Only put footpuffs and prints on the terrain
            if( rInfo.object->getTypeMask() & TerrainObjectType)
            {
               TerrainBlock* tBlock = static_cast<TerrainBlock*>(rInfo.object);

               // Footpuffs, if we can get the material color...
               S32 mapIndex = tBlock->getTerrainMapIndex(rInfo.point);
               if (mapIndex != -1) {
                  MaterialPropertyMap* pMatMap = static_cast<MaterialPropertyMap*>(Sim::findObject("MaterialPropertyMap"));
                  const MaterialPropertyMap::MapEntry* pEntry = pMatMap->getMapEntryFromIndex(mapIndex);
                  if(pEntry)
                  {
                     sound = pEntry->sound;
                     if( rInfo.t <= 0.5f && mWaterCoverage == 0.0f)
                     {
                        // New emitter every time for visibility reasons
                        ParticleEmitter * emitter = new ParticleEmitter;
                        emitter->onNewDataBlock( mDataBlock->footPuffEmitter );

                        S32 x;
                        ColorF colorList[ParticleEngine::PC_COLOR_KEYS];

                        for(x = 0; x < 2; ++x)
                           colorList[x].set( pEntry->puffColor[x].red, pEntry->puffColor[x].green, pEntry->puffColor[x].blue, pEntry->puffColor[x].alpha );
                        for(x = 2; x < ParticleEngine::PC_COLOR_KEYS; ++x)
                           colorList[x].set( 1.0f, 1.0f, 1.0f, 0.0f );

                        emitter->setColors( colorList );
                        if( !emitter->registerObject() )
                        {
                           Con::warnf( ConsoleLogEntry::General, "Could not register emitter for particle of class: %s", mDataBlock->getName() );
                           delete emitter;
                           emitter = NULL;
                        }
                        else
                        {
                           emitter->emitParticles( pos, Point3F( 0.0f, 0.0f, 1.0f ), mDataBlock->footPuffRadius,
                                                   Point3F(0.0f, 0.0f, 0.0f), mDataBlock->footPuffNumParts );
                           emitter->deleteWhenEmpty();
                        }
                     }
                  }
               }

               // Footprint...
               if (mDataBlock->decalData != NULL)
                  mSceneManager->getCurrentDecalManager()->addDecal(rInfo.point, rot,
                     Point3F(rInfo.normal), getScale(), mDataBlock->decalData);
            }
            else
               if ( rInfo.object->getTypeMask() & VehicleObjectType)
                  sound = 2; // Play metal sound
               else if( rInfo.object->getTypeMask() & InteriorObjectType)
               {
                  MaterialPropertyMap* pMatMap = static_cast<MaterialPropertyMap*> (Sim::findObject ("MaterialPropertyMap")); 
                  const MaterialPropertyMap::MapEntry* pEntry = pMatMap->getMapEntryFromIndex(rInfo.material);
                  if(pEntry)
                     sound = pEntry->sound;
                  else
                     sound = 0;
               } 
               else
                  sound = 0;

            // Play footstep sounds
            playFootstepSound(triggeredLeft, sound);
         }
      }
   }

   // Mount pending variable puts a hold on the delayTicks below so players don't
   // inadvertently stand up because their mount has not come over yet.
   if (mMountPending)
      mMountPending = (isMounted() ? 0 : (mMountPending - 1));

   if (mActionAnimation.action == rpgPlayerData::NullAnimation ||
       ((!mActionAnimation.waitForEnd || mActionAnimation.atEnd)) &&
       !mActionAnimation.holdAtEnd && (mActionAnimation.delayTicks -= !mMountPending) <= 0)
   {
      //The scripting language will get a call back when a script animation has finished...
      //  example: When the chat menu animations are done playing...
      if ( isServerObject() && mActionAnimation.action >= rpgPlayerData::NumTableActionAnims )
         Con::executef(mDataBlock,3,"animationDone",scriptThis());
      pickActionAnimation();
   }

   // AFX CODE BLOCK (anim-clip) <<
   // prevent scaling of AFX picked actions
   if ( (mActionAnimation.action != rpgPlayerData::LandAnim) &&
        (mActionAnimation.action != rpgPlayerData::NullAnimation) &&
        !(anim_clip_flags & ANIM_OVERRIDDEN))
   / * ORIGINAL CODE
   if ( (mActionAnimation.action != rpgPlayerData::LandAnim) &&
        (mActionAnimation.action != rpgPlayerData::NullAnimation) )
    * /
   // AFX CODE BLOCK (anim-clip) >>
   {
      // Update action animation time scale to match ground velocity
      rpgPlayerData::ActionAnimation &anim =
         mDataBlock->actionList[mActionAnimation.action];
      F32 scale = 1;
      if (anim.velocityScale && anim.speed) {
         VectorF vel;
         mWorldToObj.mulV(mVelocity,&vel);
         scale = mFabs(mDot(vel, anim.dir) / anim.speed);

         if (scale > mDataBlock->maxTimeScale)
            scale = mDataBlock->maxTimeScale;
      }

      mShapeInstance->setTimeScale(mActionAnimation.thread,
                                   mActionAnimation.forward? scale: -scale);
   }
   PROFILE_END();
}

void rpgPlayer::pickActionAnimation()
{
   // Only select animations in our normal move state.
   if (mState != MoveState || mDamageState != Enabled)
      return;

   if (isMounted())
   {
      // Go into root position unless something was set explicitly
      // from a script.
      if (mActionAnimation.action != rpgPlayerData::RootAnim &&
          mActionAnimation.action < rpgPlayerData::NumTableActionAnims)
         setActionThread(rpgPlayerData::RootAnim,true,false,false);
      return;
   }

   bool forward = true;
   U32 action = rpgPlayerData::RootAnim;
   if (mFalling)
   {
      // Not in contact with any surface and falling
      action = rpgPlayerData::FallAnim;
   }
   else
   {
      if (mContactTimer >= sContactTickTime) {
         // Nothing under our feet
         action = rpgPlayerData::RootAnim;
      }
      else
      {
         // Our feet are on something
         // Pick animation that is the best fit for our current velocity.
         // Assumes that root is the first animation in the list.
         F32 curMax = 0.1f;
         VectorF vel;
         mWorldToObj.mulV(mVelocity,&vel);
         for (U32 i = 1; i < rpgPlayerData::NumMoveActionAnims; i++)
         {
            rpgPlayerData::ActionAnimation &anim = mDataBlock->actionList[i];
            if (anim.sequence != -1 && anim.speed) {
               F32 d = mDot(vel, anim.dir);
               if (d > curMax)
               {
                  curMax = d;
                  action = i;
                  forward = true;
               }
               else
               {
                  // Special case, re-use slide left animation to slide right
                  if (i == rpgPlayerData::SideLeftAnim && -d > curMax)
                  {
                     curMax = -d;
                     action = i;
                     forward = false;
                  }
               }
            }
         }
      }
   }
   setActionThread(action,forward,false,false);
}

void rpgPlayer::onImageRecoil(U32,ShapeBaseImageData::StateData::RecoilState)
{
   if (mRecoilThread)
   {
      mShapeInstance->setPos(mRecoilThread,0);
      mShapeInstance->setTimeScale(mRecoilThread,1);
   }
}
*/

void rpgPlayer::onUnmount(ShapeBase* obj,S32 node)
{
   // Reset back to root position during dismount.
/* Original TGE Code:
   setActionThread(rpgPlayerData::RootAnim,true,false,false);
*/

   // Re-orient the player straight up
   Point3F pos,vec;
   getTransform().getColumn(1,&vec);
   getTransform().getColumn(3,&pos);
   Point3F rot(0.0f,0.0f,-mAtan(-vec.x,vec.y));
   setPosition(pos,rot);

   // Parent function will call script
   Parent::onUnmount(obj,node);
}


//----------------------------------------------------------------------------

/* Original TGE Code:
void rpgPlayer::updateAnimation(F32 dt)
{
   if ((isGhost() || mActionAnimation.animateOnServer) && mActionAnimation.thread)
      mShapeInstance->advanceTime(dt,mActionAnimation.thread);
   if (mRecoilThread)
      mShapeInstance->advanceTime(dt,mRecoilThread);

   // AFX CODE BLOCK (anim-clip) <<
   // update any active blend clips
   if (isGhost())
      for (S32 i = 0; i < blend_clips.size(); i++)
        mShapeInstance->advanceTime(dt, blend_clips[i].thread);
   // AFX CODE BLOCK (anim-clip) >>

   // If we are the client's player on this machine, then we need
   // to make sure the transforms are up to date as they are used
   // to setup the camera.
   if (isGhost())
   {
      if (getControllingClient())
      {
         updateAnimationTree(isFirstPerson());
         mShapeInstance->animate();
      }
      else
      {
         updateAnimationTree(false);
      }
   }
}

void rpgPlayer::updateAnimationTree(bool firstPerson)
{
   S32 mode = 0;
   if (firstPerson)
      if (mActionAnimation.firstPerson)
         mode = 0;
//            TSShapeInstance::MaskNodeRotation;
//            TSShapeInstance::MaskNodePosX |
//            TSShapeInstance::MaskNodePosY;
      else
         mode = TSShapeInstance::MaskNodeAllButBlend;

   for (U32 i = 0; i < rpgPlayerData::NumSpineNodes; i++)
      if (mDataBlock->spineNode[i] != -1)
         mShapeInstance->setNodeAnimationState(mDataBlock->spineNode[i],mode);
}
*/

//----------------------------------------------------------------------------

bool rpgPlayer::step(Point3F *pos,F32 *maxStep,F32 time)
{
	#ifdef DARREN_MMO

   const Point3F& scale = getScale();
   Box3F box;
   VectorF offset = mVelocity * time;

// Begin MMO Kit
   Box3F    mbox;
   F32 r = 1.2f;//mDataBlock->radius;

   mbox.max.x = r*.3f;//mDataBlock->boxSize.x * 0.5;
   mbox.max.y = r*.3f;//mDataBlock->boxSize.y * 0.5;
   mbox.max.z = 2.0f;//mDataBlock->boxSize.z;
   mbox.min.x = -r*.3f;//-mObjBox.max.x;
   mbox.min.y = -r*.3f;//-mObjBox.max.y;
   mbox.min.z = 0.0f;

   box.min = mbox.min + offset + *pos;
   box.max = mbox.max + offset + *pos;
   box.max.z += mDataBlock->maxStepHeight + sMinFaceDistance;
/* Original TGE Code:
   box.min = mObjBox.min + offset + *pos;
   box.max = mObjBox.max + offset + *pos;
   box.max.z += mDataBlock->maxStepHeight * scale.z + sMinFaceDistance;
*/
// End MMO Kit

   SphereF sphere;
   sphere.center = (box.min + box.max) * 0.5f;
   VectorF bv = box.max - sphere.center;
   sphere.radius = bv.len();

   ClippedPolyList polyList;
   polyList.mPlaneList.clear();
   polyList.mNormal.set(0.0f, 0.0f, 0.0f);
   polyList.mPlaneList.setSize(6);
   polyList.mPlaneList[0].set(box.min,VectorF(-1.0f, 0.0f, 0.0f));
   polyList.mPlaneList[1].set(box.max,VectorF(0.0f, 1.0f, 0.0f));
   polyList.mPlaneList[2].set(box.max,VectorF(1.0f, 0.0f, 0.0f));
   polyList.mPlaneList[3].set(box.min,VectorF(0.0f, -1.0f, 0.0f));
   polyList.mPlaneList[4].set(box.min,VectorF(0.0f, 0.0f, -1.0f));
   polyList.mPlaneList[5].set(box.max,VectorF(0.0f, 0.0f, 1.0f));

   CollisionWorkingList& rList = mConvex.getWorkingList();
   CollisionWorkingList* pList = rList.wLink.mNext;
   while (pList != &rList) {
      Convex* pConvex = pList->mConvex;
      if ((pConvex->getObject()->getType() & StaticObjectType) != 0)
      {
         Box3F convexBox = pConvex->getBoundingBox();
         if (box.isOverlapped(convexBox))
            pConvex->getPolyList(&polyList);
      }
      pList = pList->wLink.mNext;
   }

   // Find max step height
   F32 stepHeight = pos->z - sMinFaceDistance;
   U32* vp = polyList.mIndexList.begin();
   U32* ep = polyList.mIndexList.end();
   for (; vp != ep; vp++) {
      F32 h = polyList.mVertexList[*vp].point.z + sMinFaceDistance;
      if (h > stepHeight)
         stepHeight = h;
   }

   F32 step = stepHeight - pos->z;
   if (stepHeight > pos->z && step < *maxStep) {
      // Go ahead and step
      pos->z = stepHeight;
      *maxStep -= step;
      return true;
   }
#endif //#ifdef DARREN_MMO

   return false;
}


//----------------------------------------------------------------------------
inline Point3F createInterpPos(const Point3F& s, const Point3F& e, const F32 t, const F32 d)
{
   Point3F ret;
   ret.interpolate(s, e, t/d);
   return ret;
}

// Begin MMO Kit

bool rpgPlayer::updatePos(const F32 travelTime)
{
#ifdef DARREN_MMO
	getTransform().getColumn(3,&delta.posVec);

   if (mVelocity.len() < .01f)
      mVelocity.set(0.0f,0.0f,0.0f);


   Interior::smIncludeClipHulls = true;

   bool lastRunSurface = mRunSurface;
   
   Point3F start;
   Point3F initialPosition;
   getTransform().getColumn(3,&start);
   initialPosition = start;
   Point3F end = start + mVelocity * travelTime;
   Point3F distance = end - start;

   F32 velZ = mVelocity.z;
   F32 initialVelLen = mVelocity.len()*travelTime;
   Point3F initialVelocity = mVelocity;
   Point3F velNorm = mVelocity;
   velNorm.normalize();
   velNorm.neg();

   SceneObject *impactObject = NULL;
   VectorF impactNormal;
   F32     impactVelocity;



   disableCollision();
   RayInfo rinfo;

   Point3F v = mVelocity;
   v.z = 0.f;
   v.normalize();
   v*=.75f;

   mCanKite = true;

   if (getContainer()->castRay(start+Point3F(0.0f,0.0f,1.0f), end+Point3F(0.0f,0.0f,1.0f)+v, mCollisionMoveMask, &rinfo))
   {
      if (rinfo.object && rinfo.object->getTypeMask()&InteriorObjectType)
         mCanKite=false;

     
         

      F32 bd = -mDot(mVelocity, rinfo.normal);

      if (bd >= 0.0f || (rinfo.object && rinfo.object->getTypeMask()&StaticTSObjectType))
      {
         

         F32 dot = mDot(Point3F(0.0f,0.0f,1.0f), rinfo.normal);//mFabs();
         F32 s = 0.05f;

         if (dot<0.55f)
         {
            mVelocity=initialVelocity;
            mVelocity.neg();
            mVelocity*=0.75f;
            s = 0.65f;
         }
         end = rinfo.point+velNorm*s;
         end.z+=0.02f;
         
         if (dot < 0.55f)
            end.z = start.z;
            

      }
      
   }

   //floor
   mRunSurface = false;
   if (getContainer()->castRay(end+Point3F(0.0f,0.0f,1.0f), end-Point3F(0.0f,0.0f,0.65f), mCollisionMoveMask, &rinfo))
   {
      F32 bd = -mDot(mVelocity, rinfo.normal);

      if (rinfo.object && rinfo.object->getTypeMask()&InteriorObjectType)
         mCanKite=false;


      //if (!(mTypeMask&AIObjectType))
      //   dPrintf("%f %f %f\n",rinfo.normal.x,rinfo.normal.y,rinfo.normal.z);

      if (bd >= 0.0 || (rinfo.object && rinfo.object->getTypeMask()&StaticTSObjectType))
      {

         // shake camera on ground impact
         if( bd > mDataBlock->groundImpactMinSpeed && isClientObject() )
         {
            F32 ampScale = (bd - mDataBlock->groundImpactMinSpeed) / mDataBlock->minImpactSpeed;

            CameraShake *groundImpactShake = new CameraShake;
            groundImpactShake->setDuration( mDataBlock->groundImpactShakeDuration );
            groundImpactShake->setFrequency( mDataBlock->groundImpactShakeFreq );

            VectorF shakeAmp = mDataBlock->groundImpactShakeAmp * ampScale;
            groundImpactShake->setAmplitude( shakeAmp );
            groundImpactShake->setFalloff( mDataBlock->groundImpactShakeFalloff );
            groundImpactShake->init();
            gCamFXMgr.addFX( groundImpactShake );
         }

         if (mFabs(initialVelocity.z) > mDataBlock->minImpactSpeed && !mMountPending) {

            if (!isGhost() && !(mTypeMask & AIObjectType))
            {
               //onImpact(rinfo.object, rinfo.normal*mFabs(initialVelocity.z));
               impactObject = rinfo.object;
               impactNormal = rinfo.normal;
               impactVelocity = mFabs(initialVelocity.z);

            }
               



         }

         F32 rcos = mDataBlock->runSurfaceCos;
         if (mTypeMask & AIObjectType)
            rcos = mCos(mDegToRad(90.0f));

         mContactNormal = rinfo.normal;

         mRunSurface  = mContactNormal.z > rcos;

         //*jump = bestVd > mDataBlock->jumpSurfaceCos;
         
         if (mRunSurface || initialVelocity.z < 0.0f)
         {
            mRunSurface = true;
            end = rinfo.point+rinfo.normal * 0.02f;
            mVelocity=initialVelocity;//step
            mVelocity.z = 0.0f;
         }
         else
         {
            end = initialPosition;
            mVelocity.set(0.0f,0.0f,0.0f);
         }

      }

   }


   enableCollision();

   Point3F vec = end-initialPosition;
   F32 d = vec.len();
   //if (!(mTypeMask&AIObjectType))
   //   dPrintf("%f %f\n",d,initialVelLen);
   if (d>initialVelLen && mRunSurface==lastRunSurface)
   {
      vec.normalize();
      end = start+vec*initialVelLen;
   }
   
   //falling thru world
   if (mFabs(initialPosition.z-end.z)>1.0f)
      if (getContainer()->castRay(initialPosition, end, mCollisionMoveMask, &rinfo))
      {
         end = rinfo.point+rinfo.normal * 0.1f;
      }




   // Set new position
   // If on the client, calc delta for backstepping
   if (isClientObject())
   {
      delta.pos = end;
      delta.posVec = delta.posVec - delta.pos;
      delta.dt = 1;
   }

   //rotation isn't being set :|
//   if (
//       mFabs(end.x-initialPosition.x)>.01f ||
//       mFabs(end.y-initialPosition.y)>.01f ||
//       mFabs(end.z-initialPosition.z)>.01f 
 //     )
   {
      setPosition(end,mRot);
      setMaskBits(MoveMask);
      updateContainer();

   }


   if (mVelocity.z > velZ)
      mVelocity.z = 0.0f;

   if (isServerObject() &&!(mTypeMask&AIObjectType))
   {
      gServerContainer.initRadiusSearch(end, 2.0f, TriggerObjectType);
      S32 id;
      while (id = gServerContainer.containerSearchNext())
      {
         Trigger* trigger = dynamic_cast<Trigger*>(Sim::findObject(id));
         if (trigger)
            trigger->potentialEnterObject(this);
         
      }
      
      
   }

   if (impactObject)
      onImpact(impactObject, impactNormal*impactVelocity);

   Interior::smIncludeClipHulls = false;
#endif // DARREN_MMO
   return true;

}

/* Original TGE Code:
bool rpgPlayer::updatePos(const F32 travelTime)
{
   PROFILE_START(Player_UpdatePos);
   getTransform().getColumn(3,&delta.posVec);

   // When mounted to another object, only Z rotation used.
   if (isMounted()) {
      mVelocity = mMount.object->getVelocity();
      setPosition(Point3F(0.0f, 0.0f, 0.0f), mRot);
      setMaskBits(MoveMask);
      PROFILE_END();
      return true;
   }

   // Try and move to new pos
   F32 totalMotion  = 0.0f;
   F32 initialSpeed = mVelocity.len();

   Point3F start;
   Point3F initialPosition;
   getTransform().getColumn(3,&start);
   initialPosition = start;
   CollisionList collisionList;
   CollisionList physZoneCollisionList;

   MatrixF collisionMatrix(true);
   collisionMatrix.setColumn(3, start);

   VectorF firstNormal;
   F32 maxStep = mDataBlock->maxStepHeight;
   F32 time = travelTime;
   U32 count = 0;

   const Point3F& scale = getScale();

   static Polyhedron sBoxPolyhedron;
   static ExtrudedPolyList sExtrudedPolyList;
   static ExtrudedPolyList sPhysZonePolyList;

   for (; count < sMoveRetryCount; count++) {
      F32 speed = mVelocity.len();
      if (!speed && !mDeath.haveVelocity())
         break;

      Point3F end = start + mVelocity * time;
      if (mDeath.haveVelocity()) {
         // Add in death movement-
         VectorF  deathVel = mDeath.getPosAdd();
         VectorF  resVel;
         getTransform().mulV(deathVel, & resVel);
         end += resVel;
      }
      Point3F distance = end - start;

      if (mFabs(distance.x) < mObjBox.len_x() &&
          mFabs(distance.y) < mObjBox.len_y() &&
          mFabs(distance.z) < mObjBox.len_z())
      {
         // We can potentially early out of this.  If there are no polys in the clipped polylist at our
         //  end position, then we can bail, and just set start = end;
         Box3F wBox = mScaledBox;
         wBox.min += end;
         wBox.max += end;

         static EarlyOutPolyList eaPolyList;
         eaPolyList.clear();
         eaPolyList.mNormal.set(0.0f, 0.0f, 0.0f);
         eaPolyList.mPlaneList.clear();
         eaPolyList.mPlaneList.setSize(6);
         eaPolyList.mPlaneList[0].set(wBox.min,VectorF(-1.0f, 0.0f, 0.0f));
         eaPolyList.mPlaneList[1].set(wBox.max,VectorF(0.0f, 1.0f, 0.0f));
         eaPolyList.mPlaneList[2].set(wBox.max,VectorF(1.0f, 0.0f, 0.0f));
         eaPolyList.mPlaneList[3].set(wBox.min,VectorF(0.0f, -1.0f, 0.0f));
         eaPolyList.mPlaneList[4].set(wBox.min,VectorF(0.0f, 0.0f, -1.0f));
         eaPolyList.mPlaneList[5].set(wBox.max,VectorF(0.0f, 0.0f, 1.0f));

         // Build list from convex states here...
         CollisionWorkingList& rList = mConvex.getWorkingList();
         CollisionWorkingList* pList = rList.wLink.mNext;
         while (pList != &rList) {
            Convex* pConvex = pList->mConvex;
            if (pConvex->getObject()->getTypeMask() & sCollisionMoveMask) {
               Box3F convexBox = pConvex->getBoundingBox();
               if (wBox.isOverlapped(convexBox))
               {
                  // No need to seperate out the physical zones here, we want those
                  //  to cause a fallthrough as well...
                  pConvex->getPolyList(&eaPolyList);
               }
            }
            pList = pList->wLink.mNext;
         }

         if (eaPolyList.isEmpty())
         {
            totalMotion += (end - start).len();
            start = end;
            break;
         }
      }

      collisionMatrix.setColumn(3, start);
      sBoxPolyhedron.buildBox(collisionMatrix, mScaledBox);

      // Setup the bounding box for the extrudedPolyList
      Box3F plistBox = mScaledBox;
      collisionMatrix.mul(plistBox);
      Point3F oldMin = plistBox.min;
      Point3F oldMax = plistBox.max;
      plistBox.min.setMin(oldMin + (mVelocity * time) - Point3F(0.1f, 0.1f, 0.1f));
      plistBox.max.setMax(oldMax + (mVelocity * time) + Point3F(0.1f, 0.1f, 0.1f));

      // Build extruded polyList...
      VectorF vector = end - start;
      sExtrudedPolyList.extrude(sBoxPolyhedron,vector);
      sExtrudedPolyList.setVelocity(mVelocity);
      sExtrudedPolyList.setCollisionList(&collisionList);

      sPhysZonePolyList.extrude(sBoxPolyhedron,vector);
      sPhysZonePolyList.setVelocity(mVelocity);
      sPhysZonePolyList.setCollisionList(&physZoneCollisionList);

      // Build list from convex states here...
      CollisionWorkingList& rList = mConvex.getWorkingList();
      CollisionWorkingList* pList = rList.wLink.mNext;
      while (pList != &rList) {
         Convex* pConvex = pList->mConvex;
         if (pConvex->getObject()->getTypeMask() & sCollisionMoveMask) {
            Box3F convexBox = pConvex->getBoundingBox();
            if (plistBox.isOverlapped(convexBox))
            {
               if (pConvex->getObject()->getTypeMask() & PhysicalZoneObjectType)
                  pConvex->getPolyList(&sPhysZonePolyList);
               else
                  pConvex->getPolyList(&sExtrudedPolyList);
            }
         }
         pList = pList->wLink.mNext;
      }

      // Take into account any physical zones...
      for (U32 j = 0; j < physZoneCollisionList.count; j++) {
         AssertFatal(dynamic_cast<PhysicalZone*>(physZoneCollisionList.collision[j].object), "Bad phys zone!");
         PhysicalZone* pZone = (PhysicalZone*)physZoneCollisionList.collision[j].object;
         if (pZone->isActive())
            mVelocity *= pZone->getVelocityMod();
      }

      if (collisionList.count != 0 && collisionList.t < 1.0f) {
         // Set to collision point
         F32 velLen = mVelocity.len();

         F32 dt = time * getMin(collisionList.t, 1.0f);
         start += mVelocity * dt;
         time -= dt;

         totalMotion += velLen * dt;

         mFalling = false;

         // Back off...
         if ( velLen > 0.f ) {
            F32 newT = getMin(0.01f / velLen, dt);
            start -= mVelocity * newT;
            totalMotion -= velLen * newT;
         }

         // Try stepping if there is a vertical surface
         if (collisionList.maxHeight < start.z + mDataBlock->maxStepHeight * scale.z) {
            bool stepped = false;
            for (U32 c = 0; c < collisionList.count; c++) {
               Collision& cp = collisionList.collision[c];
               // if (mFabs(mDot(cp.normal,VectorF(0,0,1))) < sVerticalStepDot)
               //    Dot with (0,0,1) just extracts Z component [lh]-
               if (mFabs(cp.normal.z) < sVerticalStepDot)
               {
                  stepped = step(&start,&maxStep,time);
                  break;
               }
            }
            if (stepped)
            {
               continue;
            }
         }

         // Pick the surface most parallel to the face that was hit.
         Collision* collision = &collisionList.collision[0];
         Collision* cp = collision + 1;
         Collision *ep = collision + collisionList.count;
         for (; cp != ep; cp++)
         {
            if (cp->faceDot > collision->faceDot)
               collision = cp;
         }

         F32 bd = -mDot(mVelocity,collision->normal);

         // shake camera on ground impact
         if( bd > mDataBlock->groundImpactMinSpeed && isControlObject() )
         {
            F32 ampScale = (bd - mDataBlock->groundImpactMinSpeed) / mDataBlock->minImpactSpeed;

            CameraShake *groundImpactShake = new CameraShake;
            groundImpactShake->setDuration( mDataBlock->groundImpactShakeDuration );
            groundImpactShake->setFrequency( mDataBlock->groundImpactShakeFreq );

            VectorF shakeAmp = mDataBlock->groundImpactShakeAmp * ampScale;
            groundImpactShake->setAmplitude( shakeAmp );
            groundImpactShake->setFalloff( mDataBlock->groundImpactShakeFalloff );
            groundImpactShake->init();
            gCamFXMgr.addFX( groundImpactShake );
         }


         if (bd > mDataBlock->minImpactSpeed && !mMountPending) {
            if (!isGhost())
               onImpact(collision->object, collision->normal*bd);

            if (mDamageState == Enabled && mState != RecoverState) {
               // Scale how long we're down for
               F32   value = (bd - mDataBlock->minImpactSpeed);
               F32   range = (mDataBlock->minImpactSpeed * 0.9f);
               U32   recover = mDataBlock->recoverDelay;
               if (value < range)
                  recover = 1 + S32(mFloor( F32(recover) * value / range) );
               // Con::printf("Used %d recover ticks", recover);
               // Con::printf("  minImpact = %g, this one = %g", mDataBlock->minImpactSpeed, bd);
               setState(RecoverState, recover);
            }
         }
         if (isServerObject() && bd > (mDataBlock->minImpactSpeed / 3.0f)) {
            mImpactSound = rpgPlayerData::ImpactNormal;
            setMaskBits(ImpactMask);
         }

         // Subtract out velocity
         VectorF dv = collision->normal * (bd + sNormalElasticity);
         mVelocity += dv;
         if (count == 0)
         {
            firstNormal = collision->normal;
         }
         else
         {
            if (count == 1)
            {
               // Re-orient velocity along the crease.
               if (mDot(dv,firstNormal) < 0.0f &&
                   mDot(collision->normal,firstNormal) < 0.0f)
               {
                  VectorF nv;
                  mCross(collision->normal,firstNormal,&nv);
                  F32 nvl = nv.len();
                  if (nvl)
                  {
                     if (mDot(nv,mVelocity) < 0.0f)
                        nvl = -nvl;
                     nv *= mVelocity.len() / nvl;
                     mVelocity = nv;
                  }
               }
            }
         }

         // Track collisions
         if (!isGhost() && collision->object->getTypeMask() & ShapeBaseObjectType) {
            ShapeBase* col = static_cast<ShapeBase*>(collision->object);
            queueCollision(col,mVelocity - col->getVelocity());
         }
      }
      else
      {
         totalMotion += (end - start).len();
         start = end;
         break;
      }
   }

   if (count == sMoveRetryCount)
   {
      // Failed to move
      start = initialPosition;
      mVelocity.set(0.0f, 0.0f, 0.0f);
   }

   // Set new position
   // If on the client, calc delta for backstepping
   if (isClientObject())
   {
      delta.pos = start;
      delta.posVec = delta.posVec - delta.pos;
      delta.dt = 1.0f;
   }

   setPosition(start,mRot);
   setMaskBits(MoveMask);
   updateContainer();

   if (!isGhost())  {
      // Collisions are only queued on the server and can be
      // generated by either updateMove or updatePos
      notifyCollision();

      // Do mission area callbacks on the server as well
      checkMissionArea();
   }
   PROFILE_END();

   // Check the totaldistance moved.  If it is more than 1000th of the velocity, then
   //  we moved a fair amount...
   if (totalMotion >= (0.001f * initialSpeed))
      return true;
   else
      return false;
}
*/
// End MMO Kit

//----------------------------------------------------------------------------

void rpgPlayer::findContact(bool* run,bool* jump,VectorF* contactNormal)
{
   Point3F pos;
   getTransform().getColumn(3,&pos);

   Box3F wBox;
   Point3F exp(0.0f,0.0f,sTractionDistance);
   wBox.min = pos + mScaledBox.min - exp;
   wBox.max.x = pos.x + mScaledBox.max.x;
   wBox.max.y = pos.y + mScaledBox.max.y;
   wBox.max.z = pos.z + mScaledBox.min.z + sTractionDistance;

   static ClippedPolyList polyList;
   polyList.clear();
   polyList.doConstruct();
   polyList.mNormal.set(0.0f, 0.0f, 0.0f);
   polyList.setInterestNormal(Point3F(0.0f, 0.0f, -1.0f));

   polyList.mPlaneList.setSize(6);
   polyList.mPlaneList[0].setYZ(wBox.min, -1.0f);
   polyList.mPlaneList[1].setXZ(wBox.max, 1.0f);
   polyList.mPlaneList[2].setYZ(wBox.max, 1.0f);
   polyList.mPlaneList[3].setXZ(wBox.min, -1.0f);
   polyList.mPlaneList[4].setXY(wBox.min, -1.0f);
   polyList.mPlaneList[5].setXY(wBox.max, 1.0f);
   Box3F plistBox = wBox;

   // Expand build box as it will be used to collide with items.
   // PickupRadius will be at least the size of the box.
   F32 pd = mDataBlock->pickupDelta;
   wBox.min.x -= pd; wBox.min.y -= pd;
   wBox.max.x += pd; wBox.max.y += pd;
   wBox.max.z = pos.z + mScaledBox.max.z;

   rpgPlayer * serverParent = NULL;
   if (bool(mServerObject))
   {
      serverParent = dynamic_cast<rpgPlayer* >((NetObject *)mServerObject);
      GameConnection * con = serverParent->getControllingClient();
      if (con && !con->isAIControlled())
         serverParent = NULL;
   }

   // Build list from convex states here...
   CollisionWorkingList& rList = mConvex.getWorkingList();
   CollisionWorkingList* pList = rList.wLink.mNext;
// Begin MMO Kit
/* Original TGE Code:
   U32 mask = isGhost() ? sClientCollisionContactMask : sServerCollisionContactMask;
*/
   U32 mask = isGhost() ? mClientCollisionContactMask : mServerCollisionContactMask;
// End MMO Kit
   while (pList != &rList)
   {
      Convex* pConvex = pList->mConvex;

      U32 objectMask = pConvex->getObject()->getTypeMask();

      // Check: triggers, corpses and items...
      //
      if (objectMask & TriggerObjectType)
      {
         Trigger* pTrigger = static_cast<Trigger*>(pConvex->getObject());
         pTrigger->potentialEnterObject(this);
      }
      else if (objectMask & CorpseObjectType)
      {
         // If we've overlapped the worldbounding boxes, then that's it...
         if (getWorldBox().isOverlapped(pConvex->getObject()->getWorldBox()))
         {
            ShapeBase* col = static_cast<ShapeBase*>(pConvex->getObject());
            queueCollision(col,getVelocity() - col->getVelocity());
         }
      }
      else if (objectMask & ItemObjectType)
      {
         // If we've overlapped the worldbounding boxes, then that's it...
         Item* item = static_cast<Item*>(pConvex->getObject());
         if (getWorldBox().isOverlapped(item->getWorldBox()))
            if (this != item->getCollisionObject())
               queueCollision(item,getVelocity() - item->getVelocity());
      }
      else if ((objectMask & mask) && !(objectMask & PhysicalZoneObjectType))
      {
         Box3F convexBox = pConvex->getBoundingBox();
         if (plistBox.isOverlapped(convexBox) && serverParent == NULL)
            pConvex->getPolyList(&polyList);
      }

      pList = pList->wLink.mNext;
   }

   if (serverParent)
   {
      // Just grab the info-
      const ContactInfo & info = serverParent->mContactInfo;
      *jump = info.jump;
      *run = info.run;
      if (info.contacted)
         *contactNormal = info.contactNormal;
      return;
   }

   if (!polyList.isEmpty())
   {
      // Pick flattest surface
      F32 bestVd = -1.0f;
      ClippedPolyList::Poly* poly = polyList.mPolyList.begin();
      ClippedPolyList::Poly* end = polyList.mPolyList.end();
      for (; poly != end; poly++)
      {
         F32 vd = poly->plane.z;       // i.e.  mDot(Point3F(0,0,1), poly->plane);
         if (vd > bestVd)
         {
            bestVd = vd;
            *contactNormal = poly->plane;
         }
      }
// Begin MMO Kit
/* Original TGE Code:
      *run  = bestVd > mDataBlock->runSurfaceCos;
*/
      
      F32 rcos = mDataBlock->runSurfaceCos;
      if (mTypeMask & AIObjectType)
         rcos = mCos(mDegToRad(90.0f));

      *run  = bestVd > rcos;
// End MMO Kit
      *jump = bestVd > mDataBlock->jumpSurfaceCos;
   }
   else
      *jump = *run = false;

   // Save the info for client peeking hack-
   mContactInfo.clear();
   if ((mContactInfo.contacted = !polyList.isEmpty()))
      mContactInfo.contactNormal = *contactNormal;
   mContactInfo.run = *run;
   mContactInfo.jump = *jump;
}

//----------------------------------------------------------------------------

bool rpgPlayer::checkMissionArea() // <-- MMO Kit
{
#ifdef DARREN_MMO
   // Checks to see if the player is in the Mission Area...
   Point3F pos;
// Begin MMO Kit
/* Original TGE Code:
   MissionArea * obj = dynamic_cast<MissionArea*>(Sim::findObject("MissionArea"));

   if(!obj)
      return;
*/
   MissionArea * obj;
   if (isGhost())
      obj = gClientMissionArea;
   else
      obj = dynamic_cast<MissionArea*>(Sim::findObject("MissionArea"));
// End MMO Kit
   

   const RectI &area = obj->getArea();
   getTransform().getColumn(3, &pos);
#endif //DARREN_MMO
// Begin MMO Kit
/* Original TGE Code:
   if ((pos.x < area.point.x || pos.x > area.point.x + area.extent.x ||
       pos.y < area.point.y || pos.y > area.point.y + area.extent.y)) {
      if(mInMissionArea) {
         mInMissionArea = false;
         Con::executef(mDataBlock,3,"onLeaveMissionArea",scriptThis());
      }
   }
   else if(!mInMissionArea)
   {
      mInMissionArea = true;
      Con::executef(mDataBlock,3,"onEnterMissionArea",scriptThis());
   }
*/
   bool inStrat = false;
#ifdef DARREN_MMO

   F32 ceiling = obj->getFlightCeiling();
   F32 strat = obj->getStratosphere();

   if ((pos.x < area.point.x || pos.x > area.point.x + area.extent.x || 
       pos.y < area.point.y || pos.y > area.point.y + area.extent.y) || pos.z > ceiling) { 

      Point3F push(0.0f,0.0f,0.0f);
   
      if (pos.x < area.point.x)
         push[0]=1.0f;
      if (pos.x > area.point.x + area.extent.x)
         push[0]=-1.0f;

      if (pos.y < area.point.y)
         push[1]=1.0f;
      if (pos.y > area.point.y + area.extent.y)
         push[1]=-1.0f;

      if (pos.z > ceiling && (strat == -1.0f || pos.z<strat))
      {
         if (mVelocity[2]>0.0f)
            push[2]=-1.0f;
      }

      if (strat != -1.0f && pos.z>=strat)
         inStrat = true;

      if (push[0] || push[1] || push[2])
      {
         mCheckPushed =true;
         setVelocity(push);

         if(mInMissionArea && !isGhost()) {
            mInMissionArea = false;
            Con::executef(mDataBlock,3,"onLeaveMissionArea",scriptThis());
         }    

      }
   }
   else if(!mInMissionArea && !isGhost())
   {
      mInMissionArea = true;
      Con::executef(mDataBlock,3,"onEnterMissionArea",scriptThis());
   }   
#endif // DARREN_MMO
   return inStrat;
// End MMO Kit
}


//----------------------------------------------------------------------------

bool rpgPlayer::isDisplacable() const
{
   return true;
}

Point3F rpgPlayer::getMomentum() const
{
	return mLastPos; //DARREN: TEMP
//   return mVelocity * mMass;
}

void rpgPlayer::setMomentum(const Point3F& newMomentum)
{
   Point3F newVelocity = newMomentum / mMass;
//DARREN_MMO   mVelocity = newVelocity;
}

F32 rpgPlayer::getMass() const
{
   return mMass;
}

#define  LH_HACK   1
// Hack for short-term soln to Training crash -
#if   LH_HACK
static U32  sBalance;

bool rpgPlayer::displaceObject(const Point3F& displacement)
{
	bool result = true;
#ifdef DARREN_MMO

   F32 vellen = mVelocity.len();
   if (vellen < 0.001f || sBalance > 16) {
      mVelocity.set(0.0f, 0.0f, 0.0f);
      return false;
   }

   F32 dt = displacement.len() / vellen;

   sBalance++;

   bool result = updatePos(dt);

   sBalance--;

   getTransform().getColumn(3, &delta.pos);
   delta.posVec.set(0.0f, 0.0f, 0.0f);
#endif // #ifdef DARREN_MMO

   return result;
}

#else

bool rpgPlayer::displaceObject(const Point3F& displacement)
{
   F32 vellen = mVelocity.len();
   if (vellen < 0.001f) {
      mVelocity.set(0.0f, 0.0f, 0.0f);
      return false;
   }

   F32 dt = displacement.len() / vellen;

   bool result = updatePos(dt);

   mObjToWorld.getColumn(3, &delta.pos);
   delta.posVec.set(0.0f, 0.0f, 0.0f);

   return result;
}

#endif

//----------------------------------------------------------------------------

void rpgPlayer::setPosition(const Point3F& pos,const Point3F& rot)
{
   MatrixF mat;
   if (isMounted()) {
      // Use transform from mounted object
      MatrixF nmat,zrot;
      mMount.object->getMountTransform(mMount.node,&nmat);
      zrot.set(EulerF(0.0f, 0.0f, rot.z));
      mat.mul(nmat,zrot);
   }
   else {
      mat.set(EulerF(0.0f, 0.0f, rot.z));
      mat.setColumn(3,pos);
   }
   Parent::setTransform(mat);
   mRot = rot;
}


void rpgPlayer::setRenderPosition(const Point3F& pos, const Point3F& rot, F32 dt)
{
   MatrixF mat;
   if (isMounted()) {
      // Use transform from mounted object
      MatrixF nmat,zrot;
      mMount.object->getRenderMountTransform(mMount.node,&nmat);
      zrot.set(EulerF(0.0f, 0.0f, rot.z));
      mat.mul(nmat,zrot);
   }
   else {
      EulerF   orient(0.0f, 0.0f, rot.z);

      mat.set(orient);
      mat.setColumn(3, pos);

/* Original TGE Code:
      if (inDeathAnim()) {
         F32   boxRad = (mDataBlock->boxSize.x * 0.5f);
         if (MatrixF * fallMat = mDeath.fallToGround(dt, pos, rot.z, boxRad))
            mat = * fallMat;
      }
      else
         mDeath.initFall();
*/
   }
   Parent::setRenderTransform(mat);
}

//----------------------------------------------------------------------------

void rpgPlayer::setTransform(const MatrixF& mat)
{
   // This method should never be called on the client.

   // This currently converts all rotation in the mat into
   // rotations around the z axis.
   Point3F pos,vec;
   mat.getColumn(1,&vec);
   mat.getColumn(3,&pos);
   Point3F rot(0.0f, 0.0f, -mAtan(-vec.x,vec.y));
   setPosition(pos,rot);
   setMaskBits(MoveMask | NoWarpMask);
   mWarpTransform = mat; // <-- MMO Kit
}

void rpgPlayer::getEyeTransform(MatrixF* mat)
{
   const Point3F& scale = getScale(); // <-- MMO Kit
   // Eye transform in world space.  We only use the eye position
   // from the animation and supply our own rotation.
   MatrixF pmat,xmat,zmat;
   xmat.set(EulerF(mHead.x, 0.0f, 0.0f));
   zmat.set(EulerF(0.0f, 0.0f, mHead.z));
   pmat.mul(zmat,xmat);

   F32 *dp = pmat;
// Begin MMO Kit
/* Original TGE Code:
   F32* sp;
   if (mDataBlock->eyeNode != -1)
   {
      sp = mShapeInstance->mNodeTransforms[mDataBlock->eyeNode];
   }
   else
   {
      Point3F center;
      mObjBox.getCenter(&center);
      MatrixF eyeMat(true);
      eyeMat.setPosition(center);

      sp = eyeMat;
   }

   const Point3F& scale = getScale();
   dp[3] = sp[3] * scale.x;
   dp[7] = sp[7] * scale.y;
   dp[11] = sp[11] * scale.z;
*/
   //AssertFatal(mDataBlock->eyeNode != -1, "Trying to transform eyeNode that does not exist in model!");
   //F32 *sp = mShapeInstance->mNodeTransforms[mDataBlock->eyeNode];
   MatrixF transform;
   transform.identity();
   float height = 2.0f*scale.z;
   transform.setColumn(3,Point3F(0,0,height));
   F32 *sp = (F32*)&transform;
   dp[3] = sp[3] ; dp[7] = sp[7] ; dp[11] = sp[11] ;
// End MMO Kit
   mat->mul(getTransform(),pmat);
}

void rpgPlayer::getRenderEyeTransform(MatrixF* mat)
{
   const Point3F& scale = getScale(); // <-- MMO Kit
   // Eye transform in world space.  We only use the eye position
   // from the animation and supply our own rotation.
   MatrixF pmat,xmat,zmat;
   xmat.set(EulerF(mHead.x, 0.0f, 0.0f));
   zmat.set(EulerF(0.0f, 0.0f, mHead.z));
   pmat.mul(zmat,xmat);

// Begin MMO Kit
/* Original TGE Code:
   F32 *dp = pmat;
   F32* sp;
   if (mDataBlock->eyeNode != -1)
   {
      sp = mShapeInstance->mNodeTransforms[mDataBlock->eyeNode];
   }
   else
   {
      Point3F center;
      mObjBox.getCenter(&center);
      MatrixF eyeMat(true);
      eyeMat.setPosition(center);

      sp = eyeMat;
   }

   const Point3F& scale = getScale();
   dp[3] = sp[3] * scale.x;
   dp[7] = sp[7] * scale.y;
   dp[11] = sp[11] * scale.z;
*/
   MatrixF transform;
   transform.identity();
   float height = 0.8f+scale.z;
   transform.setColumn(3,Point3F(0.0f,0.0f,height));
   F32 *sp = (F32*)&transform;

   F32 *dp = pmat;//,*sp = mShapeInstance->mNodeTransforms[mDataBlock->eyeNode];
   dp[3] = sp[3] ; dp[7] = sp[7] ; dp[11] = sp[11] ;
// End MMO Kit
   mat->mul(getRenderTransform(), pmat);
}

void rpgPlayer::getMuzzleTransform(U32 imageSlot,MatrixF* mat)
{
   MatrixF nmat;
   Parent::getRetractionTransform(imageSlot,&nmat);
   MatrixF smat;
   Parent::getImageTransform(imageSlot,&smat);

   disableCollision();

   // See if we are pushed into a wall...
   if (getDamageState() == Enabled) {
      Point3F start, end;
      smat.getColumn(3, &start);
      nmat.getColumn(3, &end);

      RayInfo rinfo;
      if (getContainer()->castRay(start, end, 0xFFFFFFFF, &rinfo)) {
         Point3F finalPoint;
         finalPoint.interpolate(start, end, rinfo.t);
         nmat.setColumn(3, finalPoint);
      }
      else
         Parent::getMuzzleTransform(imageSlot,&nmat);
   }
   else
      Parent::getMuzzleTransform(imageSlot,&nmat);

   enableCollision();

   // If we are in one of the standard player animations, adjust the
   // muzzle to point in the direction we are looking.
// Begin MMO Kit
/* Original TGE Code:
   if (mActionAnimation.action < rpgPlayerData::NumTableActionAnims)
   {
      MatrixF xmat;
      xmat.set(EulerF(mHead.x, 0.0f, 0.0f));
      mat->mul(getTransform(),xmat);
      F32 *sp = nmat;
      F32 *dp = *mat;
      dp[3] = sp[3];
      dp[7] = sp[7];
      dp[11] = sp[11];
   }
   else
   {
      *mat = nmat;
   }
*/
   *mat = nmat;
// End MMO Kit
}


void rpgPlayer::getRenderMuzzleTransform(U32 imageSlot,MatrixF* mat)
{
   MatrixF nmat;
   Parent::getRenderRetractionTransform(imageSlot,&nmat);
   MatrixF smat;
   Parent::getRenderImageTransform(imageSlot,&smat);

   disableCollision();

   // See if we are pushed into a wall...
   if (getDamageState() == Enabled)
   {
      Point3F start, end;
      smat.getColumn(3, &start);
      nmat.getColumn(3, &end);

      RayInfo rinfo;
      if (getContainer()->castRay(start, end, 0xFFFFFFFF, &rinfo)) {
         Point3F finalPoint;
         finalPoint.interpolate(start, end, rinfo.t);
         nmat.setColumn(3, finalPoint);
      }
      else
      {
         Parent::getRenderMuzzleTransform(imageSlot,&nmat);
      }
   }
   else
   {
      Parent::getRenderMuzzleTransform(imageSlot,&nmat);
   }

   enableCollision();

   // If we are in one of the standard player animations, adjust the
   // muzzle to point in the direction we are looking.
// Begin MMO Kit
/* Original TGE Code:
   if (mActionAnimation.action < rpgPlayerData::NumTableActionAnims) {
      MatrixF xmat;
      xmat.set(EulerF(mHead.x, 0.0f, 0.0f));
      mat->mul(getRenderTransform(),xmat);
      F32 *sp = nmat;
      F32 *dp = *mat;
      dp[3] = sp[3];
      dp[7] = sp[7];
      dp[11] = sp[11];
   }
   else
   {
      *mat = nmat;
   }
*/
   *mat = nmat;
// End MMO Kit
}


// Bot aiming code calls this frequently and will work fine without the check
// for being pushed into a wall, which shows up on profile at ~ 3% (eight bots)
void rpgPlayer::getMuzzlePointAI(U32 imageSlot, Point3F* point)
{
   MatrixF nmat;
   Parent::getMuzzleTransform(imageSlot, &nmat);

   // If we are in one of the standard player animations, adjust the
   // muzzle to point in the direction we are looking.
// Begin MMO Kit
/* Original TGE Code:
   if (mActionAnimation.action < rpgPlayerData::NumTableActionAnims)
   {
      MatrixF xmat;
      xmat.set(EulerF(mHead.x, 0.0f, 0.0f));
      MatrixF result;
      result.mul(getTransform(), xmat);
      F32 *sp = nmat;
      F32 *dp = result;
      dp[3] = sp[3];
      dp[7] = sp[7];
      dp[11] = sp[11];
      result.getColumn(3, point);
   }
   else
   {
      nmat.getColumn(3, point);
   }
*/
   nmat.getColumn(3, point);
// End MMO Kit
}

void rpgPlayer::getCameraParameters(F32 *min,F32* max,Point3F* off,MatrixF* rot)
{
   if (!mControlObject.isNull() && mControlObject == getObjectMount()) {
      mControlObject->getCameraParameters(min,max,off,rot);
      return;
   }
   const Point3F& scale = getScale();
// Begin MMO Kit
/* Original TGE Code:
   *min = mDataBlock->cameraMinDist * scale.y;
   *max = mDataBlock->cameraMaxDist * scale.y;
*/
   *min = mDataBlock->cameraMinDist;
   *max = mDataBlock->cameraMaxDist;
// End MMO Kit
   off->set(0.0f, 0.0f, 0.0f);
   rot->identity();
}


//----------------------------------------------------------------------------

Point3F rpgPlayer::getVelocity() const
{
	return mLastPos; // DARREN: TEMP
// DARREN_MMO   return mVelocity;
}

void rpgPlayer::setVelocity(const VectorF& vel)
{
//DARREN_MMO   mVelocity = vel;
   setMaskBits(MoveMask);
}

void rpgPlayer::applyImpulse(const Point3F&,const VectorF& vec)
{
   // Players ignore angular velocity
   VectorF vel;
   vel.x = vec.x / mMass;
   vel.y = vec.y / mMass;
   vel.z = vec.z / mMass;
//DARREN_MMO   setVelocity(mVelocity + vel);
}


//----------------------------------------------------------------------------

bool rpgPlayer::castRay(const Point3F &start, const Point3F &end, RayInfo* info)
{
// Begin MMO Kit
/*
   // AFX CODE BLOCK (obj-select) <<
   // In standard TGE there's a rather brute force culling of all
   // non-enabled players (corpses) from the ray cast. But, to
   // demonstrate a resurrection spell, we need corpses to be
   // selectable, so this code change allows consideration of corpses
   // in the ray cast if corpsesHiddenFromRayCast is set to false.
   if (sCorpsesHiddenFromRayCast && getDamageState() != Enabled)
       return false;
   / * ORIGINAL CODE
   if (getDamageState() != Enabled)
      return false;
    * /
   // AFX CODE BLOCK (obj-select) >>
*/
   //if (getDamageState() != Enabled)
   //   return false; JMR

   
    //projectiles
    if (info->projectile)
    {
        if (info->srcObject)
        {
            rpgPlayer *op = dynamic_cast<rpgPlayer*>(info->srcObject);
            if(!allowHarmful(op))
                return false;
        }
    }
// End MMO Kit

   // Collide against bounding box. Need at least this for the editor.
   F32 st,et,fst = 0.0f,fet = 1.0f;
   F32 *bmin = &mObjBox.min.x;
   F32 *bmax = &mObjBox.max.x;
   F32 const *si = &start.x;
   F32 const *ei = &end.x;

   for (int i = 0; i < 3; i++) {
      if (*si < *ei) {
         if (*si > *bmax || *ei < *bmin)
            return false;
         F32 di = *ei - *si;
         st = (*si < *bmin)? (*bmin - *si) / di: 0.0f;
         et = (*ei > *bmax)? (*bmax - *si) / di: 1.0f;
      }
      else {
         if (*ei > *bmax || *si < *bmin)
            return false;
         F32 di = *ei - *si;
         st = (*si > *bmax)? (*bmax - *si) / di: 0.0f;
         et = (*ei < *bmin)? (*bmin - *si) / di: 1.0f;
      }
      if (st > fst) fst = st;
      if (et < fet) fet = et;
      if (fet < fst)
         return false;
      bmin++; bmax++;
      si++; ei++;
   }

   info->normal = start - end;
   info->normal.normalizeSafe();
   getTransform().mulV( info->normal );

   info->t = fst;
   info->object = this;
   info->point.interpolate(start,end,fst);
   info->material = 0;
   return true;
}


//----------------------------------------------------------------------------

static MatrixF IMat(1);

bool rpgPlayer::buildPolyList(AbstractPolyList* polyList, const Box3F&, const SphereF&)
{
   // Collision with the player is always against the player's object
   // space bounding box axis aligned in world space.
   Point3F pos;
   getTransform().getColumn(3,&pos);
   IMat.setColumn(3,pos);
   polyList->setTransform(&IMat, Point3F(1.0f,1.0f,1.0f));
   polyList->setObject(this);
   polyList->addBox(mObjBox);
   return true;
}


void rpgPlayer::buildConvex(const Box3F& box, Convex* convex)
{
   if (mShapeInstance == NULL)
      return;

   // These should really come out of a pool
   mConvexList->collectGarbage();

   Box3F realBox = box;
   mWorldToObj.mul(realBox);
   realBox.min.convolveInverse(mObjScale);
   realBox.max.convolveInverse(mObjScale);

   if (realBox.isOverlapped(getObjBox()) == false)
      return;

   Convex* cc = 0;
   CollisionWorkingList& wl = convex->getWorkingList();
   for (CollisionWorkingList* itr = wl.wLink.mNext; itr != &wl; itr = itr->wLink.mNext) {
      if (itr->mConvex->getType() == BoxConvexType &&
          itr->mConvex->getObject() == this) {
         cc = itr->mConvex;
         break;
      }
   }
   if (cc)
      return;

   // Create a new convex.
   BoxConvex* cp = new OrthoBoxConvex;
   mConvexList->registerObject(cp);
   convex->addToWorkingList(cp);
   cp->init(this);

   mObjBox.getCenter(&cp->mCenter);
   cp->mSize.x = mObjBox.len_x() / 2.0f;
   cp->mSize.y = mObjBox.len_y() / 2.0f;
   cp->mSize.z = mObjBox.len_z() / 2.0f;
}


//----------------------------------------------------------------------------

void rpgPlayer::updateWorkingCollisionSet()
{
#ifdef DARREN_MMO
   // First, we need to adjust our velocity for possible acceleration.  It is assumed
   // that we will never accelerate more than 20 m/s for gravity, plus 10 m/s for
   // jetting, and an equivalent 10 m/s for jumping.  We also assume that the
   // working list is updated on a Tick basis, which means we only expand our
   // box by the possible movement in that tick.
   Point3F scaledVelocity = mVelocity * TickSec;
   F32 len    = scaledVelocity.len();
   F32 newLen = len + (10.0f * TickSec);

   // Check to see if it is actually necessary to construct the new working list,
   // or if we can use the cached version from the last query.  We use the x
   // component of the min member of the mWorkingQueryBox, which is lame, but
   // it works ok.
   bool updateSet = false;

   Box3F convexBox = mConvex.getBoundingBox(getTransform(), getScale());
   F32 l = (newLen * 1.1f) + 0.1f;  // from Convex::updateWorkingList
   const Point3F  lPoint( l, l, l );
   convexBox.min -= lPoint;
   convexBox.max += lPoint;

   // Check containment
   if (mWorkingQueryBox.min.x != -1e9f)
   {
      if (mWorkingQueryBox.isContained(convexBox) == false)
         // Needed region is outside the cached region.  Update it.
         updateSet = true;
   }
   else
   {
      // Must update
      updateSet = true;
   }
   
   // Actually perform the query, if necessary
   if (updateSet == true)
   {
      const Point3F  twolPoint( 2.0f * l, 2.0f * l, 2.0f * l );
      mWorkingQueryBox = convexBox;
      mWorkingQueryBox.min -= twolPoint;
      mWorkingQueryBox.max += twolPoint;

      disableCollision();
// Begin MMO Kit
/* Original TGE Code:
      mConvex.updateWorkingList(mWorkingQueryBox,
         isGhost() ? sClientCollisionContactMask : sServerCollisionContactMask);
*/
      mConvex.updateWorkingList(mWorkingQueryBox,
         isGhost() ? mClientCollisionContactMask : mServerCollisionContactMask);
// End MMO Kit
      enableCollision();
   }
#endif //DARREN_MMO
}


//----------------------------------------------------------------------------

void rpgPlayer::writePacketData(GameConnection *connection, BitStream *stream)
{
   Parent::writePacketData(connection, stream);

/* Original TGE Code:
   stream->writeInt(mState,NumStateBits);
   if (stream->writeFlag(mState == RecoverState))
      stream->writeInt(mRecoverTicks,rpgPlayerData::RecoverDelayBits);
   if (stream->writeFlag(mJumpDelay))
      stream->writeInt(mJumpDelay,rpgPlayerData::JumpDelayBits);

   Point3F pos;
   getTransform().getColumn(3,&pos);
   if (stream->writeFlag(!isMounted())) {
      // Will get position from mount
      stream->setCompressionPoint(pos);
      stream->write(pos.x);
      stream->write(pos.y);
      stream->write(pos.z);
      stream->write(mVelocity.x);
      stream->write(mVelocity.y);
      stream->write(mVelocity.z);
      stream->writeInt(mJumpSurfaceLastContact > 15 ? 15 : mJumpSurfaceLastContact, 4);
   }
   stream->write(mHead.x);
   stream->write(mHead.z);
   stream->write(mRot.z);

   if (mControlObject) {
      S32 gIndex = connection->getGhostIndex(mControlObject);
      if (stream->writeFlag(gIndex != -1)) {
         stream->writeInt(gIndex,NetConnection::GhostIdBitSize);
         mControlObject->writePacketData(connection, stream);
      }
   }
   else
      stream->writeFlag(false);
*/
}


void rpgPlayer::readPacketData(GameConnection *connection, BitStream *stream)
{
   Parent::readPacketData(connection, stream);
#ifdef DARREN_MMO

// Begin MMO Kit
/* Original TGE Code:
   mState = (ActionState)stream->readInt(NumStateBits);
   if (stream->readFlag())
      mRecoverTicks = stream->readInt(rpgPlayerData::RecoverDelayBits);
   if (stream->readFlag())
      mJumpDelay = stream->readInt(rpgPlayerData::JumpDelayBits);
   else
      mJumpDelay = 0;

   Point3F pos,rot;
   if (stream->readFlag()) {
      // Only written if we are not mounted
      stream->read(&pos.x);
      stream->read(&pos.y);
      stream->read(&pos.z);
      stream->read(&mVelocity.x);
      stream->read(&mVelocity.y);
      stream->read(&mVelocity.z);
      stream->setCompressionPoint(pos);
      delta.pos = pos;
      mJumpSurfaceLastContact = stream->readInt(4);
   }
   else
      pos = delta.pos;
   stream->read(&mHead.x);
   stream->read(&mHead.z);
   stream->read(&rot.z);
   rot.x = rot.y = 0;
   setPosition(pos,rot);
   delta.head = mHead;
   delta.rot = rot;

   if (stream->readFlag()) {
      S32 gIndex = stream->readInt(NetConnection::GhostIdBitSize);
      ShapeBase* obj = static_cast<ShapeBase*>(connection->resolveGhost(gIndex));
      setControlObject(obj);
      obj->readPacketData(connection, stream);
   }
   else
      setControlObject(0);
*/

   delta.pos = getPosition();
   mRot.x = mRot.y = 0.0f;
   delta.rot = mRot;
   delta.head = mHead;
// End MMO Kit
#endif // #ifdef DARREN_MMO

}

U32 rpgPlayer::packUpdate(NetConnection *con, U32 mask, BitStream *stream)
{
	U32 retMask = 0;
#ifdef DARREN_MMO

// Begin MMO Kit
   //need to fix this, RPG stuff needs to be guaranteed, create a special net event
   if (mask & InitialUpdateMask)
   {
      mask |= RpgMask;
      setMaskBits(RpgMask); //before parent
   }
// End MMO Kit

   U32 retMask = Parent::packUpdate(con, mask, stream);

   if (stream->writeFlag((mask & ImpactMask) && !(mask & InitialUpdateMask)))
      stream->writeInt(mImpactSound, rpgPlayerData::ImpactBits);

// Begin MMO Kit
/* Original TGE Code:
   if (stream->writeFlag(mask & ActionMask &&
         mActionAnimation.action != rpgPlayerData::NullAnimation &&
         mActionAnimation.action >= rpgPlayerData::NumTableActionAnims)) {
      stream->writeInt(mActionAnimation.action,rpgPlayerData::ActionAnimBits);
      stream->writeFlag(mActionAnimation.holdAtEnd);
      stream->writeFlag(mActionAnimation.atEnd);
      stream->writeFlag(mActionAnimation.firstPerson);
      if (!mActionAnimation.atEnd) {
         // If somewhere in middle on initial update, must send position-
         F32   where = mShapeInstance->getPos(mActionAnimation.thread);
         if (stream->writeFlag((mask & InitialUpdateMask) != 0 && where > 0))
            stream->writeSignedFloat(where, 6);
      }
   }

   if (stream->writeFlag(mask & ActionMask &&
         mArmAnimation.action != rpgPlayerData::NullAnimation &&
         (!(mask & InitialUpdateMask) ||
         mArmAnimation.action != mDataBlock->lookAction))) {
      stream->writeInt(mArmAnimation.action,rpgPlayerData::ActionAnimBits);
   }
*/
   if (stream->writeFlag(mask & RpgMask))
   {
      stream->writeInt(mMoveModifier*100,16);
      stream->writeInt(mLight.mRadius*100,16);
      stream->write(mFlyingMod);
      stream->write(mRealm);
      stream->write(mEncounterSetting);
      stream->write(mAllianceLeader);
      stream->writeString(mGuildName);
      
      //stream->writeInt(mScaleModifier*100,16);

      //skinning
      if (1)
      {
         U32 i;
         for (i=0;i<SkinMax;i++)
         {
            stream->writeString(mSkins[i]);
         }
      }
      else
         stream->writeFlag(false);

   }
   
   if (stream->writeFlag(mask & NoWarpMask && getControllingClient() == con))
   {
      Point3F pos,vec;
      mWarpTransform.getColumn(3,&pos);
      stream->setCompressionPoint(pos);
      stream->write(pos.x);
      stream->write(pos.y);
      stream->write(pos.z);
      mWarpTransform.getColumn(1,&vec);
      stream->write(-mAtan(-vec.x,vec.y));
   }
// End MMO Kit

   // AFX CODE BLOCK (misc) <<
   retMask = afx_packUpdate(con, mask, stream, retMask);
   // AFX CODE BLOCK (misc) <<

   // The rest of the data is part of the control object packet update.
   // If we're controlled by this client, we don't need to send it.
   // we only need to send it if this is the initial update - in that case,
   // the client won't know this is the control object yet.
   if(stream->writeFlag(getControllingClient() == con && !(mask & InitialUpdateMask)))
      return(retMask);

   if (stream->writeFlag(mask & MoveMask))
   {
      stream->writeFlag(mFalling);
      stream->writeFlag(mSwimming); // <-- MMO Kit
      

      stream->writeInt(mState,NumStateBits);
      if (stream->writeFlag(mState == RecoverState))
         stream->writeInt(mRecoverTicks,rpgPlayerData::RecoverDelayBits);

      Point3F pos;
      getTransform().getColumn(3,&pos);
      stream->writeCompressedPoint(pos);
      F32 len = mVelocity.len();
      if(stream->writeFlag(len > 0.02f))
      {
         Point3F outVel = mVelocity;
         outVel *= 1.0f/len;
         stream->writeNormalVector(outVel, 10);
         len *= 32.0f;  // 5 bits of fraction
         if(len > 8191)
            len = 8191;
         stream->writeInt((S32)len, 13);
      }
      stream->writeFloat(mRot.z / M_2PI_F, 7);
      stream->writeSignedFloat(mHead.x / mDataBlock->maxLookAngle, 6);
      stream->writeSignedFloat(mHead.z / mDataBlock->maxLookAngle, 6);
      delta.move.pack(stream);
      stream->writeFlag(!(mask & NoWarpMask));
   }
   // Ghost need energy to predict reliably
   stream->writeFloat(getEnergyLevel() / mDataBlock->maxEnergy,EnergyLevelBits);
#endif //  DARREN_MMO

   return retMask;
}

void rpgPlayer::unpackUpdate(NetConnection *con, BitStream *stream)
{
   Parent::unpackUpdate(con,stream);
#ifdef DARREN_MMO

   if (stream->readFlag())
      mImpactSound = stream->readInt(rpgPlayerData::ImpactBits);

// Begin MMO Kit
/* Original TGE Code:
   // Server specified action animation
   if (stream->readFlag()) {
      U32 action = stream->readInt(rpgPlayerData::ActionAnimBits);
      bool hold = stream->readFlag();
      bool atEnd = stream->readFlag();
      bool fsp = stream->readFlag();

      F32   animPos = -1.0f;
      if (!atEnd && stream->readFlag())
         animPos = stream->readSignedFloat(6);

      if (isProperlyAdded()) {
         setActionThread(action,true,hold,true,fsp);
         bool  inDeath = inDeathAnim();
         if (atEnd)
         {
            mShapeInstance->clearTransition(mActionAnimation.thread);
            mShapeInstance->setPos(mActionAnimation.thread,
                                   mActionAnimation.forward? 1: 0);
            if (inDeath)
               mDeath.lastPos = 1.0f;
         }
         else if (animPos > 0) {
            mShapeInstance->setPos(mActionAnimation.thread, animPos);
            if (inDeath)
               mDeath.lastPos = animPos;
         }

         // mMountPending suppresses tickDelay countdown so players will sit until
         // their mount, or another animation, comes through (or 13 seconds elapses).
         mMountPending = (S32) (inSittingAnim() ? sMountPendingTickWait : 0);
      }
      else {
         mActionAnimation.action = action;
         mActionAnimation.holdAtEnd = hold;
         mActionAnimation.atEnd = atEnd;
         mActionAnimation.firstPerson = fsp;
      }
   }

   // Server specified arm animation
   if (stream->readFlag()) {
      U32 action = stream->readInt(rpgPlayerData::ActionAnimBits);
      if (isProperlyAdded())
         setArmThread(action);
      else
         mArmAnimation.action = action;
   }
*/
   //these need to be guaranteed!
   if (stream->readFlag())
   {
      mMoveModifier = float(stream->readInt(16))/100.f;
      mLight.mRadius = float( stream->readInt(16))/100.f;
      stream->read(&mFlyingMod);
      stream->read(&mRealm);
      stream->read(&mEncounterSetting);
      stream->read(&mAllianceLeader);
      mGuildName = stream->readSTString();
      //mScaleModifier = float(stream->readInt(16))/100.f;

      U32 i;
      for (i=0;i<SkinMax;i++)
      {
         setSkin((SkinType)i,stream->readSTString());
      }

   }


   if (stream->readFlag())
   {
      Point3F pos;
      
      stream->read(&pos.x);
      stream->read(&pos.y);
      stream->read(&pos.z);
      stream->read(&mRot.z);

      stream->setCompressionPoint(pos);

      setPosition(pos,mRot);
   }
// End MMO Kit

   // AFX CODE BLOCK (misc) <<
   afx_unpackUpdate(con, stream);
   // AFX CODE BLOCK (misc) >>

   // controlled by the client?
   if(stream->readFlag())
      return;

   if (stream->readFlag()) {
      mPredictionCount = sMaxPredictionTicks;
      mFalling = stream->readFlag();
      mSwimming = stream->readFlag(); // <-- MMO Kit
      

      ActionState actionState = (ActionState)stream->readInt(NumStateBits);
      if (stream->readFlag()) {
         mRecoverTicks = stream->readInt(rpgPlayerData::RecoverDelayBits);
         setState(actionState, mRecoverTicks);
      }
      else
         setState(actionState);

      Point3F pos,rot;
      stream->readCompressedPoint(&pos);
      F32 speed = mVelocity.len();
      if(stream->readFlag())
      {
         stream->readNormalVector(&mVelocity, 10);
         mVelocity *= stream->readInt(13) / 32.0f;
      }
      else
      {
         mVelocity.set(0.0f, 0.0f, 0.0f);
      }
      
      rot.y = rot.x = 0.0f;
      rot.z = stream->readFloat(7) * M_2PI_F;
      mHead.x = stream->readSignedFloat(6) * mDataBlock->maxLookAngle;
      mHead.z = stream->readSignedFloat(6) * mDataBlock->maxLookAngle;
      delta.move.unpack(stream);

      delta.head = mHead;
      delta.headVec.set(0.0f, 0.0f, 0.0f);

      if (stream->readFlag() && isProperlyAdded())
      {
         // Determin number of ticks to warp based on the average
         // of the client and server velocities.
         delta.warpOffset = pos - delta.pos;
         F32 as = (speed + mVelocity.len()) * 0.5f * TickSec;
         F32 dt = (as > 0.00001f) ? delta.warpOffset.len() / as: sMaxWarpTicks;
         delta.warpTicks = (S32)((dt > sMinWarpTicks) ? getMax(mFloor(dt + 0.5f), 1.0f) : 0.0f);

         if (delta.warpTicks) {
            // Setup the warp to start on the next tick.
            if (delta.warpTicks > sMaxWarpTicks)
               delta.warpTicks = sMaxWarpTicks;
            delta.warpOffset /= delta.warpTicks;

            delta.rotOffset = rot - delta.rot;
            if(delta.rotOffset.z < - M_PI)
               delta.rotOffset.z += M_2PI;
            else if(delta.rotOffset.z > M_PI)
               delta.rotOffset.z -= M_2PI;
            delta.rotOffset /= delta.warpTicks;
         }
         else {
            // Going to skip the warp, server and client are real close.
            // Adjust the frame interpolation to move smoothly to the
            // new position within the current tick.
            if (delta.dt == 0.0f) {
               delta.posVec.set(0.0f, 0.0f, 0.0f);
               delta.rotVec.set(0.0f, 0.0f, 0.0f);
            }
            else {
               Point3F cp = delta.pos + delta.posVec * delta.dt;
               F32 dti = 1.0f / delta.dt;
               delta.posVec = (cp - pos) * dti;
               delta.rotVec.z = mRot.z - rot.z;

               if(delta.rotVec.z > M_PI_F)
                  delta.rotVec.z -= M_2PI_F;
               else if(delta.rotVec.z < -M_PI_F)
                  delta.rotVec.z += M_2PI_F;

               delta.rotVec.z *= dti;
            }
            delta.pos = pos;
            delta.rot = rot;
            setPosition(pos,rot);
         }
      }
      else {
         // Set the player to the server position
         delta.pos = pos;
         delta.rot = rot;
         delta.posVec.set(0.0f, 0.0f, 0.0f);
         delta.rotVec.set(0.0f, 0.0f, 0.0f);
         delta.warpTicks = 0;
         delta.dt = 0.0f;
         setPosition(pos,rot);
      }
   }
   F32 energy = stream->readFloat(EnergyLevelBits) * mDataBlock->maxEnergy;
   setEnergyLevel(energy);
#endif // #ifdef DARREN_MMO

}


//----------------------------------------------------------------------------
ConsoleMethod( rpgPlayer, getState, const char*, 2, 2, "Return the current state name.")
{
   return object->getStateName();
}

ConsoleMethod( rpgPlayer, getDamageLocation, const char*, 3, 3, "(Point3F pos)")
{
   const char *buffer1;
   const char *buffer2;
   char *buff = Con::getReturnBuffer(128);

   Point3F pos(0.0f, 0.0f, 0.0f);
   dSscanf(argv[2], "%g %g %g", &pos.x, &pos.y, &pos.z);
   object->getDamageLocation(pos, buffer1, buffer2);

   dSprintf(buff, 128, "%s %s", buffer1, buffer2);
   return buff;
}

/* Original TGE Code:
ConsoleMethod( rpgPlayer, setArmThread, bool, 3, 3, "(string sequenceName)")
{
   return object->setArmThread(argv[2]);
}

ConsoleMethod( rpgPlayer, setActionThread, bool, 3, 5, "(string sequenceName, bool hold, bool fsp)")
{
   bool hold = (argc > 3)? dAtob(argv[3]): false;
   bool fsp  = (argc > 4)? dAtob(argv[4]): true;
   return object->setActionThread(argv[2],hold,true,fsp);
}
*/

// Begin MMO Kit
ConsoleMethod( rpgPlayer, setMoveModifier, void, 3, 3, "(ShapeBase obj)")
{
   object->setMoveModifier(dAtof(argv[2]));
}

ConsoleMethod(rpgPlayer, setEncounterSetting, void, 3, 3, "(ShapeBase obj)")
{
   object->setEncounterSetting(dAtoi(argv[2]));
}

ConsoleMethod(rpgPlayer, setAllianceLeader, void, 3, 3, "(ShapeBase obj)")
{
   object->setAllianceLeader(dAtoi(argv[2]));
}

ConsoleMethod(rpgPlayer, setGuildName, void, 3, 3, "(ShapeBase obj)")
{
   object->setGuildName(argv[2]);
}

ConsoleMethod( rpgPlayer, setFlyingMod, void, 3, 3, "(ShapeBase obj)")
{
   object->setFlyingMod(dAtof(argv[2]));
}


ConsoleMethod( rpgPlayer, setLightRadius, void, 3, 3, "(ShapeBase obj)")
{
   object->setLightRadius(dAtof(argv[2]));
}


ConsoleMethod( rpgPlayer, setScaleModifier, void, 3, 3, "(ShapeBase obj)")
{
   object->setScaleModifier(dAtof(argv[2]));
}
// End MMO Kit

ConsoleMethod( rpgPlayer, setControlObject, bool, 3, 3, "(ShapeBase obj)")
{
   ShapeBase* controlObject;
   if (Sim::findObject(argv[2],controlObject)) {
      object->setControlObject(controlObject);
      return true;
   }
   else
      object->setControlObject(0);
   return false;
}

ConsoleMethod( rpgPlayer, getControlObject, F32, 2, 2, "Get the current control object.")
{
   ShapeBase* controlObject = object->getControlObject();
   return controlObject? controlObject->getId(): 0;
}

ConsoleMethod( rpgPlayer, clearControlObject, void, 2, 2, "")
{
   object->setControlObject(0);
}

ConsoleMethod( rpgPlayer, checkDismountPoint, bool, 4, 4, "(Point3F oldPos, Point3F pos)")
{
   Point3F oldPos(0.0f, 0.0f, 0.0f);
   Point3F pos(0.0f, 0.0f, 0.0f);
   dSscanf(argv[2], "%g %g %g",
           &oldPos.x,
           &oldPos.y,
           &oldPos.z);
   dSscanf(argv[3], "%g %g %g",
           &pos.x,
           &pos.y,
           &pos.z);
   MatrixF oldPosMat(true);
   oldPosMat.setColumn(3, oldPos);
   MatrixF posMat(true);
   posMat.setColumn(3, pos);
   return object->checkDismountPosition(oldPosMat, posMat);
}

//----------------------------------------------------------------------------
void rpgPlayer::consoleInit()
{
   Con::addVariable("pref::rpgPlayer::renderMyPlayer",TypeBool, &sRenderMyPlayer);
   Con::addVariable("pref::rpgPlayer::renderMyItems",TypeBool, &sRenderMyItems);

   Con::addVariable("rpgPlayer::minWarpTicks",TypeF32,&sMinWarpTicks);
   Con::addVariable("rpgPlayer::maxWarpTicks",TypeS32,&sMaxWarpTicks);
   Con::addVariable("rpgPlayer::maxPredictionTicks",TypeS32,&sMaxPredictionTicks);

   // AFX CODE BLOCK (misc) <<
   afx_consoleInit();
   // AFX CODE BLOCK (misc) >>
}

//--------------------------------------------------------------------------
void rpgPlayer::calcClassRenderData()
{
   Parent::calcClassRenderData();

/* Orignal TGE Code:
   disableCollision();
   MatrixF nmat;
   MatrixF smat;
   Parent::getRetractionTransform(0,&nmat);
   Parent::getImageTransform(0, &smat);

   // See if we are pushed into a wall...
   Point3F start, end;
   smat.getColumn(3, &start);
   nmat.getColumn(3, &end);

   RayInfo rinfo;
   if (getContainer()->castRay(start, end, 0xFFFFFFFF & ~(WaterObjectType|DefaultObjectType), &rinfo)) {
      if (rinfo.t < 1.0f)
         mWeaponBackFraction = 1.0f - rinfo.t;
      else
         mWeaponBackFraction = 0.0f;
   } else {
      mWeaponBackFraction = 0.0f;
   }
   enableCollision();
*/
}




// Begin MMO Kit
void rpgPlayer::setLightRadius(F32 r)
{
   if (isServerObject())
   {
      if (r != mLight.mRadius)
         setMaskBits(RpgMask);
   }

   mLight.mRadius = r;   
}

void rpgPlayer::setFlyingMod(F32 mod)
{
   if (isServerObject())
   {
      if (mod != mFlyingMod)
         setMaskBits(RpgMask);
   }

   mFlyingMod = mod;
}


void rpgPlayer::setMoveModifier(F32 mod)
{
   if (isServerObject())
   {
      if (mod != mMoveModifier)
         setMaskBits(RpgMask);
   }

   mMoveModifier = mod;
}


void rpgPlayer::setEncounterSetting(S8 encounterSetting)
{
   if(isServerObject())
   {
      if(encounterSetting != mEncounterSetting)
         setMaskBits(RpgMask);
   }

   mEncounterSetting = encounterSetting;
}


void rpgPlayer::setAllianceLeader(S32 leader)
{
   if(isServerObject())
   {
      if(leader != mAllianceLeader)
         setMaskBits(RpgMask);
   }

   mAllianceLeader = leader;
}


void rpgPlayer::setGuildName(StringTableEntry guild)
{
   if(isServerObject())
   {
      if(dStrcmp(guild,mGuildName))
         setMaskBits(RpgMask);
   }

   mGuildName = guild;
}


void rpgPlayer::setScaleModifier(F32 mod)
{
   if (mod != 1.0f)
      mod = mod;

   if (isServerObject())
   {
      if (mod != mScaleModifier)
      {
         Point3F scale = getScale();
         scale *= 1.0f / mScaleModifier; //undo
         scale *= mod;
         setScale(scale);

         mScaleModifier = mod;
         //onNewDataBlock(mDataBlock);
         //setMaskBits(RpgMask);
      }

   }

   
   
}
// End MMO Kit

void rpgPlayer::renderMountedImage(SceneState* state, ShapeImageRenderImage* rimage)
{
   AssertFatal(rimage->mSBase == this, "Error, wrong image");
   GameConnection *con = GameConnection::getConnectionToServer();
// Begin MMO Kit
/* Original TGE Code:
   bool renderMounts = true;
   if(con && con->getControlObject() == this && con->isFirstPerson())
      renderMounts = sRenderMyItems;
   if (renderMounts == false)
      return;
*/
   if(con && con->getControlObject() == this && con->isFirstPerson())
      return;

   if (mDataBlock->mountPointNode[rimage->mIndex]==-1)
      return;
// End MMO Kit

   bool fogExemption = false;
   if(con && con->getControlObject() == this && con->isFirstPerson() == true)
      fogExemption = true;
   F32 fogAmount = 0.0f;
   if (fogExemption == false)
   {
      Point3F cameraOffset;
      getRenderTransform().getColumn(3,&cameraOffset);
      cameraOffset -= state->getCameraPosition();
      F32 dist = cameraOffset.len();
      fogAmount = state->getHazeAndFog(dist,cameraOffset.z);
   }

   // Mounted items
   PROFILE_START(PlayerRenderMounted);
   MountedImage& image = *getImageStruct(rimage->mIndex);
   if (image.dataBlock && image.shapeInstance && DetailManager::selectCurrentDetail(image.shapeInstance)) {
      MatrixF mat;
      getRenderImageTransform(rimage->mIndex, &mat);
      glPushMatrix();

      if (rimage->mIndex == 0 && mWeaponBackFraction != 0.0f && getDamageState() == Enabled) {
         MatrixF nmat;
         MatrixF smat;
         Parent::getRenderMuzzleTransform(0,&nmat);
         Parent::getRenderImageTransform(0,&smat);

         // See if we are pushed into a wall...
         Point3F start, end;
         smat.getColumn(3, &start);
         nmat.getColumn(3, &end);

         Point3F displace = (start - end) * mWeaponBackFraction;
         glTranslatef(displace.x, displace.y, displace.z);
      }
      dglMultMatrix(&mat);

// Begin MMO Kit
/* Original TGE Code:
      if (image.dataBlock->cloakable && mCloakLevel != 0.0f)
         image.shapeInstance->setAlphaAlways(0.04f + (1.0f - mCloakLevel) * 0.96f);
      else
         image.shapeInstance->setAlphaAlways(1.0f);
*/
      image.shapeInstance->setAlphaAlways(1.0f);

/* Original TGE Code:
      if (image.dataBlock->cloakable && mCloakLevel == 0.0f &&
          (image.dataBlock->emap && gRenderEnvMaps) &&
          state->getEnvironmentMap().getGLName() != 0) {
         image.shapeInstance->setEnvironmentMap(state->getEnvironmentMap());
         image.shapeInstance->setEnvironmentMapOn(true, 1.0f);
      } else {
         image.shapeInstance->setEnvironmentMapOn(false, 1.0f);
      }
*/

      image.shapeInstance->setEnvironmentMapOn(false, 1.0);

      TSMesh::setOverrideFade(1.0f - mCloakLevel);
// End MMO Kit
      image.shapeInstance->setupFog(fogAmount,state->getFogColor());
      image.shapeInstance->animate();
      image.shapeInstance->render();
      TSMesh::setOverrideFade(1.f); // <-- MMO Kit

      // easiest just to shut it off here.  If we're cloaked on the next frame,
      //  we don't want envmaps...
      image.shapeInstance->setEnvironmentMapOn(false, 1.0f);

      glPopMatrix();
   }
   PROFILE_END();
}


void rpgPlayer::renderImage(SceneState* state, SceneRenderImage* image)
{
   glMatrixMode(GL_MODELVIEW);

   // Base shape
   F32 fogAmount = 0.0f;
   F32 dist = 0.0f;
   PROFILE_START(PlayerRenderPrimary);

   GameConnection *con = GameConnection::getConnectionToServer();

   // Decide whether we are going to render the player shape or not
   bool renderPlayer = true;
   bool renderMounts = true;
   if (con && con->getControlObject() == this && con->isFirstPerson()) {
      renderPlayer = mDataBlock->renderFirstPerson;

      // Options that let the client turn off (but not on) rendering
      if (!sRenderMyPlayer)
         renderPlayer = false;
      if (!sRenderMyItems)
         renderMounts = false;
   }

   //
   if (mShapeInstance && renderPlayer && DetailManager::selectCurrentDetail(mShapeInstance)) {
      glPushMatrix();
      dglMultMatrix(&getRenderTransform());
      glScalef(mObjScale.x,mObjScale.y,mObjScale.z);

// Begin MMO Kit
/* Original TGE Code:
      if (mCloakLevel != 0.0f) {
         glMatrixMode(GL_TEXTURE);
         glPushMatrix();

         static U32 shiftX = 0;
         static U32 shiftY = 0;

         shiftX = (shiftX + 1) % 128;
         shiftY = (shiftY + 1) % 127;
         glTranslatef(F32(shiftX) / 127.0f, F32(shiftY)/126.0f, 0.0f);
         glMatrixMode(GL_MODELVIEW);

         mShapeInstance->smRenderData.renderDecals = false;
         mShapeInstance->setAlphaAlways(0.04f + (1.0f - mCloakLevel) * 0.96f);
         mShapeInstance->setOverrideTexture(mCloakTexture);
      }
      else {
         mShapeInstance->setAlphaAlways(1.0f);
      }
*/
         mShapeInstance->setAlphaAlways(1.0f);
// End MMO Kit

/* Original TGE Code:
      if (mCloakLevel == 0.0f && (mDataBlock->emap && gRenderEnvMaps) && state->getEnvironmentMap().getGLName() != 0) {
         mShapeInstance->setEnvironmentMap(state->getEnvironmentMap());
         mShapeInstance->setEnvironmentMapOn(true, 1.0f);
      } else {
         mShapeInstance->setEnvironmentMapOn(false, 1.0f);
      }
*/

      mShapeInstance->setEnvironmentMapOn(false, 1.0f); // <-- MMO Kit

      Point3F cameraOffset;
      getRenderTransform().getColumn(3,&cameraOffset);
      cameraOffset -= state->getCameraPosition();
      dist = cameraOffset.len();

      bool fogExemption = false;
      GameConnection *con = GameConnection::getConnectionToServer();
      if(con && con->getControlObject() == this && con->isFirstPerson() == true)
         fogExemption = true;
      fogAmount = fogExemption ? 0.0f : state->getHazeAndFog(dist,cameraOffset.z);

/* Original TGE Code:
      if( mCloakLevel > 0.0f )
      {
         fogAmount = 0.0f;
      }
*/

// Begin MMO Kit
/* Original TGE Code:
      TSMesh::setOverrideFade( mFadeVal );
*/
      TSMesh::setOverrideFade( 1.0f - mCloakLevel );
// End MMO Kit

      mShapeInstance->setupFog(fogAmount,state->getFogColor());
      mShapeInstance->animate();
      mShapeInstance->render();

      TSMesh::setOverrideFade( 1.0f );

      mShapeInstance->setEnvironmentMapOn(false, 1.0f);

/* Original TGE Code:
      if (mCloakLevel != 0.0f) {
         glMatrixMode(GL_TEXTURE);
         glPopMatrix();

         mShapeInstance->clearOverrideTexture();
         mShapeInstance->smRenderData.renderDecals = true;
      }
*/

      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
   }
   PROFILE_END();

   // draw the shadow...
   // only render shadow if 1) we have one, 2) we are close enough to be visible
   // 3) we are not the very last visible detail level (stop rendering shadow a little before shape)

   PROFILE_START(PlayerRenderShadow);
   TSMesh::setOverrideFade( mFadeVal );

   if (dist < 30.f) // <-- MMO Kit
   if (mShapeInstance && renderPlayer && mCloakLevel == 0.0f &&
       mMount.object == NULL && image->isTranslucent == true)
   {
      renderShadow(dist,fogAmount);
   }

   TSMesh::setOverrideFade( 1.0f );
   PROFILE_END();

   dglSetCanonicalState();

   // Debugging Bounding Box
   if (!mShapeInstance || gShowBoundingBox) {
      Point3F box;
      glPushMatrix();
      box = (mWorkingQueryBox.min + mWorkingQueryBox.max) * 0.5f;
      glTranslatef(box.x,box.y,box.z);
      box = (mWorkingQueryBox.max - mWorkingQueryBox.min) * 0.5f;
      glScalef(box.x,box.y,box.z);
      glColor3f(1.0f, 1.0f, 0.0f);
      wireCube(Point3F(1.0f, 1.0f, 1.0f),Point3F(0.0f, 0.0f, 0.0f));
      glPopMatrix();

      Box3F convexBox = mConvex.getBoundingBox(getRenderTransform(), getScale());
      glPushMatrix();
      box = (convexBox.min + convexBox.max) * 0.5f;
      glTranslatef(box.x,box.y,box.z);
      box = (convexBox.max - convexBox.min) * 0.5f;
      glScalef(box.x,box.y,box.z);
      glColor3f(1.0f, 1.0f, 1.0f);
      wireCube(Point3F(1.0f, 1.0f, 1.0f),Point3F(0.0f, 0.0f, 0.0f));
      glPopMatrix();

      glEnable(GL_DEPTH_TEST);
   }
}

void rpgPlayer::playFootstepSound(bool triggeredLeft, S32 sound)
{
   MatrixF footMat = getTransform();

   if (mWaterCoverage == 0.0f) {
      switch (sound) {
         case 0: // Soft
            alxPlay(mDataBlock->sound[rpgPlayerData::FootSoft], &footMat);
            break;
         case 1: // Hard
            alxPlay(mDataBlock->sound[rpgPlayerData::FootHard], &footMat);
            break;
         case 2: // Metal
            alxPlay(mDataBlock->sound[rpgPlayerData::FootMetal], &footMat);
         break;
         case 3: // Snow
            alxPlay(mDataBlock->sound[rpgPlayerData::FootSnow], &footMat);
            break;
         default: //Hard
            alxPlay(mDataBlock->sound[rpgPlayerData::FootHard], &footMat);
         break;
      }
   }
   else {
      if (mWaterCoverage < mDataBlock->footSplashHeight)
         alxPlay(mDataBlock->sound[rpgPlayerData::FootShallowSplash], &footMat);
      else
         if (mWaterCoverage < 1.0f)
            alxPlay(mDataBlock->sound[rpgPlayerData::FootWading], &footMat);
         else
            if(triggeredLeft) {
               alxPlay(mDataBlock->sound[rpgPlayerData::FootUnderWater], &footMat);
               alxPlay(mDataBlock->sound[rpgPlayerData::FootBubbles], &footMat);
            }
   }
}

void rpgPlayer:: playImpactSound()
{
   if(mWaterCoverage == 0.0f)
   {
      Point3F pos;
      RayInfo rInfo;
      MatrixF mat = getTransform();
// Begin MMO Kit
/* Original TGE Code:
      mat.mulP(Point3F(mDataBlock->decalOffset,0.0f,0.0f), &pos);
*/
      mat.mulP(Point3F(0.0f,0.0f,0.0f), &pos);
// End MMO Kit
      if(gClientContainer.castRay(Point3F(pos.x, pos.y, pos.z + 0.01f),
                                  Point3F(pos.x, pos.y, pos.z - 2.0f ), TerrainObjectType | InteriorObjectType | VehicleObjectType, &rInfo))
      {
         S32 sound = -1;
         if( rInfo.object->getTypeMask() & TerrainObjectType)
         {
            TerrainBlock* tBlock = static_cast<TerrainBlock*>(rInfo.object);
            S32 mapIndex = tBlock->mMPMIndex[0];
            if (mapIndex != -1) {
               MaterialPropertyMap* pMatMap = static_cast<MaterialPropertyMap*>(Sim::findObject("MaterialPropertyMap"));
               const MaterialPropertyMap::MapEntry* pEntry = pMatMap->getMapEntryFromIndex(mapIndex);
               if(pEntry)
                  sound = pEntry->sound;
            }
         }
         else if( rInfo.object->getTypeMask() & VehicleObjectType)
            sound = 2; // Play metal sound

         switch(sound) {
            case 0:
               //Soft
               alxPlay(mDataBlock->sound[rpgPlayerData::ImpactSoft], &getTransform());
            break;
            case 1:
               //Hard
               alxPlay(mDataBlock->sound[rpgPlayerData::ImpactHard], &getTransform());
            break;
            case 2:
               //Metal
               alxPlay(mDataBlock->sound[rpgPlayerData::ImpactMetal], &getTransform());
            break;
            case 3:
               //Snow
               alxPlay(mDataBlock->sound[rpgPlayerData::ImpactSnow], &getTransform());
            break;
            default:
               //Hard
               alxPlay(mDataBlock->sound[rpgPlayerData::ImpactHard], &getTransform());
            break;
         }
      }
   }
   mImpactSound = 0;
}

//--------------------------------------------------------------------------
// Update splash
//--------------------------------------------------------------------------

void rpgPlayer::updateSplash()
{
   F32 speed = getVelocity().len();
   if( speed < mDataBlock->splashVelocity || isMounted() ) return;

   Point3F curPos = getPosition();

   if ( curPos.equal( mLastPos ) )
      return;

   if (pointInWater( curPos )) {
      if (!pointInWater( mLastPos )) {
         Point3F norm = getVelocity();
         norm.normalize();

         // make sure player is moving vertically at good pace before playing splash
         F32 splashAng = mDataBlock->splashAngle / 360.0f;
         if( mDot( norm, Point3F(0.0f, 0.0f, -1.0f) ) < splashAng )
            return;

         RayInfo rInfo;
         if (gClientContainer.castRay(mLastPos, curPos,
               WaterObjectType, &rInfo)) {
            createSplash( rInfo.point, speed );
            mBubbleEmitterTime = 0.0f;
         }
      }
   }
}


//--------------------------------------------------------------------------

void rpgPlayer::updateFroth( F32 dt )
{
   // update bubbles
   Point3F moveDir = getVelocity();
   mBubbleEmitterTime += dt;

   if (mBubbleEmitterTime < mDataBlock->bubbleEmitTime) {
      if (mSplashEmitter[rpgPlayerData::BUBBLE_EMITTER]) {
         Point3F emissionPoint = getRenderPosition();
         U32 emitNum = rpgPlayerData::BUBBLE_EMITTER;
         mSplashEmitter[emitNum]->emitParticles(mLastPos, emissionPoint,
            Point3F( 0.0f, 0.0f, 1.0f ), moveDir, (U32)(dt * 1000.0f));
      }
   }

   Point3F contactPoint;
// Begin MMO Kit
/* Original TGE Code:
   if (!collidingWithWater(contactPoint)) {
*/
   if (!collidingWithWater(contactPoint) || mSwimming || mWaterCoverage > 0.4f) {
      mLastWaterPos = mLastPos;
      return;
   }
// End MMO Kit

   F32 speed = moveDir.len();
   if( speed < mDataBlock->splashVelEpsilon )
      speed = 0.0f;
   U32 emitRate = (U32) (speed * mDataBlock->splashFreqMod * dt);

   U32 i;
   for ( i=0; i<rpgPlayerData::BUBBLE_EMITTER; i++ ) {
      if (mSplashEmitter[i] )
         mSplashEmitter[i]->emitParticles( mLastWaterPos,
            contactPoint, Point3F( 0.0f, 0.0f, 1.0f ),
            moveDir, emitRate );
   }
   mLastWaterPos = contactPoint;
}

void rpgPlayer::updateWaterSounds(F32 dt)
{
#ifdef DARREN_MMO

   if (mWaterCoverage >= 1.0f && mDamageState == Enabled) {
      // We're under water and still alive, so let's play something
      if (mVelocity.len() > 1.0f) {
         if (!mMoveBubbleHandle)
            mMoveBubbleHandle = alxPlay(mDataBlock->sound[rpgPlayerData::MoveBubbles], &getTransform());
         alxSourceMatrixF(mMoveBubbleHandle, &getTransform());
      }
      else
         if (mMoveBubbleHandle) {
         alxStop(mMoveBubbleHandle);
         mMoveBubbleHandle = 0;
      }

      if (!mWaterBreathHandle)
         mWaterBreathHandle = alxPlay(mDataBlock->sound[rpgPlayerData::WaterBreath], &getTransform());
      alxSourceMatrixF(mWaterBreathHandle, &getTransform());
   }
   else {
      // Stop everything
      if (mMoveBubbleHandle) {
         alxStop(mMoveBubbleHandle);
         mMoveBubbleHandle = 0;
      }
      if (mWaterBreathHandle) {
         alxStop(mWaterBreathHandle);
         mWaterBreathHandle = 0;
      }
   }
#endif // #ifdef DARREN_MMO

}


//--------------------------------------------------------------------------
// Returns true if player is intersecting a water surface
//--------------------------------------------------------------------------
bool rpgPlayer::collidingWithWater( Point3F &waterHeight )
{
   Point3F curPos = getPosition();

   F32 height = mFabs( mObjBox.max.z - mObjBox.min.z );

   RayInfo rInfo;
   if( gClientContainer.castRay( curPos + Point3F(0.0f, 0.0f, height), curPos, WaterObjectType, &rInfo) )
   {
      WaterBlock* pBlock = dynamic_cast<WaterBlock*>(rInfo.object);

      if( !pBlock )
         return false;

      if( !pBlock->isWater( pBlock->getLiquidType() ))
         return false;

      waterHeight = rInfo.point;
      return true;
   }

   return false;
}

//--------------------------------------------------------------------------
bool rpgPlayer::pointInWater( Point3F &point )
{
   SimpleQueryList sql;
   if (isServerObject())
      gServerSceneGraph->getWaterObjectList(sql);
   else
      gClientSceneGraph->getWaterObjectList(sql);

   for (U32 i = 0; i < sql.mList.size(); i++)
   {
      WaterBlock* pBlock = dynamic_cast<WaterBlock*>(sql.mList[i]);

      if (pBlock && pBlock->isWater( pBlock->getLiquidType() ))
      {
         if (pBlock->isPointSubmergedSimple( point ))
            return true;
      }

   }

   return false;
}

//--------------------------------------------------------------------------
void rpgPlayer::createSplash( Point3F &pos, F32 speed )
{
   if(speed >= mDataBlock->hardSplashSoundVel)
      alxPlay(mDataBlock->sound[rpgPlayerData::ImpactWaterHard], &getTransform());
   else if(speed >= mDataBlock->medSplashSoundVel)
      alxPlay(mDataBlock->sound[rpgPlayerData::ImpactWaterMedium], &getTransform());
   else
      alxPlay(mDataBlock->sound[rpgPlayerData::ImpactWaterEasy], &getTransform());

   if( mDataBlock->splash )
   {
      MatrixF trans = getTransform();
      trans.setPosition( pos );
      Splash *splash = new Splash;
      splash->onNewDataBlock( mDataBlock->splash );
      splash->setTransform( trans );
      splash->setInitialState( trans.getPosition(), Point3F( 0.0f, 0.0f, 1.0f ) );
      if (!splash->registerObject())
         delete splash;
   }
}

bool rpgPlayer::isControlObject()
{
   GameConnection* connection = GameConnection::getConnectionToServer();
   if( !connection ) return false;
   ShapeBase *obj = connection->getControlObject();
   return ( obj == this );
}

// Begin MMO Kit

//skinning

//head, arms, legs, feet, body, hands (only valid for player models)
//otherwise, it's a full body skin (monsters) (use folder in this case) (reskin function already exists!!)

/*   enum SkinType {
      SkinHead,
      SkinArms,
      SkinLegs,
      SkinFeet,
      SkinHands,
      SkinBody,
      SkinSingle, //for monsters with one texture
      SkinMax
   };
*/

void rpgPlayer::setSkin(SkinType skType, StringTableEntry texture)
{
   char* materials[] = {"head","arms","legs","feet","hands","body","single"};
   
   char* material = NULL;
   char* nmaterial = NULL;

   if (isGhost())
   {
      //if (mSkins[skType]==texture)
      //   return;

      material = materials[skType];
      
      mSkins[skType]=texture;

      if (!mShapeInstance->ownMaterialList())
         mShapeInstance->cloneMaterialList();

      // Cycle through the materials.
      TSMaterialList* pMatList = mShapeInstance->getMaterialList();
      for (S32 j = 0; j < pMatList->mMaterialNames.size(); j++) {
         // Get the name of this material.
         const char* pName = pMatList->mMaterialNames[j];
         
         /*
         //hack for spider
         if (!dStricmp("base.spider",pName))
         {
            pMatList->mMaterials[j]=TextureHandle(texture, MeshTexture, false);
         }
         */

         
         
         if ((dStrstr(pName,"special") || dStrstr(pName,"wings")) && skType == 6)
         {
            if (pMatList->mMaterials[j] && pMatList->mMaterials[j].getName())
               if (!dStrcmp(pMatList->mMaterials[j].getName(),texture))
               {
                  return;
               }

             pMatList->mMaterials[j]=TextureHandle(texture, MeshTexture, false);
         }
         else if (dStrstr(pName,material) || (skType==6 && (texture && dStrlen(texture) && pMatList->mMaterialNames.size()==1 )))
         {
            if (pMatList->mMaterials[j] && pMatList->mMaterials[j].getName())
               if (!dStrcmp(pMatList->mMaterials[j].getName(),texture))
               {
                  return;
               }

            pMatList->mMaterials[j]=TextureHandle(texture, MeshTexture, false);

         }

      }

   }
   else
   {
      //server side
      if (mSkins[skType]!=texture)
      {
         mSkins[skType]=texture;
         setMaskBits(RpgMask);
      }

   }
  
}


ConsoleMethod( rpgPlayer, setSkin, void, 4, 4, "setSkin(SkinType,texture)")
{

  if (object->isGhost())
      return; //only valid server side

   S32 skinType = dAtoi(argv[2]);
   //dSscanf(argv[2], "%d", &skinType);

   object->setSkin((rpgPlayer::SkinType) skinType,StringTable->insert(argv[3]));
      
}

ConsoleMethod( rpgPlayer, isSwimming, bool, 2, 2, "bool isSwimming()")
{

  if (object->isGhost())
      return false; //only valid server side

   return object->getWaterCoverage() > 0.5f;
      
}


ConsoleMethod( rpgPlayer, isSimZombie, bool, 2, 2, "isSimZombie()")
{

   return object->mSimZombie;
 
}


ConsoleMethod( rpgPlayer, setZombie, void, 3, 3, "isZombie(b)")
{


   object->mZombie = dAtob(argv[2]);

}


ConsoleMethod( rpgPlayer, hasFlyingAnimation, bool, 2, 2, "bool hasFlyingAnimation()")
{

   if (object->isGhost())
      return false; //only valid server side

   return object->getShape()->findSequence("fly")!=-1;
}


ConsoleMethod( rpgPlayer, isFlying, bool, 2, 2, "bool isFlying()")
{

   if (object->isGhost())
      return false; //only valid server side

   return object->getContactTimer() > 46;

}

/* Original MMO Kit Code:
void rpgPlayer::registerLights(LightManager * lightManager, bool lightingScene)
{

   Parent::registerLights(lightManager,lightingScene);
   if (mLight.mRadius && !lightingScene)
   {
      mLight.mType = LightInfo::Point;
      mLight.mPos = getPosition();
      mLight.mPos+=Point3F(0.f,0.f,1.5f);
      mLight.mColor=ColorF(1.0,1.0,1.0,1.0);
      mLight.mAttachedId = getId();
      mLight.sgLightInfoData.sgAdvancedLightingModel=true;

      lightManager->addLight(&mLight);   
   }
}
*/

void rpgPlayer::stickHack(bool moves)
{
   if (moves)
   {
      mUnstickTimer = 5000.0f;
      return;
   }
   mUnstickTimer -= TickMs;
   if (mUnstickTimer <= 0.0f)
   {
      mUnstickTimer = 5000.0f;
      setTransform(getTransform());
   }
}
void rpgPlayer::setScale(const VectorF & scale)
{
	#ifdef DARREN_MMO

   if (!mDataBlock)
   {
      ShapeBase::setScale(scale);
      return;
   }
   VectorF nscale = scale;
   if (nscale.x > mDataBlock->scaleLimit)
      nscale.x = mDataBlock->scaleLimit;
   if (nscale.y > mDataBlock->scaleLimit)
      nscale.y = mDataBlock->scaleLimit;
   if (nscale.z > mDataBlock->scaleLimit)
      nscale.z = mDataBlock->scaleLimit;

   ShapeBase::setScale(nscale);
#endif // #ifdef DARREN_MMO
   
}

S32 rpgPlayer::castSpell(const char* spellname, rpgPlayer* tgt)
{
   // create a new spell instance
   afxMagicSpellData* spell_data = dynamic_cast<afxMagicSpellData*>(Sim::findObject(spellname));
   if (!spell_data)
      return -1;

   // cast_spell(afxMagicSpellData*, ShapeBase* caster, SceneObject* target)
   afxMagicSpell* spell = afxMagicSpell::cast_spell(spell_data, this, (tgt) ? tgt : this);
   return spell->getId();

/* Original MMO Kit Code:
   // create a new spell instance
   afxMagicSpellData* spell_data = dynamic_cast<afxMagicSpellData*>(Sim::findObject(spellname));
   if (!spell_data)
      return -1;
   afxMagicSpell* spell = new afxMagicSpell();
   

   spell->setDataBlock(spell_data);
   spell->caster = this;
   spell->target = this;
   if (tgt)
      spell->target=tgt;
   spell->caster_client = getControllingClient();
   if (!spell->registerObject())
   {
      Con::printf("afxMagicSpell: failed to register spell instance.");
      delete spell;
      return -1;
   }
   registerForCleanup(spell);
   spell->activate();

   return spell->getId();
*/
}

ConsoleMethod( rpgPlayer, castSpell,void, 3, 3, "")
{

   object->castSpell(argv[2]);
}

ConsoleFunction(LaunchAFXProjectile, S32, 4, 4, "LaunchAFXProjectile(srcId,tgtId,pdata)")
{
   rpgPlayer* psrc = dynamic_cast<rpgPlayer*>(Sim::findObject(argv[1]));
   rpgPlayer* pdst = dynamic_cast<rpgPlayer*>(Sim::findObject(argv[2]));

   if (!psrc || !pdst)
      return -1;

   return psrc->castSpell(argv[3],pdst);
}

bool rpgPlayer::allowHarmful(const rpgPlayer *opponent)
{
#ifdef DARREN_MMO

    if(!opponent)
        return false;
    
    // one of both opponents is a mob, only allow harmful between two AIs if not of same realm
    if(mTypeMask&AIObjectType && !mPlayerPet)
    {
        if(opponent->mTypeMask&AIObjectType && !opponent->mPlayerPet)
            if(mRealm == opponent->mRealm)
                return false;
        return true;
    }
    
    if(mAllianceLeader && mAllianceLeader == opponent->mAllianceLeader)
        return false;
    
    if(mEncounterSetting == RPG_ENCOUNTER_PVE || opponent->mEncounterSetting == RPG_ENCOUNTER_PVE)
        return false;
    
    if(mEncounterSetting == RPG_ENCOUNTER_RVR || opponent->mEncounterSetting == RPG_ENCOUNTER_RVR)
        if(mRealm == opponent->mRealm)
            return false;
    
    if(mEncounterSetting == RPG_ENCOUNTER_GVG || opponent->mEncounterSetting == RPG_ENCOUNTER_GVG)
        if(*mGuildName != 0 && !dStrcmp(mGuildName, opponent->mGuildName))
            return false;
#endif //#ifdef DARREN_MMO
    
    return true;
}

// End MMO Kit

// AFX CODE BLOCK (misc) <<

// static 
bool rpgPlayer::sCorpsesHiddenFromRayCast = true;

// static 
void rpgPlayer::afx_consoleInit()
{
  Con::addVariable("pref::rpgPlayer::corpsesHiddenFromRayCast", TypeBool, &sCorpsesHiddenFromRayCast);
}

void rpgPlayer::afx_init()
{
  anim_clip_flags = 0;
  last_anim_tag = 0;         
  last_anim_lock_tag = 0;
  overrideLookAnimation = false;
  armLookOverridePos = 0.5f;
  headVLookOverridePos = 0.5f;
  headHLookOverridePos = 0.5f;
}

U32 rpgPlayer::afx_packUpdate(NetConnection* con, U32 mask, BitStream* stream, U32 retMask)
{
  if (stream->writeFlag(mask & LookOverrideMask))
    stream->write(overrideLookAnimation);
  
  return retMask;
}

void rpgPlayer::afx_unpackUpdate(NetConnection* con, BitStream* stream)
{
  if (stream->readFlag()) // LookOverrideMask
    stream->read(&overrideLookAnimation);
}

// AFX CODE BLOCK (misc) >>

// AFX CODE BLOCK (anim-clip) <<

//
// Code for overriding player's animation with sequences selected by the
// anim-clip component effect.
//

U32 rpgPlayer::unique_anim_tag_counter = 1;
#define BAD_ANIM_ID  999999999

void rpgPlayer::restoreAnimation(U32 tag, bool is_death_anim)
{
  // check if this is a blended clip
  if ((tag & 0x80000000) != 0)
  {
    for (S32 i = 0; i < blend_clips.size(); i++)
    {
      if (blend_clips[i].tag == tag)
      {
        if (blend_clips[i].thread)
        {
          Con::printf("DESTROY THREAD %d of %d tag=%d" , i, blend_clips.size(), tag & 0x7fffffff);
          mShapeInstance->destroyThread(blend_clips[i].thread);
        }
        blend_clips.erase_fast(i);
        break;
      }
    }
    return;
  }

  if (tag != 0 && tag == last_anim_tag)
  {
    anim_clip_flags &= ~ANIM_OVERRIDDEN;

    if (isClientObject())
    {
      if (mDamageState != Enabled)
      {
        if (!is_death_anim)
        {
          // this is a bit hardwired and desperate,
          // but if he's dead he needs to look like it.
/* Original AFX Code:
          setActionThread("death10", false, false, false);
*/
        }
      }
      else if (mState != MoveState)
      {
        // not sure what happens here
      }
      else
      {
/* Original AFX Code:
        pickActionAnimation();
*/
      }
    }

    last_anim_tag = 0;
  }
}

U32 rpgPlayer::getAnimationID(const char* name)
{
/* Original AFX Code:
  for (U32 i = 0; i < mDataBlock->actionCount; i++)
  {
    rpgPlayerData::ActionAnimation &anim = mDataBlock->actionList[i];
    if (!dStricmp(anim.name, name))
      return i;
  }
*/
  Con::printf("BAD_ANIM_ID for %s", name);  
  return BAD_ANIM_ID;
}

U32 rpgPlayer::playAnimation(const char* name, F32 pos, F32 rate, F32 trans, bool hold, bool wait, bool is_death_anim)
{
  return playAnimation(getAnimationID(name), pos, rate, trans, hold, wait);
}

U32 rpgPlayer::playAnimation(U32 anim_id, F32 pos, F32 rate, F32 trans, bool hold, bool wait)
{
   return 0;
/* Original AFX Code:
  if (anim_id == BAD_ANIM_ID)
  {
    Con::printf("rpgPlayer::playAnimation() problem. BAD_ANIM_ID");
    return 0;
  }

  S32 seq_id = mDataBlock->actionList[anim_id].sequence;
  if (seq_id == -1)
  {
    Con::printf("rpgPlayer::playAnimation() problem. BAD_SEQ_ID");
    return 0;
  }

  if (mShapeInstance->getShape()->sequences[seq_id].isBlend())
  {
    BlendThread blend_clip; 
    blend_clip.tag = ((unique_anim_tag_counter++) | 0x80000000);
    blend_clip.thread = 0;

    if (isClientObject())
    {
      blend_clip.thread = mShapeInstance->addThread();
      mShapeInstance->setSequence(blend_clip.thread, anim_id, pos);
      mShapeInstance->setTimeScale(blend_clip.thread, rate);
    }

    blend_clips.push_back(blend_clip);

    return blend_clip.tag;
  }
    
  if (isClientObject())
  {
    rpgPlayerData::ActionAnimation &anim = mDataBlock->actionList[anim_id];
    if (anim.sequence != -1) 
    {
      mActionAnimation.action          = anim_id;
      mActionAnimation.forward         = (rate > 0);
      mActionAnimation.firstPerson     = false;
      mActionAnimation.holdAtEnd       = hold;
      mActionAnimation.waitForEnd      = hold? true: wait;
      mActionAnimation.animateOnServer = false;
      mActionAnimation.atEnd           = false;
      mActionAnimation.delayTicks      = (S32)sNewAnimationTickTime;
    
      F32 transTime = (trans < 0) ? sAnimationTransitionTime : trans;  
      
      mShapeInstance->setTimeScale(mActionAnimation.thread, rate);
      mShapeInstance->transitionToSequence(mActionAnimation.thread,anim.sequence,
                                           pos, transTime, true);
    }
  }

  anim_clip_flags |= ANIM_OVERRIDDEN;
  last_anim_tag = unique_anim_tag_counter++;

  return last_anim_tag;
*/
}

F32 rpgPlayer::getAnimationDuration(const char* name)
{
  return getAnimationDuration(getAnimationID(name));
}

F32 rpgPlayer::getAnimationDuration(U32 anim_id)
{
   return 0.0f;
/* Original AFX Code:
  if (anim_id == BAD_ANIM_ID)
    return 0.0f;
  S32 seq_id = mDataBlock->actionList[anim_id].sequence;
  if (seq_id >= 0 && seq_id < mDataBlock->shape->sequences.size())
    return mDataBlock->shape->sequences[seq_id].duration;
  else
    return 0.0f;
*/
}

bool rpgPlayer::isBlendAnimation(const char* name)
{
   return false;
/* Original AFX Code:
  U32 anim_id = getAnimationID(name);
  if (anim_id == BAD_ANIM_ID)
    return false;
  S32 seq_id = mDataBlock->actionList[anim_id].sequence;
  if (seq_id >= 0 && seq_id < mDataBlock->shape->sequences.size())
    return mDataBlock->shape->sequences[seq_id].isBlend();
  else
    return false;
*/
}

void rpgPlayer::unlockAnimation(U32 tag, bool force)
{
  if ((tag != 0 && tag == last_anim_lock_tag) || force)
    anim_clip_flags &= ~BLOCK_USER_CONTROL;
}

U32 rpgPlayer::lockAnimation()
{
  anim_clip_flags |= BLOCK_USER_CONTROL;
  last_anim_lock_tag = unique_anim_tag_counter++;

  return last_anim_lock_tag;
}

ConsoleMethod(rpgPlayer, isAnimationLocked, bool, 2, 2, "isAnimationLocked()")
{
  return object->isAnimationLocked();
}

// AFX CODE BLOCK (anim-clip) >>

// AFX CODE BLOCK (player-look) <<

void rpgPlayer::setLookAnimationOverride(bool flag) 
{ 
  overrideLookAnimation = flag; 
  setMaskBits(LookOverrideMask);
}

ConsoleMethod(rpgPlayer, setLookAnimationOverride, void, 3, 3, "setLookAnimationOverride(flag)")
{
  object->setLookAnimationOverride(dAtob(argv[2]));
}

// AFX CODE BLOCK (player-look) >>
