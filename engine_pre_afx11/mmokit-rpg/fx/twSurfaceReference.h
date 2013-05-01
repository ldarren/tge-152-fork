//-----------------------------------------------------------------------------
// TubettiWorld Enhancement
// Surface Reference Object
//
// for the TGE
//
// by Kenneth C. Finney  copyright 2002
// portions written by Melvyn May, 4th August 2002.
//-----------------------------------------------------------------------------

#ifndef _SURFACEREFERENCE_H_
#define _SURFACEREFERENCE_H_

#ifndef _SCENEOBJECT_H_
#include "sim/sceneObject.h"
#endif

#define AREA_ANIMATION_ARC			(1.0f / 360.0f)

#define NUM_SURFACES 16
struct SurfaceStruct
{
   bool isAvailable;
   U8   *alphas;
};

extern struct SurfaceStruct BaseSurfaces[];
extern EnumTable gSurfaceTypeTable;

//------------------------------------------------------------------------------
// Class: twSurfaceReference
//------------------------------------------------------------------------------
class twSurfaceReference : public SceneObject
{
private:
	typedef SceneObject		Parent;

protected:

	void GetSurface(void);

	void SyncSurfaceReferencers(void);

	enum {	SurfaceReferenceMask	= (1 << 0) };


	U32								mCreationAreaAngle;
	bool							mClientReferenceStarted;
	bool							mAddedToScene;

	MRandomLCG						RandomGen;
	S32								mLastRenderTime;
	U32								mFrameSerialID;

   bool getTerrainTextures(float distance, Point3F &pt, U8 *alphas);

public:
	twSurfaceReference();
	~twSurfaceReference();

	void StartUp(void);

	// SceneObject
	void renderObject(SceneState*, SceneRenderImage*);
	virtual bool prepRenderImage(SceneState*, const U32 stateKey, const U32 startZone,
								const bool modifyBaseZoneState = false);

	// SimObject
	bool onAdd();
	void onRemove();
	void onEditorEnable();
	void onEditorDisable();
	void inspectPostApply();

	// NetObject
	U32 packUpdate(NetConnection *, U32, BitStream *);
	void unpackUpdate(NetConnection *, BitStream *);

	// ConObject.
	static void initPersistFields();

   enum ESurfaceType
   {
     eAny     = 0,
     eBare    = 1,
     eDirt    = 2,
     eGravel  = 3,
     eStone   = 4,
     eRock    = 5,
     ePath    = 6,
     eTrail   = 7,
     eRoad    = 8,
     eGrass   = 9,
     eSand    = 10,
     ePGrass  = 11,
     ePSand   = 12,
     eDGrass  = 13,
     eSnow    = 14,
     ePSnow   = 15
   };

	// Field Data.
	struct tagFieldData
	{

      U32      mTheSurfaceRef;

		bool				mShowPlacementArea;
		U32					mPlacementBandHeight;
		ColorF				mPlaceAreaColour;

		tagFieldData::tagFieldData()
		{
         mTheSurfaceRef         = (U32)eAny;
			mShowPlacementArea		= true;
			mPlacementBandHeight	= 25;
			mPlaceAreaColour		.set(0.4f, 0, 0.8f);
		}

	} mFieldData;

	// Declare Console Object.
	DECLARE_CONOBJECT(twSurfaceReference);
};

#endif // _SURFACEREFERENCE_H_
