//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _LIVINGSKILL_H_ 
#define _LIVINGSKILL_H_
#include <hash_map>
#include "Gameplay/GameObjects/GameObjectData.h"
#define SUB(a,b) a * 100 + b
#define GETSUBCATEGORY(skillId) (skillId/10000)%10000
#define LIVINGSKILL_ID_CATEGORYLIMIT   500000001
#define LIVINGSKILL_ID_LIMIT           501000000

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LivingSkillData
{
public:
	enum Constants
	{ 
		GATHER                  = 1,                 // 采集
		PRODUCE_FOUNDRY         = 2,                 // 铸造
		PRODUCE_NEEDLEMAN       = 3,                 // 裁缝
		PRODUCE_TECHNICS        = 4,                 // 工艺
		PRODUCE_CHARM           = 5,                 // 符咒
		PRODUCE_ELIXIR          = 6,                 // 炼丹
		PRODUCE_COOKING         = 7,                 // 烹饪
		PRODUCE_ARTISAM         = 8,                 // 工匠
		PRODUCE_POISON          = 9,                 // 蛊术

		GATHER_MINING           = SUB(GATHER,1),     // 采矿
		GATHER_FELLING          = SUB(GATHER,2),     // 伐木
		GATHER_FISHING          = SUB(GATHER,3),     // 钓鱼
		GATHER_PLANTING         = SUB(GATHER,4),     // 种植
		GATHER_HERB             = SUB(GATHER,5),     // 采药
		GATHER_SHIKAR           = SUB(GATHER,6),     // 狩猎
	};

	enum LivingSkillType
	{
		LIVINGSKILL_NORM = 1,             // 普通技能
		LIVINGSKILL_PRO ,                 // 专精技能
	};

	enum CastMode
	{
		CAST_NOTHING = 0,                // 无消耗
		CAST_NOCAST,                     // 不消耗
		CAST_ITEMUSETIMES,               // 使用次数
		
		
	};
	enum GuerdonType
	{
		GUERDON_RIPE,                   // 熟练度
	};

	struct LearnLimit 
	{
		U32 prepSkill;			 // 前置技能
		U32 level;				 // 等级
		U32 money;				 // 金钱
		U32 ripe;                // 熟练度
		U32 exp;				 // 经验
		U32 title;               // 称号
		StringTableEntry scriptFunction;	// 脚本判断
	};
	
	struct Cost//Cast
	{
		U8  mode;                // 消耗方式
		U16 itemSubCategory;     // 道具子类
		U16 itemUseTimes;        // 使用次数
		U32 vigour;              // 活力
		U32 insight;             // 悟性
	}; 

	struct Cast//Cost
	{
		SimTime cooldown;        // CD时间
		SimTime readyTime;       // 吟唱时间
		S32 cdGroup;			 // CD组
		GameObjectData::Animations readyBeginAction;      //吟唱动作开始
		GameObjectData::Animations readyLoopAction;       //吟唱动作循环
		GameObjectData::Animations readyEndAction;        //吟唱动作结束
	};

	struct Guerdon
	{
		U8  type;                   // 奖励类型
		U8  num;                    // 奖励数量
	};

	friend class LivingSkillRepository;
	friend class LivingSkillTable;

protected:
	U32               mSkillID;             // 编号
	U16               mlevel;               // 等级
	U32               mRipe;                // 熟练度
	U32               mRouteID;             // 寻径ID
	U16				  mCategory;			// 技能类别
	U16				  mSubCategory;			// 技能子类别
	U8                mType;                // 技能类型
	StringTableEntry  mName;                // 名称
	StringTableEntry  mIconName;            // Icon
	StringTableEntry  mDesc;                // 描述
	LearnLimit        mLearnLimit;          // 学习限制
	Cost              mCost;                // 施放
	Cast              mCast;                // 消耗
	Guerdon           mGuerdon;             // 奖励

public:
	LivingSkillData();

	inline U32                getID             ()          { return mSkillID; }
	inline U32                getRipe           ()          { return mRipe; }
	inline U16                getLevel          ()          { return mlevel; }
	inline U16                getCategory       ()          { return mCategory;}       
	inline U16                getSubCategory    ()          { return mSubCategory;}
	inline U8                 getType           ()          { return mType; }
	inline StringTableEntry   getName           ()          { return mName; }
	inline StringTableEntry   getIconName       ()          { return mIconName; }
	inline StringTableEntry   getDesc           ()          { return mDesc; }
	inline LearnLimit&        getLearnLimit     ()          { return mLearnLimit; }
	inline Cost&              getCost           ()          { return mCost; }
	inline Cast&              getCast           ()          { return mCast; }
	inline Guerdon&           getGuerdon        ()          { return mGuerdon; }
	StringTableEntry          getCategoryName   ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IColumnData;
class LivingSkillRepository
{
public:
	typedef stdext::hash_map<U32, LivingSkillData*> LivingSkillMap;
	LivingSkillMap mLivingSkillMap;
	IColumnData* mColumnData;
public:
	LivingSkillRepository();
	~LivingSkillRepository();

	bool	          read();
	void	          clear();
	bool	          insert(LivingSkillData* pData);
	LivingSkillData*  getLivingSkillData(U32 SkillID);
};
extern LivingSkillRepository g_LivingSkillRespository;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LivingSkill
{
	friend class LivingSkillTable;
protected:
	U32              mRipe;                // 熟练度
	LivingSkillData* mLivingSkillData;
public:
	LivingSkill();
	~LivingSkill();
	inline U32              getRipe() { return mRipe; }
	inline void             setRipe(U32 Value) { mRipe = Value; }
	inline LivingSkillData* getData() { return mLivingSkillData; }
	void                    setData(LivingSkillData* pData) { mLivingSkillData = pData;}
private:
	static LivingSkill*     create(LivingSkillData* pData);
	
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Player;
struct stPlayerStruct;
class GameConnection;
class LivingSkillTable
{
public:
	enum
	{
		MAXTAB = 9,
	};
	typedef stdext::hash_map<U32, LivingSkill*> LivingSkillTableMap;
private:
	LivingSkillTableMap  mLivingSkillTable;    // 已学技能表
#ifdef NTJ_CLIENT
	Vector<U32> mLivingSkillView[MAXTAB];  // 技能预览表
#endif
public:
	LivingSkillTable();
	~LivingSkillTable();
	enWarnMessage	canLearn(Player* player, U32 skillID);
	bool            learnLivingSkill(Player* player, U32 skillID);
	bool            isLearn(U32 skillID);        //生活技能是否已学

	LivingSkill*    addLivingSkill(U32 skillID) ;
	LivingSkill*    addLivingSkill(U32 skillID,U32 currRipe) ;
	LivingSkill*    addLivingSkill(LivingSkillData* pData,U32 currRipe=0);
	LivingSkill*    getLivingSkill(U32 skillID);    
	void            removeLivingSkill(U32 skillID);
	void            clear();

	inline LivingSkillTableMap& getLivingSkillTable(){ return mLivingSkillTable; }

#ifdef NTJ_CLIENT
	void            updateLiveingSkillView(U32 skillID);
	void            initLivingSkillPreview();
	bool            findViewByID(U8 iTab,U32 skillId);//查找同类技能
	Vector<U32>&    getLivingSkillView(U8 index);
#endif

#ifdef NTJ_SERVER
	bool            saveData(stPlayerStruct* playerInfo);
	bool            updateToClient(GameConnection* conn,U32 skillID=0,bool bAddSkill=true);
#endif
};
#endif