//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef __PLAYEROBJECT_H__
#define __PLAYEROBJECT_H__

#pragma once
#include "gui/core/guiTypes.h"
#include "core/tVector.h"
#include "Common/PacketType.h"
#include "Gameplay/GameObjects/GameObject.h"
#include "Gameplay/GameObjects/PlayerData.h"
#include "Gameplay/GameObjects/ShortcutObject.h"
#include "Gameplay/Item/ItemShortcut.h"
#include "Common/PlayerStruct.h"
#include "Common/TeamBase.h"
#include "Gameplay/GameObjects/StatsPlus.h"
#include "Gameplay/item/Player_Item.h"
#include "Gameplay/Item/ItemShortcut.h"
#include "Gameplay/GameObjects/PetTable.h"
#include "Gameplay/GameObjects/SpiritTable.h"
#include "Gameplay/Item/Player_ItemSplit.h"
#include "Gameplay/Item/Player_ItemCompose.h"
#include "Gameplay/GameObjects/Talent.h"

struct	stPlayerStruct;
class	GameConnection;
struct	NpcShopItem;
class	NpcObject;
class   LivingSkillTable;
class   Prescription;
class	CollectionObject;
class   ScheduleEvent;
class	TimerTriggerManager;
class	CommonFontEX;
class   MPManager;

class Player: public GameObject
{
public:
	typedef GameObject Parent;
	// ���������־
	const static U64 TeamMask		= Parent::NextFreeMask << 0;
	const static U64 SwitchMask		= Parent::NextFreeMask << 1;
	const static U64 PetMask		= Parent::NextFreeMask << 2;
	const static U64 MountPetMask	= Parent::NextFreeMask << 3;
	const static U64 SocialMask		= Parent::NextFreeMask << 4;
	const static U64 NextFreeMask	= Parent::NextFreeMask << 5;
	
	Player();
	~Player();

	DECLARE_CONOBJECT(Player);
    // ========================================================================
	// Begin:����˺Ϳͻ��˹��еı����ͷ�����Both Server and Client��
	// ========================================================================
public:
	// ͨ�ó�������
	enum enConstants
	{
		VIGOR_MAX				= 65000,			//��������ֵ
		IMAGE_NUMS				= 120,				//ÿ���࿨Ƭ���120
		ACTIVITY_MAX			= 50000000,			//����Ծ��
		MAX_MONEY_NUM			= 0x7FFFFFFF,		//��Ǯ����
		MAX_TRADEMONEY_NUM		= 0x2FAF080,		//�޶�һ����ཻ��50000000
		MAX_LOGIC_REPLYTIME		= 60000,			//�������ظ�ʱ��(60��)
	};

	// ����ѡ��(8bit)
	enum enRise
	{
		RISE_LOWERWORLD			= 0,				//�����
		RISE_ONTHESPOT,								//ԭ��
		RISE_SPELL,									//����
	};	

	// ����״̬
	enum enSwitchState
	{
		SWITCHSTATE_TEAM		= 1 << 0,			//��ӿ���
		SWITCHSTATE_TRADE		= 1 << 1,			//���׿���
		SWITCHSTATE_PK			= 1 << 2,			//PK����
	};

	// ����״̬
	enum enInteractionState
	{
		INTERACTION_NONE,							//��״̬
		INTERACTION_PLAYERTRADE,					//��ҽ���״̬
		INTERACTION_NPCTRADE,						//NPCϵͳ�̵꽻��
		INTERACTION_NPCTALK,						//NPC�Ի�״̬
		INTERACTION_STALLTRADE,						//��̯��������״̬
		INTERACTION_STALLBUY,						//��̯���߽���״̬
		INTERACTION_BANKTRADE,						//���н���״̬
		INTERACTION_PICKUP,							//ʰȡ��Ʒ״̬
		INTERACTION_MAIL,							//����ʹ��״̬
		INTERACTION_LEARNSKILL,						//ѧϰ����״̬
		INTERACTION_IDENTIFY,						//װ������
		INTERACTION_STRENGTHEN,						//װ��ǿ��
		INTERACTION_GEMMOUNT,						//װ����Ƕ
		INTERACTION_PUNCHHOLE,						//װ�����
		INTERACTION_REPAIR,							//װ������
		INTERACTION_SELECTSKILL,                    //����ϵѡ��״̬
        INTERACTION_STUDYLIVESKILL,                 //�����ѧϰ״̬
		INTERACTION_PETIDENTIFY,					//�������
		INTERACTION_PETINSIGHT,						//����������
		INTERACTION_PETLIANHUA,						//��������
		INTERACTION_PETHUANTONG,					//���ﻹͯ
		INTERACTION_SUPERMARKET,					//�̳�
		INTERACTION_SPIRITSKILL,					//Ԫ�����
	};

	// ������Ҹ��µ��¼������־
	enum enPlayerEventBits
	{
		EventMoneyMask			= BIT(0),			//���½�Ǯ
		EventBankMoneyMask		= BIT(1),			//�������д��
		EventBindMoneyMask		= BIT(2),			//���½�Ԫ
		EventExperienceMask		= BIT(3),			//���¾���
		EventTitleMask			= BIT(4),			//����ͷ��
		EventCreditMask			= BIT(5),			//��������
		EventInteractionMask	= BIT(6),			//���½�������
		EventBankLockMask		= BIT(7),			//��������������־
		EventTradeLockMask		= BIT(8),			//���µڶ������������ܱ�־
		EventUpgradeExp			= BIT(9),			//��������
		EventDoubleExpMask		= BIT(10),			//����˫������ֵ
		EventGoldMask			= BIT(11),			//����[���]
		EventOfflineExpTime		= BIT(13),			//�������߾���ʱ��
		EventSkillMask          = BIT(14),          //��������ר������
		EventOtherFlagMask		= BIT(15),			//�����������ܱ�־
		EventVigorMask          = BIT(16),          //���»���ֵ
	};
	
	// �������ù��ܱ�־
	enum enOtherFlag
	{
		OTHERFLAG_NEWBIE		= BIT(0),			//���ֽ�ɫ
		OTHERFLAG_USEFASHION	= BIT(1),			//ʹ��ʱװ��ʾ
		OTHERFLAG_FAMILYFASHION = BIT(2),			//����ʱװ��ʾ
	};

	// �߼��¼�����
	enum enLogicEventType
	{
		LOGICEVENT_TRADEINVITE		= 1,			//���뽻��
		LOGICEVENT_TEAMINVITE		= 2,			//�������
		LOGICEVENT_TEAMREQUEST		= 3,			//�������
		LOGICEVENT_TEAMFLLOWINVITE	= 4,			//������Ӹ���
		LOGICEVENT_TEAMFLLOWREQUEST = 5,			//������Ӹ���
		LOGICEVENT_FRIENDINVITE		= 6,			//����������
		LOGICEVENT_FRIENDREQUEST	= 7,			//����������
		LOGICEVENT_FACTIONINVITE	= 8,			//���������
		LOGICEVENT_FACTIONREQUEST	= 9,			//���������
		LOGICEVENT_MARRYINVITE		= 10,			//������
		LOGICEVENT_MATEINVITE		= 11,			//������
		LOGICEVENT_MATCHINVITE		= 12,			//�����д�
		LOGICEVENT_DOUBLERIDEINVITE	= 13,			//����˫����
		LOGICEVENT_DOUBLERIDEREQUEST= 14,			//����˫����
		LOGICEVENT_TRANSPORTINVITE	= 15,			//���봫��
		LOGICEVENT_COPYMAPINVITE	= 16,			//���븱��
		LOGICEVENT_COPYMAPREQUEST	= 17,			//���󸱱�
		LOGICEVENT_JOINGROUPINVITE	= 18,			//��������
		LOGICEVENT_INVITEMASTER		= 19,			//�����ʦ
		LOGICEVENT_INVITEAPPRENTICE	= 20,			//������ͽ
		LOGICEVENT_MARKETPLACEINVITE= 21,			//�����̳ǹ���
		LOGICEVENT_NOTIFYNEWMAIL	= 22,			//֪ͨ���ʼ�
	};

	// �������
	enum enPetOperate
	{
		PetOp_Spawn						= 0,		//�ٻ�����
		PetOp_Combo,								//�������
		PetOp_Dispatch,								//��ǲ����
		PetOp_Disband,								//��ɢ����
		PetOp_Release,								//��������
		PetOp_LianHua,								//��������
		PetOp_Insight,								//����������
		PetOp_Identify,								//����ɳ��ʼ���
		PetOp_Feed,									//����ιʳ
		PetOp_Learn,								//��������
		PetOp_UseEgg,								//ʹ�ó��ﵰ
		PetOp_BecomeBaby,							//���ﻹͯ
		PetOp_ChangeName,							//�������
		PetOp_ChangeProperties,						//�����޸����Ե�
		PetOp_UnLink,								//�Ͽ�����ָ��

		PetOp_AddExp,								//��߳��ﾭ��
		PetOp_AddLevel,								//��߳���ȼ�
	};

	// ������
	enum enMountPetOperate
	{
		PetOp_UseMountPetEgg			= 0,		//ʹ����走
		PetOp_SpawnMountPet,						//�ٻ����
		PetOp_DisbandMountPet,						//��ɢ���
		PetOp_ReleaseMountPet,						//�������
	};

	enum enSpiritOperate
	{
		SpiritOp_OpenSpiritSlot			= 0,		//����Ԫ����λ		
		SpiritOp_AddSpirit,							//����Ԫ��
		SpiritOp_RemoveSpirit,						//����Ԫ��
		SpiritOp_ActivateSpirit,					//����Ԫ��
		SpiritOp_HuanHuaSpirit,						//�û�Ԫ��
		SpiritOp_CancelHuanHuaSpirit,				//ȡ���û�Ԫ��
		SpiritOp_Initialize,						//��ʼ��
		SpiritOp_OpenJueZhaoWnd,					//��Ԫ����в�������
		SpiritOp_CloseJueZhaoWnd,					//�ر�Ԫ����в�������
		SpiritOp_OpenJueZhaoSlot,					//Ԫ����������
		SpiritOp_AddJueZhao,						//���Ԫ����м���
		SpiritOp_RemoveJueZhao,						//ɾ��Ԫ����м���
		SpiritOp_AddSpiritLevel,					//���Ԫ��ȼ�
		SpiritOp_AddSpiritExp,						//���Ԫ����
		SpiritOp_AddTalentLevel,					//���Ԫ���츳�ȼ�
	};


	struct stLogicEvent
	{
		U32		id;									//�߼��¼�ID
		U32		start;								//����ʱ��
		U32		type;								//�߼��¼�����
		U32		sender;								//������ID
		U32		relationid;							//�����߼��¼�ID
		StringTableEntry senderName;				//�����˽�ɫ����
		char*	data;								//�߼��¼�˽������
		U32		datalen;							//˽�����ݳ���		
	};
	//����ʱЧ���ߵļ�ʱ����
	struct stTimeItem
	{
		U64		uID;								//��ƷuID
		U32		uStartTime;							//��ʼʱ��
		U32		uLeaveTime;							//ʣ��ʱ��
	};
	// ���ҷ�ʽ
	enum enCurrencyType
	{
		Currentcy_Money			= 1,				//��Ǯ����Ԫ��
		Currentcy_BindMoney		= 2,				//�󶨽�Ǯ����Ԫ��
		Currentcy_Gold			= 3,				//��ʯ
		Currentcy_BindGold		= 4,				//��ʯ
		Currentcy_Credit		= 5,				//����
		Currentcy_BankMoney		= 6,				//���н�Ǯ
	};

	enum enTradeLockFlag
	{
		TradeLock_Password,							//�趨�˵ڶ�����
		TradeLock_Bank,								//��������
		TradeLock_Trade,							//��ҽ�������
		TradeLock_Stall,							//��̯����
	};

	stTeamInfo		mTeamInfo;						//��ɫ�����Ϣ
	bool			mIsSaveDirty;
protected:
	// ------------------------------------------------------------------------
	//	���һ������
	// ------------------------------------------------------------------------
	StatsPlusTable	mPlusTable;						//��ֵ��ǿ
	S32				mAccountId;						//�ʺ�ID
	S32				mPlayerId;						//��ɫID
	U32				mSwitchState;					//��ҿ��ƵĿ���״̬
	U32				mPlayerFunction;				//��ɫ���ܱ�־
	S32				mZoneID;						//���ߵ�ͼ���
	S32				mMoney;							//��ɫ��Ǯ
	S32				mBankMoney;						//���вֿ��Ǯ
	S32				mBindMoney;						//��ɫ�󶨽�Ǯ
	bool			mBankLock;						//
	U32				mTradeLockFlag;					//�ڶ������������ܱ�־
	U32				mVigor;							//��ǰ�����������ʹ��
	U32             mMaxVigor;                      //������ 
	U32             mCurrentLivingExp;              //��ǰ�����ֵ
	U32             mLivingLevel;                   //����ȼ�
	U32             mLivingSkillStudyID;            //����ר�������ID

	Vector<stLogicEvent*>	mLogicEventQueue;		//��Ϸ�߼��ӳٴ�����¼�����
	
	SimObjectPtr<SceneObject>	mInteractionObject;	//����Ŀ��
	U32							mInteractionState;	//����״̬

	int				mLastLoginTime;

	PetTable		mPetTable;						//����ϵͳ
	SpiritTable		mSpiritTable;					//Ԫ��ϵͳ
	stFame			mFame[MaxInfluence];			//����
	//---------------------------------------------------------------------------
	// �������ϵ����
	//---------------------------------------------------------------------------

	MPManager*      pMPManagerInfo;                 // ʦͽ

	int				mLastSaveTime;
	int				mLastLotRequestTime;			// ����Ҵ���70���Ժ�ÿ������ʱ���Զ�����������Ե���Ŷ�


	
public:
	U32				mOtherFlag;						//�������ܱ�־(����:�Ƿ���ʾʱװ)
	TalentTable		mTalentTable;					//�츳

	void			setSaveDirty()					{mIsSaveDirty = true;}
	bool			setCollectionTarget				(CollectionObject *pCollectionObj);
	bool			isSelfPlayer();

	U32				insertLogicEvent				(stLogicEvent* logicEvent);
	void			removeLogicEvent				(U32 id);
	stLogicEvent*	findLogicEvent					(U32 id);
	void			checkOverTimeLogicEvent			();
	U32				getLogicEventCount				() { return mLogicEventQueue.size();}
	stLogicEvent*	getLogicEvent					(S32 index);

	inline S32		getAccountID					();
	inline S32		getPlayerID						();
	inline StatsPlusTable& getPlusTable				();
	inline stTeamInfo&	getTeamInfo					();
	void			setPlayerName					(StringTableEntry name);
	inline StringTableEntry getPlayerName			();
	
	bool			addMoney						(S32 money, U32 type = Currentcy_Money);
	bool			reduceMoney						(S32 money, U32 type = Currentcy_Money);
	S32				getMoney						(U32 type = Currentcy_Money);
	bool			canReduceMoney					(S32 money, U32 type);
	bool			canTradeMoney					(Player* target, S32 givemoney, S32 gotmoney);
	void			setMoney						(U32 money) { mMoney = money; }

	inline S32		getZoneID						();
	inline void		setZoneID						(S32 zoneid);
	inline U32		getVigor						();
    void            addVigor                        (S32 vigor);  
	inline U32      getMaxVigor                     ();
	void		    setMaxVigor						(S32 vigor); 
	inline U32		getLivingCurrentLevelExp        ();
	inline U32      getLivingExp                    ();
	void            addLivingExp                    (S32 exp);
	inline U32      getLivingLevel                  ();
	void            addLivingLevel                  (S32 level);
	inline U32		getBody							();
	inline U32		getFace							();
	inline U32		getHair							();
	inline U32		getHairCol						();
	inline U32      getLivingSkillPro               ();
	void            setLivingSkillPro               (U32 skillId);
	U32             getLivingSkillRipe              (U32 skillId);
	void            setLivingSkillGuerdon           (U32 skillId,U32 extRipe = 0);
	inline void		setOtherFlag					(U32 flag);
	inline void		clearOtherFlag					(U32 flag);
	inline bool		IsOtherFlag						(U32 flag);
	U32				getOtherFlag					() { return mOtherFlag;}
	void			setOtherFlagValue				(U32 flag) { mOtherFlag = flag;}

	inline MPManager*     getMPInfo();


	enWarnMessage	isBusy							(enInteractionState state = INTERACTION_NONE);
	inline U32			getInteractionState			();
	inline SceneObject*	getInteraction				();
	bool			setInteraction					(SceneObject* obj, U32 state);
	bool			breakInteraction				();

	void			setSwitchState					(U32 flag, bool IsOpen);
	inline bool		getSwitchState					(U32 flag);
	inline bool		getPKState						();

	bool			onAdd							();
	void			onRemove						();
	bool			onNewDataBlock					(GameBaseData* dptr);
	void			setUpdateTeam					();
	void			processTick						(const Move *move);
	U64				packUpdate						(NetConnection *conn, U64 mask, BitStream *stream);
	void			unpackUpdate					(NetConnection *conn,           BitStream *stream);
	void			writePacketData					(GameConnection *conn,			BitStream *stream);
	void			onDisconnect					();
	void			inNpcScope						(NpcObject* npc);
	bool			onChangeDataBlock				(GameBaseData* dptr);
	void			onEnabled						();
	void			onDisabled						();
	static void     initPersistFields               ();
	void			setArmStatus					(GameObjectData::ArmStatus arm);

	virtual bool	CanAttackTarget					(GameObject* obj);
	virtual bool	isPKArea						();
	virtual void	addLevel						(S32 level);
	virtual void	addExp							(S32 exp);
	virtual void    onDamage                        (S32 dam,GameObject* pSource);
	virtual bool    setShapeShifting				(bool val);

	void			addFavour						(U32 infl, S32 favour);
	S32				getFavour						(U32 infl);
	void			setFame							(U32 infl, U32 fame, S32 favour = 0);
	S32				getFame							(U32 infl);

	// ------------------------------------------------------------------------
	//	����ϵͳ
	// ------------------------------------------------------------------------

	const inline PetTable&	getPetTable				() { return mPetTable;}
	S32						mountPet				(U32 slot);
	bool					comboPet				(U32 slot);
	bool					dispatchPet				(U32 slot);
	PetObject*				getPet					(U32 petIdx) { return mPetTable.getPet(petIdx);}
	void					setPet					(PetObject* pet) { mPetTable.setPet(pet);}

	// ------------------------------------------------------------------------
	//	Ԫ��ϵͳ
	// ------------------------------------------------------------------------
	inline SpiritTable&		getSpiritTable			() { return mSpiritTable;}

	// ------------------------------------------------------------------------
	//	װ���;ö���غ���
	// ------------------------------------------------------------------------
	U32						getCumulateDamage		() { return mCumulateDamage; }
	U32						getCumulateAttack		() { return mCumulateAttack; }
	void					setCumulateDamage		(U32 damage) { mCumulateDamage = damage; }
	void					setCumulateAttack		(U32 attack) { mCumulateAttack = attack; }
#ifdef NTJ_SERVER
	void					reduceWear				(U32 type , U32 damage); // type 1 Ϊ���� 2 Ϊ����
	void					reduceAllWear			();
#endif//NTJ_SERVER

	//-------------------------------------------------------------------------
	// ����ϵͳ
	//-------------------------------------------------------------------------
	
	bool			getBankFlag()	{ return mBankLock; } 
	void			setBankFlag(bool flag) { mBankLock = flag; }
	U32				getTradeFlag() { return mTradeLockFlag; }
	void			setTradeFlag(U32 flag) { mTradeLockFlag = flag; }
	void			setBankMoney(U32 money) { mBankMoney = money; }
	// ------------------------------------------------------------------------
	// ��ʼģ������
	// ------------------------------------------------------------------------
	U32				mBody;
	U32				mFace;
	U32				mHair;
	U32				mHairCol;

	// ------------------------------------------------------------------------
	//	�����������
	// ------------------------------------------------------------------------
	stMission		missionInfo;
	Vector<S32>		missionShareList;

	// ------------------------------------------------------------------------
	//	�����Ʒ����
	// ------------------------------------------------------------------------
	InventoryList	inventoryList;					//��Ʒ��
	PanelList		panelList;						//�����
	TradeList		tradeList;						//��ҽ�����
	EquipList		equipList;						//װ����
	DumpList		dumpList;						//�ع���
	BankList		bankList;						//���вֿ���
	NpcShopList		npcShopList;					//���NPC��Ʒ��
	PickupList		pickupList;						//���ʰȡ��Ʒ��
	EquipIdentifyList identifylist;					//��Ҽ�����
	EquipStrengthenList strengthenlist;				//װ��ǿ����
	MountGemList	mountGemList;					//��ʯ��Ƕ��
	PunchHoleList	punchHoleList;					//װ�������
	TempList		tempList;						//��ʱ��
	MissionSubmitList missionSubmitList;			//�����ύ��
	U32				mShowTriggerID;					//����ʾ������ID
	Prescription*   pPrescription;                  //�䷽
	LivingSkillTable*    pLivingSkill;              //�����
#ifndef NTJ_EDITOR
	ItemSplit       mItemSplitList;                 //��Ʒ�ֽ��� 
	ItemCompose     mItemComposeList;               //��Ʒ�ϳ���
	SkillList_Hint   skillList_Hint;                //��Ҽ�����ʾ��

#endif

	IndividualStallList individualStallList;		//���˰�̯��
	StallPetList		stallPetList;				//��̯������
	StallBuyItemList	stallBuyItemList;			//��̯�չ���
	PetList			petList;						//������
	MountPetList	mountPetList;					//�����
	SpiritList mSpiritList;							//Ԫ����
	RepairList mRepairList;							//������

	
	Vector<stTradeRecord*>	tradeRecord;			//���߽��׼�¼�б�
	

	TimerTriggerManager *mTimerTriggerMgr;			//��ʱ������������
	TimerTriggerManager *mTimerPetStudyMgr;			//�������м�ʱ������������

	//���ڿۼ��;ö�
	U32				mCumulateDamage;				// �˺��ۼ�ֵ
	U32				mCumulateAttack;				// ��������ۼ�ֵ
	// ========================================================================
	// End:����˺Ϳͻ��˹��еı����ͷ�����Both Server and Client��
	// ========================================================================

	// ========================================================================
	// Begin:����˽��еı����ͷ�����Only Server��
	// ========================================================================
#ifdef NTJ_SERVER
public:
	U32				mBeginPlayerTime;				//��ǰ��ʼʱ��
	U32				mOfflineTime;					//����ʱ��
	U32				mOnlineMinutes;					//�ۼƽ�ɫ���߷�����
	ScheduleEvent*  pScheduleEvent;
	static U16      mRecoverVigorVelocity;          //�����ָ��ٶ�
	bool            mIsRecoverVigor;                //�Ƿ�ɻָ�����            

	bool			mIsAdult;						// �Ƿ��ǳ���
	U32				dTotalOnlineTime;				// �������ۼ�����ʱ��

	int				mPointNum;

	int				mSocialCount;
	stSocialItem	mSocialItems[SOCAIL_ITEM_MAX];

	std::string		mCopyMapPosition;				// ��Ҫ����ĸ�������

	int				mCopymapInstId;

	Vector<stTimeItem*>	mTimeItemVec;				//ʱЧ�����б�
private:
	bool			mEventInitialized;				//�¼���ʼ�����
	T_UID			mUID;
	bool			mTransporting;					//���ͱ�־
	bool			mIsInitializeEvent;				//�Ƿ���ͻ��˸��¹���ʼ������
	StringTableEntry mSecondPassword;				//��������

	U8				mPlayerDataFlags[MAX_PLAYERDATAFLAGS];	//�߻���ֵ

public:
	int mTransLineId;
	int mTransTriggerId;
	int mTransMapId;
	F32 mTransX;
	F32 mTransY;
	F32 mTransZ;

public:
	inline T_UID	getUID()						{return mUID;}
	StringTableEntry getFlagsDate					(S32 index);
	bool			setFlagsDate					(S32 index, S32 year, S32 month, S32 day, S32 hour, S32 minute, S32 second);
	S32				getFlagsByte					(S32 index);
	bool			setFlagsByte					(S32 index, S32 ibyte);
	bool			getFlagsBit						(S32 index, S32 ibit);
	bool			setFlagsBit						(S32 index, S32 ibit, bool bFlag);
	S32				compareFlagsDate				(S32 index, S32 Y, S32 M, S32 D, S32 h, S32 m, S32 s);
	bool			saveFlagsDate					(S32 index);

	bool			isTransporting					()									{ return mTransporting;}
	void			setTransporting					(bool flag=true)					{ mTransporting = flag;}
	void			transportObject					(int LineId,int TriggerId,int MapId,F32 x,F32 y,F32 z);
	void			preTransportObject				(int LineId,int TriggerId,int MapId,F32 x,F32 y,F32 z);
	void			doTransportObject				(int ErrorCode,T_UID UID=0,int PlayerId=0,int LineId=0,int ZoneId=0,int IP=0,int Port=0);
	void			testTransportObject				();

	void			initializeData					(GameConnection *,T_UID,stPlayerStruct *);
	stPlayerStruct*	buildPlayerData					();
	void			sendToWorld						();
	void			saveToWorld						();

	bool			sendPlayerEvent					(U32 mask, bool isFirstUpdate = false);	// ��ͻ��˷��ͻ�����������(ֻ����ұ����ܿ���������)
	bool			sendInitializEvent				();//Player���ݳ�ʼ����ͻ��˸����¼�
	bool			hasInitializEvent				() {return mIsInitializeEvent;};
	void			sendInitializCompleteEvent		();

	void			initializeSendSkill				();		//��ͻ��˷��ͼ����б�
	void            initSendLivingSkill             ();     //��ͻ��˷���������б�
	void            initSendPrescription            ();     //��ͻ��˷����䷽�б�
	void			killNpc							(NpcObject* npc);
	StringTableEntry getSecondPassWord				() { return mSecondPassword; }//�õ���������
	
	void            recoverVigor                    ();
	stSocialItem*   getPlayerSocialItem             (U32 playerID);  // ���������Ŀ���������ϵ
	inline stSocialItem* getSocialItem              (){ return mSocialItems;}   
	inline S32      getSocialCount                  (){ return mSocialCount;}
	void			updateFashionFlags				();

	//��ӵ�ʵЧ�����б�
	void			SaveTimeItem					();
	void			LoadTimeItem					();
	void			updateTimeItem					();
	void			removeFromTimeItemVec			(U64 uId);
	void			pushToTimeItemVec				(stTimeItem* item);

protected:
	bool			initializeSkill					(stPlayerStruct* playerInfo);
	bool			initializeCooldown				(stPlayerStruct* playerInfo);
	bool            initLivingSkill                 (stPlayerStruct* playerInfo);
	bool            initPrescription                (stPlayerStruct* playerInfo);
	bool			initializeBuff					(stPlayerStruct* playerInfo);



#endif

	// ========================================================================
	// End:����˽��еı����ͷ�����Only Server��
	// ========================================================================

	// ========================================================================
	// Begin:�ͻ��˽��еı����ͷ�����Only Client��
	// ========================================================================
#ifdef NTJ_CLIENT
public:
	// ------------------------------------------------------------------------
	//	��Ҽ������� - �����ڿͻ���
	// ------------------------------------------------------------------------
	SkillList_Common skillList_Common;				//���ͨ�ü�����
	SkillList_Study  skillList_Study;               //��Ҽ���ѧϰ��
	AutoSellList	 autoSellList;					//��������
	MissionItemList	 missionItemList;				//��������Ʒ��
	StallLookUpList  lookUpList;					//�����嵥
	PetHelpList		 mPetHelpList;					//�����嵥�б�
	TradeAndStallHelpList mTradeStallHelpList;		//��̯��������
	SuperMarketList mSuperMarketList;				//�̳���
	RecommendItemList mRecommendList;				//�Ƽ���
	ShopBaksetList mShopBaksetList;					//������

	PrizeBox*		mPrizeBox;
	Vector< SimObjectPtr<CollectionObject> > mCollectionList;		//����֪�Ĳɼ��Ｏ��
	SimObjectPtr<CollectionObject>		mSelCollectionPtr;			// ��ǰѡ��Ĳɼ�������
	SimObjectPtr<PrizeBox>				mSelPrizeBoxPtr;			// ��ǰѡ��ı�������
	LivingSkillList* mLivingSkillList;             // �������
	PrescriptionList* mPrescriptionList;           // �䷽������

	RectF texRect;

	enum PickupType
	{
		Type_OpenDropList,
		Type_AutoPickup,
	};
	
	PickupType		m_enPickUpType;
	void			setPickupPath(PrizeBox *pPrizeBox, PickupType enType);
	void			setCollectionPath(CollectionObject * pCoillectionObj);
	void			addToCollectionList(CollectionObject *pCollectionObj);
	void			deleteFromCollectionList(CollectionObject *pCollectionObj);
	void			inNpcScope(GameObject* obj_npc);

protected:
	virtual void	onAddToClient					();
	virtual void	onRemoveFromClient				();
	
	static CommonFontEX*	m_pFont1;
	static CommonFontEX*	m_pFont2;
	bool			updateData						(Player* pPlayer,Vector<GFXTexHandle> *pTexList,GuiControlProfile** pStyleList,RectF *rect);
	void			drawObjectInfo					(GameObject *,void *,void *);
	void            drawObjectWord                  (void *);
#endif

#ifndef NTJ_EDITOR
	virtual void	addHate(GameObject* pObject, S32 nHate);
#endif

	// ========================================================================
	// End:�ͻ��˽��еı����ͷ�����Only Client��
	// ========================================================================

	// ========================================================================
	// Begin:�༭�����еı����ͷ�����Only Editor��
	// ========================================================================

	// ========================================================================
	// End:�༭�����еı����ͷ�����Only Editor��
	// ========================================================================

#ifdef NTJ_CLIENT
public:
	void SetStall();
	void SetStallPath(Point3F stallPos);
	Point3F mStallPosition;
#endif
};

#include "Gameplay/GameObjects/PlayerObject_inline.h"

#endif//__PLAYEROBJECT_H__