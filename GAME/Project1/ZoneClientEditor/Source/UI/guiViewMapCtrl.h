//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _GUIVIEWMAPCTRL_H_
#define _GUIVIEWMAPCTRL_H_

#include "gfx/gfxTextureHandle.h"
#include "gfx/gfxDevice.h"
#include "gui/core/guiControl.h"
#include "Gameplay/Data/ViewMapData.h"

#define    POS2D_WIDTH     100.0f                                // ƽ������߳�
#define    GUIMAPIMAGE     "gameres/data/missions/Maps/%s"       // ��ͼͼƬ
 
class guiViewMapCtrl : public GuiControl
{
	typedef GuiControl Parent;
public:
	enum Constant
	{
		MIN_ZONEID  = 1000,       // ��С��ͼ���
		MAX_ZONEID  = 9999,       // ����ͼ���
		MAX_COPYID  = 100,        // ��󸱱�����
	};
protected:
	GFXTexHandle		  m_TerrainTexture;		            // ��ͼͼƬ
	const static Point2I  mMapExtent;                       // ��ͼͼƬextent
	
	const static RectI    m_PathAvailableRect;              // Ѱ����Ч����
	RectF	              m_TextureRect;			        // ͼƬ�����Ƶ�����
	const static RectF    m_TextureAvailableRect;   	    // ͼƬ��������
	Point2F				  m_TerrainPlayer;		            // �����terrain�еĵ�
	                                                        // ע��terrain���Ͻ�Ϊ(0,0)��terrain���½�Ϊ(1,1)�������඼�Դ�����ϵΪ��׼
	Point2I               m_DestPos;                        // Ѱ��Ŀ��λ��

	Point2F               mTerrPos3D;                       // ��ǰ��ͼ����
	S32                   mCurrentMapId;                    // ��ǰ��ͼID
	S32                   mSelectMapId;                     // ѡ�е�ͼID

	StringTableEntry      m_SelfIconFileName;               // �Լ�ͼ��
	GFXTexHandle          m_SelfIconTexture;

	StringTableEntry      m_PlayerIconFileName;             // ���ͼ��
	GFXTexHandle          m_PlayerIconTexture;

	StringTableEntry      m_TeammateIconFileName;           // ����ͼ��
	GFXTexHandle          m_TeammateIconTexture;

	StringTableEntry      m_CameraImageFileName;            // �������ͼ��
	GFXTexHandle          m_CameraImageTexture;

	StringTableEntry      m_NpcIconFileName;                // Npcͼ��
	GFXTexHandle          m_NpcIconTexture;

	StringTableEntry      m_MonsterIconFileName;            // ����ͼ��
	GFXTexHandle          m_MonsterIconTexture;

	StringTableEntry      m_DestIconFileName;               // Ѱ���յ�ͼ��
	GFXTexHandle          m_DestIconTexture;

	GFXTexHandle		  m_TargetCover;

	static bool			  m_DisplayMonsters;			    // �Ƿ���ʾ����
	static bool			  m_DisplayNpc;				        // �Ƿ���ʾNPC
	static bool           m_DisplayPlayers;                 // �Ƿ���ʾ���
	static bool           m_DisplayAll;                     // ��ʾ�����ܿ���

public:
	DECLARE_CONOBJECT(guiViewMapCtrl);

	guiViewMapCtrl();
	~guiViewMapCtrl();

    void         onPreRender();
	static  void initPersistFields();

	bool onWake();
	void onSleep();

	inline Point2F            getTerrPos()                               { return mTerrPos3D; }
	inline S32                getCurrentMapId()                          { return mCurrentMapId; }
	inline void               setCurrentMapId(S32 mapId)                 { mCurrentMapId = mapId; }
	inline S32                getSelectMapId()                           { return mSelectMapId; }
	inline void               setSelectMapId(S32 mapId)                  { mSelectMapId = mapId; }
	S32                       convertZoneIdToMapId(const S32 zoneId);
	bool                      isValidMapId(const S32 mapId);
	static void               setDisplayAll(bool value);
	static void               setDisplayPlayer(bool value);
	static void               setDisplayNpc(bool value);
	static void               setDisplayMonster(bool value);

	void               drawPath(const RectF &drawRect);   //��Ѱ��·��
	void               drawBitmap(GFXTextureObject*texture,RectI &destRect);
	void               getMapTextureUV(const RectF &rect,Point2F& pos);         // ����UVת��
    Point2F            getTerrainPlayerScale(const Point2F &terrPos3D,const Point3F &pos3D);
	Point2F            convert3DposToMap2D(S32 mapId,const Point3F &pos3D);            // ת����ƽ���ͼ����
	Point2I            pos3DTo2D(S32 mapId,const Point3F &pos3D);                      // ��������ת�����굽(0~100)֮��
	Point2I            map2DToFixed2D(const Point2F &map2D);                           // ת�����굽(0~100)֮��

	virtual Point3F    convertMapPosTo3D(S32 mapId,Point2I pos2D){ return Point3F(0,0,0);}// ƽ��ת��������
	virtual void       initTexture();
};
extern guiViewMapCtrl g_ViewMap;
#endif