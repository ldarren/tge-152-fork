//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "math/mMatrix.h"
#include "math/mPoint.h"
#include "core/realComp.h"
#include "game/aiWheeledVehicle.h"

IMPLEMENT_CO_NETOBJECT_V1(AIWheeledVehicle);


EulerF extractEuler(const MatrixF & matrix)
{
    const F32 * mat = (const F32*)matrix;
    
    EulerF r;   
    r.x = mAsin(mat[MatrixF::idx(2,1)]);

    if(mCos(r.x) != 0.f)
    {
        r.y = mAtan(-mat[MatrixF::idx(2,0)], mat[MatrixF::idx(2,2)]);
        r.z = mAtan(-mat[MatrixF::idx(0,1)], mat[MatrixF::idx(1,1)]);
    }
    else
    {
        r.y = 0.f;
        r.z = mAtan(mat[MatrixF::idx(1,0)], mat[MatrixF::idx(0,0)]);
    }

    return(r);
}

AIWheeledVehicle::AIWheeledVehicle() :
	WheeledVehicle()
{
   mMoveDestination.set( 0.0f, 0.0f, 0.0f );
   mMoveSpeed = 1.0f;
   mMoveTolerance = 0.25f;
   mMoveSlowdown = true;
}

/**
 * Sets the speed at which this AI moves
 *
 * @param speed Speed to move, default player was 10
 */
void AIWheeledVehicle::setMoveSpeed( F32 speed )
{
   mMoveSpeed = getMax(0.0f, getMin( 1.0f, speed ));
}

/**
 * Stops movement for this AI
 */
void AIWheeledVehicle::stopMove()
{
   mMoveState = ModeStop;
}

/**
 * Sets how far away from the move location is considered
 * "on target"
 * 
 * @param tolerance Movement tolerance for error
 */
void AIWheeledVehicle::setMoveTolerance( const F32 tolerance )
{
   mMoveTolerance = getMax( 0.1f, tolerance );
}

/**
 * Sets the location for the bot to run to
 *
 * @param location Point to run to
 */
void AIWheeledVehicle::setMoveDestination( const Point3F &location, bool slowdown )
{
   mMoveDestination = location;
   mMoveState = ModeMove;
   mMoveSlowdown = slowdown;
}

// Build a Triangle .. calculate angle of rotation required to meet target..
// man there has to be a better way! >:)
F32 AIWheeledVehicle::getSteeringAngle()
{
   // What is our target
   Point3F desired;
   desired=mMoveDestination;

   MatrixF mat = getTransform();
   Point3F center, front;
   Point3F wFront;   
   Box3F box = getObjBox();	   

   box.getCenter(&center);
   front=center;
   front.y=box.max.y; // should be true for all these objects

   getWorldBox().getCenter(&center);
   front=center+front;

   Point3F objFront=front;
   Point3F offset = front - center;
   EulerF rot;
   rot=extractEuler(mat);
   MatrixF transform(rot);
   transform.mulV(offset, &wFront);
   front = wFront + center;

   Point3F ftoc;
   ftoc.x=mFabs(front.x-center.x);
   ftoc.y=mFabs(front.y-center.y);
   ftoc.z=mFabs(front.z-center.z);
   F32 fToc=mSqrt((ftoc.x*ftoc.x)+(ftoc.y*ftoc.y));

   Point3F ltoc;
   ltoc.x=mFabs(desired.x-center.x);
   ltoc.y=mFabs(desired.y-center.y);
   ltoc.z=mFabs(desired.z-center.z);
   F32 lToc=mSqrt((ltoc.x*ltoc.x)+(ltoc.y*ltoc.y));

   Point3F ftol;
   ftol.x=mFabs(front.x-desired.x);
   ftol.y=mFabs(front.y-desired.y);
   ftol.z=mFabs(front.z-desired.z);
   F32 fTol=mSqrt((ftol.x*ftol.x)+(ftol.y*ftol.y));
   
   F32 myAngle = mAcos(((lToc*lToc) + (fToc * fToc) - (fTol*fTol))/(2*lToc*fToc));
   Point3F location = getPosition();

   F32 xDiff = desired.x - location.x;
   F32 yDiff = desired.y - location.y;

   F32 finalYaw=mRadToDeg(myAngle);

   F32 maxSteeringAngle=0;
   
	VehicleData *vd= (VehicleData*) getDataBlock();
	maxSteeringAngle=vd->maxSteeringAngle;
   
//   if(finalYaw > 150)
//      steerState = TurnAround;
   if(finalYaw < 5 && mLastSteered != 0)
      steerState = Straight;
   else if(finalYaw < 5)
      steerState = SteerNull;
   else
   {// Quickly Hack out left or right turn info
      Point3F rotData=objFront-desired;
      MatrixF leftM(-rot);
      Point3F leftP;
      leftM.mulV(rotData, &leftP);
      leftP = leftP + desired;

      if(leftP.x<desired.x)
         steerState=Right;
      else
         steerState=Left;
   }
   
   Point2F steering = mSteering;

   F32 steer=0;
   switch(steerState)
   {
      case SteerNull:
         break;
      case Left:
         steer=myAngle < maxSteeringAngle ? -myAngle-steering.x: -maxSteeringAngle-steering.x;
         mLastSteered=steer;
         break;
      case Right:
         steer=myAngle < maxSteeringAngle ? myAngle-steering.x: maxSteeringAngle-steering.x;
         mLastSteered=steer;
         break;
      case Straight:
         steer=-steering.x;
         mLastSteered=0;
         break;
      case TurnAround:
         steer=maxSteeringAngle-steering.x;
         mLastSteered=steer;
         break;
   };

//   Con::printf("AI Steering : %f", steer);
   return steer;
}

/**
 * This method calculates the moves for the AI player
 *
 * @param movePtr Pointer to move the move list into
 */
bool AIWheeledVehicle::getAIMove(Move *movePtr)
{
   *movePtr = NullMove;

   if (!mDisableMove) {
		// Use the eye as the current position.
		MatrixF eye;
		getEyeTransform(&eye);
		Point3F location = getPosition(); 
		Point4F rotation;
		getTransform().getColumn(1, &rotation);
			
		// Orient towards our destination.
		if (mMoveState == ModeMove || mMoveState == ModeReverse) {      
				movePtr->yaw = getSteeringAngle();
		}

		// Move towards the destination
		if (mMoveState == ModeMove) {
			F32 xDiff = mMoveDestination.x - location.x;
			F32 yDiff = mMoveDestination.y - location.y;

			// Check if we should mMove, or if we are 'close enough'
			if (mFabs(xDiff) < mMoveTolerance && mFabs(yDiff) < mMoveTolerance) {
				mMoveState = ModeStop;
				throwCallback("onReachDestination");
			}
			else {
				// Build move direction in world space
				if (isZero(xDiff))
					movePtr->y = (location.y > mMoveDestination.y)? -1 : 1;
				else
					if (isZero(yDiff))
					movePtr->x = (location.x > mMoveDestination.x)? -1 : 1;
					else
					if (mFabs(xDiff) > mFabs(yDiff)) {
						F32 value = mFabs(yDiff / xDiff);
						movePtr->y = value; // (location.y > mMoveDestination.y)? -value : value;
						movePtr->x = (location.x > mMoveDestination.x)? -1 : 1;
					}
					else {
						F32 value = mFabs(xDiff / yDiff);
						movePtr->x = (location.x > mMoveDestination.x)? -value : value;
						movePtr->y = 1;  // (location.y > mMoveDestination.y)? -1 : 1;
					}

				// Rotate the move into object space (this really only needs
				// a 2D matrix)
				Point3F newMove;
				MatrixF moveMatrix;
				moveMatrix.set(EulerF(0, 0, -(rotation.z + movePtr->yaw)));
				moveMatrix.mulV( Point3F( movePtr->x, movePtr->y, 0 ), &newMove );
				movePtr->x = newMove.x;
				movePtr->y = newMove.y;

				// Set movement speed.  We'll slow down once we get close
				// to try and stop on the spot...
				if (mMoveSlowdown) {
					F32 speed = mMoveSpeed;
					F32 dist = mSqrt(xDiff*xDiff + yDiff*yDiff);
					F32 maxDist = 5;
					if (dist < maxDist)
					speed *= dist / maxDist;
					movePtr->x *= speed;
					movePtr->y *= speed;
				}
				else {
					movePtr->x *= mMoveSpeed;
					movePtr->y *= mMoveSpeed;
				}

				// We should check to see if we are stuck...
				if (location == mLastLocation) {
					throwCallback("onMoveStuck");
					mMoveState = ModeStop;
				}
			}
		}

		// Replicate the trigger state into the move so that
		// triggers can be controlled from scripts.

		for( int i = 0; i < MaxTriggerKeys; i++ )
			movePtr->trigger[i] = getImageTriggerState(i);

   } else {
	   mRigid.setAtRest();
   }

   return true;
}

/**
 * Utility function to throw callbacks. Callbacks always occure
 * on the datablock class.
 *
 * @param name Name of script function to call
 */
void AIWheeledVehicle::throwCallback( const char *name )
{
   Con::executef(getDataBlock(), 2, name, scriptThis());
}


// --------------------------------------------------------------------------------------------
// Console Functions
// --------------------------------------------------------------------------------------------

ConsoleMethod( AIWheeledVehicle, stop, void, 2, 2, "()"
              "Stop moving.")
{
   object->stopMove();
}

ConsoleMethod( AIWheeledVehicle, setMoveSpeed, void, 3, 3, "( float speed )"
              "Sets the move speed for an AI object.")
{
   object->setMoveSpeed( dAtof( argv[2] ) );
}

ConsoleMethod( AIWheeledVehicle, setMoveTolerance, void, 3, 3, "(float speed)" "Sets the movetolerance")
{
	object->setMoveTolerance(dAtof(argv[2]));
}

ConsoleMethod( AIWheeledVehicle, setMoveDestination, void, 3, 4, "(Point3F goal, bool slowDown=true)"
              "Tells the AI to move to the location provided.")
{
   Point3F v( 0.0f, 0.0f, 0.0f );
   dSscanf( argv[2], "%f %f %f", &v.x, &v.y, &v.z );
   bool slowdown = (argc > 3)? dAtob(argv[3]): true;
   object->setMoveDestination( v, slowdown);
}

ConsoleMethod( AIWheeledVehicle, getMoveDestination, const char *, 2, 2, "()"
              "Returns the point the AI is set to move to.")
{
   Point3F movePoint = object->getMoveDestination();

   char *returnBuffer = Con::getReturnBuffer( 256 );
   dSprintf( returnBuffer, 256, "%f %f %f", movePoint.x, movePoint.y, movePoint.z );

   return returnBuffer;
}


