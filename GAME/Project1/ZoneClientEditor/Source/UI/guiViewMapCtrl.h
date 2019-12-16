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

#define    POS2D_WIDTH     100.0f                                // 平面坐标边长
#define    GUIMAPIMAGE     "gameres/data/missions/Maps/%s"       // 地图图片
 
class guiViewMapCtrl : public GuiControl
{
	typedef GuiControl Parent;
public:
	enum Constant
	{
		MIN_ZONEID  = 1000,       // 最小地图编号
		MAX_ZONEID  = 9999,       // 最大地图编号
		MAX_COPYID  = 100,        // 最大副本数量
	};
protected:
	GFXTexHandle		  m_TerrainTexture;		            // 地图图片
	const static Point2I  mMapExtent;                       // 地图图片extent
	
	const static RectI    m_PathAvailableRect;              // 寻径有效区域
	RectF	              m_TextureRect;			        // 图片所绘制的区域
	const static RectF    m_TextureAvailableRect;   	    // 图片可用区域
	Point2F				  m_TerrainPlayer;		            // 玩家在terrain中的点
	                                                        // 注意terrain左上角为(0,0)，terrain右下角为(1,1)，整个类都以此坐标系为标准
	Point2I               m_DestPos;                        // 寻径目标位置

	Point2F               mTerrPos3D;                       // 当前地图坐标
	S32                   mCurrentMapId;                    // 当前地图ID
	S32                   mSelectMapId;                     // 选中地图ID

	StringTableEntry      m_SelfIconFileName;               // 自己图标
	GFXTexHandle          m_SelfIconTexture;

	StringTableEntry      m_PlayerIconFileName;             // 玩家图标
	GFXTexHandle          m_PlayerIconTexture;

	StringTableEntry      m_TeammateIconFileName;           // 队友图标
	GFXTexHandle          m_TeammateIconTexture;

	StringTableEntry      m_CameraImageFileName;            // 摄像机的图标
	GFXTexHandle          m_CameraImageTexture;

	StringTableEntry      m_NpcIconFileName;                // Npc图标
	GFXTexHandle          m_NpcIconTexture;

	StringTableEntry      m_MonsterIconFileName;            // 怪物图标
	GFXTexHandle          m_MonsterIconTexture;

	StringTableEntry      m_DestIconFileName;               // 寻径终点图标
	GFXTexHandle          m_DestIconTexture;

	GFXTexHandle		  m_TargetCover;

	static bool			  m_DisplayMonsters;			    // 是否显示怪物
	static bool			  m_DisplayNpc;				        // 是否显示NPC
	static bool           m_DisplayPlayers;                 // 是否显示玩家
	static bool           m_DisplayAll;                     // 显示对象总开关

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

	void               drawPath(const RectF &drawRect);   //画寻径路径
	void               drawBitmap(GFXTextureObject*texture,RectI &destRect);
	void               getMapTextureUV(const RectF &rect,Point2F& pos);         // 纹理UV转换
    Point2F            getTerrainPlayerScale(const Point2F &terrPos3D,const Point3F &pos3D);
	Point2F            convert3DposToMap2D(S32 mapId,const Point3F &pos3D);            // 转换到平面地图坐标
	Point2I            pos3DTo2D(S32 mapId,const Point3F &pos3D);                      // 世界坐标转换坐标到(0~100)之间
	Point2I            map2DToFixed2D(const Point2F &map2D);                           // 转换坐标到(0~100)之间

	virtual Point3F    convertMapPosTo3D(S32 mapId,Point2I pos2D){ return Point3F(0,0,0);}// 平面转世界坐标
	virtual void       initTexture();
};
extern guiViewMapCtrl g_ViewMap;
#endif