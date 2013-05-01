//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "game/staticShape.h"
#include "game/gameConnection.h"


struct ScopeAlwaysShapeData: public StaticShapeData {
   typedef StaticShapeData Parent;

  public:
   //ScopeAlwaysShapeData();

   //
   DECLARE_CONOBJECT(ScopeAlwaysShapeData);
   //static void initPersistFields();
   //virtual void packData(BitStream* stream);
   //virtual void unpackData(BitStream* stream);
};

IMPLEMENT_CO_DATABLOCK_V1(ScopeAlwaysShapeData);


class ScopeAlwaysShape : public StaticShape
{
      typedef StaticShape Parent;

      ScopeAlwaysShapeData*  mDataBlock;
      
   public:
      ScopeAlwaysShape();
      static void initPersistFields();
      DECLARE_CONOBJECT(ScopeAlwaysShape);
};

ScopeAlwaysShape::ScopeAlwaysShape()
{
   mNetFlags.set(Ghostable|ScopeAlways);
   
   mTypeMask |= ShadowCasterObjectType;
}

void ScopeAlwaysShape::initPersistFields()
{
   Parent::initPersistFields();
}

IMPLEMENT_CO_NETOBJECT_V1(ScopeAlwaysShape);
