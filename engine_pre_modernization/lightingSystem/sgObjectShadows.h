//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#ifndef _SGOBJECTSHADOWS_H_
#define _SGOBJECTSHADOWS_H_

#include "game/shadow.h"
#include "lightingSystem/sgHashMap.h"


typedef hash_multimap<U32, Shadow *> sgShadowMultimap;


class sgObjectShadows
{
private:
	enum
	{
		sgosFirstEntryHash = 0
	};

	bool sgRegistered;
	U32 sgLastRenderTime;

	//bool sgEnable;
	//bool sgCanMove;
	//bool sgCanRTT;
	//bool sgCanSelfShadow;
	//U32 sgRequestedShadowSize;
	//U32 sgFrameSkip;
	//F32 sgMaxVisibleDistance;
	//F32 sgProjectionDistance;
	//F32 sgSphereAdjust;
	sgShadowMultimap sgShadows;

	// for non-multi-shadows...
	LightInfo sgSingleShadowSource;

	// returns an empty entry (use linkHigh to traverse the list)...
	// generally it's a good idea to expect some entries to be empty...
	sgShadowMultimap *sgGetFirstShadowEntry();
	Shadow *sgFindShadow(SceneObject *parentobject,
		LightInfo *light, TSShapeInstance *shapeinstance);
	//void sgUpdateShadow(sgShadowProjector *shadow);
	//void sgUpdateShadows();
	void sgClearMap();
	U32 sgLightToHash(LightInfo *light) {return U32(light);}
	LightInfo *sgHashToLight(U32 hash) {return (LightInfo *)hash;}

public:
	static F32 sgCurrentFade;

	sgObjectShadows();
	~sgObjectShadows();
	void sgRender(SceneObject *parentobject, TSShapeInstance *shapeinstance,
		F32 dist, F32 fogamount, F32 genericshadowlevel,
		F32 noshadowlevel, U32 shadownode, bool move, bool animate);
	const Point3F sgGetShadowLightDirection(SceneObject *obj, Shadow *shadow) const;
	void sgCleanupUnused(U32 time);
	//void sgSetValues(bool enable, bool canmove, bool canrtt,
	//	bool selfshadow, U32 shadowsize, U32 frameskip,
	//	F32 maxvisibledist, F32 projectiondist, F32 adjust);
};


/**
 * Someone needs to make sure the resource
 * usage doesn't get out of hand...
 */
class sgObjectShadowMonitor
{
private:
	// I should be a map!!!
	static Vector<sgObjectShadows *> sgAllObjectShadows;

public:
	static void sgRegister(sgObjectShadows *shadows);
	static void sgUnregister(sgObjectShadows *shadows);
	static void sgCleanupUnused();
};


#endif//_SGOBJECTSHADOWS_H_
