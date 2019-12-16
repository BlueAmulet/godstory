#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

#define ITOA(NUMBER) #NUMBER
#define __TEXT_LINE__(LINE) ITOA(LINE)
#ifdef _DEBUG
#define HACK(MSG) __FILE__ "("__TEXT_LINE__(__LINE__) ") : ��HACK�� �� " #MSG
#else
#define HACK(MSG) __FILE__ "("__TEXT_LINE__(__LINE__) ") : HACK!! Error: " #MSG
#endif

#define DB_CATCH_LOG(TAG)                                                     \
	catch (CDBException & e)                                                  \
	{                                                                         \
		stringstream LogStream;                                               \
		LogStream << "Error Message(" << #TAG << "):" << e.w_msgtext << ends; \
		g_Log.WriteError(LogStream);                                          \
	}                                                                         \
	catch (...)                                                               \
	{                                                                         \
		stringstream LogStream;                                               \
		LogStream << "Error Message(" << #TAG << "):���Զ����쳣" << ends;    \
		g_Log.WriteError(LogStream);                                          \
	}

const int MAX_PACKET_SIZE = 60 * 1024;
const int NETWORK_PROTOCOL_VERSION = 1;
const int COMMON_STRING_LENGTH = 32;
const int PET_MESSAGE_LENGTH = 64;
const int MD5_STRING_LENGTH = 16;
const int MAX_ACTOR_COUNT = 5;
const int MAX_DROP_TIME = 30000;
const int MAX_SPIRIT_SKILL = 6;
const int MAX_TALENT_ROW = 6;
const int MAX_TALENT_COL = 6;
const int MAX_TALENT = MAX_TALENT_ROW * MAX_TALENT_COL;

const unsigned int PLAYERSTRUCT_VERSION = 20090925; // ������ݽṹ�汾

const unsigned int MISSION_FLAG_MAX = 40;   // ÿ�������40���������
const unsigned int MISSION_ACCEPT_MAX = 20; // ÿ������20���ѽ�����
const unsigned int MISSION_CYCLE_MAX = 200; // ÿ�������200��ѭ������
const unsigned int MISSION_MAX = 4096 << 3; // ��Ϸ�������32768
const unsigned int MISSION_STATE_LENGTH = MISSION_MAX >> 3;
const unsigned int MISSION_FLAG_BITS = MISSION_FLAG_MAX * sizeof(int) * 2;
const unsigned int MAX_EMBEDSLOTS = 8;		  // ��Ƕ��ʯ������
const unsigned int MAX_IDEPROS = 12;		  // ��Ʒ�����󸽼�����������
const unsigned int MAX_EQUIPSTENGTHENS = 3;   // װ��ǿ���ȼ����Ч����
const unsigned int INVENTROY_MAXSLOTS = 120;  // ��Ʒ������λ��
const unsigned int PANEL_MAXSLOTS = 12 * 4;   // ���������λ��
const unsigned int BANK_MAXSLOTS = 120;		  // ����������λ��
const unsigned int EQUIP_MAXSLOTS = 26;		  // װ��������λ��
const unsigned int SKILL_MAXSLOTS = 200;	  // ����������λ��
const unsigned int BUFF_MAXSLOTS = 32;		  // ��󱣴�buff/debuff����
const unsigned int COOLDOWN_MAX = 250;		  // ��ȴʱ���������
const unsigned int PET_MAXSLOTS = 8;		  // ����������λ��
const unsigned int MOUNTPET_MAXSLOTS = 8;	 // ���������λ��
const unsigned int SPIRIT_MAXSLOTS = 5;		  // Ԫ��������λ��
const unsigned int PRESCRIPTION_MAX = 1000;   // �䷽������
const unsigned int MAX_PLAYERDATAFLAGS = 128; // �߻����ڼ�¼���ĳЩ���ݵ����
const unsigned int MAX_TIMER_SIZE = 4;		  // ʱ�䴥��������������
const unsigned int LIVINGSKILL_MAXSLOTS = 90; // �����������λ��
const unsigned int ITEMSPLIT_MAXSLOTS = 8;	// ���������λ��

const unsigned int SOCAIL_ITEM_MAX = 300; // ����ϵ�������ֵ

const unsigned int MPM_CALLBOARD_MAXSTRING = 128; // ʦ������������ַ���

#define T_UID int
#define UID_BITS 32

enum LOGIN_STATUS
{
	SF_FROM_ACCOUNT,
	SF_FROM_WORLD,
	SF_FROM_GATE,
	SF_FROM_ZONE
};

enum SERVER_TYPE
{
	SERVICE_NONE,
	SERVICE_CLIENT,
	SERVICE_ZONESERVER,
	SERVICE_ACCOUNTSERVER,
	SERVICE_WORLDSERVER,
	SERVICE_GATESERVER,
	SERVICE_GAMESERVER,
	SERVICE_DATAAGENT,
	SERVICE_LOGSERVER,
};

enum GAME_LOGIC_MSG
{
	BEGIN_LOGIC_MSG,
	ON_ZONE_RESTART,
	END_LOGIC_MSG,
};

enum GAME_NET_MSG
{
	BEGIN_NET_MESSAGE = 1,

	//�ڲ�ͨѶ��Ϣ---------------------------
	//ϵͳ��Ϣ
	COMMON_RegisterRequest, //������֮���ע����Ϣ
	COMMON_RegisterResponse,

	GATE_WORLD_ZoneRegisterRequest, //���ظ�������������Լ������Ǽ�����ͼ��ͨѶ

	ACCOUNT_WORLD_ClientLoginRequest, //�ʺŷ�����ת����¼��Ϣ�����������
	ACCOUNT_WORLD_ClientLoginResponse,

	GATE_WORLD_ClientLoginRequest, //���ط�����ת����¼��Ϣ�����������
	GATE_WORLD_ClientLoginResponse,

	WORLD_ZONE_PlayerData,
	WORLD_ZONE_PlayerDataResponse,

	CLIENT_WORLD_ItemRequest, // ��������������ȡ��Ʒ

	WORLD_COPYMAP_PlayerData, //�������ݵ�Zone

	WORLD_CLIENT_NOTIFY_KICKED, //֪ͨ������

	CLIENT_WORLD_IdCodeRequest, // ��֤

	ZONE_WORLD_AddPoint, // ������ʯ

	ZONE_WORLD_PlayerLogin, // ���ȷʵ��½��ZONE

	// DataAgent�����Ϣ
	//------------------------------------------------------------------------
	MSG_DATAAGENT_BEGIN,

	DATAAGENT_WORLD_NotifyPlayerDataAck, // WorldServer֪ͨDataAgent�����յ�
	DATAAGENT_WORLD_NotifyPlayerData,	// DataAgent��WorldServer֪ͨ��Ҕ���

	SERVER_DATAAGENT_Message, // ������DataAgent������Ҫ����

	SERVER_DATAAGENT_PlayerDataRequest, // WorldServer������DataAgent��ѯ����
	SERVER_DATAAGENT_PlayerDataResponse,

	ZONE_WORLD_PlayerSaveRequest, // Zone�����������
	ZONE_WORLD_PlayerSaveResponse,

	MSG_DATAAGENT_END,
	//-------------------------------------------------------------------------

	GATE_WORLD_ClientLost, // �ͻ��˶Ͽ��¼�֪ͨ
	GATE_WORLD_ZoneLost,   // ��ͼ�Ͽ��¼�֪ͨ

	ZONE_WORLD_PlayerDispChanged, // ��ͼ����֪ͨ�����ʾ���ݱ��

	WORLD_ZONE_TickPlayerRequest, // ֪ͨZoneServer�ߵ����
	WORLD_ZONE_TickPlayerResponse,
	WORLD_ZONE_TransportPlayerRequest, //word֪ͨZONE�������
	WORLD_ZONE_GetPlayerPosRequest,	//��ZONE��ȡ��ҵ�ǰ���ڵ�λ��
	WORLD_ZONE_GetPlayerPosResponse,

	//��Ϸ��Ϣ
	ZONE_WORLD_SwitchMapRequest, //�ͻ��������л���ͼ(���Կ���)
	ZONE_WORLD_SwitchMapResponse,

	//�ͻ�ͨѶ��Ϣ---------------------------
	CLIENT_ACCOUNT_LoginRequest, //�ͻ��������ʺŷ�������֤�ʺ�
	CLIENT_ACCOUNT_LoginResponse,
	CLIENT_ACCOUNT_CheckRequest, // �ͻ�����֤�ʺ�
	CLIENT_ACCOUNT_CheckResponse,
	CLIENT_GATE_LoginRequest, //�ͻ��˵�¼���ط�����
	CLIENT_GATE_LoginResponse,
	CLIENT_GAME_LoginRequest, //�ͻ���׼��ͨ�����ص�¼��Ϸ
	CLIENT_GAME_LoginResponse,
	CLIENT_GAME_LogoutRequest, //�ͻ��˶Ͽ���ͼ�������������˳���Ϸ
	CLIENT_GAME_LogoutResponse,

	CLIENT_WORLD_SelectPlayerRequest, //�ͻ��˲�ѯ��ɫ
	CLIENT_WORLD_SelectPlayerResponse,
	CLIENT_WORLD_DeletePlayerRequest, //�ͻ���ɾ����ɫ
	CLIENT_WORLD_DeletePlayerResponse,
	CLIENT_WORLD_CreatePlayerRequest, //�ͻ��˴�����ɫ
	CLIENT_WORLD_CreatePlayerResponse,

	CLIENT_WORLD_PlayerDataTransRequest, //���ͽ�ɫ��Ϣ
	CLIENT_WORLD_PlayerDataTransResponse,

	WORLD_ZONE_SavePlayerRequest, // �������󱣴��������
	WORLD_ZONE_SavePlayerResponse,

	// ������Ϣ--------------------------------
	MSG_TEAM_BEGIN,

	CLIENT_TEAM_Disband,			// ��ɢ����
	CLIENT_TEAM_BuildRequest,		// �������
	CLIENT_TEAM_AcceptBuildRequest, // �����������

	CLIENT_TEAM_BuildAlongRequest, // �����������

	CLIENT_TEAM_JoinRequest,		// ��������
	CLIENT_TEAM_AcceptdJoinRequest, // ͬ�����

	CLIENT_TEAM_LeaveRequest, // �뿪����

	CLIENT_TEAM_DropRequest, // ����

	CLIENT_TEAM_AddRequest,		  // ����
	CLIENT_TEAM_AcceptAddRequest, // ͬ�����

	CLIENT_TEAM_PlayerJoined, // ��Ա����
	CLIENT_TEAM_PlayerLeave,  // ��Ա�˳�
	CLIENT_TEAM_TeamInfo,	 // ������Ϣ
	CLIENT_TEAM_ShowTeamInfo, // �鿴������Ϣ

	CLIENT_TEAM_Error, // ������Ϣ

	CLIENT_TEAM_Refused, // �ܾ���Ϣ
	ClIENT_TEAM_Cancel,  // ȡ������

	CLIENT_TEAM_MapMark,	// ��ͼ���
	CLIENT_TEAM_TargetMark, // Ŀ����

	CLIENT_TEAM_BaseInfo, // ���������Ϣ,���������,�������,��Ʒ�����

	CLIENT_TEAM_Follow, // ������Ӹ���

	CLIENT_TEAM_InvationCopymap, // ���������

	MSG_TEAM_END,

	//���������Ϣ------------------------------------
	MSG_COPYMAP_BEGIN,

	CLIENT_COPYMAP_EnterRequest, // ���븱��
	CLIENT_COPYMAP_EnterResponse,

	CLIENT_COPYMAP_OpenRequest, // ��������
	CLIENT_COPYMAP_OpenResponse,

	CLIENT_COPYMAP_CloseRequest, // �رո���

	CLIENT_COPYMAP_LeaveRequest, // �뿪����
	CLIENT_COPYMAP_LeaveResponse,

	WORLD_COPYMAP_PlayerDataResponse,

	MSG_COPYMAP_END,

	//������Ϣ----------------------------------

	MSG_CHAT_BEGIN,
	//�ͻ���������Ϣ----------------------------
	CLIENT_CHAT_SendMessageRequest, // �ͻ��˷���������Ϣ
	CLIENT_CHAT_SendMessageResponse,
	CLIENT_CHAT_ChangeMessageTypeStatus,

	//�����������Ϣ----------------------------
	SERVER_CHAT_SendMessage,	// ����˷���������Ϣ
	SERVER_CHAT_SendMessageAll, // �������ӵ������û�����Ϣ

	MSG_CHAT_END,

	//�ʼ������Ϣ----------------------------------
	MSG_MAIL_BEGIN,

	ZONE_WORLD_MailListRequest, // �����ʼ��б�
	ZONE_WORLD_MailListResponse,

	ZONE_WORLD_SendMailRequest, // �����ʼ�
	ZONE_WORLD_SendMailResponse,

	ZONE_WORLD_DeleteMailRequest, // ɾ���ʼ�

	ZONE_WORLD_MailReaded, // ����Ѷ�ȡ�ʼ�

	ZONE_WORLD_MailTryGetItems,
	ZONE_WORLD_MailGetItems, // �����ȡ����

	ZONE_WORLD_DeleteAllMail, // �������ʼ�

	ZONE_WORLD_MailTryGetMoney,
	ZONE_WORLD_MailGetMoney, // �����ȡ��Ǯ

	WORLD_CLIENT_MailNotify, // ֪ͨ������ʼ�

	MSG_MAIL_END,

	// ��־�����Ϣ----------------------------------
	MSG_LOG_BEGIN,

	ANY_LOGSERVER_Record,

	MSG_LOG_END,

	// ȫ���¼����
	MSG_EVENT_BEGIN,

	ZONE_WORLD_EventNotify,

	CLIENT_WORLD_EventRequest,
	CLIENT_WORLD_EventResponse,

	MSG_EVENT_END,

	// ����ϵ�����Ϣ-------------------------------
	MSG_SOCIAL_BEGIN,

	ZONE_WORLD_PlayerStatusChangeRequest, // ��ͼ֪ͨ�ı���ҵ�ǰ״̬

	WORLD_CLIENT_PlayerStatusChange, // ���״̬�ı�

	CLIENT_WORLD_SocialInfoRequest, // ��ȡ����ϵ�б�
	CLIENT_WORLD_SocialInfoResponse,

	WORLD_CLIENT_SocialAdded, // ��Һ�������

	WORLD_CLIENT_SocialRemoved, // ��Һ���ɾ��

	WORLD_CLIENT_SocialNoAddedRequest, // ��ҽ�ֹ����Ϊ����

	WORLD_CLIENT_SocialMake, // �Ӻ���

	WORLD_CLIENT_SocialDestory, // ɾ����

	CLIENT_WORLD_FindPlayerRequest, // �������
	CLIENT_WORLD_FindPlayerResponse,

	CLIENT_WORLD_QueryFriendRequest, // ���Һ���
	CLIENT_WORLD_QueryFriendResonse,

	CLIENT_WORLD_QueryPlayerHotRequest, // ��ѯ��Ե
	CLIENT_WORLD_QueryPlayerHotResponse,

	CLIENT_WORLD_QueryChearCountRequest, // ��ѯʣ�������
	CLIENT_WORLD_QueryChearCountResponse,

	ZONE_WORLD_EncourageNotify, // ��Ҳ���
	WORLD_CLIENT_EncourageNotify,

	ZONE_WORLD_LotRequest, // ������Ե��
	ZONE_WORLD_LotResponse,

	ZONE_WORLD_LotNotify, // ������Ե��֪ͨ

	ZONE_WORLD_AddToLotRequest, // ���뵽������Ե��

	ZONE_WORLD_RemoveFromLotRequest, // ��������Ե�����޳�

	ZONE_WORLD_LotLockOver, // ����ѡ��״̬����

	CLIENT_WORLD_MakeLinkRequest, // Clinet����
	CLIENT_WORLD_MakeLinkResponse,

	CLIENT_WORLD_DestoryLinkRequest, // Client�Ͻ�
	CLIENT_WORLD_DestoryLinkResponse,

	ZONE_WORLD_MakeLinkRequest, // Zone����
	ZONE_WORLD_MakeLinkResponse,

	ZONE_WORLD_DestoryLinkRequest, // Zone�Ͻ�
	ZONE_WORLD_DestoryLinkResponse,

	ZONE_WORLD_ChangeLinkRequest, // Zone�ı��������ϵ
	ZONE_WORLD_ChangeLinkResponse,

	WORLD_ZONE_SocialInfo, // ֪ͨZone�������ϵ

	ZONE_WORLD_AddFriendValue, // Zone�޸�����Ѻö�

	ZONE_WORLD_QueryFriendValueRequest, // ��ѯ���Ѷ�
	ZONE_WORLD_QueryFriendValueResponse,

	CLIENT_WORLD_QueryPlayerRequest, // �ͻ��˲鿴�����Ϣ
	CLIENT_WORLD_QueryPlayerResponse,

	CLIENT_WORLD_ChearRequest, // ����
	CLIENT_WORLD_ChearResponse,

	WORLD_CLIENT_ChearNotify, // ����֪ͨ
	WORLD_ZONE_ChearNotify,

	CLIENT_WORLD_PlayerInfoRequest, // ������������������
	CLIENT_WORLD_PlayerInfoResponse,

	MSG_SOCIAL_END,

	//GM��Ϣ
	GM_MESSAGE,
	GM_MESSAGE_RESP,

	// ���а������Ϣ
	MSG_TOP_BEGIN,

	Client_WORLD_RequestRankSeriesInfo, // ���а�����

	Client_WORLD_RequestSnooty, // ����
	Client_WORLD_RequestRevere, // ����

	WORLD_ZONE_Snooty,
	WORLD_ZONE_Revere,

	ZONE_WORLD_UpdateTop,

	MSG_TOP_END,

	END_NET_MESSAGE,
};

_inline bool IsValidMessage(int Msg) { return Msg > BEGIN_NET_MESSAGE && Msg < END_NET_MESSAGE; }

enum ERROR_CODE
{
	NONE_ERROR,
	VERSION_ERROR,		   //�汾�Ŵ���
	LOGIN_SERVER_INVALIDE, //ѡ��ķ�����������
	GAME_UID_ERROR,		   //�Ự����
	PLAYER_ERROR,		   //��¼�Ľ�ɫ������
	PLAYER_FULL,		   //��ɫ����

	ACCOUNT_ID_ERROR,  //��¼�ʺŴ���
	ACCOUNT_PWD_ERROR, //��¼���� ����
	ACCOUNT_UNACTIVE,  //�ʺ�δ����
	ACCOUNT_DENY,	  //�ʺ�ͣ��
	ACCOUNT_NO_POINT,  //��������

	COMPRESS_ERROR, //ѹ�����ݴ���
	DB_ERROR,		//���ݿ��������

	DB_VERSION_ERROR, //���ݿ�汾����

	TEAM_SWITCH_DISABLE,   //�ر���ӿ���
	TEAM_PLAYER_ERROR,	 //���ʱ���ID����
	TEAM_SELF_ERROR,	   //���ܶ��Լ����
	TEAM_SELF_HAVE_TEAM,   //���Ѿ��ж�����
	TEAM_OTHER_HAVE_TEAM,  //�Է��Ѿ��ж�����
	TEAM_NOTIFY_LIST_FULL, //������Ϣ�б���
	TEAM_ERROR,			   //δ֪����
	TEAM_TEAMMATE_FULL,	//��������

	ERROR_BAN,	//����ֹ
	ERROR_FREEZE, //������

	UNKNOW_ERROR, //δ֪����
};

enum ePlayerDataTransferType
{
	PLAYER_TRANS_TYPE_BASE = 0,
	PLAYER_TRANS_TYPE_NORMAL,
	PLAYER_TRANS_TYPE_ADV,

	PLAYER_TRANS_TYPE_END,
};

#endif