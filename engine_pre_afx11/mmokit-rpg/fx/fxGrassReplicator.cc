//-----------------------------------------------------------------------------
// Torque Game Engine
// 
// Written by Melvyn May and Bendik Stang
// in 2002.
//
// "My code is written for the Torque community, so do your worst with it,
//	just don't rip-it-off and call it your own without even thanking me".
//
//	- Melv.
//
// "Thank you Melv!"
// "This code was written for Adellion, and the Torque community, so do your worst with it,
//	just don't rip-it-off and call it your own without even thanking Adellion, Melv and Me".
//
// - Bendik.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "math/mRandom.h"
#include "math/mathIO.h"
#include "terrain/terrData.h"
#include "game/gameConnection.h"
#include "console/simBase.h"
#include "sceneGraph/sceneGraph.h"
// To allow surfacetexture lookup
#include "rpg/fx/twSurfaceReference.h"

#include "fxGrassReplicator.h"

#include "rpg/fx/daynight.h"



//------------------------------------------------------------------------------
//
//	Put the function in /example/common_15/editor/ObjectBuilderGui.gui [around line 458] ...
//
//	function ObjectBuilderGui::buildfxGrassReplicator(%this)
//	{
//		%this.className = "fxGrassReplicator";
//		%this.process();
//	}
//
//------------------------------------------------------------------------------
//
//	Put this in /example/common_15/editor/EditorGui.cs in [function Creator::init( %this )]
//
//   %Environment_Item[8] = "fxGrassReplicator";  <-- ADD THIS.
//  
//   PS: if [8] is taken, give it the next number in the sequence.
//------------------------------------------------------------------------------
//
//	Put this in /example/common_15/client/missionDownload.cs in [function clientCmdMissionStartPhase3(%seq,%missionName)] (line 65)
//	after codeline 'onPhase2Complete();'.
//
//	StartGrassReplication();
//
//------------------------------------------------------------------------------
//
//	Put this in /engine/console/simBase.h (around line 509) in
//
//	namespace Sim
//  {
//	   DeclareNamedSet(fxGrassSet)  <-- ADD THIS (Note no semi-colon).
//
//------------------------------------------------------------------------------
//
//	Put this in /engine/console/simBase.cc (around line 19) in
//
//  ImplementNamedSet(fxGrassSet)  <-- ADD THIS (Note no semi-colon).
//
//------------------------------------------------------------------------------
//
//	Put this in /engine/console/simManager.cc [function void init()] (around line 269).
//
//	namespace Sim
//  {
//		InstantiateNamedSet(fxGrassSet);  <-- ADD THIS (Including Semi-colon).
//
//------------------------------------------------------------------------------
extern bool gEditingMission;

//------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(fxGrassReplicator);


//------------------------------------------------------------------------------
//
// Trig Table Lookups.
//
//------------------------------------------------------------------------------


static bool						mTrigTableInitialised;
static F32						mCosTable[720];
static F32						mSinTable[720];


//--------------------------------------------
// GetTerrainTextures
//   (adapted from original code written by James Holmes)
//
// Get the terrain texture under an x,y coord
//
// Fills an array with the alpha value of each of the
// textures under that point.
//
//   distance - How far to look down for the terrain block
//   pt - Point in world space
//   alphas - U8 array of size NUM_SURFACES
//
bool fxGrassReplicator::GetTerrainTextures(float distance, Point3F& pt, U8 *alphas)
{
   RayInfo rInfo;

   if (gClientContainer.castRay(pt, Point3F(pt.x, pt.y, pt.z - distance ), TerrainObjectType, &rInfo))
   {
      U32 CollisionType = rInfo.object->getTypeMask();

      if (CollisionType & TerrainObjectType)
      {
		   TerrainBlock* ter = static_cast<TerrainBlock*>(rInfo.object);
		   Point2I gPos;
		   const MatrixF & mat = ter->getTransform();
		   Point3F origin;
		   mat.getColumn(3, &origin);
		   F32 squareSize = (F32) ter->getSquareSize();
		   F32 halfSquareSize = squareSize / 2;

		   float x = (pt.x - origin.x + halfSquareSize) / squareSize;
		   float y = (pt.y - origin.y + halfSquareSize) / squareSize;

		   ter->getMaterialAlpha(Point2I(x,y), alphas);
		   return true;
      }
   }
   return false;
}

//------------------------------------------------------------------------------
//
// Class: fxGrassRenderList
//
//------------------------------------------------------------------------------

void fxGrassRenderList::SetupClipPlanes(SceneState* state, const F32 FarClipPlane)
{
	// Fetch Camera Position.
	CameraPosition  = state->getCameraPosition();
	// Calculate Perspective.
	F32 FarOverNear = FarClipPlane / state->getNearPlane();

	// Calculate Clip-Planes.
	FarPosLeftUp    = Point3F(	state->getBaseZoneState().frustum[0] * FarOverNear,
								FarClipPlane,
								state->getBaseZoneState().frustum[3] * FarOverNear);
	FarPosLeftDown  = Point3F(	state->getBaseZoneState().frustum[0] * FarOverNear,
								FarClipPlane,
								state->getBaseZoneState().frustum[2] * FarOverNear);
	FarPosRightUp   = Point3F(	state->getBaseZoneState().frustum[1] * FarOverNear,
								FarClipPlane,
								state->getBaseZoneState().frustum[3] * FarOverNear);
	FarPosRightDown = Point3F(	state->getBaseZoneState().frustum[1] * FarOverNear,
								FarClipPlane,
								state->getBaseZoneState().frustum[2] * FarOverNear);

	// Calculate our World->Object Space Transform.
	MatrixF InvXForm = state->mModelview;
	InvXForm.inverse();
	// Convert to Object-Space.
	InvXForm.mulP(FarPosLeftUp);
	InvXForm.mulP(FarPosLeftDown);
	InvXForm.mulP(FarPosRightUp);
	InvXForm.mulP(FarPosRightDown);

	// Calculate Bounding Box (including Camera).
	mBox.min = CameraPosition;
	mBox.min.setMin(FarPosLeftUp);
	mBox.min.setMin(FarPosLeftDown);
	mBox.min.setMin(FarPosRightUp);
	mBox.min.setMin(FarPosRightDown);
	mBox.max = CameraPosition;
	mBox.max.setMax(FarPosLeftUp);
	mBox.max.setMax(FarPosLeftDown);
	mBox.max.setMax(FarPosRightUp);
	mBox.max.setMax(FarPosRightDown);

	// Setup Our Viewplane.
	ViewPlanes[0].set(CameraPosition,	FarPosLeftUp,		FarPosLeftDown);
	ViewPlanes[1].set(CameraPosition,	FarPosRightUp,		FarPosLeftUp);
	ViewPlanes[2].set(CameraPosition,	FarPosRightDown,	FarPosRightUp);
	ViewPlanes[3].set(CameraPosition,	FarPosLeftDown,		FarPosRightDown);
	ViewPlanes[4].set(FarPosLeftUp,		FarPosRightUp,		FarPosRightDown);
}

//------------------------------------------------------------------------------


inline void fxGrassRenderList::DrawQuadBox(const Box3F& QuadBox, const ColorF Colour)
{
	// Define our debug box.
	static Point3F BoxPnts[] = {
								  Point3F(0,0,0),
								  Point3F(0,0,1),
								  Point3F(0,1,0),
								  Point3F(0,1,1),
								  Point3F(1,0,0),
								  Point3F(1,0,1),
								  Point3F(1,1,0),
								  Point3F(1,1,1)
								};

	static U32 BoxVerts[][4] = {
								  {0,2,3,1},     // -x
								  {7,6,4,5},     // +x
								  {0,1,5,4},     // -y
								  {3,2,6,7},     // +y
								  {0,4,6,2},     // -z
								  {3,7,5,1}      // +z   
								};

	static Point3F BoxNormals[] = {
								  Point3F(-1, 0, 0),
								  Point3F( 1, 0, 0),
								  Point3F( 0,-1, 0),
								  Point3F( 0, 1, 0),
								  Point3F( 0, 0,-1),
								  Point3F( 0, 0, 1)
								};

	// Select the Box Colour.
	glColor4fv(Colour);

	// Project our Box Points.
	Point3F ProjectionPoints[8];
	for(U32 i = 0; i < 8; i++)
	{  
		ProjectionPoints[i].set(BoxPnts[i].x ? QuadBox.max.x : QuadBox.min.x, 
								BoxPnts[i].y ? QuadBox.max.y : QuadBox.min.y,
								BoxPnts[i].z ? (mHeightLerp * QuadBox.max.z) + (1-mHeightLerp) * QuadBox.min.z : QuadBox.min.z);

	}

	// Draw the Box.
	for(U32 x = 0; x < 6; x++)
	{
		// Draw a line-loop.
		glBegin(GL_LINE_LOOP);

		for(U32 y = 0; y < 4; y++)
		{
			// Draw Vertex.
			glVertex3f(	ProjectionPoints[BoxVerts[x][y]].x,
						ProjectionPoints[BoxVerts[x][y]].y,
						ProjectionPoints[BoxVerts[x][y]].z);
		}

		glEnd();
	}
}

//------------------------------------------------------------------------------

void fxGrassRenderList::CompileVisibleSet(const fxGrassQuadrantNode* pNode, const MatrixF& RenderTransform, const bool UseDebug)
{
	// Attempt to trivially reject the Node.
	//
	// Is any of the quadrant visible?
	if (IsQuadrantVisible(pNode->QuadrantBox, RenderTransform))
	{
		// Draw the Quad Box (Debug Only).
		if (UseDebug) DrawQuadBox(pNode->QuadrantBox, ColorF(0,.8,.1,.2));

		// Yes, so are we at sub-level 0?
		if (pNode->Level == 0)
		{
			// Yes, so merge visible object set with node render list.
			mVisObjectSet.merge(pNode->RenderList);
		}
		else
		{
			// No, so compile quadrants.
			for (U32 q = 0; q < 4; q++)
				if (pNode->QuadrantChildNode[q]) CompileVisibleSet(pNode->QuadrantChildNode[q], RenderTransform, UseDebug);
		}
	}
	else
	{
		// Draw the Quad Box (Debug Only).
		if (UseDebug) DrawQuadBox(pNode->QuadrantBox, ColorF(0,.1,8,.2));
	}

	return;
}

//------------------------------------------------------------------------------

bool fxGrassRenderList::IsQuadrantVisible(const Box3F VisBox, const MatrixF& RenderTransform)
{
	// Can we trivially accept the visible box?
	if (mBox.isOverlapped(VisBox))
	{
		// Yes, so calculate Object-Space Box.
		MatrixF InvXForm = RenderTransform;
		InvXForm.inverse();
		Box3F OSBox = VisBox;
		InvXForm.mulP(OSBox.min);
		InvXForm.mulP(OSBox.max);

		// Yes, so fetch Box Center.
		Point3F Center;
		OSBox.getCenter(&Center);

		// Scale.
      //JMR, using radius was giving up false clips (multiplying by .5), not that we
      //are still getting false clips, though maybe good enough
		Point3F XRad(OSBox.len_x() * 1.0, 0, 0);
		Point3F YRad(0, OSBox.len_y() * 1.0, 0);
		Point3F ZRad(0, 0, OSBox.len_z() * 1.0);

		// Render Transformation.
		RenderTransform.mulP(Center);
		RenderTransform.mulV(XRad);
		RenderTransform.mulV(YRad);
		RenderTransform.mulV(ZRad);

		// Check against View-planes.
		for (U32 i = 0; i < 5; i++)
		{
			// Reject if not visible.
			if (ViewPlanes[i].whichSideBox(Center, XRad, YRad, ZRad, Point3F(0, 0, 0)) == PlaneF::Back) return false;
		}

		// Visible.
		return true;
	}

	// Not visible.
	return false;
}



//------------------------------------------------------------------------------
//
// Class: fxGrassCulledList
//
//------------------------------------------------------------------------------
fxGrassCulledList::fxGrassCulledList(Box3F SearchBox, fxGrassCulledList* InVec)
{
	// Find the Candidates.
	FindCandidates(SearchBox, InVec);
}

//------------------------------------------------------------------------------

void fxGrassCulledList::FindCandidates(Box3F SearchBox, fxGrassCulledList* InVec)
{
	// Search the Culled List.
	for (U32 i = 0; i < InVec->GetListCount(); i++)
	{
		// Is this Box overlapping our search box?
		if (SearchBox.isOverlapped(InVec->GetElement(i)->GrassBox))
		{
			// Yes, so add it to our culled list.
			mCulledObjectSet.push_back(InVec->GetElement(i));
		}
	}
}



//------------------------------------------------------------------------------
//
// Class: fxGrassReplicator
//
//------------------------------------------------------------------------------

fxGrassReplicator::fxGrassReplicator()
{
	// Setup NetObject.
	mTypeMask |= StaticObjectType | StaticTSObjectType | StaticRenderedObjectType;
	mAddedToScene = false;
	mNetFlags.set(Ghostable | ScopeAlways);

	// Reset Client Replication Started.
	mClientReplicationStarted = false;

	// Reset Grass Count.
	mCurrentGrassCount = 0;

	// Reset Creation Area Angle Animation.
	mCreationAreaAngle = 0;

	// Reset Last Render Time.
	mLastRenderTime = 0;

	// Reset Grass Nodes.
	mPotentialGrassNodes = 0;
	// Reset Billboards Acquired.
	mBillboardsAcquired = 0;

	// Reset Frame Serial ID.
	mFrameSerialID = 0;
}

//------------------------------------------------------------------------------

fxGrassReplicator::~fxGrassReplicator()
{
}

//------------------------------------------------------------------------------

void fxGrassReplicator::initPersistFields()
{
	// Initialise parents' persistent fields.
	Parent::initPersistFields();

	// Add out own persistent fields.
	//addGroup( "Debugging" );	// MM: Added Group Header.
	addField( "UseDebugInfo",		TypeBool,		Offset( mFieldData.mUseDebugInfo,			fxGrassReplicator ) );	
	addField( "DebugBoxHeight",		TypeF32,		Offset( mFieldData.mDebugBoxHeight,			fxGrassReplicator ) );	
	addField( "HideGrass",		TypeBool,		Offset( mFieldData.mHideGrass,			fxGrassReplicator ) );
	addField( "ShowPlacementArea",	TypeBool,		Offset( mFieldData.mShowPlacementArea,		fxGrassReplicator ) );
	addField( "PlacementAreaHeight",TypeS32,		Offset( mFieldData.mPlacementBandHeight,	fxGrassReplicator ) );
	addField( "PlacementColour",	TypeColorF,		Offset( mFieldData.mPlaceAreaColour,		fxGrassReplicator ) );
	//endGroup( "Debugging" );	// MM: Added Group Footer.

	//addGroup( "Media" );	// MM: Added Group Header.
	addField( "Seed",				TypeS32,		Offset( mFieldData.mSeed,					fxGrassReplicator ) );
	addField( "GrassFile",		TypeFilename,	Offset( mFieldData.mGrassFile,			fxGrassReplicator ) );
	addField( "GrassCount",		TypeS32,		Offset( mFieldData.mGrassCount,			fxGrassReplicator ) );
	addField( "GrassRetries",		TypeS32,		Offset( mFieldData.mGrassRetries,			fxGrassReplicator ) );
	//endGroup( "Media" );	// MM: Added Group Footer.

	//addGroup( "Area" );	// MM: Added Group Header.
	addField( "InnerRadiusX",		TypeS32,		Offset( mFieldData.mInnerRadiusX,			fxGrassReplicator ) );	
	addField( "InnerRadiusY",		TypeS32,		Offset( mFieldData.mInnerRadiusY,			fxGrassReplicator ) );	
	addField( "OuterRadiusX",		TypeS32,		Offset( mFieldData.mOuterRadiusX,			fxGrassReplicator ) );	
	addField( "OuterRadiusY",		TypeS32,		Offset( mFieldData.mOuterRadiusY,			fxGrassReplicator ) );	
	//endGroup( "Area" );	// MM: Added Group Footer.

	//addGroup( "Dimensions" );	// MM: Added Group Header.
	addField( "MinWidth",			TypeF32,		Offset( mFieldData.mMinWidth,				fxGrassReplicator ) );	
	addField( "MaxWidth",			TypeF32,		Offset( mFieldData.mMaxWidth,				fxGrassReplicator ) );	
	addField( "MinHeight",			TypeF32,		Offset( mFieldData.mMinHeight,				fxGrassReplicator ) );	
	addField( "MaxHeight",			TypeF32,		Offset( mFieldData.mMaxHeight,				fxGrassReplicator ) );	
	addField( "FixAspectRatio",		TypeBool,		Offset( mFieldData.mFixAspectRatio,			fxGrassReplicator ) );	
	addField( "FixSizeToMax",		TypeBool,		Offset( mFieldData.mFixSizeToMax,			fxGrassReplicator ) );	
	addField( "OffsetZ",			TypeF32,		Offset( mFieldData.mOffsetZ,				fxGrassReplicator ) );	
	addField( "RandomFlip",			TypeBool,		Offset( mFieldData.mRandomFlip,				fxGrassReplicator ) );	
	//endGroup( "Dimensions" );	// MM: Added Group Footer.

	//addGroup( "Culling" );	// MM: Added Group Header.
	addField( "UseCulling",			TypeBool,		Offset( mFieldData.mUseCulling,				fxGrassReplicator ) );	
	addField( "CullResolution",		TypeS32,		Offset( mFieldData.mCullResolution,			fxGrassReplicator ) );	
	addField( "ViewDistance",		TypeF32,		Offset( mFieldData.mViewDistance,			fxGrassReplicator ) );	
	addField( "ViewClosest",		TypeF32,		Offset( mFieldData.mViewClosest,			fxGrassReplicator ) );	
	addField( "FadeInRegion",		TypeF32,		Offset( mFieldData.mFadeInRegion,			fxGrassReplicator ) );	
	addField( "FadeOutRegion",		TypeF32,		Offset( mFieldData.mFadeOutRegion,			fxGrassReplicator ) );	
	addField( "AlphaCutoff",		TypeF32,		Offset( mFieldData.mAlphaCutoff,			fxGrassReplicator ) );	
	addField( "GroundAlpha",		TypeF32,		Offset( mFieldData.mGroundAlpha,			fxGrassReplicator ) );	
	//endGroup( "Culling" );	// MM: Added Group Footer.

	//addGroup( "Animation" );	// MM: Added Group Header.
	addField( "SwayOn",				TypeBool,		Offset( mFieldData.mSwayOn,					fxGrassReplicator ) );	
	addField( "SwaySync",			TypeBool,		Offset( mFieldData.mSwaySync,				fxGrassReplicator ) );	
	addField( "SwayMagSide",		TypeF32,		Offset( mFieldData.mSwayMagnitudeSide,		fxGrassReplicator ) );	
	addField( "SwayMagFront",		TypeF32,		Offset( mFieldData.mSwayMagnitudeFront,		fxGrassReplicator ) );	
	addField( "MinSwayTime",		TypeF32,		Offset( mFieldData.mMinSwayTime,			fxGrassReplicator ) );	
	addField( "MaxSwayTime",		TypeF32,		Offset( mFieldData.mMaxSwayTime,			fxGrassReplicator ) );	
	//endGroup( "Animation" );	// MM: Added Group Footer.

	//addGroup( "Lighting" );	// MM: Added Group Header.
	addField( "LightOn",			TypeBool,		Offset( mFieldData.mLightOn,				fxGrassReplicator ) );	
	addField( "LightSync",			TypeBool,		Offset( mFieldData.mLightSync,				fxGrassReplicator ) );	
	addField( "MinLuminance",		TypeF32,		Offset( mFieldData.mMinLuminance,			fxGrassReplicator ) );	
	addField( "MaxLuminance",		TypeF32,		Offset( mFieldData.mMaxLuminance,			fxGrassReplicator ) );	
	addField( "LightTime",			TypeF32,		Offset( mFieldData.mLightTime,				fxGrassReplicator ) );	
	//endGroup( "Lighting" );	// MM: Added Group Footer.

	//addGroup( "Restrictions" );	// MM: Added Group Header.
	addField( "AllowOnTerrain",		TypeBool,		Offset( mFieldData.mAllowOnTerrain,			fxGrassReplicator ) );
	addField( "AllowOnInteriors",	TypeBool,		Offset( mFieldData.mAllowOnInteriors,		fxGrassReplicator ) );
	addField( "AllowOnStatics",		TypeBool,		Offset( mFieldData.mAllowStatics,			fxGrassReplicator ) );	
	addField( "AllowOnWater",		TypeBool,		Offset( mFieldData.mAllowOnWater,			fxGrassReplicator ) );
	addField( "AllowWaterSurface",	TypeBool,		Offset( mFieldData.mAllowWaterSurface,		fxGrassReplicator ) );
	addField( "AllowedTerrainSlope",TypeS32,		Offset( mFieldData.mAllowedTerrainSlope,	fxGrassReplicator ) );
	//endGroup( "Restrictions" );	// MM: Added Group Footer.
	
	// more options for the foilage...
	addField( "FoilageColourTop",	TypeColorF,		Offset( mFieldData.mFoilageColourTop,		fxGrassReplicator ) );
	addField( "FoilageColourBtm",	TypeColorF,		Offset( mFieldData.mFoilageColourBtm,		fxGrassReplicator ) );
	addField( "UseColour",			TypeBool,		Offset( mFieldData.mUseColour,				fxGrassReplicator ) );
	addField( "IsRandomRot",		TypeBool,		Offset( mFieldData.mIsRandom,				fxGrassReplicator ) );
	addField( "IsSquareArea",		TypeBool,		Offset( mFieldData.mIsSquare,				fxGrassReplicator ) );
	addField( "RotationAngle",		TypeF32,		Offset( mFieldData.mRotateAngle,			fxGrassReplicator ) );
	addField( "SurfaceExclusionMode",TypeBool,		Offset( mFieldData.mSurfaceExclusionMode,	fxGrassReplicator ) );
    addField( "SurfaceType",		TypeEnum,		Offset( mFieldData.mTheSurface,				fxGrassReplicator ), 1, &gSurfaceTypeTable );

}

//------------------------------------------------------------------------------

void fxGrassReplicator::CreateGrass(void)
{
	F32				HypX, HypY;
	F32				Angle;
	U32				RelocationRetry;
	Point3F			GrassPosition;
	Point3F	        GrassDirection;
	Point3F			GrassStart;
	Point3F			GrassEnd;
	Point3F			GrassScale;
	bool			CollisionResult;
	RayInfo			RayEvent;

	// Let's get a minimum bounding volume.
	Point3F	MinPoint( -0.5, -0.5, -0.5 );
	Point3F	MaxPoint(  0.5,  0.5,  0.5 );

	// Check Host.
	AssertFatal(isClientObject(), "Trying to create Grass on Server, this is bad!")

	// Cannot continue without Grass Texture!
	if (mFieldData.mGrassFile == "") return;

	// Check that we can position somewhere!
	if (!(	mFieldData.mAllowOnTerrain ||
			mFieldData.mAllowOnInteriors ||
			mFieldData.mAllowStatics ||
			mFieldData.mAllowOnWater))
	{
		// Problem ...
		Con::warnf(ConsoleLogEntry::General, "fxGrassReplicator - Could not place Grass, All alloweds are off!");

		// Return here.
		return;
	}

	// Destroy Grass if we've already got some.
	if (mCurrentGrassCount != 0) DestroyGrass();

	// Inform the user if culling has been disabled!
	if (!mFieldData.mUseCulling)
	{
		// Console Output.
		Con::printf("fxGrassReplicator - Culling has been disabled!");
	}

	// ----------------------------------------------------------------------------------------------------------------------
	// > Calculate the Potential Grass Nodes Required to achieve the selected culling resolution.
	// > Populate Quad-tree structure to depth determined by culling resolution.
	//
	// A little explanation is called for here ...
	//
	//			The approach to this problem has been choosen to make it *much* easier for
	//			the user to control the quad-tree culling resolution.  The user enters a single
	//			world-space value 'mCullResolution' which controls the highest resolution at
	//			which the replicator will check visibility culling.
	//
	//			example:	If 'mCullResolution' is 32 and the size of the replicated area is 128 radius
	//						(256 diameter) then this results in the replicator creating a quad-tree where
	//						there are 256/32 = 8x8 blocks.  Each of these can be checked to see if they
	//						reside within the viewing frustum and if not then they get culled therefore
	//						removing the need to parse all the billboards that occcupy that region.
	//						Most of the time you will get better than this as the culling algorithm will
	//						check the culling pyramid from the top to bottom e.g. the follow 'blocks'
	//						will be checked:-
	//
	//						 1 x 256 x 256 (All of replicated area)
	//						 4 x 128 x 128 (4 corners of above)
	//						16 x  64 x  64 (16 x 4 corners of above)
	//						etc.
	//
	//
	//	1.		First-up, the replicator needs to create a fixed-list of quad-tree nodes to work with.
	//
	//			To calculate this we take the largest outer-radius value set in the replicator and
	//			calculate how many quad-tree levels are required to achieve the selected 'mCullResolution'.
	//			One of the initial problems is that the replicator has seperate radii values for X & Y.
	//			This can lead to a culling resolution smaller in one axis than the other if there is a
	//			difference between the Outer-Radii.  Unfortunately, we just live with this as there is
	//			not much we can do here if we still want to allow the user to have this kind of
	//			elliptical placement control.
	//
	//			To calculate the number of nodes needed we using the following equation:-
	//
	//			Note:- We are changing the Logarithmic bases from 10 -> 2 ... grrrr!
	//
	//			Cr = mCullResolution
	//			Rs = Maximum Radii Diameter
	//
	//
	//				( Log10( Rs / Cr )       )
	//			int ( ---------------- + 0.5 )
	//				( Log10( 2 )             )
	//
	//					---------|
	//					 \
	//					  \			 n
	//					  /			4
	//					 /
	//					---------|
	//					   n = 0
	//
	//
	//			So basically we calculate the number of blocks in 1D at the highest resolution, then
	//			calculate the inverse exponential (base 2 - 1D) to achieve that quantity of blocks.
	//			We round that upto the next highest integer = e.  We then sum 4 to the power 0->e
	//			which gives us the correct number of nodes required.  e is also stored as the starting
	//			level value for populating the quad-tree (see 3. below).
	//
	//	2.		We then proceed to calculate the billboard positions as normal and calculate and assign
	//			each billboard a basic volume (rather than treat each as a point).  We need to take into
	//			account possible front/back swaying as well as the basic plane dimensions here.
	//			When all the billboards have been choosen we then proceed to populate the quad-tree.
	//
	//	3.		To populate the quad-tree we start with a box which completely encapsulates the volume
	//			occupied by all the billboards and enter into a recursive procedure to process that node.
	//			Processing this node involves splitting it into quadrants in X/Y untouched (for now).
	//			We then find candidate billboards with each of these quadrants searching using the
	//			current subset of shapes from the parent (this reduces the searching to a minimum and
	//			is very efficient).
	//
	//			If a quadrant does not enclose any billboards then the node is dropped otherwise it
	//			is processed again using the same procedure.
	//
	//			This happens until we have recursed through the maximum number of levels as calculated
	//			using the summation max (see equation above).  When level 0 is reached, the current list
	//			of enclosed objects is stored within the node (for the rendering algorithm).
	//
	//	4.		When this is complete we have finished here.  The next stage is when rendering takes place.
	//			An algorithm steps through the quad-tree from the top and does visibility culling on
	//			each box (with respect to the viewing frustum) and culls as appropriate.  If the box is
	//			visible then the next level is checked until we reach level 0 where the node contains
	//			a complete subset of billboards enclosed by the visible box.
	//
	//
	//	Using the above algorithm we can now generate *massive* quantities of billboards and (using the
	//	appropriate 'mCullResolution') only visible blocks of billboards will be processed.
	//
	//	- Melv.
	//			
	// ----------------------------------------------------------------------------------------------------------------------



	// ----------------------------------------------------------------------------------------------------------------------
	// Step 1.
	// ----------------------------------------------------------------------------------------------------------------------

	// Calculate the maximum dimension.
	F32 MaxDimension = 2 * ( (mFieldData.mOuterRadiusX > mFieldData.mOuterRadiusY) ? mFieldData.mOuterRadiusX : mFieldData.mOuterRadiusY );

	// Let's check that our cull resolution is not greater than half our maximum dimension (and less than 1).
	if (mFieldData.mCullResolution > (MaxDimension/2) || mFieldData.mCullResolution < 8)
	{
		// Problem ...
		Con::warnf(ConsoleLogEntry::General, "fxGrassReplicator - Could not create Grass, invalid Culling Resolution!");
		Con::warnf(ConsoleLogEntry::General, "fxGrassReplicator - Culling Resolution *must* be >=8 or <= %0.2f!", (MaxDimension/2));

		// Return here.
		return;
	}

	// Take first Timestamp.
	F32 mStartCreationTime = Platform::getRealMilliseconds();

	// Calculate the quad-tree levels needed for selected 'mCullResolution'.
	mQuadTreeLevels = (U32)(mCeil(mLog( MaxDimension / mFieldData.mCullResolution ) / mLog( 2.0f )));

	// Calculate the number of potential nodes required.
	mPotentialGrassNodes = 0;
	for (U32 n = 0; n <= mQuadTreeLevels; n++)
		mPotentialGrassNodes += mCeil(mPow(4.0f, n));	// Ceil to be safe!

	// ----------------------------------------------------------------------------------------------------------------------
	// Step 2.
	// ----------------------------------------------------------------------------------------------------------------------

	// Set Seed.
	RandomGen.setSeed(mFieldData.mSeed);

	// Have we setup the Trig Tables?
	if (!mTrigTableInitialised)
	{
		F32 tIdx = 0.0f;

		// No, so setup Tables.
		for (U32 idx = 0; idx < 720; idx++, tIdx+=M_2PI/720.0f)
		{
			mCosTable[idx] = mCos(tIdx);
			mSinTable[idx] = mSin(tIdx);
		}

		// Signal Trig Tables Initialised.
		mTrigTableInitialised = true;
	}

	// Fetch Grass Ratio.
	F32 GrassRatio = Con::getFloatVariable("$pref::Grass::replicationRatio", 1.0f);
	

	// Calculate the distrobution over 10 circle elements.
	// First we will divide the total circular area in 10 rings, like a dart board.
	F32 relativeRadiusX = (float)mFieldData.mOuterRadiusX/100.0f;
	F32 relativeRadiusY = (float)mFieldData.mOuterRadiusY/100.0f;
	Con::warnf(ConsoleLogEntry::General, "fxGrassReplicator - rel_radiusX: %f, rel_radiusY:%f",relativeRadiusX,relativeRadiusY);

	// Next we will need to find the amount of grass elements that will go into each ring.
	F32 distributedInArea[100]; // number of grass elements in each of the 10 rings.
	F32 totalArea = M_PI*(float)mFieldData.mOuterRadiusX*(float)mFieldData.mOuterRadiusY; // Total area
	F32 areaDensity =(float)mFieldData.mGrassCount/totalArea; // density of grass.
	
	Con::warnf(ConsoleLogEntry::General, "fxGrassReplicator - area density: %f , area:%f",areaDensity,totalArea);



	for (int i_area = 0; i_area < 100; i_area ++ ){
		F32 temp_inner_area = M_PI * (relativeRadiusX*(float)i_area) * (relativeRadiusY*(float)i_area);
		F32 temp_outer_area = M_PI * (relativeRadiusX*((float)i_area+1.0f)) * (relativeRadiusY*((float)i_area+1.0f));
		F32 temp_area = temp_outer_area - temp_inner_area;
		//Con::warnf(ConsoleLogEntry::General, "fxGrassReplicator - area of ring %d: %f",i_area,temp_area);
		distributedInArea[i_area] = (temp_outer_area - temp_inner_area)*areaDensity;
	}


	for (int i_area = 0; i_area < 99; i_area ++ ){
		distributedInArea[i_area+1] = distributedInArea[i_area+1]+ distributedInArea[i_area];
	}
	
		int circularCounter = 0;
	
	// Add Grass.
	for (U32 idx = 0; idx < (mFieldData.mGrassCount * GrassRatio); idx++)
	{
		fxGrassItem*	pGrassItem;
		Point3F			GrassOffsetPos;
		float Angle2;
		// Reset Relocation Retry.
		RelocationRetry = mFieldData.mGrassRetries;
		U32 RetryCounter = 1;
		// Find it a location for the grass elements
		
		do
		{
			// Get the fxGrassReplicator Position.
			GrassPosition = getPosition();
			Angle2	= RandomGen.randF(0, M_2PI);
			

			if (!mFieldData.mIsSquare){
			// Calculate a random offset for the CIRCULAR distribution system.

		
			// change the range where the random mumbers are generated.			
			if (idx > distributedInArea[circularCounter] && circularCounter < 99) circularCounter++;
			
			HypX	= RandomGen.randF(relativeRadiusX*(float)circularCounter, relativeRadiusX*((float)circularCounter+1.0f));
			HypY	= RandomGen.randF(relativeRadiusY*(float)circularCounter, relativeRadiusY*((float)circularCounter+1.0f));
			Angle	= RandomGen.randF(0, M_2PI);
			
			// Calcualte the new position.
			GrassPosition.x += HypX * mCos(Angle);
			GrassPosition.y += HypY * mSin(Angle);
			}	
			else{
			// Calculate a random offset for the SQUARE distribution system.
			float mXn,mYn,dX,dY,mX,mY;
			int n,N;

			n = mCurrentGrassCount + (101 - RelocationRetry); // current iteration
			N = mFieldData.mGrassCount; // n goes to N
			mX = mFieldData.mOuterRadiusX; // lenght of X
			mY = mFieldData.mOuterRadiusY; //length of Y
			mXn = (float)N/mY;  // number of divisions along X
			mYn = (float)N/mX;  // number of divisions along Y
			dX = mX/mXn; // distance between the X divisions
			dY = mY/mYn; // distance between the Y divisions
			
			if (mFieldData.mIsRandom)
				{
				GrassPosition.x += (-1* mX/2) + (float)(1+n%(int)mXn)*dX + RandomGen.randF(-dX,dX);
				GrassPosition.y += (-1* mY/2) + (float)((int)(1+(n/mXn))%(int)mYn)*dY + RandomGen.randF(-dY,dY);
				}
			else 
				{
				GrassPosition.x += (-1* mX/2) + (float)(1+n%(int)mXn)*dX;
				GrassPosition.y += (-1* mY/2) + (float)((int)(1+(n/mXn))%(int)mYn)*dY;
				}

			}
			if (mFieldData.mIsRandom)
				{
				GrassDirection.x = GrassPosition.x + mFieldData.mMaxWidth * mCos(Angle2);
				GrassDirection.y = GrassPosition.y + mFieldData.mMaxWidth * mSin(Angle2);
				}
			else
				{
				GrassDirection.x = GrassPosition.x + mFieldData.mMaxWidth * mCos(mFieldData.mRotateAngle);
				GrassDirection.y = GrassPosition.y + mFieldData.mMaxWidth * mSin(mFieldData.mRotateAngle);
				}


			// Initialise RayCast Search Start/End Positions.
			GrassStart = GrassEnd = GrassDirection;
			GrassStart.z = 2000.f;
			GrassEnd.z= -2000.f;

			// Perform Ray Cast Collision on Client.
			CollisionResult = gClientContainer.castRay(	GrassStart, GrassEnd, FXGRASSREPLICATOR_COLLISION_MASK, &RayEvent);

			// Did we hit anything?
			if (CollisionResult)
			{
				// For now, let's pretend we didn't get a collision.
				CollisionResult = false;

				// Yes, so get it's type.
				U32 CollisionType = RayEvent.object->getTypeMask();

				// Check Illegal Placements, fail if we hit a disallowed type.
				if (((CollisionType & TerrainObjectType) && !mFieldData.mAllowOnTerrain)	||
					((CollisionType & InteriorObjectType) && !mFieldData.mAllowOnInteriors)	||
					((CollisionType & StaticTSObjectType) && !mFieldData.mAllowStatics)	||
					((CollisionType & WaterObjectType) && !mFieldData.mAllowOnWater) ) continue;

				// If we collided with water and are not allowing on the water surface then let's find the
				// terrain underneath and pass this on as the original collision else fail.
				if ((CollisionType & WaterObjectType) && !mFieldData.mAllowWaterSurface &&
					!gClientContainer.castRay( GrassStart, GrassEnd, FXGRASSREPLICATOR_NOWATER_COLLISION_MASK, &RayEvent)) continue;

				// We passed with flying colour so carry on.
				CollisionResult = true;
			}

			GrassStart = GrassEnd = GrassPosition;
			GrassStart.z = 2000.f;
			GrassEnd.z= -2000.f;

			CollisionResult = gClientContainer.castRay(	GrassStart, GrassEnd, FXGRASSREPLICATOR_COLLISION_MASK, &RayEvent);

			// Did we hit anything?
			if (CollisionResult)
			{
				// For now, let's pretend we didn't get a collision.
				CollisionResult = false;

				// Yes, so get it's type.
				U32 CollisionType = RayEvent.object->getTypeMask();

				// Check Illegal Placements, fail if we hit a disallowed type.
				if (((CollisionType & TerrainObjectType) && !mFieldData.mAllowOnTerrain)	||
					((CollisionType & InteriorObjectType) && !mFieldData.mAllowOnInteriors)	||
					((CollisionType & StaticTSObjectType) && !mFieldData.mAllowStatics)	||
					((CollisionType & WaterObjectType) && !mFieldData.mAllowOnWater) ) continue;

				// If we collided with water and are not allowing on the water surface then let's find the
				// terrain underneath and pass this on as the original collision else fail.
				if ((CollisionType & WaterObjectType) && !mFieldData.mAllowWaterSurface &&
					!gClientContainer.castRay( GrassStart, GrassEnd, FXGRASSREPLICATOR_NOWATER_COLLISION_MASK, &RayEvent)) continue;

				if ((twSurfaceReference::ESurfaceType)mFieldData.mTheSurface)  // if we are checking for specific surfaces and not just 'any'
            {
               if ( BaseSurfaces[(twSurfaceReference::ESurfaceType)mFieldData.mTheSurface].isAvailable)
               {
				   CollisionResult = false;
               U8 alphas[TerrainBlock::MaterialGroups];
                  // Checks if the new position is over a blended terrain texture (a 'surface' ) that is *exactly* the same as the surface where
                  // the specified SurfaceReference marker was placed.

                  if (GetTerrainTextures(4000.0f, GrassStart, alphas))
                  {
                  bool skip = mFieldData.mSurfaceExclusionMode;

                     for (int i=0; i < TerrainBlock::MaterialGroups; i++)
                     {
                        if (alphas[ i] != BaseSurfaces[(twSurfaceReference::ESurfaceType)mFieldData.mTheSurface].alphas[ i])
                        {
                           skip = !mFieldData.mSurfaceExclusionMode;                 // even one non-match means this is not the same surface
                           break;
                        }
                     }
                     if (skip) continue;              // don't place grass here
                  }
               }
				} // End terrain texture check
				


				// We passed with flying colour so carry on.
				CollisionResult = true;
			}
			

			// Invalidate if we are below Allowed Terrain Angle.
			if (RayEvent.normal.z < mSin(mDegToRad(90.0f-mFieldData.mAllowedTerrainSlope))) CollisionResult = false;
			
			if (!CollisionResult) RetryCounter++;

		// Wait until we get a collision.
		} while(!CollisionResult && --RelocationRetry);

		// Check for Relocation Problem.
		if (RelocationRetry > 0)
		{
			// Adjust Impact point.
			RayEvent.point.z += mFieldData.mOffsetZ;

			// Set New Position.
			GrassPosition = RayEvent.point;
		}
		else
		{
			// Warning.
			Con::warnf(ConsoleLogEntry::General, "fxGrassReplicator - Could not find satisfactory position for Grass!  %0.2f!", (RetryCounter));

			// Skip to next.
			continue;
		}

		// Monitor the total volume.
		GrassOffsetPos = GrassPosition - getPosition();
		MinPoint.setMin(GrassOffsetPos);
		MaxPoint.setMax(GrassOffsetPos);

		GrassStart = GrassEnd = GrassDirection;
		GrassStart.z = 2000.f;
		GrassEnd.z= -2000.f;
		// Find the vector to the other side of the polygon.
		CollisionResult = gClientContainer.castRay(	GrassStart, GrassEnd, FXGRASSREPLICATOR_COLLISION_MASK, &RayEvent);
		

		// Create our Grass Item.
		pGrassItem = new fxGrassItem;
		pGrassItem->Direction = RayEvent.point - GrassPosition;
		pGrassItem->rAngle = Angle2 * 360/M_2PI; 
		// Reset Frame Serial.
		pGrassItem->LastFrameSerialID = 0;

		// Reset Transform.
		pGrassItem->Transform.identity();

		// Set Position.
		pGrassItem->Transform.setColumn(3, GrassPosition);
		
	
		// Are we fixing size @ max?
		if (mFieldData.mFixSizeToMax)
		{
			// Yes, so set height maximum height.
			pGrassItem->Height = mFieldData.mMaxHeight;
			// Is the Aspect Ratio Fixed?
			if (mFieldData.mFixAspectRatio)
				// Yes, so lock to height.
				pGrassItem->Width = pGrassItem->Height;
			else
				// No, so set width to maximum width.
				pGrassItem->Width = mFieldData.mMaxWidth;
		}
		else
		{
			// No, so choose a new Scale.
			pGrassItem->Height = RandomGen.randF(mFieldData.mMinHeight, mFieldData.mMaxHeight);
			// Is the Aspect Ratio Fixed?
			if (mFieldData.mFixAspectRatio)
				// Yes, so lock to height.
				pGrassItem->Width = pGrassItem->Height;
			else
				// No, so choose a random width.
				pGrassItem->Width = RandomGen.randF(mFieldData.mMinWidth, mFieldData.mMaxWidth);
		}

		// Are we randomly flipping horizontally?
		if (mFieldData.mRandomFlip)
			// Yes, so choose a random flip for this object.
			pGrassItem->Flipped = (RandomGen.randF(0, 1000) < 500.0f) ? false : true;
		else
			// No, so turn-off flipping.
			pGrassItem->Flipped = false;
		
		
		// Calculate Grass Item World Box.
		// NOTE:-	We generate a psuedo-volume here.  It's basically the volume to which the
		//			plane can move and this includes swaying!
		//
		// Is Sway On?
		if (mFieldData.mSwayOn)
		{
			// Yes, so take swaying into account...
			pGrassItem->GrassBox.min =	GrassPosition +
											Point3F(-pGrassItem->Width / 2.0f - mFieldData.mSwayMagnitudeSide,
													-0.5f - mFieldData.mSwayMagnitudeFront,
													pGrassItem->Height );

			pGrassItem->GrassBox.max =	GrassPosition +
											Point3F(+pGrassItem->Width / 2.0f + mFieldData.mSwayMagnitudeSide,
													+0.5f + mFieldData.mSwayMagnitudeFront,
													pGrassItem->Height );
		}
		else
		{
			// No, so give it a minimum volume...
			pGrassItem->GrassBox.min =	GrassPosition +
											Point3F(-pGrassItem->Width / 2.0f,
													-0.5f,
													pGrassItem->Height );

			pGrassItem->GrassBox.max =	GrassPosition +
											Point3F(+pGrassItem->Width / 2.0f,
													+0.5f,
													pGrassItem->Height );
		}

		// Store Shape in Replicated Shapes Vector.
		mReplicatedGrass.push_back(pGrassItem);

		// Increase Grass Count.
		mCurrentGrassCount++;
	}

	// Is Lighting On?
	if (mFieldData.mLightOn)
	{
		// Yes, so step through Grass.
		for (U32 idx = 0; idx < mCurrentGrassCount; idx++)
		{
			fxGrassItem*	pGrassItem;

			// Fetch the Grass Item.
			pGrassItem = mReplicatedGrass[idx];

			// Do we have an item?
			if (pGrassItem)
			{
				// Yes, so are lights syncronised?
				if (mFieldData.mLightSync)
				{
					// Yes, so reset Global Light phase.
					mGlobalLightPhase = 0.0f;
					// Set Global Light Time Ratio.
					mGlobalLightTimeRatio = 719.0f / mFieldData.mLightTime;
				}
				else
				{
					// No, so choose a random Light phase.
					pGrassItem->LightPhase = RandomGen.randF(0, 719.0f);
					// Set Light Time Ratio.
					pGrassItem->LightTimeRatio = 719.0f / mFieldData.mLightTime;
				}
			}
		}

	}

	// Is Swaying Enabled?
	if (mFieldData.mSwayOn)
	{
		// Yes, so step through Grass.
		for (U32 idx = 0; idx < mCurrentGrassCount; idx++)
		{
			fxGrassItem*	pGrassItem;

			// Fetch the Grass Item.
			pGrassItem = mReplicatedGrass[idx];

			// Do we have an item?
			if (pGrassItem)
			{
				// Are we using Sway Sync?
				if (mFieldData.mSwaySync)
				{
					// Yes, so reset Global Sway phase.
					mGlobalSwayPhase = 0.0f;
					// Set Global Sway Time Ratio.
					mGlobalSwayTimeRatio = 719.0f / RandomGen.randF(mFieldData.mMinSwayTime, mFieldData.mMaxSwayTime);
				}
				else
				{
					// No, so choose a random Sway phase.
					pGrassItem->SwayPhase = RandomGen.randF(0, 719.0f);
					// Set to random Sway Time.
					pGrassItem->SwayTimeRatio = 719.0f / RandomGen.randF(mFieldData.mMinSwayTime, mFieldData.mMaxSwayTime);
				}
			}
		}
	}

	// Update our Object Volume.
	mObjBox.min.set(MinPoint);
	mObjBox.max.set(MaxPoint);
	setTransform(mObjToWorld);

	// ----------------------------------------------------------------------------------------------------------------------
	// Step 3.
	// ----------------------------------------------------------------------------------------------------------------------

	// Reset Next Allocated Node to Stack base.
	mNextAllocatedNodeIdx = 0;

	// Allocate a new Node.
	fxGrassQuadrantNode* pNewNode = new fxGrassQuadrantNode;

	// Store it in the Quad-tree.
	mGrassQuadTree.push_back(pNewNode);

	// Populate Initial Node.
	//
	// Set Start Level.
	pNewNode->Level = mQuadTreeLevels;
	// Calculate Total Grass Area.
	pNewNode->QuadrantBox = getWorldBox();
	// Reset Quadrant child nodes.
	pNewNode->QuadrantChildNode[0] =
	pNewNode->QuadrantChildNode[1] =
	pNewNode->QuadrantChildNode[2] =
	pNewNode->QuadrantChildNode[3] = NULL;

	// Create our initial cull list with *all* billboards into.
	fxGrassCulledList CullList;
	CullList.mCulledObjectSet = mReplicatedGrass;

	// Move to next node Index.
	mNextAllocatedNodeIdx++;

	// Let's start this thing going by recursing it's children.
	ProcessNodeChildren(pNewNode, &CullList);

	// Calculate Elapsed Time and take new Timestamp.
	F32 ElapsedTime = (Platform::getRealMilliseconds() - mStartCreationTime) * 0.001f;

	// Console Output.
	Con::printf("fxGrassReplicator - Lev: %d  PotNodes: %d  Used: %d  Objs: %d  Time: %0.4fs.",
				mQuadTreeLevels,
				mPotentialGrassNodes,
				mNextAllocatedNodeIdx-1,
				mBillboardsAcquired,
				ElapsedTime);

	// Dump (*very*) approximate allocated memory.
	F32 MemoryAllocated = (mNextAllocatedNodeIdx-1) * sizeof(fxGrassQuadrantNode);
	MemoryAllocated		+=	mCurrentGrassCount * sizeof(fxGrassItem);
	MemoryAllocated		+=	mCurrentGrassCount * sizeof(fxGrassItem*);
	Con::printf("fxGrassReplicator - Approx. %0.2fMb allocated.", MemoryAllocated / 1048576.0f);

	// ----------------------------------------------------------------------------------------------------------------------

	// Take first Timestamp.
	mLastRenderTime = Platform::getVirtualMilliseconds();
}

//------------------------------------------------------------------------------

Box3F fxGrassReplicator::FetchQuadrant(Box3F Box, U32 Quadrant)
{
	Box3F QuadrantBox;

	// Select Quadrant.
	switch(Quadrant)
	{
		// UL.
		case 0:
			QuadrantBox.min = Box.min + Point3F(0, Box.len_y()/2, 0);
			QuadrantBox.max = QuadrantBox.min + Point3F(Box.len_x()/2, Box.len_y()/2, Box.len_z());
			break;

		// UR.
		case 1:
			QuadrantBox.min = Box.min + Point3F(Box.len_x()/2, Box.len_y()/2, 0);
			QuadrantBox.max = QuadrantBox.min + Point3F(Box.len_x()/2, Box.len_y()/2, Box.len_z());
			break;

		// LL.
		case 2:
			QuadrantBox.min = Box.min;
			QuadrantBox.max = QuadrantBox.min + Point3F(Box.len_x()/2, Box.len_y()/2, Box.len_z());
			break;

		// LR.
		case 3:
			QuadrantBox.min = Box.min + Point3F(Box.len_x()/2, 0, 0);
			QuadrantBox.max = QuadrantBox.min + Point3F(Box.len_x()/2, Box.len_y()/2, Box.len_z());
			break;

		default:
			return Box;
	}

	return QuadrantBox;
}

//------------------------------------------------------------------------------

void fxGrassReplicator::ProcessNodeChildren(fxGrassQuadrantNode* pParentNode, fxGrassCulledList* pCullList)
{
	// ---------------------------------------------------------------
	// Split Node into Quadrants and Process each.
	// ---------------------------------------------------------------

	// Process All Quadrants (UL/UR/LL/LR).
	for (U32 q = 0; q < 4; q++)
		ProcessQuadrant(pParentNode, pCullList, q);
}

//------------------------------------------------------------------------------

void fxGrassReplicator::ProcessQuadrant(fxGrassQuadrantNode* pParentNode, fxGrassCulledList* pCullList, U32 Quadrant)
{
	// Fetch Quadrant Box.
	const Box3F QuadrantBox = FetchQuadrant(pParentNode->QuadrantBox, Quadrant);

	// Create our new Cull List.
	fxGrassCulledList CullList(QuadrantBox, pCullList);

	// Did we get any objects?
	if (CullList.GetListCount() > 0)
	{
		// Yes, so allocate a new Node.
		fxGrassQuadrantNode* pNewNode = new fxGrassQuadrantNode;

		// Store it in the Quad-tree.
		mGrassQuadTree.push_back(pNewNode);

		// Move to next node Index.
		mNextAllocatedNodeIdx++;

		// Populate Quadrant Node.
		//
		// Next Sub-level.
		pNewNode->Level = pParentNode->Level - 1;
		// Calculate Quadrant Box.
		pNewNode->QuadrantBox = QuadrantBox;
		// Reset Child Nodes.
		pNewNode->QuadrantChildNode[0] =
		pNewNode->QuadrantChildNode[1] =
		pNewNode->QuadrantChildNode[2] =
		pNewNode->QuadrantChildNode[3] = NULL;
		
		// Put a reference in parent.
		pParentNode->QuadrantChildNode[Quadrant] = pNewNode;

		// If we're not at sub-level 0 then process this nodes children.
		if (pNewNode->Level != 0) ProcessNodeChildren(pNewNode, &CullList);
		// If we've reached sub-level 0 then store Cull List (for rendering).
		if (pNewNode->Level == 0)
		{
			// Store the render list from our culled object set.
			pNewNode->RenderList = CullList.mCulledObjectSet;
			// Keep track of the total billboard acquired.
			mBillboardsAcquired += CullList.GetListCount();
		}
	}
}

//------------------------------------------------------------------------------

void fxGrassReplicator::SyncGrassReplicators(void)
{
	// Check Host.
	AssertFatal(isServerObject(), "We *MUST* be on server when Synchronising Grass!")

	// Find the Replicator Set.
	SimSet *fxGrassSet = dynamic_cast<SimSet*>(Sim::findObject("fxGrassSet"));

	// Return if Error.
	if (!fxGrassSet)
	{
		// Console Warning.
		Con::warnf("fxGrassReplicator - Cannot locate the 'fxGrassSet', this is bad!");
		// Return here.
		return;
	}

	// Parse Replication Object(s).
	for (SimSetIterator itr(fxGrassSet); *itr; ++itr)
	{
		// Fetch the Replicator Object.
		fxGrassReplicator* Replicator = static_cast<fxGrassReplicator*>(*itr);
		// Set Grass Replication Mask.
		if (Replicator->isServerObject())
		{
			Con::printf("fxGrassReplicator - Restarting fxGrassReplicator Object...");
			Replicator->setMaskBits(GrassReplicationMask);
		}
	}

	// Info ...
	Con::printf("fxGrassReplicator - Client Grass Sync has completed.");
}


//------------------------------------------------------------------------------

void fxGrassReplicator::DestroyGrass(void)
{
	// Check Host.
	AssertFatal(isClientObject(), "Trying to destroy Grass on Server, this is bad!")

	// Destroy Quad-tree.
	mPotentialGrassNodes = 0;
	// Reset Billboards Acquired.
	mBillboardsAcquired = 0;

	// Finish if we didn't create any shapes.
	if (mCurrentGrassCount == 0) return;

	// Remove shapes.
	for (U32 idx = 0; idx < mCurrentGrassCount; idx++)
	{
		fxGrassItem*	pGrassItem;

		// Fetch the Grass Item.
		pGrassItem = mReplicatedGrass[idx];

		// Delete Shape.
		if (pGrassItem) delete pGrassItem;
	}

	// Let's remove the Quad-Tree allocations.
	for (	Vector<fxGrassQuadrantNode*>::iterator QuadNodeItr = mGrassQuadTree.begin();
			QuadNodeItr != mGrassQuadTree.end();
			QuadNodeItr++ )
		{
			// Remove the node.
			delete *QuadNodeItr;
		}

	// Clear the Grass Quad-Tree Vector.
	mGrassQuadTree.clear();

	// Clear the Replicated Grass Vector.
	mReplicatedGrass.clear();

	// Clear the Frustum Render Set Vector.
	mFrustumRenderSet.mVisObjectSet.clear();


	// Reset Grass Count.
	mCurrentGrassCount = 0;
}

//------------------------------------------------------------------------------

void fxGrassReplicator::StartUp(void)
{
	// Flag, Client Replication Started.
	mClientReplicationStarted = true;

	// Create foliage on Client.
	if (isClientObject()) CreateGrass();
}

//------------------------------------------------------------------------------

bool fxGrassReplicator::onAdd()
{
	if(!Parent::onAdd()) return(false);

	// Add the Replicator to the Replicator Set.
	dynamic_cast<SimSet*>(Sim::findObject("fxGrassSet"))->addObject(this);

	// Set Default Object Box.
	mObjBox.min.set( -0.5, -0.5, -0.5 );
	mObjBox.max.set(  0.5,  0.5,  0.5 );
	// Reset the World Box.
	resetWorldBox();
	// Set the Render Transform.
	setRenderTransform(mObjToWorld);

	// Add to Scene.
	addToScene();
	mAddedToScene = true;

	// Are we on the client?
    if ( isClientObject() )
	{
		// Yes, so load foliage texture.
		mFieldData.mGrassTexture = TextureHandle( mFieldData.mGrassFile,  MeshTexture );

		// If we are in the editor then we can manually startup replication.
		if (gEditingMission) mClientReplicationStarted = true;
	}

	// Return OK.
	return(true);
}

//------------------------------------------------------------------------------

void fxGrassReplicator::onRemove()
{
	// Remove the Replicator from the Replicator Set.
	dynamic_cast<SimSet*>(Sim::findObject("fxGrassSet"))->removeObject(this);

	// Remove from Scene.
	removeFromScene();
	mAddedToScene = false;

	// Are we on the Client?
	if (isClientObject())
	{
		// Yes, so destroy Grass.
		DestroyGrass();

		// Remove Texture.
		mFieldData.mGrassTexture = NULL;
	}

	// Do Parent.
	Parent::onRemove();
}

//------------------------------------------------------------------------------

void fxGrassReplicator::inspectPostApply()
{
	// Set Parent.
	Parent::inspectPostApply();

	// Set Grass Replication Mask (this object only).
	setMaskBits(GrassReplicationMask);
}

//------------------------------------------------------------------------------

void fxGrassReplicator::onEditorEnable()
{
}

//------------------------------------------------------------------------------

void fxGrassReplicator::onEditorDisable()
{
}

//------------------------------------------------------------------------------

ConsoleFunction(StartGrassReplication, void, 1, 1, "StartGrassReplication()")
{
	// Find the Replicator Set.
	SimSet *fxGrassSet = dynamic_cast<SimSet*>(Sim::findObject("fxGrassSet"));

	// Return if Error.
	if (!fxGrassSet)
	{
		// Console Warning.
		Con::warnf("fxGrassReplicator - Cannot locate the 'fxGrassSet', this is bad!");
		// Return here.
		return;
	}

	// Parse Replication Object(s).
	for (SimSetIterator itr(fxGrassSet); *itr; ++itr)
	{
		// Fetch the Replicator Object.
		fxGrassReplicator* Replicator = static_cast<fxGrassReplicator*>(*itr);
		// Start Client Objects Only.
		if (Replicator->isClientObject()) Replicator->StartUp();
	}

	// Info ...
	Con::printf("fxGrassReplicator - Client Grass Replication Startup is complete.");
}

//------------------------------------------------------------------------------

bool fxGrassReplicator::prepRenderImage(SceneState* state, const U32 stateKey, const U32 startZone,
								const bool modifyBaseZoneState)
{
	// Return if last state.
	if (isLastState(state, stateKey)) return false;
	// Set Last State.
	setLastState(state, stateKey);

   // Is Object Rendered?
   if (state->isObjectRendered(this))
   {	   
		// Yes, so get a SceneRenderImage.
		SceneRenderImage* image = new SceneRenderImage;
		// Populate it.
		image->obj = this;
		image->sortType = SceneRenderImage::Normal;
		// Insert it into the scene images.
		state->insertRenderImage(image);
   }

   return false;
}

//------------------------------------------------------------------------------

void fxGrassReplicator::renderObject(SceneState* state, SceneRenderImage*)
{
#ifdef DARREN_MMO
	// Check we are in Canonical State.
	AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

	// Calculate Elapsed Time and take new Timestamp.
	S32 Time = Platform::getVirtualMilliseconds();
	F32 ElapsedTime = (Time - mLastRenderTime) * 0.001f;
	mLastRenderTime = Time;

	// Setup out the Projection Matrix/Viewport.
	RectI viewport;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	dglGetViewport(&viewport);
	state->setupBaseProjection();

	// Draw Placement Area (if needed and Editing Mission).
	if (gEditingMission && mFieldData.mShowPlacementArea)
	{
		// Setup our rendering state.
		glPushMatrix();
		dglMultMatrix(&getTransform());
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

/*		// Do we need to draw the Inner Radius?
		if (mFieldData.mInnerRadiusX || mFieldData.mInnerRadiusY)
		{
			// Yes, so draw Inner Radius.
			glBegin(GL_TRIANGLE_STRIP);
			for (U32 Angle = mCreationAreaAngle; Angle < (mCreationAreaAngle+360); Angle++)
			{
				F32		XPos, YPos;

				// Calculate Position.
				XPos = mFieldData.mInnerRadiusX * mCos(mDegToRad(-(F32)Angle));
				YPos = mFieldData.mInnerRadiusY * mSin(mDegToRad(-(F32)Angle));

				// Set Colour.
				glColor4f(	mFieldData.mPlaceAreaColour.red,
							mFieldData.mPlaceAreaColour.green,
							mFieldData.mPlaceAreaColour.blue,
							AREA_ANIMATION_ARC * (Angle-mCreationAreaAngle));

				// Draw Arc Line.
				glVertex3f(	XPos, YPos, -(F32)mFieldData.mPlacementBandHeight/2.0f);
				glVertex3f(	XPos, YPos, +(F32)mFieldData.mPlacementBandHeight/2.0f);

			}
			glEnd();
		}
*/
		// Do we need to draw the Outer Radius?
		if (mFieldData.mOuterRadiusX || mFieldData.mOuterRadiusY)
		{
			// Yes, so draw Outer Radius.
			glBegin(GL_TRIANGLE_STRIP);
			for (U32 Angle = mCreationAreaAngle; Angle < (mCreationAreaAngle+360); Angle++)
			{
				F32		XPos, YPos;

				// Calculate Position.
				XPos = mFieldData.mOuterRadiusX * mCos(mDegToRad(-(F32)Angle));
				YPos = mFieldData.mOuterRadiusY * mSin(mDegToRad(-(F32)Angle));

				// Set Colour.
				glColor4f(	mFieldData.mPlaceAreaColour.red,
							mFieldData.mPlaceAreaColour.green,
							mFieldData.mPlaceAreaColour.blue,
							AREA_ANIMATION_ARC * (Angle-mCreationAreaAngle));

				// Draw Arc Line.
				glVertex3f(	XPos, YPos, -(F32)mFieldData.mPlacementBandHeight/2.0f);
				glVertex3f(	XPos, YPos, +(F32)mFieldData.mPlacementBandHeight/2.0f);

			}
			glEnd();
		}

		// Restore rendering state.
		glDisable(GL_BLEND);
		glPopMatrix();

		// Animate Area Selection.
		mCreationAreaAngle += 1000 * ElapsedTime;
		mCreationAreaAngle = mCreationAreaAngle % 360;
	}

	// Draw Grass.
	if (!mFieldData.mHideGrass && mCurrentGrassCount)
	{
		// Calculate some constants.
      const F32	ClippedViewDistance		= (mFieldData.mViewDistance*SceneGraph::smMyVisibleDistanceMod);
		const F32	MinimumViewDistance		= (mFieldData.mViewClosest - mFieldData.mFadeOutRegion)*SceneGraph::smMyVisibleDistanceMod;
		const F32	MaximumViewDistance		= (ClippedViewDistance + mFieldData.mFadeInRegion)*SceneGraph::smMyVisibleDistanceMod;
		const F32	LuminanceMidPoint		= (mFieldData.mMinLuminance + mFieldData.mMaxLuminance) / 2.0f;
		const F32	LuminanceMagnitude		= mFieldData.mMaxLuminance - LuminanceMidPoint;

		// Grass Element Details.
		MatrixF			ModelView, tempModelView;
		Point4F			Position;
		const Point4F	XRotation(1,0,0,0);
		const Point4F	YRotation(0,1,0,0);
		Point4F			ZRotation(0,0,1,0);
		F32				LeftTexPos;
		F32				RightTexPos;

		// Sway Luminance.
		F32	Luminance = 1.0f;

		// Reset Sway Offsets.
		F32	SwayOffsetX = 0.0f;
		F32	SwayOffsetY = 0.0f;

		// Is Swaying On and *in* Sync?
		if (mFieldData.mSwayOn && mFieldData.mSwaySync)
		{
			// Yes, so calculate Global Sway Offset.
			SwayOffsetX = mFieldData.mSwayMagnitudeSide * mCosTable[(U32)mGlobalSwayPhase];
			SwayOffsetY = mFieldData.mSwayMagnitudeFront * mSinTable[(U32)mGlobalSwayPhase];

			// Animate Global Sway Phase (Modulus).
			mGlobalSwayPhase = mGlobalSwayPhase + (mGlobalSwayTimeRatio * ElapsedTime);
			if (mGlobalSwayPhase >= 720.0f) mGlobalSwayPhase -= 720.0f;
		}

		// Is Light On and *in* Sync?
		if (mFieldData.mLightOn && mFieldData.mLightSync)
		{
			// Yes, so calculate Global Light Luminance.
			Luminance = LuminanceMidPoint + LuminanceMagnitude * mCosTable[(U32)mGlobalLightPhase];

			// Animate Global Light Phase (Modulus).
			mGlobalLightPhase = mGlobalLightPhase + (mGlobalLightTimeRatio * ElapsedTime);
			if (mGlobalLightPhase >= 720.0f) mGlobalLightPhase -= 720.0f;
		}

		// Are we using culling?
		if (mFieldData.mUseCulling)
		{
			// Yes, so clear the Render Object Set.
			mFrustumRenderSet.mVisObjectSet.clear();

			// Calculate nearest Clipping Far-Plane.
			//
			// NOTE:-	Here we want the nearest plane to which we want to clip.
			//			This will be either the 'mViewDistance'/'mFadeInRegion' or the
			//			frustum FarPlane.
			F32 FarClipPlane = getMin((F32)state->getFarPlane(), mFieldData.mViewDistance + mFieldData.mFadeInRegion);

			// Setup the Clip-Planes.
			mFrustumRenderSet.SetupClipPlanes(state, FarClipPlane);

			if ( mFieldData.mUseDebugInfo)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			}

			// Compile the Visible Set.
			mFrustumRenderSet.CompileVisibleSet(mGrassQuadTree[0], getRenderTransform(), mFieldData.mUseDebugInfo);

			if ( mFieldData.mUseDebugInfo)
			{
				// Restore rendering state.
				glDisable(GL_BLEND);
			}
		}
		else
		{
			// No, so clear the Render Object Set.
			mFrustumRenderSet.mVisObjectSet.clear();

			// Handle *all* objects ... potential eeek!
			mFrustumRenderSet.mVisObjectSet.merge(mReplicatedGrass);
		}

		// Increase Frame Serial ID.
		mFrameSerialID++;

		// Reset Grass Processed Counter.
		U32 GrassProcessed = 0;

		// Only process if we have any trivially visible objects.
		if (mFrustumRenderSet.mVisObjectSet.size() > 0)
		{
			// Setup Render State.
			glEnable            ( GL_TEXTURE_2D );
			glBindTexture		( GL_TEXTURE_2D, mFieldData.mGrassTexture.getGLName() );
			glEnable			( GL_BLEND );
			glBlendFunc			( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
			glTexEnvi           ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
			glEnable			( GL_ALPHA_TEST );
			glDisable			( GL_CULL_FACE );
			glAlphaFunc			( GL_GREATER, mFieldData.mAlphaCutoff );

			// Fetch our Modelview.
			dglGetModelview(&ModelView);

			// Store our Modelview. 
			glPushMatrix();

			// Optimize the fields so that only every 2nd or 3rd object is drawn from afar.
			// taken out as it causes severe popping
			//int step = 1;
			//if((getPosition() - state->getCameraPosition()).len() > (mFieldData.mViewDistance - mFieldData.mFadeInRegion)/1.5)
			//step = 2;
			//if((getPosition() - state->getCameraPosition()).len() > mFieldData.mViewDistance - mFieldData.mFadeInRegion)
			//step = 3;

			// Step through Grass.
			for (U32 idx = 0; idx < mFrustumRenderSet.mVisObjectSet.size(); idx ++)
			{
				fxGrassItem*	pGrassItem;
				F32				Width, Height;
				F32				Distance;
				F32				ItemAlpha = 1.0f;
				F32				shrinkage;
				F32				FogAlpha;
				Point3F			GrassPosition;
				
			
				// Fetch the Grass Item.
				pGrassItem = mFrustumRenderSet.mVisObjectSet[idx];

				// Check to see if we've already rendered this object.
				//
				// NOTE:-	We need to do this as objects can overlap over quad-tree boundaries and are
				//			therefore picked up.  We don't want to remove these duplicates otherwise the
				//			overlapping billboards would be contained within a single quad-block only
				//			and could possibly be culled even though they are visible in the adjacent
				//			quad-block.  In other words, billboards are *not* treated as a point but rather
				//			a psuedo volume which stops them popping-up when the central point is visible.
				//
				//			We also mark the billboard as rendered even if it results in being culled
				//			further on as this stops it being rendered further this frame.
				//
				// Has the item been rendered this frame?
				if (pGrassItem->LastFrameSerialID != mFrameSerialID)
				{
					// No, so flag as rendered this frame.
					pGrassItem->LastFrameSerialID = mFrameSerialID;

					// Increase Grass Processed Counter.
					GrassProcessed++;

					// Is Swaying On and *not* in Sync?
					if (mFieldData.mSwayOn && !mFieldData.mSwaySync)
					{
						// Yes, so calculate Sway Offset.
						SwayOffsetX = mFieldData.mSwayMagnitudeSide * mCosTable[(U32)pGrassItem->SwayPhase];
						SwayOffsetY = mFieldData.mSwayMagnitudeFront * mSinTable[(U32)pGrassItem->SwayPhase];

						// Animate Sway Phase (Modulus).
						pGrassItem->SwayPhase = pGrassItem->SwayPhase + (pGrassItem->SwayTimeRatio * ElapsedTime);
						if (pGrassItem->SwayPhase >= 720.0f) pGrassItem->SwayPhase -= 720.0f;
					}

					// Is Light On and *not* in Sync?
					if (mFieldData.mLightOn && !mFieldData.mLightSync)
					{
						// Yes, so calculate Light Luminance.
						Luminance = LuminanceMidPoint + LuminanceMagnitude * mCosTable[(U32)pGrassItem->LightPhase];

						// Animate Light Phase (Modulus).
						pGrassItem->LightPhase = pGrassItem->LightPhase + (pGrassItem->LightTimeRatio * ElapsedTime);
						if (pGrassItem->LightPhase >= 720.0f) pGrassItem->LightPhase -= 720.0f;
					}

					// Calculate Distance to Item.
					Distance = (pGrassItem->Transform.getPosition() - state->getCameraPosition()).len();

					// Trivially reject the billboard if it's beyond the SceneGraphs visible distance.
					if (Distance > state->getVisibleDistance())	continue;

					// Calculate Fog Alpha.
					FogAlpha = 1.0f - state->getHazeAndFog(Distance, pGrassItem->Transform.getPosition().z - state->getCameraPosition().z);

					// Trivially reject the billboard if it's totally transparent.
					if (FogAlpha < FXGRASS_ALPHA_EPSILON) continue;

					// Can we trivially accept the billboard?
					if (Distance >= MinimumViewDistance && Distance <= MaximumViewDistance)
					{
						// Yes, so are we fading out?
						if (Distance < mFieldData.mViewClosest)
						{
							// Yes, so set fade-out.
							//ItemAlpha = 1.0f - ((mFieldData.mViewClosest - Distance) * mFadeOutGradient);
							//Yes, so we will shrink the vertical axis.
							//shrinkage = 1.0f; //+ ((mFieldData.mViewClosest - Distance) * mFadeOutGradient);
							shrinkage = 1-((mFieldData.mViewClosest - Distance)/(mFieldData.mViewClosest - mFieldData.mFadeOutRegion));
						}
						// No, so are we fading in?
						else if (Distance > ClippedViewDistance)
						{
							// Yes, so set fade-in
							//ItemAlpha = 1.0f - ((Distance - ClippedViewDistance) * mFadeInGradient);
							// just decrease the shrinkage instead
							shrinkage = ((MaximumViewDistance - Distance)/(MaximumViewDistance - ClippedViewDistance));
						}
						// No, so set full.
						else
						{
							//ItemAlpha = 1.0f;
							shrinkage = 1.0f;
						}

						// Clamp upper-limit to Fog Alpha.
						if (ItemAlpha > FogAlpha) ItemAlpha = FogAlpha;

						// Let's billboard the foliage.

						// Fetch modelview.
						tempModelView = ModelView;
						tempModelView.mul(pGrassItem->Transform);
						tempModelView.getColumn(3, &GrassPosition);
						// Counter-roll our view to create world-aligned billboards.
						//tempModelView = Roll;
						tempModelView.setColumn(3, GrassPosition);
						dglLoadMatrix(&tempModelView);

						// Fetch Width/Height.
						Width	= pGrassItem->Width / 2.0f;
						Height	= pGrassItem->Height;
						Height = Height * shrinkage;

						// Fetch Flipped Flag.
						LeftTexPos	= pGrassItem->Flipped ? 1.0f : 0.0f;
						RightTexPos	= 1.0f - LeftTexPos;
						
						//LightManager * lightManager = gServerSceneGraph->getLightManager();
						//VectorF dir_sun = lightManager->getLights(



						float tmpColorTop[3],tmpColorBtm[3];
						if (mFieldData.mUseColour){
						      tmpColorTop[0] = mFieldData.mFoilageColourTop.red * Luminance;
						      tmpColorBtm[0] = mFieldData.mFoilageColourBtm.red * Luminance;
							  tmpColorTop[1] = mFieldData.mFoilageColourTop.green * Luminance;
						      tmpColorBtm[1] = mFieldData.mFoilageColourBtm.green * Luminance;
							  tmpColorTop[2] = mFieldData.mFoilageColourTop.blue * Luminance;
						      tmpColorBtm[2] = mFieldData.mFoilageColourBtm.blue * Luminance;	   						
						}
						else {
						for (int i = 0;i<3;i++){
						      tmpColorTop[i] = Luminance;
						      tmpColorBtm[i] = Luminance;
							  //tmpColorTop[0] = tmp_sun.red;
						      //tmpColorBtm[0] = tmp_sun.red;
							  //tmpColorTop[1] = tmp_sun.green;
						      //tmpColorBtm[1] = tmp_sun.green;
							  //tmpColorTop[2] = tmp_sun.blue;
						      //tmpColorBtm[2] = tmp_sun.blue;
							

						   }
						}

                  tmpColorTop[0]*=tgeDNCurrentColor[0];
                  tmpColorTop[1]*=tgeDNCurrentColor[1];
                  tmpColorTop[2]*=tgeDNCurrentColor[2];
                  tmpColorBtm[0]*=tgeDNCurrentColor[0];
                  tmpColorBtm[1]*=tgeDNCurrentColor[1];
                  tmpColorBtm[2]*=tgeDNCurrentColor[2];


						// set an abitrar angle to the face
						float tmpSplitFactor =0;
						if (mFieldData.mIsRandom){
						 glRotatef(pGrassItem->rAngle, 0,0,1);
						 tmpSplitFactor = Height/2.0;
						}
						else {
						 glRotatef(mFieldData.mRotateAngle, 0,0,1);
						 tmpSplitFactor = Height/4.0;
						}
						
						// Draw Billboard.
						glBegin(GL_QUADS);  // Draw the first part of the grass quad.
							// Set Blend Function.
							glColor4f(tmpColorTop[0],tmpColorTop[1],tmpColorTop[2], ItemAlpha);
							
							// Draw Top part of billboard.
							glTexCoord2f	(LeftTexPos,0);
							glVertex3f		(SwayOffsetX,SwayOffsetY-tmpSplitFactor,Height);
							glTexCoord2f	(RightTexPos,0);
							glVertex3f		(2*Width+SwayOffsetX,SwayOffsetY-tmpSplitFactor,Height + pGrassItem->Direction[2]);

							// Set Ground Blend.
							glColor4f(tmpColorBtm[0],tmpColorBtm[1],tmpColorBtm[2], ItemAlpha);
							// Draw bottom part of billboard.
							glTexCoord2f	(RightTexPos,1);
							glVertex3f		(2*Width,0 + tmpSplitFactor/5,+ pGrassItem->Direction[2]);
							glTexCoord2f	(LeftTexPos,1);
							glVertex3f		(0,0 + tmpSplitFactor/5,0);
						glEnd();
						glBegin(GL_QUADS);  // Draw the second part of the grass quad.
							// Set Blend Function.
							glColor4f(tmpColorTop[0],tmpColorTop[1],tmpColorTop[2], ItemAlpha);

							// Draw Top part of billboard.
							glTexCoord2f	(RightTexPos,0);
							glVertex3f		(SwayOffsetX,SwayOffsetY+tmpSplitFactor,Height);
							glTexCoord2f	(LeftTexPos,0);
							glVertex3f		(2*Width+SwayOffsetX,SwayOffsetY+tmpSplitFactor,Height + pGrassItem->Direction[2]);

							// Set Ground Blend.
							glColor4f(tmpColorBtm[0],tmpColorBtm[1],tmpColorBtm[2], ItemAlpha);
							// Draw bottom part of billboard.
							glTexCoord2f	(LeftTexPos,1);
							glVertex3f		(2*Width,0 - tmpSplitFactor/5,+ pGrassItem->Direction[2]);
							glTexCoord2f	(RightTexPos,1);
							glVertex3f		(0,0 - tmpSplitFactor/5,0);
						glEnd();
					}
				}
			}

			// Restore our Modelview.
			glPopMatrix();

			// Restore rendering state.
			glTexEnvi			( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
			glDisable			( GL_CULL_FACE );
			glDisable			( GL_ALPHA_TEST );
			glDisable			( GL_BLEND );
			glDisable			( GL_TEXTURE_2D );
		}

		// Dump Debug Information (Uncomment this if needed).
		//
		// NOTE:-	Here we show the number of objects rendered compared to the potential objects in the viewing frustum.
		//			Remember that the difference is simply billboards that intersect quad-tree boundaries.
		//
		//if (mFieldData.mUseDebugInfo) Con::printf("fxGrassReplicator - %d of %d Grass Items Processed", GrassProcessed, mFrustumRenderSet.mVisObjectSet.size());
	}

	// Restore out nice and friendly canonical state.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	dglSetViewport(viewport);

	// Check we have restored Canonical State.
	AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
#endif // DARREN_MMO
}

//------------------------------------------------------------------------------

U32 fxGrassReplicator::packUpdate(NetConnection * con, U32 mask, BitStream * stream)
{
	// Pack Parent.
	U32 retMask = Parent::packUpdate(con, mask, stream);

	// Write Grass Replication Flag.
	if (stream->writeFlag(mask & GrassReplicationMask))
	{
		stream->writeAffineTransform(mObjToWorld);						// Grass Master-Object Position.

		stream->writeFlag(mFieldData.mUseDebugInfo);					// Grass Debug Information Flag.
		stream->write(mFieldData.mDebugBoxHeight);						// Grass Debug Height.
		stream->write(mFieldData.mSeed);								// Grass Seed.
		stream->write(mFieldData.mGrassCount);						// Grass Count.
		stream->write(mFieldData.mGrassRetries);						// Grass Retries.
		stream->writeString(mFieldData.mGrassFile);					// Grass File.

		stream->write(mFieldData.mInnerRadiusX);						// Grass Inner Radius X.
		stream->write(mFieldData.mInnerRadiusY);						// Grass Inner Radius Y.
		stream->write(mFieldData.mOuterRadiusX);						// Grass Outer Radius X.
		stream->write(mFieldData.mOuterRadiusY);						// Grass Outer Radius Y.

		stream->write(mFieldData.mMinWidth);							// Grass Minimum Width.
		stream->write(mFieldData.mMaxWidth);							// Grass Maximum Width.
		stream->write(mFieldData.mMinHeight);							// Grass Minimum Height.
		stream->write(mFieldData.mMaxHeight);							// Grass Maximum Height.
		stream->write(mFieldData.mFixAspectRatio);						// Grass Fix Aspect Ratio.
		stream->write(mFieldData.mFixSizeToMax);						// Grass Fix Size to Max.
		stream->write(mFieldData.mOffsetZ);								// Grass Offset Z.
		stream->write(mFieldData.mRandomFlip);							// Grass Random Flip.

		stream->write(mFieldData.mUseCulling);							// Grass Use Culling.
		stream->write(mFieldData.mCullResolution);						// Grass Cull Resolution.
		stream->write(mFieldData.mViewDistance);						// Grass View Distance.
		stream->write(mFieldData.mViewClosest);							// Grass View Closest.
		stream->write(mFieldData.mFadeInRegion);						// Grass Fade-In Region.
		stream->write(mFieldData.mFadeOutRegion);						// Grass Fade-Out Region.
		stream->write(mFieldData.mAlphaCutoff);							// Grass Alpha Cutoff.
		stream->write(mFieldData.mGroundAlpha);							// Grass Ground Alpha.

		stream->writeFlag(mFieldData.mSwayOn);							// Grass Sway On Flag.
		stream->writeFlag(mFieldData.mSwaySync);						// Grass Sway Sync Flag.
		stream->write(mFieldData.mSwayMagnitudeSide);					// Grass Sway Magnitude Side2Side.
		stream->write(mFieldData.mSwayMagnitudeFront);					// Grass Sway Magnitude Front2Back.
		stream->write(mFieldData.mMinSwayTime);							// Grass Minimum Sway Time.
		stream->write(mFieldData.mMaxSwayTime);							// Grass Maximum way Time.

		stream->writeFlag(mFieldData.mLightOn);							// Grass Light On Flag.
		stream->writeFlag(mFieldData.mLightSync);						// Grass Light Sync
		stream->write(mFieldData.mMinLuminance);						// Grass Minimum Luminance.
		stream->write(mFieldData.mMaxLuminance);						// Grass Maximum Luminance.
		stream->write(mFieldData.mLightTime);							// Grass Light Time.

		stream->writeFlag(mFieldData.mAllowOnTerrain);					// Allow on Terrain.
		stream->writeFlag(mFieldData.mAllowOnInteriors);				// Allow on Interiors.
		stream->writeFlag(mFieldData.mAllowStatics);					// Allow on Statics.
		stream->writeFlag(mFieldData.mAllowOnWater);					// Allow on Water.
		stream->writeFlag(mFieldData.mAllowWaterSurface);				// Allow on Water Surface.
		stream->write(mFieldData.mAllowedTerrainSlope);					// Grass Offset Z.

		stream->writeFlag(mFieldData.mHideGrass);						// Hide Grass.
		stream->writeFlag(mFieldData.mShowPlacementArea);				// Show Placement Area Flag.
		stream->write(mFieldData.mPlacementBandHeight);					// Placement Area Height.
		stream->write(mFieldData.mPlaceAreaColour);						// Placement Area Colour.

		stream->write(mFieldData.mFoilageColourTop);
		stream->write(mFieldData.mFoilageColourBtm);
		
		stream->writeFlag(mFieldData.mUseColour);
		stream->writeFlag(mFieldData.mIsRandom);
		stream->writeFlag(mFieldData.mIsSquare);
		stream->write(mFieldData.mRotateAngle);
		
		// Terrain texture check:
		stream->writeFlag(mFieldData.mSurfaceExclusionMode);			// allow/disallow toggle
        stream->write( mFieldData.mTheSurface );                        // the surface reference


	}

	// Were done ...
	return(retMask);
}

//------------------------------------------------------------------------------

void fxGrassReplicator::unpackUpdate(NetConnection * con, BitStream * stream)
{
	// Unpack Parent.
	Parent::unpackUpdate(con, stream);

	// Read Replication Details.
	if(stream->readFlag())
	{
		MatrixF		ReplicatorObjectMatrix;

		stream->readAffineTransform(&ReplicatorObjectMatrix);			// Grass Master Object Position.

		mFieldData.mUseDebugInfo = stream->readFlag();					// Grass Debug Information Flag.
		stream->read(&mFieldData.mDebugBoxHeight);						// Grass Debug Height.
		stream->read(&mFieldData.mSeed);								// Grass Seed.
		stream->read(&mFieldData.mGrassCount);						// Grass Count.
		stream->read(&mFieldData.mGrassRetries);						// Grass Retries.
		mFieldData.mGrassFile = stream->readSTString();				// Grass File.

		stream->read(&mFieldData.mInnerRadiusX);						// Grass Inner Radius X.
		stream->read(&mFieldData.mInnerRadiusY);						// Grass Inner Radius Y.
		stream->read(&mFieldData.mOuterRadiusX);						// Grass Outer Radius X.
		stream->read(&mFieldData.mOuterRadiusY);						// Grass Outer Radius Y.

		stream->read(&mFieldData.mMinWidth);							// Grass Minimum Width.
		stream->read(&mFieldData.mMaxWidth);							// Grass Maximum Width.
		stream->read(&mFieldData.mMinHeight);							// Grass Minimum Height.
		stream->read(&mFieldData.mMaxHeight);							// Grass Maximum Height.
		stream->read(&mFieldData.mFixAspectRatio);						// Grass Fix Aspect Ratio.
		stream->read(&mFieldData.mFixSizeToMax);						// Grass Fix Size to Max.
		stream->read(&mFieldData.mOffsetZ);								// Grass Offset Z.
		stream->read(&mFieldData.mRandomFlip);							// Grass Random Flip.

		stream->read(&mFieldData.mUseCulling);							// Grass Use Culling.
		stream->read(&mFieldData.mCullResolution);						// Grass Cull Resolution.
		stream->read(&mFieldData.mViewDistance);						// Grass View Distance.
		stream->read(&mFieldData.mViewClosest);							// Grass View Closest.
		stream->read(&mFieldData.mFadeInRegion);						// Grass Fade-In Region.
		stream->read(&mFieldData.mFadeOutRegion);						// Grass Fade-Out Region.
		stream->read(&mFieldData.mAlphaCutoff);							// Grass Alpha Cutoff.
		stream->read(&mFieldData.mGroundAlpha);							// Grass Ground Alpha.

		mFieldData.mSwayOn = stream->readFlag();						// Grass Sway On Flag.
		mFieldData.mSwaySync = stream->readFlag();						// Grass Sway Sync Flag.
		stream->read(&mFieldData.mSwayMagnitudeSide);					// Grass Sway Magnitude Side2Side.
		stream->read(&mFieldData.mSwayMagnitudeFront);					// Grass Sway Magnitude Front2Back.
		stream->read(&mFieldData.mMinSwayTime);							// Grass Minimum Sway Time.
		stream->read(&mFieldData.mMaxSwayTime);							// Grass Maximum way Time.

		mFieldData.mLightOn = stream->readFlag();						// Grass Light On Flag.
		mFieldData.mLightSync = stream->readFlag();						// Grass Light Sync
		stream->read(&mFieldData.mMinLuminance);						// Grass Minimum Luminance.
		stream->read(&mFieldData.mMaxLuminance);						// Grass Maximum Luminance.
		stream->read(&mFieldData.mLightTime);							// Grass Light Time.

		mFieldData.mAllowOnTerrain = stream->readFlag();				// Allow on Terrain.
		mFieldData.mAllowOnInteriors = stream->readFlag();				// Allow on Interiors.
		mFieldData.mAllowStatics = stream->readFlag();					// Allow on Statics.
		mFieldData.mAllowOnWater = stream->readFlag();					// Allow on Water.
		mFieldData.mAllowWaterSurface = stream->readFlag();				// Allow on Water Surface.
		stream->read(&mFieldData.mAllowedTerrainSlope);					// Allowed Terrain Slope.

		mFieldData.mHideGrass = stream->readFlag();					// Hide Grass.
		mFieldData.mShowPlacementArea = stream->readFlag();				// Show Placement Area Flag.
		stream->read(&mFieldData.mPlacementBandHeight);					// Placement Area Height.
		stream->read(&mFieldData.mPlaceAreaColour);

						
		stream->read(&mFieldData.mFoilageColourTop);					// Define the top vertex colour
		stream->read(&mFieldData.mFoilageColourBtm);					// Define the bottom vertex colour
		mFieldData.mUseColour = stream->readFlag();						// Use a separate vertex colour	
		mFieldData.mIsRandom = stream->readFlag();						// Random rotation of the foilage??
		mFieldData.mIsSquare = stream->readFlag();	
		stream->read(&mFieldData.mRotateAngle);							// if not; use this angle

		mFieldData.mSurfaceExclusionMode = stream->readFlag();			// Allow/Disallow toggle
        stream->read( &mFieldData.mTheSurface);                         // the surface reference

		// Calculate Fade-In/Out Gradients.
		mFadeInGradient		= 1.0f / mFieldData.mFadeInRegion;
		mFadeOutGradient	= 1.0f / mFieldData.mFadeOutRegion;

		// Set Transform.
		setTransform(ReplicatorObjectMatrix);

		// Load Grass Texture on the client.
		mFieldData.mGrassTexture = TextureHandle( mFieldData.mGrassFile,  MeshTexture );

		// Set Quad-Tree Box Height Lerp.
		mFrustumRenderSet.mHeightLerp = mFieldData.mDebugBoxHeight;

		// Create Grass (if Replication has begun).
		if (mClientReplicationStarted) CreateGrass();
	}
}

