#include <console/console.h>
#include <console/consoleTypes.h>
#include <dgl/dgl.h>
#include <string.h>
#include <stdlib.h>

#include "./guiAniBitmapCtrlEx.h"

IMPLEMENT_CONOBJECT(GuiAniBitmapCtrlEx);

bool GuiAniBitmapCtrlEx::setFrameNum( void *obj, const char *data )
{
   // Prior to this, you couldn't do bitmap.bitmap = "foo.jpg" and have it work.
   // With protected console types you can now call the setBitmap function and
   // make it load the image.
   static_cast<GuiAniBitmapCtrlEx *>( obj )->removeImageList();
   static_cast<GuiAniBitmapCtrlEx *>( obj )->mFrameNum = atoi(data);
   static_cast<GuiAniBitmapCtrlEx *>( obj )->loadImageList();

   // Return false because the setBitmap method will assign 'mBitmapName' to the
   // argument we are specifying in the call.
   return false;
}

bool GuiAniBitmapCtrlEx::setBitmapName( void *obj, const char *data )
{
   // Prior to this, you couldn't do bitmap.bitmap = "foo.jpg" and have it work.
   // With protected console types you can now call the setBitmap function and
   // make it load the image.
   static_cast<GuiAniBitmapCtrlEx *>( obj )->setBitmap( data );

   // Return false because the setBitmap method will assign 'mBitmapName' to the
   // argument we are specifying in the call.
   return false;
}

void GuiAniBitmapCtrlEx::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Animation");		// MM: Added Group Header.

   addProtectedField( "bitmap", TypeFilename, Offset( mBitmapName, GuiAniBitmapCtrlEx ), &setBitmapName, &defaultProtectedGetFn, "base path of image list, e.g. image_xxx -> image" );
   addProtectedField( "frameNum",TypeS32,		Offset(mFrameNum,	GuiAniBitmapCtrlEx), &setFrameNum, &defaultProtectedGetFn, "Number of animation frames, min 1" );
   addField( "period",	TypeS32,		Offset(mPeriod,		GuiAniBitmapCtrlEx));
   addField( "loop",	TypeBool,		Offset(mLoop,		GuiAniBitmapCtrlEx));
   addField("wrap",   TypeBool,     Offset(mWrap,       GuiAniBitmapCtrlEx));
 
   endGroup("Animation");		// MM: Added Group Footer.
}

GuiAniBitmapCtrlEx::GuiAniBitmapCtrlEx()
:	mFrameNum(1),
	mPeriod(20),
	mLoop(false),
	mWrap(false),
	mCurFrame(0),
	mLastTime(Sim::getCurrentTime()),
	pTextureHandleList(0)
{
	mBitmapName = StringTable->insert("");
	startPoint.set(0, 0);
}

//-------------------------------------
void GuiAniBitmapCtrlEx::inspectPostApply()
{
	// if the extent is set to (0,0) in the gui editor and appy hit, this control will
	// set it's extent to be exactly the size of the bitmap (if present)
	Parent::inspectPostApply();

	if (!mWrap && (mBounds.extent.x == 0) && (mBounds.extent.y == 0) && pTextureHandleList)
	{
		TextureObject *texture = (TextureObject *) pTextureHandleList[0];
		mBounds.extent.x = texture->bitmapWidth;
		mBounds.extent.y = texture->bitmapHeight;
	}
}

bool GuiAniBitmapCtrlEx::onWake()
{
   if (! Parent::onWake())
      return false;
   setActive(true);
   setBitmap(mBitmapName);
   return true;
}

void GuiAniBitmapCtrlEx::onSleep()
{
	removeImageList();
	Parent::onSleep();
}

void GuiAniBitmapCtrlEx::loadImageList()
{
	// if more than 128 frames, avi control is more suitable
	if (!strcmp(mBitmapName, "") || mFrameNum < 1 || mFrameNum > 128) return;

	if (pTextureHandleList) removeImageList();

	pTextureHandleList = new TextureHandle[mFrameNum];
	
	char listName[266] = "";
	StringTableEntry listBitmap;
	TextureHandle listTexture;


	for (int i = 0; i < mFrameNum; i++)
	{
		sprintf(listName, "%s_%02d", mBitmapName, i);
		listBitmap = StringTable->insert(listName);
		if (*listBitmap) 
		{
			listTexture = TextureHandle(listBitmap, BitmapTexture, true);
			if (listTexture) pTextureHandleList[i] = listTexture;
			else pTextureHandleList[i] = 0;
		}
	}
}

void GuiAniBitmapCtrlEx::removeImageList()
{
	if (!(pTextureHandleList && pTextureHandleList[0])) return;
	for (int i = 0; i < mFrameNum; i++)
	{
		pTextureHandleList[i] = 0;
	}

	delete []pTextureHandleList;
	pTextureHandleList = 0;
}

void GuiAniBitmapCtrlEx::setBitmap(const char *name, bool resize)
{
	removeImageList();
	mBitmapName = StringTable->insert(name);
	loadImageList();

	// Resize the control to fit the bitmap
	if (pTextureHandleList && pTextureHandleList[0] && resize) {
		TextureObject* texture = (TextureObject *) pTextureHandleList[0];
		mBounds.extent.x = texture->bitmapWidth;
		mBounds.extent.y = texture->bitmapHeight;
		Point2I extent = getParent()->getExtent();
		parentResized(extent,extent);
	}
	
	setUpdate();
}

void GuiAniBitmapCtrlEx::setBitmap(const TextureHandle &handle, bool resize)
{
	TextureHandle * tempList = new TextureHandle[mFrameNum+1];
	for (int i = 0; i < mFrameNum; i++)
	{
		tempList[i] = pTextureHandleList[i];
	}
	tempList[mFrameNum] = handle;
	mFrameNum++;
	delete []pTextureHandleList;
	pTextureHandleList = tempList;

   // Resize the control to fit the bitmap
   if (pTextureHandleList && pTextureHandleList[0] && resize) {
      TextureObject* texture = (TextureObject *) pTextureHandleList[0];
      mBounds.extent.x = texture->bitmapWidth;
      mBounds.extent.y = texture->bitmapHeight;
      Point2I extent = getParent()->getExtent();
      parentResized(extent,extent);
   }
}

void GuiAniBitmapCtrlEx::onRender(Point2I offset, const RectI &updateRect)
{
	if (pTextureHandleList && pTextureHandleList[0])
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

		dglClearBitmapModulation();
		if(mWrap)
		{
			TextureObject* texture = (TextureObject *) pTextureHandleList[mCurFrame];
			RectI srcRegion;
			RectI dstRegion;
			float xdone = ((float)mBounds.extent.x/(float)texture->bitmapWidth)+1;
			float ydone = ((float)mBounds.extent.y/(float)texture->bitmapHeight)+1;

			int xshift = startPoint.x%texture->bitmapWidth;
			int yshift = startPoint.y%texture->bitmapHeight;
			for(int y = 0; y < ydone; ++y)
				for(int x = 0; x < xdone; ++x)
				{
					srcRegion.set(0,0,texture->bitmapWidth,texture->bitmapHeight);
					dstRegion.set( ((texture->bitmapWidth*x)+offset.x)-xshift,
					((texture->bitmapHeight*y)+offset.y)-yshift,
					texture->bitmapWidth,	
					texture->bitmapHeight);
					dglDrawBitmapStretchSR(texture,dstRegion, srcRegion, false);
				}
		}
		else
		{        
			RectI rect(offset, mBounds.extent);
			dglDrawBitmapStretch(pTextureHandleList[mCurFrame], rect);
		}
	}

	if (mProfile->mBorder || !(pTextureHandleList && pTextureHandleList[0]))
	{
		RectI rect(offset.x, offset.y, mBounds.extent.x, mBounds.extent.y);
		dglDrawRect(rect, mProfile->mBorderColor);
	}

	renderChildControls(offset, updateRect);
}

void GuiAniBitmapCtrlEx::setValue(S32 x, S32 y)
{
	if (pTextureHandleList && pTextureHandleList[0])
	{
		TextureObject* texture = (TextureObject *) pTextureHandleList[0];
		x+=texture->bitmapWidth/2;
		y+=texture->bitmapHeight/2;
	}
	while (x < 0) x += 256;
	startPoint.x = x % 256;
			
	while (y < 0) y += 256;
	startPoint.y = y % 256;
}
