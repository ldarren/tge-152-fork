
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _ARCANE_FX_H_
#define _ARCANE_FX_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _TORQUECONFIG_H_
#include "core/torqueConfig.h"
#endif

#define AFX_VERSION_STRING "1.1"
#define AFX_VERSION         1.1 

// #define AFX_CUSTOMIZED_BRANCH

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
#if defined(AFX_CUSTOMIZED_BRANCH)

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
#elif (TORQUE_SHADER_ENGINE >= 1000)

#define TGEA_ENGINE
#define AFX_TGEA_1_0
#define GFX_GRAPHICS_LAYER
#define BROKEN_SPLASH_CODE

#define   AFX_CAP_LIGHT_EMITTER_LINKAGE
#define   AFX_CAP_ROLLOVER_RAYCASTS
#define   AFX_CAP_LIGHT_ILLUMINATION_MASKING
//#define AFX_CAP_TRANSP_INTERIOR_SURFS

// these additional AFX_CAP defines can be found in the engine code:
//     AFX_CAP_AFXMODEL_TYPE      (default:undefined)
//     AFX_CAP_DATABLOCK_CACHE    (default:defined)
//     AFX_CAP_PHYSICAL_ZONE_OPTS (default:defined)
//

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
#elif (TORQUE_GAME_ENGINE >= 1420)

#define TGE_ENGINE
#define AFX_TGE_1_5 
#define DGL_GRAPHICS_LAYER

#define     AFX_CAP_LIGHT_EMITTER_LINKAGE
#define     AFX_CAP_ROLLOVER_RAYCASTS
//#define   AFX_CAP_LIGHT_ILLUMINATION_MASKING
//#define   AFX_CAP_TRANSP_INTERIOR_SURFS

// these additional AFX_CAP defines can be found in the engine code:
//     AFX_CAP_AFXMODEL_TYPE      (default:undefined)
//     AFX_CAP_DATABLOCK_CACHE    (default:defined)
//     AFX_CAP_PHYSICAL_ZONE_OPTS (default:defined)
//

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
#else

// TGE VERSIONS OLDER THAN 1.4.2 NOT SUPPORTED

#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif
#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif
#ifndef _BITSTREAM_H_
#include "core/bitStream.h"
#endif
#ifndef _GAMEBASE_H_
#include "game/gameBase.h"
#endif

#if defined(DGL_GRAPHICS_LAYER)
#ifndef _DGL_H_
#include "dgl/dgl.h"
#endif
#endif


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
  static void endMissionNotify();
  static S32  rolloverRayCast(Point3F start, Point3F end, U32 mask, U32 dmask); //DARREN MOD: mouse click movement, added dmask

#if defined(TGEA_ENGINE)
private:
  static F32  mesh_fade;
public:
  static void setMeshFade(F32 fad_amt) { mesh_fade = fad_amt; }
  static void clearMeshFade() { mesh_fade = 1.0f; }
  static F32  getMeshFade() { return mesh_fade; }
#endif
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

DefineConsoleType( TypeByteRange )

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

