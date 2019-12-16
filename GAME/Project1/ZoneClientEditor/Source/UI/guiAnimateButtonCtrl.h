//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUIANIMATEBUTTON_H_
#define _GUIANIMATEBUTTON_H_

#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif
#ifndef GFX_Texture_Manager_H_
#include "gfx/gfxTextureManager.h"
#endif

class GuiAnimateButtonCtrl : public GuiButtonCtrl
{
private:
	typedef GuiButtonCtrl Parent;
protected:
	StringTableEntry	mBitmapName;
	U32					mFrameRate;
	U32					mLastTime;
	U32					mFrame;
	GFXTexHandle		mTexture;
	Vector<RectI>		mTextureRects;

	void renderButton(GFXTexHandle &texture, Point2I &offset, const RectI& updateRect);

public:
	DECLARE_CONOBJECT(GuiAnimateButtonCtrl);
	GuiAnimateButtonCtrl();
	~GuiAnimateButtonCtrl();

	static void initPersistFields();

	bool onWake();
	void onSleep();
	void inspectPostApply();
	void setBitmap(const char *name);
	void setFrameRate(U32 rate) { mFrameRate = rate;}

	S32 constructBitmapArray();
	RectI getBitmapArrayRect(U32 index);

	void onRender(Point2I offset, const RectI &updateRect);
};

#endif //_GUIANIMATEBUTTON_H_
