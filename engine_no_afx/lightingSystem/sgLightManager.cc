//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#include "lightingSystem/sgLighting.h"

#include "lightingSystem/sgLightManager.h"
#include "lightingSystem/sgLightingModel.h"
#include "lightingSystem/sgMissionLightingFilter.h"
#include "interior/interiorInstance.h"
#include "terrain/terrData.h"
#include "game/shadow.h"
#include "lightingSystem/sgLightObject.h"
#include "sim/netConnection.h"
#include "editor/worldEditor.h"
#include "platform/profiler.h"


bool LightManager::sgLightingProperties[sgPropertyCount];
bool LightManager::sgUseSelfIlluminationColor = false;
ColorF LightManager::sgSelfIlluminationColor(0.0f, 0.0f, 0.0f);
bool LightManager::sgFilterZones = false;
S32 LightManager::sgZones[2] = {-1, -1};
bool LightManager::sgDynamicParticleSystemLighting = true;
bool LightManager::sgBlendedTerrainDynamicLighting = true;
U32 LightManager::sgLightingProfileQuality = LightManager::lpqtProduction;
bool LightManager::sgLightingProfileAllowShadows = true;
LightInfo LightManager::sgDefaultLight;
bool LightManager::sgDetailMaps = true;
S32 LightManager::sgMaxBestLights = 10;
bool LightManager::sgUseDynamicShadows = true;

U32 LightManager::sgDynamicLightingOcclusionQuality = dlqtNoShadows;
U32 LightManager::sgDynamicShadowQuality = 0;
bool LightManager::sgMultipleDynamicShadows = true;
bool LightManager::sgInGUIEditor = false;

bool sgRelightFilter::sgFilterRelight = false;
bool sgRelightFilter::sgFilterRelightVisible = true;
bool sgRelightFilter::sgFilterRelightByDistance = true;
F32 sgRelightFilter::sgFilterRelightByDistanceRadius = 60;
Point3F sgRelightFilter::sgFilterRelightByDistancePosition;

U32 sgStatistics::sgInteriorLexelCount = 0;
U32 sgStatistics::sgInteriorLexelIlluminationCount = 0;
U32 sgStatistics::sgInteriorLexelIlluminationTime = 0;
U32 sgStatistics::sgInteriorLexelDiffuseCount = 0;
U32 sgStatistics::sgInteriorObjectCount = 0;
U32 sgStatistics::sgInteriorObjectIncludedCount = 0;
U32 sgStatistics::sgInteriorObjectIlluminationCount = 0;
U32 sgStatistics::sgInteriorSurfaceIncludedCount = 0;
U32 sgStatistics::sgInteriorSurfaceIlluminationCount = 0;
U32 sgStatistics::sgInteriorSurfaceIlluminatedCount = 0;
U32 sgStatistics::sgInteriorSurfaceSmoothedCount = 0;
U32 sgStatistics::sgInteriorSurfaceSmoothedLexelCount = 0;
U32 sgStatistics::sgInteriorSurfaceSetupTime = 0;
U32 sgStatistics::sgInteriorSurfaceSetupCount = 0;
U32 sgStatistics::sgInteriorSurfaceMergeTime = 0;
U32 sgStatistics::sgInteriorSurfaceMergeCount = 0;
U32 sgStatistics::sgStaticMeshSurfaceOccluderCount = 0;
U32 sgStatistics::sgStaticMeshBVPTPotentialOccluderCount = 0;
U32 sgStatistics::sgStaticMeshCastRayCount = 0;
U32 sgStatistics::sgTerrainLexelCount = 0;
U32 sgStatistics::sgTerrainLexelTime = 0;

sgInteriorDynamicLightingCache::sgCacheEntry sgInteriorDynamicLightingCache::sgCache;
sgDTSDynamicLightingCache::sgCacheEntry sgDTSDynamicLightingCache::sgCache;


LightInfo::LightInfo()
{
	mType = Vector;
	mPos = Point3F(0.0f, 0.0f, 0.0f);
	mDirection = Point3F(0.0f, 0.0f, 1.0f);
	mColor = ColorF(0.0f, 0.0f, 0.0f);
	mAmbient = ColorF(0.0f, 0.0f, 0.0f);
	mRadius = 1;
	mScore = 0;

	sgSpotAngle = 90.0f;
	sgAssignedToTSObject = false;
	sgCastsShadows = true;
	sgDiffuseRestrictZone = false;
	sgAmbientRestrictZone = false;
	sgZone[0] = -1;
	sgZone[1] = -1;
	sgLocalAmbientAmount = 0.0f;
	sgSmoothSpotLight = false;
	sgDoubleSidedAmbient = false;
	sgAssignedToParticleSystem = false;
	sgLightingModelName = NULL;
	sgUseNormals = true;
	sgSpotPlane = PlaneF(0.0f, 0.0f, 0.0f, 0.0f);
    sgDTSLightingOcclusionAdjust = 1.0f;

	sgMoveSnapshotId = 0;
	sgTrackMoveSnapshot = false;

	sgLightingTransform.identity();
}

bool LightInfo::sgIsInZone(S32 zone)
{
	if((zone == sgZone[0]) || (zone ==sgZone[1]))
		return true;
	return false;
}

bool LightInfo::sgAllowDiffuseZoneLighting(S32 zone)
{
	if(!sgDiffuseRestrictZone)
		return true;
	if(sgIsInZone(zone))
		return true;
	return false;
}

void LightInfoList::sgRegisterLight(LightInfo *light)
{
	if(!light)
		return;
	// just add the light, we'll try to scan for dupes later...
	push_back(light);
}

void LightInfoList::sgUnregisterLight(LightInfo *light)
{
	// remove all of them...
	LightInfoList &list = *this;
	for(U32 i=0; i<list.size(); i++)
	{
		if(list[i] != light)
			continue;
		// this moves last to i, which allows
		// the search to continue forward...
		list.erase_fast(i);
		// want to check this location again...
		i--;
	}
}


//-----------------------------------------------
//-----------------------------------------------

bool LightManager::sgAllowBlendedTerrainDynamicLighting()
{
	if(!sgBlendedTerrainDynamicLighting)
		return false;
	return (dglGetMaxTextureUnits() > 2);
}

LightInfo *LightManager::sgGetSpecialLight(sgSpecialLightTypesEnum type)
{
	if(sgSpecialLights[type])
		return sgSpecialLights[type];
	// return a default light...
	return &sgDefaultLight;
}

void LightManager::sgRegisterGlobalLight(LightInfo *light)
{
	sgRegisteredGlobalLights.sgRegisterLight(light);
}

void LightManager::sgRegisterGlobalLights(bool staticlighting)
{
	// make sure we're clean...
	sgUnregisterAllLights();

	// ask all light objects to register themselves...
	SimSet *lightset = Sim::getLightSet();
	for(SimObject **itr=lightset->begin(); itr!=lightset->end(); itr++)
		(*itr)->registerLights(this, staticlighting);
}

void LightManager::sgUnregisterAllLights()
{
	sgRegisteredGlobalLights.clear();
	sgRegisteredLocalLights.clear();

	dMemset(&sgSpecialLights, 0, sizeof(sgSpecialLights));
}

void LightManager::sgGetAllUnsortedLights(LightInfoList &list)
{
	list.clear();
	list.merge(sgRegisteredGlobalLights);
	list.merge(sgRegisteredLocalLights);

	// find dupes...
	dQsort(list.address(), list.size(), sizeof(LightInfo*), sgSortLightsByAddress);
	LightInfo *last = NULL;
	for(U32 i=0; i<list.size(); i++)
	{
		if(list[i] == last)
		{
			list.erase(i);
			i--;
			continue;
		}
		last = list[i];
	}
}

void LightManager::sgSetupLights(SceneObject *obj)
{
   PROFILE_START(LightManager_sgSetupLights);

	sgResetLights();

	bool outside = false;
	for(U32 i=0; i<obj->getNumCurrZones(); i++)
	{
		if(obj->getCurrZone(i) == 0)
		{
			outside = true;
			break;
		}
	}

	sgSetProperty(sgReceiveSunLightProp, obj->receiveSunLight);
	sgSetProperty(sgAdaptiveSelfIlluminationProp, obj->useAdaptiveSelfIllumination);
	sgSetProperty(sgCustomAmbientLightingProp, obj->useCustomAmbientLighting);
	sgSetProperty(sgCustomAmbientForSelfIlluminationProp, obj->customAmbientForSelfIllumination);

	ColorF ambientColor;
	ColorF selfillum = obj->customAmbientLighting * 0.5f;

	LightInfo *sun = sgGetSpecialLight(sgSunLightType);
	if(obj->getLightingAmbientColor(&ambientColor))
	{
		const F32 directionalFactor = 0.5f;
		const F32 ambientFactor = 0.5f;

		dMemset(&obj->mLightingInfo.smAmbientLight, 0, sizeof(obj->mLightingInfo.smAmbientLight));

		LightInfo &light = obj->mLightingInfo.smAmbientLight;
		light.mType = LightInfo::Ambient;
		light.mDirection = VectorF(0.0f, 0.0f, -1.0f);
		light.sgCastsShadows = sun->sgCastsShadows;

		// players, vehicles, ...
		if(obj->overrideOptions)
		{
			if(outside)
			{
				light.mType = LightInfo::Vector;
				light.mDirection = sun->mDirection;
			}
			//else
			//{
				light.mColor = ambientColor * directionalFactor;
				light.mAmbient = ambientColor * ambientFactor;
			//}
		}// beyond here are the static dts options...
		else if(sgAllowDiffuseCustomAmbient())
		{
			light.mColor = obj->customAmbientLighting * 0.5f;
			light.mAmbient = obj->customAmbientLighting * 0.5f;
		}
		else if(sgAllowReceiveSunLight() && sun)
		{
			light.mType = LightInfo::Vector;
			if(outside)
				light.mDirection = sun->mDirection;
			if(obj->receiveLMLighting)
				light.mColor = ambientColor * 0.8f;
			else
				light.mColor = sun->mColor;
			light.mAmbient = sun->mAmbient;
		}
		else if(obj->receiveLMLighting)
		{
			light.mColor = ambientColor * directionalFactor;
			light.mAmbient = ambientColor * ambientFactor;
		}

		if(sgAllowCollectSelfIlluminationColor())
		{
			selfillum = light.mAmbient + light.mColor;
			selfillum.clamp();
		}

		light.mPos = light.mDirection * -10000.0f;
		sgRegisterLocalLight(&obj->mLightingInfo.smAmbientLight);
	}

	// install assigned baked lights from simgroup...
	// step one get the objects...
	U32 i;
	NetConnection *connection = NetConnection::getConnectionToServer();

	for(i=0; i<obj->lightIds.size(); i++)
	{
		SimObject *sim = connection->resolveGhost(obj->lightIds[i]);
		if(!sim)
			continue;

		sgLightObject *light = dynamic_cast<sgLightObject*>(sim);
		if(!light)
			continue;

		sgLightObjectData *data = static_cast<sgLightObjectData *>(light->getDataBlock());
		if((data) && (data->sgStatic) && (data->mLightOn) && (light->mEnable))
		{
			light->mLight.sgAssignedToTSObject = true;
			sgRegisterLocalLight(&light->mLight);
		}
	}

	// step three install dynamic lights...
	sgUseSelfIlluminationColor = sgGetProperty(sgAdaptiveSelfIlluminationProp);
	if(sgUseSelfIlluminationColor)
		sgSelfIlluminationColor = selfillum;

	sgSetupZoneLighting(true, obj);

	sgFindBestLights(obj, obj->getRenderWorldBox(), sgMaxBestLights, Point3F(0.0f, 0.0f, 0.0f), false);
	sgInstallLights();

   PROFILE_END();
}

void LightManager::sgSetupLights(SceneObject *obj, const Point3F &camerapos,
								 const Point3F &cameradir, F32 viewdist, S32 maxlights)
{
	sgResetLights();

	sgSetupZoneLighting(true, obj);

	Box3F box;
	box.max = camerapos + Point3F(viewdist, viewdist, viewdist);
	box.min = camerapos - Point3F(viewdist, viewdist, viewdist);
	sgFindBestLights(obj, box, maxlights, cameradir, true);
}

void LightManager::sgSetupLights(SceneObject *obj, const Box3F &box, S32 maxlights)
{
	sgResetLights();
	sgSetupZoneLighting(true, obj);
	sgFindBestLights(obj, box, maxlights, Point3F(0.0f, 0.0f, 0.0f), true);
}

void LightManager::sgFilterLights(bool allowstatics, bool allowdynamics)
{
   for(S32 i=0; i<sgBestLights.size(); i++)
   {
      LightInfo *light = sgBestLights[i];
      if(allowstatics && ((light->mType == LightInfo::SGStaticPoint) || (light->mType == LightInfo::SGStaticSpot)))
         continue;
      if(allowdynamics && ((light->mType != LightInfo::SGStaticPoint) && (light->mType != LightInfo::SGStaticSpot)))
         continue;

      sgBestLights.erase_fast(i);
      i--;
   }
}

void LightManager::sgInstallLights()
{
	U32 count = getMin(U32(sgBestLights.size()), U32(8));
	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (const F32 *)ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	for(U32 i=0; i<count; i++)
	{
		LightInfo *light = sgBestLights[i];
		sgLightingModel &lightingmodel = sgLightingModelManager::sgGetLightingModel(
			light->sgLightingModelName);
		lightingmodel.sgSetState(light);
		
		U32 gllight = GL_LIGHT0 + i;
		glEnable(gllight);
		
		lightingmodel.sgLightingGL(gllight);
		lightingmodel.sgResetState();
	}
}

void LightManager::sgResetLights()
{
	sgSetupZoneLighting(false, NULL);
	sgRegisteredLocalLights.clear();
	sgBestLights.clear();
	
	// ok these seem weird, but dts rendering changes them...
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (const F32 *)ColorF(0.0f, 0.0f, 0.0f, 0.0f));
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (const F32 *)ColorF(1.0f, 1.0f, 1.0f, 1.0f));

	for(S32 i=7; i>=0; i--)
		glDisable(GL_LIGHT0 + i);
	glDisable(GL_LIGHTING);
}

void LightManager::sgFindBestLights(SceneObject *obj, const Box3F &box, S32 maxlights, const Point3F &viewdi, bool camerabased)
{
	sgBestLights.clear();

	// gets them all and removes any dupes...
	sgGetAllUnsortedLights(sgBestLights);

	SphereF sphere;
	box.getCenter(&sphere.center);
	sphere.radius = Point3F(box.max - sphere.center).len();

	for(U32 i=0; i<sgBestLights.size(); i++)
		sgScoreLight(sgBestLights[i], obj, box, sphere, camerabased);

	dQsort(sgBestLights.address(), sgBestLights.size(), sizeof(LightInfo*), sgSortLightsByScore);

	for(U32 i=0; i<sgBestLights.size(); i++)
	{
		if((sgBestLights[i]->mScore > 0) && (i < maxlights))
			continue;

		sgBestLights.setSize(i);
		break;
	}
}

void LightManager::sgScoreLight(LightInfo *light, SceneObject *obj, const Box3F &box, const SphereF &sphere, bool camerabased)
{
	if(sgFilterZones && light->sgDiffuseRestrictZone)
	{
		bool allowdiffuse = false;
		if(sgZones[0] > -1)
		{
			if(light->sgAllowDiffuseZoneLighting(sgZones[0]))
				allowdiffuse = true;
			else if(sgZones[1] > -1)
			{
				if(light->sgAllowDiffuseZoneLighting(sgZones[1]))
					allowdiffuse = true;
			}
		}

		if(!allowdiffuse)
		{
			light->mScore = 0;
			return;
		}
	}


	F32 distintensity = 1.0f;
	F32 colorintensity = 1.0f;
	F32 weight = SG_LIGHTMANAGER_DYNAMIC_PRIORITY;

   light->sgDTSLightingOcclusionAdjust = 1.0f;

	if(camerabased)
	{
		sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(light->sgLightingModelName);
		model.sgSetState(light);
		F32 maxrad = model.sgGetMaxRadius(true);
		model.sgResetState();

		Point3F vect = sphere.center - light->mPos;
		F32 dist = vect.len();
		F32 distlightview = sphere.radius + maxrad;

		if(distlightview <= 0.0f)
			distintensity = 0.0f;
		else
		{
			distintensity = 1.0f - (dist / distlightview);
			distintensity = mClampF(distintensity, 0.0f, 1.0f);
		}
	}
	else
	{
		// side test...
		if((light->mType == LightInfo::Spot) || (light->mType == LightInfo::SGStaticSpot))
		{
			bool anyfront = false;
			F32 x, y, z;

			for(U32 i=0; i<8; i++)
			{
				if(i & 0x1)
					x = box.max.x;
				else
					x = box.min.x;

				if(i & 0x2)
					y = box.max.y;
				else
					y = box.min.y;

				if(i & 0x4)
					z = box.max.z;
				else
					z = box.min.z;

				if(light->sgSpotPlane.whichSide(Point3F(x, y, z)) == PlaneF::Back)
					continue;

				anyfront = true;
				break;
			}

			if(!anyfront)
			{
				light->mScore = 0;
				return;
			}
		}

		if((light->mType == LightInfo::Vector) || (light->mType == LightInfo::Ambient))
		{
			colorintensity =
				(light->mColor.red   + light->mAmbient.red) * 0.346f +
				(light->mColor.green + light->mAmbient.green) * 0.588f + 
				(light->mColor.blue  + light->mAmbient.blue) * 0.070f;
			distintensity = 1;
			weight = SG_LIGHTMANAGER_SUN_PRIORITY;
		}
		else
		{
			if(light->sgAssignedToParticleSystem)
			{
				colorintensity = SG_PARTICLESYSTEMLIGHT_FIXED_INTENSITY;
			}
			else
			{
				colorintensity =
					(light->mColor.red * 0.3333f) +
					(light->mColor.green * 0.3333f) +
					(light->mColor.blue * 0.3333f);
			}

			sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(light->sgLightingModelName);
			model.sgSetState(light);
			distintensity = model.sgScoreLight(light, sphere);
			model.sgResetState();

			if(light->sgAssignedToTSObject)
				weight = SG_LIGHTMANAGER_ASSIGNED_PRIORITY;
			else if((light->mType == LightInfo::SGStaticPoint) || (light->mType == LightInfo::SGStaticSpot))
				weight = SG_LIGHTMANAGER_STATIC_PRIORITY;


         if(((light->mType == LightInfo::SGStaticPoint) || (light->mType == LightInfo::SGStaticSpot)) &&
            light->sgCastsShadows && (distintensity > 0.0f) && (sgDynamicLightingOcclusionQuality < dlqtNoShadows) &&
            obj->useLightingOcclusion)
         {
            sgDTSDynamicLightingCache::sgCacheEntry *entry = sgDTSDynamicLightingCache::sgFindCacheEntry((void *)obj, light);
            if((!entry->info.sgAlreadyProcessed) ||
               (entry->info.sgLastObjectMoveSnapshotId != obj->moveSnapshotId) ||
               (entry->info.sgLastLightMoveSnapshotId != light->sgMoveSnapshotId))
            {
               entry->info.sgAlreadyProcessed = true;
               entry->info.sgLastObjectMoveSnapshotId = obj->moveSnapshotId;
               entry->info.sgLastLightMoveSnapshotId = light->sgMoveSnapshotId;
               
               U32 shadowcalcs = 0;
               F32 shadowamount = 0.0f;
               F32 x, y, z;
               RayInfo info;

               obj->disableCollision();

               U32 samples = (sgDynamicLightingOcclusionQuality < dlqtCenterSample) ? 7 : 1;
               for(U32 a=0; a<samples; a++)
               {
                  Point3F pos = sphere.center;
                  switch(a)
                  {
                  //case 0:
                  //   pos = pos;
                  //   break;
                  case 1:
                     pos.x = box.max.x;
                     break;
                  case 2:
                     pos.x = box.min.x;
                     break;
                  case 3:
                     pos.y = box.max.y;
                     break;
                  case 4:
                     pos.y = box.min.y;
                     break;
                  case 5:
                     pos.z = box.max.z;
                     break;
                  case 6:
                     pos.z = box.min.z;
                     break;
                  };

                  Point3F planenorm = pos - sphere.center;
                  Point3F lightdir = sphere.center - light->mPos;

                  if((sgDynamicLightingOcclusionQuality > dlqtSevenSample) && (mDot(planenorm, lightdir) > 0.0f))
                     continue;

                  shadowcalcs++;
                  if(gClientContainer.castRay(light->mPos, pos, ShadowCasterObjectType, &info))
                     shadowamount += 1.0f;
               }

               obj->enableCollision();

               entry->info.sgCachedLightingMultiplier = 1.0f - (shadowamount / ((F32)shadowcalcs));
            }

            distintensity *= entry->info.sgCachedLightingMultiplier;
            light->sgDTSLightingOcclusionAdjust = entry->info.sgCachedLightingMultiplier;
         }
		}
	}

	F32 intensity = colorintensity * distintensity;
	if(intensity < SG_MIN_LEXEL_INTENSITY)
		intensity = 0.0f;
	light->mScore = S32(intensity * weight * 1024.0f);
}

void LightManager::sgInit()
{
	for(U32 i=0; i<sgPropertyCount; i++)
		sgLightingProperties[i] = false;

	Con::addVariable("$pref::LightManager::sgDynamicParticleSystemLighting", TypeBool, &sgDynamicParticleSystemLighting);
	Con::addVariable("$pref::LightManager::sgBlendedTerrainDynamicLighting", TypeBool, &sgBlendedTerrainDynamicLighting);
	Con::addVariable("$pref::LightManager::sgMaxBestLights", TypeS32, &sgMaxBestLights);
	Con::addVariable("$pref::LightManager::sgLightingProfileQuality", TypeS32, &sgLightingProfileQuality);
	Con::addVariable("$pref::LightManager::sgLightingProfileAllowShadows", TypeBool, &sgLightingProfileAllowShadows);

	Con::addVariable("$pref::LightManager::sgUseDynamicShadows", TypeBool, &sgUseDynamicShadows);

   Con::addVariable("$pref::LightManager::sgDynamicLightingOcclusionQuality", TypeS32, &sgDynamicLightingOcclusionQuality);
	Con::addVariable("$pref::LightManager::sgDynamicShadowQuality", TypeS32, &sgDynamicShadowQuality);
	Con::addVariable("$pref::LightManager::sgMultipleDynamicShadows", TypeBool, &sgMultipleDynamicShadows);

	Con::addVariable("$LightManager::sgInGUIEditor", TypeBool, &sgInGUIEditor);

	Con::addVariable("$pref::Interior::sgDetailMaps", TypeBool, &sgDetailMaps);

	sgRelightFilter::sgInit();
}

bool LightManager::sgAllowDetailMaps()
{
	return (sgDetailMaps && (dglGetMaxTextureUnits() >= 4));
}

void LightManager::sgGetFilteredLightColor(ColorF &color, ColorF &ambient, S32 objectzone)
{
	sgMissionLightingFilter *filterbasezone = NULL;
	sgMissionLightingFilter *filtercurrentzone = NULL;
	SimSet *filters = Sim::getsgMissionLightingFilterSet();

	for(SimObject ** itr = filters->begin(); itr != filters->end(); itr++)
	{
		sgMissionLightingFilter *filter = dynamic_cast<sgMissionLightingFilter*>(*itr);
		if(!filter)
			continue;

		S32 zone = filter->getCurrZone(0);
		if(zone == 0)
			filterbasezone = filter;
		if(zone == objectzone)
		{
			filtercurrentzone = filter;
			break;
		}
	}

	if(filtercurrentzone)
		filterbasezone = filtercurrentzone;

	if(!filterbasezone)
		return;

	sgMissionLightingFilterData *datablock = (sgMissionLightingFilterData *)filterbasezone->getDataBlock();

	if(!datablock)
		return;

	ColorF composite = datablock->sgLightingFilter * datablock->sgLightingIntensity;

	color *= composite;
	color.clamp();

	ambient *= composite;
	ambient.clamp();

	if(!datablock->sgCinematicFilter)
		return;

	// must use the lighting filter intensity
	// to lock the reference value relative
	// to the lighting intensity
	composite = datablock->sgCinematicFilterReferenceColor *
		datablock->sgCinematicFilterReferenceIntensity *
		datablock->sgLightingIntensity;

	F32 intensity = color.red + color.green + color.blue + ambient.red + ambient.green + ambient.blue;
	F32 intensityref = composite.red + composite.green + composite.blue;

	intensity -= intensityref;

	// blue is too intense...
	if(intensity > 0.0f)
		intensity *= 0.25f;

	F32 redoffset = 1.0f - ((intensity) * 0.1f * datablock->sgCinematicFilterAmount);
	F32 blueoffset = 1.0f + ((intensity) * 0.1f * datablock->sgCinematicFilterAmount);
	F32 greenoffset = 1.0f - ((1.0f - getMin(redoffset, blueoffset)) * 0.5f);

	ColorF multiplier = ColorF(redoffset, greenoffset, blueoffset);

	color *= multiplier;
	color.clamp();
	ambient *= multiplier;
	ambient.clamp();
}

void LightManager::sgSetupZoneLighting(bool enable, SimObject *sobj)
{
	sgFilterZones = false;
	// these must be -2...
	sgZones[0] = -2;
	sgZones[1] = -2;
	if(!enable)
		return;
	if(dynamic_cast<InteriorInstance *>(sobj))
		return;
	SceneObject *obj = dynamic_cast<SceneObject *>(sobj);
	if(!obj)
		return;
	sgFilterZones = true;
	U32 count = getMin(obj->getNumCurrZones(), U32(2));
	for(U32 i=0; i<count; i++)
	{
		sgZones[i] = obj->getCurrZone(i);
	}
}

void LightManager::sgSetupExposureRendering()
{
	if(!dglDoesSupportARBMultitexture())
		return;

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, SG_LIGHTING_OVERBRIGHT_AMOUNT);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
}

void LightManager::sgResetExposureRendering()
{
	if(!dglDoesSupportARBMultitexture())
		return;

	glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, SG_LIGHTING_NORMAL_AMOUNT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

// adds proper support for self-illumination...
void LightManager::sgSetAmbientSelfIllumination(LightInfo *lightinfo, F32 *lightColor,
		F32 *ambientColor)
{
	//LightInfo *lightInfo = (LightInfo *)lightinfo;
	if(sgAllowCollectSelfIlluminationColor())
	{
		if(lightinfo->mType == LightInfo::Vector)
			sgSelfIlluminationColor = ColorF(lightColor[0], lightColor[1], lightColor[2]);
		else
			sgSelfIlluminationColor = ColorF(ambientColor[0], ambientColor[1], ambientColor[2]);
	}
}

void sgStatistics::sgClear()
{
    sgInteriorLexelCount = 0;
	sgInteriorLexelIlluminationCount = 0;
	sgInteriorLexelIlluminationTime = 0;
    sgInteriorLexelDiffuseCount = 0;
    sgInteriorObjectCount = 0;
    sgInteriorObjectIncludedCount = 0;
    sgInteriorObjectIlluminationCount = 0;
    sgInteriorSurfaceIncludedCount = 0;
    sgInteriorSurfaceIlluminationCount = 0;
    sgInteriorSurfaceIlluminatedCount = 0;
    sgInteriorSurfaceSmoothedCount = 0;
    sgInteriorSurfaceSmoothedLexelCount = 0;
    sgInteriorSurfaceSetupTime = 0;
    sgInteriorSurfaceSetupCount = 0;
    sgInteriorSurfaceMergeTime = 0;
    sgInteriorSurfaceMergeCount = 0;
    sgStaticMeshSurfaceOccluderCount = 0;
   sgStaticMeshBVPTPotentialOccluderCount = 0;
   sgStaticMeshCastRayCount = 0;
    sgTerrainLexelCount = 0;
    sgTerrainLexelTime = 0;
}

void sgStatistics::sgPrint()
{
   Con::printf("");
   Con::printf("  Lighting Pack lighting system stats:");
   Con::printf("    Interior Lexel Count:                        %d", sgInteriorLexelCount);
   Con::printf("    Interior Lexel Illumination Count:           %d", sgInteriorLexelIlluminationCount);
   Con::printf("    Interior Lexel Illumination Time (ms):       %f", F32(sgInteriorLexelIlluminationTime) / getMax(1.0f, F32(sgInteriorLexelIlluminationCount)));
   Con::printf("    Interior Lexel Illumination Time Total (ms): %d", sgInteriorLexelIlluminationTime);
   Con::printf("    Interior Lexel Diffuse Count:                %d", sgInteriorLexelDiffuseCount);
   Con::printf("    Interior Object Count:                       %d", sgInteriorObjectCount);
   Con::printf("    Interior Object Included Count:              %d", sgInteriorObjectIncludedCount);
   Con::printf("    Interior Object Illumination Count:          %d", sgInteriorObjectIlluminationCount);
   Con::printf("    Interior Surface Included Count:             %d", sgInteriorSurfaceIncludedCount);
   Con::printf("    Interior Surface Illumination Count:         %d", sgInteriorSurfaceIlluminationCount);
   Con::printf("    Interior Surface Illuminated Count:          %d", sgInteriorSurfaceIlluminatedCount);
   Con::printf("    Interior Surface Smoothed Count:             %d", sgInteriorSurfaceSmoothedCount);
   Con::printf("    Interior Surface Smoothed Lexel Count:       %d", sgInteriorSurfaceSmoothedLexelCount);
   Con::printf("    Interior Surface Setup Count:                %d", sgInteriorSurfaceSetupCount);
   Con::printf("    Interior Surface Setup Time Total (ms):      %d", sgInteriorSurfaceSetupTime);
   Con::printf("    Interior Surface Merge Count:                %d", sgInteriorSurfaceMergeCount);
   Con::printf("    Interior Surface Merge Time Total (ms):      %d", sgInteriorSurfaceMergeTime);
   Con::printf("    Static Mesh BVPT Potential Occluder Count:   %d", sgStaticMeshBVPTPotentialOccluderCount);
   Con::printf("    Static Mesh Cast Ray Mesh Count:             %d", sgStaticMeshCastRayCount);
   Con::printf("    Static Mesh Surface Occluder Count:          %d", sgStaticMeshSurfaceOccluderCount);
   Con::printf("    Terrain Lexel Count:                         %d", sgTerrainLexelCount);
   Con::printf("    Terrain Lexel Time (ms):                     %f", F32(sgTerrainLexelTime) / getMax(1.0f, F32(sgTerrainLexelCount)));
   Con::printf("    Terrain Lexel Time Total (ms):               %d", sgTerrainLexelTime);
}

S32 QSORT_CALLBACK LightManager::sgSortLightsByAddress(const void* a, const void* b)
{
	return ((*(LightInfo**)b) - (*(LightInfo**)a));
}

S32 QSORT_CALLBACK LightManager::sgSortLightsByScore(const void* a, const void* b)
{
	return((*(LightInfo**)b)->mScore - (*(LightInfo**)a)->mScore);
}

void sgRelightFilter::sgInit()
{
	Con::addVariable("SceneLighting::sgFilterRelight", TypeBool, &sgRelightFilter::sgFilterRelight);
	Con::addVariable("SceneLighting::sgFilterRelightVisible", TypeBool, &sgRelightFilter::sgFilterRelightVisible);
	Con::addVariable("SceneLighting::sgFilterRelightByDistance", TypeBool, &sgRelightFilter::sgFilterRelightByDistance);
	Con::addVariable("SceneLighting::sgFilterRelightByDistanceRadius", TypeF32, &sgRelightFilter::sgFilterRelightByDistanceRadius);
	Con::addVariable("SceneLighting::sgFilterRelightByDistancePosition", TypePoint3F, &sgRelightFilter::sgFilterRelightByDistancePosition);
}

bool sgRelightFilter::sgAllowLighting(const Box3F &box, bool forcefilter)
{
	if((sgRelightFilter::sgFilterRelight && sgRelightFilter::sgFilterRelightByDistance) || forcefilter)
	{
		if(!sgRelightFilter::sgFilterRelightVisible)
			return false;

		Point3F   min = EditTSCtrl::smCamPos;
		min.x = min.x - sgRelightFilter::sgFilterRelightByDistanceRadius;
		min.y = min.y - sgRelightFilter::sgFilterRelightByDistanceRadius;
		min.z = min.z - sgRelightFilter::sgFilterRelightByDistanceRadius;

		Point3F   max = EditTSCtrl::smCamPos;
		max.x = max.x + sgRelightFilter::sgFilterRelightByDistanceRadius;
		max.y = max.y + sgRelightFilter::sgFilterRelightByDistanceRadius;
		max.z = max.z + sgRelightFilter::sgFilterRelightByDistanceRadius;

		Box3F lightbox(min, max);

		if(!box.isOverlapped(lightbox))
			return false;
	}

	return true;
}

void sgRelightFilter::sgRenderAllowedObjects(void *editor)
{
	U32 i;
	WorldEditor *worldeditor = (WorldEditor *)editor;
	Vector<SceneObject *> objects;
	gServerContainer.findObjects(InteriorObjectType, sgFindObjectsCallback, &objects);

    const ColorI color(255, 0, 255);
    
	for(i = 0; i < objects.size(); i++)
	{
		SceneObject * obj = objects[i];
		if(worldeditor->objClassIgnored(obj))
			continue;

		if(!sgRelightFilter::sgAllowLighting(obj->getWorldBox(), true))
			continue;

		worldeditor->renderObjectBox(obj, color);
	}
}

