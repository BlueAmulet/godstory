//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

//#include "console/console.h"
//#include "console/consoleTypes.h"
//#include "gfx/gBitmap.h"
#include "gui/core/guiControl.h"
//#include "gfx/gfxDevice.h"
//#include "gfx/gfxTextureHandle.h"

class GuiChunkedBitmapCtrl : public GuiControl
{
private:
	typedef GuiControl Parent;
	void renderRegion(const Point2I &offset, const Point2I &extent);

protected:
	StringTableEntry mBitmapName;
	GFXTexHandle mTexHandle;
	bool  mUseVariable;
	bool  mTile;

public:
	//creation methods
	DECLARE_CONOBJECT(GuiChunkedBitmapCtrl);
	GuiChunkedBitmapCtrl();
	static void initPersistFields();

	//Parental methods
	bool onWake();
	void onSleep();

	void setBitmap(const char *name);

	void onRender(Point2I offset, const RectI &updateRect);
private:
	static GFXStateBlock* mStoreSB;
	static GFXStateBlock* mSetSB;
public:
	static void init();
	static void shutdown();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};
