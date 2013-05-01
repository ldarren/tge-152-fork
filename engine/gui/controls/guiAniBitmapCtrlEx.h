#ifndef _GUIANIBITMAPCTRLEX_H_
#define _GUIANIBITMAPCTRLEX_H_

#ifndef _GUICONTROL_H_
#include <gui/core/guiControl.h>
#endif
#ifndef _GTEXMANAGER_H_
#include <dgl/gTexManager.h>
#endif

class GuiAniBitmapCtrlEx : public GuiControl
{
private:
	typedef GuiControl Parent;

protected:
	static bool setFrameNum( void *obj, const char *data );
	static bool setBitmapName( void *obj, const char *data );

	StringTableEntry mBitmapName;
	Point2I startPoint;
	bool mWrap;

	TextureHandle * pTextureHandleList; 
	S32 mCurFrame;
	S32 mLastTime;
	bool mLoop;

	S32	mFrameNum;
	S32	mPeriod;

public:
	DECLARE_CONOBJECT(GuiAniBitmapCtrlEx);
	static void initPersistFields();
	GuiAniBitmapCtrlEx();

	bool onWake();
	void onSleep();
	void inspectPostApply();

	void loadImageList();
	void removeImageList();

	void setBitmap(const char *name, bool resize = false);
	void setBitmap(const TextureHandle &handle, bool resize = false);

	S32 getWidth() const       { return(pTextureHandleList ? pTextureHandleList[0].getWidth() : 0); }
	S32 getHeight() const      { return(pTextureHandleList ? pTextureHandleList[0].getHeight(): 0); }

	void onRender(Point2I offset, const RectI &updateRect);
	void setValue(S32 x, S32 y);
};

#endif // _GUIANIBITMAPCTRLEX_H_