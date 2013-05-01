/////////////////////////////////////////////////////////////////
// Parashar Krishnamachari
// Volume Light class for integration into Torque Game Engine
/////////////////////////////////////////////////////////////////

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//    Changes:
//           volume-light-fade -- added fading to volumeLight using fade_amt member
//                already present in fxLight.
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "console/consoleTypes.h"
#include "dgl/dgl.h"
#include "core/bitStream.h"
#include "game/gameConnection.h"
#include "sceneGraph/sceneGraph.h"
#include "volLight.h"

#include "platform/event.h"
#include "platform/gameInterface.h"


IMPLEMENT_CO_NETOBJECT_V1(volumeLight);

volumeLight::volumeLight()
{
	// Setup NetObject.
	mTypeMask |= StaticObjectType | StaticTSObjectType | StaticRenderedObjectType;
	mNetFlags.set(Ghostable | ScopeAlways);
	mAddedToScene = false;

	mLastRenderTime = 0;
	mLightHandle = NULL;
	mLTextureName = StringTable->insert("");

	mlpDistance = 8.0f;
	mShootDistance = 15.0f;
	mXextent = 6.0f;
	mYextent = 6.0f;

	mSubdivideU = 32;
	mSubdivideV = 32;

	mfootColour = ColorF(1.f, 1.f, 1.f, 0.2f);
	mtailColour = ColorF(0.f, 0.f, 0.f, 0.0f);
}

volumeLight::~volumeLight()
{

}


///////////////////////////////////////////////////////////////////////////////
// NetObject functions
///////////////////////////////////////////////////////////////////////////////

U32 volumeLight::packUpdate(NetConnection * con, U32 mask, BitStream * stream)
{
	// Pack Parent.
	U32 retMask = Parent::packUpdate(con, mask, stream);

	// Position.and rotation from Parent
	stream->writeAffineTransform(mObjToWorld);

	// Light Field Image
	stream->writeString(mLTextureName);

	// Dimensions
	stream->write(mlpDistance);
	stream->write(mShootDistance);
	stream->write(mXextent);
	stream->write(mYextent);

	// Subdivisions
	stream->write(mSubdivideU);
	stream->write(mSubdivideV);

	// Colors
	stream->write(mfootColour);
	stream->write(mtailColour);

	return retMask;
}

void volumeLight::unpackUpdate(NetConnection * con, BitStream * stream)
{
	// Unpack Parent.
	Parent::unpackUpdate(con, stream);

	MatrixF ObjectMatrix;

	// Position.and rotation
	stream->readAffineTransform(&ObjectMatrix);

	// Light Field Image
	setLtexture(stream->readSTString());

	// Dimensions
	stream->read(&mlpDistance);
	stream->read(&mShootDistance);
	stream->read(&mXextent);
	stream->read(&mYextent);

	// Subdivisions
	stream->read(&mSubdivideU);
	stream->read(&mSubdivideV);

	// Colors
	stream->read(&mfootColour);
	stream->read(&mtailColour);

	// Set Transform.
	setTransform(ObjectMatrix);

	// Very rough, and not complete estimate of the bounding box.
	// A complete one would actually shoot out rays from the hypothetical lightpoint
	// through the vertices to find the extents of the light volume.

	// But there's no point doing so here.  There's no real collision with
	// beams of light.
	buildObjectBox();

	// Set the Render Transform.
	setRenderTransform(mObjToWorld);
}



///////////////////////////////////////////////////////////////////////////////
// Set Value functions (internal to class)
///////////////////////////////////////////////////////////////////////////////

void volumeLight::setLtexture(const char *name)
{
	mLTextureName = StringTable->insert(name);
	mLightHandle = NULL;

	if (*mLTextureName)
	{
		mLightHandle = TextureHandle(mLTextureName, BitmapTexture, true);
	}

	// Set Config Change Mask.
	if (isServerObject()) setMaskBits(volLightMask);
}

void volumeLight::setlpDistance(F32 Dist)
{
	mlpDistance = Dist;
	// Set Config Change Mask.
	if (isServerObject()) setMaskBits(volLightMask);
}

void volumeLight::setShootDistance(F32 Dist)
{
	mShootDistance = Dist;

	buildObjectBox();

	// Set the Render Transform.
	setRenderTransform(mObjToWorld);

	// Set Config Change Mask.
	if (isServerObject()) setMaskBits(volLightMask);
}

void volumeLight::setXextent(F32 extent)
{
	mXextent = extent;

	buildObjectBox();

	// Set the Render Transform.
	setRenderTransform(mObjToWorld);

	// Set Config Change Mask.
	if (isServerObject()) setMaskBits(volLightMask);
}

void volumeLight::setYextent(F32 extent)
{
	mYextent = extent;

	buildObjectBox();
	
	// Set the Render Transform.
	setRenderTransform(mObjToWorld);

	// Set Config Change Mask.
	if (isServerObject()) setMaskBits(volLightMask);
}

void volumeLight::setSubdivideU(U32 val)
{
	mSubdivideU = val;
	// Set Config Change Mask.
	if (isServerObject()) setMaskBits(volLightMask);
}

void volumeLight::setSubdivideV(U32 val)
{
	mSubdivideV = val;
	// Set Config Change Mask.
	if (isServerObject()) setMaskBits(volLightMask);
}

void volumeLight::setfootColour(ColorF col)
{
	mfootColour = col;
	// Set Config Change Mask.
	if (isServerObject()) setMaskBits(volLightMask);
}

void volumeLight::settailColour(ColorF col)
{
	mtailColour = col;
	// Set Config Change Mask.
	if (isServerObject()) setMaskBits(volLightMask);
}



///////////////////////////////////////////////////////////////////////////////
// Set Value functions (external interfaces for scripts)
///////////////////////////////////////////////////////////////////////////////

static void csetLTexture(SimObject * obj, S32, const char ** argv)
{
	volumeLight *vlight = static_cast<volumeLight*>(obj);
	if (!vlight) return;
	vlight->setLtexture(argv[2]);
}

static void csetlpDistance(SimObject * obj, S32, const char ** argv)
{
	volumeLight *vlight = static_cast<volumeLight*>(obj);
	if (!vlight)
		return;
	vlight->setlpDistance(dAtof(argv[2]));
}

static void csetShootDistance(SimObject * obj, S32, const char ** argv)
{
	volumeLight *vlight = static_cast<volumeLight*>(obj);
	if (!vlight)
		return;
	vlight->setShootDistance(dAtof(argv[2]));
}

static void csetXextent(SimObject * obj, S32, const char ** argv)
{
	volumeLight *vlight = static_cast<volumeLight*>(obj);
	if (!vlight)
		return;
	vlight->setXextent(dAtof(argv[2]));
}

static void csetYextent(SimObject * obj, S32, const char ** argv)
{
	volumeLight *vlight = static_cast<volumeLight*>(obj);
	if (!vlight)
		return;
	vlight->setYextent(dAtof(argv[2]));
}

static void csetSubdivideU(SimObject * obj, S32, const char ** argv)
{
	volumeLight *vlight = static_cast<volumeLight*>(obj);
	if (!vlight)
		return;
	vlight->setSubdivideU(dAtoi(argv[2]));
}

static void csetSubdivideV(SimObject * obj, S32, const char ** argv)
{
	volumeLight *vlight = static_cast<volumeLight*>(obj);
	if (!vlight)
		return;
	vlight->setSubdivideV(dAtoi(argv[2]));
}

static void csetfootColour(SimObject * obj, S32, const char ** argv)
{
	volumeLight *vlight = static_cast<volumeLight*>(obj);
	if (!vlight)
		return;
	vlight->setfootColour(ColorF(dAtof(argv[2]),dAtof(argv[3]),dAtof(argv[4]),dAtof(argv[5])));
}

static void csettailColour(SimObject * obj, S32, const char ** argv)
{
	volumeLight *vlight = static_cast<volumeLight*>(obj);
	if (!vlight)
		return;
	vlight->settailColour(ColorF(dAtof(argv[2]),dAtof(argv[3]),dAtof(argv[4]),dAtof(argv[5])));
}



///////////////////////////////////////////////////////////////////////////////
// SimObject functions
///////////////////////////////////////////////////////////////////////////////

bool volumeLight::onAdd()
{
	// Add Parent.
	if(!Parent::onAdd())
		return(false);

	buildObjectBox();
	
	// Set the Render Transform.
	setRenderTransform(mObjToWorld);

	mAddedToScene = true;

	return true;
}

void volumeLight::onRemove()
{
	mAddedToScene = false;

	// remove the texture handle
	mLightHandle = NULL;

	// Do Parent.
	Parent::onRemove();
}

void volumeLight::inspectPostApply()
{
	// Reset the World Box.
	resetWorldBox();
	// Set the Render Transform.
	setRenderTransform(mObjToWorld);

	// Set Parent.
	Parent::inspectPostApply();

	setMaskBits(volLightMask);
}


///////////////////////////////////////////////////////////////////////////////
// SceneObject functions  -- The only meaty part
///////////////////////////////////////////////////////////////////////////////

bool volumeLight::prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState)
{
	// Return if last state.
	if (isLastState(state, stateKey))
		return false;
	// Set Last State.
	setLastState(state, stateKey);

	// no need to render if it isn't enabled
	if((!mEnable) || (!mLightHandle))
		return false;

	// Is Object Rendered?
	if (state->isObjectRendered(this))
	{
		// Yes, so get a SceneRenderImage.
		SceneRenderImage* image = new SceneRenderImage;

		// Populate it.
		image->obj = this;
		image->isTranslucent = true;
		image->sortType = SceneRenderImage::Point;
		state->setImageRefPoint(this, image);

		// Insert it into the scene images.
		state->insertRenderImage(image);
	}

	return false;
}

void volumeLight::renderObject(SceneState *state, SceneRenderImage *image)
{
	Parent::renderObject(state, image);

	// Check we are in Canonical State.
	AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

	// Check that we have a texture
	AssertFatal(mLightHandle, "Error : No texture loaded or file failed to open");

	//////////////////////////// -- Entry assertions

	// Calculate Elapsed Time and take new Timestamp.
	S32 Time = Platform::getVirtualMilliseconds();
	// F32 ElapsedTime = (Time - mLastRenderTime) * 0.001f;
	mLastRenderTime = Time;

	RectI viewport;
	MatrixF RXF;
	Point3F position;

	// Setup out the Projection Matrix/Viewport.
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	dglGetViewport(&viewport);
	state->setupBaseProjection();

	// Remember -- SetupBaseProjection leaves current matrix mode as MODELVIEW when done.
	// Save ModelView Matrix so we can restore Canonical state at exit.
	glPushMatrix();

	// Transform by the objects' transform e.g move it.
	RXF = getTransform();
	RXF.getColumn(3, &position);
	dglMultMatrix(&RXF);

	// Draw the damn thing
	renderGL(state, image);

	//////////////////////////// -- Exit assertions

	// Restore rendering state.
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	// Restore our nice, friendly and dull canonical state.
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	dglSetViewport(viewport);

	// Check we have restored Canonical State.
	AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
}


void volumeLight::sgRenderY(const Point3F &near1, const Point3F &far1, const Point3F &far2,
						   const ColorF &nearcol, const ColorF &farcol, F32 offset)
{
	glBegin(GL_QUADS);
	glColor4f(nearcol.red, nearcol.green, nearcol.blue, nearcol.alpha);
	glTexCoord2f(offset, 0.0);
	glVertex3f(near1.x, -near1.y, 0.0f);

	glColor4f(nearcol.red, nearcol.green, nearcol.blue, nearcol.alpha);
	glTexCoord2f(offset, 0.5);
	glVertex3f(near1.x, 0.0f, 0.0f);

	glColor4f(farcol.red, farcol.green, farcol.blue, farcol.alpha);
	glTexCoord2f(offset, 0.5);
	glVertex3f(far1.x, 0.0, far1.z);

	glColor4f(farcol.red, farcol.green, farcol.blue, farcol.alpha);
	glTexCoord2f(offset, 0.0);
	glVertex3f(far1.x, far1.y, far1.z);
	glEnd();

	glBegin(GL_QUADS);
	glColor4f(farcol.red, farcol.green, farcol.blue, farcol.alpha);
	glTexCoord2f(offset, 0.5);
	glVertex3f(far1.x, 0.0, far1.z);

	glColor4f(nearcol.red, nearcol.green, nearcol.blue, nearcol.alpha);
	glTexCoord2f(offset, 0.5);
	glVertex3f(near1.x, 0.0f, 0.0f);

	glColor4f(nearcol.red, nearcol.green, nearcol.blue, nearcol.alpha);
	glTexCoord2f(offset, 1.0);
	glVertex3f(near1.x, near1.y, 0.0f);

	glColor4f(farcol.red, farcol.green, farcol.blue, farcol.alpha);
	glTexCoord2f(offset, 1.0);
	glVertex3f(far2.x, far2.y, far2.z);
	glEnd();
}

void volumeLight::sgRenderX(const Point3F &near1, const Point3F &far1, const Point3F &far2,
						   const ColorF &nearcol, const ColorF &farcol, F32 offset)
{
	glBegin(GL_QUADS);
	glColor4f(nearcol.red, nearcol.green, nearcol.blue, nearcol.alpha);
	glTexCoord2f(0.0, offset);
	glVertex3f(-near1.x, near1.y, 0.0f);

	glColor4f(nearcol.red, nearcol.green, nearcol.blue, nearcol.alpha);
	glTexCoord2f(0.5, offset);
	glVertex3f(0.0f, near1.y, 0.0f);

	glColor4f(farcol.red, farcol.green, farcol.blue, farcol.alpha);
	glTexCoord2f(0.5, offset);
	glVertex3f(0.0, far1.y, far1.z);

	glColor4f(farcol.red, farcol.green, farcol.blue, farcol.alpha);
	glTexCoord2f(0.0, offset);
	glVertex3f(far1.x, far1.y, far1.z);
	glEnd();

	glBegin(GL_QUADS);
	glColor4f(farcol.red, farcol.green, farcol.blue, farcol.alpha);
	glTexCoord2f(0.5, offset);
	glVertex3f(0.0, far1.y, far1.z);

	glColor4f(nearcol.red, nearcol.green, nearcol.blue, nearcol.alpha);
	glTexCoord2f(0.5, offset);
	glVertex3f(0.0f, near1.y, 0.0f);

	glColor4f(nearcol.red, nearcol.green, nearcol.blue, nearcol.alpha);
	glTexCoord2f(1.0, offset);
	glVertex3f(near1.x, near1.y, 0.0f);

	glColor4f(farcol.red, farcol.green, farcol.blue, farcol.alpha);
	glTexCoord2f(1.0, offset);
	glVertex3f(far2.x, far2.y, far2.z);
	glEnd();
}


///////////////////////////////////////////////////////////////////////////////
// GL Render function...
///////////////////////////////////////////////////////////////////////////////

void volumeLight::renderGL(SceneState *state, SceneRenderImage *image)
{
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindTexture(GL_TEXTURE_2D, mLightHandle.getGLName());

  // AFX CODE BLOCK (volume-light-fade) <<
  // These local variables intentionally eclipse class members
  // with the same name
  ColorF mfootColour = this->mfootColour;
  mfootColour.alpha *= fade_amt;
  ColorF mtailColour = this->mtailColour;
  mtailColour.alpha *= fade_amt;
  // AFX CODE BLOCK (volume-light-fade) >>

	Point3F lightpoint;

	// This is where the hypothetical point source of the spot will be
	// The volume slices are projected along the lines from
	lightpoint.x = lightpoint.y = 0.0f;
	lightpoint.z = -mlpDistance;

	F32 ax = mXextent / 2;
	F32 ay = mYextent / 2;

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// Draw the bottom foot...  this is basically the glowing region.
	glBegin(GL_QUADS);
    // AFX CODE BLOCK (volume-light-fade) <<
		glColor4f(mfootColour.red, mfootColour.green, mfootColour.blue, fade_amt);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-ax, -ay, 0.0f);

		glColor4f(mfootColour.red, mfootColour.green, mfootColour.blue, fade_amt);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(ax, -ay, 0.0f);

		glColor4f(mfootColour.red, mfootColour.green, mfootColour.blue, fade_amt);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(ax, ay, 0.0f);

		glColor4f(mfootColour.red, mfootColour.green, mfootColour.blue, fade_amt);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-ax, ay, 0.0f);
    /* ORIGINAL CODE
		glColor4f(mfootColour.red, mfootColour.green, mfootColour.blue, 1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-ax, -ay, 0.0f);

		glColor4f(mfootColour.red, mfootColour.green, mfootColour.blue, 1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(ax, -ay, 0.0f);

		glColor4f(mfootColour.red, mfootColour.green, mfootColour.blue, 1.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(ax, ay, 0.0f);

		glColor4f(mfootColour.red, mfootColour.green, mfootColour.blue, 1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-ax, ay, 0.0f);
     */
    // AFX CODE BLOCK (volume-light-fade) >>
	glEnd();

	S32 i;

	glDepthMask(GL_FALSE);

	// Slices in X/U space
	for(i = mSubdivideU; i >= 0; i--)
	{
		F32 k = ((F32)i) / mSubdivideU;				// use for the texture coord
		F32 bx = i * mXextent / mSubdivideU - ax;	// use for point positions

		// These are the two endpoints for a slice at the foot
		Point3F end1(bx, -ay, 0.0f);
		Point3F end2(bx, ay, 0.0f);

		end1 -= lightpoint;		// get a vector from point to lightsource
		end1.normalize();		// normalize vector
		end1 *= mShootDistance;	// multiply it out by shootlength

		end1.x += bx;			// Add the original point location to the vector
		end1.y -= ay;

		// Do it again for the other point.
		end2 -= lightpoint;
		end2.normalize();
		end2 *= mShootDistance;

		end2.x += bx;
		end2.y += ay;

		sgRenderY(Point3F(bx, ay, 0.0f), end1, end2, mfootColour, mtailColour, k);
	}

	// Slices in Y/V space
	for(i = 0; i <= mSubdivideV; i++)
	{
		F32 k = ((F32)i) / mSubdivideV;				// use for the texture coord
		F32 by = i * mXextent / mSubdivideV - ay;	// use for point positions

		// These are the two endpoints for a slice at the foot
		Point3F end1(-ax, by, 0.0f);
		Point3F end2(ax, by, 0.0f);

		end1 -= lightpoint;		// get a vector from point to lightsource
		end1.normalize();		// normalize vector
		end1 *= mShootDistance;	// extend it out by shootlength

		end1.x -= ax;			// Add the original point location to the vector
		end1.y += by;


		// Do it again for the other point.
		end2 -= lightpoint;
		end2.normalize();
		end2 *= mShootDistance;

		end2.x += ax;
		end2.y += by;

		sgRenderX(Point3F(ax, by, 0.0f), end1, end2, mfootColour, mtailColour, k);
	}

	glDepthMask(GL_TRUE);
}

///////////////////////////////////////////////////////////////////////////////
// ConObject functions
///////////////////////////////////////////////////////////////////////////////

void volumeLight::initPersistFields()
{
	// Initialise parents' persistent fields.
	Parent::initPersistFields();

	// Our own persistent fields

	// Light Field Image
	addField( "Texture", 			TypeFilename,	Offset( mLTextureName, 		volumeLight ) );

	// Dimensions
	addField( "lpDistance",			TypeF32,		Offset( mlpDistance,		volumeLight ) );
	addField( "ShootDistance",		TypeF32,		Offset( mShootDistance,		volumeLight ) );
	addField( "Xextent",			TypeF32,		Offset( mXextent,			volumeLight ) );
	addField( "Yextent",			TypeF32,		Offset( mYextent,			volumeLight ) );

	// Subdivisions
	addField( "SubdivideU",			TypeS32,		Offset( mSubdivideU,		volumeLight ) );
	addField( "SubdivideV",			TypeS32,		Offset( mSubdivideV,		volumeLight ) );

	// Colors
	addField( "FootColour",			TypeColorF,		Offset( mfootColour,		volumeLight ) );
	addField( "TailColour",			TypeColorF,		Offset( mtailColour,		volumeLight ) );
}

void volumeLight::consoleInit()
{
	// Light Field Image
	Con::addCommand("volumeLight", "setLTexture", csetLTexture, "vlight.setTexture(bitmap)", 3, 3);

	// Dimensions
	Con::addCommand("volumeLight", "setlpDistance", csetlpDistance, "vlight.setlpDistance(value)", 3, 3);
	Con::addCommand("volumeLight", "setShootDistance", csetShootDistance, "vlight.setShootDistance(value)", 3, 3);
	Con::addCommand("volumeLight", "setXextent", csetXextent, "vlight.setXextent(value)", 3, 3);
	Con::addCommand("volumeLight", "setYextent", csetYextent, "vlight.setYextent(value)", 3, 3);

	// Subdivisions
	Con::addCommand("volumeLight", "setSubdivideU", csetSubdivideU, "vlight.setSubdivideU(value)", 3, 3);
	Con::addCommand("volumeLight", "setSubdivideV", csetSubdivideV, "vlight.setSubdivideV(value)", 3, 3);

	// Colors
	Con::addCommand("volumeLight", "setfootColour", csetfootColour, "vlight.setfootColour(r, g, b, a)", 6, 6);
	Con::addCommand("volumeLight", "settailColour", csetfootColour, "vlight.settailColour(r, g, b, a)", 6, 6);
}
