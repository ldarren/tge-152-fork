#ifndef _GUIANIBITMAPCTRL_H_
#define _GUIANIBITMAPCTRL_H_

#ifndef _GUIBITMAPCTRL_H_
#include "./guiBitmapCtrl.h"
#endif
#ifndef _GTEXMANAGER_H_
#include "dgl/gTexManager.h"
#endif

class GuiAniBitmapCtrl : public GuiBitmapCtrl
{
private:
	typedef GuiBitmapCtrl Parent;

protected:
	S32 mCurFrame;
	S32 mLastTime;
	bool mLoop;

	S32	mRowDiv, mColDiv;
	S32	mFrameNum;
	S32	mPeriod;

public:
	DECLARE_CONOBJECT(GuiAniBitmapCtrl);
	static void initPersistFields();
	GuiAniBitmapCtrl();

	void onRender(Point2I offset, const RectI &updateRect);
	void setFrame(S32  frame);
};

#endif // _GUIANIBITMAPCTRL_H_