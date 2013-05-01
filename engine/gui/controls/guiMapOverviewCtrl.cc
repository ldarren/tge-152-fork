//-----------------------------------------------------------------------------
// guiMapOverviewCtrl.cc
// Made by Thomas Larsen (StoneGroup)
//  
//-----------------------------------------------------------------------------
//
// Torque Engine
//
// Portions Copyright (c) 2001 by Sierra Online, Inc.
//
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleTypes.h"
#include "dgl/dgl.h"
#include "game/game.h"
#include "game/missionArea.h"
#include "game/gameConnection.h"
#include "./guiMapOverviewCtrl.h"


IMPLEMENT_CONOBJECT(GuiMapOverviewCtrl);

GuiMapOverviewCtrl::GuiMapOverviewCtrl(void)
{
	for (int i = 0; i < TR_MAX; i++)
	{
		mTrackVisible[i] = true;
		mTrackColor[i].set(1.00F,0.00F,0.00F,1.00F); // Red;
		mTrackMaxNum[i] = 1;
		mTrackNum[i] = 0;
		mTrackObj[i] = 0;
	}

	mDestination.set(0, 0, 0);

	mBounds.set(0, 0, 256, 256);

	mPointSize = 10.0;

	mMapAlpha = 1.0;

	mMapQuad = glGenLists(1);
}

GuiMapOverviewCtrl::~GuiMapOverviewCtrl(void)
{
	for (int i = 0; i < TR_MAX; i++)
	{
		if (mTrackObj[i]) delete [] mTrackObj[i];
		mTrackObj[i] = 0;
	}
}

void GuiMapOverviewCtrl::initPersistFields()
{
	Parent::initPersistFields();
	
	addGroup("TrackProperties");		
		addField("MapAlpha", TypeF32, Offset(mMapAlpha, GuiMapOverviewCtrl));
		addField("PointSize", TypeF32, Offset(mPointSize, GuiMapOverviewCtrl));
	endGroup("TrackProperties");		
	
	addGroup("TrackController");		
		addField("ControllerVisible", TypeBool, Offset(mTrackVisible[TR_CONTROLLER], GuiMapOverviewCtrl));
		addField("ControllerColor", TypeColorF, Offset(mTrackColor[TR_CONTROLLER], GuiMapOverviewCtrl));
	endGroup("TrackController");		

	addGroup("TrackPlayers");		
		addField("PlayersVisible", TypeBool, Offset(mTrackVisible[TR_PLAYERS], GuiMapOverviewCtrl));
		addField("PlayersColor", TypeColorF, Offset(mTrackColor[TR_PLAYERS], GuiMapOverviewCtrl));
		addField("PlayersMaxNum", TypeS32, Offset(mTrackMaxNum[TR_PLAYERS], GuiMapOverviewCtrl));
	endGroup("TrackPlayers");		

	addGroup("TrackAI");		
		addField("AIVisible", TypeBool, Offset(mTrackVisible[TR_AI], GuiMapOverviewCtrl));
		addField("AIColor", TypeColorF, Offset(mTrackColor[TR_AI], GuiMapOverviewCtrl));
		addField("AIMaxNum", TypeS32, Offset(mTrackMaxNum[TR_AI], GuiMapOverviewCtrl));
	endGroup("TrackAI");		

	addGroup("TrackVehicles");		
		addField("VehiclesVisible", TypeBool, Offset(mTrackVisible[TR_VEHICLES], GuiMapOverviewCtrl));
		addField("VehiclesColor", TypeColorF, Offset(mTrackColor[TR_VEHICLES], GuiMapOverviewCtrl));
		addField("VehiclesMaxNum", TypeS32, Offset(mTrackMaxNum[TR_VEHICLES], GuiMapOverviewCtrl));
	endGroup("TrackVehicles");		

	addGroup("TrackDestination");		
		addField("DestinationVisible", TypeBool, Offset(mTrackVisible[TR_DESTINATION], GuiMapOverviewCtrl));
		addField("DestinationColor", TypeColorF, Offset(mTrackColor[TR_DESTINATION], GuiMapOverviewCtrl));
		addField("Destination", TypePoint3F, Offset(mDestination, GuiMapOverviewCtrl));
	endGroup("TrackDestination");		
}

ConsoleMethod( GuiMapOverviewCtrl, updateMap, void, 2, 2, "(void)"
              "Recalculate map dimension and mission area")
{
   object->updateMap();
}

ConsoleMethod( GuiMapOverviewCtrl, updateTrackObj, void, 2, 2, "(void)"
              "Rescan for trackable objects")
{
   object->updateTrakingObjs();
}

void GuiMapOverviewCtrl::onRender(Point2I offset, const RectI &updateRect) 
{
	if (!mTextureHandle) return;

	if (mTrackNum[TR_CONTROLLER] == 0) updateTrakingObjs();

	const RectI &mArea = MissionArea::smMissionArea;

   	//Find distance from top-left corner to center of map image (absolute position)
	Point2F center(offset.x+mHWidth,offset.y+mHHeight); 

	glPushMatrix();
	glTranslatef(center.x, center.y, 0);  //  Center of Map Control
	glColor4f(1.0, 1.0, 1.0, mMapAlpha);  //  Background transparency control
	glEnable(GL_BLEND);

	// Draw Map
	glCallList(mMapQuad);

	glPushAttrib(GL_POINT_BIT);
	glEnable(GL_POINT_SMOOTH);

	F32 x, y;
	F32 FlashLevel;
	Point3F newCoord(0,0,0);
	for (int i = 0; i < TR_MAX; ++i)
	{
		if (!mTrackVisible[i]) continue;

		switch(i)
		{
		case TR_CONTROLLER:
			glPointSize(mPointSize);
		    mTrackColor[i].alpha = float(Platform::getRealMilliseconds() % 1000) / 1000.0;
			break;
		case TR_PLAYERS:
			glPointSize(mPointSize);
			break;
		case TR_AI:
			glPointSize(mPointSize);
			break;
		case TR_VEHICLES:
			glPointSize(mPointSize+1);
			break;
		case TR_DESTINATION:
			glPointSize(mPointSize+3);
		    mTrackColor[i].alpha = float(Platform::getRealMilliseconds() % 1000) / 1000.0;
			break;
		}

		glBegin(GL_POINTS);

		glColor4f(mTrackColor[i].red,mTrackColor[i].green,mTrackColor[i].blue,mTrackColor[i].alpha);

		for (int j = 0; j < mTrackNum[i]; ++j)
		{
            ShapeBase* shape = mTrackObj[i][j];

			// Get coords of object
			if (i != TR_DESTINATION) 
				newCoord.set(shape->getPosition());   			    
			else
				newCoord.set(mDestination.x, mDestination.y, mDestination.z);   			    

			bool InMissionArea = !( (newCoord.x < mArea.point.x) || (newCoord.x > mArea.point.x + mArea.extent.x) || 
				                    (newCoord.y < mArea.point.y) || (newCoord.y > mArea.point.y + mArea.extent.y) );

			if (InMissionArea) {
                F32 x = -(newCoord.x - (mArea.point.x+mArea.extent.x/2)) * mBounds.extent.x / mArea.extent.x;
                F32 y = (newCoord.y - (mArea.point.y+mArea.extent.y/2)) * mBounds.extent.y / mArea.extent.y;

				glVertex2f(x,y);
			}
		}

		glEnd();
	}

	glPopAttrib();		
	glDisable(GL_BLEND);

	glPopMatrix();  //  Undo glTranslate

	renderChildControls(offset, updateRect); 
}

bool GuiMapOverviewCtrl::onWake()
{
	if (! Parent::onWake()) return false;
	updateMap();
	updateTrakingObjs();

	return true;
}

void GuiMapOverviewCtrl::onSleep()
{
	Parent::onSleep();
}

void GuiMapOverviewCtrl::updateMap()
{
	mHWidth = mBounds.extent.x/2.0;
	mHHeight = mBounds.extent.y/2.0;

	if (mTextureHandle) {
		glNewList(mMapQuad,GL_COMPILE);
		glEnable(GL_TEXTURE_2D);

		TextureObject* texture = (TextureObject *) mTextureHandle; 
		glBindTexture(GL_TEXTURE_2D, texture->texGLName);		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);  glVertex2f(-mHWidth, -mHHeight);
			glTexCoord2f(1, 0);  glVertex2f( mHWidth, -mHHeight);
			glTexCoord2f(1, 1);  glVertex2f( mHWidth,  mHHeight);
			glTexCoord2f(0, 1);  glVertex2f(-mHWidth,  mHHeight);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glEndList();
	}

	return;
}

void GuiMapOverviewCtrl::updateTrakingObjs() 
{
	for (int i = 0; i < TR_MAX; i++)
	{
		if (mTrackObj[i]) delete [] mTrackObj[i];
		if (mTrackMaxNum[i] < 1) mTrackMaxNum[i] = 1;
		mTrackObj[i]	= new pShapeBase[mTrackMaxNum[i]];
		mTrackNum[i]	= 0;
	}
	mTrackNum[TR_DESTINATION]	= 1;

	// Must have a connection
    GameConnection* conn = GameConnection::getConnectionToServer();
    if (!conn) return;
    
	// Must have controlled object
    ShapeBase* control = conn->getControlObject();
    if (!control) return;
	
	for (SimSetIterator itr(conn); *itr; ++itr) {
		if ((*itr)->getType() & AIObjectType) {
			if (mTrackNum[TR_AI] > mTrackMaxNum[TR_AI]) continue;
			mTrackObj[TR_AI][mTrackNum[TR_AI]] = static_cast<ShapeBase*>(*itr);
			mTrackNum[TR_AI]++;
		}
		else if ((*itr)->getType() & PlayerObjectType) {
			ShapeBase* shape = static_cast<ShapeBase*>(*itr);

			// If this is the controlling object then save position for it now.
			if (shape == control) {
				if (mTrackNum[TR_CONTROLLER] > mTrackMaxNum[TR_CONTROLLER]) continue;
				mTrackObj[TR_CONTROLLER][mTrackNum[TR_CONTROLLER]] = shape;
				mTrackNum[TR_CONTROLLER]++;
			}
			else {
				if (mTrackNum[TR_PLAYERS] > mTrackMaxNum[TR_PLAYERS]) continue;
				mTrackObj[TR_PLAYERS][mTrackNum[TR_PLAYERS]] = shape;
				mTrackNum[TR_PLAYERS]++;
			}
		}
		else if ((*itr)->getType() & VehicleObjectType) {
			ShapeBase* shape = static_cast<ShapeBase*>(*itr);

			// If this is the controlling object then save position for it now.
			if (shape == control) {
				if (mTrackNum[TR_CONTROLLER] > mTrackMaxNum[TR_CONTROLLER]) continue;
				mTrackObj[TR_CONTROLLER][mTrackNum[TR_CONTROLLER]] = shape;
				mTrackNum[TR_CONTROLLER]++;
			}
			else {
				if (mTrackNum[TR_VEHICLES] > mTrackMaxNum[TR_VEHICLES]) continue;
				mTrackObj[TR_VEHICLES][mTrackNum[TR_VEHICLES]] = shape;
				mTrackNum[TR_VEHICLES]++;
			}
		}
	}
}
