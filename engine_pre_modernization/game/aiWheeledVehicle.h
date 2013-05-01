//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _AIWheeledVehicle_h
#define _AIWheeledVehicle_h

#ifndef _WHEELEDVEHICLE_H_
#include "game/vehicles/wheeledVehicle.h"
#endif

class AIWheeledVehicle : public WheeledVehicle 
{
	typedef WheeledVehicle Parent;
public:
	enum MoveState {
		ModeStop,
		ModeMove,
		ModeStuck,
		ModeReverse
	};

	enum DrivingState {
		SteerNull,
		Left,
		Right,
		Straight,
		TurnAround
	};
protected:
	MoveState mMoveState;
	F32 mMoveSpeed;
	F32 mMoveTolerance;                 // Distance from destination before we stop
	Point3F mMoveDestination;           // Destination for movement
	Point3F mLastLocation;              // For stuck check
	bool mMoveSlowdown;                 // Slowdown as we near the destination

	// Steering
	DrivingState   steerState;
	F32   mLastSteered;
	virtual F32 getSteeringAngle();

	// Utility Methods
	void throwCallback( const char *name );
	virtual bool getAIMove(Move* move);
public:
	AIWheeledVehicle();

	// Movement sets/gets
	void setMoveSpeed( const F32 speed );
	F32 getMoveSpeed() const { return mMoveSpeed; }
	void setMoveTolerance( const F32 tolerance );
	F32 getMoveTolerance() const { return mMoveTolerance; }
	void setMoveDestination( const Point3F &location, bool slowdown );
	const Point3F & getMoveDestination() const { return mMoveDestination; }
	void stopMove();


	DECLARE_CONOBJECT(AIWheeledVehicle);
};

#endif