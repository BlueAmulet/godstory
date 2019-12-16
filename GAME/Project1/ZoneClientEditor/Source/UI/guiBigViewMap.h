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
    //GFXTexHandle          m_MarkIconTexture;               //���
	PlatformWindow        *mPlatformWindow;    
	Point2F               mTextureRectOffset;                //���������ԭ��(0,0)ƫ��
	Point2I               mMouseFixedPos;                    //ת��������굽(0~100)
	Point2I               mLocalMousePos;                    //��ǰ�������

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
	//�豸��ʧʱ����
	//
	static void releaseStateBlock();

	//
	//�豸����ʱ����
	//
	static void resetStateBlock();

	DECLARE_CONOBJECT(GuiBigViewMap);
};
#endif