//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#ifndef _SGLIGHTMANAGER_H_
#define _SGLIGHTMANAGER_H_

#ifndef _MMATH_H_
#include "math/mMath.h"
#endif
#ifndef _COLOR_H_
#include "core/color.h"
#endif
#ifndef _TVECTOR_H_
#include "core/tVector.h"
#endif
#ifndef _DATACHUNKER_H_
#include "core/dataChunker.h"
#endif
#include "console/console.h"
#include "console/consoleTypes.h"
#include "core/stringTable.h"
#include "core/crc.h"
#include "lightingSystem/sgHashMap.h"


class SceneObject;

static void sgFindObjectsCallback(SceneObject* obj, void *val)
{
	Vector<SceneObject*> * list = (Vector<SceneObject*>*)val;
	list->push_back(obj);
}


//-----------------------------------------------
// Original name maintained due to widespread use...

class LightInfo
{
	friend class LightManager;

public:
	enum Type {
		Point    = 0,
		Spot     = 1,
		Vector   = 2,
		Ambient  = 3,
		SGStaticPoint,
		SGStaticSpot
	};
	Type        mType;

	Point3F     mPos;
	VectorF     mDirection;
	ColorF      mColor;
	ColorF      mAmbient;
	F32         mRadius;
	S32         mScore;

public:
	F32 sgSpotAngle;
	bool sgAssignedToTSObject;
	bool sgCastsShadows;
	bool sgDiffuseRestrictZone;
	bool sgAmbientRestrictZone;
	S32 sgZone[2];
	F32 sgLocalAmbientAmount;
	bool sgSmoothSpotLight;
	bool sgDoubleSidedAmbient;
	bool sgAssignedToParticleSystem;
	StringTableEntry sgLightingModelName;
	bool sgUseNormals;
	MatrixF sgLightingTransform;
	PlaneF sgSpotPlane;

   // temp data used by the system...
   Point3F sgTempModelInfo;
   F32 sgDTSLightingOcclusionAdjust;

	// this increases on tracked lights as they move...
	U32 sgMoveSnapshotId;
	bool sgTrackMoveSnapshot;

	LightInfo();
	bool sgIsInZone(S32 zone);
	bool sgAllowDiffuseZoneLighting(S32 zone);
};

class LightInfoList : public Vector<LightInfo*>
{
public:
	void sgRegisterLight(LightInfo *light);
	void sgUnregisterLight(LightInfo *light);
};


//-----------------------------------------------
// Original name maintained due to widespread use...

class LightManager
{
public:
	enum sgSpecialLightTypesEnum
	{
		sgSunLightType,

		sgSpecialLightTypesCount
	};

	LightManager()
   {
      dMemset(&sgSpecialLights, 0, sizeof(sgSpecialLights));
      sgInit();
   }

	// registered before scene traversal...
	void sgRegisterGlobalLight(LightInfo *light);
	void sgUnregisterGlobalLight(LightInfo *light) {sgRegisteredGlobalLights.sgUnregisterLight(light);}
	// registered per object...
	void sgRegisterLocalLight(LightInfo *light) {sgRegisteredLocalLights.sgRegisterLight(light);}
	void sgUnregisterLocalLight(LightInfo *light) {sgRegisteredLocalLights.sgUnregisterLight(light);}

	void sgRegisterGlobalLights(bool staticlighting);
	void sgUnregisterAllLights();

	/// Returns all unsorted and un-scored lights (both global and local).
	void sgGetAllUnsortedLights(LightInfoList &list);
	/// Copies the best lights list - this DOESN'T find the lights!  Call
	/// sgSetupLights to populate the list.
	void sgGetBestLights(LightInfoList &list)
	{
		list.clear();
		list.merge(sgBestLights);
	}

	/// For DST objects.  Finds the best lights
	/// including a composite based on the environmental
	/// ambient lighting amount *and installs them in OpenGL*.
	void sgSetupLights(SceneObject *obj);
	/// For the terrain and Atlas.  Finds the best
	/// lights in the viewing area based on distance to camera.
	void sgSetupLights(SceneObject *obj, const Point3F &camerapos,
		const Point3F &cameradir, F32 viewdist, S32 maxlights);
	/// Finds the best lights that overlap with the bounding box
	/// based on the box center.
	void sgSetupLights(SceneObject *obj, const Box3F &box, S32 maxlights);
   void sgFilterLights(bool allowstatics, bool allowdynamics);
	/// Add the top 8 best lights to OpenGL.
	void sgInstallLights();
	/// Reset the best lights list and all associated data.
	void sgResetLights();

private:
	LightInfo *sgSpecialLights[sgSpecialLightTypesCount];
public:
	LightInfo *sgGetSpecialLight(sgSpecialLightTypesEnum type);
	void sgSetSpecialLight(sgSpecialLightTypesEnum type, LightInfo *light) {sgSpecialLights[type] = light;}
	void sgClearSpecialLights();

private:
	// registered before scene traversal...
	LightInfoList sgRegisteredGlobalLights;
	// registered per object...
	LightInfoList sgRegisteredLocalLights;

	// best lights per object...
	LightInfoList sgBestLights;
	void sgFindBestLights(SceneObject *obj, const Box3F &box, S32 maxlights, const Point3F &viewdir, bool camerabased);

	// used in DTS lighting...
	void sgScoreLight(LightInfo *light, SceneObject *obj, const Box3F &box, const SphereF &sphere, bool camerabased);

public:
	enum lightingProfileQualityType
	{
		// highest quality - for in-game and final tweaks...
		lpqtProduction = 0,
		// medium quality - for lighting layout...
		lpqtDesign = 1,
		// low quality - for object placement...
		lpqtDraft = 2
	};
   enum dynamicLightingQualityType
   {
      dlqtSevenSample = 0,
      dlqtFourSample = 1,
      dlqtCenterSample = 2,
      dlqtNoShadows = 3
   };
	enum sgLightingPropertiesEnum
	{
		sgReceiveSunLightProp,
		sgAdaptiveSelfIlluminationProp,
		sgCustomAmbientLightingProp,
		sgCustomAmbientForSelfIlluminationProp,

		sgPropertyCount
	};
	static bool sgGetProperty(U32 prop)
	{
		AssertFatal((prop < sgPropertyCount), "Invalid property type!");
		return sgLightingProperties[prop];
	}
	static void sgSetProperty(U32 prop, bool val)
	{
		AssertFatal((prop < sgPropertyCount), "Invalid property type!");
		sgLightingProperties[prop] = val;
	}
	static bool sgAllowDiffuseCustomAmbient() {return sgLightingProperties[sgCustomAmbientLightingProp] && (!sgLightingProperties[sgCustomAmbientForSelfIlluminationProp]);}
	static bool sgAllowAdaptiveSelfIllumination() {return sgLightingProperties[sgAdaptiveSelfIlluminationProp];}
	static bool sgAllowCollectSelfIlluminationColor() {return !sgLightingProperties[sgCustomAmbientLightingProp];}
	static bool sgAllowReceiveSunLight() {return sgLightingProperties[sgReceiveSunLightProp] && (!sgAllowDiffuseCustomAmbient());}
private:
	static bool sgLightingProperties[sgPropertyCount];
	static U32 sgLightingProfileQuality;
	static bool sgLightingProfileAllowShadows;
	static LightInfo sgDefaultLight;
	static bool sgDetailMaps;
   static U32 sgDynamicLightingOcclusionQuality;
	static bool sgUseDynamicShadows;
	static U32 sgDynamicShadowQuality;

	static bool sgUseSelfIlluminationColor;
	static ColorF sgSelfIlluminationColor;
	static bool sgDynamicParticleSystemLighting;
	static bool sgBlendedTerrainDynamicLighting;
	static bool sgFilterZones;
	static S32 sgZones[2];
	static S32 sgMaxBestLights;
	static bool sgInGUIEditor;

public:
	static bool sgMultipleDynamicShadows;

   static S32 sgGetMaxBestLights() {return sgMaxBestLights;}

	static bool sgAllowDynamicShadows() {return sgUseDynamicShadows;}
	static U32 sgGetDynamicShadowQuality() {return sgDynamicShadowQuality;}
	static void sgSetDynamicShadowQuality(U32 quality) {sgDynamicShadowQuality = quality;}
	static bool sgAllowDynamicParticleSystemLighting() {return sgDynamicParticleSystemLighting;}
	static bool sgAllowBlendedTerrainDynamicLighting();
	static ColorF sgGetSelfIlluminationColor(ColorF defaultcol)
	{
		if(sgUseSelfIlluminationColor)
			return sgSelfIlluminationColor;
		return defaultcol;
	}
	static void sgInit();
	static bool sgAllowDetailMaps();
	static bool sgAllowShadows() {return sgLightingProfileAllowShadows;}
	static bool sgAllowFullLightMaps() {return (sgLightingProfileQuality == lpqtProduction);}
	static U32 sgGetLightMapScale()
	{
		if(sgLightingProfileQuality == lpqtDesign) return 2;
		if(sgLightingProfileQuality == lpqtDraft) return 4;
		return 1;
	}
	static void sgGetFilteredLightColor(ColorF &color, ColorF &ambient, S32 objectzone);
	static void sgSetupZoneLighting(bool enable, SimObject *sobj);
	static void sgSetupExposureRendering();
	static void sgResetExposureRendering();
	static void sgSetAmbientSelfIllumination(LightInfo *lightinfo, F32 *lightColor,
		F32 *ambientColor);

private:
	static S32 QSORT_CALLBACK sgSortLightsByAddress(const void *, const void *);
	static S32 QSORT_CALLBACK sgSortLightsByScore(const void *, const void *);
};

class sgRelightFilter
{
public:
	static bool sgFilterRelight;
	static bool sgFilterRelightVisible;
	static bool sgFilterRelightByDistance;
	static F32 sgFilterRelightByDistanceRadius;
	static Point3F sgFilterRelightByDistancePosition;
	static void sgInit();
	static bool sgAllowLighting(const Box3F &box, bool forcefilter);
	static void sgRenderAllowedObjects(void *worldeditor);
};

class sgStatistics
{
public:
	static U32 sgInteriorLexelCount;
	static U32 sgInteriorLexelIlluminationCount;
	static U32 sgInteriorLexelIlluminationTime;
   static U32 sgInteriorLexelDiffuseCount;
	static U32 sgInteriorObjectCount;
	static U32 sgInteriorObjectIncludedCount;
	static U32 sgInteriorObjectIlluminationCount;
	static U32 sgInteriorSurfaceIncludedCount;
	static U32 sgInteriorSurfaceIlluminationCount;
	static U32 sgInteriorSurfaceIlluminatedCount;
   static U32 sgInteriorSurfaceSmoothedCount;
   static U32 sgInteriorSurfaceSmoothedLexelCount;
   static U32 sgInteriorSurfaceSetupTime;
   static U32 sgInteriorSurfaceSetupCount;
   static U32 sgInteriorSurfaceMergeTime;
   static U32 sgInteriorSurfaceMergeCount;
	static U32 sgStaticMeshSurfaceOccluderCount;
   static U32 sgStaticMeshBVPTPotentialOccluderCount;
   static U32 sgStaticMeshCastRayCount;
	static U32 sgTerrainLexelCount;
	static U32 sgTerrainLexelTime;

	static void sgClear();
	static void sgPrint();
};

class sgInteriorDynamicLightingCache
{
public:
	struct data
	{
		bool sgAlreadyProcessed;
		U32 sgLastMoveSnapshotId;

		// internally managed changes...
		void *sgInteriorDetail;
		U32 sgLightCRC;
	};
	typedef hash_multimap<U32, data> sgCacheEntry;
private:
	static sgCacheEntry sgCache;
public:
	static sgCacheEntry *sgFindCacheEntry(void *interiorinstance,
		void *interiordetail, LightInfo *light)
	{
		// get the hash...
		U32 val = U32(interiorinstance);
		U32 hash = calculateCRC(&val, sizeof(U32));
		val = U32(light);
		hash = calculateCRC(&val, sizeof(U32), hash);

		// get the entry...
		sgCacheEntry *entry = sgCache.find(hash);

		// verify nothings changed...
		U32 lightcrc = calculateCRC(light, sizeof(LightInfo));
		if((entry->info.sgInteriorDetail != interiordetail) ||
		   (entry->info.sgLightCRC != lightcrc))
		{
			entry->info.sgAlreadyProcessed = false;
			entry->info.sgInteriorDetail = interiordetail;
			entry->info.sgLightCRC = lightcrc;
			entry->object.clear();
		}

		return entry;
	}
};

class sgDTSDynamicLightingCache
{
public:
	struct data
	{
		bool sgAlreadyProcessed;
		U32 sgLastObjectMoveSnapshotId;
      U32 sgLastLightMoveSnapshotId;
      F32 sgCachedLightingMultiplier;
	};
	typedef hash_multimap<U32, data> sgCacheEntry;
private:
	static sgCacheEntry sgCache;
public:
	static sgCacheEntry *sgFindCacheEntry(void *sceneobject, LightInfo *light)
	{
		// get the hash...
		U32 val = U32(sceneobject);
		U32 hash = calculateCRC(&val, sizeof(U32));
		val = U32(light);
		hash = calculateCRC(&val, sizeof(U32), hash);

		// get the entry...
		return sgCache.find(hash);
	}
};


//----------------------------------------------
// performance testing

class elapsedTimeAggregate
{
private:
   U32 time;
	U32 *resultVar;
public:
	elapsedTimeAggregate(U32 &timeresultvar)
	{
		resultVar = &timeresultvar;
		time = Platform::getRealMilliseconds();
	}
	~elapsedTimeAggregate()
	{
		*resultVar += (Platform::getRealMilliseconds() - time);
	}
};

#define SG_PERFORMANCE_TESTING_OUTPUT
#ifdef SG_PERFORMANCE_TESTING_OUTPUT
class elapsedTime
{
private:
	char info[256];
	U32 time;
public:
	elapsedTime(char *infostr)
	{
		dStrcpy(info, infostr);
		time = Platform::getRealMilliseconds();
	}
	~elapsedTime()
	{
		Con::printf(info, (Platform::getRealMilliseconds() - time));
	}
};
#else
class elapsedTime
{
public:
	elapsedTime(char *infostr) {}
};
#endif


#endif//_SGLIGHTMANAGER_H_
