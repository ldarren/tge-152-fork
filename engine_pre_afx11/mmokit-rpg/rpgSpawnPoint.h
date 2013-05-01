
#ifndef _rpgSpawnPoint_H_
#define _rpgSpawnPoint_H_

#ifndef _MISSIONMARKER_H_
#include "game/missionMarker.h"
#endif
#ifndef _BITSTREAM_H_
#include "core/bitStream.h"
#endif
#ifndef _SIMBASE_H_
#include "console/simBase.h"
#endif
#ifndef _SHAPEBASE_H_
#include "game/shapeBase.h"
#endif
#ifndef _MATHIO_H_
#include "math/mathIO.h"
#endif
#ifndef _SPHERE_H_
#include "game/sphere.h"
#endif
#ifndef _COLOR_H_
#include "core/color.h"
#endif

//------------------------------------------------------------------------------
// Class: rpgSpawnPoint
//------------------------------------------------------------------------------



class rpgSpawnPoint : public MissionMarker
{
   private:
      typedef MissionMarker         Parent;
      static Sphere                 smSphere;

   public:
      rpgSpawnPoint();

      // SimObject
      bool onAdd();
      void inspectPostApply();

      // NetObject
      enum rpgSpawnPointMasks {
         UpdateSphereMask = Parent::NextFreeMask,
         NextFreeMask = Parent::NextFreeMask << 1
      };

      U32  packUpdate  (NetConnection *conn, U32 mask, BitStream *stream);
      void unpackUpdate(NetConnection *conn,           BitStream *stream);

      // field data
      StringTableEntry mSpawnGroup;
      S32              mWanderGroup;
      

      static void initPersistFields();

      DECLARE_CONOBJECT(rpgSpawnPoint);
};


//Bindpoints!

class rpgBindPoint : public MissionMarker
{
   private:
      typedef MissionMarker         Parent;
      static Sphere                 smSphere;

   public:
      rpgBindPoint();

      // SimObject
      bool onAdd();
      void inspectPostApply();

      // NetObject
      enum rpgBindPointMasks {
         UpdateSphereMask = Parent::NextFreeMask,
         NextFreeMask = Parent::NextFreeMask << 1
      };

      U32  packUpdate  (NetConnection *conn, U32 mask, BitStream *stream);
      void unpackUpdate(NetConnection *conn,           BitStream *stream);

      static void initPersistFields();

      DECLARE_CONOBJECT(rpgBindPoint);
};



#endif
