#pragma once

#ifndef _GUISCREENSHOW_H_
#define _GUISCREENSHOW_H_

#ifndef _GFXTEXTUREHANDLE_H_
#include "gfx/gfxTextureHandle.h"
#endif

#include "gui/core/guiControl.h"
#include "gfx/gfxDevice.h"
#include "terrain/terrData.h"

class GuiScreenShowCtrl:public GuiControl
{
	typedef GuiControl Parent;

protected:
	StringTableEntry      m_ScreenTextureName;
	GFXTexHandle          m_ScreenTexture;
	bool                  m_IsStartChangeAlpha;
	U32					  m_dwLastTime;

	S32					  alpha;
public:
	GuiScreenShowCtrl();

	void onPreRender();
	void onRender(Point2I offset, const RectI &updateRect);
	bool onWake();
	void onSleep();

	//void loadScreenTexture();
	static void initPersistFields();

    DECLARE_CONOBJECT(GuiScreenShowCtrl);
private:
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
	void setTranSportTexture(const char* Name);
};
#endif