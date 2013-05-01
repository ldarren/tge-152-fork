
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _ARCANE_FX_H_
#define _ARCANE_FX_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "console/consoleTypes.h"
#include "console/simBase.h"
#include "core/bitStream.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
#if (TORQUE_GAME_ENGINE >= 1420)

#define AFX_TGE_1_5 
#define AFX_TLK_SUPPORT_ENABLED

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
#elif (TORQUE_GAME_ENGINE >= 1400)

#define AFX_TGE_1_4

// This is included here for TLK detection. 
#include "game/fx/fxLight.h"

#ifdef _SGLIGHTING_H_
#define AFX_TLK_SUPPORT_ENABLED
#define AFX_TLK_1_4_SUPPORT_ENABLED
#define sgLightObjectData sgUniversalStaticLightData
#define sgLightObject sgUniversalStaticLight
#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
#else

#define AFX_TGE_1_3

#define IMPLEMENT_CONSOLETYPE(T) 
#define DECLARE_CONSOLETYPE(T)

#define getConnectionToServer getServerConnection

enum afxConsoleTypes 
{
  TypeafxMagicSpellData = NumConsoleTypes,
  TypeAfxSpellBookDataPtr,
  TypeAfxZodiacDataPtr,
  TypeafxMagicMissileDataPtr,
  TypeAfxModelDataPtr,
  TypeAfxAnimClipDataPtr,
  TypeafxEffectWrapperDataPtr,
  TypeAfxLightDataPtr,
  TypeSimDataBlockPtr,
  TypeAfxScriptEventDataPtr,
  TypeafxXM_BaseDataPtr,
  TypeAfxCameraShakeDataPtr,
  TypeAfxAnimLockDataPtr,
  TypeAfxDamageDataPtr,
  TypeAfxPathDataPtr,
  TypeAfxParticleEmitterDataPtr,
  TypeafxEffectronData,
  TypeafxChoreographerData,
  TypeafxSelectronData,
  TypeafxMachineGunPtr,
  TypeafxMooringDataPtr,
  TypeByteRange,

  LastAfxConsoleType
};

#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxChoreographer;
class afxSelectronData;
class GameConnection;
class SceneObject;

class arcaneFX
{
private:
  static Vector<afxChoreographer*> active_choreographers;
  static Vector<afxChoreographer*> client_choreographers;
  static Vector<afxSelectronData*> selectrons;
  static Vector<SceneObject*>      scoped_objs;

public:
  static StringTableEntry   NULLSTRING;
  static U32				sDestinationMask; // DARREN MOD: mouse click movement
  static U32                sTargetSelectionMask;
  static F32                sTargetSelectionRange;
  static U32                sTargetSelectionTimeoutMS;
  static bool               sClickToTargetSelf;
  static U32                sMissileCollisionMask;
  static U32                master_choreographer_id;
  static U16                master_scope_id;
  static SceneObject*		pExempt;	// DARREN MOD: exempted object for mouse selection

public:
  static void init();
  static void shutdown();
  static void advanceTime(U32 delta);

  static U32  registerChoreographer(afxChoreographer*);
  static void unregisterChoreographer(afxChoreographer*);
  static void registerClientChoreographer(afxChoreographer*);
  static void unregisterClientChoreographer(afxChoreographer*);
  static afxChoreographer* findClientChoreographer(U32 id);

  static void registerSelectronData(afxSelectronData*);
  static void unregisterSelectronData(afxSelectronData*);
  static afxSelectronData* findSelectronData(U32 obj_type_mask, U8 code);

  static U16            generateScopeId();
  static void           registerScopedObject(SceneObject*);
  static SceneObject*   findScopedObject(U16 scope_id);
  static void           unregisterScopedObject(SceneObject*);

  static void syncToNewConnection(GameConnection* conn);
  static S32  rolloverRayCast(Point3F start, Point3F end, U32 mask, U32 dmask); //DARREN MOD: mouse click movement, added dmask
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class ByteRange
{
public:
  U8     low;
  U8     high;
  
public:
  /*C*/  ByteRange() { low = 0; high = 255; }
  /*C*/  ByteRange(U8 l, U8 h=255) { low = l; high = h; }
  
  void   set(U8 l, U8 h=255) { low = l; high = h; }
  bool   outOfRange(U8 v) { return (v < low || v > high); }
  bool   inRange(U8 v) { return !outOfRange(v); }
};

#ifdef AFX_TGE_1_3
const char *getDataTypeByteRange(void * dptr, EnumTable *, BitSet32 /*flag*/);
void setDataTypeByteRange(void *dptr, S32 argc, const char **argv, EnumTable *, BitSet32 /*flag*/);
#else
DefineConsoleType( TypeByteRange )
#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

inline void writeDatablockID(BitStream* s, SimObject* simobj, bool packed=false)
{
  if (s->writeFlag(simobj))
    s->writeRangedU32(packed ? SimObjectId(simobj) : simobj->getId(), 
                      DataBlockObjectIdFirst, DataBlockObjectIdLast);
}

inline S32 readDatablockID(BitStream* s)
{
  return (!s->readFlag()) ? 0 : ((S32)s->readRangedU32(DataBlockObjectIdFirst, 
          DataBlockObjectIdLast));
}

inline void registerForCleanup(SimObject* obj)
{
  SimGroup* cleanup_grp = dynamic_cast<SimGroup*>(Sim::findObject("MissionCleanup"));
  if (cleanup_grp)
    cleanup_grp->addObject(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

#define ST_NULLSTRING (arcaneFX::NULLSTRING)


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _ARCANE_FX_H_

