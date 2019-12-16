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
/* GM�������
/************************************************************************/
enum GMError
{
    GM_ERROR_NONE = 0,       //�����ɹ�
    GM_ERROR_AUTH,           //Ȩ��
    GM_ERROR_CMD,            //�������
    GM_ERROR_NOPLAYER,       //�Ҳ������
    GM_ERROR_ACCOUNT_ONLINE, //�ʺ�����
    GM_ERROR_PLAYER_ONLINE,  //�������
    GM_ERROR_TOOMUCHPLAYER,  //��ɫ��������

    GM_ERROR_COUNT          
};

static char* StrGMError[GM_ERROR_COUNT] = 
{
    "�����ɹ�",
    "û��Ȩ��",
    "�������",
    "�Ҳ������",
    "�ʺ���Ҫ������",
    "�����Ҫ������",
    "��ɫ��������",
};

/************************************************************************/
/* �ʺŵ�״̬
/************************************************************************/
enum AccountStatus
{
    ACCOUNT_STATUS_NORMAL = 0,          //����
    ACCOUNT_STATUS_TEST      = BIT(0)  ,   //�����ʺ�
    ACCOUNT_STATUS_BAN       = BIT(1)   ,  //��ֹ����
    ACCOUNT_STATUS_FREEZE    = BIT(2)   ,  //����
    ACCOUNT_STATUS_IPFREEZE  = BIT(3)   ,   //ip����
    ACCOUNT_STATUS_MUTE      = BIT(4)   ,  //���� 
    ACCOUNT_STATUS_TODEL     = BIT(5)   ,  //��ɾ��
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
/* ��ɫ��ѯ����
/************************************************************************/
class CGMAccountQueryResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMAccountQueryResp);

    char    accountName[32];    //�ʺ�����
    bool    isGM;               //�Ƿ���GM
    int     accountId;          //�ʺ�ID
    int     gameZone;           //����
    int     status;             //״̬
    int     freezeTime;         //����ʱ��
    char    freezeReason[100];  //����ԭ��
    int     lastLoginTime;      //����¼��ʱ��
    int     lastLogoutTime;     //���ǳ���ʱ��
    char    lastLoginIp[32];    //����¼��IP
    char    actor[5][32];       //�ʺŶ�Ӧ�Ľ�ɫ
    int     actorId[5];
};

enum AccountFreezeError
{
    ERROR_ACCOUNT_FREEZE_NONE,      //������ȷ
    ERROR_ACCOUNT_FREEZE_ONLINE,    //�������
};

class CGMActorAccountOpResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMActorAccountOpResp);

    char        name[32];        //�ʺŻ��ɫ����
    int         error;          
};

/************************************************************************/
/* GM��ɫ��ѯ
/************************************************************************/
class CGMQueryResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMQueryResp);

    int     accountId;           //�ʺ�ID
    char    accountName[32];     //����
};

/************************************************************************/
/* GM��ɫȨ�޷���
/************************************************************************/
class CGMRoleResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMRoleResp);

    int     gmFlag;       
    char    role[60];        //����
};

/************************************************************************/
/* ���ɾ��Ȩ�޷���
/************************************************************************/
class CGMAddRoleResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMAddRoleResp);

    int     gmFlag;          
    char    role[60];           //����
};

class CGMDelRoleResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMDelRoleResp);

    int     gmFlag;             
    char    role[60];           //����
};

class CGMMdfRolResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMMdfRolResp);

    int     gmFlag;             
    char    accountName[32]; 
};

/************************************************************************/
/* ��һ�����Ϣ����
/************************************************************************/
class CGMPlayerBaseInfo:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerBaseInfo);

    int     accountId;                      //�ʺ�ID
    char    playerName[32];                 //�������
    int     hp;                             
    int     mp;                             
    int     playerHot;                      //��Եֵ
    int     CurrentPP;                      //��ǰԪ��ֵ
    int     CurrentVigor;                   //��ǰ����ֵ
    int     freezeTime;                     //����ʱ��
    int     muteTime;                       //����ʱ��
    int     zone;                           //��ͼ
    int     playerId;                       //���ID
    int     sex;                            //����Ա�
    int     level;                          //�ȼ�
    int     status;                         //״̬
    int     isOnline;                       //�Ƿ�����
    int     gold;                           //���
    int     exp;                            //��ǰ����
    int     loginCount;                     //��¼����
    char    Race;					        //����
    char    Family;					        //����
    char    Classes[MaxClassesMasters];     //ְҵ
    int     createTime;                     //����ʱ��
    int     lastLoginTime;                  //�����¼ʱ��
    int     lastLogoutTime;                 //����ǳ�ʱ��
    char    LeavePos[COMMON_STRING_LENGTH]; //�뿪��ͼʱ��λ��
};

/************************************************************************/
/* ��Ҽ�����Ϣ����
/************************************************************************/
class CGMPlayerSkillInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerSkillInfoResp);
    
    int          playerId;
    unsigned int skillId;                //����ID
};

class CGMPlayerSkillOpResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerSkillOpResp);

    int      playerId;
    unsigned int skillId;                //����ID
};

class CGMPlayerLivingSkillResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerLivingSkillResp);

    int          playerId;
    unsigned int skillId;                //����ID
    unsigned int Ripe;                   //������
};

/************************************************************************/
/* ���������Ϣ����
/************************************************************************/

//�����
class CGMPlayerQuestFinishedInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerQuestFinishedInfoResp);

    int       playerId;
    int       questId;
};

//ѭ������
class CGMPlayerQuestCycleInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerQuestCycleInfoResp);

    int       playerId;
    stMissionCycle info;
};

//��ǰ�ѽ�����
class CGMPlayerQuestAcceptInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerQuestAcceptInfoResp);

    int         playerId;
    stMissionFlag info;
};

/************************************************************************/
/* ��ɫ������Ϣ����
/************************************************************************/
class CGMPlayerFriendInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerFriendInfoResp);

    int     playerId;
    char    friendName[32];     //��������
    int     type;               //����
    int     friendValue;        //�Ѻö�
    bool    isOnline;           //�Ƿ�����
};

/************************************************************************/
/* ��ɫ������Ϣ����
/************************************************************************/
class CGMPlayerRideInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerRideInfoResp);

    int         playerId;
    stMountInfo info;
};

/************************************************************************/
/* �����Ʒ�Ļ�����Ϣ����
/************************************************************************/
class CGMPlayerItemBaseInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerItemBaseInfoResp);

    int     playerId;               //���ID
    U64     uid;                    //ΨһID
    int     itemID;                 //ģ��ID
    int     slot;                   //
};

//��ҳ�����Ϣ
class CGMPlayerPetResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerPetResp);

    int     playerId;
    int     petId;
    int     petDataId;
};

//�������ϸ��Ϣ
class CGMPlayerPetDetailInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerPetDetailInfoResp);

    int       playerId;
    stPetInfo petInfo;
};

/************************************************************************/
/* ��ϸ����Ʒ��Ϣ
/************************************************************************/
class CGMPlayerItemDetailInfoResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerItemDetailInfoResp);
    
    char Producer[COMMON_STRING_LENGTH];// ����������
    U64 UID;				// ��Ʒ����ΨһID
    U32 ItemID;				// ��Ʒģ��ID
    U32 Quantity;			// ��Ʒ����(����װ����ֻ��Ϊ1)
    S32 LapseTime;			// ʣ������
    S32 RemainUseTimes;		// ʣ��ʹ�ô���
    U32 BindPlayer;			// ����ID(Ҳ�����ڴ洢������ӿ��������ID)

    U32 IsEquip;			// �Ƿ�װ����Ʒ
    U32	Quality;			// ��ƷƷ�ʵȼ�

    S32 CurWear;			// ��ǰ�;ö�
    S32 CurMaxWear;			// ��ǰ����;ö�
    U32 EnhanceWears;		// ǿ���;ô���

    S32 CurAdroit;			// ��ǰ������
    U32 CurAdroitLv;		// ��ǰ�����ȵȼ�

    U32 ActivatePro;		// Ч�������־

    U32 RandPropertyID;		// �Ƿ������������
    U32 IDEProNum;			// �����󸽼����Ը���
    U32 IDEProValue[MAX_IDEPROS];// �����󸽼�����ֵ

    U32 EquipStrengthens;		// װ��ǿ��������
    U32 EquipStrengthenValue[MAX_EQUIPSTENGTHENS][2];

    U32 ImpressID;			// ����ID(״̬ID)
    U32 BindProID;			// ���󶨸�������(״̬ID)

    U32 WuXingID;			// ����ID
    U32 WuXingLinkID;		// ��������ID
    U32 WuXingPro;			// ����Ч������

    U32 SkillAForEquip;		// װ���ϻ�ȡ����������ID
    U32 SkillBForEquip;		// װ���ϻ�ȡ�ı�������ID

    U32 EmbedOpened;		// ������Ƕ�ף�ÿ4bitλ����һ���ף�
    U32	EmbedSlot[MAX_EMBEDSLOTS];// װ����Ƕ�׵ı�ʯID
};

/************************************************************************/
/* ��Ʒ�޸ķ���
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
/* ��Ʒɾ������
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
/* ��Ʒɾ������
/************************************************************************/
class CGMPlayerTransportResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerTransportResp);

    int playerId;
};

/************************************************************************/
/* GM��־��Ϣ����
/************************************************************************/
class CGMLogResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMLogResp);

    char    gmName[32];             //��ǰ���
    int     gmFlag;                 //GM��־
    char    gmCmd[60];              //GM����
    char    gmContent[200];         //GM����
    int     time;                   //ʱ��
};

/************************************************************************/
/* �޸�GM���뷵��
/************************************************************************/
class CGMPasswordChangeResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPasswordChangeResp);
};

/************************************************************************/
/* ��ɾ����ɫ��Ϣ����
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
/* ɾ����ɫ����
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
/* ��ɫ������Ϣ�޸ķ���
/************************************************************************/
class CGMPlayerBaseInfoMdfResp:public CGMRespBase
{
public:
    DECL_GM_CLS(CGMPlayerBaseInfoMdfResp);

    int  playerId;
};

enum CharTarget
{
    CHAT_TARGET_PERSON = 0, //����
    CHAT_TARGET_TEAM,       //���
    CHAT_TARGET_GUILD,      //����
    CHAT_TARGET_ZONE,       //��ͼ
    CHAT_TARGET_ALLZONE,    //ȫ��
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