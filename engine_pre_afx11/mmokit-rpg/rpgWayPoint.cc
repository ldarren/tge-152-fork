//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "rpg/rpgWayPoint.h"
#include "console/consoleTypes.h"
#include "dgl/dgl.h"
#include "core/color.h"



//------------------------------------------------------------------------------
// Class: rpgSpawnPoint
//------------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(rpgWayPoint);

Sphere rpgWayPoint::smSphere(Sphere::Octahedron);

rpgWayPoint::rpgWayPoint()
{
    mWanderGroup = -1;
}

bool rpgWayPoint::onAdd()
{
   if(!Parent::onAdd())
      return(false);

   if(!isClientObject())
      setMaskBits(UpdateSphereMask);

   return true;
}

void rpgWayPoint::inspectPostApply()
{
   Parent::inspectPostApply();
   setMaskBits(UpdateSphereMask);
}

U32 rpgWayPoint::packUpdate(NetConnection * con, U32 mask, BitStream * stream)
{
   U32 retMask = Parent::packUpdate(con, mask, stream);

   // 
   if(stream->writeFlag(mask & UpdateSphereMask))
   {
//      stream->writeString(mSpawnpointDesc);
//      stream->writeString(mWanderDesc);
   }
   return(retMask);
}

void rpgWayPoint::unpackUpdate(NetConnection * con, BitStream * stream)
{
   Parent::unpackUpdate(con, stream);
   if(stream->readFlag())
   {
//      mSpawnpointDesc= stream->readSTString();
//      mSpawnpointDesc= stream->readSTString();
   }
}

void rpgWayPoint::initPersistFields()
{
   Parent::initPersistFields();

   addField( "WanderGroup",			TypeS32,		Offset( mWanderGroup, rpgWayPoint ) );
}



