//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _VIEWMAPDATA_H_
#define _VIEWMAPDATA_H_

#define    MAP_WIDTH       512.0f     // ���α߳�
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
	U16                             mMapId;              // ��ͼID
	StringTableEntry                mMapArea;            // ������½
	StringTableEntry                mMapName;            // ��ͼ����
	StringTableEntry                mMapFileName;        // ͼƬ�ļ��� 
	StringTableEntry                mAreaPos;            // �������Ͻ�����
	StringTableEntry                mTerrainPos;         // ��������
	U16                             mSideLength;         // ���������α߳�
	bool                            mIsNavigation;       // �Ƿ��Ѱ��
	Point2F                         mProjectOffset;      // ͶӰƫ��
	RectF                           mProjectRect;        // ƽ��ͶӰ
	static RectF                    mInvalidRect;        // ��ЧͶӰRect
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