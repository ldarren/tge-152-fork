//-----------------------------------------------------------------------------
// TubettiWorld Enhancement
// Surface Reference Object
//
// for the TGE
//
// by Kenneth C. Finney  copyright 2002
// portions written by Melvyn May, 4th August 2002.
//              and by James Holmes, 2002
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
#include "rpg/fx/twSurfaceReference.h"

extern bool gEditingMission;

EnumTable::Enums gSurfaceTypeEnums[] =
{
   { twSurfaceReference::eAny,   "Any"   },
   { twSurfaceReference::eBare,  "Bare"  },
   { twSurfaceReference::eDirt,  "Dirt"  },
   { twSurfaceReference::eGravel,"Gravel"},
   { twSurfaceReference::eStone, "Stone" },
   { twSurfaceReference::eRock,  "Rock"  },
   { twSurfaceReference::ePath,  "Path"  },
   { twSurfaceReference::eTrail, "Trail" },
   { twSurfaceReference::eRoad,  "Road"  },
   { twSurfaceReference::eGrass, "Grass" },
   { twSurfaceReference::eSand,  "Sand"  },
   { twSurfaceReference::ePGrass,"PGrass"},
   { twSurfaceReference::ePSand, "PSand" },
   { twSurfaceReference::eDGrass,"DGrass"},
   { twSurfaceReference::eSnow,  "Snow"  },
   { twSurfaceReference::ePSnow, "PSnow" }
};
EnumTable gSurfaceTypeTable( NUM_SURFACES, gSurfaceTypeEnums );



struct SurfaceStruct BaseSurfaces[] = {   { false, NULL},{ false, NULL},{ false, NULL},{ false, NULL},
                                       { false, NULL},{ false, NULL},{ false, NULL},{ false, NULL},
                                       { false, NULL},{ false, NULL},{ false, NULL},{ false, NULL},
                                       { false, NULL},{ false, NULL},{ false, NULL},{ false, NULL} } ;


//------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(twSurfaceReference);



//------------------------------------------------------------------------------
//
// Class: twSurfaceReference
//
//------------------------------------------------------------------------------

twSurfaceReference::twSurfaceReference()
{
	// Setup NetObject.
	mTypeMask |= StaticObjectType | StaticTSObjectType | StaticRenderedObjectType;
	mAddedToScene = false;
	mNetFlags.set(Ghostable | ScopeAlways);

	// Reset Client Replication Started.
	mClientReferenceStarted = false;

	// Reset Creation Area Angle Animation.
	mCreationAreaAngle = 0;

	// Reset Last Render Time.
	mLastRenderTime = 0;

	// Reset Frame Serial ID.
	mFrameSerialID = 0;
}

//------------------------------------------------------------------------------

twSurfaceReference::~twSurfaceReference()
{
}

//------------------------------------------------------------------------------

void twSurfaceReference::initPersistFields()
{
	// Initialise parents' persistent fields.
	Parent::initPersistFields();

	// Add our own persistent fields.
	//addGroup( "Debugging" );	// MM: Added Group Header.
	addField( "ShowPlacementArea",	TypeBool,		Offset( mFieldData.mShowPlacementArea,		twSurfaceReference ) );
	addField( "PlacementAreaHeight",TypeS32,		Offset( mFieldData.mPlacementBandHeight,	twSurfaceReference ) );
	addField( "PlacementColour",	TypeColorF,		Offset( mFieldData.mPlaceAreaColour,		twSurfaceReference ) );
	//endGroup( "Debugging" );	// MM: Added Group Footer.

   addField( "SurfaceType",    TypeEnum,      Offset(mFieldData.mTheSurfaceRef,	twSurfaceReference ), 1, &gSurfaceTypeTable );
	//endGroup( "Restrictions" );	// MM: Added Group Footer.
}


//--------------------------------------------
// Get the terrain texture under an x,y coord
//
// This fills an array with the alpha value of each of the
// textures under that point. The actual texture names can be
// obtained from the TextureHandle array in the terrain block.
//
// distance - How far to look down for the terrain block
// pt - Point in world space
// alphas - U8 array of size TerrainBlock::materialGroups
//
//
//
bool twSurfaceReference::getTerrainTextures(float distance, Point3F& pt, U8 *alphas)
{
   RayInfo rInfo;
   bool collision;

	if (isServerObject())
	   collision = gServerContainer.castRay(pt, Point3F(pt.x, pt.y, pt.z - distance ), TerrainObjectType, &rInfo);
	else
	   collision = gClientContainer.castRay(pt, Point3F(pt.x, pt.y, pt.z - distance ), TerrainObjectType, &rInfo);

   if (collision)
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
   return false;
}

//------------------------------------------------------------------------------

void twSurfaceReference::GetSurface(void)
{

   BaseSurfaces[(ESurfaceType)mFieldData.mTheSurfaceRef].alphas = new U8[TerrainBlock::MaterialGroups];
   //JMR disabled, doesn't compile on OSX and don't want to spend even a second figuring out why
   /*
   if(getTerrainTextures(300.0f, getPosition(), BaseSurfaces[(ESurfaceType)mFieldData.mTheSurfaceRef].alphas) )
   {
      BaseSurfaces[(ESurfaceType)mFieldData.mTheSurfaceRef].isAvailable = true;
   }
   */
}

//------------------------------------------------------------------------------

void twSurfaceReference::SyncSurfaceReferencers(void)
{
	// Check Host.
	AssertFatal(isServerObject(), "We *MUST* be on server when Synchronising Surface Referencer!")

	// Find the Referencer Set.
	SimSet *twSurfaceSet = dynamic_cast<SimSet*>(Sim::findObject("twSurfaceSet"));

	// Return if Error.
	if (!twSurfaceSet)
	{
		// Console Warning.
		Con::warnf("twSurfaceReference - Cannot locate the 'twSurfaceSet', this is bad!");
		// Return here.
		return;
	}

	// Parse Replication Object(s).
	for (SimSetIterator itr(twSurfaceSet); *itr; ++itr)
	{
		// Fetch the Replicator Object.
		twSurfaceReference* Referencer = static_cast<twSurfaceReference*>(*itr);
		// Set   Mask.
		if (Referencer->isServerObject())
		{
			Con::printf("twSurfaceReference - Restarting twSurfaceReference Object...");
			Referencer->setMaskBits(SurfaceReferenceMask);
		}
	}

	// Info ...
	Con::printf("twSurfaceReference - Client Foliage Sync has completed.");
}


//------------------------------------------------------------------------------

void twSurfaceReference::StartUp(void)
{
	// Flag, Get surfaces sarted.
	mClientReferenceStarted = true;

   if (isServerObject())
			Con::printf("twSurfaceReference: Started on SERVER");

	// Get surfaces
	GetSurface();
}

//------------------------------------------------------------------------------

bool twSurfaceReference::onAdd()
{
	if(!Parent::onAdd()) return(false);

	// Add the Referencer to the Surface Set.
	dynamic_cast<SimSet*>(Sim::findObject("twSurfaceSet"))->addObject(this);

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
		// If we are in the editor then we can manually startup replication.
		if (gEditingMission) mClientReferenceStarted = true;
	}

	if (isServerObject())
      GetSurface();
	// Return OK.
	return(true);
}

//------------------------------------------------------------------------------

void twSurfaceReference::onRemove()
{
	// Remove the Replicator from the Replicator Set.
	dynamic_cast<SimSet*>(Sim::findObject("twSurfaceSet"))->removeObject(this);

	// Remove from Scene.
	removeFromScene();
	mAddedToScene = false;

	// Do Parent.
	Parent::onRemove();
}

//------------------------------------------------------------------------------

void twSurfaceReference::inspectPostApply()
{
	// Set Parent.
	Parent::inspectPostApply();

	// Set Foliage Replication Mask (this object only).
	setMaskBits(SurfaceReferenceMask);
}

//------------------------------------------------------------------------------

void twSurfaceReference::onEditorEnable()
{
}

//------------------------------------------------------------------------------

void twSurfaceReference::onEditorDisable()
{
}

//------------------------------------------------------------------------------

ConsoleFunction(StartSurfaceReferencer, void, 1, 1, "StartSurfaceReferencer()")
{
	// Find the Surface Set.
	SimSet *twSurfaceSet = dynamic_cast<SimSet*>(Sim::findObject("twSurfaceSet"));

	// Return if Error.
	if (!twSurfaceSet)
	{
		// Console Warning.
		Con::warnf("twSurfaceReference - Cannot locate the 'twSurfaceSet', this is bad!");
		// Return here.
		return;
	}

	// Parse Surface Object(s).
	for (SimSetIterator itr(twSurfaceSet); *itr; ++itr)
	{
		// Fetch the Surface Object.
		twSurfaceReference* SurfRef = static_cast<twSurfaceReference*>(*itr);
		// Start Client Objects Only.
		if (SurfRef->isClientObject()) SurfRef->StartUp();
	}

	// Info ...
	Con::printf("twSurfaceReference - Client Surface Reference Startup is complete.");
}

//------------------------------------------------------------------------------

bool twSurfaceReference::prepRenderImage(SceneState* state, const U32 stateKey, const U32 startZone,
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

void twSurfaceReference::renderObject(SceneState* state, SceneRenderImage*)
{
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


			// Draw Outer Radius.
		glBegin(GL_TRIANGLE_STRIP);
		for (U32 Angle = mCreationAreaAngle; Angle < (mCreationAreaAngle+360); Angle++)
		{
			F32		XPos, YPos;

			// Calculate Position.
			XPos = mCos(mDegToRad(-(F32)Angle));
			YPos = mSin(mDegToRad(-(F32)Angle));

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

		// Restore rendering state.
		glDisable(GL_BLEND);
		glPopMatrix();

		// Animate Area Selection.
		mCreationAreaAngle += 1000 * ElapsedTime;
		mCreationAreaAngle = mCreationAreaAngle % 360;
	}

	glMatrixMode(GL_MODELVIEW);
	dglSetViewport(viewport);

	// Check we have restored Canonical State.
	AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
}

//------------------------------------------------------------------------------

U32 twSurfaceReference::packUpdate(NetConnection * con, U32 mask, BitStream * stream)
{
	// Pack Parent.
	U32 retMask = Parent::packUpdate(con, mask, stream);

	// Write Surface Referencer Flag.
	if (stream->writeFlag(mask & SurfaceReferenceMask))
	{
		stream->writeAffineTransform(mObjToWorld);						// Foliage Master-Object Position.

      stream->write( mFieldData.mTheSurfaceRef );                               // the reference surface (if applicable)

		stream->writeFlag(mFieldData.mShowPlacementArea);				// Show Placement Area Flag.
		stream->write(mFieldData.mPlacementBandHeight);					// Placement Area Height.
		stream->write(mFieldData.mPlaceAreaColour);						// Placement Area Colour.
	}

	// Were done ...
	return(retMask);
}

//------------------------------------------------------------------------------

void twSurfaceReference::unpackUpdate(NetConnection * con, BitStream * stream)
{
	// Unpack Parent.
	Parent::unpackUpdate(con, stream);

	// Read Surface Details.
	if(stream->readFlag())
	{
		MatrixF		SurfaceObjectMatrix;

		stream->readAffineTransform(&SurfaceObjectMatrix);			// Foliage Master Object Position.

      stream->read( &mFieldData.mTheSurfaceRef);

		mFieldData.mShowPlacementArea = stream->readFlag();				// Show Placement Area Flag.
		stream->read(&mFieldData.mPlacementBandHeight);					// Placement Area Height.
		stream->read(&mFieldData.mPlaceAreaColour);

		// Set Transform.
		setTransform(SurfaceObjectMatrix);

		// Create Foliage (if Replication has begun).
		if (mClientReferenceStarted) GetSurface();
	}
}

