#ifndef _GUICONTROLEXT_H_
#define _GUICONTROLEXT_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

class GFXStateBlock;

//==================================================================================================================================
//   GuiControlExt
//==================================================================================================================================
class GuiControlExt : public GuiControl
{
	typedef GuiControl Parent;
private:
	bool m_BuffBinding;
	GFXTexHandle    m_BuffTexture;
	static GFXTexHandle	m_DepthTex;
	bool mCanUseSeparateAlphaBlend;

public:
	GuiControlExt();
	DECLARE_CONOBJECT(GuiControlExt);

	bool onParentSetVisible(bool value)
	{
		if(value)
			m_DirtyFlag = true;
		else
			if(!m_BuffBinding)
				m_BuffTexture = NULL;

		return true;
	}

	void SetDirty(bool bDirty)
	{
		m_DirtyFlag = bDirty;
	}

	//bool onAdd();
	//void onRemove();

	//void handleSyncEvent(DeviceId did);
	
	void onPreRender();
	void onRender(Point2I offset, const RectI &updateRect);
	virtual void onRenderExt(Point2I offset, const RectI &updateRect);

	void recreateBuff();
	void drawBuff(GFXTextureObject *texture, const RectI &dstRect);
private:
	static GFXStateBlock* mSetSB;
public:
	static void init();
	static void shutdown();

	static void clear();
	//
	//设备丢失时调用
	//
	static void releaseStateBlock();

	//
	//设备重置时调用
	//
	static void resetStateBlock();
};


#endif

