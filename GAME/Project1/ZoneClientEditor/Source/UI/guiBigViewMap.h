//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUIBIGVIEWMAP_H_
#define _GUIBIGVIEWMAP_H_
#include "ui/guiViewMapCtrl.h"

class GFXStateBlock;
class GuiBigViewMap:public guiViewMapCtrl
{
	typedef guiViewMapCtrl Parent;

protected:
	//StringTableEntry      m_MarkIconFileName;           
    //GFXTexHandle          m_MarkIconTexture;               //标记
	PlatformWindow        *mPlatformWindow;    
	Point2F               mTextureRectOffset;                //相对于坐标原点(0,0)偏移
	Point2I               mMouseFixedPos;                    //转换鼠标坐标到(0~100)
	Point2I               mLocalMousePos;                    //当前鼠标坐标

public:
	GuiBigViewMap();
	~GuiBigViewMap();

	void onRender(Point2I offset, const RectI &updateRect);
    void onMouseDown(const GuiEvent &event); 
	void onMouseLeave(const GuiEvent &event);
	void onMouseMove(const GuiEvent &event);

	bool onAdd();
	void handleResize(DeviceId did, S32 width, S32 height);

	bool onWake();
	void onSleep();

	void           initTexture();
	void           setSelectMap(S32 mapId);
	Point3F        convertMapPosTo3D(S32 mapId,Point2I pos2D);
    void           moveToPos2D(S32 mapId,Point2I DestPos);
	void           moveToPos3D(S32 mapId,Point3F &pos3D,Point2I pos2D);
	static void    initPersistFields();
	void           renderText(Point2I offset,const char* text,ColorI &color);

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

	DECLARE_CONOBJECT(GuiBigViewMap);
};
#endif