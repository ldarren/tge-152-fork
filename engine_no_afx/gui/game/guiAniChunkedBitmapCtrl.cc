//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleTypes.h"
#include "dgl/dgl.h"
#include "dgl/gBitmap.h"
#include "gui/core/guiControl.h"
#include "dgl/gTexManager.h"
#include "dgl/gChunkedTexManager.h"

#include <string.h>
#include <stdlib.h>

class GuiAniChunkedBitmapCtrl : public GuiControl
{
private:
	typedef GuiControl Parent;
	void renderRegion(const Point2I &offset, const Point2I &extent);

protected:
	static bool setFrameNum( void *obj, const char *data );
	static bool setBitmapName( void *obj, const char *data );

	StringTableEntry mBitmapName;
	ChunkedTextureHandle * pTextureHandleList; 
	S32 mCurFrame;
	S32 mLastTime;
	S32	mFrameNum;
	S32	mPeriod;
	bool mLoop;

	bool  mUseVariable;
	bool  mTile;

public:
	//creation methods
	DECLARE_CONOBJECT(GuiAniChunkedBitmapCtrl);
	GuiAniChunkedBitmapCtrl();
	static void initPersistFields();

	//Parental methods
	bool onWake();
	void onSleep();

	void loadImageList();
	void removeImageList();

	void setBitmap(const char *name);

	void onRender(Point2I offset, const RectI &updateRect);
};

IMPLEMENT_CONOBJECT(GuiAniChunkedBitmapCtrl);

bool GuiAniChunkedBitmapCtrl::setFrameNum( void *obj, const char *data )
{
   // Prior to this, you couldn't do bitmap.bitmap = "foo.jpg" and have it work.
   // With protected console types you can now call the setBitmap function and
   // make it load the image.
   static_cast<GuiAniChunkedBitmapCtrl *>( obj )->removeImageList();
   static_cast<GuiAniChunkedBitmapCtrl *>( obj )->mFrameNum = atoi(data);
   static_cast<GuiAniChunkedBitmapCtrl *>( obj )->loadImageList();

   // Return false because the setBitmap method will assign 'mBitmapName' to the
   // argument we are specifying in the call.
   return false;
}

bool GuiAniChunkedBitmapCtrl::setBitmapName( void *obj, const char *data )
{
   // Prior to this, you couldn't do bitmap.bitmap = "foo.jpg" and have it work.
   // With protected console types you can now call the setBitmap function and
   // make it load the image.
   static_cast<GuiAniChunkedBitmapCtrl *>( obj )->setBitmap( data );

   // Return false because the setBitmap method will assign 'mBitmapName' to the
   // argument we are specifying in the call.
   return false;
}

void GuiAniChunkedBitmapCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Misc");		

   addProtectedField( "bitmap", TypeFilename, Offset( mBitmapName, GuiAniChunkedBitmapCtrl ), &setBitmapName, &defaultProtectedGetFn, "base path of image list, e.g. image_xxx -> image" );
   addProtectedField( "frameNum",TypeS32,		Offset(mFrameNum,	GuiAniChunkedBitmapCtrl), &setFrameNum, &defaultProtectedGetFn, "Number of animation frames, min 1" );
   addField( "period",	TypeS32,		Offset(mPeriod,		GuiAniChunkedBitmapCtrl));
   addField( "loop",	TypeBool,		Offset(mLoop,		GuiAniChunkedBitmapCtrl));
   addField( "useVariable",   TypeBool,      Offset( mUseVariable, GuiAniChunkedBitmapCtrl ) );
   addField( "tile",          TypeBool,      Offset( mTile, GuiAniChunkedBitmapCtrl ) );

   endGroup("Misc");		
}

ConsoleMethod( GuiAniChunkedBitmapCtrl, setBitmap, void, 3, 3, "(string filename)"
              "Set the bitmap contained in this control.")
{
   object->setBitmap( argv[2] );
}

GuiAniChunkedBitmapCtrl::GuiAniChunkedBitmapCtrl()
:	mFrameNum(1),
	mPeriod(20),
	mLoop(false),
	mCurFrame(0),
	mLastTime(Sim::getCurrentTime()),
	pTextureHandleList(0),
	mUseVariable(false),
	mTile(false)
{
	mBitmapName = StringTable->insert("");
}

void GuiAniChunkedBitmapCtrl::setBitmap(const char *name)
{
   bool awake = mAwake;
   if(awake)
      onSleep();

   mBitmapName = StringTable->insert(name);
   if(awake)
      onWake();
   setUpdate();
}

bool GuiAniChunkedBitmapCtrl::onWake()
{
	if(!Parent::onWake())
		return false;

	loadImageList();

	return true;
}

void GuiAniChunkedBitmapCtrl::onSleep()
{
	removeImageList();
	Parent::onSleep();
}

void GuiAniChunkedBitmapCtrl::loadImageList()
{
	StringTableEntry baseName;
	if ( mUseVariable )
		baseName = StringTable->insert(getVariable());
	else
		baseName = mBitmapName;

	// if more than 128 frames, avi control is more suitable
	if (!strcmp(baseName, "") || mFrameNum < 1 || mFrameNum > 128) return;

	if (pTextureHandleList) removeImageList();

	pTextureHandleList = new ChunkedTextureHandle[mFrameNum];
	
	char listName[266] = "";
	StringTableEntry listBitmap;
	ChunkedTextureHandle listTexture;

	for (int i = 0; i < mFrameNum; i++)
	{
		sprintf(listName, "%s_%02d", mBitmapName, i);
		listBitmap = StringTable->insert(listName);
		if (*listBitmap) 
		{
			listTexture = ChunkedTextureHandle(listBitmap);
			if (listTexture) pTextureHandleList[i] = listTexture;
			else pTextureHandleList[i] = 0;
		}
	}
}

void GuiAniChunkedBitmapCtrl::removeImageList()
{
	if (!(pTextureHandleList && pTextureHandleList[0])) return;
	for (int i = 0; i < mFrameNum; i++)
	{
		pTextureHandleList[i] = 0;
	}

	delete []pTextureHandleList;
	pTextureHandleList = 0;
}

void GuiAniChunkedBitmapCtrl::renderRegion(const Point2I &offset, const Point2I &extent)
{
	if(mLoop)
	{
		S32 thisTime = Sim::getCurrentTime();

		S32 timeDelta = (thisTime - mLastTime)/10;

		if (timeDelta > mPeriod)
		{
			mLastTime = Sim::getCurrentTime();
			mCurFrame ++;
			if (mCurFrame >= mFrameNum)
			{
				mCurFrame = 0;
			}
		}
	}

	U32 widthCount = pTextureHandleList[mCurFrame].getTextureCountWidth();
	U32 heightCount = pTextureHandleList[mCurFrame].getTextureCountHeight();
	if(!widthCount || !heightCount)
	  return;

	F32 widthScale = F32(extent.x) / F32(pTextureHandleList[mCurFrame].getWidth());
	F32 heightScale = F32(extent.y) / F32(pTextureHandleList[mCurFrame].getHeight());
	dglSetBitmapModulation(ColorF(1,1,1));
	for(U32 i = 0; i < widthCount; i++)
	{
	  for(U32 j = 0; j < heightCount; j++)
	  {
		 TextureHandle t = pTextureHandleList[mCurFrame].getSubTexture(i, j);
		 RectI stretchRegion;
		 stretchRegion.point.x = (S32)(i * 256 * widthScale  + offset.x);
		 stretchRegion.point.y = (S32)(j * 256 * heightScale + offset.y);
		 if(i == widthCount - 1)
			stretchRegion.extent.x = extent.x + offset.x - stretchRegion.point.x;
		 else
			stretchRegion.extent.x = (S32)((i * 256 + t.getWidth() ) * widthScale  + offset.x - stretchRegion.point.x);
		 if(j == heightCount - 1)
			stretchRegion.extent.y = extent.y + offset.y - stretchRegion.point.y;
		 else
			stretchRegion.extent.y = (S32)((j * 256 + t.getHeight()) * heightScale + offset.y - stretchRegion.point.y);
		 dglDrawBitmapStretch(t, stretchRegion);
	  }
	}
}


void GuiAniChunkedBitmapCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   if(pTextureHandleList && pTextureHandleList[0])
   {
      if (mTile)
      {
         int stepy = 0;
         for(int y = 0; offset.y + stepy < mBounds.extent.y; stepy += pTextureHandleList[0].getHeight())
         {
            int stepx = 0;
            for(int x = 0; offset.x + stepx < mBounds.extent.x; stepx += pTextureHandleList[0].getWidth())
               renderRegion(Point2I(offset.x+stepx, offset.y+stepy), Point2I(pTextureHandleList[0].getWidth(), pTextureHandleList[0].getHeight()) );
         }
      }
      else
         renderRegion(offset, mBounds.extent);

      renderChildControls(offset, updateRect);
   }
   else
      Parent::onRender(offset, updateRect);
}
