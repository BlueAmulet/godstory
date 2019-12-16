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
	// 更新掩码标志
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
	// Begin:服务端和客户端共有的变量和方法（Both Server and Client）
	// ========================================================================
public:
	// 通用常量定义
	enum enConstants
	{
		VIGOR_MAX				= 65000,			//最大的体力值
		IMAGE_NUMS				= 120,				//每种类卡片最多120
		ACTIVITY_MAX			= 50000000,			//最大活跃度
		MAX_MONEY_NUM			= 0x7FFFFFFF,		//金钱上限
		MAX_TRADEMONEY_NUM		= 0x2FAF080,		//限定一次最多交易50000000
		MAX_LOGIC_REPLYTIME		= 60000,			//最大邀请回复时间(60秒)
	};

	// 复活选项(8bit)
	enum enRise
	{
		RISE_LOWERWORLD			= 0,				//复活点
		RISE_ONTHESPOT,								//原地
		RISE_SPELL,									//法术
	};	

	// 开关状态
	enum enSwitchState
	{
		SWITCHSTATE_TEAM		= 1 << 0,			//组队开关
		SWITCHSTATE_TRADE		= 1 << 1,			//交易开关
		SWITCHSTATE_PK			= 1 << 2,			//PK开关
	};

	// 交互状态
	enum enInteractionState
	{
		INTERACTION_NONE,							//无状态
		INTERACTION_PLAYERTRADE,					//玩家交易状态
		INTERACTION_NPCTRADE,						//NPC系统商店交易
		INTERACTION_NPCTALK,						//NPC对话状态
		INTERACTION_STALLTRADE,						//摆摊卖主交易状态
		INTERACTION_STALLBUY,						//摆摊买者交易状态
		INTERACTION_BANKTRADE,						//银行交易状态
		INTERACTION_PICKUP,							//拾取物品状态
		INTERACTION_MAIL,							//邮箱使用状态
		INTERACTION_LEARNSKILL,						//学习技能状态
		INTERACTION_IDENTIFY,						//装备鉴定
		INTERACTION_STRENGTHEN,						//装备强化
		INTERACTION_GEMMOUNT,						//装备镶嵌
		INTERACTION_PUNCHHOLE,						//装备打孔
		INTERACTION_REPAIR,							//装备修理
		INTERACTION_SELECTSKILL,                    //技能系选择状态
        INTERACTION_STUDYLIVESKILL,                 //生活技能学习状态
		INTERACTION_PETIDENTIFY,					//宠物鉴定
		INTERACTION_PETINSIGHT,						//宠物提高灵慧
		INTERACTION_PETLIANHUA,						//宠物炼化
		INTERACTION_PETHUANTONG,					//宠物还童
		INTERACTION_SUPERMARKET,					//商城
		INTERACTION_SPIRITSKILL,					//元神绝招
	};

	// 玩家自我更新的事件掩码标志
	enum enPlayerEventBits
	{
		EventMoneyMask			= BIT(0),			//更新金钱
		EventBankMoneyMask		= BIT(1),			//更新银行存款
		EventBindMoneyMask		= BIT(2),			//更新金元
		EventExperienceMask		= BIT(3),			//更新经验
		EventTitleMask			= BIT(4),			//更新头衔
		EventCreditMask			= BIT(5),			//更新声望
		EventInteractionMask	= BIT(6),			//更新交互对象
		EventBankLockMask		= BIT(7),			//更新银行锁定标志
		EventTradeLockMask		= BIT(8),			//更新第二密码锁定功能标志
		EventUpgradeExp			= BIT(9),			//升级经验
		EventDoubleExpMask		= BIT(10),			//更新双倍经验值
		EventGoldMask			= BIT(11),			//更新[金币]
		EventOfflineExpTime		= BIT(13),			//更新离线经验时间
		EventSkillMask          = BIT(14),          //更新修练专精技能
		EventOtherFlagMask		= BIT(15),			//更新其它功能标志
		EventVigorMask          = BIT(16),          //更新活力值
	};
	
	// 其它启用功能标志
	enum enOtherFlag
	{
		OTHERFLAG_NEWBIE		= BIT(0),			//新手角色
		OTHERFLAG_USEFASHION	= BIT(1),			//使用时装显示
		OTHERFLAG_FAMILYFASHION = BIT(2),			//门宗时装显示
	};

	// 逻辑事件类型
	enum enLogicEventType
	{
		LOGICEVENT_TRADEINVITE		= 1,			//邀请交易
		LOGICEVENT_TEAMINVITE		= 2,			//邀请组队
		LOGICEVENT_TEAMREQUEST		= 3,			//请求组队
		LOGICEVENT_TEAMFLLOWINVITE	= 4,			//邀请组队跟随
		LOGICEVENT_TEAMFLLOWREQUEST = 5,			//请求组队跟随
		LOGICEVENT_FRIENDINVITE		= 6,			//邀请加入好友
		LOGICEVENT_FRIENDREQUEST	= 7,			//请求加入好友
		LOGICEVENT_FACTIONINVITE	= 8,			//邀请加入帮会
		LOGICEVENT_FACTIONREQUEST	= 9,			//请求加入帮会
		LOGICEVENT_MARRYINVITE		= 10,			//邀请结婚
		LOGICEVENT_MATEINVITE		= 11,			//邀请结拜
		LOGICEVENT_MATCHINVITE		= 12,			//邀请切磋
		LOGICEVENT_DOUBLERIDEINVITE	= 13,			//邀请双人骑
		LOGICEVENT_DOUBLERIDEREQUEST= 14,			//请求双人骑
		LOGICEVENT_TRANSPORTINVITE	= 15,			//邀请传送
		LOGICEVENT_COPYMAPINVITE	= 16,			//邀请副本
		LOGICEVENT_COPYMAPREQUEST	= 17,			//请求副本
		LOGICEVENT_JOINGROUPINVITE	= 18,			//邀请组团
		LOGICEVENT_INVITEMASTER		= 19,			//邀请拜师
		LOGICEVENT_INVITEAPPRENTICE	= 20,			//邀请收徒
		LOGICEVENT_MARKETPLACEINVITE= 21,			//邀请商城购买
		LOGICEVENT_NOTIFYNEWMAIL	= 22,			//通知新邮件
	};

	// 宠物操作
	enum enPetOperate
	{
		PetOp_Spawn						= 0,		//召唤宠物
		PetOp_Combo,								//合体宠物
		PetOp_Dispatch,								//派遣宠物
		PetOp_Disband,								//解散宠物
		PetOp_Release,								//放生宠物
		PetOp_LianHua,								//炼化宠物
		PetOp_Insight,								//宠物提高灵慧
		PetOp_Identify,								//宠物成长率鉴定
		PetOp_Feed,									//宠物喂食
		PetOp_Learn,								//宠物修行
		PetOp_UseEgg,								//使用宠物蛋
		PetOp_BecomeBaby,							//宠物还童
		PetOp_ChangeName,							//宠物改名
		PetOp_ChangeProperties,						//宠物修改属性点
		PetOp_UnLink,								//断开宠物指针

		PetOp_AddExp,								//提高宠物经验
		PetOp_AddLevel,								//提高宠物等级
	};

	// 骑宠操作
	enum enMountPetOperate
	{
		PetOp_UseMountPetEgg			= 0,		//使用骑宠蛋
		PetOp_SpawnMountPet,						//召唤骑宠
		PetOp_DisbandMountPet,						//解散骑宠
		PetOp_ReleaseMountPet,						//放生骑宠
	};

	enum enSpiritOperate
	{
		SpiritOp_OpenSpiritSlot			= 0,		//开启元神栏位		
		SpiritOp_AddSpirit,							//增加元神
		SpiritOp_RemoveSpirit,						//丢弃元神
		SpiritOp_ActivateSpirit,					//激活元神
		SpiritOp_HuanHuaSpirit,						//幻化元神
		SpiritOp_CancelHuanHuaSpirit,				//取消幻化元神
		SpiritOp_Initialize,						//初始化
		SpiritOp_OpenJueZhaoWnd,					//打开元神绝招操作界面
		SpiritOp_CloseJueZhaoWnd,					//关闭元神绝招操作界面
		SpiritOp_OpenJueZhaoSlot,					//元神绝招栏解封
		SpiritOp_AddJueZhao,						//添加元神绝招技能
		SpiritOp_RemoveJueZhao,						//删除元神绝招技能
		SpiritOp_AddSpiritLevel,					//提高元神等级
		SpiritOp_AddSpiritExp,						//提高元神经验
		SpiritOp_AddTalentLevel,					//提高元神天赋等级
	};


	struct stLogicEvent
	{
		U32		id;									//逻辑事件ID
		U32		start;								//发起时间
		U32		type;								//逻辑事件类型
		U32		sender;								//发起人ID
		U32		relationid;							//关联逻辑事件ID
		StringTableEntry senderName;				//发起人角色名称
		char*	data;								//逻辑事件私有数据
		U32		datalen;							//私有数据长度		
	};
	//用于时效道具的计时操作
	struct stTimeItem
	{
		U64		uID;								//物品uID
		U32		uStartTime;							//开始时间
		U32		uLeaveTime;							//剩余时间
	};
	// 货币方式
	enum enCurrencyType
	{
		Currentcy_Money			= 1,				//金钱（金元）
		Currentcy_BindMoney		= 2,				//绑定金钱（灵元）
		Currentcy_Gold			= 3,				//仙石
		Currentcy_BindGold		= 4,				//神石
		Currentcy_Credit		= 5,				//声望
		Currentcy_BankMoney		= 6,				//银行金钱
	};

	enum enTradeLockFlag
	{
		TradeLock_Password,							//设定了第二密码
		TradeLock_Bank,								//银行锁定
		TradeLock_Trade,							//玩家交易锁定
		TradeLock_Stall,							//摆摊锁定
	};

	stTeamInfo		mTeamInfo;						//角色组队信息
	bool			mIsSaveDirty;
protected:
	// ------------------------------------------------------------------------
	//	玩家一般数据
	// ------------------------------------------------------------------------
	StatsPlusTable	mPlusTable;						//数值增强
	S32				mAccountId;						//帐号ID
	S32				mPlayerId;						//角色ID
	U32				mSwitchState;					//玩家控制的开关状态
	U32				mPlayerFunction;				//角色功能标志
	S32				mZoneID;						//下线地图编号
	S32				mMoney;							//角色金钱
	S32				mBankMoney;						//银行仓库金钱
	S32				mBindMoney;						//角色绑定金钱
	bool			mBankLock;						//
	U32				mTradeLockFlag;					//第二密码锁定功能标志
	U32				mVigor;							//当前活力，生活技能使用
	U32             mMaxVigor;                      //最大活力 
	U32             mCurrentLivingExp;              //当前生活经验值
	U32             mLivingLevel;                   //生活等级
	U32             mLivingSkillStudyID;            //修练专精生活技能ID

	Vector<stLogicEvent*>	mLogicEventQueue;		//游戏逻辑延迟处理的事件队列
	
	SimObjectPtr<SceneObject>	mInteractionObject;	//交互目标
	U32							mInteractionState;	//交互状态

	int				mLastLoginTime;

	PetTable		mPetTable;						//宠物系统
	SpiritTable		mSpiritTable;					//元神系统
	stFame			mFame[MaxInfluence];			//声望
	//---------------------------------------------------------------------------
	// 玩家社会关系数据
	//---------------------------------------------------------------------------

	MPManager*      pMPManagerInfo;                 // 师徒

	int				mLastSaveTime;
	int				mLastLotRequestTime;			// 当玩家大于70级以后每隔若干时间自动进入命运有缘人排队


	
public:
	U32				mOtherFlag;						//其它功能标志(例如:是否显示时装)
	TalentTable		mTalentTable;					//天赋

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
	//	宠物系统
	// ------------------------------------------------------------------------

	const inline PetTable&	getPetTable				() { return mPetTable;}
	S32						mountPet				(U32 slot);
	bool					comboPet				(U32 slot);
	bool					dispatchPet				(U32 slot);
	PetObject*				getPet					(U32 petIdx) { return mPetTable.getPet(petIdx);}
	void					setPet					(PetObject* pet) { mPetTable.setPet(pet);}

	// ------------------------------------------------------------------------
	//	元神系统
	// ------------------------------------------------------------------------
	inline SpiritTable&		getSpiritTable			() { return mSpiritTable;}

	// ------------------------------------------------------------------------
	//	装备耐久度相关函数
	// ------------------------------------------------------------------------
	U32						getCumulateDamage		() { return mCumulateDamage; }
	U32						getCumulateAttack		() { return mCumulateAttack; }
	void					setCumulateDamage		(U32 damage) { mCumulateDamage = damage; }
	void					setCumulateAttack		(U32 attack) { mCumulateAttack = attack; }
#ifdef NTJ_SERVER
	void					reduceWear				(U32 type , U32 damage); // type 1 为武器 2 为防具
	void					reduceAllWear			();
#endif//NTJ_SERVER

	//-------------------------------------------------------------------------
	// 银行系统
	//-------------------------------------------------------------------------
	
	bool			getBankFlag()	{ return mBankLock; } 
	void			setBankFlag(bool flag) { mBankLock = flag; }
	U32				getTradeFlag() { return mTradeLockFlag; }
	void			setTradeFlag(U32 flag) { mTradeLockFlag = flag; }
	void			setBankMoney(U32 money) { mBankMoney = money; }
	// ------------------------------------------------------------------------
	// 初始模型数据
	// ------------------------------------------------------------------------
	U32				mBody;
	U32				mFace;
	U32				mHair;
	U32				mHairCol;

	// ------------------------------------------------------------------------
	//	玩家任务数据
	// ------------------------------------------------------------------------
	stMission		missionInfo;
	Vector<S32>		missionShareList;

	// ------------------------------------------------------------------------
	//	玩家物品数据
	// ------------------------------------------------------------------------
	InventoryList	inventoryList;					//物品栏
	PanelList		panelList;						//快捷栏
	TradeList		tradeList;						//玩家交易栏
	EquipList		equipList;						//装备栏
	DumpList		dumpList;						//回购栏
	BankList		bankList;						//银行仓库栏
	NpcShopList		npcShopList;					//玩家NPC商品栏
	PickupList		pickupList;						//玩家拾取物品栏
	EquipIdentifyList identifylist;					//玩家鉴定栏
	EquipStrengthenList strengthenlist;				//装备强化栏
	MountGemList	mountGemList;					//宝石镶嵌栏
	PunchHoleList	punchHoleList;					//装备打孔栏
	TempList		tempList;						//临时栏
	MissionSubmitList missionSubmitList;			//任务提交栏
	U32				mShowTriggerID;					//需显示的区域ID
	Prescription*   pPrescription;                  //配方
	LivingSkillTable*    pLivingSkill;              //生活技能
#ifndef NTJ_EDITOR
	ItemSplit       mItemSplitList;                 //物品分解栏 
	ItemCompose     mItemComposeList;               //物品合成栏
	SkillList_Hint   skillList_Hint;                //玩家技能提示栏

#endif

	IndividualStallList individualStallList;		//个人摆摊栏
	StallPetList		stallPetList;				//摆摊宠物栏
	StallBuyItemList	stallBuyItemList;			//摆摊收购栏
	PetList			petList;						//宠物栏
	MountPetList	mountPetList;					//骑宠栏
	SpiritList mSpiritList;							//元神栏
	RepairList mRepairList;							//修理栏

	
	Vector<stTradeRecord*>	tradeRecord;			//在线交易记录列表
	

	TimerTriggerManager *mTimerTriggerMgr;			//计时触发器管理类
	TimerTriggerManager *mTimerPetStudyMgr;			//宠物修行计时触发器管理类

	//用于扣减耐久度
	U32				mCumulateDamage;				// 伤害累加值
	U32				mCumulateAttack;				// 攻击输出累加值
	// ========================================================================
	// End:服务端和客户端共有的变量和方法（Both Server and Client）
	// ========================================================================

	// ========================================================================
	// Begin:服务端仅有的变量和方法（Only Server）
	// ========================================================================
#ifdef NTJ_SERVER
public:
	U32				mBeginPlayerTime;				//当前开始时间
	U32				mOfflineTime;					//下线时间
	U32				mOnlineMinutes;					//累计角色在线分钟数
	ScheduleEvent*  pScheduleEvent;
	static U16      mRecoverVigorVelocity;          //活力恢复速度
	bool            mIsRecoverVigor;                //是否可恢复活力            

	bool			mIsAdult;						// 是否是成人
	U32				dTotalOnlineTime;				// 防沉迷累计在线时间

	int				mPointNum;

	int				mSocialCount;
	stSocialItem	mSocialItems[SOCAIL_ITEM_MAX];

	std::string		mCopyMapPosition;				// 将要进入的副本坐标

	int				mCopymapInstId;

	Vector<stTimeItem*>	mTimeItemVec;				//时效道具列表
private:
	bool			mEventInitialized;				//事件初始化标记
	T_UID			mUID;
	bool			mTransporting;					//传送标志
	bool			mIsInitializeEvent;				//是否向客户端更新过初始化数据
	StringTableEntry mSecondPassword;				//二次密码

	U8				mPlayerDataFlags[MAX_PLAYERDATAFLAGS];	//策划用值

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

	bool			sendPlayerEvent					(U32 mask, bool isFirstUpdate = false);	// 向客户端发送基本属性数据(只有玩家本身能看到的数据)
	bool			sendInitializEvent				();//Player数据初始化向客户端更新事件
	bool			hasInitializEvent				() {return mIsInitializeEvent;};
	void			sendInitializCompleteEvent		();

	void			initializeSendSkill				();		//向客户端发送技能列表
	void            initSendLivingSkill             ();     //向客户端发送生活技能列表
	void            initSendPrescription            ();     //向客户端发送配方列表
	void			killNpc							(NpcObject* npc);
	StringTableEntry getSecondPassWord				() { return mSecondPassword; }//得到二次密码
	
	void            recoverVigor                    ();
	stSocialItem*   getPlayerSocialItem             (U32 playerID);  // 获得自已与目标玩家社会关系
	inline stSocialItem* getSocialItem              (){ return mSocialItems;}   
	inline S32      getSocialCount                  (){ return mSocialCount;}
	void			updateFashionFlags				();

	//添加到实效道具列表
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
	// End:服务端仅有的变量和方法（Only Server）
	// ========================================================================

	// ========================================================================
	// Begin:客户端仅有的变量和方法（Only Client）
	// ========================================================================
#ifdef NTJ_CLIENT
public:
	// ------------------------------------------------------------------------
	//	玩家技能数据 - 仅存在客户端
	// ------------------------------------------------------------------------
	SkillList_Common skillList_Common;				//玩家通用技能栏
	SkillList_Study  skillList_Study;               //玩家技能学习栏
	AutoSellList	 autoSellList;					//售物助手
	MissionItemList	 missionItemList;				//任务奖励物品栏
	StallLookUpList  lookUpList;					//查找清单
	PetHelpList		 mPetHelpList;					//宠物清单列表
	TradeAndStallHelpList mTradeStallHelpList;		//摆摊交易助手
	SuperMarketList mSuperMarketList;				//商城栏
	RecommendItemList mRecommendList;				//推荐栏
	ShopBaksetList mShopBaksetList;					//购物篮

	PrizeBox*		mPrizeBox;
	Vector< SimObjectPtr<CollectionObject> > mCollectionList;		//被感知的采集物集合
	SimObjectPtr<CollectionObject>		mSelCollectionPtr;			// 当前选择的采集物引用
	SimObjectPtr<PrizeBox>				mSelPrizeBoxPtr;			// 当前选择的宝箱引用
	LivingSkillList* mLivingSkillList;             // 生活技能栏
	PrescriptionList* mPrescriptionList;           // 配方材料栏

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
	// End:客户端仅有的变量和方法（Only Client）
	// ========================================================================

	// ========================================================================
	// Begin:编辑器仅有的变量和方法（Only Editor）
	// ========================================================================

	// ========================================================================
	// End:编辑器仅有的变量和方法（Only Editor）
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