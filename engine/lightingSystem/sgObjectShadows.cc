//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#include "sceneGraph/sceneGraph.h"
#include "math/mathUtils.h"
#include "game/shapeBase.h"
#include "game/vehicles/wheeledVehicle.h"

#include "lightingSystem/sgLighting.h"
#include "lightingSystem/sgLightingModel.h"
#include "lightingSystem/sgObjectShadows.h"

#define SG_UNUSED_TIMEOUT	2000
#define SG_SHADOW_TIMEOUT	6000
// this can be a long time, only used for cleaning
// up texture memory after an intensive scene (TSE only)...
//#define SG_TEXTURE_TIMEOUT	30000

extern SceneGraph* gClientSceneGraph;

Vector<sgObjectShadows *> sgObjectShadowMonitor::sgAllObjectShadows;

F32 sgObjectShadows::sgCurrentFade = 1.0;


bool sgTimeElapsed(U32 currenttime, U32 &lasttime, U32 period)
{
	if(currenttime < lasttime)
	{
		lasttime = 0;
		return false;
	}

	if((currenttime - lasttime) < period)
		return false;

	lasttime = currenttime;
	return true;
}

//-----------------------------------------------

void sgObjectShadowMonitor::sgRegister(sgObjectShadows *shadows)
{
	sgAllObjectShadows.push_back(shadows);
}

void sgObjectShadowMonitor::sgUnregister(sgObjectShadows *shadows)
{
	for(U32 i=0; i<sgAllObjectShadows.size(); i++)
	{
		if(sgAllObjectShadows[i] == shadows)
		{
			sgAllObjectShadows.erase_fast(i);
			return;
		}
	}
}

void sgObjectShadowMonitor::sgCleanupUnused()
{
	static U32 lasttime = 0;

	U32 time = Platform::getRealMilliseconds();
	if(!sgTimeElapsed(time, lasttime, SG_UNUSED_TIMEOUT))
		return;

	//Con::warnf("Checking for unused shadows...");

	for(U32 i=0; i<sgAllObjectShadows.size(); i++)
		sgAllObjectShadows[i]->sgCleanupUnused(time);

	//Con::warnf("Done.");
}

//-----------------------------------------------

sgObjectShadows::sgObjectShadows()
{
	sgRegistered = false;
	sgLastRenderTime = 0;

	sgSingleShadowSource.mColor = ColorF(0.5, 0.5, 0.5);

	//sgEnable = false;
	//sgCanMove = false;
	//sgCanRTT = false;
	//sgCanSelfShadow = false;
	//sgRequestedShadowSize = 64;
	//sgFrameSkip = 5;
	//sgMaxVisibleDistance = 15.0f;
	//sgProjectionDistance = 7.0f;

	//sgFirstEntry = sgGetFirstShadowEntry();
}

sgObjectShadows::~sgObjectShadows()
{
	sgClearMap();
}

void sgObjectShadows::sgClearMap()
{
	sgShadowMultimap *entry = sgGetFirstShadowEntry();
	while(entry)
	{
		if(entry->info)
		{
			delete entry->info;
			entry->info = NULL;
		}

		entry = entry->linkHigh;
	}

	// all shadows are deleted, so nothing left to monitor...
	if(sgRegistered)
	{
		sgObjectShadowMonitor::sgUnregister(this);
		sgRegistered = false;
	}
}

void sgObjectShadows::sgRender(SceneObject *parentobject,
		TSShapeInstance *shapeinstance, F32 dist, F32 fogamount,
		F32 genericshadowlevel, F32 noshadowlevel, U32 shadownode,
		bool move, bool animate)
{
	if(!LightManager::sgAllowDynamicShadows())
		return;

	if(Shadow::getGlobalShadowDetailLevel() < noshadowlevel)
		return;
	if(shapeinstance->getShape()->subShapeFirstTranslucentObject.empty() ||
		shapeinstance->getShape()->subShapeFirstTranslucentObject[0] == 0)
		return;

	// prior to this no shadows exist, so no resources are used...
	if(!sgRegistered)
	{
		sgObjectShadowMonitor::sgRegister(this);
		sgRegistered = true;
	}

	sgLastRenderTime = Platform::getRealMilliseconds();

	ShapeBase *shapebase = dynamic_cast<ShapeBase *>(parentobject);
	WheeledVehicle *wheeledvehicle = dynamic_cast<WheeledVehicle *>(parentobject);

	LightInfoList lights;
	LightManager *lm = gClientSceneGraph->getLightManager();

	lm->sgGetBestLights(lights);

	// env light must be last!
	for(U32 i=0; i<lights.size(); i++)
	{
		LightInfo *light = lights[i];
		if((light->mType != LightInfo::Ambient) &&
		   (light->mType != LightInfo::Vector))
			continue;

		U32 il = lights.size() - 1;
		if(i == il)
			continue;

		// swap...
		LightInfo *last = lights[il];
		lights[il] = light;
		lights[i] = last;
		break;
	}

	F32 shadowLen = 10.0f * shapeinstance->getShape()->radius;
	Point3F pos = shapeinstance->getShape()->center;

	// this is a bit of a hack...move generic shadows towards feet/base of shape
	if(Shadow::getGlobalShadowDetailLevel() < genericshadowlevel)
		pos *= 0.5f;
	S32 shadowNode = shadownode;
	if(shadowNode >= 0)
	{
		// adjust for movement of shape outside of bounding box by tracking this node
		Point3F offset;
		shapeinstance->mNodeTransforms[shadowNode].getColumn(3,&offset);
		offset -= shapeinstance->getShape()->defaultTranslations[shadowNode];
		offset.z = 0.0f;
		pos += offset;
	}
	pos.convolve(parentobject->getScale());
	parentobject->getRenderTransform().mulP(pos);

	U32 usedlights = 0;

	for(U32 i=0; i<lights.size(); i++)
	{
		LightInfo *light = lights[i];

		// we want the env light!
		if((!LightManager::sgMultipleDynamicShadows) &&
		   (light->mType != LightInfo::Vector) &&
		   (light->mType != LightInfo::Ambient))
		    continue;

		// cast shadows?
		if(!light->sgCastsShadows)
			continue;

		// avoid overlapping dynamic and static shadows...
		if(((light->mType == LightInfo::SGStaticPoint) ||
			(light->mType == LightInfo::SGStaticSpot) ||
			(light->mType == LightInfo::Vector)) && (parentobject->getTypeMask() & ShadowCasterObjectType))
			continue;

		// find the shadow...
		Shadow *shadow = sgFindShadow(parentobject, light, shapeinstance);
		AssertFatal((shadow), "Shadow not found?");

		if(Shadow::getGlobalShadowDetailLevel() < genericshadowlevel)
			shadow->setGeneric(true);
		else
			shadow->setGeneric(false);

		shadow->setMoving(move);
		shadow->setAnimating(animate);

		// Get a real light dir...
		// keep this default, it makes the sun shadow darker...
		F32 fade = 3.0;
		Point3F lightDir;

		if(LightManager::sgMultipleDynamicShadows)
		{
			//if(light->mType == LightInfo::Vector)
			//	lightDir = light->mDirection;
			//else if(light->mType == LightInfo::Ambient)
			if((light->mType == LightInfo::Vector) ||
			   (light->mType == LightInfo::Ambient))
			{
				lightDir = sgGetShadowLightDirection(parentobject, shadow);

				if(usedlights > 0)
					fade *= 1.0f / F32(usedlights + 1);
			}
			else
			{
				lightDir = parentobject->getPosition() - light->mPos;
				F32 len = lightDir.len();
				if(len == 0.0f)
					continue;
				lightDir /= len;

				sgLightingModel &model = sgLightingModelManager::sgGetLightingModel(
					light->sgLightingModelName);
				model.sgSetState(light);
				//F32 maxrad = model.sgGetMaxRadius(true, true);
				SphereF s;
				s.center = parentobject->getPosition();
				fade = model.sgScoreLight(light, s);
				model.sgResetState();
			}
		}
		else
		{
			lightDir = sgGetShadowLightDirection(parentobject, shadow);
		}

		// help out dim lights a little...
		if(light->sgAssignedToParticleSystem)
			fade *= SG_PARTICLESYSTEMLIGHT_FIXED_INTENSITY;
		else
			fade *= getMax(light->mColor.red, getMax(light->mColor.green, light->mColor.blue));
		fade *= 1.5f;
		// prevent dark shadows...
		fade = mClampF(fade, 0.0f, 1.0f);
		if(fade <= 0.1f)
			continue;

		usedlights++;
		sgObjectShadows::sgCurrentFade = fade;

		// pos is where shadow will be centered (in world space)
		if(wheeledvehicle)
			pos -= lightDir;
		Point3F scale = parentobject->getScale();
		shadow->setRadius(shapeinstance, scale);
		if(!shadow->prepare(pos, lightDir, shadowLen, scale, dist, fogamount, shapeinstance))
			continue;

		F32 maxscale = getMax(scale.x, getMax(scale.y, scale.z));

		if(shadow->needBitmap())
		{
			shadow->beginRenderToBitmap();
			shadow->selectShapeDetail(shapeinstance, dist, maxscale);
			shadow->renderToBitmap(shapeinstance, parentobject->getRenderTransform(), pos, scale);

			if(shapebase)
			{
				// render mounted items to shadow bitmap
				for(U32 i=0; i<ShapeBase::MaxMountedImages; i++)
				{
					TSShapeInstance *instance = shapebase->getImageShapeInstance(i);
					if(instance)
					{
						MatrixF mat;
						shapebase->getRenderImageTransform(i, &mat);
						shadow->selectShapeDetail(instance, dist, maxscale);
						shadow->renderToBitmap(instance, mat, pos, Point3F(1,1,1));
					}
				}

				// We only render the first mounted object for now...
				ShapeBase *mount = shapebase->getMountedObject(0);
				if(mount && mount->getShapeInstance())
				{
					Point3F linkscale = mount->getScale();
					maxscale = getMax(linkscale.x, getMax(linkscale.y, linkscale.z));
					shadow->selectShapeDetail(mount->getShapeInstance(), dist, maxscale);
					shadow->renderToBitmap(mount->getShapeInstance(),
						mount->getRenderTransform(), pos, linkscale);
				}
			}

			if(wheeledvehicle)
			{
				for(U32 w=0; w<wheeledvehicle->getWheelCount(); w++)
				{
					WheeledVehicle::Wheel *wheel = wheeledvehicle->getWheel(w);

					if(wheel->shapeInstance)
					{
						MatrixF m = parentobject->getRenderTransform();
						MatrixF hub(EulerF(0, 0, wheeledvehicle->getSteering().x * wheel->steering));
						Point3F p = wheel->data->pos;
						p.z -= wheel->spring->length * wheel->extension;
						hub.setColumn(3, p);
						m.mul(hub);

						MatrixF rot(EulerF(wheel->apos * M_2PI,0,0));
						m.mul(rot);

						MatrixF wrot(EulerF(0, 0, (wheel->data->pos.x > 0)? M_PI/2: -M_PI/2));
						m.mul(wrot);

						shadow->selectShapeDetail(wheel->shapeInstance, dist, maxscale);
						shadow->renderToBitmap(wheel->shapeInstance, m, pos, scale);
					}
				}
			}

			shadow->endRenderToBitmap();
		}

		shadow->render();
		shadow->sgLastRenderTime = sgLastRenderTime;

		if(!LightManager::sgMultipleDynamicShadows)
		{
			//only need to render one here...
			break;
		}
	}
}

const Point3F sgObjectShadows::sgGetShadowLightDirection(SceneObject *obj, Shadow *shadow) const
{
	// don't want this to run too fast on newer systems (otherwise shadows snap into place)...
	U32 time = Platform::getRealMilliseconds();

	if((time - shadow->sgPreviousShadowTime) < SG_DYNAMIC_SHADOW_TIME)
		return shadow->sgPreviousShadowLightingVector;

	shadow->sgPreviousShadowTime = time;

	LightManager *lm = gClientSceneGraph->getLightManager();

	// ok get started...
	U32 zone = obj->getCurrZone(0);
	
	VectorF vectcomposite = VectorF(0, 0, -1.0f);
	if(!LightManager::sgMultipleDynamicShadows)
	{
		U32 score;
		U32 maxscore[2] = {0, 0};
		LightInfo *light[2] = {NULL, NULL};
		VectorF vector[2] = {VectorF(0, 0, 0), VectorF(0, 0, 0)};

		LightInfoList lights;
		lm->sgGetBestLights(lights);

		for(U32 i=0; i<lights.size(); i++)
		{
			LightInfo *l = lights[i];

			if((l->mType == LightInfo::Ambient) || (l->mType == LightInfo::Vector))
				score = l->mScore / SG_LIGHTMANAGER_SUN_PRIORITY;
			else if((l->mType == LightInfo::SGStaticPoint) || (l->mType == LightInfo::SGStaticSpot))
				score = l->mScore / SG_LIGHTMANAGER_STATIC_PRIORITY;
			else
				score = l->mScore / SG_LIGHTMANAGER_DYNAMIC_PRIORITY;

			if(score > maxscore[0])
			{
				light[1] = light[0];
				maxscore[1] = maxscore[0];

				light[0] = l;
				maxscore[0] = score;
			}
			else if(score > maxscore[1])
			{
				light[1] = l;
				maxscore[1] = score;
			}
		}

		for(U32 i=0; i<2; i++)
		{
			if(!light[i])
				break;

			if((light[i]->mType == LightInfo::Ambient) || (light[i]->mType == LightInfo::Vector))
			{
				if(zone == 0)
					vector[i] = light[i]->mDirection;
				else
					vector[i] = Point3F(0, 0, -1.0);
			}
			else
			{
				VectorF vect = obj->getPosition() - light[i]->mPos;
				vect.normalize();
				vector[i] = vect;
			}
		}

		if(light[0])
		{
			if(light[1])
			{
				F32 ratio = F32(maxscore[0]) / F32(maxscore[0] + maxscore[1]);
				vectcomposite = (vector[0] * ratio) + (vector[1] * (1.0f - ratio));
			}
			else
				vectcomposite = vector[0];
		}
	}
	else
	{
		if(zone != 0)
			vectcomposite = VectorF(0.0f, 0.0f, -1.0f);
		else
		{
			LightInfo *sun = lm->sgGetSpecialLight(LightManager::sgSunLightType);
			vectcomposite = sun->mDirection;
		}
	}

	VectorF step = (vectcomposite - shadow->sgPreviousShadowLightingVector) / SG_DYNAMIC_SHADOW_STEPS;
	shadow->sgPreviousShadowLightingVector += step;
	shadow->sgPreviousShadowLightingVector.normalize();
	
	return shadow->sgPreviousShadowLightingVector;
}

void sgObjectShadows::sgCleanupUnused(U32 time)
{
	// wrapped around?
	if(time < sgLastRenderTime)
	{
		sgLastRenderTime = 0;
		return;
	}

	// try to ditch the whole thing first...
	if((time - sgLastRenderTime) > SG_SHADOW_TIMEOUT)
	{
		//Con::warnf("Found a whole set...");

		sgClearMap();
		return;
	}

	// no? alright lets try to get rid of some old shadows...
	sgShadowMultimap *entry = sgGetFirstShadowEntry();
	while(entry)
	{
		if(entry->info)
		{
			if(sgTimeElapsed(time, entry->info->sgLastRenderTime, SG_SHADOW_TIMEOUT))
			{
				//Con::warnf("Found one...");

				delete entry->info;
				entry->info = NULL;
			}
		}

		entry = entry->linkHigh;
	}
}

Shadow *sgObjectShadows::sgFindShadow(SceneObject *parentobject,
		LightInfo *light, TSShapeInstance *shapeinstance)
{
	sgShadowMultimap *entry = sgShadows.find(sgLightToHash(light));

	if(entry->info)
		return entry->info;

	Shadow *shadow = new Shadow();
	entry->info = shadow;

	return entry->info;
}

sgShadowMultimap *sgObjectShadows::sgGetFirstShadowEntry()
{
	return sgShadows.find(sgosFirstEntryHash);
}

