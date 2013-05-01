//-----------------------------------------------------------------------------
// Torque Game Engine
// 
// Written by Melvyn May, 9th September 2002.
//-----------------------------------------------------------------------------

#ifndef _FXRENDEROBJECT_H_
#define _FXRENDEROBJECT_H_

#ifndef _SCENEOBJECT_H_
#include "sim/sceneObject.h"
#endif


//------------------------------------------------------------------------------
// Class: fxRenderObject
//------------------------------------------------------------------------------
class fxRenderObject : public SceneObject
{
private:
	typedef SceneObject		Parent;

protected:

	// Create and use these to specify custom events.
	//
	// NOTE:-	Using these allows you to group the changes into related
	//			events.  No need to change everything if something minor
	//			changes.  Only really important if you have *lots* of these
	//			objects at start-up or you send alot of changes whilst the
	//			game is in progress.
	//
	//			Use "setMaskBits(fxRenderObjectMask)" to signal.

	enum {	fxRenderObjectMask		= (1 << 0),
			fxRenderObjectAnother	= (1 << 1) };

	S32								mLastRenderTime;
	TextureHandle					mTextureHandle;
	F32								mCurrentAngle;

	// Fields.
	F32								mQuadSize;
	F32								mQuadRotateSpeed;
    StringTableEntry				mTextureName;
    ColorI                    mObjColor;



public:
	fxRenderObject();
	~fxRenderObject();

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

	// Declare Console Object.
	DECLARE_CONOBJECT(fxRenderObject);
};

#endif // _FXRENDEROBJECT_H_
