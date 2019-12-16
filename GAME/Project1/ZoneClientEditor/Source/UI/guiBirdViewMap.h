//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _GUIBIRDVIEWMAP_H_
#define _GUIBIRDVIEWMAP_H_
#include "ui/guiViewMapCtrl.h"

class GFXStateBlock;
class guiBirdViewMap:public guiViewMapCtrl
{
	typedef guiViewMapCtrl Parent;

protected:
	F32					m_Scale;				// 缩放 即小地图的视野范围，单位是terrain边长
	F32                 m_MaxScale;             // 小地图最大缩放比例值
	F32                 m_MinScale;             // 小地图最小缩放比例值
	F32                 mRadius;                // 圆半径
	RectI               mPosTextRect;           // 自身坐标显示位置

	U32				    m_UpdateInterval;	    // < Unit: ms.
	U32				    m_LastUpdateTime;
	U32				    m_CurrentTime;
	Platform::LocalTime localTime;

	StringTableEntry    m_CoverIconFileName;
	GFXTexHandle        m_CoverIconTexture; 
	
public:
	guiBirdViewMap();
	~guiBirdViewMap();

	void onPreRender();
	void onRender(Point2I offset, const RectI &updateRect);
	void onMouseDown(const GuiEvent &event);

	bool onWake();
	void onSleep();

	void           initTexture();
	bool           updateData();
	Point3F        convertMapPosTo3D(S32 mapId,Point2I pos2D);
	static void    initPersistFields();
private:
	static GFXStateBlock* mSetSB;
	static GFXStateBlock* mSetStencilSB;
	static GFXStateBlock* mStencilFalseSB;
	static GFXStateBlock* mClearSB;
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
	DECLARE_CONOBJECT(guiBirdViewMap);
};
#endif