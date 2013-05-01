//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

/*

MMO Kit
-------

- Expanded collision struct to include RPG specific bits
- Added realm and projectile declarations

*/

#ifndef _COLLISION_H_
#define _COLLISION_H_

#ifndef _DATACHUNKER_H_
#include "core/dataChunker.h"
#endif
#ifndef _MPLANE_H_
#include "math/mPlane.h"
#endif

class SceneObject;

//----------------------------------------------------------------------------

struct Collision
{
   SceneObject* object;
   SceneObject* srcObject; // <-- MMO Kit
    
   Point3F point;
   VectorF normal;
   U32 material;

   // Face and Face dot are currently only set by the extrudedPolyList
   // clipper.  Values are otherwise undefined.
   U32 face;                  // Which face was hit
   F32 faceDot;               // -Dot of face with poly normal
   F32 distance;
   
// Begin MMO Kit
/* Original TGE Code:
   Collision() { material = 0; }
*/

   bool projectile;
   S32 realm;

   Collision()
   {
      projectile = false;
      srcObject = NULL;
      object=NULL;
      realm=-1;
      material = 0; // <-- TGE
   }
// End MMO Kit
};

struct CollisionList
{
   enum  {
      MaxCollisions = 64
   };
   int count;
   Collision collision[MaxCollisions];
   F32 t;
   // MaxHeight is currently only set by the extrudedPolyList
   // clipper.  It represents the maximum vertex z value of
   // the returned collision surfaces.
   F32 maxHeight;
};


//----------------------------------------------------------------------------
// BSP Collision tree
// Solid nodes are represented by structures with NULL frontNode and
// backNode pointers. The material field is only valid on a solid node.
// There is no structure for empty nodes, frontNode or backNode
// should be set to NULL to represent empty half-spaces.

struct BSPNode
{
   U32 material;
   PlaneF plane;
   BSPNode *frontNode, *backNode;
};

typedef Chunker<BSPNode> BSPTree;

#endif
