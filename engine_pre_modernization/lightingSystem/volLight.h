/////////////////////////////////////////////////////////////////
// Parashar Krishnamachari
// Volume Light class for integration into Torque Game Engine
/////////////////////////////////////////////////////////////////

#ifndef _VOLLIGHT_H_
#define _VOLLIGHT_H_

#ifndef _SCENEOBJECT_H_
#include "sim/sceneObject.h"
#endif


#include "lightingSystem/sgLightObject.h"


class volumeLight : public sgLightObject
{
private:
	typedef sgLightObject Parent;
public:
	void buildObjectBox()
	{
		mObjBox.min.set(-mXextent, -mYextent, -mShootDistance);
		mObjBox.max.set(mXextent, mYextent, mShootDistance);
		resetWorldBox();
	}
	void sgRenderX(const Point3F &near1, const Point3F &far1, const Point3F &far2,
						   const ColorF &nearcol, const ColorF &farcol, F32 offset);
	void sgRenderY(const Point3F &near1, const Point3F &far1, const Point3F &far2,
						   const ColorF &nearcol, const ColorF &farcol, F32 offset);


protected :
	enum {	volLightMask		= (1 << 0),
			volLightOther		= (1 << 1) };

	bool mAddedToScene;

	S32 mLastRenderTime;

	TextureHandle	mLightHandle;		// Light beam texture used.

public :
	volumeLight();
	~volumeLight();

	// SceneObject functions
	void renderObject(SceneState*, SceneRenderImage*);
	void renderGL(SceneState*, SceneRenderImage*);
	virtual bool prepRenderImage(SceneState*, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState = false);

	// SimObject functions
	bool onAdd();
	void onRemove();
	void inspectPostApply();

	// NetObject functions
	U32 packUpdate(NetConnection *, U32, BitStream *);
	void unpackUpdate(NetConnection *, BitStream *);

	// ConObject functions
	static void consoleInit();
	static void initPersistFields();

	void setLtexture(const char *name);

	void setlpDistance(F32 Dist);
	void setShootDistance(F32 Dist);
	void setXextent(F32 extent);
	void setYextent(F32 extent);

	void setSubdivideU(U32 val);
	void setSubdivideV(U32 val);

	void setfootColour(ColorF col);
	void settailColour(ColorF col);

	DECLARE_CONOBJECT(volumeLight);
	

	StringTableEntry mLTextureName;		// Filename for light texture

	F32  mlpDistance;		// Distance to hypothetical lightsource point -- affects fov angle
	F32  mShootDistance;	// Distance of shooting -- Length of beams
	F32  mXextent;			// xExtent and yExtent determine the size/dimension of the plane
	F32  mYextent;

	U32  mSubdivideU;		// Number of subdivisions in U and V space.
	U32  mSubdivideV;		// Controls the number of "slices" in the volume.
	// NOTE : Total number of polygons = 2 + ((mSubdiveU + 1) + (mSubdivideV + 1)) * 2
	// Each slice being a quad plus the rectangular plane at the bottom.

	ColorF mfootColour;		// Color at the source
	ColorF mtailColour;		// Color at the end.

	// For normal operation, we'll just have white with alpha 0.2 at the source
	// and white with alpha 0 at the ends.  Moreover, we can have individual color in
	// the texture, but this will allow us to affect things ever so slightly and
	// tint the beams a little bit.
};

#endif	// _VOLLIGHT_H_