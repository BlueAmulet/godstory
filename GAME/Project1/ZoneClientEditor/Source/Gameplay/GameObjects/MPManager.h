//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _MPMANAGER_H_
#define _MPMANAGER_H_

#include "platform/types.h"

#define  MAXEXP_POOL    0x4C4B400  //最大目前8000万
struct stMPTable
{
	StringTableEntry     mTitleName;           // 资格名称
	U8                   mMaxPrenticeNum;      // 收徒数量
	U32                  mNeedExploit;         // 所需师德
	U16                  mTradeExp;            // 师德可兑换经验值
	U8                   mPrizeExp;            // 师徒组队奖励
	U16                  mOtherPrize;          // 其它奖励
};

class Player;
class MPManager
{
public:
	enum enConstants
	{
		MaxMasterLevel = 6,    // 师傅最大资格等级
	};

	enum updateType
	{
		MASTERLEVEL    = BIT(0),       // 等级
		MASTEREXPLOIT  = BIT(1),       // 师德
		EXPPOOL        = BIT(2),       // 经验
		PRENTICEFLAGE  = BIT(3),       // 徒状态
		LEAVEMASTER    = BIT(4),       // 出师
		CALLBOARDTEXT  = BIT(5),       // 师傅公告板信息
		STUDYLEVEL     = BIT(6),       // 拜师等级
		PRENTICENUM    = BIT(7),       // 徒弟数量
	};

protected:
	U8                   mMasterLevel;         // 师傅资格等级
	U8                   mCurrPrenticeNum;     // 当前拥有徒弟数量
	U32                  mCurrMasterExp;       // 当前师德
	U32                  mExpPool;             // 经验池 
	bool                 mbPrentice;           // 是否徒状态
	bool                 mbFarPrentice;        // 出师标记
	U16                  mStudyLevel;          // 拜师等级
	StringTableEntry     mCallboardString;     // 师傅公告  
	static stMPTable     MPManagerData[];

public:
	MPManager();

	inline   U8                getMasterLevel()               { return mMasterLevel;}
	inline   void              setMasterLevel(U8 level)       { mMasterLevel = level;}
	inline   U8                getCurrPrenticeNum()           { return mCurrPrenticeNum;}
	inline   U32               getCurrMasterExp()             { return mCurrMasterExp;}
	inline   void              setCurrMasterExp(U32 exp)      { mCurrMasterExp = exp;}
	inline   U32               getExpPool()                   { return mExpPool;}
	inline   void              setExpPool(U32 exp)            { mExpPool = exp;}
	inline   bool              getPrenticeState()             { return mbPrentice; }
	inline   bool              getFarPrentice()               { return mbFarPrentice;}
	inline   U16               getStudyLevel()                { return mStudyLevel;}
	inline   StringTableEntry  getMastermTitle();
	inline   StringTableEntry  getCallboardString()           { return mCallboardString;}
	inline   static stMPTable  getData(U8 level)              { return MPManagerData[level];}

	void                       setPrenticeNum(Player* pPlayer,S8 num);                  // 徒弟数量
	void                       setStudyLevel(Player* pPlayer,U16 level);                // 记录拜师等级
	void                       setCallboardString(const char* string);                  // 更新师傅公告板信息
	void                       setPrenticeState(Player* pPlayer,bool flage);            // 设置徒弟标记
	void                       setFarPrentice(Player* pPlayer,bool flage);              // 设置出师标记
	void                       addMasterLevel(Player* pPlayer,S32 level);               // 增加资格              
	void                       addMasterExploit(Player* pPlayer,S32 exp);               // add师德
	void                       addExpPool(Player* pPlayer,S32 exp,S32 friendValue=0);   // add经验
	void                       tradeExp(Player* pPlayer,S32 expPool);                   // 用师德对换经验沲经验
	enWarnMessage              canMaster(Player* pPlayer);                              // 判断成为师傅

#ifdef NTJ_SERVER
	enWarnMessage              recruitPrentice(Player* pPlayer,Player* destPlayer);   // 判断能否收徒
	enWarnMessage              followMaster(Player* pPlayer,Player* destPlayer);      // 判断能否拜师
	void                       createRelation(Player* pPlayer,SocialType::Type type); // 建立师徒关系 
	void                       destroyRelation(Player* pPlayer,S32 destPlayerId,SocialType::Type type);  // 解除师徒关系
	bool                       sendClientRequest(Player* pPlayer,Player* destPlayer,SocialType::Type type);
	void                       initialize(stPlayerStruct *pPlayerData);
	bool                       saveData(stPlayerStruct *pPlayerData);
	bool                       sendInitialData(Player* pPlayer);
	bool                       sendEvent(Player* pPlayer,U16 mask, bool isFirstUpdate = false) ;                      
	void                       calcMasterExp(Player* pPlayer,NpcObject* npc,F32 expDist);             // 获取师德
	void                       calcAttachPrize(Player* pPlayer);           // 额外奖励
	bool                       autoLeaveMaster(Player* pPlayer);           // 徒弟出师
	Player*                    getMasterPlayer(Player* pPlayer);           // 获取自己的师傅
	U32                        getMasterPlayerId(Player* pPlayer);         // 获取自己师傅的ID
	void                       getPrenticeList(Player* pPlayer,Vector<int>& listId);           // 获取自己的徒弟列表
#endif
#ifdef NTJ_CLIENT
	bool                       sendZoneResonse(Player* pPlayer,Player* destPlayer,SocialType::Type type);
	bool                       sendZoneChangeText(Player* pPlayer,const char* str);
#endif
};

#endif