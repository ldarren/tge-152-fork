//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright © Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------
#ifndef _SGSCENELIGHTING_H_
#define _SGSCENELIGHTING_H_

#ifndef _SCENEOBJECT_H_
#include "sim/sceneObject.h"
#endif
#ifndef _TERRDATA_H_
#include "terrain/terrData.h"
#endif
#ifndef _INTERIORINSTANCE_H_
#include "interior/interiorInstance.h"
#endif
#ifndef _SHADOWVOLUMEBSP_H_
#include "sceneGraph/shadowVolumeBSP.h"
#endif

#include "math/mathUtils.h"
#include "lightingSystem/sgLightManager.h"
#include "lightingSystem/sgScenePersist.h"
#include "lightingSystem/sgLightMap.h"


class SceneLighting : public SimObject
{
	typedef SimObject Parent;
public:
	S32 sgTimeTemp;
	S32 sgTimeTemp2;
	void sgNewEvent(U32 light, S32 object, U32 event);

	void sgLightingStartEvent();
	void sgLightingCompleteEvent();

	void sgTGEPassSetupEvent();
	void sgTGELightStartEvent(U32 light);
	void sgTGELightProcessEvent(U32 light, S32 object);
	void sgTGELightCompleteEvent(U32 light);
	void sgTGESetProgress(U32 light, S32 object);

	void sgSGPassSetupEvent();
	void sgSGObjectStartEvent(S32 object);
	void sgSGObjectProcessEvent(U32 light, S32 object);
	void sgSGObjectCompleteEvent(S32 object);
	void sgSGSetProgress(U32 light, S32 object);

	bool sgIsCorrectStaticObjectType(SceneObject *obj);

	// 'sg' prefix omitted to conform with existing 'addInterior' method...
	void addStatic(void *terrainproxy, ShadowVolumeBSP *shadowVolume,
		SceneObject *sceneobject, LightInfo *light, S32 level);


	// persist objects moved to 'sgScenePersist.h' for clarity...
	// everything below this line should be original code...

	U32 calcMissionCRC();

	bool verifyMissionInfo(PersistInfo::PersistChunk *);
	bool getMissionInfo(PersistInfo::PersistChunk *);

	bool loadPersistInfo(const char *);
	bool savePersistInfo(const char *);

	class ObjectProxy;
	class TerrainProxy;
	class InteriorProxy;

	enum {
		SHADOW_DETAIL = -1
	};

	void addInterior(ShadowVolumeBSP *, InteriorProxy &, LightInfo *, S32);
	ShadowVolumeBSP::SVPoly * buildInteriorPoly(ShadowVolumeBSP *, InteriorProxy &, Interior *, U32, LightInfo *, bool);

	//------------------------------------------------------------------------------
	/// Create a proxy for each object to store data.
	class ObjectProxy
	{
	public:
		SimObjectPtr<SceneObject>     mObj;
		U32                           mChunkCRC;

		ObjectProxy(SceneObject * obj) : mObj(obj){mChunkCRC = 0;}
		virtual ~ObjectProxy(){}
		SceneObject * operator->() {return(mObj);}
		SceneObject * getObject() {return(mObj);}

		/// @name Lighting Interface
		/// @{
		virtual bool loadResources() {return(true);}
		virtual void init() {}
		virtual bool preLight(LightInfo *) {return(false);}
		virtual void light(LightInfo *) {}
		virtual void postLight(bool lastLight) {}
		/// @}

		/// @name Persistence
		///
		/// We cache lighting information to cut down on load times.
		///
		/// There are flags such as ForceAlways and LoadOnly which allow you
		/// to control this behaviour.
		/// @{
		bool calcValidation();
		bool isValidChunk(PersistInfo::PersistChunk *);

		virtual U32 getResourceCRC() = 0;
		virtual bool setPersistInfo(PersistInfo::PersistChunk *);
		virtual bool getPersistInfo(PersistInfo::PersistChunk *);
		/// @}
	};

	class InteriorProxy : public ObjectProxy
	{
	private:
		typedef  ObjectProxy       Parent;
		bool isShadowedBy(InteriorProxy *);

	public:

		InteriorProxy(SceneObject * obj);
		~InteriorProxy();
		InteriorInstance * operator->() {return(static_cast<InteriorInstance*>(static_cast<SceneObject*>(mObj)));}
		InteriorInstance * getObject() {return(static_cast<InteriorInstance*>(static_cast<SceneObject*>(mObj)));}

		// current light info
		ShadowVolumeBSP *                   mBoxShadowBSP;
		Vector<ShadowVolumeBSP::SVPoly*>    mLitBoxSurfaces;
		Vector<PlaneF>                      mOppositeBoxPlanes;
		Vector<PlaneF>                      mTerrainTestPlanes;


		struct sgSurfaceInfo
		{
			Interior *sgDetail;
         S32 sgSurfaceIndex;// SG_NULL_SURFACE == static mesh...
         ConstructorSimpleMesh *sgStaticMesh;
         PlaneF sgSurfacePlane;
         bool sgSurfaceOutsideVisible;
         Vector<sgPlanarLightMap::sgSmoothingVert> sgTriStrip;
         Point3D sgWorldPos;
         Point3D sgSVector;
         Point3D sgTVector;
         Point2I sgLightMapExtent;
         Point2I sgLightMapOffset;
         U32 sgLightMapIndex;
		};
		U32 sgCurrentSurfaceIndex;
		U32 sgSurfacesPerPass;
		InteriorInstance *sgInterior;
		Vector<LightInfo *> sgLights;
		Vector<sgSurfaceInfo *> sgSurfaces;

      void sgClearSurfaces()
      {
         for(U32 s=0; s<sgSurfaces.size(); s++)
         {
            if(sgSurfaces[s])
               delete sgSurfaces[s];
         }
         sgSurfaces.clear();
      }

		void sgAddLight(LightInfo *light, InteriorInstance *interior);
		//void sgLightUniversalPoint(LightInfo *light);
		void sgProcessSurface(sgSurfaceInfo &surfaceinfo);
      void sgConvertStaticMeshPrimitiveToSurfaceInfo(const ConstructorSimpleMesh *staticmesh, U32 primitiveindex, Interior *detail, sgSurfaceInfo &surfaceinfo);
      void sgConvertInteriorSurfaceToSurfaceInfo(const Interior::Surface &surface, U32 i, Interior *detail, sgSurfaceInfo &surfaceinfo);
      void sgReorganizeSurface(sgSurfaceInfo &surfaceinfo);
      void sgExtractLightingInformation(const Interior *detail, const PlaneF &lmEqX, const PlaneF &lmEqY,
         const PlaneF &surfplane, const Point2I &lmoff, const Point2I &lmext, const Point2I &lmsheetsize,
         Point3D &worldpos, Point3D &vectS, Point3D &vectT, Point2I &lmoffactual, Point2I &lmextactual);


		// lighting interface
		bool loadResources();
		void init();
		bool preLight(LightInfo *);
		void light(LightInfo *);
		void postLight(bool lastLight);

		// persist
		U32 getResourceCRC();
		bool setPersistInfo(PersistInfo::PersistChunk *);
		bool getPersistInfo(PersistInfo::PersistChunk *);
	};

	class TerrainProxy : public ObjectProxy
	{
	private:
		typedef  ObjectProxy    Parent;

		BitVector               mShadowMask;
		ShadowVolumeBSP *       mShadowVolume;
		ColorF *                mLightmap;


		ColorF *sgBakedLightmap;
		Vector<LightInfo *> sgLights;
		void sgAddUniversalPoint(LightInfo *light);
		void sgLightUniversalPoint(LightInfo *light, TerrainBlock * terrain);
		bool sgMarkStaticShadow(void *terrainproxy, SceneObject *sceneobject, LightInfo *light);
		void postLight(bool lastLight);


		void lightVector(LightInfo *);

		struct SquareStackNode
		{
			U8          mLevel;
			U16         mClipFlags;
			Point2I     mPos;
		};

		S32 testSquare(const Point3F &, const Point3F &, S32, F32, const Vector<PlaneF> &);
		bool markInteriorShadow(InteriorProxy *);

	public:

		TerrainProxy(SceneObject * obj);
		~TerrainProxy();
		TerrainBlock * operator->() {return(static_cast<TerrainBlock*>(static_cast<SceneObject*>(mObj)));}
		TerrainBlock * getObject() {return(static_cast<TerrainBlock*>(static_cast<SceneObject*>(mObj)));}

		bool getShadowedSquares(const Vector<PlaneF> &, Vector<U16> &);

		// lighting
		void init();
		bool preLight(LightInfo *);
		void light(LightInfo *);

		// persist
		U32 getResourceCRC();
		bool setPersistInfo(PersistInfo::PersistChunk *);
		bool getPersistInfo(PersistInfo::PersistChunk *);
	};

	typedef Vector<ObjectProxy*>  ObjectProxyList;

	ObjectProxyList            mSceneObjects;
	ObjectProxyList            mLitObjects;

	LightInfoList              mLights;

	SceneLighting();
	~SceneLighting();

	enum Flags {
		ForceAlways    = BIT(0),   ///< Regenerate the scene lighting no matter what.
		ForceWritable  = BIT(1),   ///< Regenerate the scene lighting only if we can write to the lighting cache files.
		LoadOnly       = BIT(2),   ///< Just load cached lighting data.
	};
	static bool lightScene(const char *, BitSet32 flags = 0);
	static bool isLighting();

	S32                        mStartTime;
	char                       mFileName[1024];
	static bool                smUseVertexLighting;

	bool light(BitSet32);
	void completed(bool success);
	void processEvent(U32 light, S32 object);
	void processCache();

	// inlined
	bool isAtlas(SceneObject *);
	bool isTerrain(SceneObject *);
	bool isInterior(SceneObject *);
};

class sgSceneLightingProcessEvent : public SimEvent
{
private:
	U32 sgLightIndex;
	S32 sgObjectIndex;
	U32 sgEvent;

public:
	enum sgEventTypes
	{
		sgLightingStartEventType,
		sgLightingCompleteEventType,

		sgSGPassSetupEventType,
		sgSGObjectStartEventType,
		sgSGObjectCompleteEventType,
		sgSGObjectProcessEventType,

		sgTGEPassSetupEventType,
		sgTGELightStartEventType,
		sgTGELightCompleteEventType,
		sgTGELightProcessEventType
	};

	sgSceneLightingProcessEvent(U32 lightIndex, S32 objectIndex, U32 event)
	{
		sgLightIndex = lightIndex;
		sgObjectIndex = objectIndex;
		sgEvent = event;
	}
	void process(SimObject * object)
	{
		AssertFatal(object, "SceneLightingProcessEvent:: null event object!");
		if(!object)
			return;

		SceneLighting *sl = static_cast<SceneLighting*>(object);
		switch(sgEvent)
		{
		case sgLightingStartEventType:
			sl->sgLightingStartEvent();
			break;
		case sgLightingCompleteEventType:
			sl->sgLightingCompleteEvent();
			break;

		case sgTGEPassSetupEventType:
			sl->sgTGEPassSetupEvent();
			break;
		case sgTGELightStartEventType:
			sl->sgTGELightStartEvent(sgLightIndex);
			break;
		case sgTGELightProcessEventType:
			sl->sgTGELightProcessEvent(sgLightIndex, sgObjectIndex);
			break;
		case sgTGELightCompleteEventType:
			sl->sgTGELightCompleteEvent(sgLightIndex);
			break;

		case sgSGPassSetupEventType:
			sl->sgSGPassSetupEvent();
			break;
		case sgSGObjectStartEventType:
			sl->sgSGObjectStartEvent(sgObjectIndex);
			break;
		case sgSGObjectProcessEventType:
			sl->sgSGObjectProcessEvent(sgLightIndex, sgObjectIndex);
			break;
		case sgSGObjectCompleteEventType:
			sl->sgSGObjectCompleteEvent(sgObjectIndex);
			break;

		default:
			return;
		};
	};
};



//------------------------------------------------------------------------------

inline bool SceneLighting::isAtlas(SceneObject * obj)
{
	return obj && ((obj->getTypeMask() & AtlasObjectType) != 0);
}

inline bool SceneLighting::isTerrain(SceneObject * obj)
{
	return obj && ((obj->getTypeMask() & TerrainObjectType) != 0);
}

inline bool SceneLighting::isInterior(SceneObject * obj)
{
	return obj && ((obj->getTypeMask() & InteriorObjectType) != 0);
}


#endif//_SGSCENELIGHTING_H_
