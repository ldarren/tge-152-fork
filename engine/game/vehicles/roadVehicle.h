//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GAME_VEHICLES_ROADVEHICLE_H_
#define _GAME_VEHICLES_ROADVEHICLE_H_

#ifndef _SHAPEBASE_H_
#include <game/shapeBase.h>
#endif

//----------------------------------------------------------------------------

struct RoadVehicleTire: public SimDataBlock 
{
   typedef SimDataBlock Parent;

   //
   StringTableEntry shapeName;// Max shape to render

   // Shape information initialized in the preload
   Resource<TSShape> shape;   // The loaded shape
   F32 radius;                // Tire radius

   //
   RoadVehicleTire();
   DECLARE_CONOBJECT(RoadVehicleTire);
   static void initPersistFields();
   bool preload(bool, char errorBuffer[256]);
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
};

//----------------------------------------------------------------------------

struct RoadVehicleData: public ShapeBaseData 
{
	typedef ShapeBaseData Parent;

public:
	RoadVehicleData();

	S8   wheelCount;

	//
	DECLARE_CONOBJECT(RoadVehicleData);
	static void initPersistFields();
	virtual void packData(BitStream* stream);
	virtual void unpackData(BitStream* stream);
};


//----------------------------------------------------------------------------

class RoadVehicle: public ShapeBase
{
   typedef ShapeBase Parent;

   RoadVehicleData*  mDataBlock;

   void onUnmount(ShapeBase* obj,S32 node);

  protected:
   enum MaskBits 
   {
      PositionMask = Parent::NextFreeMask,
	  advancedStaticOptionsMask = Parent::NextFreeMask << 1,
	  NextFreeMask = Parent::NextFreeMask << 2
   };

public:
   DECLARE_CONOBJECT(RoadVehicle);

   RoadVehicle();
   ~RoadVehicle();

   bool onAdd();
   void onRemove();
   bool onNewDataBlock(GameBaseData* dptr);

   void processTick(const Move *move);
   void interpolateTick(F32 delta);
   void setTransform(const MatrixF &mat);
   virtual void renderShadow(F32 dist, F32 fogAmount);

   U32  packUpdate  (NetConnection *conn, U32 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn,           BitStream *stream);
   
   static void initPersistFields();
   void inspectPostApply();
};


#endif // _GAME_VEHICLES_ROADVEHICLE_H_
