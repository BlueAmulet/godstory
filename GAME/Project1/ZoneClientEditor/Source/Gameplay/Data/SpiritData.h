#pragma once

#include "core/tVector.h"
#include "math/mPoint.h"
#include <hash_map>
#include "Common/PacketType.h"


struct SpiritData
{
	U32					mId;										//元神编号
	StringTableEntry	mName;										//元神名称
	U32					mTalentSetId;								//天赋树编号
	U8					mRace;										//种族
	StringTableEntry	mShapeName;									//模型编号
	StringTableEntry	mShapeImage;								//模型贴图
	StringTableEntry	mIcon;										//Icon
	U32					mSkillIDs[MAX_SPIRIT_SKILL];				//绝招(最多6种)
	U8					mOpenedSkillFlag;							//开启的绝招栏位,bit表示。（注：不表示默认绝招，即绝招栏0）
	Point3F				mScale;										//x y z 方向的缩放

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