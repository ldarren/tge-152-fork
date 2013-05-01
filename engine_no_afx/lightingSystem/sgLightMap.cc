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
#include "math/mBox.h"
#include "math/mathUtils.h"
#include "sceneGraph/sceneGraph.h"
#include "terrain/terrData.h"
#include "platform/profiler.h"
#include "interior/interior.h"
#include "interior/interiorInstance.h"
#include "lightingSystem/sgLightMap.h"
#include "lightingSystem/sgLightingModel.h"

#include "math/mRandom.h"
#include "math/mathUtils.h"
#include "util/triRayCheck.h"

/// used to calculate the start and end points
/// for ray casting directional light.
#define SG_STATIC_LIGHT_VECTOR_DIST    100.0f
#define SG_MIN_LEXEL_INTENSITY			0.0039215f

#define SG_STATICMESH_BVPT_SHADOWS

bool badtexgen = false;


Vector<sgShadowObjects::sgObjectInfo *> sgShadowObjects::sgObjectInfoStorage;
sgShadowObjects::sgStaticMeshBVPTEntry sgShadowObjects::sgStaticMeshBVPTMap;
VectorPtr<SceneObject *> sgShadowObjects::sgObjects;

U32 sgPlanarLightMap::sgCurrentOccluderMaskId = 0;


inline F32 sgGetDistanceSquared(const Point3F &linepointa,
                                const Point3F &linepointb, const Point3F &point)
{
   Point3F vect = linepointb - linepointa;
   F32 dist = vect.lenSquared();
   if(dist <= 0.0f)
      return 100000.0f;

   F32 tang = ((point.x + linepointa.x) * (linepointb.x + linepointa.x)) +
      ((point.y + linepointa.y) * (linepointb.y + linepointa.y)) +
      ((point.z + linepointa.z) * (linepointb.z + linepointa.z));

   tang /= dist;
   if(tang >= 1.0f)
   {
      vect = linepointb - point;
      return vect.lenSquared();
   }

   if(tang <= 0.0f)
   {
      vect = linepointa - point;
      return vect.lenSquared();
   }

   vect = linepointa + (vect * tang);
   vect = vect - point;
   return vect.lenSquared();
}

inline bool sgDegenerateTriangle(const Point3F &k, const Point3F &j, const Point3F &l)
{
   const F32 ax = k.x-j.x;
   const F32 ay = k.y-j.y;
   const F32 az = k.z-j.z;
   const F32 bx = l.x-j.x;
   const F32 by = l.y-j.y;
   const F32 bz = l.z-j.z;
   const F32 x = ay*bz - az*by;
   const F32 y = az*bx - ax*bz;
   const F32 z = ax*by - ay*bx;
   return (x*x + y*y + z*z) == 0.0f;
}

void sgObjectCallback(SceneObject *object, void *vector)
{
   VectorPtr<SceneObject *> *intersectingobjects = static_cast<VectorPtr<SceneObject *> *>(vector);
   intersectingobjects->push_back(object);
}

void sgShadowObjects::sgGetObjects(SceneObject *obj)
{
   sgObjects.clear();
   obj->getContainer()->findObjects(ShadowCasterObjectType, &sgObjectCallback, &sgObjects);
}

bool sgShadowObjects::sgCastRayStaticMesh(Point3F s, Point3F e, ConstructorSimpleMesh *staticmesh)
{
   sgStatistics::sgStaticMeshCastRayCount++;

   sgStaticMeshBVPTEntry *entry = sgStaticMeshBVPTMap.find(U32(staticmesh));
   AssertFatal((entry), "hash_map should always return an entry!");

   // is the BVPT there?
   if(!entry->info)
   {
      sgObjectInfo *objinfo = new sgObjectInfo();
      entry->info = objinfo;
      sgObjectInfoStorage.push_back(objinfo);

      objinfo->sgInverseTransform = staticmesh->transform;
      objinfo->sgInverseTransform.inverse();

      // get the tri count...
      U32 tricount = 0;
      for(U32 p=0; p<staticmesh->primitives.size(); p++)
      {
         const ConstructorSimpleMesh::primitive &prim = staticmesh->primitives[p];
         if(prim.alpha)
            continue;
         tricount += prim.count;
      }

      objinfo->sgTris.reserve(tricount);

      // add the tris...
      for(U32 p=0; p<staticmesh->primitives.size(); p++)
      {
         const ConstructorSimpleMesh::primitive &prim = staticmesh->primitives[p];
         if(prim.alpha || (prim.count < 3))
            continue;

         PlaneF plane(staticmesh->verts[0], staticmesh->verts[1], staticmesh->verts[2]);
         Point3F norm = (staticmesh->norms[0] + staticmesh->norms[1] + staticmesh->norms[2]) * 0.3333f;
         bool flip = (mDot(plane, norm) < 0.0f);

         for(U32 t=2; t<prim.count; t++)
         {
            U32 baseindex = prim.start + t;
            objinfo->sgTris.increment();
            sgStaticMeshTri &tri = objinfo->sgTris.last();

            bool winding = (t & 0x1);
            if(flip)
               winding = !winding;

            Point3F *v0, *v1, *v2;
            if(winding)
            {
               v0 = &staticmesh->verts[baseindex-1];
               v1 = &staticmesh->verts[baseindex-2];
               v2 = &staticmesh->verts[baseindex];
            }
            else
            {
               v0 = &staticmesh->verts[baseindex-2];
               v1 = &staticmesh->verts[baseindex-1];
               v2 = &staticmesh->verts[baseindex];
            }

            tri.sgVert[0].set(v0->x, v0->y, v0->z);
            tri.sgVert[1].set(v1->x, v1->y, v1->z);
            tri.sgVert[2].set(v2->x, v2->y, v2->z);
            tri.sgPlane.set((*v0), (*v1), (*v2));

            tri.sgBox.min = (*v0);
            tri.sgBox.max = (*v0);
            tri.sgBox.min.setMin((*v1));
            tri.sgBox.max.setMax((*v1));
            tri.sgBox.min.setMin((*v2));
            tri.sgBox.max.setMax((*v2));
         }
      }

      // find the avoid axis if any...
      sgStaticMeshBVPT::axisType avoidaxis = sgStaticMeshBVPT::atNone;
      U32 maxspancount = 0;
      Point3F mid;

      tricount = objinfo->sgTris.size();
      staticmesh->bounds.getCenter(&mid);

      for(S32 a=0; a<3; a++)
      {
         Point3F normal(0.0f, 0.0f, 0.0f);
         normal[a] = 1.0f;
         PlaneF plane(mid, normal);

         U32 spancount = 0;
         for(U32 t=0; t<tricount; t++)
         {
            sgStaticMeshTri &tri = objinfo->sgTris[t];
            bool frontmin = (plane.distToPlane(tri.sgBox.min) > 0.0f);
            bool frontmax = (plane.distToPlane(tri.sgBox.max) > 0.0f);

            if(frontmin == frontmax)
               continue;

            spancount++;
         }

         if((tricount > 0) && (maxspancount < spancount) && ((F32(spancount) / F32(tricount)) > 0.5f))
         {
            maxspancount = spancount;
            avoidaxis = sgStaticMeshBVPT::axisType(a);
         }
      }

      objinfo->sgBVPT.init(staticmesh->bounds, avoidaxis);

      // need to do this last so any vector copy/resize doesn't kill the pointers...
      for(U32 t=0; t<objinfo->sgTris.size(); t++)
      {
         sgStaticMeshTri &tri = objinfo->sgTris[t];
         objinfo->sgBVPT.storeObject(tri.sgBox, &tri);
      }
   }

   // convert to static mesh space...
   sgObjectInfo *objinfo = entry->info;
   objinfo->sgInverseTransform.mulP(s);
   objinfo->sgInverseTransform.mulP(e);
   s.convolveInverse(staticmesh->scale);
   e.convolveInverse(staticmesh->scale);

   // early out test...
   F32 t;
   Point3F n;
   if(!staticmesh->bounds.collideLine(s, e, &t, &n))
      return false;

   // get the likely occluders...
   static S32 lastlistsize = 100;
   sgStaticMeshBVPT::objectList list;
   list.reserve(lastlistsize);
   objinfo->sgBVPT.collectObjectsClipped(s, e, list);
   lastlistsize = getMax(lastlistsize, list.size());

   sgStatistics::sgStaticMeshBVPTPotentialOccluderCount += list.size();

   Point3D s64(s.x, s.y, s.z);
   Point3D vect64 = Point3D(e.x, e.y, e.z) - s64;
   Point3F vect(vect64.x, vect64.y, vect64.z);

   // now cast against them...
   U32 tc = list.size();
   sgStaticMeshTri **tris = list.address();

   for(U32 i=0; i<tc; i++)
   {
      sgStaticMeshTri *tri = tris[i];

      if(mDot(tri->sgPlane, vect) >= 0.0f)
         continue;

      //stats
      sgStatistics::sgStaticMeshSurfaceOccluderCount++;

      if(castRayTriangle(s64, vect64, tri->sgVert[0], tri->sgVert[1], tri->sgVert[2]))
         return true;
   }

   return false;
}

void sgShadowObjects::sgClearStaticMeshBVPTData()
{
   for(U32 i=0; i<sgObjectInfoStorage.size(); i++)
      delete sgObjectInfoStorage[i];

   sgObjectInfoStorage.clear();
   sgStaticMeshBVPTMap.clear();
}

void sgColorMap::sgFillInLighting()
{
   U32 x, y;
   U32 lastgoodx, lastgoody;
   U32 lastgoodyoffset, yoffset;

   if(LightManager::sgAllowFullLightMaps())
      return;

   U32 lmscalemask = LightManager::sgGetLightMapScale() - 1;

   for(y=0; y<sgHeight; y++)
   {
      // do we have a good y?
      if((y & lmscalemask) == 0)
      {
         lastgoody = y;
         lastgoodyoffset = lastgoody * sgWidth;
      }

      yoffset = y * sgWidth;

      for(x=0; x<sgWidth; x++)
      {
         // do we have a good x?
         if((x & lmscalemask) == 0)
         {
            lastgoodx = x;

            // only bailout if we're on a good y, otherwise
            // all of the x entries are empty...
            if((y & lmscalemask) == 0)
               continue;
         }

         sgData[(yoffset + x)] = sgData[(lastgoodyoffset + lastgoodx)];
      }
   }
}

void sgColorMap::sgBlur()
{
   static F32 blur[3][3] = {{0.1f, 0.125f, 0.1f}, {0.125f, 0.1f, 0.125f}, {0.1f, 0.125f, 0.1f}};
   ColorF *buffer = new ColorF[sgWidth * sgHeight];

   // lets get the values that we don't blur...
   dMemcpy(buffer, sgData, (sizeof(ColorF) * sgWidth * sgHeight));

   for(U32 y=1; y<(sgHeight-1); y++)
   {
      for(U32 x=1; x<(sgWidth-1); x++)
      {
         ColorF &col = buffer[((y * sgWidth) + x)];

         col.set(0.0f, 0.0f, 0.0f);

         for(S32 by=-1; by<2; by++)
         {
            for(S32 bx=-1; bx<2; bx++)
            {
               col += sgData[(((y + by) * sgWidth) + (x + bx))] * blur[bx+1][by+1];
            }
         }
      }
   }

   delete[] sgData;
   sgData = buffer;
}

void sgColorMap::sgTestDump()
{
   GBitmap *bmp = new GBitmap(sgWidth, sgHeight);

   for(U32 y=0; y<sgHeight; y++)
   {
      for(U32 x=0; x<sgWidth; x++)
      {
         ColorI col = sgData[(y * sgWidth) + x];
         bmp->setColor(x, y, col);
      }
   }

   static U32 id = 0;
   FileStream file;
   Platform::createPath("lightmaps/");
   file.open(avar("lightmaps/lightmap_%d.png", id++), FileStream::ReadWrite);
   bmp->writePNG(file);
   file.close();

   delete bmp;
}

void sgLightMap::sgGetIntersectingObjects(const Box3F &surfacebox, const SceneObject *skipobject)
{
   sgIntersectingSceneObjects.clear();
   sgIntersectingStaticMeshObjects.clear();

   bool boxset = false;
   Box3F maxworld = Box3F(F32_MIN, F32_MIN, F32_MIN, F32_MAX, F32_MAX, F32_MAX);
   Vector<sgStaticMeshInfo> meshes;
   meshes.reserve(256);

   for(U32 i=0; i<sgShadowObjects::sgObjects.size(); i++)
   {
      SceneObject *obj = sgShadowObjects::sgObjects[i];
      if(obj == skipobject)
         continue;
      if(!surfacebox.isOverlapped(obj->getWorldBox()))
         continue;

      sgIntersectingSceneObjects.push_back(obj);

      InteriorInstance *inst = dynamic_cast<InteriorInstance *>(obj);
      if(!inst)
         continue;

      Interior *detail = inst->getDetailLevel(0);
      for(U32 sm=0; sm<detail->getStaticMeshCount(); sm++)
      {
         const ConstructorSimpleMesh *smobj = detail->getStaticMesh(sm);
         Box3F bounds = smobj->bounds;
         Box3F worldbounds;

         MathUtils::transformBoundingBox(bounds, smobj->transform, smobj->scale, worldbounds);
         bounds = worldbounds;
         MathUtils::transformBoundingBox(bounds, inst->getTransform(), inst->getScale(), worldbounds);

         if(!surfacebox.isOverlapped(worldbounds))
            continue;

         meshes.increment(1);
         sgStaticMeshInfo &sminfo = meshes.last();

         sminfo.sgWorldBounds = worldbounds;
         sminfo.sgStaticMesh = (ConstructorSimpleMesh *)smobj;
         sminfo.sgInteriorInstance = inst;

         if(!boxset)
         {
            maxworld = worldbounds;
            boxset = true;
         }
         else
         {
            maxworld.max.setMax(worldbounds.max);
            maxworld.min.setMin(worldbounds.min);
         }
      }
   }

   sgIntersectingStaticMeshObjects.init(maxworld);

   for(U32 i=0; i<meshes.size(); i++)
   {
      const sgStaticMeshInfo &sminfo = meshes[i];
      sgIntersectingStaticMeshObjects.storeObject(sminfo.sgWorldBounds, sminfo);
   }
}

bool sgPlanarLightMap::sgCastRay(Point3F s, Point3F e, SceneObject *obj, Interior *detail, ConstructorSimpleMesh *sm, sgOccluder &occluderinfo)
{
   obj->getWorldTransform().mulP(s);
   obj->getWorldTransform().mulP(e);
   s.convolveInverse(obj->getScale());
   e.convolveInverse(obj->getScale());

   if(sm)
   {
#ifdef SG_STATICMESH_BVPT_SHADOWS
      // expects points in interior space...
      if(!sgShadowObjects::sgCastRayStaticMesh(s, e, sm))
         return false;
#else
      MatrixF mat = sm->transform;
      mat.inverse();
      mat.mulP(s);
      mat.mulP(e);
      s.convolveInverse(sm->scale);
      e.convolveInverse(sm->scale);

      F32 t;
      Point3F n;
      if(!sm->bounds.collideLine(s, e, &t, &n))
         return false;

      RayInfo ri;
      if(!sm->castRay(s, e, &ri))
         return false;
#endif

      occluderinfo.sgObject = sm;
      occluderinfo.sgSurface = SG_NULL_SURFACE;
   }
   else if(detail)
   {
      RayInfo ri;
      // not set by all objects...
      ri.face = -1;
      if(!detail->castRay(s, e, &ri))
         return false;

      occluderinfo.sgObject = obj;
      occluderinfo.sgSurface = ri.face;
   }
   else
   {
      RayInfo ri;
      // not set by all objects...
      ri.face = -1;
      if(!obj->castRay(s, e, &ri))
         return false;

      occluderinfo.sgObject = obj;
      occluderinfo.sgSurface = ri.face;
   }

   return true;
}

bool sgPlanarLightMap::sgIsValidOccluder(const sgOccluder &occluderinfo, hash_multimap<void *, sgOccluder> &validoccluders, bool isinnerlexel)
{
   hash_multimap<void *, sgOccluder> *entry = validoccluders.find(calculateCRC(&occluderinfo, sizeof(occluderinfo)));

   if(isinnerlexel)
   {
      entry->info = occluderinfo;
      return true;
   }

   if((entry->info.sgObject == occluderinfo.sgObject) && (entry->info.sgSurface == occluderinfo.sgSurface))
      return true;

   return false;
}

void sgPlanarLightMap::sgSetupLighting()
{
   // stats
   elapsedTimeAggregate time = elapsedTimeAggregate(sgStatistics::sgInteriorSurfaceSetupTime);
   sgStatistics::sgInteriorSurfaceSetupCount++;

   // get tranformed points...
   U32 windingcount = triStrip.size();
   if(windingcount < 3)
      return;

   // create plane from tri...
   bool foundplane = false;
   PlaneF testplane;
   Point3F norm = (triStrip[0].sgNorm + triStrip[1].sgNorm + triStrip[2].sgNorm) * 0.3333f;

   for(U32 i=2; i<triStrip.size(); i++)
   {
      const Point3F &a = triStrip[i-2].sgVert;
      const Point3F &b = triStrip[i-1].sgVert;
      const Point3F &c = triStrip[i].sgVert;

      if(sgDegenerateTriangle(a, b, c))
         continue;

      foundplane = true;
      testplane.set(a, b, c);
      break;
   }

   if(!foundplane)
      return;

   bool flippedtestplane = (mDot(testplane, norm) < 0.0f);
   bool flippedplane = (mDot(surfacePlane, norm) < 0.0f);

   if(flippedtestplane)
      sgFlippedWindings = true;

   if(flippedplane)
   {
      surfacePlane.x *= -1.0f;
      surfacePlane.y *= -1.0f;
      surfacePlane.z *= -1.0f;
      surfacePlane.d *= -1.0f;
   }

   // test for smoothing...
   Point3F normal = triStrip[0].sgNorm;
   for(U32 i=1; i<windingcount; i++)
   {
      if(mDot(triStrip[i].sgNorm, normal) < 0.999f)
      {
         sgUseSmoothing = true;
         break;
      }
   }

   // get the axis info...
   F64 smax = 0.0f;
   F64 tmax = 0.0f;
   for(S32 i=0; i<3; i++)
   {
      F64 s = mFabs(sgLightMapSVector[i]);
      F64 t = mFabs(sgLightMapTVector[i]);
      //if((s > 0.0f) && (t > 0.0f))
      //	continue;
      if(s > smax)
      {
         sgSAxis = i;
         smax = s;
      }
      if(t > tmax)
      {
         sgTAxis = i;
         tmax = t;
      }
   }

   // try again...
   badtexgen = false;
   if(sgSAxis == sgTAxis)
   {
      // find the axis with the minimal diff (the bad axis)...
      U32 a = (sgSAxis + 1) % 3;
      U32 b = (sgSAxis + 2) % 3;
      F64 absa = mFabs(mFabs(sgLightMapSVector[sgSAxis]) - mFabs(sgLightMapSVector[a]));
      F64 absb = mFabs(mFabs(sgLightMapSVector[sgSAxis]) - mFabs(sgLightMapSVector[b]));
      smax = getMin(absa, absb);

      a = (sgTAxis + 1) % 3;
      b = (sgTAxis + 2) % 3;
      absa = mFabs(mFabs(sgLightMapTVector[sgTAxis]) - mFabs(sgLightMapTVector[a]));
      absb = mFabs(mFabs(sgLightMapTVector[sgTAxis]) - mFabs(sgLightMapTVector[b]));
      tmax = getMin(absa, absb);

      S32 avoidaxis = -1;
      S32 *newaxis = NULL;
      Point3D *vector = NULL;
      if(smax < tmax)
      {
         avoidaxis = sgTAxis;
         newaxis = &sgSAxis;
         vector = &sgLightMapSVector;
      }
      else
      {
         avoidaxis = sgSAxis;
         newaxis = &sgTAxis;
         vector = &sgLightMapTVector;
      }

      // find the max on the bad axis that's not on the original axis...
      F64 max = 0.0f;
      for(S32 i=0; i<3; i++)
      {
         if(i == avoidaxis)
            continue;
         F64 val = mFabs((*vector)[i]);
         if(val > max)
         {
            (*newaxis) = i;
            max = val;
         }
      }

      //Con::warnf("Questionable light map axis gen...");
      badtexgen = true;
   }

   AssertFatal(((sgSAxis != -1) && (sgTAxis != -1) && (sgSAxis != sgTAxis)),
      "Unable to determine axis info!");


   Vector<sgSmoothingTri> tris;
   tris.increment(windingcount - 2);

   // these are tristrips!!!
   for(U32 t=0; t<tris.size(); t++)
   {
      if(sgDegenerateTriangle(triStrip[t].sgVert, triStrip[t+1].sgVert, triStrip[t+2].sgVert))
      {
         tris.decrement(1);
         continue;
      }

      for(U32 v=0; v<3; v++)
      {
         U32 w = v + t;
         U32 k;

         // reverse winding?
         if(t & 0x1)
            k = ((v+2) % 3) + t;
         else
            k = ((v+1) % 3) + t;

         sgSmoothingVert &vect = tris[t].sgVerts[v];

         const Point3F &pointw = triStrip[w].sgVert;
         const Point3F &pointk = triStrip[k].sgVert;
         const Point3F &normalw = triStrip[w].sgNorm;

         vect.sgVert = pointw;
         vect.sgVect = pointk - pointw;
         vect.sgNorm = normalw;

         AssertFatal((vect.sgVect.lenSquared() > 0.0f), "!");

         // could break this out for speed...
         if((t == 0) && (v == 0))
            sgSurfaceBox = Box3F(pointw, pointw);
         else
         {
            sgSurfaceBox.max.setMax(pointw);
            sgSurfaceBox.min.setMin(pointw);
         }
      }

      if(sgUseSmoothing)
         sgBuildDerivatives(tris[t]);
   }

   sgBuildLexels(tris);


   // stats...
   sgStatistics::sgInteriorSurfaceIncludedCount++;
   if(sgUseSmoothing)
   {
      sgStatistics::sgInteriorSurfaceSmoothedCount++;
      sgStatistics::sgInteriorSurfaceSmoothedLexelCount +=
         sgInnerLexels.size() + sgOuterLexels.size();
   }
}

void sgPlanarLightMap::sgBuildDerivatives(sgSmoothingTri &tri)
{
   // build the derivatives for linear interpolation...
   const sgSmoothingVert &va = tri.sgVerts[0];
   const sgSmoothingVert &vb = tri.sgVerts[1];
   const sgSmoothingVert &vc = tri.sgVerts[2];

   const F32 vsac = va.sgVert[sgSAxis] - vc.sgVert[sgSAxis];
   const F32 vsbc = vb.sgVert[sgSAxis] - vc.sgVert[sgSAxis];
   const F32 vtac = va.sgVert[sgTAxis] - vc.sgVert[sgTAxis];
   const F32 vtbc = vb.sgVert[sgTAxis] - vc.sgVert[sgTAxis];
   const F32 spartial = 1.0f / ((vsbc * vtac) - (vsac * vtbc));
   const F32 tpartial = 1.0f / ((vsac * vtbc) - (vsbc * vtac));

   for(S32 c=0; c<3; c++)
   {
      const F32 nac = va.sgNorm[c] - vc.sgNorm[c];
      const F32 nbc = vb.sgNorm[c] - vc.sgNorm[c];
      tri.sgSDerivative[c] = ((nbc * vtac) - (nac * vtbc)) * spartial;
      tri.sgTDerivative[c] = ((nbc * vsac) - (nac * vsbc)) * tpartial;
   }
}

void sgPlanarLightMap::sgBuildLexels(const Vector<sgSmoothingTri> &tris)
{
   // loop through the texels...
   // sort by inner and outer...
   const U32 lexelmax = sgHeight * sgWidth;
   const U32 buffersize = lexelmax * sizeof(sgLexel);
   sgInnerLexels.clear();
   sgInnerLexels.reserve(lexelmax);
   sgOuterLexels.clear();
   sgOuterLexels.reserve(lexelmax);

   // this is faster than Vector[]...
   sgSmoothingTri *trisptr = tris.address();

   bool outer;
   Point3F vec2;
   Point3F cross;
   Point3D pos = sgWorldPosition;
   Point3D run = sgLightMapSVector * sgWidth;
   sgSmoothingTri *container;

   bool halfsize = !LightManager::sgAllowFullLightMaps();
   U32 lmscalemask = LightManager::sgGetLightMapScale() - 1;

   for(U32 y=0; y<sgHeight; y++)
   {
      if(halfsize && (y & lmscalemask))
      {
         pos += sgLightMapTVector;
         continue;
      }

      for(U32 x=0; x<sgWidth; x++)
      {
         if(halfsize && (x & lmscalemask))
         {
            pos += sgLightMapSVector;
            continue;
         }

         sgStatistics::sgInteriorLexelCount++;

         Point3F pos32(pos.x, pos.y, pos.z);

         // find containing tri...
         outer = true;
         container = NULL;
         for(U32 t=0; t<tris.size(); t++)
         {
            for(U32 v=0; v<3; v++)
            {
               vec2 = pos32 - trisptr[t].sgVerts[v].sgVert;
               mCross(vec2, trisptr[t].sgVerts[v].sgVect, &cross);

               // don't check against 0.0f, otherwise lexels on the edge become "inner"
               // and cause shadowing from adjacent brushes...
               if(sgFlippedWindings)
               {
                  if(mDot(surfacePlane, cross) > -0.001f)
                     break;//not this one...
               }
               else
               {
                  if(mDot(surfacePlane, cross) < 0.001f)
                     break;//not this one...
               }

               if(v == 2)
               {
                  //must be this one...
                  outer = false;
                  container = &trisptr[t];
               }
            }

            if(container)
               break;//already found...
         }

         // use line test to find a container for the outer lexels if smoothing...
         // get the normal...
         Point3F normal = surfacePlane;
         if(sgUseSmoothing)
         {
            if(!container)
            {
               // set a default...
               container = trisptr;

               F32 maxdist = 1000000.0f;
               for(U32 t=0; t<tris.size(); t++)
               {
                  for(U32 v=0; v<3; v++)
                  {
                     U32 v2 = (v + 1) % 3;
                     const F32 d = sgGetDistanceSquared(
                        trisptr[t].sgVerts[v].sgVert,
                        trisptr[t].sgVerts[v2].sgVert, pos32);

                     if(d < maxdist)
                     {
                        maxdist = d;
                        container = &trisptr[t];
                     }
                  }
               }
            }

            // get the normal...
            Point3F posrelative = pos32 - container->sgVerts[0].sgVert;
            normal = container->sgVerts[0].sgNorm;
            normal += (container->sgSDerivative * posrelative[sgSAxis]) +
               (container->sgTDerivative * posrelative[sgTAxis]);
            normal.normalize();
         }

         // find respective vector...
         if(outer)
         {
            sgOuterLexels.increment();
            sgLexel &temp = sgOuterLexels.last();
            temp.shadowTestOnly = false;
            temp.lmPos.x = x;
            temp.lmPos.y = y;
            temp.worldPos = pos32;
            temp.normal = normal;
         }
         else
         {
            sgInnerLexels.increment();
            sgLexel &temp = sgInnerLexels.last();
            temp.shadowTestOnly = false;
            temp.lmPos.x = x;
            temp.lmPos.y = y;
            temp.worldPos = pos32;
            temp.normal = normal;
         }

         pos += sgLightMapSVector;
      }

      pos -= run;
      pos += sgLightMapTVector;
   }

   // add lexels at the verts to help out skinny geometry...
   for(U32 i=0; i<tris.size(); i++)
   {
      // if few inner lexels exist fake one for shadow testing...
      if(sgInnerLexels.size() < 5)
      {
         Point3F norm = tris[i].sgVerts[0].sgNorm + tris[i].sgVerts[1].sgNorm + tris[i].sgVerts[2].sgNorm;
         norm.normalize();

         // center
         sgInnerLexels.increment();
         sgLexel &tempa = sgInnerLexels.last();
         tempa.shadowTestOnly = true;
         tempa.lmPos.x = 0;
         tempa.lmPos.y = 0;
         tempa.worldPos = (tris[i].sgVerts[0].sgVert + tris[i].sgVerts[1].sgVert + tris[i].sgVerts[2].sgVert) * 0.3333333f;
         tempa.normal = norm;
      }

      // vert 0
      sgInnerLexels.increment();
      sgLexel &tempb = sgInnerLexels.last();
      tempb.shadowTestOnly = true;
      tempb.lmPos.x = 0;
      tempb.lmPos.y = 0;
      tempb.worldPos = tris[i].sgVerts[0].sgVert;
      tempb.normal = tris[i].sgVerts[0].sgNorm;

      // last 2
      if(i == (tris.size() - 1))
      {
         for(U32 v=1; v<3; v++)
         {
            sgInnerLexels.increment();
            sgLexel &temp = sgInnerLexels.last();
            temp.shadowTestOnly = true;
            temp.lmPos.x = 0;
            temp.lmPos.y = 0;
            temp.worldPos = tris[i].sgVerts[v].sgVert;
            temp.normal = tris[i].sgVerts[v].sgNorm;
         }
      }
   }
}

void sgPlanarLightMap::sgCalculateLighting(LightInfo *light)
{
   // stats...
   sgStatistics::sgInteriorSurfaceIlluminationCount++;
   elapsedTimeAggregate time = elapsedTimeAggregate(sgStatistics::sgInteriorLexelIlluminationTime);


   // setup zone info...
   bool isinzone = false;
   if(light->sgDiffuseRestrictZone || light->sgAmbientRestrictZone)
   {
      for(U32 z=0; z<sgInteriorInstance->getNumCurrZones(); z++)
      {
         S32 zone = sgInteriorInstance->getCurrZone(z);
         if(zone > 0)
         {
            if((zone == light->sgZone[0]) || (zone == light->sgZone[1]))
            {
               isinzone = true;
               break;
            }
         }
      }

      if(!isinzone && (sgInteriorSurface != SG_NULL_SURFACE))
      {
         S32 zone = sgInteriorInstance->getSurfaceZone(sgInteriorSurface, sgInteriorDetail);
         if((light->sgZone[0] == zone) || (light->sgZone[1] == zone))
            isinzone = true;
      }

      if(!isinzone && sgInteriorStaticMesh)
      {
         // need to find zone(s)...
      }
   }

   // allow what?
   bool allowdiffuse = (!light->sgDiffuseRestrictZone) || isinzone;
   bool allowambient = (!light->sgAmbientRestrictZone) || isinzone;

   // should I bother?
   if((!allowdiffuse) && (!allowambient))
      return;

   // first get lighting model...
   sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(
      light->sgLightingModelName);
   model.sgSetState(light);

   // test for early out...
   if(!model.sgCanIlluminate(sgSurfaceBox))
   {
      model.sgResetState();
      return;
   }

   // this is slow, so do it after the early out...
   model.sgInitStateLM();

   // setup shadow objects...
   Box3F lightvolume = sgSurfaceBox;
   if(light->mType == LightInfo::Vector)
   {
      for(U32 i=0; i<triStrip.size(); i++)
      {
         Point3F lightpos = triStrip[i].sgVert +
            (SG_STATIC_LIGHT_VECTOR_DIST * light->mDirection * -1.0f);
         lightvolume.max.setMax(lightpos);
         lightvolume.min.setMin(lightpos);
      }
   }
   else
   {
      lightvolume.max.setMax(light->mPos);
      lightvolume.min.setMin(light->mPos);
   }

   // build a list of potential shadow casters...
   if(light->sgCastsShadows && LightManager::sgAllowShadows())
      sgGetIntersectingObjects(lightvolume, sgInteriorInstance);


   // stats...
   sgStatistics::sgInteriorSurfaceIlluminatedCount++;
   sgStatistics::sgInteriorLexelIlluminationCount += sgInnerLexels.size() + sgOuterLexels.size();


   hash_multimap<void *, sgOccluder> shadowingsurfaces;

   // put rayCast into lighting mode?
   bool faststaticmeshshadows = !LightManager::sgAllowFullLightMaps();
   if(!faststaticmeshshadows || sgInteriorStaticMesh)
      Interior::smLightingCastRays = true;

   for(U32 i=0; i<sgPlanarLightMap::sglpCount; i++)
   {
      // set which list...
      U32 templexelscount;
      sgLexel *templexels;
      if(i == sgPlanarLightMap::sglpInner)
      {
         templexelscount = sgInnerLexels.size();
         templexels = sgInnerLexels.address();
      }
      else
      {
         templexelscount = sgOuterLexels.size();
         templexels = sgOuterLexels.address();
      }

      for(U32 ii=0; ii<templexelscount; ii++)
      {
         // get the current lexel...
         const sgLexel &lexel = templexels[ii];
         const U32 x = lexel.lmPos.x;
         const U32 y = lexel.lmPos.y;
         const U32 lmindex = ((y * sgWidth) + x);

         // early out...
         // not likely to happen, but great potential...
         ColorF &col = sgTexels->sgData[lmindex];
         if((col.red >= 1.0f) && (col.green >= 1.0f) && (col.blue >= 1.0f))
            continue;

         // too often unset, must do these here...
         ColorF diffuse(0.0f, 0.0f, 0.0f);
         ColorF ambient(0.0f, 0.0f, 0.0f);
         VectorF lightingnormal(0.0f, 0.0f, 0.0f);
         model.sgLightingLM(lexel.worldPos, lexel.normal, diffuse, ambient, lightingnormal);


         // testing...
         //diffuse = ColorF(lexel.normal.x, lexel.normal.y, lexel.normal.z);
         /*if(i == sgPlanarLightMap::sglpOuter)
         diffuse = ColorF(1.0, 0, 1.0);
         else
         diffuse = ColorF(0.0, 1.0, 1.0);

         if((x & 0x1) == (y & 0x1))
         diffuse.green = 0;
         else
         diffuse.blue = 0;*/

         /*Point3F p = lexel.worldPos;
         diffuse = ColorF(mFabs(p.x - mFloor(p.x)),
         mFabs(p.y - mFloor(p.y)),
         mFabs(p.z - mFloor(p.z)));*/

         /*if(sgUseSmoothing)
         {
         Point3F col = lexel.normal * 0.5 + Point3F(0.5, 0.5, 0.5);//ColorF(1, 0, 0);
         ambient = ColorF(col.x, col.y, col.z);
         //ambient = ColorF(1, 0, 0);
         }*/

         //if(badtexgen)
         //   diffuse = ColorF(1.0, 1.0, 0.0);


         // increment the occluder mask...
         sgCurrentOccluderMaskId++;
         if(sgCurrentOccluderMaskId == 0)
            sgCurrentOccluderMaskId++;

         // in the event the diffuse is too small or
         // the alpha occluders attenuated the value too much...
         if(allowdiffuse && ((diffuse.red > SG_MIN_LEXEL_INTENSITY) || (diffuse.green > SG_MIN_LEXEL_INTENSITY) || (diffuse.blue > SG_MIN_LEXEL_INTENSITY)))
         {
            // stats
            sgStatistics::sgInteriorLexelDiffuseCount++;

            // step four: check for shadows...
            bool shadowed = false;

            if(light->sgCastsShadows && LightManager::sgAllowShadows())
            {
               // set light pos for shadows...
               Point3F lightpos = light->mPos;
               if(light->mType == LightInfo::Vector)
               {
                  lightpos = SG_STATIC_LIGHT_VECTOR_DIST * light->mDirection * -1.0f;
                  lightpos = lexel.worldPos + lightpos;
               }

               // potential problem with this setup is the rare but possible
               // occurrence of outer lexel light leaks from raycast hitting an
               // interior surface that's not in the shadow list but also has valid
               // occluders between it and the lexel (ie: stops the ray before
               // finding the valid occluders).
               //
               // possible solutions:
               // -Ctor style shadow system
               // -custom raycast that collects ALL occluding surfaces
               //
               // both solutions are heavyweight - first see if the problem occurs
               // in the wild, this may be a theoretical problem.

               // start by finding any shadow casters
               sgOccluder info;
               for(U32 o=0; o<sgIntersectingSceneObjects.size(); o++)
               {
                  if(!sgCastRay(lightpos, lexel.worldPos, sgIntersectingSceneObjects[o], NULL, NULL, info))
                     continue;

                  // no valid testing on non-self objects...

                  shadowed = true;
                  break;
               }

               // if false then self interior test (include sms at the end)...
               if(!shadowed)
               {
                  if(sgCastRay(lightpos, lexel.worldPos, sgInteriorInstance, sgInteriorDetail, NULL, info))
                  {
                     if(info.sgSurface != sgInteriorSurface)
                     {
                        sgAdjacent adj = sgAreAdjacent(sgInteriorSurface, info.sgSurface);
                        if(adj == sgaFalse)
                           shadowed = true;
                        else if(adj == sgaMaybe)
                        {
                           if(sgIsValidOccluder(info, shadowingsurfaces, (i == sglpInner)))
                              shadowed = true;
                        }
                     }
                  }

                  if(!shadowed && !faststaticmeshshadows)
                  {
                     for(U32 sm=0; sm<sgInteriorDetail->getStaticMeshCount(); sm++)
                     {
                        ConstructorSimpleMesh *mesh = (ConstructorSimpleMesh *)sgInteriorDetail->getStaticMesh(sm);
                        if(mesh == sgInteriorStaticMesh)
                           continue;

                        if(!sgCastRay(lightpos, lexel.worldPos, sgInteriorInstance, NULL, mesh, info))
                           continue;

                        if(!sgIsValidOccluder(info, shadowingsurfaces, (i == sglpInner)))
                           continue;

                        shadowed = true;
                        break;
                     }
                  }
               }

               // if false then try the sm list...
               if(!shadowed && !faststaticmeshshadows)
               {
                  sgStaticMeshBVPT::objectList list;
                  sgIntersectingStaticMeshObjects.collectObjectsClipped(lightpos, lexel.worldPos, list);

                  for(U32 o=0; o<list.size(); o++)
                  {
                     sgStaticMeshInfo &sminfo = list[o];
                     if(!sgCastRay(lightpos, lexel.worldPos, sminfo.sgInteriorInstance, NULL, sminfo.sgStaticMesh, info))
                        continue;

                     // no valid testing on non-self objects...

                     shadowed = true;
                     break;
                  }
               }

               // if false then self sm test...
               if(!shadowed && sgInteriorStaticMesh && !faststaticmeshshadows)
               {
                  // reverse cast direction...
                  if(sgCastRay(lexel.worldPos, lightpos, sgInteriorInstance, NULL, sgInteriorStaticMesh, info))
                  {
                     if(sgIsValidOccluder(info, shadowingsurfaces, (i == sglpInner)))
                        shadowed = true;
                  }
               }
            }

            if(!lexel.shadowTestOnly && !shadowed)
            {
               // cool, got this far so either there was no occluder
               // or it was an alpha and eventually we found a light
               // ray segment that didn't get blocked...
               //
               // step five: apply the lighting to the light map...
               sgDirty = true;
               sgTexels->sgData[lmindex] += diffuse;
            }
         }

         if(!lexel.shadowTestOnly && allowambient && ((ambient.red > SG_MIN_LEXEL_INTENSITY) || (ambient.green > SG_MIN_LEXEL_INTENSITY) || (ambient.blue > SG_MIN_LEXEL_INTENSITY)))
         {
            sgDirty = true;
            sgTexels->sgData[lmindex] += ambient;
         }
      }
   }

   // put rayCast back to normal mode...
   Interior::smLightingCastRays = false;

   model.sgResetState();
}

sgPlanarLightMap::sgAdjacent sgPlanarLightMap::sgAreAdjacent(U32 surface1, U32 surface2)
{
	U32 i1, i2;
	Vector<U32> index1, index2;

	//if we can't identify the surfaces then assume the worst...
	if((surface1 == U32(-1)) || (surface2 == U32(-1)))
		return sgaFalse;

	if((surface1 == SG_NULL_SURFACE) || (surface2 == SG_NULL_SURFACE))
		return sgaFalse;

	// get the compare surfaces...
	const Interior::Surface &surf1 = sgInteriorDetail->getSurface(surface1);
	const Interior::Surface &surf2 = sgInteriorDetail->getSurface(surface2);

	// grab the surface's winding indexes...
	for(i1=0; i1<surf1.windingCount; i1++)
		index1.push_back(sgInteriorDetail->getWinding(surf1.windingStart + i1));

	// grab the surface's winding indexes...
	for(i2=0; i2<surf2.windingCount; i2++)
		index2.push_back(sgInteriorDetail->getWinding(surf2.windingStart + i2));

	// compare for common index...
	long indexcount = 0;
	for(i1=0; i1<index1.size(); i1++)
	{
		for(i2=0; i2<index2.size(); i2++)
		{
			if(index1[i1] == index2[i2])
			{
				indexcount++;
				if(indexcount > 1)
					return sgaTrue;
			}
		}
	}

	if(indexcount < 1)
		return sgaFalse;

	return sgaMaybe;
}

void sgPlanarLightMap::sgMergeLighting(GBitmap *lightmap, U32 xoffset, U32 yoffset)
{
   // stats
   elapsedTimeAggregate time = elapsedTimeAggregate(sgStatistics::sgInteriorSurfaceMergeTime);
   sgStatistics::sgInteriorSurfaceMergeCount++;

   sgTexels->sgFillInLighting();
   sgTexels->sgBlur();
   //sgTexels->sgTestDump();

   ColorF *texels = sgTexels->sgData;

   for(U32 y=0; y<sgHeight; y++)
   {
      U8 *bits = lightmap->getAddress(xoffset, (yoffset + y));
      U32 d = 0;

      for(U32 x=0; x<sgWidth; x++)
      {
         U32 s = ((y * sgWidth) + x);
         ColorF &texel = texels[s];

         U32 frag = bits[d] + (texel.red * 255.0f);
         if(frag > 255) frag = 255;
         bits[d++] = frag;

         frag = bits[d] + (texel.green * 255.0f);
         if(frag > 255) frag = 255;
         bits[d++] = frag;

         frag = bits[d] + (texel.blue * 255.0f);
         if(frag > 255) frag = 255;
         bits[d++] = frag;
      }
   }
}

//----------------------------------------------

void sgTerrainLightMap::sgCalculateLighting(LightInfo *light)
{
   // setup zone info...
   bool isinzone = (light->sgZone[0] == 0) || (light->sgZone[1] == 0);

   // allow what?
   bool allowdiffuse = (!light->sgDiffuseRestrictZone) || isinzone;
   bool allowambient = (!light->sgAmbientRestrictZone) || isinzone;

   // should I bother?
   if((!allowdiffuse) && (!allowambient))
      return;


   AssertFatal((sgTerrain), "Member 'sgTerrain' must be populated.");

   // setup constants...
   const F32 terrainlength = (sgTerrain->getSquareSize() * TerrainBlock::BlockSize);
   const F32 halfterrainlength = terrainlength * 0.5f;


   U32 time = Platform::getRealMilliseconds();

   Point2F s, t;
   s[0] = sgLightMapSVector[0];
   s[1] = sgLightMapSVector[1];
   t[0] = sgLightMapTVector[0];
   t[1] = sgLightMapTVector[1];
   Point2F run = t * sgWidth;

   Point2F start;
   start[0] = sgWorldPosition[0] + halfterrainlength;
   start[1] = sgWorldPosition[1] + halfterrainlength;

   sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(
      light->sgLightingModelName);
   model.sgSetState(light);
   model.sgInitStateLM();

   Point2I lmindexmin, lmindexmax;
   if(light->mType == LightInfo::Vector)
   {
      lmindexmin.x = 0;
      lmindexmin.y = 0;
      lmindexmax.x = (sgWidth - 1);
      lmindexmax.y = (sgHeight - 1);
   }
   else
   {
      F32 maxrad = model.sgGetMaxRadius();
      Box3F worldbox = Box3F(light->mPos, light->mPos);
      worldbox.min -= Point3F(maxrad, maxrad, maxrad);
      worldbox.max += Point3F(maxrad, maxrad, maxrad);

      lmindexmin.x = (worldbox.min.x - sgWorldPosition.x) / s.x;
      lmindexmin.y = (worldbox.min.y - sgWorldPosition.y) / t.y;
      lmindexmax.x = (worldbox.max.x - sgWorldPosition.x) / s.x;
      lmindexmax.y = (worldbox.max.y - sgWorldPosition.y) / t.y;

      lmindexmin.x = getMax(lmindexmin.x, S32(0));
      lmindexmin.y = getMax(lmindexmin.y, S32(0));
      lmindexmax.x = getMin(lmindexmax.x, S32(sgWidth - 1));
      lmindexmax.y = getMin(lmindexmax.y, S32(sgHeight - 1));
   }

   S32 lmx, lmy, lmindex;
   Point3F lexelworldpos;
   Point3F normal;
   //Point3F lightingnormal = Point3F(0.0f, 0.0f, 0.0f);

   Point2F point = ((t * lmindexmin.y) + start + (s * lmindexmin.x));

   for(lmy=lmindexmin.y; lmy<lmindexmax.y; lmy++)
   {
      for(lmx=lmindexmin.x; lmx<lmindexmax.x; lmx++)
      {
         lmindex = (lmx + (lmy * sgWidth));

         // get lexel 2D world pos...
         lexelworldpos[0] = point[0] - halfterrainlength;
         lexelworldpos[1] = point[1] - halfterrainlength;

         // use 2D terrain space pos to get the world space z and normal...
         sgTerrain->getNormalAndHeight(point, &normal, &lexelworldpos.z, false);

         // too often unset, must do these here...
         ColorF diffuse(0.0f, 0.0f, 0.0f);
         ColorF ambient(0.0f, 0.0f, 0.0f);
         VectorF lightingnormal(0.0f, 0.0f, 0.0f);
         model.sgLightingLM(lexelworldpos, normal, diffuse, ambient, lightingnormal);

         if(allowdiffuse && ((diffuse.red > SG_MIN_LEXEL_INTENSITY) ||
            (diffuse.green > SG_MIN_LEXEL_INTENSITY) || (diffuse.blue > SG_MIN_LEXEL_INTENSITY)))
         {
            // step four: check for shadows...

            bool shadowed = false;
            RayInfo info;
            if(light->sgCastsShadows && LightManager::sgAllowShadows())
            {
               // set light pos for shadows...
               Point3F lightpos = light->mPos;
               if(light->mType == LightInfo::Vector)
               {
                  lightpos = SG_STATIC_LIGHT_VECTOR_DIST * light->mDirection * -1.0f;
                  lightpos = lexelworldpos + lightpos;
               }

               // make texels terrain space coord into a world space coord...
               RayInfo info;
               if(sgTerrain->getContainer()->castRay(lightpos, (lexelworldpos + (lightingnormal * 0.5f)),
                  ShadowCasterObjectType, &info))
               {
                  shadowed = true;
               }
            }

            if(!shadowed)
            {
               // step five: apply the lighting to the light map...
               sgTexels->sgData[lmindex] += diffuse;
            }
         }

         if(allowambient && ((ambient.red > 0.0f) || (ambient.green > 0.0f) || (ambient.blue > 0.0f)))
         {
            //sgTexels->sgData[lmindex] += ambient;
         }

         /*if((lmx & 0x1) == (lmy & 0x1))
         sgTexels[lmindex] += ColorF(1.0, 0.0, 0.0);
         else
         sgTexels[lmindex] += ColorF(0.0, 1.0, 0.0);*/


         // stats...
         sgStatistics::sgTerrainLexelCount++;


         point += s;
      }

      point = ((t * lmy) + start + (s * lmindexmin.x));
   }

   model.sgResetState();


   // stats...
   sgStatistics::sgTerrainLexelTime += Platform::getRealMilliseconds() - time;
}

void sgTerrainLightMap::sgMergeLighting(ColorF *lightmap)
{
   sgTexels->sgBlur();

   U32 y, x, index;
   for(y=0; y<sgHeight; y++)
   {
      for(x=0; x<sgWidth; x++)
      {
         index = (y * sgWidth) + x;
         ColorF &pixel = lightmap[index];
         pixel += sgTexels->sgData[index];
      }
   }
}

