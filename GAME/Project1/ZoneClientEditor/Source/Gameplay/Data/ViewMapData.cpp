//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#include "gui/core/guiTypes.h"
#include "BuildPrefix.h"
#include "Util/ColumnData.h"
#include "ui/guiViewMapCtrl.h"
#include "Gameplay/Data/ViewMapData.h"

//=====================================================================
//ƽ��������Ч��ռ��Ч������
//ͼƬ��С1024*768,ͶӰ��Ч��640*640,��Ч�� ���(100,50),�Ҳ�(260,10)
RectF viewMapData::mInvalidRect(0.15625f,0.078125f,0.5625f,0.09375f);
//=====================================================================
viewMapData::viewMapData() : mMapId(0),
							 mIsNavigation(true),
							 mSideLength(0),
							 mProjectOffset(0,0)
{
	mMapName     = StringTable->insert("");
	mMapArea     = StringTable->insert("");
	mMapFileName = StringTable->insert("");
	mAreaPos     = StringTable->insert("");
	mTerrainPos  = StringTable->insert("");
}

void viewMapData::getAreaPos(Point2F &pos)
{
	if(mAreaPos && mAreaPos[0])
	{
		dSscanf(mAreaPos,"%g %g",&pos.x,&pos.y);
		return;
	}

	AssertFatal(false,"viewMapData::Area position is null");
}

void viewMapData::getTerrainPos(Point2F &pos)
{
	if(mTerrainPos && mTerrainPos[0])
	{
		dSscanf(mTerrainPos,"%g %g",&pos.x,&pos.y);
		return;
	}

	AssertFatal(false,"viewMapData::Terrain position is null");
}

void viewMapData::projectOffset(const char* strPos)
{
	if(dStrcmp(strPos,"") == 0)
		return;

	Point2F tempPos;
	getTerrainPos(tempPos);
	//���������ԭ��(0,0)ƫ�ơ�ע:����γ����Ϊ512,����-Yӳ�䵽Y���512
	mProjectOffset.x =  tempPos.x / MAP_WIDTH;
	mProjectOffset.y = (tempPos.y + MAP_WIDTH)/MAP_WIDTH; 

}

void viewMapData::ProjectRectTo2D()
{
	Point2F areaPos;
	getAreaPos(areaPos);
	Point2F offset = getProjectOffset();

	F32 terrScaleLen = mSideLength / MAP_WIDTH;
	//��������ϵ,������ʼ��Ϊ��,���Կɼ�������
	F32 terrScaleX = areaPos.x / MAP_WIDTH - offset.x;
	F32 terrScaleY = offset.y - areaPos.y / MAP_WIDTH;

	mProjectRect.point.x = terrScaleX - mInvalidRect.point.x * terrScaleLen;
	mProjectRect.point.y = terrScaleY - mInvalidRect.point.y * terrScaleLen;
	mProjectRect.extent.x = terrScaleLen + terrScaleLen * mInvalidRect.extent.x;
	mProjectRect.extent.y = terrScaleLen + terrScaleLen * mInvalidRect.extent.y;
}
//////////////////////////////////////////////////////////////////////////////////////////
viewMapRepository g_viewMapRepository;

viewMapRepository::viewMapRepository()
{
	mColumnData = NULL;
}

viewMapRepository::~viewMapRepository()
{
	clear();
}

void viewMapRepository::clear()
{
	if(mColumnData)
	{
		delete mColumnData;
		mColumnData = NULL;
	}

	viewMap_Map::iterator it = mViewMap.begin();
	for (; it != mViewMap.end(); ++it)
	{
		if(it->second)
			delete it->second;
	}

	mViewMap.clear();
}

bool viewMapRepository::read()
{
	CDataFile file;
	RData lineData;
	char filename[1024];
	file.ReadDataInit();
	Platform::makeFullPathName(GAME_VIEWMAPDATA_FILE, filename, sizeof(filename));

	if (!file.readDataFile(filename))
	{
		file.ReadDataClose();
		AssertRelease(false, "Cannt read file : ViewMapRepository.dat!");
		return false;
	}

	mColumnData = new IColumnData(file.ColumNum,"ViewMapRepository.dat");
	mColumnData->setField(CalcOffset(mMapId,               viewMapData),       DType_U16,            "��ͼ���");
	mColumnData->setField(CalcOffset(mMapArea,             viewMapData),       DType_string,         " ������½");
	mColumnData->setField(CalcOffset(mMapName,             viewMapData),       DType_string,         "��ͼ����");
	mColumnData->setField(CalcOffset(mMapFileName,         viewMapData),       DType_string,         "��ͼ�ļ���");
	mColumnData->setField(CalcOffset(mAreaPos,             viewMapData),       DType_string,         "��������");
	mColumnData->setField(CalcOffset(mSideLength,          viewMapData),       DType_U16,            "���������α߳�");
	mColumnData->setField(CalcOffset(mTerrainPos,          viewMapData),       DType_string,         "������Ч��");
	mColumnData->setField(CalcOffset(mIsNavigation,        viewMapData),       DType_U8,             "�Ƿ��Ѱ��");

	for (int i=0; i<file.RecordNum; i++)
	{
		viewMapData* pData = new viewMapData;
		for (int j=0; j<file.ColumNum;j++)
		{
			file.GetData(lineData);
			mColumnData->setData(pData,j,lineData);
		}
		//����ƫ��ֵ
		pData->projectOffset(pData->mTerrainPos);
		pData->ProjectRectTo2D();
		insert(pData);
	}

	file.ReadDataClose();
	return true;
}

bool viewMapRepository::insert(viewMapData* pData)
{
	if(NULL == pData)
		return false;

	mViewMap.insert(viewMap_Map::value_type(pData->mMapId,pData));
	return true;
}

viewMapData* viewMapRepository::getViewMapData(U16 mapId)
{
	viewMap_Map::iterator it = mViewMap.find(mapId);
	if(it != mViewMap.end())
		return it->second;

	AssertFatal(false,"viewMapData::Cannot find mapId")
	return NULL;
}

ConsoleFunction(getViewMapData,const char*,3,3,"getViewMapData(%zoneId,%col)")
{
	S32 mapId = g_ViewMap.convertZoneIdToMapId(dAtoi(argv[1]));
	if(g_ViewMap.isValidMapId(mapId))
	{
		
		viewMapData* pData = g_viewMapRepository.getViewMapData(mapId);
		if(NULL == pData || NULL == g_viewMapRepository.getColumnData())
			return "";

		char* value = Con::getReturnBuffer(64);
		std::string to;
		g_viewMapRepository.getColumnData()->getData(pData,dAtoi(argv[2]),to);
		dStrcpy(value,64,to.c_str());
		return value;
	}
	return "";
}

ConsoleFunction(getViewMapDataSize,S32,1,1,"getViewMapDataSize()")
{
	viewMapRepository::viewMap_Map& mapData = g_viewMapRepository.getData();
	return mapData.size();
}