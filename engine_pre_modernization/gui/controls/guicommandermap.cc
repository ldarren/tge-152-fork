//-----------------------------------------------------------------------------
//Edited by Dreamer
// This started life as CommanderMapHud by Ben Garney
// It has been ported to TGE 1.4 and had several community mods introduced
//
//-----------------------------------------------------------------------------
// Commander Map HUD
//
// Portions Copyright (c) GarageGames.Com
// Copyright (c) Ben Garney
//-----------------------------------------------------------------------------
// These includes are probably overkill -- BJG

#include "terrain/terrRender.h"
#include "terrain/waterBlock.h"
#include "sceneGraph/sceneGraph.h"
#include "gui/core/guiTSControl.h"
#include "game/gameConnection.h"
#include "core/frameAllocator.h"
#include "platform/profiler.h"

//-----------------------------------------------------------------------------

#define MIN_MAPHUD_ZOOM 0.01
#define MAX_MAPHUD_ZOOM 1.75

class GuiMapHud : public GuiTSCtrl
{
private:
	typedef GuiTSCtrl Parent;
	F32		mPanSpeed;
	F32		mZoomSpeed;
	S32		mLastRenderTime;
	
public:		
	Point2F mPanGoal, mCurPan;
	F32     mZoomGoal, mCurZoom;
	
	GuiMapHud();
	bool processCameraQuery(CameraQuery *query);
	void renderWorld(const RectI &updateRect);
	void onRender( Point2I, const RectI &);
	static void initPersistFields();
	bool onMouseWheelUp(const GuiEvent &event);
	bool onMouseWheelDown(const GuiEvent &event);
	DECLARE_CONOBJECT( GuiMapHud );
};
	
//-----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT( GuiMapHud );

GuiMapHud::GuiMapHud()
:	mPanSpeed(2.5),
	mZoomSpeed(1),
	mCurPan(0,0),
	mCurZoom(M_PI_F/2),
	mPanGoal(0,0),
	mZoomGoal(M_PI_F/2),
	mLastRenderTime(0)
{
}

void GuiMapHud::initPersistFields()
{   
	Parent::initPersistFields();   
	addField("panSpeed",  TypeF32, Offset(mPanSpeed,  GuiMapHud), "Set the speed we pan to our goal.");   
	addField("zoomSpeed", TypeF32,     Offset(mZoomSpeed, GuiMapHud), "Set the speed we zoom with to our goal.");
}

bool GuiMapHud::processCameraQuery(CameraQuery *q)
{   
	// Scale ranges based on the highest/lowest point in the terrain   
	F32 maxHi = gClientSceneGraph->getCurrentTerrain()->findSquare(8, 0,0)->maxHeight / 10;   
	F32 minHi = gClientSceneGraph->getCurrentTerrain()->findSquare(8, 0,0)->minHeight / 10;   
	q->object = NULL;   
	q->nearPlane = 1;   
	q->farPlane  = mFabs(maxHi) + mFabs(minHi);   
	q->fov       = mCurZoom;
	//q->ortho	 = true;

	GameConnection* conn = GameConnection::getConnectionToServer();   
	if (!conn)      return false;  

	MatrixF playercam, mat1, mat2;   
	conn->getControlCameraTransform(0, &playercam); 
	// store camera information   
	// Get camera angle   
	AngAxisF aa(playercam);   
	aa.axis.x = 0.0f;   
	aa.axis.y = 0.0f;   
	aa.setMatrix(&mat1);   
	// Create angle to look straight down   
	aa.axis.x = 1.0f;   
	aa.axis.y = 0.0f;   
	aa.axis.z = 0.0f;   
	aa.angle = 3.14/2;   
	aa.setMatrix(&mat2);   
	// Combine the camera angles   
	mat1.mul(mat2);   
	// Make sure we're high enough that we we won't clip   
	mat1.setColumn(3, Point3F(mCurPan.x, mCurPan.y, maxHi-100));  // adjust camera height 
	q->cameraMatrix = mat1;   
	return true;
}

void GuiMapHud::renderWorld(const RectI &updateRect)
{   
	// Set up state   
	TerrainRender::mRenderingCommander = true;   
	F32 oldVisDist = gClientSceneGraph->getVisibleDistance();   
	gClientSceneGraph->setVisibleDistance(2000);   
	F32 oldFogDist = gClientSceneGraph->getFogDistance();   
	gClientSceneGraph->setFogDistance(2000);   
	// set up the camera and viewport stuff:   
	// Render (stolen from GameRenderWorld)   
	PROFILE_START(GameRenderCommanderWorld);   
	FrameAllocator::setWaterMark(0);
#if defined(GATHER_METRICS) && GATHER_METRICS > 1   
	TextureManager::smTextureCacheMisses = 0;
#endif   
	glEnable(GL_DEPTH_TEST);   
	glDepthFunc(GL_LEQUAL);   
	glClear(GL_DEPTH_BUFFER_BIT);   
	glDisable(GL_CULL_FACE);   
	glMatrixMode(GL_MODELVIEW);   
	dglSetCanonicalState();   
	// If you want to render other things, change this mask.   
	gClientSceneGraph->renderScene(    EnvironmentObjectType | TerrainObjectType | InteriorObjectType | WaterObjectType | PlayerObjectType | ShapeBaseObjectType | StaticShapeObjectType | AIObjectType | StaticTSObjectType | StaticRenderedObjectType);   
	glDisable(GL_DEPTH_TEST);
#if defined(GATHER_METRICS) && GATHER_METRICS > 1   
	Con::setFloatVariable("Video::texResidentPercentage", TextureManager::getResidentFraction());   
	Con::setIntVariable("Video::textureCacheMisses", TextureManager::smTextureCacheMisses);
#endif   
	AssertFatal(FrameAllocator::getWaterMark() == 0, "Error, someone didn't reset the water mark on the frame allocator!");   
	FrameAllocator::setWaterMark(0);   
	PROFILE_END();   
	// Restore state   
	gClientSceneGraph->setVisibleDistance(oldVisDist);   
	gClientSceneGraph->setFogDistance    (oldFogDist);   
	TerrainRender::mRenderingCommander = false;   
	dglSetClipRect(updateRect);
}

void GuiMapHud::onRender(Point2I offset, const RectI &updateRect)
{   
	// Update pan/zoom   
	S32 time = Platform::getVirtualMilliseconds();   
	S32 dt = time - mLastRenderTime;   
	mLastRenderTime = time;   
	mCurPan  += (mPanGoal  - mCurPan)  * (F32)dt/1000.f;   
	mCurZoom += (mZoomGoal - mCurZoom) * ((F32)(dt/1000.f) * mZoomSpeed);   
	// Render the world...   
	Parent::onRender(offset, updateRect);   
	// If you wanted to render custom GUI elements, like a sensor map, icons for   
	// players/vehicles/objectives, you would do it here by calling project()  
	// for all their positions and drawing bitmaps at the appropriate locations.
}

bool GuiMapHud::onMouseWheelUp(const GuiEvent &event)
{   
	F32 delta = (mZoomGoal/16) * mZoomSpeed;   
	// Don't let the player zoom so far in that they can't   
	// tell what's they are looking at.   
	if (mZoomGoal - delta >= MIN_MAPHUD_ZOOM)       
	mZoomGoal -= delta;   
	return( true );
}

bool GuiMapHud::onMouseWheelDown(const GuiEvent &event)
{   
	F32 delta = (mZoomGoal/16) * mZoomSpeed;   
	// Don't let the player zoom so far out that the land doesn't   
	// fill the whole commander map   
	if (mZoomGoal + delta <= MAX_MAPHUD_ZOOM)      
	mZoomGoal += delta;   
	return( true );
}

ConsoleMethod(GuiMapHud, pan, void, 4, 4, "(x, y) Cut to a location.")
{   
	object->mPanGoal.set(dAtof(argv[2]), dAtof(argv[3]));   
	object->mCurPan.set (dAtof(argv[2]), dAtof(argv[3]));
}

ConsoleMethod(GuiMapHud, panTo, void, 4, 4, "(x, y) Smoothly pan to a location.")
{   
	object->mPanGoal.set(dAtof(argv[2]), dAtof(argv[3]));
}

ConsoleMethod(GuiMapHud, zoom, void, 3, 3, "(val) Zoom to a specified level.")
{   
	object->mZoomGoal = dAtof(argv[2]);   
	object->mCurZoom  = dAtof(argv[2]);
}

ConsoleMethod(GuiMapHud, zoomTo, void, 3, 3, "(val) Smoothly zoom to a specified level.")
{   
	object->mZoomGoal = dAtof(argv[2]);
}

ConsoleMethod(GuiMapHud, zoomToArea, void, 6, 7, "(top, left, right, bottom, bool cut) Smoothly zoom to view the specified area. If cut is set, we jump there.")
{   
	// Parse arguments   
	F32 top, left, right, bottom;   
	top    = dAtof(argv[2]);   
	left   = dAtof(argv[3]);   
	right  = dAtof(argv[4]);   
	bottom = dAtof(argv[5]);   
	// Figure out the center of the area   
	Point2F center;   
	center.x = (left + right) * 0.5f;   
	center.y = (top + bottom) * 0.5f;   
	object->mZoomGoal = mFabs(left - right) / 200; 
	// Cheesy scaling fakery.   
	// And set our motion   
	object->mPanGoal = center;   
	// Cut if requested   
	if(argc > 6)      
		if(dAtob(argv[6]))      
		{         
			object->mCurPan  = object->mPanGoal;         
			object->mCurZoom = object->mZoomGoal;      
		}
} 