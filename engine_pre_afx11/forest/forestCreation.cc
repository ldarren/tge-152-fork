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

class CreateForestEvent : public SimEvent
{
public:
   void process(SimObject *object)
   {
      ((Forest *) object)->createForest();
   }
};

void alignTransform(MatrixF &in_transform, Point3F in_normalVector)
{
   Point3F yPrime(0,0,0);
   Point3F x(0,0,0);
   Point3F y(0,0,0);
   Point3F z = in_normalVector;

   in_transform.getColumn(1, &yPrime);
   mCross(yPrime, z, &x);
   x.normalize();
   mCross(z, x, &y);
   in_transform.setColumn(0, x);
   in_transform.setColumn(1, y);
   in_transform.setColumn(2, z);
}

const U32 gForestCreateWaitTime = 2;

// some globals so we don't have to recompute them...defined in
// createForest and only valid in functions called from there
static S32 squareSizeS32;
static F32 squareSizeF32;

void Forest::createForest()
{
   TerrainBlock * terrain = GlobDock::getTerrain(isServerObject());

   // CHECK FOR TERRAIN
   if ( terrain == 0 )
   {
      // NO TERRAIN!! - BAIL OUT AND TRY AGAIN LATER
      Sim::postEvent(this, new CreateForestEvent, Sim::getCurrentTime() + gForestCreateWaitTime);
      return;
   }

   // For server side forests, we want to wait until all the objects in the mission
   // have had a chance to initialize, so we wait a tick.
   if ( !mInitialWait )
   {
      Sim::postEvent(this, new CreateForestEvent, Sim::getCurrentTime() + gForestCreateWaitTime);
      mInitialWait = true;
      return;
   }

   const S32 time = Platform::getRealMilliseconds();

   // Let script have a chance to set up information...
   scriptOnAdd();

   // create some shape instances
   createShapeInstances();

   mRandomNumber.setSeed(mRandomSeed);

   // Map ForestEntries to the appropriate tile indices.
   for (S32 i=0; i<mForestEntries.size(); i++)
   {
      ForestEntryList & entries = *mForestEntries[i];
      for (S32 j=0; j<entries.size(); j++)
      {
         ForestEntry * fe = entries[j];
         StringTableEntry name = fe->getTileType();
         for (S32 k=0; k<TerrainBlock::MaterialGroups; k++)
            if (terrain->getFile()->mMaterialFileName[k] && !dStricmp(name,terrain->getFile()->mMaterialFileName[k]))
            {
               fe->setTileIndex(k);
               break;
            }
      }
   }

   // save ourselves some ftol's
   squareSizeS32 = terrain->getSquareSize();
   squareSizeF32 = squareSizeS32;

   if(mForestCount == 0)
      Con::warnf("Forest::createForest - Forest count is zero. To save on memory hits, you should set it to the number of items this forest generates.");

   mForestItems.setSize(mForestCount);

   for (S32 i=0; i < mForestItems.size(); i++)
      constructInPlace(&mForestItems[i]);

   ForestEntries theForestList = 0;
   VectorPtr<ForestEntryList *>::iterator  forestEntriesIdx;
   U32 groupNum   = 0;
   S32 size       = 0;
   S32 entryIdx   = 0;
   U32 cell       = 0;
   bool maxedOut  = false;
   mTotalItems    = 0;

   // Go through terrain block square by square
   for (S32 y = 0; y < TerrainBlock::BlockSize; y++)
   {
      for (S32 x = 0; x < TerrainBlock::BlockSize; x++)
      {
         maxedOut  = false;
         cell  = x + (y << TerrainBlock::BlockShift);

         mForestGrid[cell].idx   = mTotalItems;
         mForestGrid[cell].count = 0;

         // What kind of terrain tile are we on?
         //groupNum = terrain->getBaseMaterial(x, y);

         // What kind of terrain tile are we on?         
		 U8 alphas[TerrainBlock::MaterialGroups];         
		 terrain->getMaterialAlpha(Point2I(x, y), alphas);         
		 S32 maxMat = 0, maxMatIdx = 0;         
		 for(S32 curMat=0; curMat<TerrainBlock::MaterialGroups; curMat++)         
		 {            
			 if(alphas[curMat] > maxMat)            
			 {               
				 maxMat = alphas[curMat];               
				 maxMatIdx = curMat;            
			 }         
		 }         
		 groupNum = maxMatIdx;

         // Do any of the forest Types have entries with the same terrain tile type?
         for (forestEntriesIdx = mForestEntries.begin(); forestEntriesIdx != mForestEntries.end(); forestEntriesIdx++)
         {
            S16 cbItems = 0;
            size = (*forestEntriesIdx)->size();

            if (!size)
               continue;

            ForestEntry ** entryStart = &(**forestEntriesIdx)[0];
            ForestEntry ** entry = entryStart + mRandomNumber.randI(0,size-1);
            ForestEntry ** entryEnd = entryStart + size;
            //entryIdx  = mRandomNumber.randI(0, size - 1);

            // mForestEntries is sorted by mMaxItems so the maxCount is the same for the whole vector
            S16 maxCount = (*entry)->getInfoBlock()->mMaxItems;

            // Go through list of ForestItems in this forest and see if any belong on this kind of tile
            for (S32 i = 0; i < size && cbItems < maxCount; i++)
            {
               if ( (S32)groupNum == (S32)(*entry)->getTileIndex())
               {
                  // There are forestItems that go on this kind of tile! Let's create some
                  cbItems += generateForestItems( *entry, Point2I(x, y));
               }

               if ((cbItems == maxCount) && (*entry)->getInfoBlock()->mCentered)
               {
                  maxedOut = true;
                  break;
               }

               entry++;
               if (entry==entryEnd)
                  entry=entryStart;
            }

            // Is the tile already maxed out?
            if (maxedOut)
               break;

         }
      }
   } 

   Con::printf("---- End createForest: RandomSeed: %u  Forest Array Size: %d    Num ForestItems: %d    Number of ShapeInstances: %d", mRandomSeed, ForestItem::getTotalItems(), mTotalItems, smForestInstances.size());
   mForestCount = mTotalItems;
   ForestItem::resetTotalItems();
   Con::printf("Forest Created in %3.2f seconds.", (Platform::getRealMilliseconds()-time)/1000.0f);
}

//----------------------------------------------------------------------------
S32 Forest::generateForestItems(ForestEntry * in_forestEntry, Point2I in_grid)
{
   S32 cbCount = 0;

   // Roll the dice on whether this forestItem even exists?
   F32 randomProb = mRandomNumber.randF(0.0f, 100.0f);

   // Did the forestItem beat the odds?
   if (randomProb <= in_forestEntry->getProbability()*mItemProbability)
   {
      // Deal with the bitmap contribution, if any.
      F32 bitmapDensityFactor=1.f;

      if(in_forestEntry->getDensityMap())
      {
         GBitmap *bmp = in_forestEntry->getDensityMap();

         // Wrap properly.
         while(in_grid.x >= bmp->getHeight())
            in_grid.x -= bmp->getHeight();
         while(in_grid.x < 0)
            in_grid.x += bmp->getHeight();

         while(in_grid.y >= bmp->getWidth())
            in_grid.y -= bmp->getWidth();
         while(in_grid.y < 0)
            in_grid.y += bmp->getWidth();

         // Sample luminosity with a crude approximation.
         ColorI col;
         bmp->getColor(in_grid.x, in_grid.y, col);

         // BJGTODO - use the real luminance function? :)
         bitmapDensityFactor = col.red + col.green + col.blue;
         bitmapDensityFactor /= 3.f * 256.f;
      }


      // What's the random density for this forestItem?
      // in_density is Maximum density
      U32 maxDensity = in_forestEntry->getDensity()*mItemDensity*bitmapDensityFactor + 0.5f;
      U32 randomDensity;

      // How many forestItems should be created?
      if (maxDensity > 1)      
         randomDensity = mRandomNumber.randI(1, maxDensity);
      else
         if(bitmapDensityFactor > 0.3)
            randomDensity = 1;
         else
            randomDensity = 0;

      // Make them.
      for (U32 i = 0; i < randomDensity; i++)
         cbCount += createForestItem(in_forestEntry, in_grid);     
   }

   return cbCount;
}

//----------------------------------------------------------------------------

S32 Forest::createForestItem(ForestEntry * in_forestEntry, Point2I in_position)
{
   Point3F          worldCoords, normalVector;

   F32 minScaleFactor = in_forestEntry->getInfoBlock()->getMinScaleFactor();
   F32 maxScaleFactor = in_forestEntry->getInfoBlock()->getMaxScaleFactor();

   F32 newRadius      = in_forestEntry->getInfoBlock()->getRadius();
   U32 cell           = in_position.x + (in_position.y << TerrainBlock::BlockShift);
   U32 itemIdx        = mForestGrid[cell].idx + mForestGrid[cell].count;
   U32 numOfItems     = mForestItems.size();

   TerrainBlock * terrain = GlobDock::getTerrain(isServerObject());

   // No terrain? Nothing to do.
   if(!terrain)
      return 0;

   if (itemIdx >= numOfItems)
   {
      mForestItems.increment(smForestGrowthSize);
      for (U32 i = 0; i < smForestGrowthSize; i++)
         constructInPlace(&mForestItems[i + numOfItems]);

      Con::printf("(%s) Forest grew - itemIdx: %d  numOfItemsBefore: %d  numOfItemsAfter: %d", getName(), itemIdx, numOfItems, mForestItems.size());
   }

   // If it's a clearcut grid and the forestitem is a clear cut item return
   if (mForestGrid[cell].clearCut && in_forestEntry->getInfoBlock()->mClearCut)
   {
      return(0);
   }

   if ((S32)itemIdx < mForestItems.size())
   {
      // Convert grid coordinates to world coordinates
      gridToWorld(in_position, worldCoords);

      // Make sure the coords aren't always at the corner of the grid
      if (in_forestEntry->getInfoBlock()->mCentered)
      {
         // Center it in the square
         worldCoords.x += squareSizeF32 * .5f;
         worldCoords.y += squareSizeF32 * .5f;
      }
      else
      {
         // place it in the square randomly
         worldCoords.x += (F32)mRandomNumber.randF(0, squareSizeF32);
         worldCoords.y += (F32)mRandomNumber.randF(0, squareSizeF32);
      }

      worldCoords.z = getTerrHeight(Point2F(worldCoords.x, worldCoords.y), &normalVector);

      // If we're inside a forest clearing, run away.
      ForestClearing *walk = ForestClearing::smClearingList;
      while(walk)
      {
         // We're in somewhere there oughtn't be forest, skip it.
         if(walk->contains(worldCoords))
            return 0;

         walk = walk->mNext;
      }

      // Deal with unaligned shapes.
      if (!in_forestEntry->getInfoBlock()->mAligned)
      {
         // Non-aligned forestItems get sunk into the terrain to hide the bottom on slopes
         normalVector.z = 0;
         if ( !(normalVector.x == 0.0f && normalVector.y == 0.0f) )
         {
            normalVector.normalize();
         }

         normalVector  *= newRadius;
         Point3F newPos = worldCoords + normalVector;

         worldCoords.z = getTerrHeight(Point2F(newPos.x, newPos.y));
      }

      F32 scaleFactor = mRandomNumber.randF(minScaleFactor, maxScaleFactor);
      F32 actualRadius = in_forestEntry->getInfoBlock()->getRadius() * scaleFactor;

      // If we intersect with another, then abort...
      if(getNearestTree(worldCoords, actualRadius * 2, NULL, in_forestEntry->getInfoBlock()))
         return 0;

      // Position the forestItem
      mForestItems[itemIdx].setPosition(
         worldCoords, 
         Point3F(0.0f, 0.0f, mRandomNumber.randF(0.0f, M_2PI)), 
         scaleFactor
         );

      // Deal with aligned shapes.
      if (in_forestEntry->getInfoBlock()->mAligned)
      {
         // Align the forest item with the terrain
         MatrixF temp = mForestItems[itemIdx].getTransform();
         normalVector.normalize();
         alignTransform(temp, normalVector);
         mForestItems[itemIdx].setTransform(temp);
      }

      // Alright, set the item up.
      mForestItems[itemIdx].setDataBlock(in_forestEntry->getInfoBlock());
      S32 shapeIndex = in_forestEntry->getShapeIndex();
      if (shapeIndex == -1)
      {
         // haven't looked it up yet, set it now
         shapeIndex = getShapeIndex(in_forestEntry->getName());
         in_forestEntry->setShapeIndex(shapeIndex);
      }

      if (shapeIndex == -1)
      {
         // ok, shape wasn't found in database. set default
         shapeIndex = 0;
      }

      mForestItems[itemIdx].setShapeInstance(smForestInstances[shapeIndex],shapeIndex);

      // Add the forest Item to our forest List so it can be rendered properly
      mForestGrid[cell].count++;
      mTotalItems++;
      updateShapeData(shapeIndex,&mForestItems[itemIdx]);
      return 1;
   }

   return(0);
}

S32 Forest::createShapeInstances()
{
   if (smForestInstances.size())
   {
      // already created...we must be client
      resetShapeData();
      return 0;
   }

   S32 time = Platform::getRealMilliseconds();

   Resource<TSShape> shape;                          // Shape handle

   // delay initialization when reading shapes
   TSShape::smInitOnRead = false;

   S32 keep = Con::getBoolVariable("$pref::Forest::highDetail",true) ? 0 : 1;
   S32 toss = keep ^ 1;

   //   char prefixBuffer[64];
   //   dSprintf(prefixBuffer,sizeof(prefixBuffer),"%sv%i",TSLastDetail::csmCachedBillboards,keep);
   //   const char * savePtr = TSLastDetail::csmCachedBillboards;
   //   TSLastDetail::csmCachedBillboards = prefixBuffer;

   // Iterate through forestShape list and compare
   for (S32 i = 0; i < mForestShapes.size(); i++)
   {
      if (!mForestShapes[i] || !mForestShapes[i][0])
         continue;

      // Resolve shapename
      shape = ResourceManager->load(findForestData(mForestShapes[i])->mShapeFile);
      if (!bool(shape)) 
      {
         Con::errorf("Forest::createShapeInstances: Couldn't load shape \"%s\" for \"%s\"", findForestData(mForestShapes[i])->mShapeFile, mForestShapes[i]);
         continue;
      }

      // we have a shape, but we delayed initialization for the following
      TSShape * shapePtr = (TSShape*)shape;
      S32 last = shapePtr->details.size()-1;
      if (shapePtr->details[last].subShapeNum<0)
      {
         // if the shape has exactly 4 equatorial snapshots, reset to just 1 (hack, hack)
         U32 properties = shapePtr->details[last].objectDetailNum;
         U32 numEquatorSteps = properties & 0x7f;
         if (numEquatorSteps == 4)
            numEquatorSteps = 1;
         properties &= ~(U32)0x7F;
         properties |= numEquatorSteps;
         shapePtr->details[last].objectDetailNum = properties;
      }

      if (last+1==3 && shapePtr->details[0].subShapeNum>=0 && shapePtr->details[1].subShapeNum>=0 && shapePtr->details[2].subShapeNum<0)
      {
         // shape is exactly as we expect it to be (2 mesh details + a billboard detail)
         // set first two details to be identical...which one we use depends on $Forest::highDetail variable
         shapePtr->details[toss].subShapeNum = shapePtr->details[keep].subShapeNum;
         shapePtr->details[toss].objectDetailNum = shapePtr->details[keep].objectDetailNum;
      }

      // now initialize the shape
      shapePtr->init();

      // Force the bounds to be correct, they're used later.
      shapePtr->computeBounds(0, shapePtr->bounds);

      smForestInstanceNames.push_back(mForestShapes[i]);
      smForestInstances.push_back(new TSShapeInstance(shape, isClientObject()));

   }

   // set back to default value
   TSShape::smInitOnRead = true;
   //   TSLastDetail::csmCachedBillboards = savePtr;
   Con::printf("CreateShapeInstances in %3.2f seconds.", (Platform::getRealMilliseconds()-time)/1000.0f);

   resetShapeData();
   return 0;
}
