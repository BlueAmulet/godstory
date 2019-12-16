#pragma once
#include "sim/netConnection.h"
#include "common/PlayerStruct.h"
#include "Gameplay/GameplayCommon.h"
#include "common/MailBase.h"

class ClientGameplayParam;

//ClientGameNetEvents��ServerGameNetEvents����
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
		StringArgValueBits	= 255,		//�������Ƴ��ȿ�����50,���Կ�����250�ֽ�	//32,
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
	Con::errorf("�������Ϣ����������! ��ϢID: %d[file=%s;line=%d]", MsgID, __FILE__,__LINE__);\
	return;												\
}


//�������д��Ϸ�߼���������Ϣ���£�

//class GameLogicEvent : public NetEvent
//{
//private:
//	U32   mParam;												//��Ҫ�߼�����Ĳ���
//
//public:
//	GameLogicEvent(U32 Param = 0xffffffff)						//��ʼ�������¼����������
//	{ mParam = Param; }
//	void pack(NetConnection *, BitStream * bstream)				//����¼���Ϣ׼��ͨ�����緢�ͣ�����涨�������漰���߼��ж�
//	{ bstream->write(mParam); }
//	void unpack(NetConnection *, BitStream * bstream)			//����������Ϣ���ҽ���������ﲻ�����κ���Ϸ�߼��ж�����Ԥ
//	{ bstream->read(&mParam); }
//	void process(NetConnection * con)							//�Խ��ܵ��Ĳ�������������߼�����
//	{  }
//
//	DECLARE_CONOBJECT(GameLogicEvent);							//�����Ӧ��д�����ʹ��������˽������
//};

//ʹ��ʱ����netConnection::postNetEvent(NetEvent *event);����



//������Ϸ��Ϣ�Ļ���
class GameNetEvent : public NetEvent
{
	typedef NetEvent Parent;

public:
	void write(NetConnection *pCon, BitStream *bstream)	{}	//���Ҫ֧��Record��Ҫ������Ӳ���
};


class GameDataNetEvent : public GameNetEvent		//����಻�����д
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

	//ֻ�ܸ�д��������������һ�����ڷ��ͣ�һ�����ڽ���
	virtual void packData(NetConnection *pCon, BitStream *bstream) = 0;
	virtual void unpackData(NetConnection *pCon, BitStream *bstream) = 0;
};

//��Ļ��Ϣ
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
	U32		mMsgType;						// ��Ϣ����
	U32		mMsgID;							// �̶�����Ϣ�ַ���ID����BuildPrefix.h��enWarnMessage)
	char	mMsgStr[MSG_STRING_MAXLEN];		// �û��Զ������Ϣ�ַ���
	U32		mShowPos;						// ��ʾλ��
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

//������֪ͨ�ͻ���׼��������Ϣ
class TransportBeginEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;

	bool   mIsSameMap;	//�Ƿ��ڴ���
public:
	TransportBeginEvent()				{ }
	void setParam(bool isSameMap)	{ mIsSameMap = isSameMap;}

	void pack(NetConnection *pCon, BitStream *bstream);
	void unpack(NetConnection *pCon, BitStream *bstream);
	void process(NetConnection *pCon);

	DECLARE_CONOBJECT(TransportBeginEvent);
};

//������֪ͨ�ͻ��˴��Ϳ�ʼ��Ϣ
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

//�ͻ��˻�������
class SwitchLineEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;

	int   mLineId;		//��·����
public:
	SwitchLineEvent()				{ }
	void setParam(int LintId)	{ mLineId = LintId;}

	void pack(NetConnection *pCon, BitStream *bstream);
	void unpack(NetConnection *pCon, BitStream *bstream);
	void process(NetConnection *pCon);

	DECLARE_CONOBJECT(SwitchLineEvent);
};

//----------------------------------------------------------------------------
//	������һ������Խṹ - ֻ����Ϊ��������ʹ��
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
	U32		mMask;				// ���±�־λ
	bool	mIsFirstUpdate;		// �Ƿ��ʼ������
};

//----------------------------------------------------------------------------
//������¼�
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
// ��Ʒ���¼�
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
// �ع����¼�
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
// װ�����¼�
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
// �ֿ����¼�
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
// �������¼�
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
// �Է��������¼�
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
// NPCϵͳ�̵��¼�
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
// ��������¼�
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
		TYPE_PRIZE_BOX,				//�������
		TYPE_COLLECTION_OBJECT,		//�ɼ������
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
//	����ϵͳ�¼�
// ========================================================================================================================================

class MissionEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	enum
	{
		BIT_4			= 4,	// ����4��bit������
		BIT_8			= 8,	// ����5��bit������
		BIT_16			= 16,	// ����16��bit������
		BIT_32			= 32,	// ����32��bit������
		STATE_BLOCK		= 8,	// �������״̬�ֳ�8�鷢��
		CYCLE_BLOCK		= 4,	// ѭ���������ݷֳ�4�鷢��
		BIT_STATE_BLOCK = MISSION_STATE_LENGTH / STATE_BLOCK,
		BIT_CYCLE_BLOCK = MISSION_CYCLE_MAX * sizeof(stMissionCycle) /CYCLE_BLOCK,
	};

	MissionEvent										();

	DECLARE_CONOBJECT									(MissionEvent);
	void setParam										(U32 opt, int arg1 = -1, int arg2 = -1, bool notify = true);
	void packData										(NetConnection *_conn, BitStream * stream);
	void unpackData										(NetConnection *_conn, BitStream * stream);
private:
	U32			mOperate;	// ���²�����־
	int			mArg1;		// �����Ż��߸����������������Position
	int			mArg2;		// ��������������������Position
	bool		mNotify;	// ϵͳ�Ƿ���ʾ���ͻ���
};

//----------------------------------------------------------------------------
// ʩ�ż�����Ϣ
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
	U32 mSkillId;			// ����ID
	bool mTarget;			// ʩ��Ŀ���Ƿ���Ŀ�����
	U32 mTargetServerId;	// Ŀ�����ķ�����ID
	Point3F mPos;			// ʩ��Ŀ�����ƶ��ص�
	U32 mItemType;			// ��Ʒ����
	U32 mItemIdx;			// ��Ʒ��
	U64 mItemUID;			// ��ƷΨһID
};

//----------------------------------------------------------------------------
// ս����־��Ϣ
//----------------------------------------------------------------------------
class CombatLogEvent : public GameNetEvent
{
	typedef GameNetEvent Parent;
public:

	enum CombatInfo
	{
		CombatInfo_Spell = 0,		// <Player> ʩ���� <Skill Name>
		CombatInfo_Buff,			// <Player> ����� <Buff Name>
		CombatInfo_Item,			// <Player> ʹ���� <Item Name>

		CombatInfo_Damage,			// <Attacker> �� <Player> ����� <number> ���˺�
									// <Attacker> Ϊ <Player> �ָ��� <number> ������
		CombatInfo_Miss,			// <Player> û�л��� <target>
		CombatInfo_Dodge,			// <Player> ������ <Attacker>�Ĺ���
		CombatInfo_Immunity,		// <Player> ������ <Attacker>�Ĺ���
		CombatInfo_Absorb,			// <Player> ������ <Attacker>�Ĺ���
		CombatInfo_Kill,			// < Anyone > ɱ���� < Anyone >

		CombatInfo_Max,
	};

	enum
	{
		CombatMask_Immunity			= BIT(0),		// ����
		CombatMask_Absorb			= BIT(1),		// ����
		CombatMask_Critical			= BIT(2),		// ����
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
	U8 mCombatInfo;			// ��Ϣ����
	U8 mCombatMask;			// ��Ϣ��ʶ
	S32 mSource;			// ʩ��Դ
	S32 mTarget;			// ʩ��Ŀ��
	U32 mGeneralId;			// ͨ��ID
	S32 mValHP;				// ֵ
	S32 mValMP;				// ֵ
	S32 mValPP;				// ֵ
};

//----------------------------------------------------------------------------
// ������Ϣ
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
	U32 mSkillId;			// ����ID
	bool mAdd;				// �Ƿ���������
};

//----------------------------------------------------------------------------
// ������Ϣ������Ϣ
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
	S32 mSlot;			// ������λ
	U32 mFlag;			// �������ݱ�־
};

//----------------------------------------------------------------------------
// �����Ϣ������Ϣ
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
	S32 mSlot;			// �����λ
};

//----------------------------------------------------------------------------
// Ԫ����Ϣ������Ϣ
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
	S32 mSlot;			// Ԫ����λ
	U32 mFlag;			// Ԫ�����ݱ�־
};


//----------------------------------------------------------------------------
// װ�������¼�
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
// װ��ǿ���¼�
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
// ��ʯ��Ƕ�¼�
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
// װ������¼�
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
// ��������Ʒ�¼�
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
// ������¼�
//----------------------------------------------------------------------------
class LivingSkillEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
	U32 mSkillID;
	bool mflag;   // ���ӻ�ɾ������

public:
	LivingSkillEvent():mSkillID(0),mflag(true){}
	LivingSkillEvent(U32 skillID,bool flag):mSkillID(skillID),mflag(flag){}
	
	
	void packData(NetConnection *pCon, BitStream *stream);
	void unpackData(NetConnection *pCon, BitStream *stream);

	DECLARE_CONOBJECT(LivingSkillEvent);

};
//------------------------------------------------------------------------------
// ����ܽ����¼�
//------------------------------------------------------------------------------
class LivingSkillGuerdonEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
	U32 mSkillID;
	U32 mExtGuerdon;    //���⽱��
public:
	LivingSkillGuerdonEvent():mSkillID(0),mExtGuerdon(0){}
	LivingSkillGuerdonEvent(U32 skillId,U32 extGuerdon):mSkillID(skillId),mExtGuerdon(extGuerdon){}

	void packData(NetConnection *pCon, BitStream *stream);
	void unpackData(NetConnection *pCon, BitStream *stream);

	DECLARE_CONOBJECT(LivingSkillGuerdonEvent);

};

//------------------------------------------------------------------------------
// �罻�¼�
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
// �䷽�¼�
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
// ��̯���¼�
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
// ��̯�������¼�
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
// ��̯�������¼�
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
// �Է���̯���¼�
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
// �쿴��̯�������¼�
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
// �쿴��̯�������¼�
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
// ��ʱ���¼�
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
// �������¼�
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
// ������¼�
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
// Ԫ�����¼�
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
// ʦͽ�¼�
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
	U16		mMask;				// ���±�־λ
	bool	mIsFirstUpdate;		// �Ƿ��ʼ������
};

//��̯���������¼
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
	bool mFlag; // 0-������Ϣ 1������Ϣ
};

//-----------------------------------------------------------------
// �ʼ��¼�
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
// ������ʾ���¼�
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
// ����ͼ������Ϣ�¼�
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
// װ�������¼�
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
// ��Ʒ�ϳ��¼�
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
// ��Ʒ����¼�
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
// ������Ұ�̯�չ����¼�
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
// �����̳ǹ������¼��������
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