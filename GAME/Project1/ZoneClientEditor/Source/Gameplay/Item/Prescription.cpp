//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Util/ColumnData.h"
#include "BuildPrefix.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/Item/ItemBaseData.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "Gameplay/GameObjects/LivingSkill.h"
#include "Gameplay/Item/Prescription.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PrescriptionData::PrescriptionData() : mSerialID(0),
                                       mCategory(0),
									   mSubCategory(0),
									   mLevel(0),
									   mVigour(0),
									   mInsight(0),
									   mLivingSkillID(0),
									   mMoney(0),
									   mSubCategoryTools(0),
									   mbaseItemID(0),
									   mbScript(false),
									   mItemNum(1)
{
	mDesc = StringTable->insert("");
	mName = StringTable->insert("");
	dMemset(&mMaterial,0,sizeof(mMaterial));
}
static char* szCategory[][17] = 
{
	{
		"法器",
		"单短",
		"双短",
		"琴",
		"弓",
		"旗杖",
		"刀斧",
		"1级近程武器材料",
		"2级近程武器材料",
		"3级近程武器材料",
		"4级近程武器材料",
		"5级近程武器材料",
		"1级远程武器材料",
		"2级远程武器材料",
		"3级远程武器材料",
		"4级远程武器材料",
		"5级远程武器材料",
	},
	{
		"头盔",
		"衣服",
		"背部",
		"肩部",
		"手部",
		"腰部",
		"脚部",
		"1级合成兽皮",
		"2级合成兽皮",
		"3级合成兽皮",
		"4级合成兽皮",
		"5级合成兽皮",
		"1级加工兽皮",
		"2级加工兽皮",
		"3级加工兽皮",
		"4级加工兽皮",
		"5级加工兽皮",
	},
	{
		"护身符",
		"项链",
		"戒指",
		"1级首饰合成材料",
		"2级首饰合成材料",
		"3级首饰合成材料",
		"4级首饰合成材料",
		"5级首饰合成材料",
	},
	{
		"",
	},
	{
		"恢复类",
		"状态类",
		"特殊类",
		"1级丹药合成材料",
		"2级丹药合成材料",
		"3级丹药合成材料",
		"4级丹药合成材料",
		"5级丹药合成材料",
	},
    {
	    "恢复类",
		"状态类",
		"状态类",
		"1级食物合成材料",
		"2级食物合成材料",
		"3级食物合成材料",
		"4级食物合成材料",
		"5级食物合成材料",
	},
	"",
};

StringTableEntry PrescriptionData::getName()
{
	if(dStrcmp(mName,"") == 0)
		return NULL;
	else
		return mName;
}

StringTableEntry PrescriptionData::getSubCategoryName()
{
	return szCategory[getCategory() - 1][GETSUB(getSubCategory()) - 1];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PrescriptionRepository g_PrescriptionRepository;
PrescriptionRepository::PrescriptionRepository()
{
	mColumnData = NULL;
}

PrescriptionRepository::~PrescriptionRepository()
{
	clear();
}
void PrescriptionRepository::clear()
{
	if (mColumnData)
	{
		delete mColumnData;
		mColumnData = NULL;
	}

	dataMap::iterator it  = mDataMap.begin();
	for (; it != mDataMap.end(); ++it)
	{
		if(it->second)
			delete it->second;
	}

	mDataMap.clear();
}
bool PrescriptionRepository::read()
{
	CDataFile file;
	RData     lineData;
	char filename[1024];
	file.ReadDataInit();
	Platform::makeFullPathName(PRESCRIPTIONFILE, filename, sizeof(filename));

	if (!file.readDataFile(filename))
	{
		file.ReadDataClose();
		AssertRelease(false, "can't read file : PrescriptionRepository.dat!");
		return false;
	}

	mColumnData = new IColumnData(file.ColumNum,"PrescriptionRepository.dat");
	mColumnData->setField(CalcOffset(mSerialID,        PrescriptionData),       DType_U32,        "配方ID");
	mColumnData->setField(CalcOffset(mCategory,        PrescriptionData),       DType_enum8,      "配方类别");
	mColumnData->setField(CalcOffset(mSubCategory,     PrescriptionData),       DType_enum16,     "配方子类别");
	mColumnData->setField(CalcOffset(mLevel,           PrescriptionData),       DType_U8,         "配方等级");
	mColumnData->setField(CalcOffset(mVigour,          PrescriptionData),       DType_U16,        "所需活力");
	mColumnData->setField(CalcOffset(mInsight,         PrescriptionData),       DType_U32,        "悟性要求");
	mColumnData->setField(CalcOffset(mLivingSkillID,   PrescriptionData),       DType_U32,        "生活技能ID");
	mColumnData->setField(CalcOffset(mMoney,           PrescriptionData),       DType_U32,        "所需金钱");
	mColumnData->setField(CalcOffset(mSubCategoryTools,PrescriptionData),       DType_U16,        "消耗道具子类别");
	mColumnData->setField(CalcOffset(mDesc,            PrescriptionData),       DType_string,     "配方描述");
	for (int i=0; i<6; i++)
	{
		mColumnData->setField(CalcOffset(mMaterial[i].resID,  PrescriptionData),  DType_U32,        "材料ID");
		mColumnData->setField(CalcOffset(mMaterial[i].resNum, PrescriptionData),  DType_U8,         "材料数量");
	}
	mColumnData->setField(CalcOffset(mbaseItemID,      PrescriptionData),       DType_U32,        "基础产出物品ID"); 
	mColumnData->setField(CalcOffset(mbScript,         PrescriptionData),       DType_U8,         "前置脚本判断");
	mColumnData->setField(CalcOffset(mItemNum,         PrescriptionData),       DType_U8,         "一次产出个数");
	mColumnData->setField(CalcOffset(mName,            PrescriptionData),       DType_string,     "配方名称");
	for (int i=0; i<file.RecordNum; i++)
	{
		PrescriptionData* pData = new PrescriptionData;
		for (int j=0; j<file.ColumNum; j++)
		{
			file.GetData(lineData);
			mColumnData->setData(pData,j,lineData);
		}
		insert(pData);
	}
	file.ReadDataClose();
	return true; 
}
bool PrescriptionRepository::insert(PrescriptionData* pData)
{
	if(!pData)
		return false;

	mDataMap.insert(dataMap::value_type(pData->getSerialID(),pData));
	return true;
}
PrescriptionData* PrescriptionRepository::getPrescriptionData(U32 SerialID)
{
	dataMap::iterator it = mDataMap.find(SerialID);
	if(it != mDataMap.end())
		return it->second;
	else
		return NULL;
	
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Prescription g_Prescription;
Prescription::Prescription()
{
	mPrescriptionTab.clear();
}

U8 Prescription::getColResCount(PrescriptionData* pData)
{
	int iMatNum = 0;
	if(!pData)
		return iMatNum;

	PrescriptionData::stMaterial* vecMat = pData->getMaterial();

	AssertFatal(vecMat,"PrescriptionData::获取配方材料出错");
	if(!vecMat)
		return iMatNum;

	for (int i=0; i< PrescriptionData::MATERIAL_MAX; i++ )
	{
		if(vecMat[i].resID != 0 && vecMat[i].resNum != 0)
			iMatNum++;
	}
	return iMatNum;
}

U16 Prescription::canCompose(Player* player,U32 SerialID)
{
	PrescriptionData* pData = NULL;
	PrescriptionMap::iterator it = mPrescriptionTab.find(SerialID);

	if(it != mPrescriptionTab.end())
		pData = it->second;

	if(!pData && !player)
		return 0;

	U8 iMaxItem = getColResCount(pData);
	if(iMaxItem == 0)
		return COMPOSE_NONMAX;

	U16 iMaxComPoseNum = COMPOSE_MAX;
	U16 iCurMatCnt = 0;
	U16 iCurNum = 0;
	PrescriptionData::stMaterial* vecMat = pData->getMaterial();

	AssertFatal(vecMat,"PrescriptionData::获取配方材料出错");
	if(!vecMat)
		return 0;

	for (int i=0; i<iMaxItem; i++)
	{
		U16 iCurMatCnt = player->inventoryList.GetItemCount(vecMat[i].resID);
		iCurNum = iCurMatCnt / vecMat[i].resNum;
		iMaxComPoseNum = getMin(iMaxComPoseNum,iCurNum);
	}

	return iMaxComPoseNum;
}

U16 Prescription::getNeedResCount(PrescriptionData* pData,U8 index)
{
	if(pData)
	{
		AssertFatal((index >7 && index < 14) , "Prescription::Parameter Index Error");
		PrescriptionData::stMaterial* vecMat = pData->getMaterial();

		AssertFatal(vecMat,"PrescriptionData::获取配方材料出错");
		if (!vecMat)
			return 0;

		return vecMat[index].resNum;
	}

	return 0;
	
}

StringTableEntry Prescription::getNeedResName(PrescriptionData* pData,U8 index)
{
	if (pData)
	{
		AssertFatal((index >7 && index < 14) , "Prescription::Parameter Index Error");
		PrescriptionData::stMaterial* vecMat = pData->getMaterial();

		AssertFatal(vecMat,"PrescriptionData::获取配方材料出错");
		if(!vecMat)
			return NULL;

		ItemBaseData* pItemData = g_ItemRepository->getItemData(vecMat[index].resID);
		if(pItemData)
			return pItemData->getItemName();
	}

	return NULL;
}
bool Prescription::addPrescription(U32 SerialID)
{
	PrescriptionMap::iterator it = mPrescriptionTab.find(SerialID);
	if(it != mPrescriptionTab.end())
		return false;

	PrescriptionData* pData = g_PrescriptionRepository.getPrescriptionData(SerialID);
	if(pData)
	{
		mPrescriptionTab.insert(PrescriptionMap::value_type(SerialID,pData));
#ifdef NTJ_CLIENT
		LivingSkillData* pLivingSkillData = g_LivingSkillRespository.getLivingSkillData(pData->getLivingSkillID());
		if (pLivingSkillData)
			Con::executef("RefreshPrescriptionList",Con::getIntArg(pLivingSkillData->getCategory()-1));
#endif
		return true;
	}
	return false;
}

U16 Prescription::getPrescriptionCount()
{
	return mPrescriptionTab.size();
}

#ifdef NTJ_SERVER

bool Prescription::saveData(stPlayerStruct* playerInfo)
{
	if (!playerInfo || mPrescriptionTab.size() > PRESCRIPTION_MAX)
	{
		AssertFatal(false, "Prescription::SaveData error!!");
		return false;
	}
	playerInfo->MainData.PrescriptionCount = mPrescriptionTab.size();
	PrescriptionMap::iterator it = mPrescriptionTab.begin();
	for (int i=0; it != mPrescriptionTab.end(); it++,i++)
		playerInfo->MainData.PrescriptionInfo[i] = it->second->getSerialID();

	return true;
}

bool Prescription::updateToClient(GameConnection* conn,U32 mSerialID/* =0 */)
{
	if(!conn)
		return false;

	PrescriptionEvent* pEvent = NULL;

	if(mSerialID > 0)
	{
		pEvent =  new PrescriptionEvent;
		if(NULL == pEvent)
			return false;

		pEvent->AddIDToList(mSerialID);
		conn->postNetEvent(pEvent);
	}
	else     //初始化
	{
		PrescriptionMap::iterator it = mPrescriptionTab.begin();
		U8 iSize = mPrescriptionTab.size() / RECORD_MAXSIZE +1;
		for(int i=0; i<iSize; i++)
		{
			pEvent = new PrescriptionEvent;
			if(NULL == pEvent)
				return false;

			for (S32 j=0; j<RECORD_MAXSIZE && it != mPrescriptionTab.end(); j++,it++)
			{
				pEvent->AddIDToList(it->first);
			}
			conn->postNetEvent(pEvent);
		}		
	}

	return true;
}

#endif

ConsoleFunction(getPrescriptionData,const char*, 3, 3,"getPrescriptionData(%ID,%col)")
{
	PrescriptionData* pData = g_PrescriptionRepository.getPrescriptionData(dAtol(argv[1]));
	if(pData)
	{
		std::string to;
		if(g_PrescriptionRepository.mColumnData)
		{
			g_PrescriptionRepository.mColumnData->getData(pData,dAtol(argv[2]),to);
			char* value = Con::getReturnBuffer(512);
			dStrcpy(value, 512, to.c_str());
			return value;
		}
	}
	return  "";
}