//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#include "editor/editTSCtrl.h"
#include "editor/worldEditor.h"
#include "game/shadow.h"
#include "game/vehicles/wheeledVehicle.h"
#include "game/gameConnection.h"
#include "sceneGraph/sceneGraph.h"
#include "terrain/terrRender.h"
#include "game/shapeBase.h"
#include "gui/core/guiCanvas.h"
#include "ts/tsShape.h"
#include "ts/tsShapeInstance.h"
#include "game/staticShape.h"
#include "game/tsStatic.h"
#include "collision/concretePolyList.h"
#include "lightingSystem/sgSceneLighting.h"
#include "lightingSystem/sgLightMap.h"
#include "lightingSystem/sgSceneLightingGlobals.h"
#include "lightingSystem/sgLightingModel.h"


/// adds the ability to bake point lights into interior light maps.
void SceneLighting::InteriorProxy::sgAddLight(LightInfo *light, InteriorInstance *interior)
{
   //elapsedTime time = elapsedTime("SceneLighting::InteriorProxy::sgAddLight (%d)");

	// need this...
	sgInterior = interior;

	sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(
		light->sgLightingModelName);
	model.sgSetState(light);

	// test for early out...
	if(!model.sgCanIlluminate(interior->getWorldBox()))
	{
		model.sgResetState();
		return;
	}

	model.sgResetState();
	sgLights.push_back(light);

	// on first light build surface list...
	if(sgLights.size() == 1)
	{
		// stats...
		sgStatistics::sgInteriorObjectIncludedCount++;


		// get the global shadow casters...
		sgShadowObjects::sgGetObjects(interior);

      sgClearSurfaces();

		sgCurrentSurfaceIndex = 0;
		InteriorResource *res = sgInterior->getResource();
		U32 countd = res->getNumDetailLevels();
		for(U32 d=0; d<countd; d++)
		{
			Interior *detail = res->getDetailLevel(d);
			U32 counti = detail->getSurfaceCount();
			U32 offset = sgSurfaces.size();
			sgSurfaces.increment(counti);
			for(U32 i=0; i<counti; i++)
			{
				sgSurfaceInfo *info = new sgSurfaceInfo();
            sgSurfaces[i + offset] = info;
            sgConvertInteriorSurfaceToSurfaceInfo(detail->getSurface(i), i, detail, *info);
			}

         U32 countsm = detail->getStaticMeshCount();
         for(U32 sm=0; sm<countsm; sm++)
         {
            const ConstructorSimpleMesh *mesh = detail->getStaticMesh(sm);
            if(!mesh)
               continue;

            counti = mesh->primitives.size();
            offset = sgSurfaces.size();
            sgSurfaces.increment(counti);
            for(U32 i=0; i<counti; i++)
            {
               sgSurfaceInfo *info = new sgSurfaceInfo();
               sgSurfaces[i + offset] = info;
               sgConvertStaticMeshPrimitiveToSurfaceInfo(mesh, i, detail, *info);
            }
			}
		}
	}

	// recalc number of surfaces per pass based on new light count...
	sgSurfacesPerPass = sgSurfaces.size() / sgLights.size();
}

void SceneLighting::InteriorProxy::light(LightInfo *light)
{
   //elapsedTime time = elapsedTime("SceneLighting::InteriorProxy::light (%d)");

	U32 i;
	U32 countthispass = 0;


	// stats...
	sgStatistics::sgInteriorObjectIlluminationCount++;


	for(i=sgCurrentSurfaceIndex; i<sgSurfaces.size(); i++)
	{
		sgProcessSurface(*sgSurfaces[i]);
		countthispass++;
		sgCurrentSurfaceIndex++;
		if((countthispass >= sgSurfacesPerPass) && (sgLights.last() != light))
			break;
	}
}

void SceneLighting::InteriorProxy::sgConvertStaticMeshPrimitiveToSurfaceInfo(const ConstructorSimpleMesh *staticmesh, U32 primitiveindex,
                                                                         Interior *detail, sgSurfaceInfo &surfaceinfo)
{
   const ConstructorSimpleMesh::primitive &prim = staticmesh->primitives[primitiveindex];
	const MatrixF &transform = sgInterior->getTransform();
	const Point3F &scale = sgInterior->getScale();

   // need to generate the plane...
   AssertFatal((prim.count >= 3), "Primitive with only 2 verts?!?");
   Point3F p0 = staticmesh->verts[prim.start];
   Point3F p1 = staticmesh->verts[prim.start+1];
   Point3F p2 = staticmesh->verts[prim.start+2];

   // and it needs to be in interior/object space, not static mesh space...
   p0.convolve(staticmesh->scale);
   p1.convolve(staticmesh->scale);
   p2.convolve(staticmesh->scale);
   staticmesh->transform.mulP(p0);
   staticmesh->transform.mulP(p1);
   staticmesh->transform.mulP(p2);

   // generate the plane...
   PlaneF plane = PlaneF(p0, p1, p2);

   // also need a world space version...
	PlaneF projPlane;
	mTransformPlane(transform, scale, plane, &projPlane);

   //-----------------------------
	// get the generic instance 0 lm...
	GBitmap *lm = gInteriorLMManager.getBitmap(detail->getLMHandle(), 0, prim.lightMapIndex);
	AssertFatal((lm), "Why was there no base light map??");

   sgExtractLightingInformation(detail, prim.lightMapEquationX, prim.lightMapEquationY, plane,
      Point2I(prim.lightMapOffset.x, prim.lightMapOffset.y), Point2I(prim.lightMapSize.x, prim.lightMapSize.y), Point2I(lm->getWidth(), lm->getHeight()),
      surfaceinfo.sgWorldPos,
      surfaceinfo.sgSVector,
      surfaceinfo.sgTVector,
      surfaceinfo.sgLightMapOffset,
      surfaceinfo.sgLightMapExtent);

   surfaceinfo.sgDetail = detail;
   surfaceinfo.sgSurfaceIndex = SG_NULL_SURFACE;
   surfaceinfo.sgStaticMesh = (ConstructorSimpleMesh *)staticmesh;
   surfaceinfo.sgSurfacePlane = projPlane;
   // currently ctor export has no zones...
   // when it does this needs to be fixed - also static mesh zone lighting...
   surfaceinfo.sgSurfaceOutsideVisible = true;
   surfaceinfo.sgLightMapIndex = prim.lightMapIndex;

   surfaceinfo.sgTriStrip.clear();
   surfaceinfo.sgTriStrip.increment(prim.count);

   for(U32 v=0; v<prim.count; v++)
   {
      U32 index = prim.start + v;
      surfaceinfo.sgTriStrip[v].sgVert = staticmesh->verts[index];
      surfaceinfo.sgTriStrip[v].sgNorm = staticmesh->norms[index];

		surfaceinfo.sgTriStrip[v].sgVert.convolve(staticmesh->scale);
		staticmesh->transform.mulP(surfaceinfo.sgTriStrip[v].sgVert);
      staticmesh->transform.mulV(surfaceinfo.sgTriStrip[v].sgNorm);

		surfaceinfo.sgTriStrip[v].sgVert.convolve(scale);
		transform.mulP(surfaceinfo.sgTriStrip[v].sgVert);
      transform.mulV(surfaceinfo.sgTriStrip[v].sgNorm);
   }

   sgReorganizeSurface(surfaceinfo);
}

void SceneLighting::InteriorProxy::sgConvertInteriorSurfaceToSurfaceInfo(const Interior::Surface &surface, U32 i,
                                                                         Interior *detail, sgSurfaceInfo &surfaceinfo)
{
	// points right way?
	PlaneF plane = detail->getPlane(surface.planeIndex);
	if(Interior::planeIsFlipped(surface.planeIndex))
		plane.neg();

	const MatrixF &transform = sgInterior->getTransform();
	const Point3F &scale = sgInterior->getScale();

	PlaneF projPlane;
	mTransformPlane(transform, scale, plane, &projPlane);

	const Interior::TexGenPlanes & lmTexGenEQ = detail->getLMTexGenEQ(i);

	// get the generic instance 0 lm...
   U32 lmindex = detail->getNormalLMapIndex(i);
	GBitmap *lm = gInteriorLMManager.getBitmap(detail->getLMHandle(), 0, lmindex);
	AssertFatal((lm), "Why was there no base light map??");

   sgExtractLightingInformation(detail, lmTexGenEQ.planeX, lmTexGenEQ.planeY, plane,
      Point2I(surface.mapOffsetX, surface.mapOffsetY), Point2I(surface.mapSizeX, surface.mapSizeY), Point2I(lm->getWidth(), lm->getHeight()),
      surfaceinfo.sgWorldPos,
      surfaceinfo.sgSVector,
      surfaceinfo.sgTVector,
      surfaceinfo.sgLightMapOffset,
      surfaceinfo.sgLightMapExtent);

   surfaceinfo.sgDetail = detail;
   surfaceinfo.sgSurfaceIndex = i;
   surfaceinfo.sgStaticMesh = NULL;
   surfaceinfo.sgSurfacePlane = projPlane;
   surfaceinfo.sgSurfaceOutsideVisible = (surface.surfaceFlags & Interior::SurfaceOutsideVisible);
   surfaceinfo.sgLightMapIndex = lmindex;

   surfaceinfo.sgTriStrip.clear();
   surfaceinfo.sgTriStrip.increment(surface.windingCount);

   for(U32 v=0; v<surface.windingCount; v++)
   {
      Point3F &vert = surfaceinfo.sgTriStrip[v].sgVert;
      Point3F &norm = surfaceinfo.sgTriStrip[v].sgNorm;

      U32 index = detail->getWinding(surface.windingStart + v);
      vert = detail->getPoint(index);
      norm = detail->getPointNormal(i, v);

		vert.convolve(scale);
		transform.mulP(vert);
      transform.mulV(norm);
   }

   sgReorganizeSurface(surfaceinfo);
}

void SceneLighting::InteriorProxy::sgReorganizeSurface(sgSurfaceInfo &surfaceinfo)
{
   /*if(surfaceinfo.sgTriStrip.size() < 3)
      return;

   // create plane from tri...
   Vector<sgPlanarLightMap::sgSmoothingVert> &originalstrip = surfaceinfo.sgTriStrip;
   PlaneF plane = PlaneF(originalstrip[0].sgVert, originalstrip[1].sgVert, originalstrip[2].sgVert);
   Point3F norm = (originalstrip[0].sgNorm + originalstrip[1].sgNorm + originalstrip[2].sgNorm) * 0.3333;

   // compare to average normal for reverse winding test...
   // if ok then return...
   if(mDot(plane, norm) > 0.0f)
      return;

   // reverse windings...
   U32 vertcount = originalstrip.size();
   sgPlanarLightMap::sgSmoothingVert tempvert;

   // pull verts in pairs and swap...
   for(U32 i=2; i<vertcount; i+=2)
   {
      dMemcpy(&tempvert, &originalstrip[i-1], sizeof(sgPlanarLightMap::sgSmoothingVert));
      dMemcpy(&originalstrip[i-1], &originalstrip[i], sizeof(sgPlanarLightMap::sgSmoothingVert));
      dMemcpy(&originalstrip[i], &tempvert, sizeof(sgPlanarLightMap::sgSmoothingVert));
   }

   // if static mesh regenerate the plane...
   if(!surfaceinfo.sgStaticMesh)
      return;

   surfaceinfo.sgSurfacePlane = PlaneF(originalstrip[0].sgVert, originalstrip[1].sgVert, originalstrip[2].sgVert);*/
}

void SceneLighting::InteriorProxy::sgExtractLightingInformation(const Interior *detail, const PlaneF &lmEqX, const PlaneF &lmEqY, const PlaneF &surfplane,
                                                                const Point2I &lmoff, const Point2I &lmext, const Point2I &lmsheetsize,
                                                                Point3D &worldpos, Point3D &vectS, Point3D &vectT, Point2I &lmoffactual, Point2I &lmextactual)
{
	const MatrixF &transform = sgInterior->getTransform();
	const Point3F &scale = sgInterior->getScale();

	S32 xlen, ylen, xoff, yoff;
	S32 lmborder = detail->getLightMapBorderSize();
	xlen = lmext.x + (lmborder * 2);
	ylen = lmext.y + (lmborder * 2);
	xoff = lmoff.x - lmborder;
	yoff = lmoff.y - lmborder;

	// very important check!!!
	AssertFatal((
		((xoff >= 0) && ((xlen + xoff) < lmsheetsize.x)) &&
		((yoff >= 0) && ((ylen + yoff) < lmsheetsize.y))), "Light map extents exceeded bitmap size!");

   lmoffactual = Point2I(xoff, yoff);
   lmextactual = Point2I(xlen, ylen);


	const F32 * const lGenX = lmEqX;
	const F32 * const lGenY = lmEqY;

	AssertFatal((lGenX[0] * lGenX[1] == 0.f) && 
		(lGenX[0] * lGenX[2] == 0.f) &&
		(lGenX[1] * lGenX[2] == 0.f), "Bad lmTexGen!");
	AssertFatal((lGenY[0] * lGenY[1] == 0.f) && 
		(lGenY[0] * lGenY[2] == 0.f) &&
		(lGenY[1] * lGenY[2] == 0.f), "Bad lmTexGen!");

	// get the axis index for the texgens (could be swapped)
	S32 si;
	S32 ti;
	S32 axis = -1;

	if(lGenX[0] == 0.f && lGenY[0] == 0.f)          // YZ
	{
		axis = 0;
		if(lGenX[1] == 0.f) { // swapped?
			si = 2;
			ti = 1;
		} else {
			si = 1;
			ti = 2;
		}
	}
	else if(lGenX[1] == 0.f && lGenY[1] == 0.f)     // XZ
	{
		axis = 1;
		if(lGenX[0] == 0.f) { // swapped?
			si = 2;
			ti = 0;
		} else {
			si = 0;
			ti = 2;
		}
	}
	else if(lGenX[2] == 0.f && lGenY[2] == 0.f)     // XY
	{
		axis = 2;
		if(lGenX[0] == 0.f) { // swapped?
			si = 1;
			ti = 0;
		} else {
			si = 0;
			ti = 1;
		}
	}

	AssertFatal(!(axis == -1), "SceneLighting::lightInterior: bad TexGen!");

	const F32 * pNormal = ((const F32*)surfplane);

	Point3F start;
	F32 *pStart = (F32 *)start;

	// get the start point on the lightmap
	F32 lumelScale = 1 / (lGenX[si] * lmsheetsize.x);
	pStart[si] = (((xoff * lumelScale) / (1 / lGenX[si])) - lGenX[3]) / lGenX[si];
	pStart[ti] = (((yoff * lumelScale) / (1 / lGenY[ti])) - lGenY[3]) / lGenY[ti];
	pStart[axis] = ((pNormal[si] * pStart[si]) + (pNormal[ti] * pStart[ti]) + surfplane.d) / -pNormal[axis];

	start.convolve(scale);
	transform.mulP(start);

   worldpos = Point3D(start.x, start.y, start.z);

	// get the s/t vecs oriented on the surface
   Point3F vS, vT;
	F32 * pSVec = ((F32*)vS);
	F32 * pTVec = ((F32*)vT);

	// s
	pSVec[si] = 1.f;
	pSVec[ti] = 0.f;

   F32 angle;
	Point3F planeNormal = surfplane;
	((F32*)planeNormal)[ti] = 0.f;
	planeNormal.normalize();

	angle = mAcos(mClampF(((F32*)planeNormal)[axis], -1.f, 1.f));
	pSVec[axis] = (((F32*)planeNormal)[si] < 0.f) ? mTan(angle) : -mTan(angle);

	// t
	pTVec[ti] = 1.f;
	pTVec[si] = 0.f;

	planeNormal = surfplane;
	((F32*)planeNormal)[si] = 0.f;
	planeNormal.normalize();

	angle = mAcos(mClampF(((F32*)planeNormal)[axis], -1.f, 1.f));
	pTVec[axis] = (((F32*)planeNormal)[ti] < 0.f) ? mTan(angle) : -mTan(angle);

   Point3D vS64 = Point3D(vS.x, vS.y, vS.z);
   Point3D vT64 = Point3D(vT.x, vT.y, vT.z);

	// scale the vectors
	vS64 *= lumelScale;
	vT64 *= lumelScale;

   Point3D m0 = Point3D(transform[0], transform[1], transform[2]);
   Point3D m1 = Point3D(transform[4], transform[5], transform[6]);
   Point3D m2 = Point3D(transform[8], transform[9], transform[10]);

   Point3D scale64 = Point3D(scale.x, scale.y, scale.z);

   vectS.x = mDot(vS64, m0);
   vectS.y = mDot(vS64, m1);
   vectS.z = mDot(vS64, m2);
   vectS.convolve(scale64);

   vectT.x = mDot(vT64, m0);
   vectT.y = mDot(vT64, m1);
   vectT.z = mDot(vT64, m2);
   vectT.convolve(scale64);

	// project vecs
	//transform.mulV(vectS);
	//vectS.convolve(scale);

	//transform.mulV(vectT);
	//vectT.convolve(scale);
}

void SceneLighting::InteriorProxy::sgProcessSurface(sgSurfaceInfo &surfaceinfo)
{
   sgPlanarLightMap *lightmap = new sgPlanarLightMap(surfaceinfo.sgLightMapExtent.x, surfaceinfo.sgLightMapExtent.y,
      sgInterior, surfaceinfo.sgDetail, surfaceinfo.sgSurfaceIndex, surfaceinfo.sgStaticMesh, surfaceinfo.sgSurfacePlane, surfaceinfo.sgTriStrip);

	lightmap->sgWorldPosition = surfaceinfo.sgWorldPos;
	lightmap->sgLightMapSVector = surfaceinfo.sgSVector;
	lightmap->sgLightMapTVector = surfaceinfo.sgTVector;
	lightmap->sgSetupLighting();

	for(U32 ii=0; ii<sgLights.size(); ii++)
	{
		// should we even bother?
		LightInfo *light = sgLights[ii];

      if(light->mType != LightInfo::Vector)
      {
         bool valid = false;
         if((light->sgLocalAmbientAmount > SG_MIN_LEXEL_INTENSITY) && light->sgDoubleSidedAmbient)
            valid = true;
         if(surfaceinfo.sgSurfacePlane.distToPlane(light->mPos) > 0)
            valid = true;
         for(U32 v=0; v<surfaceinfo.sgTriStrip.size(); v++)
         {
            sgPlanarLightMap::sgSmoothingVert &vert = surfaceinfo.sgTriStrip[v];
            if(mDot(vert.sgNorm, (light->mPos - vert.sgVert)) > 0)
               valid = true;
         }

         if(!valid)
            continue;
      }
      else
      {
         if(!surfaceinfo.sgSurfaceOutsideVisible)
            continue;
      }

      lightmap->sgCalculateLighting(light);
	}

	if(lightmap->sgIsDirty())
	{
      TextureHandle *normHandle = gInteriorLMManager.duplicateBaseLightmap(
         surfaceinfo.sgDetail->getLMHandle(), sgInterior->getLMHandle(), surfaceinfo.sgLightMapIndex);
		GBitmap *normLightmap = normHandle->getBitmap();
      lightmap->sgMergeLighting(normLightmap, surfaceinfo.sgLightMapOffset.x, surfaceinfo.sgLightMapOffset.y);
	}

	delete lightmap;
}

void SceneLighting::addInterior(ShadowVolumeBSP * shadowVolume, InteriorProxy & interior, LightInfo * light, S32 level)
{
	if(light->mType != LightInfo::Vector)
		return;

	ColorF ambient = light->mAmbient;

	bool shadowedTree = true;

	// check if just getting shadow detail
	if(level == SHADOW_DETAIL)
	{
		shadowedTree = false;
		level = interior->mInteriorRes->getNumDetailLevels() - 1;
	}

	Interior * detail = interior->mInteriorRes->getDetailLevel(level);
	bool hasAlarm = detail->hasAlarmState();

	// make sure surfaces do not get processed more than once
	BitVector surfaceProcessed;
	surfaceProcessed.setSize(detail->mSurfaces.size());
	surfaceProcessed.clear();

	bool isoutside = false;
	for(U32 zone=0; zone<interior->getNumCurrZones(); zone++)
	{
		if(interior->getCurrZone(zone) == 0)
		{
			isoutside = true;
			break;
		}
	}
	if(!isoutside)
		return;

	for(U32 i = 0; i < detail->getNumZones(); i++)
	{
		Interior::Zone & zone = detail->mZones[i];
		for(U32 j = 0; j < zone.surfaceCount; j++)
		{
			U32 surfaceIndex = detail->mZoneSurfaces[zone.surfaceStart + j];

			// dont reprocess a surface
			if(surfaceProcessed.test(surfaceIndex))
				continue;
			surfaceProcessed.set(surfaceIndex);

			Interior::Surface & surface = detail->mSurfaces[surfaceIndex];

			// outside visible?
			if(!(surface.surfaceFlags & Interior::SurfaceOutsideVisible))
				continue;

			// good surface?
			PlaneF plane = detail->getPlane(surface.planeIndex);
			if(Interior::planeIsFlipped(surface.planeIndex))
				plane.neg();

			// project the plane
			PlaneF projPlane;
			mTransformPlane(interior->getTransform(), interior->getScale(), plane, &projPlane);

			// fill with ambient? (need to do here, because surface will not be
			// added to the SVBSP tree)
			F32 dot = mDot(projPlane, light->mDirection);
			if(dot > -gParellelVectorThresh)
				continue;

			ShadowVolumeBSP::SVPoly * poly = buildInteriorPoly(shadowVolume, interior, detail,
				surfaceIndex, light, shadowedTree);

			// insert it into the SVBSP tree
			shadowVolume->insertPoly(poly);
		}
	}
}

//------------------------------------------------------------------------------
ShadowVolumeBSP::SVPoly * SceneLighting::buildInteriorPoly(ShadowVolumeBSP * shadowVolumeBSP,
														   InteriorProxy & interior, Interior * detail, U32 surfaceIndex, LightInfo * light,
														   bool createSurfaceInfo)
{
	// transform and add the points...
	const MatrixF & transform = interior->getTransform();
	const VectorF & scale = interior->getScale();

	const Interior::Surface & surface = detail->mSurfaces[surfaceIndex];

	ShadowVolumeBSP::SVPoly * poly = shadowVolumeBSP->createPoly();

	poly->mWindingCount = surface.windingCount;

	// project these points
	for(U32 j = 0; j < poly->mWindingCount; j++)
	{
		Point3F iPnt = detail->mPoints[detail->mWindings[surface.windingStart + j]].point;
		Point3F tPnt;
		iPnt.convolve(scale);
		transform.mulP(iPnt, &tPnt);
		poly->mWinding[j] = tPnt;
	}

	// convert from fan
	U32 tmpIndices[ShadowVolumeBSP::SVPoly::MaxWinding];
	Point3F fanIndices[ShadowVolumeBSP::SVPoly::MaxWinding];

	tmpIndices[0] = 0;

	U32 idx = 1;
	U32 i;
	for(i = 1; i < poly->mWindingCount; i += 2)
		tmpIndices[idx++] = i;
	for(i = ((poly->mWindingCount - 1) & (~0x1)); i > 0; i -= 2)
		tmpIndices[idx++] = i;

	idx = 0;
	for(i = 0; i < poly->mWindingCount; i++)
		if(surface.fanMask & (1 << i))
			fanIndices[idx++] = poly->mWinding[tmpIndices[i]];

	// set the data
	poly->mWindingCount = idx;
	for(i = 0; i < poly->mWindingCount; i++)
		poly->mWinding[i] = fanIndices[i];

	// flip the plane - shadow volumes face inwards
	PlaneF plane = detail->getPlane(surface.planeIndex);
	if(!Interior::planeIsFlipped(surface.planeIndex))
		plane.neg();

	// transform the plane
	mTransformPlane(transform, scale, plane, &poly->mPlane);
	shadowVolumeBSP->buildPolyVolume(poly, light);

	// do surface info?
	if(createSurfaceInfo)
	{
		ShadowVolumeBSP::SurfaceInfo * surfaceInfo = new ShadowVolumeBSP::SurfaceInfo;
		shadowVolumeBSP->mSurfaces.push_back(surfaceInfo);

		// fill it
		surfaceInfo->mSurfaceIndex = surfaceIndex;
		surfaceInfo->mShadowVolume = shadowVolumeBSP->getShadowVolume(poly->mShadowVolume);

		// POLY and POLY node gets it too
		ShadowVolumeBSP::SVNode * traverse = shadowVolumeBSP->getShadowVolume(poly->mShadowVolume);
		while(traverse->mFront)
		{
			traverse->mSurfaceInfo = surfaceInfo;
			traverse = traverse->mFront;
		}

		// get some info from the poly node
		poly->mSurfaceInfo = traverse->mSurfaceInfo = surfaceInfo;
		surfaceInfo->mPlaneIndex = traverse->mPlaneIndex;
	}

	return(poly);
}


SceneLighting::InteriorProxy::InteriorProxy(SceneObject * obj) :
Parent(obj)
{
	mBoxShadowBSP = 0;

	sgCurrentSurfaceIndex = 0;
	sgSurfacesPerPass = 0;
}

SceneLighting::InteriorProxy::~InteriorProxy()
{
   sgClearSurfaces();

	delete mBoxShadowBSP;
}

bool SceneLighting::InteriorProxy::loadResources()
{
	InteriorInstance * interior = getObject();
	if(!interior)
		return(false);

	Resource<InteriorResource> & interiorRes = interior->getResource();
	if(!bool(interiorRes))
		return(false);

   if(!gInteriorLMManager.loadBaseLightmaps(interiorRes->getDetailLevel(0)->getLMHandle(),
      interior->getLMHandle()))
      return(false);

	return(true);
}

void SceneLighting::InteriorProxy::init()
{
	InteriorInstance * interior = getObject();
	if(!interior)
		return;

   // clear out the lightmaps
   for(U32 i = 0; i < interior->getResource()->getNumDetailLevels(); i++)
   {
      Interior * detail = interior->getResource()->getDetailLevel(i);
      gInteriorLMManager.clearLightmaps(detail->getLMHandle(), interior->getLMHandle());
   }
}

/// reroutes InteriorProxy::preLight for point light and TSStatic support.
bool SceneLighting::InteriorProxy::preLight(LightInfo * light)
{
	// create shadow volume of the bounding box of this object
	InteriorInstance * interior = getObject();
	if(!interior)
		return(false);

	if(!sgRelightFilter::sgAllowLighting(interior->getWorldBox(), false))
		return false;

	// build light list...
	sgAddLight(light, interior);
	return(true);
}

bool SceneLighting::InteriorProxy::isShadowedBy(InteriorProxy * test)
{
	// add if overlapping world box
	if((*this)->getWorldBox().isOverlapped((*test)->getWorldBox()))
		return(true);

	// test the box shadow volume
	for(U32 i = 0; i < mLitBoxSurfaces.size(); i++)
	{
		ShadowVolumeBSP::SVPoly * poly = mBoxShadowBSP->copyPoly(mLitBoxSurfaces[i]);
		if(test->mBoxShadowBSP->testPoly(poly))
			return(true);
	}

	return(false);
}

void SceneLighting::InteriorProxy::postLight(bool lastLight)
{
	delete mBoxShadowBSP;
	mBoxShadowBSP = 0;

	InteriorInstance * interior = getObject();
	if(!interior)
		return;

   // only rebuild the vertex colors after the last light
   if(lastLight)
      interior->rebuildVertexColors();
}

//------------------------------------------------------------------------------
U32 SceneLighting::InteriorProxy::getResourceCRC()
{
	InteriorInstance * interior = getObject();
	if(!interior)
		return(0);
	return(interior->getCRC());
}

//------------------------------------------------------------------------------
bool SceneLighting::InteriorProxy::setPersistInfo(PersistInfo::PersistChunk * info)
{

	if(!Parent::setPersistInfo(info))
		return(false);

	PersistInfo::InteriorChunk * chunk = dynamic_cast<PersistInfo::InteriorChunk*>(info);
	AssertFatal(chunk, "SceneLighting::InteriorProxy::setPersistInfo: invalid info chunk!");

	InteriorInstance * interior = getObject();
	if(!interior)
		return(false);

	U32 numDetails = interior->getNumDetailLevels();

	// check the lighting method
	AssertFatal(SceneLighting::smUseVertexLighting == Interior::smUseVertexLighting, "SceneLighting::InteriorProxy::setPersistInfo: invalid vertex lighting state");
	if(SceneLighting::smUseVertexLighting != Interior::smUseVertexLighting)
		return(false);

   // process the vertex lighting...
   if(chunk->mDetailVertexCount.size() != numDetails)
      return(false);

   AssertFatal(chunk->mVertexColorsNormal.size(), "SceneLighting::InteriorProxy::setPersistInfo: invalid chunk info");
   AssertFatal(!chunk->mHasAlarmState || chunk->mVertexColorsAlarm.size(), "SceneLighting::InteriorProxy::setPersistInfo: invalid chunk info");
   AssertFatal(!(chunk->mHasAlarmState ^ interior->getDetailLevel(0)->hasAlarmState()), "SceneLighting::InteriorProxy::setPersistInfo: invalid chunk info");

   U32 curPos = 0;
   for(U32 i = 0; i < numDetails; i++)
   {
      U32 count = chunk->mDetailVertexCount[i];
      Vector<ColorI>* normal = interior->getVertexColorsNormal(i);
      Vector<ColorI>* alarm  = interior->getVertexColorsAlarm(i);
      AssertFatal(normal != NULL && alarm != NULL, "Error, bad vectors returned!");

      normal->setSize(count);
      dMemcpy(normal->address(), &chunk->mVertexColorsNormal[curPos], count * sizeof(ColorI));

      if(chunk->mHasAlarmState)
      {
         alarm->setSize(count);
         dMemcpy(alarm->address(), &chunk->mVertexColorsAlarm[curPos], count * sizeof(ColorI));
      }

      curPos += count;
   }

	// need lightmaps?
	if(!SceneLighting::smUseVertexLighting)
	{
		if(chunk->mDetailLightmapCount.size() != numDetails)
			return(false);

		LM_HANDLE instanceHandle = interior->getLMHandle();
		U32 idx = 0;

		for(U32 i = 0; i < numDetails; i++)
		{
			Interior * detail = interior->getDetailLevel(i);

			LM_HANDLE interiorHandle = detail->getLMHandle();
			Vector<TextureHandle *> & baseHandles = gInteriorLMManager.getHandles(interiorHandle, 0);

			if(chunk->mDetailLightmapCount[i] > baseHandles.size())
				return(false);

			for(U32 j = 0; j < chunk->mDetailLightmapCount[i]; j++)
			{
				U32 baseIndex = chunk->mDetailLightmapIndices[idx];
				if(baseIndex >= baseHandles.size())
					return(false);

				AssertFatal(chunk->mLightmaps[idx], "SceneLighting::InteriorProxy::setPersistInfo: bunk bitmap!");
				if(chunk->mLightmaps[idx]->getWidth() != baseHandles[baseIndex]->getWidth() ||
					chunk->mLightmaps[idx]->getHeight() != baseHandles[baseIndex]->getHeight())
					return(false);

				TextureHandle *tHandle = gInteriorLMManager.duplicateBaseLightmap(interiorHandle, instanceHandle, baseIndex);

				// create the diff bitmap
				U8 * pDiff = chunk->mLightmaps[idx]->getAddress(0,0);
				U8 * pBase = baseHandles[baseIndex]->getBitmap()->getAddress(0,0);
				U8 * pDest = tHandle->getBitmap()->getAddress(0,0);

				Point2I extent(tHandle->getWidth(), tHandle->getHeight());
				for(U32 y = 0; y < extent.y; y++)
				{
					for(U32 x = 0; x < extent.x; x++)
					{
						*pDest++ = *pBase++ + *pDiff++;
						*pDest++ = *pBase++ + *pDiff++;
						*pDest++ = *pBase++ + *pDiff++;
					}
				}

				idx++;
			}
		}
	}

	return(true);
}

bool SceneLighting::InteriorProxy::getPersistInfo(PersistInfo::PersistChunk * info)
{
	if(!Parent::getPersistInfo(info))
		return(false);

	PersistInfo::InteriorChunk * chunk = dynamic_cast<PersistInfo::InteriorChunk*>(info);
	AssertFatal(chunk, "SceneLighting::InteriorProxy::getPersistInfo: invalid info chunk!");

	InteriorInstance * interior = getObject();
	if(!interior)
		return(false);

	LM_HANDLE instanceHandle = interior->getLMHandle();

	AssertFatal(!chunk->mDetailLightmapCount.size(), "SceneLighting::InteriorProxy::getPersistInfo: invalid array!");
	AssertFatal(!chunk->mDetailLightmapIndices.size(), "SceneLighting::InteriorProxy::getPersistInfo: invalid array!");
	AssertFatal(!chunk->mLightmaps.size(), "SceneLighting::InteriorProxy::getPersistInfo: invalid array!");

	U32 numDetails = interior->getNumDetailLevels();
	U32 i;
	for(i = 0; i < numDetails; i++)
	{
		Interior * detail = interior->getDetailLevel(i);
		LM_HANDLE interiorHandle = detail->getLMHandle();

      Vector<TextureHandle*> & baseHandles = gInteriorLMManager.getHandles(interiorHandle, 0);
      Vector<TextureHandle*> & instanceHandles = gInteriorLMManager.getHandles(interiorHandle, instanceHandle);

		U32 litCount = 0;

		// walk all the instance lightmaps and grab diff lighting from them
		for(U32 j = 0; j < instanceHandles.size(); j++)
		{
			if(!instanceHandles[j])
				continue;

			litCount++;
			chunk->mDetailLightmapIndices.push_back(j);

			GBitmap * baseBitmap = baseHandles[j]->getBitmap();
			GBitmap * instanceBitmap = instanceHandles[j]->getBitmap();

			Point2I extent(baseBitmap->getWidth(), baseBitmap->getHeight());

         GBitmap * diffLightmap = new GBitmap(extent.x, extent.y, false, GBitmap::RGB);

			U8 * pBase = baseBitmap->getAddress(0,0);
			U8 * pInstance = instanceBitmap->getAddress(0,0);
			U8 * pDest = diffLightmap->getAddress(0,0);

			// fill the diff lightmap
			for(U32 y = 0; y < extent.y; y++)
				for(U32 x = 0; x < extent.x; x++)
				{
					*pDest++ = *pInstance++ - *pBase++;
					*pDest++ = *pInstance++ - *pBase++;
					*pDest++ = *pInstance++ - *pBase++;
				}

				chunk->mLightmaps.push_back(diffLightmap);
		}

		chunk->mDetailLightmapCount.push_back(litCount);
	}

	// process the vertex lighting...
	AssertFatal(!chunk->mDetailVertexCount.size(), "SceneLighting::InteriorProxy::getPersistInfo: invalid chunk info");
	AssertFatal(!chunk->mVertexColorsNormal.size(), "SceneLighting::InteriorProxy::getPersistInfo: invalid chunk info");
	AssertFatal(!chunk->mVertexColorsAlarm.size(), "SceneLighting::InteriorProxy::getPersistInfo: invalid chunk info");

	chunk->mHasAlarmState = interior->getDetailLevel(0)->hasAlarmState();
	chunk->mDetailVertexCount.setSize(numDetails);

	U32 size = 0;
	for(i = 0; i < numDetails; i++)
	{
		Interior * detail = interior->getDetailLevel(i);

		U32 count = detail->getWindingCount();
		chunk->mDetailVertexCount[i] = count;
		size += count;
	}

   chunk->mVertexColorsNormal.setSize(size);
   if(chunk->mHasAlarmState)
      chunk->mVertexColorsAlarm.setSize(size);

   U32 curPos = 0;
   for(i = 0; i < numDetails; i++)
   {
      Vector<ColorI>* normal = interior->getVertexColorsNormal(i);
      Vector<ColorI>* alarm  = interior->getVertexColorsAlarm(i);
      AssertFatal(normal != NULL && alarm != NULL, "Error, no normal or alarm vertex colors!");

      U32 count = chunk->mDetailVertexCount[i];
      dMemcpy(&chunk->mVertexColorsNormal[curPos], normal->address(), count * sizeof(ColorI));

      if(chunk->mHasAlarmState)
         dMemcpy(&chunk->mVertexColorsAlarm[curPos], alarm->address(), count * sizeof(ColorI));

      curPos += count;
   }

	return(true);
}