//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "dgl/dgl.h"
#include "core/dnet.h"
#include "game/game.h"
#include "math/mMath.h"
#include "console/simBase.h"
#include "console/console.h"
#include "console/ConsoleTypes.h"
#include "console/consoleInternal.h"
#include "terrain/terrData.h"
#include "terrain/terrRender.h"
#include "sim/sceneObject.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sgUtil.h"
#include "game/gameBase.h"
#include "ts/TSShapeInstance.h"
#include "ts/TSShapeConstruct.h"
#include "ts/tsLastDetail.h"
#include "math/mathIO.h"
#include "platform/profiler.h"
#include "core/frameAllocator.h"

#include "forest/forestItem.h"
#include "forest/forest.h"
#include "forest/globDock.h"
#include "forest/forestClearing.h"

bool terrCheck(TerrainBlock* pBlock,
               const Box3F rBox,
               const Point3F localCamPos,
               F32 height,
               bool aboveTerrain)
{
   Point3F ul(rBox.min.x, rBox.min.y, rBox.max.z);
   Point3F ur(rBox.min.x, rBox.max.y, rBox.max.z);
   Point3F ll(rBox.max.x, rBox.min.y, rBox.max.z);
   Point3F lr(rBox.max.x, rBox.max.y, rBox.max.z);

   pBlock->getWorldTransform().mulP(ul);
   pBlock->getWorldTransform().mulP(ur);
   pBlock->getWorldTransform().mulP(ll);
   pBlock->getWorldTransform().mulP(lr);

   Point3F xBaseL0_s = ul - localCamPos;
   Point3F xBaseL0_e = lr - localCamPos;
   Point3F xBaseL1_s = ur - localCamPos;
   Point3F xBaseL1_e = ll - localCamPos;

   static F32 checkPoints[3] = {0.75, 0.5, 0.25};
   RayInfo rinfo;
   for (U32 i = 0; i < 3; i++)
   {
      Point3F start = (xBaseL0_s * checkPoints[i]) + localCamPos;
      Point3F end   = (xBaseL0_e * checkPoints[i]) + localCamPos;

      if (pBlock->castRay(start, end, &rinfo))
         continue;

      pBlock->getHeight(Point2F(start.x, start.y), &height);
      if ((height <= start.z) == aboveTerrain)
         continue;

      start = (xBaseL1_s * checkPoints[i]) + localCamPos;
      end   = (xBaseL1_e * checkPoints[i]) + localCamPos;

      if (pBlock->castRay(start, end, &rinfo))
         continue;

      Point3F test = (start + end) * 0.5;
      if (pBlock->castRay(localCamPos, test, &rinfo) == false)
         continue;

      return true;
   }

   return false;
}

