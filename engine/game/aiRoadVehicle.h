//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _AIRoadVehicle_h
#define _AIRoadVehicle_h

#ifndef _AIWheeledVehicle_h
#include "game/aiWheeledVehicle.h"
#endif

class AIRoadVehicle : public AIWheeledVehicle 
{
	typedef AIWheeledVehicle Parent;

protected:
	MatrixF mMoveTransform;           // Destination for movement

protected:
	virtual F32		getSteeringAngle();

public:
	void			setMoveTransform(const MatrixF & mat, bool slowdown = true);
	const MatrixF &	getMoveTransform() const {return mMoveTransform;};


public:
	AIRoadVehicle();


	DECLARE_CONOBJECT(AIRoadVehicle);
};

#endif //_AIRoadVehicle_h