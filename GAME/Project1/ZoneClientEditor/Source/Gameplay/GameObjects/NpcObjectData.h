//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

#include "Gameplay/GameObjects/GameObjectData.h"
#include "Gameplay/GameplayCommon.h"

class IColumnData;
// ========================================================================================================================================
//	NpcObjectData
//	NpcObjectData 保存了Npc的模板信息
// ========================================================================================================================================
struct NpcObjectData: public GameObjectData
{
	typedef GameObjectData Parent;

	enum Flags
	{
		Flags_Combative			= BIT(0),	// 是否参与战斗
		Flags_Tame				= BIT(1),	// 是否捕捉
		Flags_Active			= BIT(2),	// 主动攻击的
		Flags_ConstBuffId		= BIT(3),	// 固定buffId（不随等级变化）
		Flags_ConstScale		= BIT(4),	// 不随机缩放
	};

	enum enIntensity
	{
		Intensity_Normal		= 0,		// 普通
		Intensity_Captain,					// 头目
		Intensity_Elite,					// 精英
		Intensity_RareElite,				// 稀有精英
		Intensity_Boss,						// BOSS
	};

	StringTableEntry	npcName;			// 名称
	StringTableEntry	npcTitle;			// 称号
	StringTableEntry	icon;				// icon文件名
	StringTableEntry	topIcon;			// 头顶图片文件名
	U32					flags;				// Npc属性标志
	enRace				race;				// 种族
	enFamily			family;				// 门宗
	enInfluence			influence;			// 势力(npc势力一定不是Influence_None)
	enIntensity			intensity;			// 强度
	U32					minLevel;			// 最小等级
	U32					maxLevel;			// 最大等级
	U32					buffId;				// 基础数值
	U32					defaultSkillId;		// 普通攻击技能Id
	U32					experience;			// 经验
	U32					teamMatchId;		// 队伍匹配

	U32					exBuffId_a;			// 附加状态1
	U32					exBuffId_b;			// 附加状态2

	F32					minScale;			// 最小缩放范围
	F32					maxScale;			// 最大缩放范围
	F32					visualField;		// 视野范围
	F32					activeField;		// 活动范围
	F32					chaseField;			// 追击范围
	F32					combatField;		// 战斗范围
	F32					expField;			// 怪物死亡后玩家的受益范围
	SimTime				scanInterval;		// 索敌间隔

	SimTime				riseInterval;		// 重生间隔
	SimTime				corpseTime;			// 尸体可见时间
	SimTime				fadeTime;			// 淡入淡出时间
	SimTime				dropTime;			// 掉物包存在时间

	U32					eventID;			// 事件ID
	U32					pathID;				// 路径ID

	U32					packageShapeId;		// 包裹模型ID

	StringTableEntry	attackSound_a;		// 攻击音效
	StringTableEntry	attackSound_b;		// 攻击音效
	U32					attackSoundOdds;	// 几率

	StringTableEntry	interactionSound_a;	// 交互音效
	StringTableEntry	interactionSound_b;	// 交互音效
	StringTableEntry	deadthSound;		// 死亡音效

	//
	DECLARE_CONOBJECT(NpcObjectData);
	NpcObjectData();
	~NpcObjectData();
	bool preload(bool server, char errorBuffer[256]);
	bool initDataBlock();
	inline bool isFlags(U32 val) { return (flags & val);}
};


// ========================================================================================================================================
//	NpcRepository
//	NpcRepository保存了所有NpcObjectData的信息
// ========================================================================================================================================
class NpcRepository
{
public:
	NpcRepository();
	~NpcRepository();
	typedef stdext::hash_map<U32, NpcObjectData*> NpcDataMap;
	NpcObjectData*			GetNpcData(U32 NpcID);
	U32						GetNpcIdByIndex(int index);
	S32						GetNpcDataCount();

	bool					Insert(NpcObjectData* pData);
	bool					Read();
	void					Clear();

	IColumnData*			mColumnData;
private:
	NpcDataMap				m_NpcDataMap;
};

extern NpcRepository g_NpcRepository;
