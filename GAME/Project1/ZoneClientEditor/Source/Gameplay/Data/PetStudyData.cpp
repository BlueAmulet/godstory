#include "console/consoleTypes.h"
#include "console/console.h"
#include "Util/ColumnData.h"
#include "platform/platform.h"
#include "Gameplay/data/readDataFile.h"
#include "Gameplay/Data/PetStudyData.h"

#ifdef NTJ_CLIENT
#include "ui/guiTextListCtrlEx.h"
#endif
// ========================================================================================================================================
//	PetStudyRepository
// ========================================================================================================================================

PetStudyRepository g_PetStudyRepository;

PetStudyRepository::PetStudyRepository()
{
	m_PetStudyDataMap.clear();
}

PetStudyRepository::~PetStudyRepository()
{
	Clear();
	if(mColumnData)
		delete mColumnData;
}

bool PetStudyRepository::Read()
{
	CDataFile file;
	RData tempData;
	char filename[1024];

	// 读取模型数据
	Platform::makeFullPathName("gameres/data/Repository/PetStudyRepository.dat",filename,sizeof(filename));
	file.ReadDataInit();
	AssertRelease(file.readDataFile(filename), "can't read file : PetStudyRepository.dat!");

	mColumnData = new IColumnData(file.ColumNum, "PetStudyRepository.dat");
	mColumnData->setField(CalcOffset(mId,				PetStudyData),	DType_U32,			"修行编号");
	mColumnData->setField(CalcOffset(mName,				PetStudyData),	DType_string,		"修行名称");
	mColumnData->setField(CalcOffset(mType,				PetStudyData),	DType_enum8,		"修行类型");
	mColumnData->setField(CalcOffset(mPetLevel,			PetStudyData),	DType_U32,			"宠物等级");
	mColumnData->setField(CalcOffset(mStudyTime,		PetStudyData),	DType_U32,			"修行时间");
	mColumnData->setField(CalcOffset(mTimeFlag,			PetStudyData),	DType_enum8,		"计时方式");
	mColumnData->setField(CalcOffset(mCostMoney,		PetStudyData),	DType_U32,			"消耗游戏币");
	mColumnData->setField(CalcOffset(mCostHappiness,	PetStudyData),	DType_U32,			"消耗快乐度");
	mColumnData->setField(CalcOffset(mRewardMoney,		PetStudyData),	DType_U32,			"奖励游戏币");
	mColumnData->setField(CalcOffset(mRewardPlayerExp,	PetStudyData),	DType_U32,			"奖励人物经验");
	mColumnData->setField(CalcOffset(mRewardPetExp,		PetStudyData),	DType_U32,			"奖励宠物经验");

	for(int k = 0; k < file.RecordNum; ++k)
	{
		PetStudyData* pData = new PetStudyData;
		for(int h = 0; h < file.ColumNum; ++h)
		{
			file.GetData(tempData);
			mColumnData->setData(pData, h, tempData);
		}
		Insert(pData);
	}
	file.ReadDataClose();
	return true;
}

bool PetStudyRepository::Insert(PetStudyData* pData)
{
	if(!pData)
		return false;

	return m_PetStudyDataMap.insert(PetStudyDataMap::value_type(pData->mId, pData)).second;
}

void PetStudyRepository::Clear()
{
	PetStudyDataMap::const_iterator it = m_PetStudyDataMap.begin();
	for(; it != m_PetStudyDataMap.end(); ++it )
	{
		if(it->second)
			delete  it->second;
	}
	m_PetStudyDataMap.clear();
}

PetStudyData*  PetStudyRepository::GetPetStudyData(U32 PetID)
{
	PetStudyDataMap::const_iterator it = m_PetStudyDataMap.find(PetID);
	if(it == m_PetStudyDataMap.end())
		return NULL;
	return it->second;
}

S32 PetStudyRepository::GetPetStudyDataCount()
{
	return (S32)m_PetStudyDataMap.size();
}

#ifdef NTJ_CLIENT
ConsoleFunction(PetStudyData_Initialize, void, 1, 1, "PetStudyData_Initialize();")
{
	GuiTextListCtrlEx *pList = dynamic_cast<GuiTextListCtrlEx*>(Sim::findObject("PetStudyWnd_List"));
	if (!pList || pList->getRowCount() != 0)
		return;

	PetStudyRepository::PetStudyDataMap *pMap = g_PetStudyRepository.GetPetStudyDataMap();
	PetStudyRepository::PetStudyDataMap::iterator it = pMap->begin();
	while(it != pMap->end())
	{
		PetStudyData *pPetStudyData = it->second;
		//pPetStudyData->mName;		//修行名称
		char sStudyType[20];		//修行类型
		if (pPetStudyData->mType == 1)
		{
			dStrcpy(sStudyType, sizeof(sStudyType), "自身修行");
		}
		else if (pPetStudyData->mType == 2)
		{
			dStrcpy(sStudyType, sizeof(sStudyType), "代主修行");
		}
		else if (pPetStudyData->mType == 3)
		{
			dStrcpy(sStudyType, sizeof(sStudyType), "代主打工");
		}
		char sPetLevel[10];		//宠物等级
		dSprintf(sPetLevel, sizeof(sPetLevel), "%u级", pPetStudyData->mPetLevel);
		char sStudyCost[40];	//快乐度消耗
		dSprintf(sStudyCost, sizeof(sStudyCost), "快乐度%u", pPetStudyData->mCostHappiness);
		U32 size = dStrlen(sStudyCost);	//游戏币消耗
		dSprintf(sStudyCost+size, sizeof(sStudyCost)-size, "\n游戏币%u", pPetStudyData->mCostMoney);
		
		char sStudyReward[40];	//修行收获
		if (pPetStudyData->mType == 1)	//自身修行
		{
			dSprintf(sStudyReward, sizeof(sStudyReward), "经验%u", pPetStudyData->mRewardPetExp);
		}
		else if (pPetStudyData->mType == 2)	//代主修行
		{
			dSprintf(sStudyReward, sizeof(sStudyReward), "经验%u", pPetStudyData->mRewardPlayerExp);
		}
		else if (pPetStudyData->mType == 3)	//代主打工
		{
			dSprintf(sStudyReward, sizeof(sStudyReward), "游戏币%u", pPetStudyData->mRewardMoney);
		}

		char sStudyTime[20];	//修行时间
		dSprintf(sStudyTime, sizeof(sStudyTime), "%u分钟", pPetStudyData->mStudyTime);

		size =  dStrlen(pPetStudyData->mName) + 1 +
				dStrlen(sStudyType) + 1 + 
				dStrlen(sPetLevel) + 1 + 
				dStrlen(sStudyCost) + 1 +
				dStrlen(sStudyReward) + 1 +
				dStrlen(sStudyTime) + 1;
		
		char *text = Con::getArgBuffer(size);
		dMemset(text, 0, size);
		if (text)
		{
			dSprintf(text, size, "%s\t%s\t%s\t%s\t%s\t%s", pPetStudyData->mName, sStudyType,
														 sPetLevel, sStudyCost, sStudyReward, sStudyTime);
			pList->addEntry(pPetStudyData->mId, text);
		}

		++it;
	}
}
#endif