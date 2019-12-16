#pragma once

#include "core/tVector.h"
#include "math/mPoint.h"
#include <hash_map>
#include "Common/PacketType.h"


struct SpiritData
{
	U32					mId;										//Ԫ����
	StringTableEntry	mName;										//Ԫ������
	U32					mTalentSetId;								//�츳�����
	U8					mRace;										//����
	StringTableEntry	mShapeName;									//ģ�ͱ��
	StringTableEntry	mShapeImage;								//ģ����ͼ
	StringTableEntry	mIcon;										//Icon
	U32					mSkillIDs[MAX_SPIRIT_SKILL];				//����(���6��)
	U8					mOpenedSkillFlag;							//�����ľ�����λ,bit��ʾ����ע������ʾĬ�Ͼ��У���������0��
	Point3F				mScale;										//x y z ���������

	SpiritData()		{ dMemset(this, 0, sizeof(SpiritData)); }
};

class SpiritRepository
{
	typedef stdext::hash_map<U32, SpiritData*> SpiritMap;
public:
	SpiritRepository();
	~SpiritRepository();
	void read();
	void clear();
	SpiritData* getSpiritData(U32 nSpiritId);
	bool insert(SpiritData* pData);

private:
	void AssertErrorMsg(bool bAssert, const char *msg, int rowNum);
private:
	SpiritMap mMap;
};

extern SpiritRepository g_SpiritRepository;