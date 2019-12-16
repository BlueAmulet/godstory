

#ifndef _GUIMOVEBITMAPCTRL_H_
#define _GUIMOVEBITMAPCTRL_H_

#ifndef _GUIBITMAPCTRL_H_
#include "gui/controls/guiBitmapCtrl.h"
#endif

class GuiMoveBitmapCtrl : public GuiBitmapCtrl
{
private:
	typedef GuiBitmapCtrl Parent;
public:
	GuiMoveBitmapCtrl();
	DECLARE_CONOBJECT(GuiMoveBitmapCtrl);

	static void initPersistFields();

	bool onWake();
	void onRender(Point2I offset, const RectI &updateRect);
	void setMove(bool isMove);
	bool isUpdate(RectI rect);

protected:
	F32 mMoveSpeed;
	bool mIsMove;
	bool mIsXDirection;
	bool mIsYDirection;
	S32 mOldTime;
	S32 mNowTime;
	F32 mChangeNum;
	S32 mMoveType;
	S32 mUpdateTime;
	S32 mCounter;
	Point2I mTexPoint;
	RectI mTexRect;
	S32 mXDirCounter;
	S32 mYDirCounter;
};

#endif