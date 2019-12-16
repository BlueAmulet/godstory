//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _VIEWMAPDATA_H_
#define _VIEWMAPDATA_H_

#define    MAP_WIDTH       512.0f     // 地形边长
class viewMapData
{
public:
	friend class viewMapRepository;
	viewMapData();

	inline U16                      getMapId()                      { return mMapId; }
	inline StringTableEntry         getMapName()                    { return mMapName; }
	inline StringTableEntry         getMapArea()                    { return mMapArea; }
	inline StringTableEntry         getMapFileName()                { return mMapFileName; }
	inline U16                      getLength()                     { return mSideLength; }
	inline bool                     isNavigation()                  { return mIsNavigation; }
	inline Point2F                  getProjectOffset()              { return mProjectOffset; }
	inline RectF                    getProjectRect()                { return mProjectRect; }
	void                            getAreaPos(Point2F &pos);
	void                            getTerrainPos(Point2F &pos);
	void                            projectOffset(const char*);
	void                            ProjectRectTo2D();

protected:
	U16                             mMapId;              // 地图ID
	StringTableEntry                mMapArea;            // 所属大陆
	StringTableEntry                mMapName;            // 地图名称
	StringTableEntry                mMapFileName;        // 图片文件名 
	StringTableEntry                mAreaPos;            // 区域左上角坐标
	StringTableEntry                mTerrainPos;         // 地形坐标
	U16                             mSideLength;         // 区域正方形边长
	bool                            mIsNavigation;       // 是否可寻径
	Point2F                         mProjectOffset;      // 投影偏移
	RectF                           mProjectRect;        // 平面投影
	static RectF                    mInvalidRect;        // 无效投影Rect
};

class viewMapRepository
{

public:
	typedef stdext::hash_map<U32,viewMapData*>    viewMap_Map;
public:
	viewMapRepository();
	~viewMapRepository();

	bool                 read();
	bool                 insert(viewMapData* pData);
	void                 clear();
	viewMapData*         getViewMapData(U16 mapId);

	inline viewMap_Map&  getData()           { return mViewMap; }
	inline IColumnData*  getColumnData()     { return mColumnData; }

protected:
	
	viewMap_Map                     mViewMap;
	IColumnData*                    mColumnData;
};

extern viewMapRepository g_viewMapRepository;

#endif