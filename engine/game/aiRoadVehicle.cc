//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "math/mMatrix.h"
#include "math/mPoint.h"
#include "core/realComp.h"
#include "game/aiRoadVehicle.h"

IMPLEMENT_CO_NETOBJECT_V1(AIRoadVehicle);


AIRoadVehicle::AIRoadVehicle() :
	AIWheeledVehicle(),
	mMoveTransform(getTransform()) 
{
}

// Build a Triangle .. calculate angle of rotation required to meet target..
// man there has to be a better way! >:)
F32 AIRoadVehicle::getSteeringAngle()
{
	// determine destination yaw and current yaw
	Point4F currRot, moveRot;
	getTransform().getColumn(1, &currRot);
	mMoveTransform.getColumn(1, &moveRot);
	F32 diffYaw = mAtan(moveRot.x, moveRot.y)-mAtan(currRot.x, currRot.y);
	if (diffYaw > M_PI) diffYaw = M_PI - diffYaw; // get vector and acute value
//Con::printf("diffYaw[%g]\n",diffYaw);

	// find heading vector for current start and destination state
	Vector4F currHead, moveHead;
	getWorldTransform().getRow(1, &currHead);
	mMoveTransform.getRow(1, &moveHead);
//Con::printf("currHead[%g %g %g] moveHead[%g %g %g]\n",currHead.x, currHead.y, currHead.z,moveHead.x, moveHead.y, moveHead.z);
	
	// find current and destination position
	Point4F currPt, movePt;
	getTransform().getColumn(3, &currPt);
	mMoveTransform.getColumn(3, &movePt);
//Con::printf("currPt[%g %g %g] movePt[%g %g %g]\n",currPt.x, currPt.y, currPt.z,movePt.x, movePt.y, movePt.z);

	// determine all function elements for finding intersecting point
	F32 A1 = currHead.x, B1 = currHead.y;
	F32 A2 = moveHead.x, B2 = moveHead.y;
	F32 C1 = A1*currPt.x + B1*currPt.y;
	F32 C2 = A2*movePt.x + B2*movePt.y;
	F32 det = A1*B2 - A2*B1;
//Con::printf("A1[%g] B1[%g] C1[%g] A2[%g] B2[%g] C2[%g] det[%g]\n",A1,B1,C1,A2,B2,C2,det);

	// filter out special cases
	if ((det > -0.009) && (det < 0.009)) return 0.0f; // diff not significant move straight first
	if ((diffYaw > -0.0009) && (diffYaw < 0.0009)) return Parent::getSteeringAngle(); // cant handle use back old method

	// approximate calculation, mMoveSpeed equal == sector size, sector angle propotional to yaw change
	F32 x = (B2*C1 - B1*C2)/det;
    F32 y = (A1*C2 - A2*C1)/det;
	F32 xDiff = x - currPt.x;
	F32 yDiff = y - currPt.y;
	Point3F currVec(xDiff, yDiff, 0.0f);
	F32 rad1 = mSqrt(xDiff*xDiff + yDiff*yDiff);
	xDiff = x - movePt.x;
	yDiff = y - movePt.y;
	Point3F moveVec(xDiff, yDiff, 0.0f);
	F32 rad2 = mSqrt(xDiff*xDiff + yDiff*yDiff);
//Con::printf("currVec[%g %g %g] moveVec[%g %g %g]\n",currVec.x, currVec.y, currVec.z,moveVec.x, moveVec.y, moveVec.z);
//Con::printf("x[%g] y[%g] rad1[%g] rad2[%g]\n",x, y, rad1, rad2);

	F32 dot = mDot(currVec, moveVec);
	F32 angle = M_PI*0.5;
	if ((dot < -0.0009) && (dot > 0.0009)) angle = mAcos(rad1*rad2/dot);
	F32 arc = angle*rad1;
	F32 steer = mMoveSpeed*(diffYaw)/arc;

	VehicleData *vd= (VehicleData*) getDataBlock();
	F32 maxSteeringAngle=vd->maxSteeringAngle;
//Con::printf("angle[%g] arc[%g] mMoveSpeed[%g] steer[%g] maxSteeringAngle[%g]\n",angle, arc, mMoveSpeed, steer, maxSteeringAngle);

	if (arc < mMoveSpeed) return diffYaw > 0 ? angle : -angle;
	return steer;
}

void AIRoadVehicle::setMoveTransform(const MatrixF & mat, bool slowdown)
{
   mMoveTransform = mat;
   mMoveTransform.getColumn(3, &mMoveDestination);
   mMoveState = ModeMove;
   mMoveSlowdown = slowdown;
}


ConsoleMethod( AIRoadVehicle, setMoveTransform, void, 3, 4, "(Transform T, bool slowDown=true)")
{
   Point3F pos;
   const MatrixF& tmat = object->getTransform();
   tmat.getColumn(3,&pos);
   AngAxisF aa(tmat);
//Con::printf("AIRoadVehicle dest[%s]\n",argv[2]);
   dSscanf(argv[2],"%g %g %g %g %g %g %g",
           &pos.x,&pos.y,&pos.z,&aa.axis.x,&aa.axis.y,&aa.axis.z,&aa.angle);

   MatrixF mat;
   aa.setMatrix(&mat);
   mat.setColumn(3,pos);

   bool slowdown = (argc > 3)? dAtob(argv[3]): true;

   object->setMoveTransform(mat, slowdown);
}

ConsoleMethod( AIRoadVehicle, getMoveTransform, const char*, 2, 2, "Get Destination transformation.")
{
   char *returnBuffer = Con::getReturnBuffer(256);
   const MatrixF& mat = object->getMoveTransform();
   Point3F pos;
   mat.getColumn(3,&pos);
   AngAxisF aa(mat);
   dSprintf(returnBuffer,256,"%g %g %g %g %g %g %g",
            pos.x,pos.y,pos.z,aa.axis.x,aa.axis.y,aa.axis.z,aa.angle);
   return returnBuffer;
}
