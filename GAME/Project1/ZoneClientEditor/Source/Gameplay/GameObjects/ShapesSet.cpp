//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------


#include "Gameplay/GameObjects/ShapesSet.h"
#include "Gameplay/Data/readDataFile.h"
#include "Gameplay/GameplayCommon.h"

// ========================================================================================================================================
//  ShapesSetRepository
// ========================================================================================================================================

ShapesSetRepository g_ShapesSetRepository;

ShapesSetRepository::ShapesSetRepository()
{
	Clear();
}

ShapesSetRepository::~ShapesSetRepository()
{
	Clear();
}

void ShapesSetRepository::Clear()
{
	ShapesSetMap::const_iterator itr = m_ShapesSetMap.begin();
	for(;itr != m_ShapesSetMap.end(); itr++)
		delete itr->second;
	m_ShapesSetMap.clear();
}

void ShapesSetRepository::Read()
{
	CDataFile file;
	RData tempData;
	char filename[1024] = {0};

	// ��ȡģ������
	Platform::makeFullPathName(GAME_MODSETDATA_FILE, filename, sizeof(filename));
	file.ReadDataInit();
	file.readDataFile(filename);
	//AssertRelease(file.readDataFile(filename), "can NOT read file : ModSetRepository.dat!");

	/*
	IColumnData *columnData = new IColumnData(file.ColumNum, "ModSetRepository.dat");
	columnData->setField(CalcOffset(shapeName,					ShapesSet),	DType_string,	"��ģ������");
	columnData->setField(CalcOffset(skinName,					ShapesSet),	DType_string,	"��ģ����ͼ");
	columnData->setField(CalcOffset(subImages[0].shapeName,		ShapesSet),	DType_string,	"����ģ������1 ����");
	columnData->setField(CalcOffset(subImages[0].skinName,		ShapesSet),	DType_string,	"����ģ����ͼ1 ����");
	columnData->setField(CalcOffset(subImages[0].mountPoint,	ShapesSet),	DType_string,	"��ģ�����ӵ�1 ����");
	columnData->setField(CalcOffset(subImages[0].mountPointSelf,ShapesSet),	DType_string,	"�������ӵ�1 ����");
	columnData->setField(CalcOffset(subImages[1].shapeName,		ShapesSet),	DType_string,	"����ģ������2 ����");
	columnData->setField(CalcOffset(subImages[1].skinName,		ShapesSet),	DType_string,	"����ģ����ͼ2 ����");
	columnData->setField(CalcOffset(subImages[1].mountPoint,	ShapesSet),	DType_string,	"��ģ�����ӵ�2 ����");
	columnData->setField(CalcOffset(subImages[1].mountPointSelf,ShapesSet),	DType_string,	"�������ӵ�2 ����");
	columnData->setField(CalcOffset(subImages[2].shapeName,		ShapesSet),	DType_string,	"����ģ������3 ����");
	columnData->setField(CalcOffset(subImages[2].skinName,		ShapesSet),	DType_string,	"����ģ����ͼ3 ����");
	columnData->setField(CalcOffset(subImages[2].mountPoint,	ShapesSet),	DType_string,	"��ģ�����ӵ�3 ����");
	columnData->setField(CalcOffset(subImages[2].mountPointSelf,ShapesSet),	DType_string,	"�������ӵ�3 ����");
	columnData->setField(CalcOffset(subImages[3].shapeName,		ShapesSet),	DType_string,	"����ģ������4 ����");
	columnData->setField(CalcOffset(subImages[3].skinName,		ShapesSet),	DType_string,	"����ģ����ͼ4 ����");
	columnData->setField(CalcOffset(subImages[3].mountPoint,	ShapesSet),	DType_string,	"��ģ�����ӵ�4 ����");
	columnData->setField(CalcOffset(subImages[3].mountPointSelf,ShapesSet),	DType_string,	"�������ӵ�4 ����");
	columnData->setField(CalcOffset(subImages[4].shapeName,		ShapesSet),	DType_string,	"����ģ������5 ����");
	columnData->setField(CalcOffset(subImages[4].skinName,		ShapesSet),	DType_string,	"����ģ����ͼ5 ����");
	columnData->setField(CalcOffset(subImages[4].mountPoint,	ShapesSet),	DType_string,	"��ģ�����ӵ�5 ����");
	columnData->setField(CalcOffset(subImages[4].mountPointSelf,ShapesSet),	DType_string,	"�������ӵ�5 ����");
	columnData->setField(CalcOffset(subImages[5].shapeName,		ShapesSet),	DType_string,	"����ģ������6");
	columnData->setField(CalcOffset(subImages[5].skinName,		ShapesSet),	DType_string,	"����ģ����ͼ6");
	columnData->setField(CalcOffset(subImages[5].mountPoint,	ShapesSet),	DType_string,	"��ģ�����ӵ�6");
	columnData->setField(CalcOffset(subImages[5].mountPointSelf,ShapesSet),	DType_string,	"�������ӵ�6");
	columnData->setField(CalcOffset(subImages[6].shapeName,		ShapesSet),	DType_string,	"����ģ������7");
	columnData->setField(CalcOffset(subImages[6].skinName,		ShapesSet),	DType_string,	"����ģ����ͼ7");
	columnData->setField(CalcOffset(subImages[6].mountPoint,	ShapesSet),	DType_string,	"��ģ�����ӵ�7");
	columnData->setField(CalcOffset(subImages[6].mountPointSelf,ShapesSet),	DType_string,	"�������ӵ�7");
	columnData->setField(CalcOffset(subImages[7].shapeName,		ShapesSet),	DType_string,	"����ģ������8");
	columnData->setField(CalcOffset(subImages[7].skinName,		ShapesSet),	DType_string,	"����ģ����ͼ8");
	columnData->setField(CalcOffset(subImages[7].mountPoint,	ShapesSet),	DType_string,	"��ģ�����ӵ�8");
	columnData->setField(CalcOffset(subImages[7].mountPointSelf,ShapesSet),	DType_string,	"�������ӵ�8");
	*/

	for(int i = 0 ; i < file.RecordNum ; ++ i)
	{
		ShapesSet* pData = new ShapesSet;
		//ID
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_U32, "ModSetRepository.dat::Read");
		U32 id = tempData.m_U32;
		//��ģ������
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->shapeName = filterNullString(tempData.m_string);
		//��ģ����ͼ
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->skinName = filterNullString(tempData.m_string);
		//����ģ������1 ����
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[0].shapeName = filterNullString(tempData.m_string);
		//����ģ����ͼ1 ����
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[0].skinName = filterNullString(tempData.m_string);
		//��ģ�����ӵ�1 ����
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[0].mountPoint = filterNullString(tempData.m_string);
		//�������ӵ�1 ����
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[0].mountPointSelf = filterNullString(tempData.m_string);
		//����ģ������2
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[1].shapeName = filterNullString(tempData.m_string);
		//����ģ����ͼ2
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[1].skinName = filterNullString(tempData.m_string);
		//��ģ�����ӵ�2
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[1].mountPoint = filterNullString(tempData.m_string);
		//�������ӵ�2
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[1].mountPointSelf = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[2].shapeName = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[2].skinName = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[2].mountPoint = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[2].mountPointSelf = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[3].shapeName = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[3].skinName = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[3].mountPoint = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[3].mountPointSelf = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[4].shapeName = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[4].skinName = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[4].mountPoint = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[4].mountPointSelf = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[5].shapeName = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[5].skinName = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[5].mountPoint = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[5].mountPointSelf = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[6].shapeName = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[6].skinName = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[6].mountPoint = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[6].mountPointSelf = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[7].shapeName = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[7].skinName = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[7].mountPoint = filterNullString(tempData.m_string);
		//
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModSetRepository.dat::Read");
		pData->subImages[7].mountPointSelf = filterNullString(tempData.m_string);

		m_ShapesSetMap.insert(ShapesSetMap::value_type(id, pData));
	}
}

const ShapesSet* ShapesSetRepository::GetSet(U32 id)
{
	ShapesSetMap::iterator itr = m_ShapesSetMap.find(id);
	if(itr != m_ShapesSetMap.end())
		return itr->second;
	return NULL;
}
