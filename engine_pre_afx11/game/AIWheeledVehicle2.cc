#include "AIWheeledVehicle2.h"
#include "math/mMatrix.h"
#include "math/mPoint.h"
#include "core/realComp.h"

//TWEAKABLE constants
#define STOPSAFETY 15 //How we pad the stop distance value to make sure car can stop in time
#define TURNANGLE 0.5 
#define LOSTDISTANCE 30 //When vehicle is this far treat as lost
#define MAXLOSTSPEED 20 //Max speed when lost

IMPLEMENT_CO_NETOBJECT_V1(AIWheeledVehicle2);


AIWheeledVehicle2::AIWheeledVehicle2() :
	WheeledVehicle()
{
   mMoveDestination.set( 0.0f, 0.0f, 0.0f );
   mMoveSpeed = 1.0f;
   mMoveTolerance = 0.25f;
   mbInit = false;
   mDistance = 1e20;

   mCurrentNode = 0;
}


/**
 * Sets how far away from the move location is considered
 * "on target"
 * 
 * @param tolerance Movement tolerance for error
 */
void AIWheeledVehicle2::setMoveTolerance( const F32 tolerance )
{
   mMoveTolerance = getMax( 0.1f, tolerance );
}


void AIWheeledVehicle2::addPathPoint( const Point3F &location, const float &speed)
{
	PathPoint v;
	v.m_Point = location;
	v.m_desiredSpeed = speed;
	mPath.push_back(v);
}

void AIWheeledVehicle2::InitAI()
{
	BuildStopVector();
	mbInit = true;
	return;
}

/*	Adjust this function for the performance of your vehicle
	mvecStop is a vector that store the stop distance need by the vehicle 
	at a certain speed
*/
void AIWheeledVehicle2::BuildStopVector()
{
	int maxSpeed = 250;
	F32 factor = 0.05;
	F32 value = 0;

	for (int i=0;i<maxSpeed;i++)
	{
		value = (i*i)*factor;
		mvecStop.push_back(value);
	}
}

/**
 * Sets the location for the bot to run to
 *
 * @param location Point to run to
 */
void AIWheeledVehicle2::setMoveDestination( const Point3F &location)
{
   mMoveDestination = location;
   mMoveState = ModeMove;
}


//Try to have vehicle follow racing line / Path
F32 AIWheeledVehicle2::followRacingLine()
{
	Point3F v1 = mIntersection-mVehiclePos;
	v1.z = 0;
	Point3F v2 = mIntersection-mPath[mCurrentNode].m_Point;
	v2.z = 0;

	v1.normalize();
	v2.normalize();
	Point3F lv = mCross(v1,v2);
	//Use cross product to figure out if we are left or right of racing line
	if (lv.z<0)
	{
		//vehicle is left of racing line
		return TurnToRaceLine(mVehicledir,mRacelinedir,TURNANGLE);

	}
	else
	{
		//vehicle is right of racing line
		return TurnToRaceLine(mVehicledir,mRacelinedir,-TURNANGLE);
	}

	return 0;
}

F32 AIWheeledVehicle2::TurnToRaceLine(Point3F vehicledir,Point3F racelinedir,F32 maxTurnAngle)
{
	Point3F desiredDir;
	
	F32 ratio = mDistance/10;
	if (ratio>1)
		ratio = 1;
	else if (ratio<(1/10))
		ratio = 0;

	//Square this value to have a smaller turn angle as we get closer to the racing line
	ratio =ratio *ratio;
	F32 turnAngle = ratio*maxTurnAngle;

	F32 targetDistance = getDistanceToNode(mTargetNode);

	if (targetDistance<30)
	{
		//Just aim right for the node instead of following the racing line
		desiredDir = mPath[mTargetNode].m_Point - mVehiclePos;
		desiredDir.normalize();
	}
	else
	{
		//rotate about the z axis so that we will turn twords racing line
		desiredDir.x = racelinedir.x*cos(turnAngle)+racelinedir.y*sin(turnAngle);
		desiredDir.y = racelinedir.x*-sin(turnAngle)+racelinedir.y*cos(turnAngle);
		desiredDir.z = 0;
		desiredDir.normalize();
	}

	F32 dot = mDot(desiredDir,vehicledir);
	
	F32 turn;
	//This section of code could use some improvement
	//Try to have vehicledir match desiredDir 

	//Don't turn
	if (dot>0.999)
		turn = 0;
	else if (dot>0.995)
		turn = 0.1;
	else if (dot>0.98)
		turn = 0.2;
	else if (dot>0.97)
		turn = 0.3;
	else if (dot>0.96)
		turn = 0.4;
	else 
		turn = 0.5;
	
	F32 cz = desiredDir.x*vehicledir.y - desiredDir.y*vehicledir.x;

	if (cz<0)
		turn *=-1;

	return turn;
}

/**
  Calculates the max turn angle based on current speed
  that won't flip the vehicle
  currently does nothing
 */
F32 AIWheeledVehicle2::getMaxTurnAngle(F32 currentSpeed)
{
	F32 maxTurnAngle = 50;
	return maxTurnAngle;
}

/**
 * Calculates the distance needed to slow down to a desired speed
 *
 */
F32 AIWheeledVehicle2::getSlowDownDistance(F32 currentspeed,F32 desiredspeed)
{
	if (currentspeed<=desiredspeed)
		return 0.0;

	F32 distance = mvecStop[(int)currentspeed] - mvecStop[(int)desiredspeed];

	return distance;
}

void AIWheeledVehicle2::setStartNode(int startnode)
{
	mCurrentNode = startnode;
	SetLastNode();
	setMoveDestination(mPath[mCurrentNode].m_Point);
}

int AIWheeledVehicle2::CalcNextNode(int node)
{
	int nextnode = node+1;
	if (nextnode>mPath.size()-1)
		nextnode = 0;
	
	return nextnode;
}

int AIWheeledVehicle2::CalcPrevNode(int node)
{
	int prevnode = node-1;
	if (prevnode<0)
		prevnode = mPath.size()-1;

	return prevnode;
}

void AIWheeledVehicle2::SetLastNode()
{
	mLastNode = CalcPrevNode(mCurrentNode);
}

void AIWheeledVehicle2::updateVehicleData()
{
   //Update vehicle data
   mVehiclePos = getPosition();
   mCurrentSpeed = fabs(getVelocity().len());

	MatrixF mat = getTransform();

	VectorF vehicledir;
	vehicledir.set(0,1,0);
	mat.mulV(vehicledir);
	//ignore the z part because we can't fix that
	vehicledir.z = 0;
	vehicledir.normalize();
	
	mVehicledir = vehicledir;
	
	Point3F pt1 = mPath[mCurrentNode].m_Point;
	Point3F pt2 = mPath[mLastNode].m_Point;

	mbBetweenLines = false;
	//Figure distance and intersection to racing line
	mTargetNode = mCurrentNode;
	bool b = true;
	if (!DistancePointToLine(mVehiclePos,pt2,pt1,mDistance,mIntersection))
	{
		//Try the next node
		mTargetNode = CalcNextNode(mCurrentNode);
		pt1 = mPath[mTargetNode].m_Point;
		pt2 = mPath[mCurrentNode].m_Point;
		if (DistancePointToLine(mVehiclePos,pt2,pt1,mDistance,mIntersection))
		{
			mLastNode = mCurrentNode;
			mCurrentNode = mTargetNode;
			mbBetweenLines = true;
		}
		else
		{
			//In between line segments
			Point3F inter = mPath[mTargetNode].m_Point;
			mDistance = getDistanceToNode(mTargetNode);
			mbBetweenLines = true;
		}
	}

	F32 distance = getDistanceToNode(mTargetNode);
	if (distance<mMoveTolerance)
	{
		//Use the next node
		mTargetNode = CalcNextNode(mCurrentNode);
		pt1 = mPath[mTargetNode].m_Point;
		pt2 = mPath[mCurrentNode].m_Point;
	}

	VectorF racelinedir;
	racelinedir = pt1-pt2;
	racelinedir.normalize();
	mRacelinedir = racelinedir;
}

// Think - figure out speed(accelerate or apply brakes) and steer the vehicle
bool AIWheeledVehicle2::getAIMove(Move *movePtr)
{
    if (!mbInit)
		return false;

	if (mDisableMove)
	{
	  mRigid.setAtRest();
	  return true;
	}

   *movePtr = NullMove;
	
   updateVehicleData();
      
   
	
	Point3F pt1 = mPath[mCurrentNode].m_Point;
	Point3F pt2 = mPath[mLastNode].m_Point;

	   // Orient towards our destination.
    if (mMoveState == ModeMove || mMoveState == ModeReverse) 
    {   
		mTurnAngle = followRacingLine();
		movePtr->yaw = 0;
		mSteering.x = mTurnAngle;
    }
	  
	F32 angle = mRadToDeg(movePtr->yaw);

    // Move towards the destination
    if (mMoveState == ModeMove) 
    {
        movePtr->y = 1;
		setMoveDestination(mPath[mCurrentNode].m_Point);

		//Do we need to slow down or speed up?
		F32 targetSpeed = mPath[mCurrentNode].m_desiredSpeed;
        if (mCurrentSpeed>targetSpeed)
		{
			F32 distance = getDistanceToNode(mCurrentNode);
			F32 slowdistance = getSlowDownDistance(mCurrentSpeed,mPath[mCurrentNode].m_desiredSpeed);
			if (distance<(slowdistance+STOPSAFETY))
			{
				movePtr->y = 0.0;
				movePtr->trigger[2] = true;
			}
			else
				if ((mPath[mCurrentNode].m_desiredSpeed>mCurrentSpeed)&&(mPath[mLastNode].m_desiredSpeed>mCurrentSpeed))
				{
					movePtr->y = 0.0;
					movePtr->trigger[2] = true;
				}
				else
				{
					if ((mPath[mCurrentNode].m_desiredSpeed<999)&&(mPath[mLastNode].m_desiredSpeed<999))
					{
							movePtr->y = 1.0;
					}
					else
					{
						movePtr->y = 1.0;
					}
				}
			}
		else
		{	
				movePtr->y = 1.0;
		}
    }
    else if(mMoveState == ModeReverse)
    {
        movePtr->y = -1 * mMoveSpeed;
    }
    else if(mMoveState == ModeStop)
    {
        movePtr->y = 0;
    }

	//Don't apply gas if vehicle is badly sliding
	Point3F vel = getVelocity();
	vel.z = 0;
	vel.normalize();
	F32 d = mDot(vel,mVehicledir);
	bool bSlide = false;
	if (mCurrentSpeed>20)
	{
		if (d<0.95)
		{
			movePtr->y = 0;
			movePtr->trigger[2] = true;
		}
	}


   // Replicate the trigger state into the move so that
   // triggers can be controlled from scripts.
   for( int i = 0; i < MaxTriggerKeys; i++ )
      movePtr->trigger[i] = getImageTriggerState(i);

	return true;
}

F32 AIWheeledVehicle2::getDistanceToNode(U32 node)
{
	Point3F v = mVehiclePos-mPath[node].m_Point;
	return v.len();
}
/**
 * Utility function to throw callbacks. Callbacks always occure
 * on the datablock class.
 *
 * @param name Name of script function to call
 */
void AIWheeledVehicle2::throwCallback( const char *name )
{
   Con::executef(getDataBlock(), 2, name, scriptThis());
}



//Perpendicular distance from point to line
//returns false if the point is not perpendicular to line
//Ignores the z value
bool DistancePointToLine( Point3F Point, Point3F LineStart, Point3F LineEnd, F32 &distance, Point3F &Intersection )
{
    F32 LineMag;
    F32 U;
	//Ignore z value
	Intersection.set(0,0,0);
	Point.z = 0;
	LineEnd.z = 0;
	LineStart.z = 0;

	Point3F l = LineEnd-LineStart;
	LineMag = l.len();
 
    U = ( ( ( Point.x - LineStart.x ) * ( LineEnd.x - LineStart.x ) ) +
        ( ( Point.y - LineStart.y ) * ( LineEnd.y - LineStart.y ) ))/
        ( LineMag * LineMag );
 
    if( U < 0.0f || U > 1.0f )
        return false;   // closest point does not fall within the line segment
 
    Intersection.x = LineStart.x + U * ( LineEnd.x - LineStart.x );
    Intersection.y = LineStart.y + U * ( LineEnd.y - LineStart.y );
 
	Point3F l2 = Point - Intersection;
    distance = l2.len();

	return true;
}
// --------------------------------------------------------------------------------------------
// Console Functions
// --------------------------------------------------------------------------------------------

ConsoleMethod( AIWheeledVehicle2, init, void, 2, 2, "()"
              "Initialize AI for vehicle")
{
	object->InitAI();
}


ConsoleMethod( AIWheeledVehicle2, setStartNode, void, 3, 3, "( int startnode )"
              "Sets the move speed for an AI object.")
{
   object->setStartNode( dAtoi( argv[2] ) );
}

ConsoleMethod( AIWheeledVehicle2, setMoveTolerance, void, 3, 3, "(float speed)" "Sets the movetolerance")
{
	object->setMoveTolerance(dAtof(argv[2]));
}

ConsoleMethod( AIWheeledVehicle2, addPathPoint, void, 4, 4, "(Point3F goal)(float speed)"
              "Add a point to the vehicle travel path")
{
	float speed = 0;
   Point3F v( 0.0f, 0.0f, 0.0f );
   dSscanf( argv[2], "%f %f %f", &v.x, &v.y, &v.z );
   dSscanf( argv[3], "%f", &speed );
   object->addPathPoint( v, speed );
}

ConsoleMethod( AIWheeledVehicle2, getMoveDestination, const char *, 2, 2, "()"
              "Returns the point the AI is set to move to.")
{
   Point3F movePoint = object->getMoveDestination();

   char *returnBuffer = Con::getReturnBuffer( 256 );
   dSprintf( returnBuffer, 256, "%f %f %f", movePoint.x, movePoint.y, movePoint.z );

   return returnBuffer;
}


