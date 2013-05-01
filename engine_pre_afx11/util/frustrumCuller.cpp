//-----------------------------------------------------------------------------
// Torque Shader Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "util/frustrumCuller.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sgUtil.h"
#include "terrain/sky.h"
#include "dgl/dgl.h"

SceneState *FrustrumCuller::smSceneState;
Point3F     FrustrumCuller::smCamPos;
U32         FrustrumCuller::smNumClipPlanes;
F32         FrustrumCuller::smFarDistance;
PlaneF      FrustrumCuller::smClipPlane[MaxClipPlanes];

void FrustrumCuller::init(SceneState *state)
{
   if(Con::getBoolVariable("$lockFrustrum", false))
      return;

   // Set up some general info.
   smSceneState = state;
   smFarDistance = gClientSceneGraph->getCurrentSky()->getVisibleDistance();

   // Now determine the frustrum.
   F64 realfrustumParam[6];

    dglGetFrustum(&realfrustumParam[0], &realfrustumParam[1],
      &realfrustumParam[2], &realfrustumParam[3],
      &realfrustumParam[4], &realfrustumParam[5]);

   MatrixF camToObj;
   dglGetModelview(&camToObj); // BUG? Test this.
   camToObj.inverse();
   camToObj.getColumn(3, &smCamPos);

   Point3F osCamPoint(0,0,0);
   camToObj.mulP(osCamPoint);
   sgComputeOSFrustumPlanes(  
      realfrustumParam,
      camToObj,
      osCamPoint,
      smClipPlane[4],
      smClipPlane[0],
      smClipPlane[1],
      smClipPlane[2],
      smClipPlane[3]);

   smNumClipPlanes = 4;

   if (state->mFlipCull) 
   {
      smClipPlane[0].neg();
      smClipPlane[1].neg();
      smClipPlane[2].neg();
      smClipPlane[3].neg();
      smClipPlane[4].neg();
   }

   AssertFatal(smNumClipPlanes <= MaxClipPlanes, "FrustrumCuller::init - got too many clip planes!");
}


F32 FrustrumCuller::getBoxDistance(const Box3F &box)
{
   Point3F vec;

   const Point3F &minPoint = box.min;
   const Point3F &maxPoint = box.max;

   if(smCamPos.z < minPoint.z)
      vec.z = minPoint.z - smCamPos.z;
   else if(smCamPos.z > maxPoint.z)
      vec.z = maxPoint.z - smCamPos.z;
   else
      vec.z = 0;

   if(smCamPos.x < minPoint.x)
      vec.x = minPoint.x - smCamPos.x;
   else if(smCamPos.x > maxPoint.x)
      vec.x = smCamPos.x - maxPoint.x;
   else
      vec.x = 0;

   if(smCamPos.y < minPoint.y)
      vec.y = minPoint.y - smCamPos.y;
   else if(smCamPos.y > maxPoint.y)
      vec.y = smCamPos.y - maxPoint.y;
   else
      vec.y = 0;

   return vec.len();
}

S32 FrustrumCuller::testBoxVisibility(const Box3F &bounds, const S32 mask, const F32 expand)
{
   S32 retMask = 0;

   Point3F minPoint, maxPoint;
   for(S32 i = 0; i < smNumClipPlanes; i++)
   {
      if(mask & (1 << i))
      {
         if(smClipPlane[i].x > 0)
         {
            maxPoint.x = bounds.max.x;
            minPoint.x = bounds.min.x;
         }
         else
         {
            maxPoint.x = bounds.min.x;
            minPoint.x = bounds.max.x;
         }
         if(smClipPlane[i].y > 0)
         {
            maxPoint.y = bounds.max.y;
            minPoint.y = bounds.min.y;
         }
         else
         {
            maxPoint.y = bounds.min.y;
            minPoint.y = bounds.max.y;
         }

         if(smClipPlane[i].z > 0)
         {
            maxPoint.z = bounds.max.z;
            minPoint.z = bounds.min.z;
         }
         else
         {
            maxPoint.z = bounds.min.z;
            minPoint.z = bounds.max.z;
         }


         F32 maxDot = mDot(maxPoint, smClipPlane[i]);
         F32 minDot = mDot(minPoint, smClipPlane[i]);
         F32 planeD = smClipPlane[i].d;

         if(maxDot <= -(planeD + expand))
            return -1;

         if(minDot <= -planeD)
            retMask |= (1 << i);
      }
   }

   // Check the far distance as well.
   if(mask & FarSphereMask)
   {
      F32 squareDistance = getBoxDistance(bounds);

      // Reject stuff outside our visible range...
      if(squareDistance >= smFarDistance)
         return -1;

      // See if the box potentially hits the far sphere. (Sort of assumes a square box!)
      if(squareDistance + bounds.len_x() + bounds.len_z() > smFarDistance)
         retMask |= FarSphereMask;
   }

   return retMask;
}