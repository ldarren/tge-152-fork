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

#include "forest/globDock.h"
#include "forest/forestClearing.h"
#include "forest/forestItem.h"
#include "forest/forest.h"
#include "forest/forestTextureSheet.h"


F32 distanceFromPoint(Point3F in_worldCoord, Point3F in_normal, Point3F in_pointOnLine)
{
   return(mFabs(mFabs(mDot(in_worldCoord, in_normal)) - mFabs(mDot(in_pointOnLine, in_normal))));   
}

bool pointInBetween(Point3F in_pointToCheck, Point3F in_startPt, Point3F in_endPt)
{
   return( ( ((in_pointToCheck.x >= in_startPt.x) && (in_pointToCheck.x <= in_endPt.x  )) ||
      ((in_pointToCheck.x <= in_startPt.x) && (in_pointToCheck.x >= in_endPt.x  )) ) &&

      ( ((in_pointToCheck.y >= in_startPt.y) && (in_pointToCheck.y <= in_endPt.y  )) ||
      ((in_pointToCheck.y <= in_startPt.y) && (in_pointToCheck.y >= in_endPt.y  )) ) );
}

//  Taken from Graphic Gems, Vol 1, page 5-6.
bool lineIntersectsCircle(Point3F in_startPt, Point3F in_endPt, Point3F in_center, F32 in_radius, Point3F *in_pointOfIntersect )
{
   //   Con::printf("*****************************************************");
   //   Con::printf("CENTER:   %f  %f ", in_center.x,in_center.y);
   //   Con::printf("START:    %f  %f  END: %f  %f   RADIUS: %f", in_startPt.x, in_startPt.y, in_endPt.x, in_endPt.y, in_radius);

   Point3F startPt(in_startPt.x, in_startPt.y, 0.0);
   Point3F endPt(in_endPt.x, in_endPt.y, 0.0);
   Point3F pathDir   = endPt - startPt;
   Point3F centerDir = startPt - Point3F(in_center.x, in_center.y, 0.0);

   F32     a = mDot(pathDir, pathDir);
   F32     b = 2 * mDot(pathDir, centerDir);
   F32     c = mDot(centerDir, centerDir) - (in_radius * in_radius);
   F32     d = (b * b) - (4 * a * c);

   if (d < 0 )
   {
      //      Con::printf("NO INTERSECTION POINTS!");
      return(false);

   }

   // RECALCULATE WITH A SLIGHTLY LARGER RADIUS, SO WE CAN TURN TO MISS THE TREE
   //   in_radius *= 1.05;
   //   c = mDot(G,G) - (in_radius * in_radius);
   //   d = (b*b) - (4*a*c);


   // CALCULATE OUR INTERSECTION POINTS
   F32 t1 = (-b + mSqrt(d)) / (2*a);
   F32 t2 = (-b - mSqrt(d)) / (2*a);
   Point3F inter1 = pathDir;
   inter1 *= t1;
   inter1 += startPt;
   Point3F inter2 = pathDir;
   inter2 *= t2;
   inter2 += startPt;
   F32 dist1 = (inter1 - startPt).lenSquared();
   F32 dist2 = (inter2 - startPt).lenSquared();

   // Both of the intersection points must be between the start and end points OR the endpoint needs to be between the two intersection points
   // else we're going the wrong way
   if (!((pointInBetween(inter1, startPt, endPt) && pointInBetween(inter2, startPt, endPt)) ||
      pointInBetween(endPt, inter1, inter2)) )
   {
      //      Con::printf("PATH GOING WRONG WAY!");
      return(false);

   }


   // RETURN CLOSEST INTERSECTION POINT
   if (dist1 <= dist2)
   {
      if (pointInBetween(inter1, startPt, endPt))
      {
         in_pointOfIntersect->x = inter1.x;
         in_pointOfIntersect->y = inter1.y;
      }
      else
      {
         in_pointOfIntersect->x = startPt.x;
         in_pointOfIntersect->y = startPt.y;
      }

      //      Con::printf("#1 IS CLOSEST!  %f  %f      Point#2:  %f  %f", inter1.x, inter1.y, inter2.x,inter2.y);
   }
   else
   {
      if (pointInBetween(inter2, startPt, endPt))
      {
         in_pointOfIntersect->x = inter2.x;
         in_pointOfIntersect->y = inter2.y;
      }
      else
      {
         in_pointOfIntersect->x = startPt.x;
         in_pointOfIntersect->y = startPt.y;
      }

      //      Con::printf("#2 IS CLOSEST!  %f  %f       Point#1:  %f  %f", inter2.x, inter2.y, inter1.x,inter1.y);
   }
   in_pointOfIntersect->z = in_startPt.z;
   //   Con::printf("*****************************************************");
   return true;
}

void Forest::getCollisionPoints(Point2F in_startPt, Point2F in_endPt, F32 in_width, Vector<ForestItem *> &in_forestList)
{
   TerrainBlock * terrain = GlobDock::getTerrain(isServerObject());
   if (terrain == 0)
      return;

   S32 squareSize = terrain->getSquareSize();
   S32 blockSize  = squareSize * TerrainBlock::BlockSquareWidth;
   F32 distance   = (0.5f * in_width);

   Point2I gridMin, gridMax;
   Point3F minPt(getMin(in_startPt.x, in_endPt.x), getMin(in_startPt.y, in_endPt.y), 0.0f);
   Point3F maxPt(getMax(in_startPt.x, in_endPt.x), getMax(in_startPt.y, in_endPt.y), 0.0f);
   Point2F path     = (in_endPt - in_startPt);
   Point3F normal((-1.0f * path.y), path.x, 0.0f);
   Point3F pointOnLine(in_startPt.x, in_startPt.y, 0.0f);

   normal.normalize();

   // Compute the min and max points of box surrounding the collision path
   minPt.x -= (distance * mFabs(normal.x));
   minPt.y -= (distance * mFabs(normal.y));

   maxPt.x += (distance * mFabs(normal.x));
   maxPt.y += (distance * mFabs(normal.y));

   worldToGrid(minPt, gridMin);
   worldToGrid(maxPt, gridMax);

   for (S32 y = gridMin.y; y <= gridMax.y; y++)
   {
      for (S32 x = gridMin.x; x <= gridMax.x; x++)
      {
         S32 gridX = x & TerrainBlock::BlockMask;
         S32 gridY = y & TerrainBlock::BlockMask;

         F32 xOffset = blockSize * (x >> TerrainBlock::BlockShift);
         F32 yOffset = blockSize * (y >> TerrainBlock::BlockShift);

         U32 cell = gridX + (gridY  << TerrainBlock::BlockShift);

         for (S32 offset = 0; offset < mForestGrid[cell].count; offset++)
         {
            ForestItem * forestItem = &mForestItems[mForestGrid[cell].idx + offset];
            if (forestItem->collidable())
            {
               Point3F itemPosition = forestItem->getPosition();
               F32     itemRadius   = forestItem->getRadius();
               itemPosition.x += xOffset;
               itemPosition.y += yOffset;
               itemPosition.z = 0.0f;

               if (distanceFromPoint(itemPosition, normal, pointOnLine) <=  (distance + itemRadius))
               {
                  forestItem->setOffsets(xOffset,yOffset);
                  in_forestList.push_back(forestItem);
               }
            }
         }
      }      
   }
}

bool Forest::getNearestTree(Point3F in_playersPos, F32 in_radius, Point3F *nearestPos, ForestItemData *fid)
{
   static Vector<ForestItem *> forestList(64);
   forestList.clear();

   in_playersPos.z = 0;
   Point2I gridMin, gridMax;
   Point3F minPt(getMin(in_playersPos.x + in_radius, in_playersPos.x - in_radius), getMin(in_playersPos.y + in_radius, in_playersPos.y - in_radius), 0.0f);
   Point3F maxPt(getMax(in_playersPos.x + in_radius, in_playersPos.x - in_radius), getMax(in_playersPos.y + in_radius, in_playersPos.y - in_radius), 0.0f);

   TerrainBlock * terrain = GlobDock::getTerrain(isServerObject());
   if (terrain == 0)
      return false;

   S32 squareSize = terrain->getSquareSize();
   S32 blockSize  = squareSize * TerrainBlock::BlockSquareWidth;

   worldToGrid(minPt, gridMin);
   worldToGrid(maxPt, gridMax);

   for (S32 y = gridMin.y; y <= gridMax.y; y++)
   {
      for (S32 x = gridMin.x; x <= gridMax.x; x++)
      {
         S32 gridX = x & TerrainBlock::BlockMask;
         S32 gridY = y & TerrainBlock::BlockMask;

         F32 xOffset = blockSize * (x >> TerrainBlock::BlockShift);
         F32 yOffset = blockSize * (y >> TerrainBlock::BlockShift);

         U32 cell = gridX + (gridY  << TerrainBlock::BlockShift);

         for (S32 offset = 0; offset < mForestGrid[cell].count; offset++)
         {
            ForestItem * forestItem = &mForestItems[mForestGrid[cell].idx + offset];

            if(fid && fid != forestItem->getData())
               continue;

            Point3F itemPosition = forestItem->getPosition();
            itemPosition.x += xOffset;
            itemPosition.y += yOffset;
            itemPosition.z = 0;

            if((itemPosition-in_playersPos).lenSquared() < in_radius*in_radius)
            {
               forestItem->setOffsets(xOffset,yOffset);
               forestList.push_back(forestItem);
            }
         }
      }
   }

   if (forestList.size() > 0)
   {

      if(!nearestPos)
         return true;

      Point3F treePnt    = forestList[0]->getCurrentPos();
      treePnt.z = 0;
      F32     treeDist   = (treePnt - in_playersPos).lenSquared();
      F32     treeRadius = forestList[0]->getRadius();

      for (S32 i = 1; i < forestList.size(); i++)
      {
         F32     tmpDist;
         Point3F treePos = forestList[i]->getCurrentPos();
         treePos.z = 0;

         if ((tmpDist = (treePos - in_playersPos).lenSquared()) < treeDist)
         {
            treePnt  = treePos;
            treeDist = tmpDist;
            treeRadius = forestList[i]->getRadius();
         }
      }

      treePnt.z = treeRadius;
      *nearestPos = treePnt;
      return true;
   }
   else
   {
      return false;
   }
}

void Forest::getCollisionPoints(Point2F in_startPt, Point2F in_endPt, F32 in_width, Vector<Point3F> &in_pointList)
{
   Vector<ForestItem *> forestList;
   
   getCollisionPoints(in_startPt, in_endPt, in_width, forestList);

   for (S32 i = 0; i < forestList.size(); i++)
   {
      ForestItem * forestItem   = forestList[i];
      if (!forestItem->collidable())
         continue;
   
      Point3F      itemPosition = forestItem->getCurrentPos();

      itemPosition.z = forestItem->getRadius();
      in_pointList.push_back(itemPosition);
   }

}

void Forest::getCollisionPoints(Point3F in_startPt, Point3F in_endPt, F32 in_width, Vector<ForestItem*> &in_forestItemList)
{
   Point2F startPt(in_startPt.x, in_startPt.y);
   Point2F endPt(in_endPt.x, in_endPt.y);
   getCollisionPoints(startPt, endPt, in_width, in_forestItemList);
}

// Send in a path ( start point, endPoint and width ) returns a list of collision points
void Forest::getCollisionPoints(Point3F in_startPt, Point3F in_endPt, F32 in_width, Vector<Point3F> &in_pointList)
{
   Point2F startPt(in_startPt.x, in_startPt.y);
   Point2F endPt(in_endPt.x, in_endPt.y);
   Point3F startPt2D( in_startPt.x, in_startPt.y, 0.0f);
   Vector<ForestItem *> forestList;

   Point3F path     = (in_endPt - in_startPt);

   if (path.x == 0 && path.y == 0)
      return;

   Point3F normal((-1.0f * path.y), path.x, 0.0f);
   normal.normalize();

   getCollisionPoints(startPt, endPt, in_width, forestList);
   path.z = 0.0f;

   for (S32 i = 0; i < forestList.size(); i++)
   {
      ForestItem * forestItem   = forestList[i];
      if (!forestItem->collidable())
         continue;
      F32          itemHeight   = forestItem->getHeight();
      Point3F      itemPosition = forestItem->getCurrentPos();
      Point3F      pathPt       = itemPosition;

      pathPt.z = 0.0f;

      F32 distance = distanceFromPoint(pathPt, normal, startPt2D);

      // Calculate the point on the path

      pathPt.x -= (distance * normal.x);
      pathPt.y -= (distance * normal.y);

      // How far along the path is this point
      F32 factor = 0;

      if ( mFabs(path.x) > mFabs(path.y))
         factor = (pathPt.x - startPt2D.x) / path.x;
      else
         factor = (pathPt.y - startPt2D.y) / path.y;

      if (!((factor >= 0.0f) && (factor <= 1.0f)))
         continue;

      // Find the z value of the point on the path closest to our tree
      pathPt.interpolate(in_startPt, in_endPt, factor);

      // make sure the path is between the root and the top of the tree
      if ((pathPt.z > itemPosition.z) && pathPt.z < (itemPosition.z + itemHeight))
      {
         // How high up the tree did it get hit?
         itemPosition.z = pathPt.z;
         in_pointList.push_back(itemPosition);
      }
   }
}

void Forest::getItems(Box3F box, Vector<ForestItem *> &in_forestItemList)
{
   // Skip it if there's no terrain...
   TerrainBlock * terrain = GlobDock::getTerrain(isServerObject());
   if (!terrain)
      return;

   PROFILE_START(forest_getItems);

   const S32 squareSize = terrain->getSquareSize();
   const S32 blockSize  = squareSize * TerrainBlock::BlockSquareWidth;

   Point2I gridMin, gridMax;
   const Point3F &minPt = box.min;
   const Point3F &maxPt = box.max;

   worldToGrid(minPt, gridMin);
   worldToGrid(maxPt, gridMax);

   // If it's single block, clamp to the block.
   if(mSingleBlock)
   {
      gridMin.x =  mClamp(gridMin.x, 0, TerrainBlock::BlockSquareWidth);
      gridMin.y =  mClamp(gridMin.y, 0, TerrainBlock::BlockSquareWidth);
      gridMax.x =  mClamp(gridMax.x, 0, TerrainBlock::BlockSquareWidth);
      gridMax.y =  mClamp(gridMax.y, 0, TerrainBlock::BlockSquareWidth);
   }

   for (S32 y = gridMin.y; y <= gridMax.y; y++)
   {
      for (S32 x = gridMin.x; x <= gridMax.x; x++)
      {
         // BJGNOTE - this may break in scary ways.
         const S32 gridX = x & TerrainBlock::BlockMask;
         const S32 gridY = y & TerrainBlock::BlockMask;

         const F32 xOffset = blockSize * (x >> TerrainBlock::BlockShift);
         const F32 yOffset = blockSize * (y >> TerrainBlock::BlockShift);

         const U32 cell = gridX + (gridY  << TerrainBlock::BlockShift);

         for (S32 offset = 0; offset < mForestGrid[cell].count; offset++)
         {
            ForestItem * forestItem = &mForestItems[mForestGrid[cell].idx + offset];

            if (forestItem->collidable())
            {
               Point3F     itemPosition = forestItem->getPosition();
               const F32   itemRadius   = forestItem->getRadius();

               itemPosition.x += xOffset;
               itemPosition.y += yOffset;

               // Get a transformed, scaled bounding box to check against.
               Box3F itemBox = forestItem->getObjBox();
               const Point3F &scale = forestItem->getObjectScale();
               itemBox.min.convolve(scale);
               itemBox.max.convolve(scale);
               forestItem->getTransform().mul(itemBox);

               if(box.isOverlapped(itemBox))
               {
                  // BJGNOTE - this may cause extreme weirdness for big (ie,
                  // on order of magnitude of size of the terrain)
                  // bounding boxes!!! (since we're reusing a single forestItem
                  // for potentially many locations...) Might want to make an
                  // assert for that...

                  // It's like a Nyquist limit for bounding boxes!

                  forestItem->setOffsets(xOffset,yOffset);
                  in_forestItemList.push_back(forestItem);
               }
            }
         }
      }      
   }

   PROFILE_END();
}

//-----------------------------------------------------------------------------

inline void forestCastRayHelper(const Point3F &start, const Point3F &end, RayInfo* info, F32 &currentT, ForestItem *ptr, Forest *thisPtr)
{
   Point3F xformedStart, xformedEnd;
   MatrixF invM;

   invM = ptr->getTransform();
   invM.inverse();

   invM.mulP(start, &xformedStart);
   invM.mulP(end,   &xformedEnd);
   xformedStart.convolveInverse(ptr->getObjectScale());
   xformedEnd.convolveInverse(ptr->getObjectScale());

   RayInfo ri;
   ri.object = thisPtr;
   if (ptr->castRay(xformedStart, xformedEnd, &ri)) 
   {
      if(ri.t < currentT) 
      {
         *info = ri;
         info->point.interpolate(start, end, info->t);
         currentT = ri.t;
      }
   }
}

inline void Forest::forestCastRayBinHelper(const Point3F &start, const Point3F &end, RayInfo* info, F32 &currentT, Forest *thisPtr, ForestGrid &fg, const F32 &xOffset, const F32 &yOffset)
{
   for (S32 offset = 0; offset < fg.count; offset++)
   {
      ForestItem * forestItem = &thisPtr->mForestItems[fg.idx + offset];

      if (forestItem->collidable())
      {
         Point3F     itemPosition = forestItem->getPosition();
         const F32   itemRadius   = forestItem->getRadius();

         itemPosition.x += xOffset;
         itemPosition.y += yOffset;

         // Get a transformed, scaled bounding box to check against.
         Box3F itemBox = forestItem->getObjBox();
         const Point3F &scale = forestItem->getObjectScale();
         itemBox.min.convolve(scale);
         itemBox.max.convolve(scale);
         forestItem->getTransform().mul(itemBox);

         // BJGNOTE - this may cause extreme weirdness for big (ie,
         // on order of magnitude of size of the terrain)
         // lines!!! (since we're reusing a single forestItem
         // for potentially many locations...) Might want to make an
         // assert for that...

         // It's like a Nyquist limit for bounding boxes!

         forestItem->setOffsets(xOffset,yOffset);

         // Ok, process the item.
         forestCastRayHelper(start, end, info, currentT, forestItem, thisPtr);
      }
   }
}

bool Forest::castRay(const Point3F &start, const Point3F &end, RayInfo* info)
{
   const F32 blockSize = getTerrainBlockSize();

   // We don't want to waste time casting rays way over the top of the
   // forest, so let's keep track of the max height something can be
   // and then stop when we get there.
   const F32 maxItemHeight = getTerrainMaxHeight() + ForestItemData::smMaxItemHeight;
   const F32 deltaZ = start.z - end.z;

   // Can we early out?
   if(maxItemHeight < start.z && maxItemHeight < end.z)
      return false;

   // This is sort of a bastardized version of what Container does.
   Vector<ForestItem *> trees;
   F32 currentT = 2.0;

   // These are just for rasterizing the line against the grid.  We want the x coord
   //  of the start to be <= the x coord of the end
   Point3F normalStart, normalEnd;
   if (start.x <= end.x) 
   {
      normalStart = start;
      normalEnd   = end;
   }
   else 
   {
      normalStart = end;
      normalEnd   = start;
   }

   // Ok, let's scan the grids.  The simplest way to do this will be to scan across in
   //  x, finding the y range for each affected bin...
   U32 minX, maxX;
   U32 minY, maxY;

   {
      U32 yA, yB;
      Point2I gridPos;
      worldToGrid(normalStart, gridPos);

      minX = gridPos.x;
      yA = gridPos.y;

      worldToGrid(normalStart, gridPos);

      maxX = gridPos.x;
      yB = gridPos.y;

      minY = getMin(yA, yB);
      maxY = getMax(yA, yB);
   }

   // We'll optimize the case that the line is contained in one bin row or column, which
   //  will be quite a few lines.  No sense doing more work than we have to...
   //
   if ((mFabs(normalStart.x - normalEnd.x) < blockSize && minX == maxX) ||
       (mFabs(normalStart.y - normalEnd.y) < blockSize && minY == maxY)) 
   {
      U32 count;
      U32 incX, incY;
      if (minX == maxX) 
      {
         count = maxY - minY + 1;
         incX  = 0;
         incY  = 1;
      } 
      else 
      {
         count = maxX - minX + 1;
         incX  = 1;
         incY  = 0;
      }

      U32 x = minX;
      U32 y = minY;
      for (U32 i = 0; i < count; i++)
      {
         const S32 gridX = x & TerrainBlock::BlockMask;
         const S32 gridY = y & TerrainBlock::BlockMask;

         const F32 xOffset = blockSize * (x >> TerrainBlock::BlockShift);
         const F32 yOffset = blockSize * (y >> TerrainBlock::BlockShift);

         const U32 cell = gridX + (gridY  << TerrainBlock::BlockShift);

         // If we're under Z, then we can test.
         if(F32(i) / F32(count) * deltaZ + start.z < maxItemHeight)
            forestCastRayBinHelper(start, end, info, currentT, this, mForestGrid[cell], xOffset, yOffset);

         x += incX;
         y += incY;
      }
   } 
   else 
   {
      // Oh well, let's earn our keep.  We know that after the above conditional, we're
      //  going to cross at least one boundary, so that simplifies our job...

      F32 currStartX = normalStart.x;

      AssertFatal(currStartX != normalEnd.x, "This is going to cause problems in Container::castRay");
      while (currStartX != normalEnd.x) 
      {
         F32 currEndX   = getMin(currStartX + blockSize, normalEnd.x);

         F32 currStartT = (currStartX - normalStart.x) / (normalEnd.x - normalStart.x);
         F32 currEndT   = (currEndX   - normalStart.x) / (normalEnd.x - normalStart.x);

         F32 y1 = normalStart.y + (normalEnd.y - normalStart.y) * currStartT;
         F32 y2 = normalStart.y + (normalEnd.y - normalStart.y) * currEndT;

         U32 subMinX, subMaxX;

         Point2I gridPos;
         worldToGrid(Point3F(currStartX, 0, 0), gridPos);
         subMinX = gridPos.x;
         worldToGrid(Point3F(currEndX, 0, 0), gridPos);
         subMaxX = gridPos.x;

         F32 subStartX = currStartX;
         F32 subEndX   = currStartX;

         if (currStartX < 0.0f)
            subEndX -= mFmod(subEndX, blockSize);
         else
            subEndX += (blockSize - mFmod(subEndX, blockSize));

         for (U32 currXBin = subMinX; currXBin <= subMaxX; currXBin++) 
         {
            U32 checkX = currXBin % TerrainBlock::BlockSize;

            F32 subStartT = (subStartX - currStartX) / (currEndX - currStartX);
            F32 subEndT   = getMin(F32((subEndX - currStartX) / (currEndX - currStartX)), 1.f);

            F32 subY1 = y1 + (y2 - y1) * subStartT;
            F32 subY2 = y1 + (y2 - y1) * subEndT;

            F32 subZ1 = start.z + deltaZ * subStartT;
            F32 subZ2 = start.z + deltaZ * subEndT;

            if(subZ1 <= maxItemHeight && subZ2 <= maxItemHeight)
            {
               U32 newMinY, newMaxY;

               Point2I gridPos;
               worldToGrid(Point3F(0, getMin(subY1, subY2), 0), gridPos);
               newMinY = gridPos.y;
               worldToGrid(Point3F(0, getMax(subY1, subY2), 0), gridPos);
               newMaxY = gridPos.y;

               for (U32 i = newMinY; i <= newMaxY; i++) 
               {
                  U32 checkY = i % TerrainBlock::BlockSize;

                  const S32 gridX = checkX & TerrainBlock::BlockMask;
                  const S32 gridY = checkY & TerrainBlock::BlockMask;

                  const F32 xOffset = blockSize * (checkX >> TerrainBlock::BlockShift);
                  const F32 yOffset = blockSize * (checkY >> TerrainBlock::BlockShift);

                  const U32 cell = gridX + (gridY  << TerrainBlock::BlockShift);

                  forestCastRayBinHelper(start, end, info, currentT, this, mForestGrid[cell], xOffset, yOffset);
               }
            }

            subStartX = subEndX;
            subEndX   = getMin(subEndX + blockSize, currEndX);
         }

         currStartX = currEndX;
      }
   }
   
   // Cheezy!
   return currentT != 2;
}

bool Forest::buildPolyList(AbstractPolyList* polyList, const Box3F &box, const SphereF& sphere)
{
   bool gotPoly = false;
   Vector<ForestItem *> trees;

   getItems(box, trees);

   polyList->setObject(this);

   for(U32 i=0; i<trees.size(); i++)
      gotPoly |= trees[i]->buildPolyList(polyList, box, sphere);

   return gotPoly;
}

void Forest::buildConvex(const Box3F& box, Convex* convex)
{
   Vector<ForestItem *> trees;

   getItems(box, trees);

   // Clean up the convexes...
   mConvexList->collectGarbage();

   for(U32 i=0; i<trees.size(); i++)
   {
      ForestItem * ptr = trees[i];

      Box3F realBox = box;
      MatrixF worldToObj = ptr->getTransform();
      worldToObj.inverse();

      Point3F objScale = ptr->getObjectScale();

      // Put the test box into object space..
      worldToObj.mul(realBox);
      realBox.min.convolveInverse(objScale);
      realBox.max.convolveInverse(objScale);

      // If no overlap, skip to the next tree.
      if (realBox.isOverlapped(ptr->getObjBox()) == false)
         continue;

      for (U32 i = 0; i < ForestItemData::MaxCollisionShapes; i++) 
      {
         if (ptr->getData()->mCollisionDetails[i] != -1) 
         {

            // If there is no convex "accelerator" for this detail,
            // there's nothing to collide with.
            TSShape::ConvexHullAccelerator* pAccel =
               ptr->getShapeInstance()->getShape()->getAccelerator(ptr->getData()->mCollisionDetails[i]);
            if (!pAccel || !pAccel->numVerts)
               continue;

            // See if this hull exists in the working set already...
            Convex* cc = NULL;
            CollisionWorkingList& wl = convex->getWorkingList();
            for (CollisionWorkingList* itr = wl.wLink.mNext; itr != &wl; itr = itr->wLink.mNext) 
            {
               if (itr->mConvex->getType() == ForestConvexType &&
                  (static_cast<ForestConvex*>(itr->mConvex)->mItem == ptr &&
                  static_cast<ForestConvex*>(itr->mConvex)->hullId  == i)) 
               {
                  cc = itr->mConvex;
                  break;
               }
            }
            if (cc)
               continue;

            //            Con::printf("Creating new fconvex for (%x)", ptr);

            // Create a new convex.
            ForestConvex* cp = new ForestConvex;
            mConvexList->registerObject(cp);
            convex->addToWorkingList(cp);

            // Initialize ForestConvex data.
            cp->mObject    = this;
            cp->mForest    = this;
            cp->mData      = ptr->getData();
            cp->mShapeInstance  = ptr->getShapeInstance();
            cp->hullId     = i;
            cp->box        = ptr->getObjBox();
            cp->mObjScale  = ptr->getObjectScale();
            cp->calculateTransform(ptr->getTransform());
			cp->mItem	   = ptr;
         }
      }
   }
}

//-------------------------------------------------------------------------

bool ForestItem::castRay(const Point3F &start, const Point3F &end, RayInfo* info)
{
   if (getShapeInstance()) 
   {
      RayInfo shortest;
      shortest.t = 1e8;
      bool gotMatch = false;

      for (U32 i = 0; i < ForestItemData::MaxCollisionShapes; i++) 
      {
         if (mDataBlock->mLOSDetails[i] != -1) 
         {
            mDataBlock->mShapeInstance->animate(mDataBlock->mLOSDetails[i]);
            if (mDataBlock->mShapeInstance->castRay(start, end, info, mDataBlock->mLOSDetails[i])) 
            {
               if (info->t < shortest.t)
               {
                  gotMatch = true;
                  shortest = *info;
               }
            }
         }
      }

      if (gotMatch)
      {
         // Copy out the shortest time...
         *info = shortest;
         return true;
      }
   }

   return false;
}

//----------------------------------------------------------------------------
bool ForestItem::buildPolyList(AbstractPolyList* polyList, const Box3F &, const SphereF &)
{
   if (getShapeInstance())
   {
      bool ret = false;

      polyList->setTransform(&mObjToWorld, mObjScale);

      for (U32 i = 0; i < ForestItemData::MaxCollisionShapes; i++) 
      {
         if (mDataBlock->mCollisionDetails[i] != -1) 
         {
            getShapeInstance()->buildPolyList(polyList, mDataBlock->mCollisionDetails[i]);
            ret = true;
         }
      }

      return ret;
   }

   return false;
}

//--------------------------------------------------------------------------
void ForestConvex::calculateTransform(const MatrixF &worldXfrm)
{
   S32 dl = mData->mCollisionDetails[hullId];

   TSShapeInstance* si = mShapeInstance;
   TSShape* shape = si->getShape();

   const TSShape::Detail* detail = &shape->details[dl];
   S32 subs  = detail->subShapeNum;
   S32 start = shape->subShapeFirstObject[subs];
   S32 end   = start + shape->subShapeNumObjects[subs];

   // Find the first object that contains a mesh for this
   // detail level. There should only be one mesh per
   // collision detail level.

   MatrixF *nodeTransform = NULL;

   for (S32 i = start; i < end; i++) 
   {
      const TSShape::Object* obj = &shape->objects[i];
      if (obj->numMeshes && detail->objectDetailNum < obj->numMeshes) 
      {
         nodeTransform = &si->mNodeTransforms[obj->nodeIndex];
      }
   }

   // Set our transform based on our node's transform
   if(nodeTransform)
      mTransform.mul(worldXfrm, *nodeTransform);
   else
      mTransform = worldXfrm;
}


Box3F ForestConvex::getBoundingBox() const
{
   // This is probably a bad idea? -- BJG
   return getBoundingBox(mTransform, mObjScale);
}

Box3F ForestConvex::getBoundingBox(const MatrixF& mat, const Point3F& scale) const
{
   Box3F newBox = box;
   newBox.min.convolve(scale);
   newBox.max.convolve(scale);
   mat.mul(newBox);
   return newBox;
}

Point3F ForestConvex::support(const VectorF& v) const
{
   TSShape::ConvexHullAccelerator* pAccel =
      mShapeInstance->getShape()->getAccelerator(mData->mCollisionDetails[hullId]);
   AssertFatal(pAccel != NULL, "Error, no accel!");

   F32 currMaxDP = mDot(pAccel->vertexList[0], v);
   U32 index = 0;
   for (U32 i = 1; i < pAccel->numVerts; i++) 
   {
      F32 dp = mDot(pAccel->vertexList[i], v);
      if (dp > currMaxDP) 
      {
         currMaxDP = dp;
         index = i;
      }
   }

   return pAccel->vertexList[index];
}

void ForestConvex::getFeatures(const MatrixF& mat, const VectorF& n, ConvexFeature* cf)
{
   cf->material = 0;
   cf->object = mObject;

   TSShape::ConvexHullAccelerator* pAccel =
      mShapeInstance->getShape()->getAccelerator(mData->mCollisionDetails[hullId]);
   AssertFatal(pAccel != NULL, "Error, no accel!");

   F32 currMaxDP = mDot(pAccel->vertexList[0], n);
   U32 index = 0;
   U32 i;
   for (i = 1; i < pAccel->numVerts; i++) 
   {
      F32 dp = mDot(pAccel->vertexList[i], n);
      if (dp > currMaxDP) 
      {
         currMaxDP = dp;
         index = i;
      }
   }

   const U8* emitString = pAccel->emitStrings[index];
   U32 currPos = 0;
   U32 numVerts = emitString[currPos++];
   for (i = 0; i < numVerts; i++) 
   {
      cf->mVertexList.increment();
      U32 index = emitString[currPos++];
      mat.mulP(pAccel->vertexList[index], &cf->mVertexList.last());
   }

   U32 numEdges = emitString[currPos++];
   for (i = 0; i < numEdges; i++) 
   {
      U32 ev0 = emitString[currPos++];
      U32 ev1 = emitString[currPos++];
      cf->mEdgeList.increment();
      cf->mEdgeList.last().vertex[0] = ev0;
      cf->mEdgeList.last().vertex[1] = ev1;
   }

   U32 numFaces = emitString[currPos++];
   for (i = 0; i < numFaces; i++) 
   {
      cf->mFaceList.increment();
      U32 plane = emitString[currPos++];
      mat.mulV(pAccel->normalList[plane], &cf->mFaceList.last().normal);
      for (U32 j = 0; j < 3; j++)
         cf->mFaceList.last().vertex[j] = emitString[currPos++];
   }
}

void ForestConvex::getPolyList(AbstractPolyList* list)
{
   list->setTransform(&mTransform, mObjScale);
   list->setObject(mForest);

   mShapeInstance->animate(mData->mCollisionDetails[hullId]);
   mShapeInstance->buildPolyList(list, mData->mCollisionDetails[hullId]);
}
