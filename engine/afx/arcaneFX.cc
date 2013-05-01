
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "sim/sceneObject.h"
#include "sceneGraph/sceneGraph.h"
#include "game/gameConnection.h"
#include "console/compiler.h"
#ifdef TGEA_ENGINE
#include "atlas/runtime/atlasInstanceGeomTOC.h"
#endif

#include "afx/afxChoreographer.h"
#include "afx/afxSelectron.h"
#include "afx/afxResidueMgr.h"
#include "afx/ce/afxZodiacMgr.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class ClientZoneInEvent : public NetEvent
{
  typedef NetEvent Parent;
public:
  ClientZoneInEvent() { mGuaranteeType = Guaranteed; }
  ~ClientZoneInEvent() { }
  
  virtual void pack(NetConnection*, BitStream*bstream) { }
  virtual void write(NetConnection*, BitStream *bstream) { }
  virtual void unpack(NetConnection* /*ps*/, BitStream *bstream) { }

  virtual void process(NetConnection* conn)
  { 
    GameConnection* game_conn = dynamic_cast<GameConnection*>(conn);
    if (game_conn && !game_conn->isZonedIn())
    {
      arcaneFX::syncToNewConnection(game_conn);
    }
  }
  
  DECLARE_CONOBJECT(ClientZoneInEvent);
};
IMPLEMENT_CO_SERVEREVENT_V1(ClientZoneInEvent);

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

Vector<afxChoreographer*> arcaneFX::active_choreographers;
Vector<afxChoreographer*> arcaneFX::client_choreographers;
Vector<afxSelectronData*> arcaneFX::selectrons;
Vector<SceneObject*>      arcaneFX::scoped_objs;

#if defined(TGEA_ENGINE)
  F32 arcaneFX::mesh_fade = 1.0f;
#endif

StringTableEntry arcaneFX::NULLSTRING = 0;
U32              arcaneFX::sDestinationMask = 0;// DARREN MOD: mouse click movement
U32              arcaneFX::sTargetSelectionMask = 0;
F32              arcaneFX::sTargetSelectionRange = 200.0f;
U32              arcaneFX::sTargetSelectionTimeoutMS = 500;
bool             arcaneFX::sClickToTargetSelf = false;
U32              arcaneFX::sMissileCollisionMask = 0;
U32              arcaneFX::master_choreographer_id = 1;
U16              arcaneFX::master_scope_id = 1;
SceneObject*	 arcaneFX::pExempt = 0;	// DARREN MOD: exempted object for mouse selection

void arcaneFX::init()
{
  NULLSTRING = StringTable->insert("");

  Con::addVariable(  "pref::AFX::destinationMask",			TypeS32,    &sDestinationMask);// DARREN MOD: mouse click movement
  Con::addVariable(  "pref::AFX::targetSelectionMask",      TypeS32,    &sTargetSelectionMask);
  Con::addVariable(  "pref::AFX::targetSelectionRange",     TypeF32,    &sTargetSelectionRange);
  Con::addVariable(  "pref::AFX::targetSelectionTimeoutMS", TypeS32,    &sTargetSelectionTimeoutMS);
  Con::addVariable(  "pref::AFX::missileCollisionMask",     TypeS32,    &sMissileCollisionMask);
  Con::addVariable(  "pref::AFX::clickToTargetSelf",        TypeBool,   &sClickToTargetSelf);

  Con::setIntVariable(    "$AFX::IMPACTED_SOMETHING", afxEffectDefs::IMPACTED_SOMETHING);
  Con::setIntVariable(    "$AFX::IMPACTED_TARGET",    afxEffectDefs::IMPACTED_TARGET);
  Con::setIntVariable(    "$AFX::IMPACTED_PRIMARY",   afxEffectDefs::IMPACTED_PRIMARY);
  Con::setIntVariable(    "$AFX::IMPACT_IN_WATER",    afxEffectDefs::IMPACT_IN_WATER);
  Con::setIntVariable(    "$AFX::CASTER_IN_WATER",    afxEffectDefs::CASTER_IN_WATER);

  Con::setIntVariable(    "$AFX::SERVER_ONLY",        afxEffectDefs::SERVER_ONLY);
  Con::setIntVariable(    "$AFX::SCOPE_ALWAYS",       afxEffectDefs::SCOPE_ALWAYS);
  Con::setIntVariable(    "$AFX::GHOSTABLE",          afxEffectDefs::GHOSTABLE);
  Con::setIntVariable(    "$AFX::CLIENT_ONLY",        afxEffectDefs::CLIENT_ONLY);
  Con::setIntVariable(    "$AFX::SERVER_AND_CLIENT",  afxEffectDefs::SERVER_AND_CLIENT);

  Con::setFloatVariable(  "$AFX::INFINITE_TIME",      -1.0f);
  Con::setIntVariable(    "$AFX::INFINITE_REPEATS",   -1);

#if defined(TGEA_ENGINE)
  Con::setIntVariable(    "$AFX::ILLUM_TERRAIN",      LightInfo::IllumTerrain);
  Con::setIntVariable(    "$AFX::ILLUM_ATLAS",        LightInfo::IllumAtlas);
  Con::setIntVariable(    "$AFX::ILLUM_DIF",          LightInfo::IllumDIF);
  Con::setIntVariable(    "$AFX::ILLUM_DTS",          LightInfo::IllumDTS);
  Con::setIntVariable(    "$AFX::ILLUM_ALL",          LightInfo::IllumALL);
#else
  Con::setIntVariable(    "$AFX::ILLUM_TERRAIN",      0);
  Con::setIntVariable(    "$AFX::ILLUM_ATLAS",        0);
  Con::setIntVariable(    "$AFX::ILLUM_DIF",          0);
  Con::setIntVariable(    "$AFX::ILLUM_DTS",          0);
  Con::setIntVariable(    "$AFX::ILLUM_ALL",          0);
#endif

  afxResidueMgr* residue_mgr = new afxResidueMgr;
  afxResidueMgr::setMaster(residue_mgr);
  gClientContainer.addObject(afxResidueMgr::getMaster());
  gClientSceneGraph->addObjectToScene(afxResidueMgr::getMaster());
  gClientProcessList.addObject(afxResidueMgr::getMaster());

  master_scope_id = 1;
  master_choreographer_id = 1;
}

#if defined(TGEA_ENGINE)
namespace afxAtlasZodiacBatchHelper
{
   void shutdown();
}
#endif

void arcaneFX::shutdown()
{
  afxResidueMgr::getMaster()->removeFromProcessList();
  gClientSceneGraph->removeObjectFromScene(afxResidueMgr::getMaster());
  gClientContainer.removeObject(afxResidueMgr::getMaster());
  afxResidueMgr* residue_mgr = afxResidueMgr::getMaster();
  delete residue_mgr;
  afxResidueMgr::setMaster(NULL);
  for (S32 i = 0; i < selectrons.size(); i++)
    if (selectrons[i])
       selectrons[i]->registered = false;
  selectrons.clear();
  scoped_objs.clear();

#if defined(TGEA_ENGINE)
  afxAtlasZodiacBatchHelper::shutdown();
#endif
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

void arcaneFX::advanceTime(U32 delta)
{
  GameConnection* conn = GameConnection::getConnectionToServer();
  if (conn && !conn->isZonedIn() && conn->getCameraObject() != 0)
  {
    conn->setZonedIn();
    conn->postNetEvent(new ClientZoneInEvent());
  }

  afxZodiacMgr::frameReset();
}

//

U32 arcaneFX::registerChoreographer(afxChoreographer* ch)
{
  if (!ch)
    return 0;

  active_choreographers.push_back(ch);

  //Con::printf("registerChoreographer() -- size=%d %s", active_choreographers.size(),
  //  (ch->isServerObject()) ? "server" : "client"); 

  return master_choreographer_id++;
}

void arcaneFX::unregisterChoreographer(afxChoreographer* ch)
{
  if (!ch)
    return;
  
  for (U32 i = 0; i < active_choreographers.size(); i++)
  {
    if (ch == active_choreographers[i])
    {
      active_choreographers.erase_fast(i);
      //Con::printf("unregisterChoreographer() -- size=%d %s", active_choreographers.size(),
      //  (ch->isServerObject()) ? "server" : "client");
      return;
    }
  }
  
  Con::errorf("arcaneFX::unregisterChoreographer() -- failed to find choreographer in list.");
}

void arcaneFX::registerClientChoreographer(afxChoreographer* ch)
{
  if (!ch || ch->getChoreographerId() == 0)
    return;

  client_choreographers.push_back(ch);
}

void arcaneFX::unregisterClientChoreographer(afxChoreographer* ch)
{
  if (!ch || ch->getChoreographerId() == 0)
    return;
  
  for (U32 i = 0; i < client_choreographers.size(); i++)
  {
    if (ch == client_choreographers[i])
    {
      client_choreographers.erase_fast(i);
      return;
    }
  }
  
  Con::errorf("arcaneFX::unregisterClientChoreographer() -- failed to find choreographer in list.");
}

afxChoreographer* arcaneFX::findClientChoreographer(U32 id)
{
  for (U32 i = 0; i < client_choreographers.size(); i++)
  {
    if (id == client_choreographers[i]->getChoreographerId())
      return client_choreographers[i];
  }

  return 0;
}

//

void arcaneFX::registerSelectronData(afxSelectronData* selectron)
{
  if (!selectron)
    return;

  selectrons.push_back(selectron);
}

void arcaneFX::unregisterSelectronData(afxSelectronData* selectron)
{
  if (!selectron)
    return;

  for (U32 i = 0; i < selectrons.size(); i++)
  {
    if (selectron == selectrons[i])
    {
      selectrons.erase_fast(i);
      return;
    }
  }

  Con::errorf("arcaneFX::unregisterSelectronData() -- failed to find selectron in list.");
}

afxSelectronData* arcaneFX::findSelectronData(U32 mask, U8 style)
{
  for (U32 i = 0; i < selectrons.size(); i++)
    if (selectrons[i]->matches(mask, style))
      return selectrons[i];

  return 0;
}

U16 arcaneFX::generateScopeId()
{
  U16 ret_id = master_scope_id++;
  if (master_scope_id >= BIT(GameBase::SCOPE_ID_BITS))
    master_scope_id = 1;
  return ret_id;
}

void arcaneFX::registerScopedObject(SceneObject* object)
{
  scoped_objs.push_back(object);

  for (S32 i = 0; i < client_choreographers.size(); i++)
    if (client_choreographers[i])
      client_choreographers[i]->restoreScopedObject(object);
}

SceneObject* arcaneFX::findScopedObject(U16 scope_id)
{
  for (S32 i = scoped_objs.size()-1; i >= 0; i--)
    if (scoped_objs[i]->getScopeId() == scope_id)
      return scoped_objs[i];
  return 0;
} 

void arcaneFX::unregisterScopedObject(SceneObject* object)
{
  for (S32 i = scoped_objs.size()-1; i >= 0; i--)
    if (scoped_objs[i] == object)
    {
      scoped_objs.erase_fast(i);
      return;
    }
}

void arcaneFX::syncToNewConnection(GameConnection* conn)
{
  if (conn)
    conn->setZonedIn();

  for (U32 i = 0; i < active_choreographers.size(); i++)
  {
    if (active_choreographers[i])
      active_choreographers[i]->sync_with_clients();
  }
}

void arcaneFX::endMissionNotify()
{
  if (afxResidueMgr::getMaster())
    afxResidueMgr::getMaster()->cleanup();
  afxZodiacMgr::missionCleanup();
}

S32 arcaneFX::rolloverRayCast(Point3F start, Point3F end, U32 mask, U32 dmask)
{
#if !defined(AFX_CAP_ROLLOVER_RAYCASTS)
  return -1;
#else
  GameConnection* conn = GameConnection::getConnectionToServer();
  SceneObject* ctrl_obj = NULL;
  
  if (!arcaneFX::sClickToTargetSelf && conn != NULL)
    ctrl_obj = conn->getControlObject();
  
  if (ctrl_obj)
    ctrl_obj->disableCollision();
  if (pExempt) pExempt->disableCollision(); // DARREN MOD: exempted object for mouse selection

  static SceneObject* rollover_obj = 0;
  SceneObject* picked_obj = 0;

  RayInfo hit_info;
  if (gClientContainer.castRay(start, end, mask | dmask, &hit_info))// DARREN MOD: mouse click movement
    picked_obj = dynamic_cast<SceneObject*>(hit_info.object);

  if (pExempt) pExempt->enableCollision();// DARREN MOD: exempted object for mouse selection
  if (ctrl_obj)
    ctrl_obj->enableCollision();

  // DARREN MOD: mouse click movement. HACK: set destination but dun set rollover
  if (conn && picked_obj)  
  {
	  if (picked_obj->getType() & dmask) 
	  {
		  conn->setDestPointNormal(picked_obj, hit_info.point, hit_info.normal);
		  picked_obj = 0; // if not terrain will get selected
	  }
	  else conn->setDestPointNormal(NULL, Point3F(0,0,0), VectorF(0,0,0));
  }

  if (picked_obj != rollover_obj)
  {
    if (rollover_obj)
      rollover_obj->setHovered(false);
    if (picked_obj)
      picked_obj->setHovered(true);
    rollover_obj = picked_obj;

    if (conn)
      conn->setRolloverObj(rollover_obj);
  }

  return (picked_obj) ? picked_obj->getId() : -1;
#endif
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Console Functions

ConsoleFunction(afxNewClientNotify, void, 2, 2, "afxNewClientNotify(client)")
{
  GameConnection* conn = dynamic_cast<GameConnection*>(Sim::findObject(argv[1]));
  if (!conn)
  {
    Con::errorf("afxNewClientNotify() -- failed to find client object [%s].", argv[1]);
    return;
  }

  arcaneFX::syncToNewConnection(conn);
}

ConsoleFunction(afxEndMissionNotify, void, 1, 1, "afxEndMissionNotify()")
{
  arcaneFX::endMissionNotify();
}

ConsoleFunction(afxUseSFXAudio, bool, 1, 1, "afxUseSFXAudio()")
{
#if defined(SFX_AUDIO_LAYER)
  return true;
#else
  return false;
#endif
}

ConsoleFunction(afxTLKSupportEnabled, bool, 1, 1, "afxTLKSupportEnabled()")
{
  return true;
}

ConsoleFunction(afxLegacyLighting, bool, 1, 1, "afxLegacyLighting()")
{
  return false;
}

ConsoleFunction(afxGetVersion, const char*, 1, 1, "afxGetVersion()")
{
  return AFX_VERSION_STRING;
}

ConsoleFunction(afxGetEngine, const char*, 1, 1, "afxGetEngine()")
{
#ifdef TGEA_ENGINE
  return "TGEA";
#else
  return "TGE";
#endif
}

ConsoleFunction(snapAtlasGeometryMorph, void, 1, 2, "snapAtlasGeometryMorph(%n_batches=20)")
{
#if defined(TGEA_ENGINE)
  S32 n_batches = 20;
  if (argc > 1)
    dSscanf(argv[1], "%d", &n_batches);
  AtlasInstanceGeomTOC::snapGeometryMorph(n_batches);
#endif
}

#if defined(AFX_CAP_ROLLOVER_RAYCASTS)
ConsoleFunction(rolloverRayCast, S32, 4, 5, "(Point3F start, Point3F end, bitset mask, bitset dmask)")
{
  Point3F start, end;
  dSscanf(argv[1], "%f %f %f", &start.x, &start.y, &start.z);
  dSscanf(argv[2], "%f %f %f", &end.x,   &end.y,   &end.z);
  U32 mask = dAtoi(argv[3]);
  U32 dmask = 0;	// DARREN MOD: mouse click movement
  if (argc == 5) dmask = dAtoi(argv[4]);

  return arcaneFX::rolloverRayCast(start, end, mask, dmask);
}
#endif

static S32 mark_modkey = -1;

ConsoleFunction(markDataBlocks, void, 1, 1, "markDataBlocks()")
{
  mark_modkey = SimDataBlock::getNextModifiedKey();
}

ConsoleFunction(touchDataBlocks, void, 1, 1, "touchDataBlocks()")
{
  if (mark_modkey < 0)
    return;
  
  SimDataBlockGroup* g = Sim::getDataBlockGroup();
  
  U32 groupCount = g->size();
  for (S32 i = groupCount-1; i >= 0; i--)
  {
    SimDataBlock* simdb = (SimDataBlock*)(*g)[i];
    if (simdb->getModifiedKey() > mark_modkey)
    {
      simdb->unregisterObject();
      simdb->registerObject();
    }
  }
  
  mark_modkey = -1;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// Syntax Error Checking 
// (for checking eval() and compile() calls)

ConsoleFunction(wasSyntaxError, bool, 1, 1, "wasSyntaxError)")
{
  return Compiler::gSyntaxError;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// Network Object Identification

//  These useful console methods come from the following code resource:
//
//  How to Identify Objects from Client to Server or Server to Client by Nathan Davies
//    http://www.garagegames.com/index.php?sec=mg&mod=resource&page=view&qid=4852
//

ConsoleMethod(NetConnection, GetGhostIndex, S32, 3, 3, "")
{
  NetObject* pObject;
  if (Sim::findObject(argv[2], pObject))
    return object->getGhostIndex(pObject);
  return 0;
}

ConsoleMethod(NetConnection, ResolveGhost, S32, 3, 3, "")
{
  S32 nIndex = dAtoi(argv[2]);
  if (nIndex != -1)
  {
    NetObject* pObject = NULL;
    if( object->isGhostingTo())
      pObject = object->resolveGhost(nIndex);
    else if( object->isGhostingFrom())
      pObject = object->resolveObjectFromGhostIndex(nIndex);
    if (pObject)
      return pObject->getId();
  } 
  return 0;
}

// DARREN MOD: exempted object for mouse selection
ConsoleFunction(afxCastRayExemptedObj, void, 2, 2, "afxCastRayExemptedObj(objectId)")
{
      U32 exemptId = dAtoi(argv[1]);
      Sim::findObject(exemptId, arcaneFX::pExempt);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

//////////////////////////////////////////////////////////////////////////
// TypeByteRange
//////////////////////////////////////////////////////////////////////////

ConsoleType( ByteRange, TypeByteRange, sizeof(ByteRange) )

ConsoleGetType( TypeByteRange )
{
   ByteRange* pt = (ByteRange *) dptr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%u %u", pt->low, pt->high);
   return returnBuffer;
}

ConsoleSetType( TypeByteRange )
{
  if(argc == 1)
  {
    ByteRange* range = (ByteRange*) dptr;
    U32 lo, hi;
    S32 args = dSscanf(argv[0], "%u %u", &lo, &hi);
    range->low = (args > 0) ? lo : 0;
    range->high = (args > 1) ? hi : 255;
  }
  else
    Con::printf("ByteRange must be set as \"low\" or \"low high\"");
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

