//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "rpg/rpgSpawnPoint.h"
#include "console/consoleTypes.h"
#include "dgl/dgl.h"
#include "core/color.h"
#include "interior/interior.h"



//------------------------------------------------------------------------------
// Class: rpgSpawnPoint
//------------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(rpgSpawnPoint);

Sphere rpgSpawnPoint::smSphere(Sphere::Octahedron);

rpgSpawnPoint::rpgSpawnPoint()
{
    mSpawnGroup=StringTable->insert("");
    mWanderGroup = -1;

}

ConsoleMethod( rpgSpawnPoint, getGroundTransform, const char*, 2, 2, "Get transform of object.")
{
   char *returnBuffer = Con::getReturnBuffer(256);
   MatrixF mat = object->getTransform();
   Point3F pos;
   mat.getColumn(3,&pos);

   U32 mask=   InteriorObjectType|TerrainObjectType;
   RayInfo ri;
   Point3F pos2;

   pos2=pos;
   pos.z+=1.f;
   pos2.z-=20.f;
#ifdef DARREN_MMO
   Interior::smIncludeClipHulls=true;
   if (gServerContainer.castRay(pos,pos2, mask, &ri))
   {
      pos=ri.point;
      pos.z+=.1f;
   }
   Interior::smIncludeClipHulls=false;
#endif //DARREN_MMO
   mat.setPosition(pos);
   


   AngAxisF aa(mat);
   dSprintf(returnBuffer,256,"%g %g %g %g %g %g %g",
      pos.x,pos.y,pos.z,aa.axis.x,aa.axis.y,aa.axis.z,aa.angle);
   return returnBuffer;
}


bool rpgSpawnPoint::onAdd()
{
   if(!Parent::onAdd())
      return(false);

   if(!isClientObject())
   {

      setMaskBits(UpdateSphereMask);

   }

   return true;
}

void rpgSpawnPoint::inspectPostApply()
{
   Parent::inspectPostApply();
   setMaskBits(UpdateSphereMask);
}

U32 rpgSpawnPoint::packUpdate(NetConnection * con, U32 mask, BitStream * stream)
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

void rpgSpawnPoint::unpackUpdate(NetConnection * con, BitStream * stream)
{
   Parent::unpackUpdate(con, stream);
   if(stream->readFlag())
   {
//      mSpawnpointDesc= stream->readSTString();
//      mSpawnpointDesc= stream->readSTString();
   }
}

void rpgSpawnPoint::initPersistFields()
{
   Parent::initPersistFields();

   addField( "SpawnGroup",			TypeString,		Offset( mSpawnGroup,rpgSpawnPoint ) );
   addField( "WanderGroup",			TypeS32,		Offset( mWanderGroup,rpgSpawnPoint ) );
   
}



//------------------------------------------------------------------------------
// Class: rpgBindPoint
//------------------------------------------------------------------------------
IMPLEMENT_CO_NETOBJECT_V1(rpgBindPoint);

Sphere rpgBindPoint::smSphere(Sphere::Octahedron);

rpgBindPoint::rpgBindPoint()
{

}

bool rpgBindPoint::onAdd()
{
   if(!Parent::onAdd())
      return(false);

   if(!isClientObject())
      setMaskBits(UpdateSphereMask);

   return true;
}

void rpgBindPoint::inspectPostApply()
{
   Parent::inspectPostApply();
   setMaskBits(UpdateSphereMask);
}

U32 rpgBindPoint::packUpdate(NetConnection * con, U32 mask, BitStream * stream)
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

void rpgBindPoint::unpackUpdate(NetConnection * con, BitStream * stream)
{
   Parent::unpackUpdate(con, stream);
   if(stream->readFlag())
   {
//      mSpawnpointDesc= stream->readSTString();
//      mSpawnpointDesc= stream->readSTString();
   }
}

void rpgBindPoint::initPersistFields()
{
   Parent::initPersistFields();

   
}
