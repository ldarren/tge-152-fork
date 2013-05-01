//-----------------------------------------------
// Synapse Gaming - Lighting Code Pack
// Copyright © Synapse Gaming 2003 - 2005
// Written by John Kabus
//
// Overview:
//  Code from the Lighting Pack's (Torque Lighting Kit)
//  lighting system, which was modified for use
//  with Constructor.
//-----------------------------------------------
#ifndef _SGLIGHTMAP_H_
#define _SGLIGHTMAP_H_

//#include "constructor/lightingSystem/stockLightingPlugin/stockLightingSchema.h"
//#include "constructor/lightingSystem/stockLightingPlugin/sgLightManager.h"
#include "lightingSystem/sgLighting.h"
#include "lightingSystem/sgBinaryVolumePartitionTree.h"
#include "math/mBox.h"


class sgShadowObjects
{
public:
   /// used for storing static mesh geometry for fast shadow detection...
   struct sgStaticMeshTri
   {
      Point3D sgVert[3];
      PlaneF sgPlane;
      Box3F sgBox;
   };
   /// BVPT to static mesh geometry mapping typedef...
   typedef BVPT<sgStaticMeshTri *> sgStaticMeshBVPT;
   /// object info...
   struct sgObjectInfo
   {
      MatrixF sgInverseTransform;
      sgStaticMeshBVPT sgBVPT;
      Vector<sgStaticMeshTri> sgTris;
   };
   /// static mesh to object mapping typedef...
   typedef hash_multimap<void *, sgObjectInfo *> sgStaticMeshBVPTEntry;

   /// s and e are in interior space, not static mesh space...
   static bool sgCastRayStaticMesh(Point3F s, Point3F e, ConstructorSimpleMesh *staticmesh);
   static void sgClearStaticMeshBVPTData();

private:
   /// master object info storage...
   static Vector<sgObjectInfo *> sgObjectInfoStorage;
   /// static mesh to BVPT mapping...
   static sgStaticMeshBVPTEntry sgStaticMeshBVPTMap;

public:
   static VectorPtr<SceneObject *> sgObjects;
   static void sgGetObjects(SceneObject *obj);
};

class sgColorMap
{
public:
   U32 sgWidth;
   U32 sgHeight;
   ColorF *sgData;
   sgColorMap(U32 width, U32 height)
   {
      sgWidth = width;
      sgHeight = height;
      sgData = new ColorF[(width * height)];
      dMemset(sgData, 0, (width * height * sizeof(ColorF)));
   }
   ~sgColorMap() {delete[] sgData;}
   void sgFillInLighting();
   void sgBlur();
   void sgTestDump();
};

/**
* The base class for generating mission level or real-time light maps
* for any sceneObject.  All actual work is performed in the descendent
* class' sgLightMap::sgCalculateLighting method.
*/
class sgLightMap
{
protected:
   U32 sgWidth;
   U32 sgHeight;
   /// The light map color buffer.
   sgColorMap *sgTexels;
public:
   /// The world space position that the texture space coord (0, 0) represents.
   Point3D sgWorldPosition;
   /// Defines the world space directional change
   /// corresponding to a change of (+1, 0) in the light map texture space.
   /// Similar to the tangent vector in dot3 bump mapping.
   Point3D sgLightMapSVector;
   /// Defines the world space directional change
   /// corresponding to a change of (0, +1) in the light map texture space.
   /// Similar to the binormal vector in dot3 bump mapping.
   Point3D sgLightMapTVector;
   sgLightMap(U32 width, U32 height)
   {
      sgWidth = width;
      sgHeight = height;
      sgWorldPosition = Point3D(0.0f, 0.0f, 0.0f);
      sgLightMapSVector = Point3D(0.0f, 0.0f, 0.0f);
      sgLightMapTVector = Point3D(0.0f, 0.0f, 0.0f);
      sgTexels = new sgColorMap(width, height);
   }
   ~sgLightMap()
   {
      delete sgTexels;
   }
   /// Object specific light mapping calculations are done here.
   virtual void sgCalculateLighting(LightInfo *light) = 0;
protected:
   struct sgStaticMeshInfo
   {
      Box3F sgWorldBounds;
      ConstructorSimpleMesh *sgStaticMesh;
      InteriorInstance *sgInteriorInstance;
   };
   typedef BVPT<sgStaticMeshInfo> sgStaticMeshBVPT;

   Vector<SceneObject *> sgIntersectingSceneObjects;
   //Vector<sgStaticMeshInfo> sgIntersectingStaticMeshObjects;
   sgStaticMeshBVPT sgIntersectingStaticMeshObjects;
   void sgGetIntersectingObjects(const Box3F &surfacebox, const SceneObject *skipobject);
};

/**
* Used to generate light maps on interiors.  This class will
* calculate one surface at a time (using sgPlanarLightMap::sgSurfaceIndex).
*/
class sgPlanarLightMap : public sgLightMap
{
public:
   struct sgSmoothingVert
   {
      Point3F sgVert;
      Point3F sgVect;
      Point3F sgNorm;
   };
   struct sgSmoothingTri
   {
      Point3F sgSDerivative;
      Point3F sgTDerivative;
      sgSmoothingVert sgVerts[3];
   };
   struct sgLexel
   {
      bool shadowTestOnly;
      Point2D lmPos;
      Point3F worldPos;
      Point3F normal;
   };
   enum sgLightingPass
   {
      sglpInner = 0,
      sglpOuter,
      sglpCount
   };
   struct sgOccluder
   {
      void *sgObject;
      S32 sgSurface;
   };
	enum sgAdjacent
	{
		sgaTrue,
		sgaFalse,
		// I like this one - fuzzy logic in action "umm... maybe?"... :)
		sgaMaybe
	};
public:
   /// Surface to generate light map.
   PlaneF surfacePlane;
   Vector<sgSmoothingVert> triStrip;
   bool sgUseSmoothing;
   S32 sgSAxis;
   S32 sgTAxis;
   bool sgFlippedWindings;
   Box3F sgSurfaceBox;
   Vector<sgLexel> sgInnerLexels;
   Vector<sgLexel> sgOuterLexels;
   InteriorInstance *sgInteriorInstance;
   Interior *sgInteriorDetail;
   S32 sgInteriorSurface;
   ConstructorSimpleMesh *sgInteriorStaticMesh;
   sgPlanarLightMap(U32 width, U32 height, InteriorInstance *interior, Interior *detail,
      S32 surface, ConstructorSimpleMesh *staticmesh, PlaneF surfaceplane, const Vector<sgSmoothingVert> &tristrip)
      : sgLightMap(width, height)
   {
      sgDirty = false;
      sgUseSmoothing = false;
      sgFlippedWindings = false;
      surfacePlane = surfaceplane;
      triStrip.clear();
      triStrip.merge(tristrip);
      sgSurfaceBox.min = Point3F(F32_MAX, F32_MAX, F32_MAX);
      sgSurfaceBox.max = Point3F(-F32_MAX, -F32_MAX, -F32_MAX);
      sgInteriorInstance = interior;
      sgInteriorDetail = detail;
      sgInteriorSurface = surface;
      sgInteriorStaticMesh = staticmesh;
   }
   /// Transfer the light map to a GBitmap and blur.
   void sgMergeLighting(GBitmap *lightmap, U32 xoffset, U32 yoffset);
   /// See: sgLightMap::sgCalculateLighting.
   void sgSetupLighting();
   virtual void sgCalculateLighting(LightInfo *light);
   bool sgIsDirty() {return sgDirty;}
protected:
   bool sgDirty;
   static U32 sgCurrentOccluderMaskId;
   void sgBuildDerivatives(sgSmoothingTri &tri);
   void sgBuildLexels(const Vector<sgSmoothingTri> &tris);
   bool sgCastRay(Point3F s, Point3F e, SceneObject *obj, Interior *detail, ConstructorSimpleMesh *sm, sgOccluder &occluderinfo);
   //bool sgIsValidOccluder(const sgOccluder &occluderinfo, Vector<sgOccluder> &validoccluders, bool isinnerlexel);
   bool sgIsValidOccluder(const sgOccluder &occluderinfo, hash_multimap<void *, sgOccluder> &validoccluders, bool isinnerlexel);

	/// Try to avoid false shadows by ignoring direct neighbors.
	sgAdjacent sgAreAdjacent(U32 surface1, U32 surface2);
};

/**
* Used to generate terrain light maps.
*/
class sgTerrainLightMap : public sgLightMap
{
public:
   TerrainBlock *sgTerrain;
   sgTerrainLightMap(U32 width, U32 height, TerrainBlock *terrain)
      : sgLightMap(width, height)
   {
      sgTerrain = terrain;
   }
   void sgMergeLighting(ColorF *lightmap);
   /// See: sgLightMap::sgGetBoundingBox.
   virtual void sgCalculateLighting(LightInfo *light);
};


#endif//_SGLIGHTMAP_H_
