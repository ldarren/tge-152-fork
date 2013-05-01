//-----------------------------------------------------------------------------
// Torque Game Engine
// 
// Written by Melvyn May, 4th August 2002.
//-----------------------------------------------------------------------------

#ifndef _GRASSREPLICATOR_H_
#define _GRASSREPLICATOR_H_

#ifndef _SCENEOBJECT_H_
#include "sim/sceneObject.h"
#endif

#define AREA_ANIMATION_ARC			(1.0f / 360.0f)

#define FXGRASSREPLICATOR_COLLISION_MASK   (	TerrainObjectType		|	\
												InteriorObjectType		|	\
												StaticObjectType		|	\
												WaterObjectType		)

#define FXGRASSREPLICATOR_NOWATER_COLLISION_MASK   (	TerrainObjectType		|	\
														InteriorObjectType		|	\
														StaticObjectType	)


#define FXGRASS_ALPHA_EPSILON				1e-4



//------------------------------------------------------------------------------
// Class: fxGrassItem
//------------------------------------------------------------------------------
class fxGrassItem
{
public:
	MatrixF		Transform;
	Point3F		Direction;
	F32			Width;
	F32			Height;
	F32			rAngle;
	Box3F		GrassBox;
	bool		Flipped;
	F32			SwayPhase;
	F32			SwayTimeRatio;
	F32			LightPhase;
	F32			LightTimeRatio;
	U32			LastFrameSerialID;
};


//------------------------------------------------------------------------------
// Class: fxGrassCulledList
//------------------------------------------------------------------------------
class fxGrassCulledList
{
public:
	fxGrassCulledList() {};
	fxGrassCulledList(Box3F SearchBox, fxGrassCulledList* InVec);
	~fxGrassCulledList() {};

	void FindCandidates(Box3F SearchBox, fxGrassCulledList* InVec);

	U32 GetListCount(void) { return mCulledObjectSet.size(); };
	fxGrassItem* GetElement(U32 index) { return mCulledObjectSet[index]; };

	Vector<fxGrassItem*>	mCulledObjectSet;		// Culled Object Set.
};


//------------------------------------------------------------------------------
// Class: fxGrassQuadNode
//------------------------------------------------------------------------------
class fxGrassQuadrantNode
{
public:
	U32						Level;
	Box3F					QuadrantBox;
	fxGrassQuadrantNode*	QuadrantChildNode[4];
	Vector<fxGrassItem*>	RenderList;

	fxGrassQuadrantNode()
	{
	}
};


//------------------------------------------------------------------------------
// Class: fxGrassRenderList
//------------------------------------------------------------------------------
class fxGrassRenderList
{
public:
	Point3F					FarPosLeftUp;		// View Frustum.
	Point3F					FarPosLeftDown; 
	Point3F					FarPosRightUp;  
	Point3F					FarPosRightDown;
	Point3F					CameraPosition;		// Camera Position.
	Box3F					mBox;				// Clipping Box.
	PlaneF					ViewPlanes[5];		// Clipping View-Planes.

	Vector<fxGrassItem*>	mVisObjectSet;		// Visible Object Set.
	F32						mHeightLerp;		// Height Lerp.

public:
	bool IsQuadrantVisible(const Box3F VisBox, const MatrixF& RenderTransform);
	void SetupClipPlanes(SceneState* state, const F32 FarClipPlane);
	void CompileVisibleSet(const fxGrassQuadrantNode* pNode, const MatrixF& RenderTransform, const bool UseDebug);
	void DrawQuadBox(const Box3F& QuadBox, const ColorF Colour);
};

//------------------------------------------------------------------------------
// Class: fxGrassReplicator
//------------------------------------------------------------------------------
class fxGrassReplicator : public SceneObject
{
private:
	typedef SceneObject		Parent;

protected:

	void CreateGrass(void);
	void DestroyGrass(void);

	void SyncGrassReplicators(void);

	Box3F FetchQuadrant(Box3F Box, U32 Quadrant);
	void ProcessQuadrant(fxGrassQuadrantNode* pParentNode, fxGrassCulledList* pCullList, U32 Quadrant);
	void ProcessNodeChildren(fxGrassQuadrantNode* pParentNode, fxGrassCulledList* pCullList);

	enum {	GrassReplicationMask	= (1 << 0) };


	U32								mCreationAreaAngle;
	bool							mClientReplicationStarted;
	bool							mAddedToScene;
	U32								mCurrentGrassCount;

	Vector<fxGrassQuadrantNode*>	mGrassQuadTree;
	Vector<fxGrassItem*>			mReplicatedGrass;
	fxGrassRenderList				mFrustumRenderSet;

	MRandomLCG						RandomGen;
	F32								mFadeInGradient;
	F32								mFadeOutGradient;
	S32								mLastRenderTime;
	F32								mGlobalSwayPhase;
	F32								mGlobalSwayTimeRatio;
	F32								mGlobalLightPhase;
	F32								mGlobalLightTimeRatio;
	U32								mFrameSerialID;

	U32								mQuadTreeLevels;				// Quad-Tree Levels.
	U32								mPotentialGrassNodes;			// Potential Grass Nodes.
	U32								mNextAllocatedNodeIdx;			// Next Allocated Node Index.
	U32								mBillboardsAcquired;			// Billboards Acquired.

public:
	fxGrassReplicator();
	~fxGrassReplicator();

	void StartUp(void);
	void ShowReplication(void);
	void HideReplication(void);

	// SceneObject
	void renderObject(SceneState*, SceneRenderImage*);
	virtual bool prepRenderImage(SceneState*, const U32 stateKey, const U32 startZone,
								const bool modifyBaseZoneState = false);


   bool GetTerrainTextures(float,class Point3F &,unsigned char *);

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

	// Field Data.
	struct tagFieldData
	{
		bool				mUseDebugInfo;
		F32					mDebugBoxHeight;
		U32					mSeed;
		StringTableEntry	mGrassFile;
		TextureHandle		mGrassTexture;
		U32					mGrassCount;
		U32					mGrassRetries;

		U32					mInnerRadiusX;
		U32					mInnerRadiusY;
		U32					mOuterRadiusX;
		U32					mOuterRadiusY;

		F32					mMinWidth;
		F32					mMaxWidth;
		F32					mMinHeight;
		F32					mMaxHeight;
		bool				mFixAspectRatio;
		bool				mFixSizeToMax;
		F32					mOffsetZ;
		bool				mRandomFlip;

		bool				mUseCulling;
		U32					mCullResolution;
		F32					mViewDistance;
		F32					mViewClosest;
		F32					mFadeInRegion;
		F32					mFadeOutRegion;
		F32					mAlphaCutoff;
		F32					mGroundAlpha;

		bool				mSwayOn;
		bool				mSwaySync;
		F32					mSwayMagnitudeSide;
		F32					mSwayMagnitudeFront;
		F32					mMinSwayTime;
		F32					mMaxSwayTime;

		bool				mLightOn;
		bool				mLightSync;
		F32					mMinLuminance;
		F32					mMaxLuminance;
		F32					mLightTime;

		bool				mAllowOnTerrain;
		bool				mAllowOnInteriors;
		bool				mAllowStatics;
		bool				mAllowOnWater;
		bool				mAllowWaterSurface;
		S32					mAllowedTerrainSlope;

		bool				mHideGrass;
		bool				mShowPlacementArea;
		U32					mPlacementBandHeight;
		ColorF				mPlaceAreaColour;
		ColorF				mFoilageColourTop;
		ColorF				mFoilageColourBtm;
		bool				mUseColour;
		bool				mIsRandom;
		bool				mIsSquare;
		F32					mRotateAngle;
      F32            mSurfaceExclusionMode;
      U32            mTheSurface;

		tagFieldData::tagFieldData()
		{
			// Set Defaults.
			mUseDebugInfo			= false;
			mDebugBoxHeight			= 1.0f;
			mSeed					= 1376312589;
			mGrassFile				= StringTable->insert("");
			mGrassTexture			= TextureHandle();
			mGrassCount				= 1000;
			mGrassRetries			= 100;

			mInnerRadiusX			= 0;
			mInnerRadiusY			= 0;
			mOuterRadiusX			= 50;
			mOuterRadiusY			= 50;

			mMinWidth				= 4;
			mMaxWidth				= 6;
			mMinHeight				= 1;
			mMaxHeight				= 1.5;
			mFixAspectRatio			= false;
			mFixSizeToMax			= false;
			mOffsetZ				= 0;
			mRandomFlip				= false;

			mUseCulling				= true;
			mCullResolution		= 32.0f;
			mViewDistance			= 70.0f;
			mViewClosest			= 1.0f;
			mFadeInRegion			= 20.0f;
			mFadeOutRegion			= 1.0f;
			mAlphaCutoff			= 0.5f;
			mGroundAlpha			= 0.9f;

			mSwayOn					= true;
			mSwaySync				= false;
			mSwayMagnitudeSide		= 0.1f;
			mSwayMagnitudeFront		= 0.2f;
			mMinSwayTime			= 4.0f;
			mMaxSwayTime			= 15.0f;

			mLightOn				= true;
			mLightSync				= false;
			mMinLuminance			= 0.7f;
			mMaxLuminance			= 1.0f;
			mLightTime				= 10.0f;

			mAllowOnTerrain			= true;
			mAllowOnInteriors		= false;
			mAllowStatics			= false;
			mAllowOnWater			= false;
			mAllowWaterSurface		= false;
			mAllowedTerrainSlope	= 40;

			mHideGrass				= false;
			mShowPlacementArea		= false;
			mPlacementBandHeight	= 15;
			mPlaceAreaColour		.set(0.4f, 0, 0.8f);
			mFoilageColourTop		.set(0.3f, 0.8f, 0.1f);
			mFoilageColourBtm		.set(0.2f, 0.6f, 0.0f);
			mUseColour				= true;
			mIsRandom				= true;
			mIsSquare				= true;
			mRotateAngle			= 0.0f;
		}

	} mFieldData;

	// Declare Console Object.
	DECLARE_CONOBJECT(fxGrassReplicator);
};

#endif // _GRASSREPLICATOR_H_
