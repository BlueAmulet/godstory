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
	F32					m_Scale;				// ���� ��С��ͼ����Ұ��Χ����λ��terrain�߳�
	F32                 m_MaxScale;             // С��ͼ������ű���ֵ
	F32                 m_MinScale;             // С��ͼ��С���ű���ֵ
	F32                 mRadius;                // Բ�뾶
	RectI               mPosTextRect;           // ����������ʾλ��

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
	//�豸��ʧʱ����
	//
	static void releaseStateBlock();

	//
	//�豸����ʱ����
	//
	static void resetStateBlock();
	DECLARE_CONOBJECT(guiBirdViewMap);
};
#endif