//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#ifndef _GLOB_DOCK_
#define _GLOB_DOCK_

#include "console/simBase.h"

// GlobDock -- or global variable docking station.
//
// This is a general purpose class for getting global singleton simobjects
// To get an object called Fish, simply call getFish(server), where "server"
// is true if you want the server version and flase if you want the client
// version.  To define a new type to add to the GlobDock, you need to add
// a DeclareGet to the header file, add a pointer of the object's
// type to Globals, add the object to onDeleteNotify, and finally
// add an ImplementGet to the .cc file.  See Terrain, Sky, or Forest
// for examples of this and where to put things.  Not that on the DefineGet
// you supply a type and a name.  For most object these are the same (e.g.,
// Forest is the type and name).  But sometimes -- in the case of terrain,
// for example, the type and name are differnt.  So in this case we use
// DefineGet(TerrainBlock,Terrain);

class TerrainBlock;
class Forest;
class Sky;
class NetObject;

#define DeclareGet(type,name) static type * get##name(bool server)

class GlobDock : public SimObject
{
   typedef SimObject Parent;
   
   static GlobDock * smGlobDock;

   struct Globals
   {
      TerrainBlock * mTerrain;
      Forest       * mForest;
      Sky          * mSky;

      void clear();
   } mClientGlobals, mServerGlobals;
   
   static GlobDock * get();

public:
   GlobDock();
   ~GlobDock();
   
   bool onAdd();
   void onRemove();
   void onDeleteNotify(SimObject * );

   DeclareGet(TerrainBlock,Terrain);
   DeclareGet(Forest,Forest);
   DeclareGet(Sky,Sky);
};

#endif _GLOB_DOCK_