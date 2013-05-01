//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#include "forest/globDock.h"
#include "terrain/terrData.h"
#include "terrain/sky.h"
#include "forest/forest.h"
#include "sceneGraph/sceneGraph.h"
#include "game/game.h"
#include "sim/netObject.h"

// Note:  the following implements the method to get the global
// what is the name of the variable to be retrieved (e.g., Terrain, Sky)
// stest is a test of whether we can call sget
// sget is a method for retrieving the server version
// ctest is a test of whether we can call cget
// cget is a method for retrieving the server version
// See examples for terrain, sky, and forest.
#define ImplementGet(type,what,stest,sget,ctest,cget) type * GlobDock::get##what(bool server)\
{                                                                                   \
   GlobDock * gd = GlobDock::get();                                                 \
   if (server)                                                                      \
   {                                                                                \
      if (gd->mServerGlobals.m##what)                                               \
         return gd->mServerGlobals.m##what;                                         \
      if (stest)                                                                    \
         gd->mServerGlobals.m##what = sget;                                         \
      if (gd->mServerGlobals.m##what)                                               \
         gd->deleteNotify(gd->mServerGlobals.m##what);                              \
      return gd->mServerGlobals.m##what;                                            \
   }                                                                                \
   else                                                                             \
   {                                                                                \
      if (gd->mClientGlobals.m##what)                                               \
         return gd->mClientGlobals.m##what;                                         \
      if (ctest)                                                                    \
         gd->mClientGlobals.m##what = cget;                                         \
      if (gd->mClientGlobals.m##what)                                               \
         gd->deleteNotify(gd->mClientGlobals.m##what);                              \
      return gd->mClientGlobals.m##what;                                            \
   }                                                                                \
}

#define ImplementOnDeleteNotify(what)                                               \
   if (obj==mClientGlobals.m##what)                                                 \
      mClientGlobals.m##what = NULL;                                                \
   if (obj==mServerGlobals.m##what)                                                 \
      mServerGlobals.m##what = NULL;

GlobDock * GlobDock::smGlobDock = NULL;

GlobDock::GlobDock()
{
   mClientGlobals.clear();
   mServerGlobals.clear();
}

GlobDock::~GlobDock()
{
}

bool GlobDock::onAdd()
{
   if (!Parent::onAdd())
      return false;
   AssertFatal(smGlobDock==NULL || smGlobDock==this,"GlobDock::onAdd: attempt to create a new GlobDock when one already exists.");
   smGlobDock = this;
   return true;
}

void GlobDock::onRemove()
{
   smGlobDock = NULL;
   mClientGlobals.clear();
   mServerGlobals.clear();
   Parent::onRemove();
}

GlobDock * GlobDock::get()
{
   if (!smGlobDock)
   {
      smGlobDock = new GlobDock();
      smGlobDock->registerObject();
      Sim::getRootGroup()->addObject(smGlobDock);
   }

   return smGlobDock;
}

void GlobDock::Globals::clear()
{
   mTerrain  = NULL;
   mForest   = NULL;
   mSky      = NULL;
}

void GlobDock::onDeleteNotify(SimObject * obj)
{
   Parent::onDeleteNotify(obj);

   ImplementOnDeleteNotify(Terrain)
   ImplementOnDeleteNotify(Forest)
   ImplementOnDeleteNotify(Sky)
}

//-----------------------------------------------------
// Implement gets here:
//-----------------------------------------------------

ImplementGet(TerrainBlock, Terrain, gServerSceneGraph, gServerSceneGraph->getCurrentTerrain(), gClientSceneGraph, gClientSceneGraph->getCurrentTerrain())
ImplementGet(Sky, Sky, gServerSceneGraph, gServerSceneGraph->getCurrentSky(), gClientSceneGraph, gClientSceneGraph->getCurrentSky())
ImplementGet(Forest, Forest, 1, Forest::getServerForest(), 1, Forest::getClientForest())

//-----------------------------------------------------


