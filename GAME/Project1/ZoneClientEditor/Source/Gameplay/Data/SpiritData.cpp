#include <iostream>
#include <fstream>
using namespace std;

#include "console/consoleTypes.h"
#include "platform/platform.h"
#include "Gameplay/Data/readDataFile.h"
#include "Gameplay/Data/SpiritData.h"

#define GAME_SPIRITE_DATA_FILE				"gameres/data/repository/SpiritRepository.dat"

SpiritRepository g_SpiritRepository;


////////////////////////////////SpiritRepository//////////////////////////////////////////

SpiritRepository::SpiritRepository()
{
	
}

SpiritRepository::~SpiritRepository()
{
	clear();
}

void SpiritRepository::read()
{
	CDataFile op;
	RData tempdata;
	char filename[256];

	Platform::makeFullPathName(GAME_SPIRITE_DATA_FILE, filename, sizeof(filename));
	op.readDataFile(filename);

	for(int i=0; i<op.RecordNum; ++i)
	{
		SpiritData *pData = new SpiritData;

		// Ԫ����
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "SpiritRepository.dat::Read - Failed to read mId", i);
		pData->mId = tempdata.m_U32;

		// Ԫ������
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "SpiritRepository.dat::Read - Failed to read mName", i);
		pData->mName = tempdata.m_string;

		// �츳���
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "SpiritRepository.dat::Read - Failed to read mTalentSetId", i);
		pData->mTalentSetId = tempdata.m_U32;

		// ����
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_enum8, "SpiritRepository.dat::Read - Failed to read mRace", i);
		pData->mRace = tempdata.m_Enum8;

		// ģ�ͱ��
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "SpiritRepository.dat::Read - Failed to read mShapeName", i);
		pData->mShapeName = StringTable->insert(tempdata.m_string);

		// ģ����ͼ
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "SpiritRepository.dat::Read - Failed to read mShapeImage", i);
		pData->mShapeImage = StringTable->insert(tempdata.m_string);

		// ͷ��Icon
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "SpiritRepository.dat::Read - Failed to read mIcon", i);
		pData->mIcon = StringTable->insert(tempdata.m_string);

		// ����(���6�־���)
		for (S32 i = 0; i < MAX_SPIRIT_SKILL; i++)
		{
			op.GetData(tempdata);
			AssertErrorMsg(tempdata.m_Type == DType_U32, "SpiritRepository.dat::Read - Failed to read mSkillIDs", i);
			pData->mSkillIDs[i] = tempdata.m_U32;
		}

		// ��ʼ�����ľ�����λ,bit��ʾ.(����ʾ������0,��Ĭ�Ͼ�����)
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U8, "SpiritRepository.dat::Read - Failed to read mOpenedSkillFlag", i);
		pData->mOpenedSkillFlag = tempdata.m_U8;

		//x����
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_F32, "SpiritRepository.dat::Read - Failed to read x scale", i);
		pData->mScale.x = tempdata.m_F32;

		//y����
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_F32, "SpiritRepository.dat::Read - Failed to read y scale", i);
		pData->mScale.y = tempdata.m_F32;

		//z����
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_F32, "SpiritRepository.dat::Read - Failed to read z scale", i);
		pData->mScale.z = tempdata.m_F32;

		insert(pData);
	}
}

void SpiritRepository::clear()
{
	for(SpiritMap::iterator it = mMap.begin(); it != mMap.end(); ++it)
	{
		if(it->second)
			delete it->second;
	}
	mMap.clear();
}

SpiritData* SpiritRepository::getSpiritData( U32 nSpiritId )
{
	SpiritMap::iterator it = mMap.find(nSpiritId);
	if(it != mMap.end())
		return it->second;
	return NULL;
}

bool SpiritRepository::insert( SpiritData* pData )
{
	if(!pData)
		return false;

	mMap.insert(SpiritMap::value_type(pData->mId, pData));
	return true;
}

void SpiritRepository::AssertErrorMsg( bool bAssert, const char *msg, int rowNum )
{
	char buf[256];

	if (!bAssert)
	{
		dSprintf(buf, sizeof(buf), "RowNum: %d  ", rowNum);
		dStrcat(buf, sizeof(buf), msg);
		AssertRelease(false, buf);
	}
}