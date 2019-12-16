#pragma once
#include "sim/netConnection.h"
#include "common/PlayerStruct.h"
#include "Gameplay/GameplayCommon.h"
#include "common/MailBase.h"

class ClientGameplayParam;

//ClientGameNetEvents和ServerGameNetEvents基类
class GameplayEvent : public NetEvent
{
	typedef NetEvent Parent;
	friend class ClientGameplayParam;
private:
	enum TransferBits
	{
		InfoTypeBits		= 8,

		IntArgCountBits		= 6,
		IntArgValueBits		= 16,

		Int32ArgCountBits	= 6,
		Int32ArgValueBits	= 32,

		StringArgCountBits	= 3,
		StringArgValueBits	= 255,		//工会名称长度可能有50,留言可能有250字节	//32,
	};

	enum Constants
	{
		MAX_INT_ARGS		= 1 << IntArgCountBits,
		MAX_INT32_ARGS		= 1 << Int32ArgCountBits,
		MAX_STRING_ARGS		= 1 << StringArgCountBits,
	};
public:
	const static unsigned int MemoryBlockSize		= 256;

	GameplayEvent(U32 info_type = 0);
	~GameplayEvent() { };
	
	void				AddMemoryBlock					(char *block, int nBlockSize);
	void				SetIntArgValues					(S32 argc, ...);
	void				SetInt32ArgValues				(S32 argc, ...);
	void				SetStringArgValues				(S32 argc, ...);

	char*				GetMemoryBlock					()				{ return m_Block; }
	S32					GetMemoryBlockSize				()				{ return m_nBlockSize; }
	S32				    GetIntArgCount					(void)			{ return m_IntArgCount; }
	S32				    GetIntArg						(S32 index)		{ return m_IntArgValue[index]; }
	S32				    GetInt32ArgCount				(void)			{ return m_Int32ArgCount; }
	S32				    GetInt32Arg						(S32 index)		{ return m_Int32ArgValue[index]; }
	S32				    GetStringArgCount				(void)			{ return m_StringArgCount; }
	const char*			GetStringArg					(S32 index)		{ return m_StringArgValue[index]; }

	void write(NetConnection *pCon, BitStream *bstream)  {pack(pCon,bstream);}
	void pack(NetConnection *pCon, BitStream *bstream);
	void unpack(NetConnection *pCon, BitStream *bstream);

protected:
	U32			    m_InfoType;

	U32				m_IntArgCount;
	S32				m_IntArgValue[MAX_INT_ARGS];

	U32				m_Int32ArgCount;
	S32				m_Int32ArgValue[MAX_INT32_ARGS];

	U32				m_StringArgCount;
	char			m_StringArgValue[MAX_STRING_ARGS][StringArgValueBits];

	char			m_Block[MemoryBlockSize];
	S32				m_nBlockSize;
};

#define CHECK_MSGCOUNT(MsgID,intArgCount,int32ArgCount,stringArgCount)	\
	if(GetIntArgCount() < intArgCount 						\
	|| GetInt32ArgCount() < int32ArgCount				\
	|| GetStringArgCount() < stringArgCount )			\
{														\
	Con::errorf("错误的消息，参数错误! 消息ID: %d[file=%s;line=%d]", MsgID, __FILE__,__LINE__);\
	return;												\
}


//在这里编写游戏逻辑的网络消息如下：

//class GameLogicEvent : public NetEvent
//{
//private:
//	U32   mParam;												//需要逻辑处理的参数
//
//public:
//	GameLogicEvent(U32 Param = 0xffffffff)						//初始化网络事件对象及其参数
//	{ mParam = Param; }
//	void pack(NetConnection *, BitStream * bstream)				//打包事件信息准备通过网络发送，这里规定不可以涉及到逻辑判断
//	{ bstream->write(mParam); }
//	void unpack(NetConnection *, BitStream * bstream)			//接收网络消息并且解包，在这里不得有任何游戏逻辑判断来干预
//	{ bstream->read(&mParam); }
//	void process(NetConnection * con)							//对接受到的参数在这里进行逻辑处理
//	{  }
//
//	DECLARE_CONOBJECT(GameLogicEvent);							//必须对应填写这个宏使引擎可以了解类对象
//};

//使用时候用netConnection::postNetEvent(NetEvent *event);发送



//其他游戏消息的基类
class GameNetEvent : public NetEvent
{
	typedef NetEvent Parent;

public:
	void write(NetConnection *pCon, BitStream *bstream)	{}	//如果要支持Record需要在这里加操作
};


class GameDataNetEvent : public GameNetEvent		//这个类不允许改写
{
	typedef NetEvent Parent;

protected:	
	char *mBuffer;
	U32   mBufferSize;

public:
	GameDataNetEvent(){mBuffer = NULL; mBufferSize=0;};
	~GameDataNetEvent(){ if(mBuffer) delete []mBuffer;}
	void prepare(NetConnection *pCon);						
	void pack(NetConnection *pCon, BitStream *bstream);
	void unpack(NetConnection *pCon, BitStream *bstream);
	void process(NetConnection *pCon);

	//只能改写下面两个方法，一个用于发送，一个用于接收
	virtual void packData(NetConnection *pCon, BitStream *bstream) = 0;
	virtual void unpackData(NetConnection *pCon, BitStream *bstream) = 0;
};

//屏幕消息
class MessageEvent : public GameNetEvent
{
public:
	typedef GameNetEvent Parent;

	enum
	{
		MSG_STRING_MAXLEN		= 128,		
		MSG_BIT4				= 4,
		MSG_BIT16				= 16,
	};
private:
	U32		mMsgType;						// 消息类型
	U32		mMsgID;							// 固定的消息字符串ID（见BuildPrefix.h的enWarnMessage)
	char	mMsgStr[MSG_STRING_MAXLEN];		// 用户自定义的消息字符串
	U32		mShowPos;						// 显示位置
public:
	MessageEvent();
	void setMessage(U32 MsgType, U32 MsgID, U32 ShowPos = SHOWPOS_SCREEN);
	void setMessage(U32 MsgType, const char* MsgStr, U32 ShowPos = SHOWPOS_SCREEN);

#ifdef NTJ_SERVER
	static void send(NetConnection* conn, U32 MsgType, U32 MsgID, U32 ShowPos = SHOWPOS_SCREEN);
	static void send(NetConnection* conn, U32 MsgType, const char* MsgStr, U32 ShowPos = SHOWPOS_SCREEN);
#endif

#ifdef NTJ_CLIENT
	static void show(U32 MsgType, U32 MsgID, U32 ShowPos = SHOWPOS_SCREEN);
	static void show(U32 MsgType, const char* MsgStr, U32 ShowPos = SHOWPOS_SCREEN);
#endif

	void pack(NetConnection *pCon, BitStream *bstream);
	void unpack(NetConnection *pCon, BitStream *bstream);
	void process(NetConnection *pCon);

	DECLARE_CONOBJECT(MessageEvent);
};

//服务器通知客户端准备传送消息
class TransportBeginEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;

	bool   mIsSameMap;	//是否内传送
public:
	TransportBeginEvent()				{ }
	void setParam(bool isSameMap)	{ mIsSameMap = isSameMap;}

	void pack(NetConnection *pCon, BitStream *bstream);
	void unpack(NetConnection *pCon, BitStream *bstream);
	void process(NetConnection *pCon);

	DECLARE_CONOBJECT(TransportBeginEvent);
};

//服务器通知客户端传送开始消息
class TransportEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;

	bool mIsInternal;
	T_UID mUID;
	int mErrorCode,mPlayerId,mLineId,mZoneId;
	int mIP,mPort;

public:
	TransportEvent()				{ }
	void setParam(int ErrorCode,bool isInternal,T_UID UID=0,int PlayerId=0,int LineId=0,int ZoneId=0,int Ip=0,int Port=0)	
	{ 
		mIsInternal = isInternal;
		mErrorCode = ErrorCode;
		mUID = UID;
		mPlayerId = PlayerId;
		mLineId = LineId;
		mZoneId = ZoneId;

		mIP = Ip;
		mPort = Port;
	}

	void pack(NetConnection *pCon, BitStream *bstream);
	void unpack(NetConnection *pCon, BitStream *bstream);
	void process(NetConnection *pCon);

	DECLARE_CONOBJECT(TransportEvent);
};

//客户端换线请求
class SwitchLineEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;

	int   mLineId;		//线路号码
public:
	SwitchLineEvent()				{ }
	void setParam(int LintId)	{ mLineId = LintId;}

	void pack(NetConnection *pCon, BitStream *bstream);
	void unpack(NetConnection *pCon, BitStream *bstream);
	void process(NetConnection *pCon);

	DECLARE_CONOBJECT(SwitchLineEvent);
};

//----------------------------------------------------------------------------
//	发送玩家基本属性结构 - 只是作为更新属性使用
//----------------------------------------------------------------------------
class PlayerBaseProEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	PlayerBaseProEvent									(U32 mask = 0xFFFFFFFF, bool isFirstUpdate = false);

	DECLARE_CONOBJECT(PlayerBaseProEvent);

	enum
	{
		BITS_CHAR			= 8,
		BITS_SHORTINT		= 16,
		BITS_UNSIGNLONG		= 32,
		BITS_NAME			= 20,
	};

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);
private:
	U32		mMask;				// 更新标志位
	bool	mIsFirstUpdate;		// 是否初始化更新
};

//----------------------------------------------------------------------------
//快捷栏事件
//----------------------------------------------------------------------------
class PanelEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
	
	S32		mIndex;

public:
	PanelEvent	(){}
	PanelEvent	(S32 index);

	void	setParam(S32 index);
	void	packData(NetConnection* pCon, BitStream* bstream);
	void	unpackData(NetConnection* pCon, BitStream* bstream);

	DECLARE_CONOBJECT(PanelEvent);
};

//----------------------------------------------------------------------------
// 物品栏事件
//----------------------------------------------------------------------------
class InventoryEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	InventoryEvent();
	InventoryEvent (S32 index, S32 flag);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(InventoryEvent);
private:
	S32 mIndex;
	S32 mFlag;
};

//----------------------------------------------------------------------------
// 回购栏事件
//----------------------------------------------------------------------------
class ReBuyEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	ReBuyEvent();
	ReBuyEvent(S32 index, S32 flag);

	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(ReBuyEvent);
private:
	S32 mIndex;
	S32 mFlag;
};

//----------------------------------------------------------------------------
// 装备栏事件
//----------------------------------------------------------------------------
class EquipEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	EquipEvent();
	EquipEvent(S32 index, S32 flag);

	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(EquipEvent);
private:
	S32 mIndex;
	S32 mFlag;
};

//----------------------------------------------------------------------------
// 仓库栏事件
//----------------------------------------------------------------------------
class BankEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	BankEvent();
	BankEvent(S32 index, S32 flag);

	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(BankEvent);
private:
	S32 mIndex;
	S32 mFlag;
};

//----------------------------------------------------------------------------
// 交易栏事件
//----------------------------------------------------------------------------
class TradeEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	TradeEvent();
	TradeEvent(S32 index, S32 flag);

	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(TradeEvent);
private:
	S32 mIndex;
	S32 mFlag;
};

//----------------------------------------------------------------------------
// 对方交易栏事件
//----------------------------------------------------------------------------
class TradeTargetEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	TradeTargetEvent();
	TradeTargetEvent(U32 targetPlayerID, S32 index);

	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(TradeTargetEvent);
private:
	S32 mIndex;
	U32 mTargetPlayerID;
};

//----------------------------------------------------------------------------
// NPC系统商店事件
//----------------------------------------------------------------------------
class NpcShopEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	NpcShopEvent();
	NpcShopEvent (S32 index, S32 shopid, U32 shoptype, S32 page, S32 totalPages);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(NpcShopEvent);
private:
	S32 mIndex;
	S32 mShopID;
	U32 mShopType;
	S32 mCurrentPage;
	S32 mTotalPage;
	U32 mCurrentServerTime;
};

//----------------------------------------------------------------------------
// 怪物掉物事件
//----------------------------------------------------------------------------
class Player;
class GameObject;
class SceneObject;
class PrizeBox;
class CollectionObject;
class DropItemEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;

public:
	enum EventType
	{
		CLIENT_AUTO_PICKUP_ITEMS,
		CLIENT_PICKUP_PRIZE_ITEM,
		CLIENT_OPEN_DROPLIST,
		CLIENT_CLOSE_DROPLIST,		
		CLIENT_PICKUP_ITEM,
		CLIENT_PICKUP_ITEM_ALL,
		CLIENT_CLEAR_ITEM,

		SERVER_ERASE_PRIZE_ITEM,
		SERVER_DROPLIST,
		SERVER_OPEN_ERROR,
		SERVER_ERASE_ITEM,
		SERVER_ERASE_ITEM_IN_LIST,
		SERVER_CLEAR_ALL_ITEMS,

		BITS_MASK = 4,
		BITS_SHORT_INT = 16,
		BITS_INT_8 = 8
	};

	enum InteractionType
	{
		TYPE_PRIZE_BOX,				//怪物掉物
		TYPE_COLLECTION_OBJECT,		//采集类掉物
	};

	DropItemEvent() : m_Flag(0), m_InteractionType(TYPE_PRIZE_BOX), m_Index(0), m_Data(0) { }
	DropItemEvent(U32 flag) : m_Flag(flag) {}
	~DropItemEvent() { }

	void setInterationType(InteractionType type) { m_InteractionType = type; }
	bool setInterationType(SceneObject *pTargetObject);
	void setData(U32 nData, S32 Index = 0) { m_Data = nData; m_Index = Index; }
	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

#ifdef NTJ_SERVER
	static bool ServerSendDropList(Player *pPicker, S32 nCount);
	static bool ServerSendEraseDropItem(Player *pPicker, S32 nItemIndex);
	static bool ServerSendClearAllDropItems(Player *pPicker);
	static bool ServerSendEraseItemsInList(Player *pPicker, S32 count, U32 mask);
#endif

#ifdef NTJ_CLIENT
	static bool ClientSendAutoPickup(PrizeBox *pPrizeBox);
	static bool ClientSendPickupPrizeItem(PrizeBox *pPrizeBox);

	static bool ClientSendOpenDropItemList(SceneObject *pTargetObject);
	static bool ClientSendCloseDropItemList();
	static bool ClientSendPickupItem(S32 nItemIndex, bool bPickupAll);
	static bool ClientSendClearItem(S32 nItemIndex);
	static void ClientSetPickupAllItem(bool bAllItem);
#endif

	DECLARE_CONOBJECT(DropItemEvent);

private:
	U32 m_Flag;
	U8	m_InteractionType;
	U32 m_Data;
	S32 m_Index;
	static bool m_bPickupAllItem;
public:
	static Vector<U16> m_itemList;
};

// ========================================================================================================================================
//	任务系统事件
// ========================================================================================================================================

class MissionEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	enum
	{
		BIT_4			= 4,	// 发送4个bit的数据
		BIT_8			= 8,	// 发送5个bit的数据
		BIT_16			= 16,	// 发送16个bit的数据
		BIT_32			= 32,	// 发送32个bit的数据
		STATE_BLOCK		= 8,	// 任务完成状态分成8块发送
		CYCLE_BLOCK		= 4,	// 循环任务数据分成4块发送
		BIT_STATE_BLOCK = MISSION_STATE_LENGTH / STATE_BLOCK,
		BIT_CYCLE_BLOCK = MISSION_CYCLE_MAX * sizeof(stMissionCycle) /CYCLE_BLOCK,
	};

	MissionEvent										();

	DECLARE_CONOBJECT									(MissionEvent);
	void setParam										(U32 opt, int arg1 = -1, int arg2 = -1, bool notify = true);
	void packData										(NetConnection *_conn, BitStream * stream);
	void unpackData										(NetConnection *_conn, BitStream * stream);
private:
	U32			mOperate;	// 更新操作标志
	int			mArg1;		// 任务编号或者更新任务数据数组的Position
	int			mArg2;		// 更新任务数据旗标数组的Position
	bool		mNotify;	// 系统是否提示到客户端
};

//----------------------------------------------------------------------------
// 施放技能消息
//----------------------------------------------------------------------------
class CastSpellEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;
public:
	CastSpellEvent():mSkillId(0),mTarget(true),mTargetServerId(0),mItemType(0),mItemIdx(0),mItemUID(0) {mPos.zero();}
	CastSpellEvent(U32 SkillId, U32 serverId, U32 itemType = 0, U32 itemIdx = 0, U64 UID = 0);
	CastSpellEvent(U32 SkillId, Point3F& pos, U32 itemType = 0, U32 itemIdx = 0, U64 UID = 0);

	void pack(NetConnection *_conn, BitStream * stream);
	void unpack(NetConnection *_conn, BitStream * stream);
	void process(NetConnection *_conn);

	DECLARE_CONOBJECT(CastSpellEvent);
private:
	U32 mSkillId;			// 技能ID
	bool mTarget;			// 施放目标是否是目标对象
	U32 mTargetServerId;	// 目标对象的服务器ID
	Point3F mPos;			// 施放目标是制定地点
	U32 mItemType;			// 物品类型
	U32 mItemIdx;			// 物品号
	U64 mItemUID;			// 物品唯一ID
};

//----------------------------------------------------------------------------
// 战斗日志消息
//----------------------------------------------------------------------------
class CombatLogEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;
public:

	enum CombatInfo
	{
		CombatInfo_Spell = 0,		// <Player> 施放了 <Skill Name>
		CombatInfo_Buff,			// <Player> 获得了 <Buff Name>
		CombatInfo_Item,			// <Player> 使用了 <Item Name>

		CombatInfo_Damage,			// <Attacker> 对 <Player> 造成了 <number> 点伤害
									// <Attacker> 为 <Player> 恢复了 <number> 点生命
		CombatInfo_Miss,			// <Player> 没有击中 <target>
		CombatInfo_Dodge,			// <Player> 闪避了 <Attacker>的攻击
		CombatInfo_Immunity,		// <Player> 免疫了 <Attacker>的攻击
		CombatInfo_Absorb,			// <Player> 吸收了 <Attacker>的攻击
		CombatInfo_Kill,			// < Anyone > 杀死了 < Anyone >

		CombatInfo_Max,
	};

	enum
	{
		CombatMask_Immunity			= BIT(0),		// 免疫
		CombatMask_Absorb			= BIT(1),		// 吸收
		CombatMask_Critical			= BIT(2),		// 暴击
	};

	enum
	{
		CombatInfoBits = 4,
		GeneralIdBits = 32,
		ValBits = 20,
	};

	CombatLogEvent();

	void setParam(CombatInfo info, S32 src, S32 tgt, U32 id, S32 hp, S32 mp, S32 pp, U32 mask);
	void pack(NetConnection *_conn, BitStream * stream);
	void unpack(NetConnection *_conn, BitStream * stream);
	void process(NetConnection *_conn);
	static void show(U32 info, GameObject* src, GameObject* tgt, U32 id, S32 hp, S32 mp, S32 pp, U32 mask = 0);

	DECLARE_CONOBJECT(CombatLogEvent);
private:
	U8 mCombatInfo;			// 消息类型
	U8 mCombatMask;			// 消息标识
	S32 mSource;			// 施放源
	S32 mTarget;			// 施放目标
	U32 mGeneralId;			// 通用ID
	S32 mValHP;				// 值
	S32 mValMP;				// 值
	S32 mValPP;				// 值
};

//----------------------------------------------------------------------------
// 技能消息
//----------------------------------------------------------------------------
class SkillEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	SkillEvent():mSkillId(0),mAdd(true) {}
	SkillEvent(U32 skillId, bool bAdd):mSkillId(skillId),mAdd(bAdd) {}

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);
	//void process(NetConnection *_conn) {}

	DECLARE_CONOBJECT(SkillEvent);
private:
	U32 mSkillId;			// 技能ID
	bool mAdd;				// 是否新增技能
};

//----------------------------------------------------------------------------
// 宠物信息更新消息
//----------------------------------------------------------------------------
class PetEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;
public:
	enum
	{
		SlotBits = 4,
	};

	PetEvent(S32 slot = -1, U32 flag = 0xFFFFFFFF):mSlot(slot), mFlag(flag) {}

	void pack(NetConnection *_conn, BitStream * stream);
	void unpack(NetConnection *_conn, BitStream * stream);
	void process(NetConnection *_conn) {}

	DECLARE_CONOBJECT(PetEvent);
private:
	S32 mSlot;			// 宠物栏位
	U32 mFlag;			// 宠物数据标志
};

//----------------------------------------------------------------------------
// 骑宠信息更新消息
//----------------------------------------------------------------------------
class MountPetEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;
public:
	enum
	{
		SlotBits = 4,
	};

	MountPetEvent(S32 slot = -1):mSlot(slot) {}

	void pack(NetConnection *_conn, BitStream * stream);
	void unpack(NetConnection *_conn, BitStream * stream);
	void process(NetConnection *_conn) {}

	DECLARE_CONOBJECT(MountPetEvent);
private:
	S32 mSlot;			// 骑宠栏位
};

//----------------------------------------------------------------------------
// 元神信息更新消息
//----------------------------------------------------------------------------
class SpiritEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;
public:
	enum
	{
		SlotBits = 4,
	};

	SpiritEvent(S32 slot = -1, U32 flag = 0xFFFFFFFF) : mSlot(slot), mFlag(flag) {}

	void pack(NetConnection *_conn, BitStream * stream);
	void unpack(NetConnection *_conn, BitStream * stream);
	void process(NetConnection *_conn) {}

	DECLARE_CONOBJECT(SpiritEvent);
private:
	S32 mSlot;			// 元神栏位
	U32 mFlag;			// 元神数据标志
};


//----------------------------------------------------------------------------
// 装备鉴定事件
//----------------------------------------------------------------------------
class IdentifyEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	IdentifyEvent();
	IdentifyEvent(U32 index, U32 flag);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(IdentifyEvent);
private:
	U32 mIndex;
	U32	mFlag;
};

//----------------------------------------------------------------------------
// 装备强化事件
//----------------------------------------------------------------------------
class EquipStrengthenEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;

public:
	EquipStrengthenEvent();
	EquipStrengthenEvent(U32 index, U32 flag);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(EquipStrengthenEvent);
private:
	U32 mIndex;
	U32	mFlag;
};


//----------------------------------------------------------------------------
// 宝石镶嵌事件
//----------------------------------------------------------------------------
class MountGemEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;

public:
	MountGemEvent();
	MountGemEvent(U32 index, U32 flag);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(MountGemEvent);
private:
	U32 mIndex;
	U32	mFlag;
};

//----------------------------------------------------------------------------
// 装备打孔事件
//----------------------------------------------------------------------------
class PunchHoleEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;

public:
	PunchHoleEvent();
	PunchHoleEvent(U32 index, U32 flag);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(PunchHoleEvent);
private:
	U32 mIndex;
	U32	mFlag;
};

//----------------------------------------------------------------------------
// 任务奖励物品事件
//----------------------------------------------------------------------------
class MissionItemEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	MissionItemEvent();
	MissionItemEvent(U32 index, U32 flag);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(MissionItemEvent);
private:
	U32 mIndex;
	U32	mFlag;
};
//----------------------------------------------------------------------------
// 生活技能事件
//----------------------------------------------------------------------------
class LivingSkillEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
	U32 mSkillID;
	bool mflag;   // 增加或删除技能

public:
	LivingSkillEvent():mSkillID(0),mflag(true){}
	LivingSkillEvent(U32 skillID,bool flag):mSkillID(skillID),mflag(flag){}
	
	
	void packData(NetConnection *pCon, BitStream *stream);
	void unpackData(NetConnection *pCon, BitStream *stream);

	DECLARE_CONOBJECT(LivingSkillEvent);

};
//------------------------------------------------------------------------------
// 生活技能奖励事件
//------------------------------------------------------------------------------
class LivingSkillGuerdonEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
	U32 mSkillID;
	U32 mExtGuerdon;    //额外奖励
public:
	LivingSkillGuerdonEvent():mSkillID(0),mExtGuerdon(0){}
	LivingSkillGuerdonEvent(U32 skillId,U32 extGuerdon):mSkillID(skillId),mExtGuerdon(extGuerdon){}

	void packData(NetConnection *pCon, BitStream *stream);
	void unpackData(NetConnection *pCon, BitStream *stream);

	DECLARE_CONOBJECT(LivingSkillGuerdonEvent);

};

//------------------------------------------------------------------------------
// 社交事件
//------------------------------------------------------------------------------
class SocialEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;

	int eventType;
	int srcPlayerId;
	int reverseId;
public:
	SocialEvent() : eventType(0), srcPlayerId(0), reverseId(0) {;}

	void packData( NetConnection* pCon, BitStream* stream );
	void unpackData( NetConnection* pCon, BitStream* stream );

	DECLARE_CONOBJECT( SocialEvent );
};
//------------------------------------------------------------------------------
// 配方事件
//------------------------------------------------------------------------------
class PrescriptionEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	PrescriptionEvent() {}
	~PrescriptionEvent() { mIDList.clear();}

	void AddIDToList(U32 id);
	void packData(NetConnection *pCon, BitStream *stream);
	void unpackData(NetConnection *pCon, BitStream *stream);

	DECLARE_CONOBJECT(PrescriptionEvent);
private:
	Vector<U32> mIDList;
};

//------------------------------------------------------------------------------
// 摆摊栏事件
//------------------------------------------------------------------------------

class StallEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	StallEvent();
	StallEvent(U32 index);

	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(StallEvent);
private:
	U32 mIndex;

};

//------------------------------------------------------------------------------
// 摆摊宠物栏事件
//------------------------------------------------------------------------------
class StallPetEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	StallPetEvent();
	StallPetEvent(U32 index);

	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(StallPetEvent);

private:
	U32 mIndex;
};

//------------------------------------------------------------------------------
// 摆摊查找栏事件
//------------------------------------------------------------------------------
class StallLookUpEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	StallLookUpEvent();
	StallLookUpEvent(U32 index);

	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(StallLookUpEvent);
private:
	U32 mIndex;
};

//------------------------------------------------------------------------------
// 对方摆摊栏事件
//------------------------------------------------------------------------------

class StallTargetEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	StallTargetEvent();
	StallTargetEvent(U32 targetPlayerID, S32 index);

	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(StallTargetEvent);
private:
	S32 mIndex;
	U32 mTargetPlayerID;
};

//------------------------------------------------------------------------------
// 察看摆摊宠物栏事件
//------------------------------------------------------------------------------
class StallPetTargetEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	StallPetTargetEvent();
	StallPetTargetEvent(U32 targetPlayerID, S32 index);

	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(StallPetTargetEvent);

private:
	U32 mIndex;
	U32 mTargetPlayerID;
};

//------------------------------------------------------------------------------
// 察看摆摊查找栏事件
//------------------------------------------------------------------------------
class StallLookUpTargetEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	StallLookUpTargetEvent();
	StallLookUpTargetEvent(U32 targetPlayerID, S32 index);

	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(StallLookUpTargetEvent);

private:
	U32 mIndex;
	U32 mTargetPlayerID;
};

//----------------------------------------------------------------------------
// 临时栏事件
//----------------------------------------------------------------------------
class TempListEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;

public:
	TempListEvent();
	TempListEvent(U32 index, U32 flag);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(TempListEvent);
private:
	U32 mIndex;
	U32	mFlag;
};

//----------------------------------------------------------------------------
// 宠物栏事件
//----------------------------------------------------------------------------
class PetListEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;

public:
	PetListEvent();
	PetListEvent(U32 index, U32 flag);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(PetListEvent);
private:
	U32 mIndex;
	U32	mFlag;
};

//----------------------------------------------------------------------------
// 骑宠栏事件
//----------------------------------------------------------------------------
class MountPetListEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;

public:
	MountPetListEvent();
	MountPetListEvent(U32 index, U32 flag);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(MountPetListEvent);
private:
	U32 mIndex;
	U32	mFlag;
};

//----------------------------------------------------------------------------
// 元神栏事件
//----------------------------------------------------------------------------
class SpiritListEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;

public:
	SpiritListEvent();
	SpiritListEvent(U32 index, U32 flag);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(SpiritListEvent);
private:
	U32 mIndex;
	U32	mFlag;
};

//----------------------------------------------------------------------------
// 师徒事件
//----------------------------------------------------------------------------
class MPManagerEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	MPManagerEvent(U32 mask = 0xFFFF, bool isFirstUpdate = false):mMask(mask),mIsFirstUpdate(isFirstUpdate){}

	void packData(NetConnection *pCon, BitStream *stream);
	void unpackData(NetConnection *pCon, BitStream *stream);

	DECLARE_CONOBJECT(MPManagerEvent);
private:
	U16		mMask;				// 更新标志位
	bool	mIsFirstUpdate;		// 是否初始化更新
};

//摆摊交易聊天记录
class StallRecordEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	StallRecordEvent();
	StallRecordEvent(U32 mPlayerID, bool flag, U32 index);

	void packData(NetConnection* pCon, BitStream* bStream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(StallRecordEvent);
private:
	U32 mIndex;
	U32 mPlayerID;
	bool mFlag; // 0-交易信息 1留言信息
};

//-----------------------------------------------------------------
// 邮件事件
//-----------------------------------------------------------------
class MailEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;
public:
	MailEvent();
	MailEvent( int index, int pageCount, int playerId );

	void pack(NetConnection* pCon, BitStream* pStream );
	void unpack(NetConnection* pCon, BitStream* pStream );
	void process(NetConnection *pCon);

#ifdef NTJ_SERVER
	void setMailHeader( stMailHeader& mailHeader );
	void addMailItem( stMailItem& item );
#endif

	DECLARE_CONOBJECT(MailEvent);
private:
	int mIndex;
	int mPageCount;
	int mPlayerId;

	stMailHeader mHeader;
	std::list< stMailItem > mMailItemList;
};

//----------------------------------------------------------------------------
// 技能提示栏事件
//----------------------------------------------------------------------------
class SkillHintEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	SkillHintEvent();
	SkillHintEvent (S32 index, S32 flag);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(SkillHintEvent);
private:
	S32 mIndex;
	S32 mFlag;
};

//------------------------------------------------------------------
// 本地图公共消息事件
//------------------------------------------------------------------
class ZoneChatEvent : public GameNetEvent
{
public:
	typedef GameNetEvent Parent;

	DECLARE_CONOBJECT(ZoneChatEvent);
	ZoneChatEvent();
	ZoneChatEvent( int channelId, const char* name, const char* msg );

	void pack(NetConnection* pCon, BitStream* pStream );
	void unpack(NetConnection* pCon, BitStream* pStream );
	void process(NetConnection *pCon);

private:
	static const int MAX_MSG_LENGTH = 512;

	char name[COMMON_STRING_LENGTH];
	char msg[MAX_MSG_LENGTH];
	int mChannelId;
};

//----------------------------------------------------------------------------
// 装备修理事件
//----------------------------------------------------------------------------
class RepairEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	RepairEvent();
	RepairEvent(U32 index);

	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(RepairEvent);
private:
	U32 mIndex;
};

//----------------------------------------------------------------------------
// 物品合成事件
//----------------------------------------------------------------------------
class ItemSplitEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	ItemSplitEvent():mIndex(0){}
	ItemSplitEvent(U8 iIndex):mIndex(iIndex){}

	void packData(NetConnection *pCon, BitStream *stream);
	void unpackData(NetConnection *pCon, BitStream *stream);

	DECLARE_CONOBJECT(ItemSplitEvent);
private:
	U8 mIndex;
};
//----------------------------------------------------------------------------
// 物品拆分事件
//----------------------------------------------------------------------------
class ItemComposeEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	ItemComposeEvent():mIndex(0){}
	ItemComposeEvent(U8 iIndex):mIndex(iIndex){}

	void packData(NetConnection *pCon, BitStream *stream);
	void unpackData(NetConnection *pCon, BitStream *stream);

	DECLARE_CONOBJECT(ItemComposeEvent);
private:
	U8 mIndex;
};

//----------------------------------------------------------------------------
// 更新玩家摆摊收购栏事件
//----------------------------------------------------------------------------
class StallerMoneyEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	StallerMoneyEvent();
	StallerMoneyEvent(U32 playerID, U32 money);

	void packData(NetConnection *pCon, BitStream *bstream);
	void unpackData(NetConnection *pCon, BitStream *bstream);

	DECLARE_CONOBJECT(StallerMoneyEvent);
private:
	U32 mMoney;
	U32 mStallerId;
};

//----------------------------------------------------------------------------
// 更新商城购物栏事件到服务端
//----------------------------------------------------------------------------
struct stBasketItem 
{
	U32 goodID;
	U32 num;
};
class BasketItemEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;
public:
	BasketItemEvent();
	~BasketItemEvent();
	BasketItemEvent( U32 type);

	void pack(NetConnection *ps, BitStream *bstream);
	void unpack(NetConnection *ps, BitStream *bstream);
	void addBasketItem(U32 id, U32 num);
	void process(NetConnection *ps);

	DECLARE_CONOBJECT(BasketItemEvent);

private:
	U32 mBasketSize;
	U32 mType;
	Vector<stBasketItem*> mBasketVector;
};