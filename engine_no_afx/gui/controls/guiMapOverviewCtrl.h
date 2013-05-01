//-----------------------------------------------------------------------------
// guiMapOverviewCtrl.cc
// Made by Thomas Larsen (Stonegroup)
//  
//-----------------------------------------------------------------------------
//
// Torque Engine
//
// Portions Copyright (c) 2001 by Sierra Online, Inc.
//
//-----------------------------------------------------------------------------

#ifndef _GuiMapOverviewCtrl_H_
#define _GuiMapOverviewCtrl_H_

#ifndef _GUIBITMAPCTRL_H_
#include "./guiBitmapCtrl.h"
#endif

#ifndef _SCENEOBJECT_H_
#include "sim/sceneObject.h"
#endif

#ifndef _PLATFORMGL_H_

#include "platform/platformAssert.h"
#if defined(TORQUE_OS_MAC)
#include "platformMacCarb/platformGL.h"
#elif defined(TORQUE_OS_WIN32)
#include "platformWin32/platformGL.h"
#elif defined(TORQUE_OS_LINUX) || defined(TORQUE_OS_OPENBSD) || defined(TORQUE_OS_FREEBSD)
#include "platformX86UNIX/platformGL.h"
#endif

#endif

class GuiMapOverviewCtrl : public GuiBitmapCtrl
{
	enum TrackObjType
	{
		TR_DESTINATION = 0,
		TR_AI,
		TR_VEHICLES,
		TR_PLAYERS,
		TR_CONTROLLER,
		TR_MAX,
	};

	typedef ShapeBase* pShapeBase;

private:
	typedef GuiBitmapCtrl Parent;

	bool mTrackVisible[TR_MAX];
	
	ColorF   mTrackColor[TR_MAX];

	S32	mTrackMaxNum[TR_MAX];

	S32 mTrackNum[TR_MAX];

	pShapeBase* mTrackObj[TR_MAX];
	Point3F mDestination;

	F32 mPointSize;

	F32 mMapAlpha;

	F32 mHWidth;
	F32 mHHeight;

	GLuint mMapQuad;
   
public:
	//creation methods
	DECLARE_CONOBJECT(GuiMapOverviewCtrl);
	GuiMapOverviewCtrl();
	virtual ~GuiMapOverviewCtrl(void);
	static void initPersistFields();

	//Parental methods    
	virtual void onRender(Point2I offset, const RectI &updateRect);
	virtual bool onWake();
	virtual void onSleep();
	void updateMap();
	void updateTrakingObjs();
};

#endif //_GuiMapOverviewCtrl_H_