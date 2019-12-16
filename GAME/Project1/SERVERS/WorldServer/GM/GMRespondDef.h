#ifndef _GMRESPONDDEF_H_
#define _GMRESPONDDEF_H_

#include <iostream>
#include "base/bitStream.h"
#include "base/types.h"
#include "Common\PacketType.h"
#include "Common\PlayerStruct.h"

#ifdef GM_CLIENT
class ClientGameplayState;
#endif

class CGMRespBase
{
public:
    CGMRespBase(void) : m_pClient(0) {}
    virtual ~CGMRespBase(void) {}

    virtual void pack( Base::BitStream& packet ) = 0;
    virtual void unpack( Base::BitStream& packet ) = 0;
    virtual int size(void) = 0;
    
#ifndef GM_CLIENT
    void* m_pClient;
#else
    ClientGameplayState* m_pClient;
#endif
};

static const char* GMHelper_ToStr(int value)
{
    static char buf[1024];
    sprintf_s(buf,sizeof(buf),"%d",value);
    
    return buf;
}

static const char* GMHelper_ToStr(unsigned int value)
{
    static char buf[1024];
    sprintf_s(buf,sizeof(buf),"%d",value);

    return buf;
}

static const char* GMHelper_ToStr(unsigned __int64 value)
{
    static char buf[1024];
    _i64toa_s(value,buf,sizeof(buf),10);

    return buf;
}

#define DECL_GM_CLS(cls)\
    virtual void pack( Base::BitStream& packet )\
    {\
         packet.writeBits(sizeof(cls) * 8,this);\
    }\
    virtual void unpack( Base::BitStream& packet )\
    {\
        packet.readBits(sizeof(cls) * 8,this);\
    }\
    virtual int size(void)  {return sizeof(cls);}\


/************************************************************************/
/* GM命令错误
/************************************************************************/
enum GMError
{
    GM_ERROR_NONE = 0,       //操作成功
    GM_ERROR_AUTH,           //权限
    GM_ERROR_CMD,            //命令错误
    GM_ERROR_NOPLAYER,       //找不到玩家
    GM_ERROR_ACCOUNT_ONLINE, //帐号在线
    GM_ERROR_PLAYER_ONLINE,  //玩家在线
    GM_ERROR_TOOMUCHPLAYER,  //角色数量过多

    GM_ERROR_COUNT          
};

static char* StrGMError[GM_ERROR_COUNT] = 
{
    "操作成功",
    "没有权限",
    "命令错误",
    "找不到玩家",
    "帐号需要先下线",
    "玩家需要先下线",
    "角色数量已满",
};

/************************************************************************/
/* 帐号的状态
/************************************************************************/
enum AccountStatus
{
    ACCOUNT_STATUS_NORMAL = 0,          //正常
    ACCOUNT_STATUS_TEST      = BIT(0)  ,   //测试帐号
    ACCOUNT_STATUS_BAN       = BIT(1)   ,  //禁止登入
    ACCOUNT_STATUS_FREEZE    = BIT(2)   ,  //冻结
    ACCOUNT_STATUS_IPFREEZE  = BIT(3)   ,   //ip冻结
    ACCOUNT_STATUS_MUTE      = BIT(4)   ,  //禁言 
    ACCOUNT_STATUS_TODEL     = BIT(5)   ,  //待删除
};

struct CGMAccountQueryFilter
{
    CGMAccountQueryFilter(void)
    {
        ZeroMemory(this,sizeof(CGMAccountQueryFilter));
    }

    int        isAdult;
    int        isGM;
    int        status;
    int        maxQuery;
    __time64_t timeStart;
    __time64_t timeEnd;
};

/************************************************************************/
/* 角色查询返回
/************************************************************************/
class CGMAccountQueryResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMAccountQueryResp);

    char    accountName[32];    //帐号名称
    bool    isGM;               //是否是GM
    int     accountId;          //帐号ID
    int     gameZone;           //大区
    int     status;             //状态
    int     freezeTime;         //冻结时间
    char    freezeReason[100];  //冻结原因
    int     lastLoginTime;      //最后登录的时间
    int     lastLogoutTime;     //最后登出的时间
    char    lastLoginIp[32];    //最后登录的IP
    char    actor[5][32];       //帐号对应的角色
    int     actorId[5];
};

enum AccountFreezeError
{
    ERROR_ACCOUNT_FREEZE_NONE,      //返回正确
    ERROR_ACCOUNT_FREEZE_ONLINE,    //玩家在线
};

class CGMActorAccountOpResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMActorAccountOpResp);

    char        name[32];        //帐号或角色名称
    int         error;          
};

/************************************************************************/
/* GM角色查询
/************************************************************************/
class CGMQueryResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMQueryResp);

    int     accountId;           //帐号ID
    char    accountName[32];     //名称
};

/************************************************************************/
/* GM角色权限返回
/************************************************************************/
class CGMRoleResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMRoleResp);

    int     gmFlag;       
    char    role[60];        //名称
};

/************************************************************************/
/* 添加删除权限返回
/************************************************************************/
class CGMAddRoleResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMAddRoleResp);

    int     gmFlag;          
    char    role[60];           //名称
};

class CGMDelRoleResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMDelRoleResp);

    int     gmFlag;             
    char    role[60];           //名称
};

class CGMMdfRolResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMMdfRolResp);

    int     gmFlag;             
    char    accountName[32]; 
};

/************************************************************************/
/* 玩家基本信息返回
/************************************************************************/
class CGMPlayerBaseInfo:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerBaseInfo);

    int     accountId;                      //帐号ID
    char    playerName[32];                 //玩家名称
    int     hp;                             
    int     mp;                             
    int     playerHot;                      //仙缘值
    int     CurrentPP;                      //当前元气值
    int     CurrentVigor;                   //当前活力值
    int     freezeTime;                     //冻结时间
    int     muteTime;                       //禁言时间
    int     zone;                           //地图
    int     playerId;                       //玩家ID
    int     sex;                            //玩家性别
    int     level;                          //等级
    int     status;                         //状态
    int     isOnline;                       //是否在线
    int     gold;                           //金币
    int     exp;                            //当前经验
    int     loginCount;                     //登录次数
    char    Race;					        //种族
    char    Family;					        //门宗
    char    Classes[MaxClassesMasters];     //职业
    int     createTime;                     //创建时间
    int     lastLoginTime;                  //最近登录时间
    int     lastLogoutTime;                 //最近登出时间
    char    LeavePos[COMMON_STRING_LENGTH]; //离开地图时的位置
};

/************************************************************************/
/* 玩家技能信息返回
/************************************************************************/
class CGMPlayerSkillInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerSkillInfoResp);
    
    int          playerId;
    unsigned int skillId;                //技能ID
};

class CGMPlayerSkillOpResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerSkillOpResp);

    int      playerId;
    unsigned int skillId;                //技能ID
};

class CGMPlayerLivingSkillResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerLivingSkillResp);

    int          playerId;
    unsigned int skillId;                //技能ID
    unsigned int Ripe;                   //熟练度
};

/************************************************************************/
/* 玩家任务信息返回
/************************************************************************/

//已完成
class CGMPlayerQuestFinishedInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerQuestFinishedInfoResp);

    int       playerId;
    int       questId;
};

//循环任务
class CGMPlayerQuestCycleInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerQuestCycleInfoResp);

    int       playerId;
    stMissionCycle info;
};

//当前已接任务
class CGMPlayerQuestAcceptInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerQuestAcceptInfoResp);

    int         playerId;
    stMissionFlag info;
};

/************************************************************************/
/* 角色好友信息返回
/************************************************************************/
class CGMPlayerFriendInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerFriendInfoResp);

    int     playerId;
    char    friendName[32];     //好友名称
    int     type;               //类型
    int     friendValue;        //友好度
    bool    isOnline;           //是否在线
};

/************************************************************************/
/* 角色坐骑信息返回
/************************************************************************/
class CGMPlayerRideInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerRideInfoResp);

    int         playerId;
    stMountInfo info;
};

/************************************************************************/
/* 玩家物品的基本信息返回
/************************************************************************/
class CGMPlayerItemBaseInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerItemBaseInfoResp);

    int     playerId;               //玩家ID
    U64     uid;                    //唯一ID
    int     itemID;                 //模板ID
    int     slot;                   //
};

//玩家宠物信息
class CGMPlayerPetResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerPetResp);

    int     playerId;
    int     petId;
    int     petDataId;
};

//宠物的详细信息
class CGMPlayerPetDetailInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerPetDetailInfoResp);

    int       playerId;
    stPetInfo petInfo;
};

/************************************************************************/
/* 详细的物品信息
/************************************************************************/
class CGMPlayerItemDetailInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerItemDetailInfoResp);
    
    char Producer[COMMON_STRING_LENGTH];// 制造者名称
    U64 UID;				// 物品世界唯一ID
    U32 ItemID;				// 物品模板ID
    U32 Quantity;			// 物品数量(对于装备，只能为1)
    S32 LapseTime;			// 剩余秒数
    S32 RemainUseTimes;		// 剩余使用次数
    U32 BindPlayer;			// 绑定者ID(也可用于存储灵魂链接开启后玩家ID)

    U32 IsEquip;			// 是否装备物品
    U32	Quality;			// 物品品质等级

    S32 CurWear;			// 当前耐久度
    S32 CurMaxWear;			// 当前最大耐久度
    U32 EnhanceWears;		// 强化耐久次数

    S32 CurAdroit;			// 当前熟练度
    U32 CurAdroitLv;		// 当前熟练度等级

    U32 ActivatePro;		// 效果激活标志

    U32 RandPropertyID;		// 是否随机名称属性
    U32 IDEProNum;			// 鉴定后附加属性个数
    U32 IDEProValue[MAX_IDEPROS];// 鉴定后附加属性值

    U32 EquipStrengthens;		// 装备强化过总数
    U32 EquipStrengthenValue[MAX_EQUIPSTENGTHENS][2];

    U32 ImpressID;			// 铭刻ID(状态ID)
    U32 BindProID;			// 灵魂绑定附加属性(状态ID)

    U32 WuXingID;			// 五行ID
    U32 WuXingLinkID;		// 五行链接ID
    U32 WuXingPro;			// 五行效果属性

    U32 SkillAForEquip;		// 装备上获取的主动技能ID
    U32 SkillBForEquip;		// 装备上获取的被动技能ID

    U32 EmbedOpened;		// 开启镶嵌孔（每4bit位代表一个孔）
    U32	EmbedSlot[MAX_EMBEDSLOTS];// 装备镶嵌孔的宝石ID
};

/************************************************************************/
/* 物品修改返回
/************************************************************************/
class CGMPlayerItemModifyResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerItemModifyResp);

    int playerId;
    U64 itemUID;
    int error;
};

/************************************************************************/
/* 物品删除返回
/************************************************************************/
class CGMPlayerItemDelResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerItemDelResp);

    int playerId;
    U64 itemUID;
    int error;
};

/************************************************************************/
/* 物品删除返回
/************************************************************************/
class CGMPlayerTransportResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerTransportResp);

    int playerId;
};

/************************************************************************/
/* GM日志信息返回
/************************************************************************/
class CGMLogResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMLogResp);

    char    gmName[32];             //当前玩家
    int     gmFlag;                 //GM标志
    char    gmCmd[60];              //GM命令
    char    gmContent[200];         //GM内容
    int     time;                   //时间
};

/************************************************************************/
/* 修改GM密码返回
/************************************************************************/
class CGMPasswordChangeResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPasswordChangeResp);
};

/************************************************************************/
/* 已删除角色信息返回
/************************************************************************/
class CGMQueryDelActorResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMQueryDelActorResp);

    int  accountId;
    int  playerId;
    char playerName[32];
};

/************************************************************************/
/* 删除角色返回
/************************************************************************/
class CGMDelActorResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMDelActorResp);

    int  playerId;
};

class CGMUnDelActorResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMUnDelActorResp);

    int  playerId;
};

/************************************************************************/
/* 角色基本信息修改返回
/************************************************************************/
class CGMPlayerBaseInfoMdfResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerBaseInfoMdfResp);

    int  playerId;
};

enum CharTarget
{
    CHAT_TARGET_PERSON = 0, //个人
    CHAT_TARGET_TEAM,       //组队
    CHAT_TARGET_GUILD,      //公会
    CHAT_TARGET_ZONE,       //地图
    CHAT_TARGET_ALLZONE,    //全服
};

class CGMChatResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMChatResp);
};

class CGMActorPosResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMActorPosResp);
    
    int playerId;
    float x;
    float y;
    float z;
};

class CGMActorRenameResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMActorRenameResp);

    int  playerId;
    char newName[32];
};

#endif /*_GMRESPONDDEF_H_*/