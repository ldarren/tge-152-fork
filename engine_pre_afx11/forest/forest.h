//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#ifndef  _H_FOREST_
#define  _H_FOREST_

#include "forest/forestItem.h"
#include "core/frameAllocator.h"
#include "terrain/terrData.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "lightingSystem/sgLightManager.h"

class TSShapeInstance;
struct ForestItemData;
class ForestBatcher;
class ForestTextureSheet;

class ForestEntry
{
   StringTableEntry  forestItemName;     // Name of ForestItem found in this forest. Must match datablock name defined in forestItem.cs
   StringTableEntry  tileType;           // Tile type on which ForestItem can exist
   ForestItemData  * infoBlock;          // Datablock for forestItem for this entry 
   F32               probability;        // Probablity that ForestItem exists on this tileType
   U16               density;            // Maximum found per tile
   GBitmap           *densityMap;        // Bitmap modulating maximum found per tile.

   // index of tile type in terrains material list...
   S32 tileIndex;
   S32 shapeIndex;

public:

   ForestEntry( const char * in_forestItemName, const char * in_tileType, F32 in_probability, U16 in_density, ForestItemData * in_infoBlock, GBitmap *in_densityMap)
   {
      forestItemName   = StringTable->insert(in_forestItemName,  true);
      tileType         = StringTable->insert(in_tileType,   true);
      tileIndex        = 999999; // cause a crash if not set properly...
      shapeIndex       = -1;     // we'll look it up first time we need it...
      probability      = in_probability;
      density          = in_density;
      infoBlock        = in_infoBlock;      
      densityMap       = in_densityMap;
   }
   StringTableEntry getName()       {return forestItemName;  }
   StringTableEntry getTileType()   {return tileType;        }
   S32              getTileIndex()  {return tileIndex;       }
   S32              getShapeIndex() {return shapeIndex;      }
   F32              getProbability(){return probability;     }
   U16              getDensity()    {return density;         }
   GBitmap        * getDensityMap() {return densityMap;         }
   ForestItemData * getInfoBlock()  {return infoBlock;       }

   void             setTileIndex(S32 ti)  { tileIndex = ti;  }
   void             setShapeIndex(S32 si) { shapeIndex = si; }
};

typedef Vector<ForestEntry *>  ForestEntryList;        // Vector that holds Associated Forest Items and terrain Tiles

struct ForestGrid
{
   S32  idx;
   S32  count;
   bool clearCut;
   ForestGrid()
   {
      idx      = 0;
      count    = 0;
      clearCut = false;
   }
};

class Forest : public SceneObject
{
   friend class ForestBatcher;

   /// Used by the visibility algorithm.
   struct SquareStackNode
   {
      U32     clipFlags;
      Point2I pos;
      U32     level;
   };

   // the following data is dependent on the current view settings...
   // that is, the number of sort bins and the current field of view (fov)
   struct ViewData
   {
      bool init;
      F32 fov;
      F32 visDist;
      F32 farDist;
      F32 visFarDist;
      TerrainBlock * terrain; // don't take this for granted...use it to check if terrain has changed
      Forest * forest;        // ditto...
      F32 backupDist;
      F32 farWidth;
      
      S32 numBins;
      F32 invBinSize;
      F32 maxCellRadius;
      F32 maxShapeRadius;

      Vector<F32> minScale;
      F32 billboardDistance;
      F32 alphaFadeOut;
      F32 alphaOutBB;
      F32 alphaInBB;
      
      // binDetailList gives detail level associated w/ each bin
      // usage:  binDetailLists[bin][shapeNum]
      struct BinDetailLevels
      {
         S32 minDL;
         S32 maxDL;
      };
      typedef Vector<BinDetailLevels> BinDetails;    // indexed by shape number
      Vector<BinDetails> binDetailLists; // indexed by bin number

      // detail data describes how to detail each shape...
      struct DetailLevel
      {
         bool interpolate1;
         S32 dl1;
         F32 alpha1;
         F32 slope1;

         F32 startDist;
         F32 endDist;
         F32 invRange;

         bool interpolate2;
         S32 dl2;
         F32 alpha2;
         F32 slope2;
      };
      typedef Vector<DetailLevel> DetailData;
      Vector<DetailData> detailData; // indexed by shape number

      void setListSizes(S32 numBins,S32 numShapes);

      ViewData() { init = false; }
      ~ViewData() { setListSizes(0,0); }
   };

   S32 getDetailLevel(S32 shapeNum, F32 dist, S32 startCheck = 0);
   void setSortBin(S32 bin, F32 binSize, S32 numShapes);
   void setupDetailData(S32 shapeNum);
   void setViewData(F32 fov, F32 fogDist, F32 visDist);
   void resetShapeData();
   void updateShapeData(S32 shapeIndex, ForestItem *);

private:

   F32 getTerrHeight(Point2F pos, Point3F *normal=NULL);

   typedef SceneObject Parent;
   typedef Vector<StringTableEntry>       ForestShapes;         // Vector that holds forest shapes
   typedef Vector<StringTableEntry>       ForestInstNames;      // Vector that holds the names of the ShapeInstances
   typedef Vector<TSShapeInstance *>      ForestInstances;      // Vector that holds forest shapeInstances. 
   typedef Vector<ForestItem>             ForestItems;          // Vector that holds forest items..
   typedef Vector<ForestItemData *>       ForestItemInfo;       // Vector that holds forest item datablocks
   typedef Vector<ForestTextureSheet *>   ForestTextureSheets;  // Vector that holds texture sheets for forest BB rendering
   typedef Vector<ForestEntryList *>      ForestEntries;        // Vector that holds lists of forest entries. Sorted by maxItems
   
   static Forest * smServerForest;
   static Forest * smClientForest;
   static ViewData smViewData;
   static SceneState *smState;
   
   static S32     mNumClipPlanes;
   static PlaneF  mClipPlane[5];
   static void    buildClippingPlanes(MatrixF camToObj, bool flipClipPlanes);
   static S32     testSquareVisibility(Point3F &min, Point3F &max, S32 mask, F32 expand);
   F32            getSquareDistance(Point3F cameraPos, const Point3F& minPoint, const Point3F& maxPoint, F32* zDiff);

   enum
   {
      GRID_SIZE = TerrainBlock::BlockSize * TerrainBlock::BlockSize,
   };

   bool            mInitialWait;
   MRandom         mRandomNumber;
   S32             mRandomSeed;                     // Seed is saved off and sent to client objects for generating the same forest
   S32             mForestCount;                    // How many trees does the random seed generate?
   S32             mTotalItems;                     // How many forest Items actually got created?
   ForestItemInfo  mForestItemInfo;                 // List of all the datablocks used by the forestItems in this forest
   ForestShapes    mForestShapes;                   // List of all unique shape names in this forest
   ForestEntries   mForestEntries;                  // Vector that holds vectors of forest entries
   F32             mTreeDistance;                   // 0-1, how far along between when fogging starts and visibility ends do we stop drawing trees
   
   static ForestInstances smForestInstances;        // List of all shape Instances
   static ForestInstNames smForestInstanceNames;    // List of all shape Instance Names
   static ForestTextureSheets smForestTSheets;      /// List of all texture sheets

   // Table of all forest Items x,y gives grid coordinates      
   ForestGrid      mForestGrid[GRID_SIZE];  
   ForestItems     mForestItems;
   
   typedef Vector<ForestItem*> SimpleForestRenderList;

   // render lists --
   // usage: nonTranslucentLists[shapeNum] returns lists of non-translucent forestItems of that shape to render
   // usage: translucentLists[bin] returns lists of translucent forestItems in that bin (diff. shapes) to render
   struct ForestRenderList
   {
      bool mTrackBounds;
      SimpleForestRenderList mItems;

      ForestRenderList()
      {
         mItems.clear();
         mTrackBounds = true;
      }

      ~ForestRenderList()
      {
         mItems.clear();
      }

      // In an effort to optimize plane checks, each bin tracks its bounds.
      Box3F mBounds;

      void addItem(ForestItem *fi)
      {
         if(mTrackBounds)
         {
            static Box3F wBox;

            wBox = fi->getObjBox();
            const MatrixF &xfrm  = fi->getTransform();

            xfrm.mul(wBox);

            // Deal with it as appropriate.
            if(mItems.size() == 0)
            {
               // Reset the box.
               mBounds = wBox;
            }
            else  
            {
               // Extend the box.
               mBounds.intersect(wBox);
            }
         }

         mItems.push_back(fi);
      }

      void clear()
      {
         mItems.clear();
      }
   };


   Vector<SimpleForestRenderList> mNonTranslucentLists; // indexed by shapeNumber
   Vector<ForestRenderList>       mTranslucentLists;    // indexed by bin number (distance)

   void setupTexturing();
   void setRenderListSizes(S32 numBins, S32 numShapes);

   void prepBBRender();
   void doBBRender(ForestItem *fi, TSShapeInstance *si, S32 dl, F32 alpha);
   void cleanBBRender();

   void prepMeshRender();
   void doMeshRender(ForestItem *fi, TSShapeInstance *si, S32 dl, F32 alpha);
   void cleanMeshRender();

   static void recursiveAddTree(SceneState::RenderBSPNode* pNode, ForestItem *fi);
   void renderNonTranslucentItems();
   void renderSpan(SimpleForestRenderList *list, F32 nearSq, F32 farSq);
   void renderSpan(SimpleForestRenderList *list);

   void renderImage(SceneRenderImage* image);
   void processNodeSpans(SceneState::RenderBSPNode& rNode);

   void renderOne(ForestItem *fi, TSShapeInstance * si, S32 dl, F32 alpha);
   void initMaterials();

   // debug render routines...
   void renderDebug();
   void renderDebugRadius(ForestItem *);

   /// Render count this frame.
   static U32  smRenderCount;

   enum MaskBits
   {
      InitialUpdateMask  = BIT(0),
      NextFreeMask       = InitialUpdateMask << 1
   };

   S32              createForestItem(ForestEntry * in_forestEntry, Point2I in_position);
   ForestItemData * findForestData(const char * in_typeName);
   ForestEntry    * findForestEntry(const char * in_forestEntryName);  // returns the first entry it finds with the same name does NOT check tile types
   bool             findShape( const char * in_forestShape);
   void             getCollisionPoints(Point2F in_startPt, Point2F in_endPt, F32 in_width, Vector<ForestItem *> &in_forestList);
   S32              getShapeIndex( const char * in_forestShape );
   S32              generateForestItems(ForestEntry * in_forestEntry, Point2I in_grid);
   bool             gridToWorld(const Point2I & gPos, Point3F & wPos);
   bool             worldToGrid(const Point3F & wPos, Point2I & gPos);
   F32              getTerrainSquareSize() const;
   F32              getTerrainBlockSize() const;
   F32              getTerrainMaxHeight() const;

   F32              mItemProbability;
   F32              mItemDensity;

   S32              mMinOcclusionLevel;
   S32              mMaxOcclusionLevel;
   bool             mSingleBlock;

   /// Get all the ForestItems in a given bounding box.
   void             getItems(Box3F box, Vector<ForestItem *> &in_forestItemList);

   static void forestCastRayBinHelper(const Point3F &start, const Point3F &end, RayInfo* info, F32 &currentT, Forest *thisPtr, ForestGrid &fg, const F32 &xOffset, const F32 &yOffset);
   Convex *mConvexList;
public:

   static F32   smMaxBillboardDistance;
   static F32   smMaxBillboardDistanceScale;
   static S32   smForestGrowthSize; // amount to increment forest arrays by when we grow them.
   static F32   smItemProbability; // these are pref variables, they are used to set
   static F32   smItemDensity;     // values of like named member variables
   static bool  smFogBillboards;   // actually fog billboards, vs. alpha out
   
   static  void         consoleInit();
   static  Forest *     getServerForest(){return smServerForest;}
   static  Forest *     getClientForest(){return smClientForest;}
   static  void         initPersistFields();
   static  S32 FN_CDECL compareDistanceFunc(const void* p1, const void* p2);
   static  void         renderParametersUpdated();
   

   DECLARE_CONOBJECT(Forest);
   Forest();
   ~Forest();

   void        addForestItem(const char * in_forestItemName, Point3F in_position);
   void        addForestClearing(F32 in_xPos, F32 in_yPos, F32 in_width, F32 in_height);
   void        addForestEntry( const char * in_forestEntryName, const char * in_tileType, F32 in_prop, U16 in_density, GBitmap *densityMap);
   void        createForest();
   S32         createShapeInstances();

   bool        getNearestTree(Point3F in_playersPos, F32 in_radius, Point3F *nearest, ForestItemData *fid);
   void        getCollisionPoints(Point3F in_startPt, Point3F in_endPt, F32 in_width, Vector<Point3F> &in_vectorList);
   void        getCollisionPoints(Point2F in_startPt, Point2F in_endPt, F32 in_width, Vector<Point3F> &in_vectorList);
   void        getCollisionPoints(Point3F in_startPt, Point3F in_endPt, F32 in_width, Vector<ForestItem *> &in_forestItemList);

   bool        onAdd();
   void        onRemove();
   void        scriptOnAdd();
   
   bool        prepRenderImage(SceneState* state, const U32 stateKey, const U32 startZone, const bool modifyBaseState);
   void        renderObject(SceneState* state, SceneRenderImage*);
   void        processSquare(F32 midDist, ForestGrid &fg, Point3F cameraPos, F32 xOffset, F32 yOffset);

   // Network
   U32         packUpdate(NetConnection *, U32 mask, BitStream *stream);
   void        unpackUpdate(NetConnection *, BitStream *stream);

   // Collision
   bool        castRay(const Point3F &start, const Point3F &end, RayInfo* info);
   bool        buildPolyList(AbstractPolyList* polyList, const Box3F &box, const SphereF& sphere);
   void        buildConvex(const Box3F& box, Convex* convex);

   // Lighting
   static void calculateAmbientOcclusion(const Point3F &pos, const Point3F &norm, F32    &col);

   // Hacks to support latest lighting kit
   void installLights()
   {
      gClientSceneGraph->getLightManager()->sgSetupLights(this);
   }
   
   void uninstallLights()
   {
      gClientSceneGraph->getLightManager()->sgResetLights();
   }
};

inline void Forest::recursiveAddTree(SceneState::RenderBSPNode* pNode, ForestItem *fi)
{
   // Determine if we're in the right place.
   if(pNode->frontIndex == 0xFFFF)
   {
      // Just add it in this case.
      // We actually got all the way on one side of the plane or the other,
      // good deal. So let's stuff the whole list in, quick-like.
      SimpleForestRenderList *list;
      if(!pNode->userData)
      {
         list = (SimpleForestRenderList*)FrameAllocator::alloc(sizeof(SimpleForestRenderList));
         constructInPlace(list);
         pNode->userData = (void*)list;
      }
      else
         list = (SimpleForestRenderList*)pNode->userData;

      list->push_back(fi);
   }
   else
   {
      SceneState::RenderBSPNode *pFront = &smState->mTranslucentBSP[pNode->frontIndex];
      SceneState::RenderBSPNode *pBack  = &smState->mTranslucentBSP[pNode->backIndex];

      // Otherwise, recurse.
      if(pNode->plane.distToPlane(fi->getPosition()) > 0)
         recursiveAddTree(pFront, fi);
      else
         recursiveAddTree(pBack, fi);
   }
}

#endif