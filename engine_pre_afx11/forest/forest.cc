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

bool terrCheck(TerrainBlock* pBlock,
               const Box3F rBox,
               const Point3F localCamPos,
               F32 height,
               bool aboveTerrain);

IMPLEMENT_CO_NETOBJECT_V1(Forest);

#ifdef FOREST_DEBUG
   bool treesSaved = false;
   Point3F saveV0, saveV1, saveV2, saveCP, saveCY, saveBackup;
   Point3F saveV0B, saveV1B, saveV2B;
#endif

S32      Forest::mNumClipPlanes    = 0;
PlaneF   Forest::mClipPlane[5];

U32      Forest::smRenderCount = 0;

S32      Forest::smForestGrowthSize = 5000;
F32      Forest::smItemProbability = 1.0f;
F32      Forest::smItemDensity     = 1.0f;
F32      Forest::smMaxBillboardDistance = 300.f;
F32      Forest::smMaxBillboardDistanceScale = 1.f;
bool     Forest::smFogBillboards   = true;
Forest * Forest::smServerForest    = NULL;
Forest * Forest::smClientForest    = NULL;

SceneState* Forest::smState = NULL;

Forest::ViewData Forest::smViewData;

Forest::ForestInstances Forest::smForestInstances;        // List of all shape Instances
Forest::ForestInstNames Forest::smForestInstanceNames;    // List of all shape Instance Names
Forest::ForestTextureSheets Forest::smForestTSheets;    // List of all texture sheets

bool    PointWithinDistance(Point2F in_gridCoord, Point2F in_normal, Point2F in_pointOnLine, F32 distance);

//----------------------------------------------------------------------------

ConsoleMethod(Forest, addForestEntry, void, 6, 7, "(forestItemName, terrainTile, probability, density, densityMap)")
{
   // argv[2] = forestItemName
   // argv[3] = terrain tile found on
   // argv[4] = probability of being found on that terrain tile
   // argv[5] = density of forestItems on that tile
   // argv[6] = texture to modulate density by
   
   GBitmap *bmp = NULL;
   if(argc == 7)
   {
      // Load the bitmap.
      bmp = TextureManager::loadBitmapInstance(argv[6]);

      if(!bmp)
         Con::errorf("Forest::addForestEntryMap - could not load density map '%s'!", argv[6] );
   }

   object->addForestEntry( argv[2], argv[3], dAtof(argv[4]), dAtoi(argv[5]), bmp);
}

ConsoleMethod(Forest, addClearing, void, 6, 6, "(xPos, yPos, width, height) - Create a new clearing of given location and size.")
{
   // argv[2] = xPosition
   // argv[3] = yPosition
   // argv[4] = width
   // argv[5] = height
   
   object->addForestClearing( dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]), dAtof(argv[5]));
}

ConsoleFunction(addForestItem, void, 5, 5, "(forestItemName, xPos, yPos, isServerObject)")
{
   // argv[1] = forestItemName
   // argv[2] = xPositionb
   // argv[3] = yPosition
   // argv[4] = isServerObject
   Forest* theForest = GlobDock::getForest(dAtob(argv[4]));
   F32 x = dAtoi(argv[2]) & (TerrainBlock::BlockSize * TerrainBlock::BlockShift - 1);
   F32 y = dAtoi(argv[3]) & (TerrainBlock::BlockSize * TerrainBlock::BlockShift - 1);
   Point3F coord(x, y, 0.0f);
   theForest->addForestItem( argv[1], coord);
}

void  Forest::addForestItem(const char * in_forestItemName, Point3F in_position)
{
   ForestEntry * fe = findForestEntry(in_forestItemName);
   Point2I gridPos;
   
   worldToGrid(in_position, gridPos);
   gridPos.x = (gridPos.x & TerrainBlock::BlockMask);
   gridPos.y = (gridPos.y & TerrainBlock::BlockMask);
   if (fe != NULL)
   {
      createForestItem(fe, gridPos);
   }
}

// this only returns the first forest entry with the same name
ForestEntry * Forest::findForestEntry(const char * in_forestEntryName)
{
   ForestEntry * returnEntry = NULL;

   for (S32 i=0; i<mForestEntries.size(); i++)
   {
      ForestEntryList & entries = *mForestEntries[i];
      for (S32 j=0; j<entries.size(); j++)
      {
         ForestEntry * fe = entries[j];
         if (!dStricmp(fe->getName(), in_forestEntryName))
         {
            returnEntry = fe;
            break;
         }
      }
   }
   return(returnEntry);
   
}

//----------------------------------------------------------------------------

Forest::Forest()
{
   mTypeMask        |= StaticObjectType | StaticTSObjectType | StaticShapeObjectType;
   mNetFlags.set(Ghostable | ScopeAlways);
   mRandomSeed      = Sim::getCurrentTime();
   mRandomNumber    = 0;
   mForestCount     = 85000;
   mTotalItems      = 0;
   mTreeDistance    = 1.0f;
   mItemDensity     = smItemDensity;
   mItemProbability = smItemProbability;
   mForestEntries.clear ();
   mForestEntries.reserve(10);
   mForestShapes.clear ();
   mObjBox.min.set(-1e10, -1e10, -1e10);
   mObjBox.max.set( 1e10,  1e10,  1e10);
   mWorldBox = mObjBox;
   smViewData.init = false;

   mMinOcclusionLevel = 3;
   mMaxOcclusionLevel = 5;
   mSingleBlock       = false;

   mConvexList = new Convex;

   mInitialWait = false;
}

//----------------------------------------------------------------------------

Forest::~Forest()
{
   for (S32 j = 0; j < mForestEntries.size(); j++)
   {
      while (mForestEntries[j]->size() > 0) 
      { 
         delete mForestEntries[j]->last(); 
         mForestEntries[j]->pop_back(); 
      }
      delete mForestEntries[j];
   } 

   // wipe our forest instances...unless still a client or server forest lying around
   while (!smClientForest && !smServerForest && smForestInstances.size() > 0) 
   { 
      delete smForestInstances.last(); 
      smForestInstances.pop_back(); 
   }

   // same as above with instance names, wipe out the texture sheets
   while (!smClientForest && !smServerForest && smForestTSheets.size() > 0) 
   { 
      delete smForestTSheets.last(); 
      smForestTSheets.pop_back(); 
   }

   // same as above with instance names, don't delete since not owned here
   if (!smClientForest && !smServerForest)
      smForestInstanceNames.clear();

   while (mForestItems.size())
   {
      destructInPlace(&mForestItems.last());
      mForestItems.pop_back();
   }
   setRenderListSizes(0,0);

   smViewData.init = false;

   delete mConvexList;
   mConvexList = NULL;

   removeFromScene();
}

//----------------------------------------------------------------------------

bool Forest::onAdd()
{
   if (!Parent::onAdd())
   {  
      Con::printf("Forest::onAdd, parent not adding!");
      return false;
   }

   const char *name = getName();
   if(name && name[0] && getClassRep())
   {
      Namespace *parent = getClassRep()->getNameSpace();
      Con::linkNamespaces(parent->mName, name);
      mNameSpace = Con::lookupNamespace(name);
   }

   if (isClientObject())
   {
      // Load up the shapes and create the shape instances here on the client
      AssertFatal(smClientForest == 0, "Already have a Client Forest!!");
      smClientForest = this;
   }
   else
   {
      AssertFatal(smServerForest == 0, "Already have a Server Forest!!");
      smServerForest = this;
   }

   mObjBox.min.set(-1e8, -1e8, -1e8);
   mObjBox.max.set( 1e8,  1e8,  1e8);
   resetWorldBox();
   addToScene();
   createForest();
   return(true);
   
}

//----------------------------------------------------------------------------

void Forest::addForestEntry( const char * in_forestItemName, 
                             const char * in_tileType, 
                             F32          in_probability, 
                             U16          in_density,
                             GBitmap     *in_densityMap)
{
   char             dataBlockName[255];
   ForestItemData * theForestData = findForestData(in_forestItemName);
   
   if (theForestData == 0)
   {
      dStrcpy(dataBlockName, in_forestItemName);

      // Set the datablock
      if(!Sim::findObject(dataBlockName, theForestData))
      {
         Con::errorf("Forest::addForestEntry - Unknown ForestDataBlock '%s'!", dataBlockName);
         return;
      }

      mForestItemInfo.push_back(theForestData);
   }

   S32 currentSize = mForestEntries.size();
   S32 entryIdx    = theForestData->mMaxItems;
   
   if (currentSize < entryIdx + 1)
   {
      mForestEntries.setSize(entryIdx + 1);
      for (S32 i = currentSize; i < entryIdx + 1; i++)
      {
         mForestEntries[i] = new Vector<ForestEntry *>;
      }
   }

   // Add the Forest Entry to the correct Forest Entry list. Sort by maxItems
   mForestEntries[entryIdx]->push_back(new ForestEntry( in_forestItemName, in_tileType, in_probability, in_density, theForestData, in_densityMap ) );

   // Update the Shapes List
   if (!findShape(in_forestItemName))
   {
      mForestShapes.push_back(StringTable->insert(in_forestItemName));
   }
}

void Forest::addForestClearing( F32 in_xPos, F32 in_yPos, F32 in_width, F32 in_height )
{
   Point2I gridMin, gridMax;
   Point3F minPt(getMin(in_xPos, in_xPos + in_width), getMin(in_yPos, in_yPos + in_height), 0.0f);
   Point3F maxPt(getMax(in_xPos, in_xPos + in_width), getMax(in_yPos, in_yPos + in_height), 0.0f);
           
   worldToGrid(minPt, gridMin);
   worldToGrid(maxPt, gridMax);
   
   for (S32 y = gridMin.y; y <= gridMax.y; y++)
   {
      for (S32 x = gridMin.x; x <= gridMax.x; x++)
      {
         //BJGTODO - broken for out of quadrant 1 stuff!
         U32 gridX = x & TerrainBlock::BlockMask;
         U32 gridY = y & TerrainBlock::BlockMask;
         
         U32 cell = gridX + (gridY  << TerrainBlock::BlockShift);
         mForestGrid[cell].clearCut = true;
      }
   }

}

//----------------------------------------------------------------------------
S32 Forest::getShapeIndex( const char * in_forestShape)
{
   for (S32 i = 0; i < smForestInstanceNames.size(); i++)
      if (dStricmp(smForestInstanceNames[i], in_forestShape) == 0)
         // Found It!!
         return i;
   return -1;
}

//----------------------------------------------------------------------------

bool Forest::findShape( const char * in_forestShape)
{
   VectorPtr<StringTableEntry>::iterator forestShapeIdx = 0;
   
   // Iterate through forestShape list and compare
   for (forestShapeIdx = mForestShapes.begin(); forestShapeIdx != mForestShapes.end(); forestShapeIdx++)
      if (dStricmp((*forestShapeIdx), in_forestShape) == 0 )
         // Found it!!
         return true;

   return false;
}

//----------------------------------------------------------------------------

ForestItemData * Forest::findForestData( const char * in_forestData)
{
   VectorPtr<ForestItemData *>::iterator forestDataIdx = 0;
   
   for (forestDataIdx = mForestItemInfo.begin(); forestDataIdx != mForestItemInfo.end(); forestDataIdx++)
      if (dStricmp((*forestDataIdx)->getName(), in_forestData) == 0 )
         return *forestDataIdx;
   
   return NULL;
}

//----------------------------------------------------------------------------


// Convert grid coordinates to world coordinates
bool Forest::gridToWorld(const Point2I & gPos, Point3F & wPos)
{
   TerrainBlock * terrain = GlobDock::getTerrain(isServerObject());
   if(terrain)
   {
      const MatrixF & mat = terrain->getTransform();
      Point3F origin;
      mat.getColumn(3, &origin);
      
      wPos.x = gPos.x * (float)terrain->getSquareSize() + origin.x;
      wPos.y = gPos.y * (float)terrain->getSquareSize() + origin.y;
      wPos.z = getTerrHeight(Point2F(wPos.x, wPos.y));
      
      return true;
   }

   return false;
}

bool Forest::worldToGrid(const Point3F & wPos, Point2I & gPos)
{
   TerrainBlock * terrain = GlobDock::getTerrain(isServerObject());
   if (terrain)
   {
      const MatrixF & mat = terrain->getTransform();
      Point3F origin;
      mat.getColumn(3, &origin);
      const F32 squareSize = (F32) terrain->getSquareSize();
      const F32 halfSquareSize = 0; //squareSize / 2;

      F32 x = (wPos.x - origin.x + halfSquareSize) / squareSize;
      F32 y = (wPos.y - origin.y + halfSquareSize) / squareSize;
      
      gPos.x = (S32)mFloor(x);
      gPos.y = (S32)mFloor(y);
  
      return true;
   }

   return false;
}

F32 Forest::getTerrainSquareSize() const
{
   TerrainBlock * terrain = GlobDock::getTerrain(isServerObject());
   if (terrain)
      return terrain->getSquareSize();
   else
      return 1.f;
}

F32 Forest::getTerrainBlockSize() const
{
   TerrainBlock * terrain = GlobDock::getTerrain(isServerObject());
   if (terrain)
      return terrain->getSquareSize() * TerrainBlock::BlockSize;
   else
      return 1.f;
}

F32 Forest::getTerrainMaxHeight() const
{
   TerrainBlock * terrain = GlobDock::getTerrain(isServerObject());
   if (terrain)
      return fixedToFloat(terrain->findSquare(TerrainBlock::BlockShift, 0, 0)->maxHeight);
   else
      return 1.f;
}

//----------------------------------------------------------------------------
void Forest::onRemove()
{
   mConvexList->nukeList();

   if (smClientForest == this)
   {
      smClientForest = 0;
   }
   else
   {
      AssertFatal(smServerForest == this, "Forest::onRemove - Doh!!! not the server forest either!!!!");
      smServerForest = 0;
   }

   Parent::onRemove();
}
//----------------------------------------------------------------------------

U32 Forest::packUpdate(NetConnection *connection, U32 mask, BitStream *bstream)
{
   Parent::packUpdate(connection,mask,bstream);
   
   if (bstream->writeFlag((mask & InitialUpdateMask) != 0)) 
   {
      bstream->writeString(getName());
      bstream->writeFlag(mSingleBlock);

      bstream->write(mRandomSeed);
      bstream->write(mForestCount);
      bstream->write(mTreeDistance);
      bstream->write(mItemProbability);
      bstream->write(mItemDensity);
      bstream->write(mMinOcclusionLevel);
      bstream->write(mMaxOcclusionLevel);
   }
   return 0;
   
}

//----------------------------------------------------------------------------

void Forest::unpackUpdate(NetConnection *connection, BitStream *bstream)
{
   Parent::unpackUpdate(connection,bstream);
   
   if (bstream->readFlag()) 
   {
      char readBuffer[1024];
      
      bstream->readString(readBuffer);
      mSingleBlock = bstream->readFlag();
      bstream->read(&mRandomSeed);
      bstream->read(&mForestCount);
      bstream->read(&mTreeDistance);
      bstream->read(&mItemProbability);
      bstream->read(&mItemDensity);
      bstream->read(&mMinOcclusionLevel);
      bstream->read(&mMaxOcclusionLevel);
      assignName(readBuffer);
   }
}


//----------------------------------------------------------------------------

void Forest::initPersistFields()
{
   Parent::initPersistFields();
   addField("randomSeed",    TypeS32,  Offset(mRandomSeed,  Forest));
   addField("forestCount",   TypeS32,  Offset(mForestCount, Forest));
   addField("treeDistance",  TypeF32,  Offset(mTreeDistance, Forest));

   addField("minOcclusionLevel", TypeS32, Offset(mMinOcclusionLevel, Forest));
   addField("maxOcclusionLevel", TypeS32, Offset(mMaxOcclusionLevel, Forest));

   addField("singleBlock",       TypeBool, Offset(mSingleBlock, Forest));
}

//----------------------------------------------------------------------------

void Forest::scriptOnAdd()
{
   // Script onAdd() must be called by the leaf class after
   // everything is ready.

   // We do some trickery around loading the right .cs file...
   //char forestFile[256];
   //dSprintf(forestFile,sizeof(forestFile),"%s.%s", getName(), ".cs");
   //Con::executef(2, "exec", forestFile);
   Con::executef(this,1,"onAdd");
}

//----------------------------------------------------------------------------

void Forest::consoleInit()
{
   Con::addVariable("pref::Forest::itemProbability", TypeF32,  &Forest::smItemProbability);
   Con::addVariable("pref::Forest::itemDensity",     TypeF32,  &Forest::smItemDensity    );
   Con::addVariable("pref::Forest::fogBillboards",   TypeBool, &Forest::smFogBillboards  );
   Con::addVariable("pref::Forest::growthSize",      TypeS32,  &Forest::smForestGrowthSize);
   Con::addVariable("pref::Forest::maxBillboardDistanceScale", TypeF32, &Forest::smMaxBillboardDistanceScale);
}

S32 FN_CDECL Forest::compareDistanceFunc(const void* p1, const void* p2)
{
   ForestItem * ptrItem1 = *(ForestItem**)(p1);
   ForestItem * ptrItem2 = *(ForestItem**)(p2);

   return ptrItem1->getSquaredDistance() < ptrItem2->getSquaredDistance() ? 1 : -1;
}

void Forest::renderParametersUpdated()
{
   smViewData.init = false;
}

S32 Forest::getDetailLevel(S32 shapeNum, F32 dist, S32 startCheck)
{
   ViewData::DetailData & dd = smViewData.detailData[shapeNum];
   for (S32 i=startCheck; i<dd.size(); i++)
   {
      if (dist<=dd[i].endDist)
      {
         AssertFatal(dist>=dd[i].startDist,"Forest::getDetailLevel: detail error");
         return i;
      }
   }
   return -1;
}

void Forest::ViewData::setListSizes(S32 numBins, S32 numShapes)
{
   // bin detail lists -- one per bin
   while (numBins<binDetailLists.size())
   {
      destructInPlace(&binDetailLists.last());
      binDetailLists.decrement();
   }
   while (numBins>binDetailLists.size())
   {
      binDetailLists.increment();
      constructInPlace(&binDetailLists.last());
   }

   //  detail data lists -- one per shape
   while (numShapes<detailData.size())
   {
      destructInPlace(&detailData.last());
      detailData.decrement();
   }
   while (numShapes>detailData.size())
   {
      detailData.increment();
      constructInPlace(&detailData.last());
   }
}

void Forest::setRenderListSizes(S32 numBins, S32 numShapes)
{
   // translucent lists -- one per bin
   while (numBins<mTranslucentLists.size())
   {
      destructInPlace(&mTranslucentLists.last());
      mTranslucentLists.decrement();
   }
   while (numBins>mTranslucentLists.size())
   {
      mTranslucentLists.increment();
      constructInPlace(&mTranslucentLists.last());
   }

   //  nonTranslucentLists -- one per shape
   while (numShapes<mNonTranslucentLists.size())
   {
      destructInPlace(&mNonTranslucentLists.last());
      mNonTranslucentLists.decrement();
   }
   while (numShapes>mNonTranslucentLists.size())
   {
      mNonTranslucentLists.increment();
      constructInPlace(&mNonTranslucentLists.last());
   }
}

void Forest::setSortBin(S32 bin, F32 binSize, S32 numShapes)
{
   smViewData.binDetailLists[bin].setSize(numShapes);
   for (S32 i=0; i<numShapes; i++)
   {
      F32 minDist = bin * binSize;
      F32 maxDist = minDist+binSize;
      smViewData.binDetailLists[bin][i].minDL = getDetailLevel(i,minDist);
      smViewData.binDetailLists[bin][i].maxDL = getDetailLevel(i,maxDist/smViewData.minScale[i]);
   }
}

void Forest::resetShapeData()
{
   if (isServerObject())
      return;
      
   // creating a new forest, clear our shape database
   smViewData.minScale.setSize(smForestInstances.size());
   for (S32 i=0; i<smViewData.minScale.size(); i++)
      smViewData.minScale[i]=1.0f;

   smViewData.maxCellRadius = 0.0f;
   smViewData.maxShapeRadius = 0.0f;

   // Sneaky hack so that we have materials on a listen server.
   for(U32 i=0; i<smForestInstances.size(); i++)
   {
      smForestInstances[i]->buildInstanceData(smForestInstances[i]->getShape(), true);

      // Here is where we want to build the billboard texture sheets

      // BJGTODO - Snag this out of the BB detail.

      // These parameters are a pretty wicked hack, they are basically just taken from
      // a debug-run of a TSLastDetail call. If you want to reverse-Clark the TS stuff
      // to get them, be my guest. -- patw
      //ForestTextureSheet bar( smForestInstances[i], 12, 3, 0.26998064, true, 0, 128 );
      smForestTSheets.push_back( new ForestTextureSheet( smForestInstances[i], 1, 12, 0.26998064, true, 0, 128 ) );
   }
}

void Forest::updateShapeData(S32 shapeIndex, ForestItem * fi)
{
   if (isServerObject())
      return;

   F32 scale = 1.0f / fi->getInvScale();

   // just added a new forest item, update our shape database as needed
   if (smViewData.minScale[shapeIndex]>scale)
      smViewData.minScale[shapeIndex] = scale;

   // update max cell radius...
   Point2I gp;
   Point3F wp;

   bool ok = worldToGrid(fi->getPosition(),gp);
   AssertFatal(ok,"Forest::updateShapeData - couldn't go from world to grid!");

   ok = gridToWorld(gp,wp);
   AssertFatal(ok,"Forest::updateShapeData - couldn't go from grid to world!");
   
   Point3F offset = fi->getPosition()-wp;
   F32 squareSize = GlobDock::getTerrain(false)->getSquareSize();

   AssertFatal(offset.x > -0.1 && offset.x <= squareSize, "Forest::updateShapeData - out of square bounds!");
   AssertFatal(offset.y > -0.1 && offset.y <= squareSize, "Forest::updateShapeData - out of square bounds!");

   // mirror towards 0,0 corner
   if (offset.x > squareSize*0.5f)
      offset.x = squareSize - offset.x;
   if (offset.y > squareSize*0.5f)
      offset.y = squareSize - offset.y;
   
   // center around 0,0
   offset.x += squareSize * 0.5f;
   offset.y += squareSize * 0.5f;
   
   // shift to farthest off-tile side
   if (offset.x < offset.y)
      offset.x -= scale * smForestInstances[shapeIndex]->getShape()->tubeRadius;
   else
      offset.y -= scale * smForestInstances[shapeIndex]->getShape()->tubeRadius;
   
   offset.z = 0.0f;
   F32 distSq = mDot(offset,offset);
   if (distSq > smViewData.maxCellRadius * smViewData.maxCellRadius)
      smViewData.maxCellRadius = mSqrt(distSq);

   // update max shape radius
   if (scale * smForestInstances[shapeIndex]->getShape()->tubeRadius > smViewData.maxShapeRadius)
      smViewData.maxShapeRadius = scale * smForestInstances[shapeIndex]->getShape()->tubeRadius;
}

void Forest::setupDetailData(S32 shapeNum)
{
   // constructInPlace detail data for this shape
   S32 numDL = 0;
   F32 prevDist = 0.0f;
   TSShapeInstance * si = smForestInstances[shapeNum];
   ViewData::DetailData & dd = smViewData.detailData[shapeNum];
   dd.clear();

//   Con::printf("Creating detail info...");

   // special case for no billboard...
   if (ForestItem::smLowDetail)
   {
//      Con::printf("     - low res mode!");

      dd.increment();
      ViewData::DetailLevel & entry = dd.last();
      entry.startDist = 0.0f;
      entry.endDist = 1000.0f;
      entry.invRange = 1.0f/1000.0f;

      // one detail level throughout
      entry.dl1 = si->getShape()->details.size()-2;
      if (entry.dl1<0)
         entry.dl1=0;
      entry.alpha1 = 1.0f;
      entry.slope1 = 0.0f;
      entry.interpolate1 = false;

      entry.dl2 = -1;
      entry.alpha2 = 1.0f;
      entry.slope2 = 0.0f;
      entry.interpolate2 = false;
      
      return;
   }
   
//   Con::printf("  - Hi res mode!");

   for (S32 i=0; i<si->getShape()->details.size(); i++)
   {
      const TSShape::Detail & detail = si->getShape()->details[i];
      bool isNextDetail = false;
      bool isNextBB = false;

      // Check for collision
      if (detail.size < 0)
         continue;

      if (i+1<si->getShape()->details.size())
      {
         // are we a repeat?
         const TSShape::Detail & nextDetail = si->getShape()->details[i+1];
         if (detail.subShapeNum==nextDetail.subShapeNum && detail.objectDetailNum==nextDetail.objectDetailNum)
            // next is same as this one (except size and name)...don't bother with this one...
            continue;

         // If the nextDetail isn't a collision mesh, note some info about it.
         if(nextDetail.size >= 0)
         {
           isNextDetail = true;

           if (nextDetail.subShapeNum<0)
              isNextBB = true;
         }
      }

      // figure out when this detail level starts and ends (distance-wise, that is)
      F32 startDist,endDist, detailSize;

      startDist  = prevDist;
      detailSize = detail.size;
      endDist    = TSShapeInstance::smDetailAdjust * si->getShape()->radius * dglGetPixelScale() * dglGetWorldToScreenScale() / detailSize;

      // billboard if beyond billboard distance...
      if (isNextBB && endDist>smViewData.billboardDistance)
         endDist = smViewData.billboardDistance;

      prevDist = endDist;

      // add a new detail level entry
      dd.increment();
      ViewData::DetailLevel & entry = dd.last();
      entry.startDist = startDist;
      entry.endDist   = endDist;
      entry.invRange  = 1.0f / F32(endDist-startDist);

      // if there is no next detail level or the next detail level is a billboard,
      // then we want to fade this dl out at some point and, in the later case,
      // fade the next detail in
      if (isNextBB)
      {
         // next detail is billboard, so blend us with bb as we move away from the camera

         // let dist be distance from camera of shape during render
         // then we will compute k = (dist-startDist) * invRange

         // non-billboard will "alpha out" as k goes from 1-alphaOutBB to 1
         // billboard will "alpha in" as k goes from 1-alphaInBB-alphaOutBB to
         // 1-alphaOutBB

         // alpha values less than 0 or greater than 1 are allowed (and are clamped to 0 or 1)

         // always draw non-billboard first...
         entry.dl1 = i;
         entry.alpha1 =  1.0f / smViewData.alphaOutBB;
         entry.slope1 = -1.0f / smViewData.alphaOutBB;
         entry.interpolate1 = true;

         entry.dl2 = i+1;
         entry.alpha2 = (smViewData.alphaOutBB + smViewData.alphaInBB - 1) / smViewData.alphaInBB;
         entry.slope2 = 1.0f / smViewData.alphaInBB;
         entry.interpolate2 = true;

  //       Con::printf("     o %d - mesh->imposter [%f, %f) dl1=%d dl2=%d", dd.size(), startDist, endDist, i, i+1);
      }
      else if (!isNextDetail)
      {
         // we're the last detail level, so fade us out as we go
         entry.dl1 = i;
         entry.alpha1 =  1.0f / smViewData.alphaFadeOut;
         entry.slope1 = -1.0f / smViewData.alphaFadeOut;
         entry.interpolate1 = true;
         
         entry.dl2 = -1;
         entry.alpha2 = 1.0f;
         entry.slope2 = 0.0f;
         entry.interpolate2 = false;

//         Con::printf("     o %d - fadeout  [%f, %f) dl1=%d dl2=%d", dd.size(), startDist, endDist, i, -1);
      }
      else
      {
         // most straightforward case -- one detail level throughout
         entry.dl1 = i;
         entry.alpha1 = 1.0f;
         entry.slope1 = 0.0f;
         entry.interpolate1 = false;

         entry.dl2 = -1;
         entry.alpha2 = 1.0f;
         entry.slope2 = 0.0f;
         entry.interpolate2 = false;

//         Con::printf("     o %d - constant [%f, %f) dl1=%d dl2=%d", dd.size(), startDist, endDist, i, -1);
      }

      numDL++;
   }
}

void Forest::setViewData(F32 fov, F32 fogDist, F32 visDist)
{
   AssertFatal(!isServerObject(),"Forest::setViewData - client only operation, but called on server!");

   // The following are preset (set during forest setup or as input parameters before init):
   //    numBins, maxCellRadius
   if (smViewData.numBins<=0) // can't have this
      smViewData.numBins = 1;

   S32 numShapes = smForestInstances.size();

   // get zoom factor...
   F32 zoom = GameGetCameraFov() / fov; 

   // record these so that we'll know when they change
   smViewData.terrain = GlobDock::getTerrain(false);
   smViewData.forest  = this;

   // make these adjustable?
   smViewData.numBins = 25;
   smViewData.alphaFadeOut = 0.2f;
   smViewData.alphaOutBB = 0.1f;
   smViewData.alphaInBB = 0.1f;

   // some specific camera info
   smViewData.fov        = fov;
   smViewData.visDist    = visDist;
   smViewData.backupDist = smViewData.maxShapeRadius / mSin(smViewData.fov*0.5f);
   smViewData.visFarDist = (1.0f-mTreeDistance) * fogDist + mTreeDistance * visDist; // used for visibility
   smViewData.farDist    = smViewData.visFarDist + smViewData.maxShapeRadius + smViewData.backupDist; // used for view cone
   smViewData.farWidth   = smViewData.farDist * mTan(smViewData.fov*0.5f);
   smViewData.billboardDistance = smMaxBillboardDistance * smMaxBillboardDistanceScale * zoom;

   // bin spacing
   F32 binSize = (smViewData.maxShapeRadius+visDist)/(F32)smViewData.numBins;
   smViewData.invBinSize = 1.0f / binSize;

   // set the list sizes
   smViewData.setListSizes(smViewData.numBins,numShapes);

   // set the render list sizes
   setRenderListSizes(smViewData.numBins,numShapes);

   // detailData -- set up detail level information for each shape
   for (S32 i=0; i<numShapes; i++)
      setupDetailData(i);

   // sortBins -- set up sort bin data (esp. dl for each shape)
   for (S32 i=0; i<smViewData.numBins; i++)
      setSortBin(i,binSize,numShapes);

   smViewData.init    = true;
}

void Forest::processSquare(F32 midDist, ForestGrid &fg, Point3F cameraPos, F32 xOffset, F32 yOffset)
{
   const S32 longBin  = mFloor((midDist + smViewData.maxCellRadius) * smViewData.invBinSize);
   const S32 shortBin = mFloor((midDist - smViewData.maxCellRadius) * smViewData.invBinSize);

   const Point3F off(xOffset, yOffset, 0);

   for (S32 i=0; i<fg.count; i++)
   {
      PROFILE_START(Forest_processSquare_rasterObj);
      ForestItem * forestItem = &mForestItems[fg.idx+i];
      
      const S32 shapeNum = forestItem->getShapeNum();
      const Point3F fiPos = forestItem->getPosition();

      const Point3F delta = (off + fiPos) - cameraPos;
      const F32 dist = getMax(0.f,delta.len() - forestItem->getRadius());
      
      if (dist>=smViewData.visFarDist)
      {
         // we want a round end so that far things don't
         // pop in and out as you turn...
         PROFILE_END();
         continue;
      }

      S32 bin = longBin; // which sort bin do we belong to?
      
      if (longBin != shortBin)   // different bins...brute force it...
         bin = dist * smViewData.invBinSize;

      // Sanity, we still sometimes get a messed up item in wrong bin here...
      bin = mClamp(bin, 0, smViewData.binDetailLists.size() - 1);

      S32 dl = smViewData.binDetailLists[bin][shapeNum].minDL;
      
      if (dl != smViewData.binDetailLists[bin][shapeNum].maxDL)    // select detail level starting at dl
         dl = getDetailLevel(shapeNum,dist*forestItem->getInvScale(),dl);

      if (dl>=0)
      {
         ViewData::DetailLevel & detail = smViewData.detailData[shapeNum][dl];
         S32 tsDL1  = detail.dl1;
         const F32 alpha1 = detail.alpha1 + detail.slope1 * detail.invRange * (forestItem->getInvScale()*dist-detail.startDist);
         S32 tsDL2  = detail.dl2;
         const F32 alpha2 = detail.alpha2 + detail.slope2 * detail.invRange * (forestItem->getInvScale()*dist-detail.startDist);
         
         if (alpha1<0.0f)
            // need to do this so that billboard details don't get treated like 
            // nontranslucent details when alpha is negative (see isBillboard below)
            tsDL1 = -1;
         
         if (alpha2<0.0f)
            // need to do this so that billboard details don't get treated like 
            // nontranslucent details when alpha is negative (see isBillboard below)
            tsDL2 = -1;

         if(!((tsDL1>=0 && alpha1>0.0f) || (tsDL2>=0 && alpha2>0.0f)))
            continue;

         // add shape to render lists...
         TSShapeInstance * shapeInstance = forestItem->getShapeInstance();
         
         const bool isBillboard = (tsDL1>=0 && alpha1>0.0f && shapeInstance->getShape()->details[tsDL1].subShapeNum<0) ||
                                  (tsDL2>=0 && alpha2>0.0f && shapeInstance->getShape()->details[tsDL2].subShapeNum<0);
         const bool fading = (tsDL1>=0 && alpha1<1.0f) || (tsDL2>=0 && alpha2<1.0f );
         
         const S32 firstTObject = shapeInstance->getShape()->subShapeFirstTranslucentObject[0];
         
         const bool separateTranslucency = !isBillboard && firstTObject>0 && !fading;

         if (separateTranslucency)   // add to non-translucent list
            mNonTranslucentLists[forestItem->getShapeNum()].push_back(forestItem);

         // add to correct sort bin
         if (isBillboard || firstTObject<shapeInstance->getShape()->subShapeNumObjects[0] || fading)
            mTranslucentLists[bin].addItem(forestItem);
         
         // set detail info.
         forestItem->setForestDetails(tsDL1, alpha1, tsDL2, alpha2,separateTranslucency);
         forestItem->setSquaredDistance(dist*dist);
         forestItem->setOffsets(xOffset,yOffset);

         const F32 dz        = forestItem->getPosition().z-cameraPos.z;
         const F32 fogAmount = smState->getHazeAndFog(dist,dz);

         forestItem->setFog(fogAmount);
      }

      PROFILE_END();
   }
}

F32 Forest::getSquareDistance(Point3F cameraPos, const Point3F& minPoint, const Point3F& maxPoint, F32* zDiff)
{
   Point3F vec;
   if(cameraPos.z < minPoint.z)
      vec.z = minPoint.z - cameraPos.z;
   else if(cameraPos.z > maxPoint.z)
      vec.z = maxPoint.z - cameraPos.z;
   else
      vec.z = 0;

   if(cameraPos.x < minPoint.x)
      vec.x = minPoint.x - cameraPos.x;
   else if(cameraPos.x > maxPoint.x)
      vec.x = cameraPos.x - maxPoint.x;
   else
      vec.x = 0;

   if(cameraPos.y < minPoint.y)
      vec.y = minPoint.y - cameraPos.y;
   else if(cameraPos.y > maxPoint.y)
      vec.y = cameraPos.y - maxPoint.y;
   else
      vec.y = 0;

   *zDiff = vec.z;

   return vec.len();
}

//-----------------------------------------------------------------------------

F32 Forest::getTerrHeight(Point2F lamePos, Point3F *normal)
{
   TerrainBlock* pBlock = GlobDock::getTerrain(isServerObject());
   Point3F pos(lamePos.x, lamePos.y, 0);

   if (pBlock != NULL) 
   {
      Point3F terrPos = pos;
      pBlock->getWorldTransform().mulP(terrPos);
      terrPos.convolveInverse(pBlock->getScale());

      F32 height;
      bool res;

      if(normal)
         res = pBlock->getNormalAndHeight(Point2F(terrPos.x, terrPos.y), normal, &height);
      else
         res = pBlock->getHeight(Point2F(terrPos.x, terrPos.y), &height);

      if (res)
      {
         terrPos.z = height;
         terrPos.convolve(pBlock->getScale());
         pBlock->getTransform().mulP(terrPos);
      }

      return height;
   }
   else
      return 0;
}
