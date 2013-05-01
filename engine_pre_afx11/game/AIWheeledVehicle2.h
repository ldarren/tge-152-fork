// AIWheeledVehicle2.h
// Defines a wheeled vehicle that is driven by AI

#ifndef _AIWheeledVehicle2_h
#define _AIWheeledVehicle2_h

#ifndef _WHEELEDVEHICLE_H_
#include "game/vehicles/wheeledVehicle.h"
#endif

#include "stdio.h"

class PathPoint
{
public:
	Point3F m_Point;
	float m_desiredSpeed;

};

class AIWheeledVehicle2 : public WheeledVehicle 
{
	typedef WheeledVehicle Parent;
public:
	enum MoveState {
		ModeStop,
		ModeMove,
		ModeStuck,
		ModeReverse,
	};

protected:

	//vehicle info
	Point3F mVehiclePos;
	F32 mCurrentSpeed;

	MoveState mMoveState;
	F32 mMoveSpeed;
	F32 mMoveTolerance;                 // Distance from destination before we stop
	Point3F mMoveDestination;           // Destination for movement
	Point3F mLastLocation;              // For stuck check
	bool mbBetweenLines;
	F32 mTurnAngle;
	S32 mCurrentNode;
	S32 mLastNode;
	bool mbInit;
	int mTargetNode;

	Point3F mstartPos;

	F32 mDistance;
	Point3F mIntersection;
	VectorF mRacelinedir;
	VectorF mVehicledir;

	Vector<PathPoint> mPath;
	Vector<F32> mvecStop;

	F32 followRacingLine();
	int CalcPrevNode(int node);
	int CalcNextNode(int node);
	void SetLastNode();

	// Utility Methods
	void throwCallback( const char *name );
	virtual bool getAIMove(Move* move);

public:
	AIWheeledVehicle2();

	void setMoveTolerance( const F32 tolerance );
	F32 getMoveTolerance() const { return mMoveTolerance; }
	void setMoveDestination( const Point3F &location );
	Point3F getMoveDestination() const { return mMoveDestination; }
	void addPathPoint( const Point3F &location, const float &speed);
	void setStartNode(int startnode);

	void BuildStopVector();
	void InitAI();

	F32 getDistanceToNode(U32 node);
	void updateVehicleData();

	F32 getMaxTurnAngle(F32 currentSpeed);
	F32 getSlowDownDistance(F32 currentspeed,F32 desiredspeed);
	F32 TurnToRaceLine(Point3F vehicledir,Point3F racelinedir,F32 maxTurnAngle);

	DECLARE_CONOBJECT(AIWheeledVehicle2);
};
bool DistancePointToLine( Point3F Point, Point3F LineStart, Point3F LineEnd, F32 &distance, Point3F &Intersection );


#endif