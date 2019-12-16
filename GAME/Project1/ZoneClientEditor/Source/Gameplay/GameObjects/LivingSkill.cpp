//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/Data/readDataFile.h"
#include "Util/ColumnData.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "BuildPrefix.h"
#include "Gameplay/GameObjects/LivingSkill.h"
#include "Gameplay/ClientGameplayState.h"
#ifdef NTJ_SERVER
#include "Common/LogHelper.h"
#include "Common/Log/LogTypes.h"
#include "Gameplay/ServerGameplayState.h"
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LivingSkillData::LivingSkillData()
{
	mSkillID = 0;
	mlevel   = 0;
	mRipe    = 0;
	mRouteID = 0;
	mType    = 0;
	mCategory    = 0;
	mSubCategory = 0;
	mName     = StringTable->insert("");
	mIconName = StringTable->insert("");
	mDesc     = StringTable->insert("");
	dMemset(&mLearnLimit,0,sizeof(LearnLimit));
}
static char* szCategory[10] =
{
	"",
	"采集",
	"铸造",
	"裁缝",
	"工艺",
	"符咒",
	"炼丹",
	"烹饪",
	"工匠",
	"蛊术"
};
StringTableEntry LivingSkillData::getCategoryName()
{
	return szCategory[getCategory()];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LivingSkillRepository g_LivingSkillRespository;
LivingSkillRepository::LivingSkillRepository()
{
	mColumnData = NULL;
}
LivingSkillRepository::~LivingSkillRepository()
{
	clear();
}
bool LivingSkillRepository::read()
{
	CDataFile file;
	RData lineData;
	char filename[1024];
	file.ReadDataInit();
	Platform::makeFullPathName(LIVINGSKILLFILE, filename, sizeof(filename));

	if (!file.readDataFile(filename))
	{
		file.ReadDataClose();
		AssertRelease(false, "can't read file : LivingSkillRepository.dat!");
		return false;
	}

	mColumnData = new IColumnData(file.ColumNum, "LivingSkillRepository.dat");
	mColumnData->setField(CalcOffset(mSkillID,         LivingSkillData),    DType_U32,           "生活技能ID");
	mColumnData->setField(CalcOffset(mName,            LivingSkillData),    DType_string,        "生活技能名称");
	mColumnData->setField(CalcOffset(mCategory,        LivingSkillData),    DType_enum8,         "生活技能类别");
	mColumnData->setField(CalcOffset(mSubCategory,     LivingSkillData),    DType_enum16,        "生活技能子类");
	mColumnData->setField(CalcOffset(mType,            LivingSkillData),    DType_enum8,         "生活技能类型");
	mColumnData->setField(CalcOffset(mlevel,           LivingSkillData),    DType_U16,           "生活技能等级");
	mColumnData->setField(CalcOffset(mRipe,            LivingSkillData),    DType_U32,           "生活技能熟练度");
	mColumnData->setField(CalcOffset(mIconName,        LivingSkillData),    DType_string,        "生活技能ICON");
	mColumnData->setField(CalcOffset(mDesc,            LivingSkillData),    DType_string,        "生活技能描述");
	mColumnData->setField(CalcOffset(mRouteID,         LivingSkillData),    DType_U32,           "学习技能寻径ID");

	mColumnData->setField(CalcOffset(mLearnLimit.prepSkill,     LivingSkillData),      DType_U32,         "学习前置技能");
	mColumnData->setField(CalcOffset(mLearnLimit.level,         LivingSkillData),      DType_U32,         "玩家学习等级");
	mColumnData->setField(CalcOffset(mLearnLimit.money,         LivingSkillData),      DType_U32,         "学习金钱");
	mColumnData->setField(CalcOffset(mLearnLimit.ripe,          LivingSkillData),      DType_U32,         "学习熟练度");
	mColumnData->setField(CalcOffset(mLearnLimit.exp,           LivingSkillData),      DType_U32,         "经验消耗");
	mColumnData->setField(CalcOffset(mLearnLimit.title,         LivingSkillData),      DType_U32,         "玩家称号ID");
	mColumnData->setField(CalcOffset(mLearnLimit.scriptFunction,LivingSkillData),      DType_string,      "学习外接脚本判断");

	mColumnData->setField(CalcOffset(mCost.mode,                LivingSkillData),      DType_enum8,        "消耗方式");
	mColumnData->setField(CalcOffset(mCost.itemSubCategory,     LivingSkillData),      DType_U16,          "道具子类");
	mColumnData->setField(CalcOffset(mCost.itemUseTimes,        LivingSkillData),      DType_U16,          "道具使用次数");
	mColumnData->setField(CalcOffset(mCost.vigour,              LivingSkillData),      DType_U32,          "活力消耗");
	mColumnData->setField(CalcOffset(mCost.insight,             LivingSkillData),      DType_U32,          "悟性限制");

	mColumnData->setField(CalcOffset(mCast.cooldown,            LivingSkillData),      DType_U32,          "施放CD时间");
	mColumnData->setField(CalcOffset(mCast.readyTime,           LivingSkillData),      DType_U32,          "施放吟唱时间");
	mColumnData->setField(CalcOffset(mCast.cdGroup,             LivingSkillData),      DType_S16,          "施放CD组");
	mColumnData->setField(CalcOffset(mCast.readyBeginAction,    LivingSkillData),      DType_enum16,       "吟唱动作开始");
	mColumnData->setField(CalcOffset(mCast.readyLoopAction,     LivingSkillData),      DType_enum16,       "吟唱动作循环");
	mColumnData->setField(CalcOffset(mCast.readyEndAction,      LivingSkillData),      DType_enum16,       "吟唱动作结束");

	mColumnData->setField(CalcOffset(mGuerdon.type,             LivingSkillData),      DType_enum8,        "奖励类型");
	mColumnData->setField(CalcOffset(mGuerdon.num,              LivingSkillData),      DType_U8,           "奖励数量");
	
	for (int i=0; i<file.RecordNum; i++)
	{
		LivingSkillData* pData = new LivingSkillData;
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

void LivingSkillRepository::clear()
{
	if(mColumnData)
	{
		delete mColumnData;
		mColumnData = NULL;
	}

	LivingSkillMap::iterator it = mLivingSkillMap.begin();
	for (; it != mLivingSkillMap.end() ; ++it)
	{
		if(it->second)
			delete it->second;
	}

	mLivingSkillMap.clear();
}

bool LivingSkillRepository::insert(LivingSkillData* data)
{
	if(!data)
		return false;
	mLivingSkillMap.insert(LivingSkillMap::value_type(data->mSkillID, data));
	return true;
}

LivingSkillData* LivingSkillRepository::getLivingSkillData(U32 id)
{
	LivingSkillMap::iterator it = mLivingSkillMap.find(id);
	if(it != mLivingSkillMap.end())
		return it->second;
	else
		return NULL;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LivingSkill::LivingSkill()
{
	mRipe = 0;
	mLivingSkillData = NULL;
}
LivingSkill::~LivingSkill()
{
	mLivingSkillData = NULL;
}
LivingSkill* LivingSkill::create(LivingSkillData* pData)
{
	if(pData)
	{
		LivingSkill* pLivingSkill = new LivingSkill();
		pLivingSkill->mLivingSkillData = pData;
		return pLivingSkill;
	}
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enWarnMessage LivingSkillTable::canLearn(Player* player,U32 skillID)
{
	if(!player)
		return MSG_UNKOWNERROR;

	LivingSkillData* pData = g_LivingSkillRespository.getLivingSkillData(skillID);
	if(!pData)
		return MSG_LIVINGSKILL_DATAERROR;

	if(isLearn(skillID))
		return MSG_LIVINGSKILL_LEARNDONE;

	LivingSkillTableMap::iterator it = mLivingSkillTable.find(GETSUBCATEGORY(pData->mLearnLimit.prepSkill));
	
	//通用判断
	if(pData->getLevel() == 1 && pData->getType() == LivingSkillData::LIVINGSKILL_NORM)
	{
		if(player->getLevel() < pData->mLearnLimit.level)       // 等级
			return MSG_PLAYER_LEVEL;
		if(player->getExp() < pData->mLearnLimit.exp)           // 经验
			return MSG_PLAYER_EXP;
		if(!player->canReduceMoney(pData->mLearnLimit.money, 21))       // 金钱
			return MSG_PLAYER_MONEYNOTENOUGH;
		//if(player->getTitle() < pData->mLearnLimit.title)     // 称号
		//	return MSG_PLAYER_TITLE;
	}

	U32 tempID = player->getLivingSkillPro();
	if (it != mLivingSkillTable.end())
	{
		LivingSkill* pSkill = it->second;
		if(!pSkill || !pSkill->getData())
			return MSG_LIVINGSKILL_DATAERROR;
		//前置技能
		if(pData->mLearnLimit.prepSkill && pSkill->getData()->getID() < pData->mLearnLimit.prepSkill)    
			return MSG_LIVINGSKILL_PREPERROR;
		//熟练度
		if (pSkill->getRipe() < pData->mLearnLimit.ripe)
			return MSG_LIVINGSKILL_RIPE;

		//专精技能
		if ((pData->getType() == LivingSkillData::LIVINGSKILL_PRO) && (tempID>0) && (GETSUBCATEGORY(tempID) != GETSUBCATEGORY(skillID)))
		{
			it = mLivingSkillTable.find(GETSUBCATEGORY(tempID));
			if (it != mLivingSkillTable.end())
			{
				LivingSkill* pSkill = it->second;
				if(!pSkill || !pSkill->getData())
					return MSG_LIVINGSKILL_DATAERROR;

				//专精技能是否为满级
				pData = g_LivingSkillRespository.getLivingSkillData(tempID+1);
				if(pData || (pSkill->getRipe() != pSkill->getData()->getRipe()))
					return MSG_LIVINGSKILL_PRO;
			}
		}
	}
	//该专精技能的前置技能还没有学
	else if(it == mLivingSkillTable.end() && pData->getType() == LivingSkillData::LIVINGSKILL_PRO)
	{
		return MSG_LIVINGSKILL_PREPERROR;
	}
	return MSG_NONE;
}
bool LivingSkillTable::learnLivingSkill(Player* player,U32 skillID)
{
	if(NULL == player)
		return false;

	GameConnection* conn = player->getControllingClient();
	if(NULL == conn)
		return false;

	enWarnMessage msg = canLearn(player,skillID);

#ifdef NTJ_CLIENT
	if (msg == MSG_NONE)
	{
		ClientGameNetEvent* mEvent = new ClientGameNetEvent(INFO_LIVINGSKILL_LEARN);
		mEvent->SetInt32ArgValues(1,skillID);
		conn->postNetEvent(mEvent);
		return true;
	}
	else
	{
		MessageEvent::show(SHOWTYPE_ERROR,msg);
	}
#endif

#ifdef NTJ_SERVER
	if (msg == MSG_NONE)
	{
		LivingSkillData* pData = NULL;
		LivingSkill* pSkill = addLivingSkill(skillID);
		if(pSkill && (pData = pSkill->getData()))
		{
			LivingSkillData::LearnLimit &stLearnLimit = pData->getLearnLimit();
			S32 iExp   = stLearnLimit.exp;
			player->reduceMoney(stLearnLimit.money, 21);
			player->addExp(-iExp); 

			updateToClient(conn,skillID,true);
			//学会生活技能回调脚本，用与增加配方等操作
			Con::executef("CallbackLearnLivingSkill",
				Con::getIntArg(player->getId()),
				Con::getIntArg(skillID));

			//记录专精技能
			if(pData && pData->getType() == LivingSkillData::LIVINGSKILL_PRO)
				player->setLivingSkillPro(skillID);

            //log it
            lg_livingskill_learn lg;

            lg.skillName  = pSkill->getData()->mName;
            lg.costGold   = stLearnLimit.money;
            lg.costExp    = iExp;
            lg.playerName = player->getPlayerName();
            
            g_ServerGameplayState->GetLog()->writeLog(&lg);
			return true;
		}
	}
	else
	{
		MessageEvent::send(conn,SHOWTYPE_ERROR,msg);
	}
#endif
	
	return false;
}

#ifdef NTJ_SERVER
bool LivingSkillTable::saveData(stPlayerStruct* playerInfo)
{
	if (!playerInfo || mLivingSkillTable.size() > LIVINGSKILL_MAXSLOTS)
	{
		AssertFatal(false, "LivingSkillTable::SaveData error!!");
		return false;
	}

	LivingSkillTableMap::iterator it = mLivingSkillTable.begin();

	for (U8 i=0; it != mLivingSkillTable.end(); ++it,i++)
	{
		playerInfo->MainData.LivingSkillInfo[i].SkillID = it->second->mLivingSkillData->getID();
		playerInfo->MainData.LivingSkillInfo[i].Ripe    = it->second->mRipe;
	}
	return true;
}

bool LivingSkillTable::updateToClient(GameConnection* conn,U32 skillID,bool bAddSkill)
{
	if(!conn)
		return false;

	LivingSkillEvent* Event = new LivingSkillEvent(skillID,bAddSkill);
	conn->postNetEvent(Event);
	return true;
}

#endif

LivingSkill* LivingSkillTable::addLivingSkill(U32 skillID)
{
	return addLivingSkill(g_LivingSkillRespository.getLivingSkillData(skillID));
}

LivingSkill* LivingSkillTable::addLivingSkill(U32 skillID,U32 currRipe)
{
	return addLivingSkill(g_LivingSkillRespository.getLivingSkillData(skillID),currRipe);
}

LivingSkill* LivingSkillTable::addLivingSkill(LivingSkillData *pData,U32 currRipe)
{
	if(!pData)
		return NULL;

	LivingSkillTableMap::iterator it = mLivingSkillTable.begin();
	for(; it != mLivingSkillTable.end(); it++)
	{
		if(it->second->mLivingSkillData->getSubCategory() == pData->getSubCategory())
		{
			if(currRipe == 0)
				currRipe = it->second->mRipe;

			if (it->second)
				delete it->second;

			mLivingSkillTable.erase(it);
			break;
		}
	}

	LivingSkill* pSkill = LivingSkill::create(pData);
	if (pSkill)
	{
		pSkill->mRipe = currRipe;
		mLivingSkillTable.insert(LivingSkillTableMap::value_type(GETSUBCATEGORY(pData->getID()),pSkill));
		return pSkill;
	}
	return NULL;
}
void LivingSkillTable::removeLivingSkill(U32 skillID)
{
	LivingSkillTableMap::iterator it = mLivingSkillTable.find(GETSUBCATEGORY(skillID));

	if(it != mLivingSkillTable.end())
	{
		delete it->second;
		mLivingSkillTable.erase(it);
	}
}
#ifdef NTJ_CLIENT

void LivingSkillTable::updateLiveingSkillView(U32 skillId)
{
	LivingSkillData* pData  = g_LivingSkillRespository.getLivingSkillData(skillId);
	if(!pData)
		return;

	if (pData->getID() < LIVINGSKILL_ID_LIMIT) //过滤生活技能大类
		return;

	U32 iTab = pData->getCategory()-1; 
	//不在界面显示,特殊处理
	if(iTab >= MAXTAB)
		return;

	Vector<U32>::iterator it = mLivingSkillView[iTab].begin();
	for (; it != mLivingSkillView[iTab].end(); it++)
	{
		if(GETSUBCATEGORY(*it) == GETSUBCATEGORY(skillId))
			*it = skillId;
	}
}

void LivingSkillTable::initLivingSkillPreview()
{
	LivingSkillData* pData = NULL;
	LivingSkillRepository::LivingSkillMap::iterator it = g_LivingSkillRespository.mLivingSkillMap.begin();
	for (; it !=g_LivingSkillRespository.mLivingSkillMap.end(); ++it)
	{
		pData = it->second;
		if(!pData)
			continue;

		if (pData->getID() < LIVINGSKILL_ID_LIMIT) //过滤生活技能大类
			continue;

		U8 iTab = pData->getCategory()- 1; 
		//不在界面显示,特殊处理
		if(iTab >= MAXTAB)
			continue;

		LivingSkillTable::LivingSkillTableMap::iterator itr = mLivingSkillTable.find(GETSUBCATEGORY(pData->getID()));
		if(itr != mLivingSkillTable.end())
		{
			U32 skillId = itr->second->mLivingSkillData->getID();

			if(skillId == pData->getID() || pData->getType() == LivingSkillData::LIVINGSKILL_PRO)
				mLivingSkillView[iTab].push_back(skillId);
		}
		else
		{
			if(findViewByID(iTab,pData->getID()) || pData->getLevel() > 1)
				continue;
			else
				mLivingSkillView[iTab].push_back(pData->getID());
		}
		std::sort(mLivingSkillView[iTab].begin(),mLivingSkillView[iTab].end());
	}
}

bool LivingSkillTable:: findViewByID(U8 iTab,U32 skillId)
{
	AssertFatal((iTab>=0 && iTab<MAXTAB),"findViewByID::函数非法参数");
	Vector<U32>::iterator it = mLivingSkillView[iTab].begin();
	LivingSkillData* pData = g_LivingSkillRespository.getLivingSkillData(skillId);
	LivingSkillData* pTempData = NULL;

	for (; it != mLivingSkillView[iTab].end(); it++)
	{
		pTempData = g_LivingSkillRespository.getLivingSkillData(*it);
		if(!pTempData || !pData)
			return false;

		if((pTempData->getSubCategory() == pData->getSubCategory()) && (pTempData->getType() == pData->getType()))
			return true;
	}
	return false;
}

Vector<U32>& LivingSkillTable::getLivingSkillView(U8 index)
{
	AssertFatal((index>=0 && index<MAXTAB),"getLivingSkillVie::函数非法参数");
	return mLivingSkillView[index];
}

#endif

void LivingSkillTable::clear()
{
	LivingSkillTableMap::iterator it = mLivingSkillTable.begin();
	for (; it != mLivingSkillTable.end(); ++it)
	{
		if (it->second)
			delete it->second;
	}
	mLivingSkillTable.clear();
#ifdef NTJ_CLIENT
	for(int i=0; i<MAXTAB; i++)
		mLivingSkillView[i].clear();
#endif
}
LivingSkillTable::LivingSkillTable()
{
#ifdef NTJ_CLIENT
	for(int i=0; i<MAXTAB; i++)
	{
		mLivingSkillView[i].clear(); 
		mLivingSkillView[i].push_back(LIVINGSKILL_ID_CATEGORYLIMIT + i);
	}
#endif
}
LivingSkillTable::~LivingSkillTable()
{
	clear();
}

bool LivingSkillTable::isLearn(U32 skillID)
{
	LivingSkillData* pData = g_LivingSkillRespository.getLivingSkillData(skillID);
	if(!pData)
		return false;

	LivingSkillTableMap::iterator it = mLivingSkillTable.find(GETSUBCATEGORY(skillID));
	if(it != mLivingSkillTable.end())
	{
		LivingSkill* pSkill = it->second;
		if(NULL == pSkill)
			return false;

		LivingSkillData* pTabData = pSkill->mLivingSkillData;
		if(NULL ==  pTabData)
			return false;

		if( pTabData->getType() == pData->getType() && pTabData->getLevel() >= pData->getLevel())
			return true;
		else
			return false;
	}
	else
	{
		return false;
	}
}

LivingSkill* LivingSkillTable::getLivingSkill(U32 skillID)
{
	LivingSkillTableMap::iterator it  = mLivingSkillTable.find(skillID);
	if(it != mLivingSkillTable.end())
		return it->second;
	else
		return NULL;
}

#ifdef NTJ_CLIENT
//通过列数获取LivingSkill字段值
ConsoleFunction( getLivingSkillData, const char*, 3, 3, "getLivingSkillData(%Skillid, %col)")
{
	LivingSkillData* data = g_LivingSkillRespository.getLivingSkillData(dAtol(argv[1]));
	if(data)
	{
		std::string to;
		g_LivingSkillRespository.mColumnData->getData(data, dAtoi(argv[2]), to);
		char* value = Con::getReturnBuffer(512);
		dStrcpy(value, 512, to.c_str());
		return value;		
	}
	return ""; 
}

ConsoleFunction(IsCanLearnLivingSkill,S32, 2, 2, "IsCanLearnLivingSkill(%SkillId)" )
{
	 Player* player = g_ClientGameplayState->GetControlPlayer();
	 if (player)
		 return player->pLivingSkill->canLearn(player,atoi(argv[1]));
	 return 0;
}
#endif


