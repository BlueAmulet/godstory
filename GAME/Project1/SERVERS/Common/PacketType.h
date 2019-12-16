#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

#define ITOA(NUMBER) #NUMBER
#define __TEXT_LINE__(LINE) ITOA(LINE)
#ifdef _DEBUG
#define HACK(MSG) __FILE__ "("__TEXT_LINE__(__LINE__) ") : ★HACK★ → " #MSG
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
		LogStream << "Error Message(" << #TAG << "):非自定义异常" << ends;    \
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

const unsigned int PLAYERSTRUCT_VERSION = 20090925; // 玩家数据结构版本

const unsigned int MISSION_FLAG_MAX = 40;   // 每任务最多40个任务旗标
const unsigned int MISSION_ACCEPT_MAX = 20; // 每玩家最多20个已接任务
const unsigned int MISSION_CYCLE_MAX = 200; // 每玩家最多接200个循环任务
const unsigned int MISSION_MAX = 4096 << 3; // 游戏任务最多32768
const unsigned int MISSION_STATE_LENGTH = MISSION_MAX >> 3;
const unsigned int MISSION_FLAG_BITS = MISSION_FLAG_MAX * sizeof(int) * 2;
const unsigned int MAX_EMBEDSLOTS = 8;		  // 镶嵌宝石最多个数
const unsigned int MAX_IDEPROS = 12;		  // 物品鉴定后附加属性最多个数
const unsigned int MAX_EQUIPSTENGTHENS = 3;   // 装备强化等级最多效果数
const unsigned int INVENTROY_MAXSLOTS = 120;  // 物品栏最大槽位数
const unsigned int PANEL_MAXSLOTS = 12 * 4;   // 快捷栏最大槽位数
const unsigned int BANK_MAXSLOTS = 120;		  // 银行栏最大槽位数
const unsigned int EQUIP_MAXSLOTS = 26;		  // 装备栏最大槽位数
const unsigned int SKILL_MAXSLOTS = 200;	  // 技能栏最大槽位数
const unsigned int BUFF_MAXSLOTS = 32;		  // 最大保存buff/debuff数量
const unsigned int COOLDOWN_MAX = 250;		  // 冷却时间最大数量
const unsigned int PET_MAXSLOTS = 8;		  // 宠物栏最大槽位数
const unsigned int MOUNTPET_MAXSLOTS = 8;	 // 骑宠栏最大槽位数
const unsigned int SPIRIT_MAXSLOTS = 5;		  // 元神栏最大槽位数
const unsigned int PRESCRIPTION_MAX = 1000;   // 配方最大个数
const unsigned int MAX_PLAYERDATAFLAGS = 128; // 策划用于记录玩家某些数据的旗标
const unsigned int MAX_TIMER_SIZE = 4;		  // 时间触发器数据最多个数
const unsigned int LIVINGSKILL_MAXSLOTS = 90; // 生活技能栏最大槽位数
const unsigned int ITEMSPLIT_MAXSLOTS = 8;	// 拆分栏最大槽位数

const unsigned int SOCAIL_ITEM_MAX = 300; // 社会关系数量最大值

const unsigned int MPM_CALLBOARD_MAXSTRING = 128; // 师傅公告牌最大字符数

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

	//内部通讯消息---------------------------
	//系统消息
	COMMON_RegisterRequest, //服务器之间的注册消息
	COMMON_RegisterResponse,

	GATE_WORLD_ZoneRegisterRequest, //网关告诉世界服务器自己管理那几个地图的通讯

	ACCOUNT_WORLD_ClientLoginRequest, //帐号服务器转发登录消息到世界服务器
	ACCOUNT_WORLD_ClientLoginResponse,

	GATE_WORLD_ClientLoginRequest, //网关服务器转发登录消息到世界服务器
	GATE_WORLD_ClientLoginResponse,

	WORLD_ZONE_PlayerData,
	WORLD_ZONE_PlayerDataResponse,

	CLIENT_WORLD_ItemRequest, // 玩家向其他玩家索取物品

	WORLD_COPYMAP_PlayerData, //发送数据到Zone

	WORLD_CLIENT_NOTIFY_KICKED, //通知被踢线

	CLIENT_WORLD_IdCodeRequest, // 验证

	ZONE_WORLD_AddPoint, // 消费仙石

	ZONE_WORLD_PlayerLogin, // 玩家确实登陆到ZONE

	// DataAgent相关消息
	//------------------------------------------------------------------------
	MSG_DATAAGENT_BEGIN,

	DATAAGENT_WORLD_NotifyPlayerDataAck, // WorldServer通知DataAgent數據收到
	DATAAGENT_WORLD_NotifyPlayerData,	// DataAgent向WorldServer通知玩家數據

	SERVER_DATAAGENT_Message, // 用于向DataAgent传输重要数据

	SERVER_DATAAGENT_PlayerDataRequest, // WorldServer主动向DataAgent查询数据
	SERVER_DATAAGENT_PlayerDataResponse,

	ZONE_WORLD_PlayerSaveRequest, // Zone保存玩家数据
	ZONE_WORLD_PlayerSaveResponse,

	MSG_DATAAGENT_END,
	//-------------------------------------------------------------------------

	GATE_WORLD_ClientLost, // 客户端断开事件通知
	GATE_WORLD_ZoneLost,   // 地图断开事件通知

	ZONE_WORLD_PlayerDispChanged, // 地图主动通知玩家显示数据变更

	WORLD_ZONE_TickPlayerRequest, // 通知ZoneServer踢掉玩家
	WORLD_ZONE_TickPlayerResponse,
	WORLD_ZONE_TransportPlayerRequest, //word通知ZONE传送玩家
	WORLD_ZONE_GetPlayerPosRequest,	//从ZONE获取玩家当前所在的位置
	WORLD_ZONE_GetPlayerPosResponse,

	//游戏消息
	ZONE_WORLD_SwitchMapRequest, //客户端请求切换地图(可以跨线)
	ZONE_WORLD_SwitchMapResponse,

	//客户通讯消息---------------------------
	CLIENT_ACCOUNT_LoginRequest, //客户端请求帐号服务器验证帐号
	CLIENT_ACCOUNT_LoginResponse,
	CLIENT_ACCOUNT_CheckRequest, // 客户端验证帐号
	CLIENT_ACCOUNT_CheckResponse,
	CLIENT_GATE_LoginRequest, //客户端登录网关服务器
	CLIENT_GATE_LoginResponse,
	CLIENT_GAME_LoginRequest, //客户端准备通过网关登录游戏
	CLIENT_GAME_LoginResponse,
	CLIENT_GAME_LogoutRequest, //客户端断开地图服务器，主动退出游戏
	CLIENT_GAME_LogoutResponse,

	CLIENT_WORLD_SelectPlayerRequest, //客户端查询角色
	CLIENT_WORLD_SelectPlayerResponse,
	CLIENT_WORLD_DeletePlayerRequest, //客户端删除角色
	CLIENT_WORLD_DeletePlayerResponse,
	CLIENT_WORLD_CreatePlayerRequest, //客户端创建角色
	CLIENT_WORLD_CreatePlayerResponse,

	CLIENT_WORLD_PlayerDataTransRequest, //传送角色信息
	CLIENT_WORLD_PlayerDataTransResponse,

	WORLD_ZONE_SavePlayerRequest, // 主动请求保存玩家数据
	WORLD_ZONE_SavePlayerResponse,

	// 队伍消息--------------------------------
	MSG_TEAM_BEGIN,

	CLIENT_TEAM_Disband,			// 解散队伍
	CLIENT_TEAM_BuildRequest,		// 组队请求
	CLIENT_TEAM_AcceptBuildRequest, // 接受组队请求

	CLIENT_TEAM_BuildAlongRequest, // 单人组队请求

	CLIENT_TEAM_JoinRequest,		// 加入申请
	CLIENT_TEAM_AcceptdJoinRequest, // 同意加入

	CLIENT_TEAM_LeaveRequest, // 离开队伍

	CLIENT_TEAM_DropRequest, // 踢人

	CLIENT_TEAM_AddRequest,		  // 加人
	CLIENT_TEAM_AcceptAddRequest, // 同意加人

	CLIENT_TEAM_PlayerJoined, // 成员加入
	CLIENT_TEAM_PlayerLeave,  // 成员退出
	CLIENT_TEAM_TeamInfo,	 // 队伍信息
	CLIENT_TEAM_ShowTeamInfo, // 查看队伍信息

	CLIENT_TEAM_Error, // 错误消息

	CLIENT_TEAM_Refused, // 拒绝信息
	ClIENT_TEAM_Cancel,  // 取消邀请

	CLIENT_TEAM_MapMark,	// 地图标记
	CLIENT_TEAM_TargetMark, // 目标标记

	CLIENT_TEAM_BaseInfo, // 队伍基本信息,如队伍名称,经验分配,物品分配等

	CLIENT_TEAM_Follow, // 发起组队跟随

	CLIENT_TEAM_InvationCopymap, // 邀请进副本

	MSG_TEAM_END,

	//副本相关消息------------------------------------
	MSG_COPYMAP_BEGIN,

	CLIENT_COPYMAP_EnterRequest, // 进入副本
	CLIENT_COPYMAP_EnterResponse,

	CLIENT_COPYMAP_OpenRequest, // 开启副本
	CLIENT_COPYMAP_OpenResponse,

	CLIENT_COPYMAP_CloseRequest, // 关闭副本

	CLIENT_COPYMAP_LeaveRequest, // 离开副本
	CLIENT_COPYMAP_LeaveResponse,

	WORLD_COPYMAP_PlayerDataResponse,

	MSG_COPYMAP_END,

	//聊天消息----------------------------------

	MSG_CHAT_BEGIN,
	//客户端聊天消息----------------------------
	CLIENT_CHAT_SendMessageRequest, // 客户端发送聊天消息
	CLIENT_CHAT_SendMessageResponse,
	CLIENT_CHAT_ChangeMessageTypeStatus,

	//服务端聊天消息----------------------------
	SERVER_CHAT_SendMessage,	// 服务端发送聊天消息
	SERVER_CHAT_SendMessageAll, // 向所连接的所有用户发消息

	MSG_CHAT_END,

	//邮件相关消息----------------------------------
	MSG_MAIL_BEGIN,

	ZONE_WORLD_MailListRequest, // 请求邮件列表
	ZONE_WORLD_MailListResponse,

	ZONE_WORLD_SendMailRequest, // 发送邮件
	ZONE_WORLD_SendMailResponse,

	ZONE_WORLD_DeleteMailRequest, // 删除邮件

	ZONE_WORLD_MailReaded, // 玩家已读取邮件

	ZONE_WORLD_MailTryGetItems,
	ZONE_WORLD_MailGetItems, // 玩家收取包裹

	ZONE_WORLD_DeleteAllMail, // 玩家清空邮件

	ZONE_WORLD_MailTryGetMoney,
	ZONE_WORLD_MailGetMoney, // 玩家收取金钱

	WORLD_CLIENT_MailNotify, // 通知玩家收邮件

	MSG_MAIL_END,

	// 日志相关消息----------------------------------
	MSG_LOG_BEGIN,

	ANY_LOGSERVER_Record,

	MSG_LOG_END,

	// 全局事件相关
	MSG_EVENT_BEGIN,

	ZONE_WORLD_EventNotify,

	CLIENT_WORLD_EventRequest,
	CLIENT_WORLD_EventResponse,

	MSG_EVENT_END,

	// 社会关系相关消息-------------------------------
	MSG_SOCIAL_BEGIN,

	ZONE_WORLD_PlayerStatusChangeRequest, // 地图通知改变玩家当前状态

	WORLD_CLIENT_PlayerStatusChange, // 玩家状态改变

	CLIENT_WORLD_SocialInfoRequest, // 获取社会关系列表
	CLIENT_WORLD_SocialInfoResponse,

	WORLD_CLIENT_SocialAdded, // 玩家好友增加

	WORLD_CLIENT_SocialRemoved, // 玩家好友删除

	WORLD_CLIENT_SocialNoAddedRequest, // 玩家禁止被加为好友

	WORLD_CLIENT_SocialMake, // 加好友

	WORLD_CLIENT_SocialDestory, // 删好友

	CLIENT_WORLD_FindPlayerRequest, // 查找玩家
	CLIENT_WORLD_FindPlayerResponse,

	CLIENT_WORLD_QueryFriendRequest, // 查找好友
	CLIENT_WORLD_QueryFriendResonse,

	CLIENT_WORLD_QueryPlayerHotRequest, // 查询仙缘
	CLIENT_WORLD_QueryPlayerHotResponse,

	CLIENT_WORLD_QueryChearCountRequest, // 查询剩余加油数
	CLIENT_WORLD_QueryChearCountResponse,

	ZONE_WORLD_EncourageNotify, // 玩家卜卦
	WORLD_CLIENT_EncourageNotify,

	ZONE_WORLD_LotRequest, // 命运有缘人
	ZONE_WORLD_LotResponse,

	ZONE_WORLD_LotNotify, // 命运有缘人通知

	ZONE_WORLD_AddToLotRequest, // 加入到命运有缘人

	ZONE_WORLD_RemoveFromLotRequest, // 从命运有缘人中剔除

	ZONE_WORLD_LotLockOver, // 命运选定状态结束

	CLIENT_WORLD_MakeLinkRequest, // Clinet建交
	CLIENT_WORLD_MakeLinkResponse,

	CLIENT_WORLD_DestoryLinkRequest, // Client断交
	CLIENT_WORLD_DestoryLinkResponse,

	ZONE_WORLD_MakeLinkRequest, // Zone建交
	ZONE_WORLD_MakeLinkResponse,

	ZONE_WORLD_DestoryLinkRequest, // Zone断交
	ZONE_WORLD_DestoryLinkResponse,

	ZONE_WORLD_ChangeLinkRequest, // Zone改变玩家社会关系
	ZONE_WORLD_ChangeLinkResponse,

	WORLD_ZONE_SocialInfo, // 通知Zone玩家社会关系

	ZONE_WORLD_AddFriendValue, // Zone修改玩家友好度

	ZONE_WORLD_QueryFriendValueRequest, // 查询好友度
	ZONE_WORLD_QueryFriendValueResponse,

	CLIENT_WORLD_QueryPlayerRequest, // 客户端查看玩家信息
	CLIENT_WORLD_QueryPlayerResponse,

	CLIENT_WORLD_ChearRequest, // 加油
	CLIENT_WORLD_ChearResponse,

	WORLD_CLIENT_ChearNotify, // 加油通知
	WORLD_ZONE_ChearNotify,

	CLIENT_WORLD_PlayerInfoRequest, // 玩家请求其他玩家数据
	CLIENT_WORLD_PlayerInfoResponse,

	MSG_SOCIAL_END,

	//GM消息
	GM_MESSAGE,
	GM_MESSAGE_RESP,

	// 排行榜相关消息
	MSG_TOP_BEGIN,

	Client_WORLD_RequestRankSeriesInfo, // 排行榜请求

	Client_WORLD_RequestSnooty, // 鄙视
	Client_WORLD_RequestRevere, // 敬仰

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
	VERSION_ERROR,		   //版本号错误
	LOGIN_SERVER_INVALIDE, //选择的服务器不可用
	GAME_UID_ERROR,		   //会话过期
	PLAYER_ERROR,		   //登录的角色有问题
	PLAYER_FULL,		   //角色建满

	ACCOUNT_ID_ERROR,  //登录帐号错误
	ACCOUNT_PWD_ERROR, //登录密码 错误
	ACCOUNT_UNACTIVE,  //帐号未激活
	ACCOUNT_DENY,	  //帐号停用
	ACCOUNT_NO_POINT,  //点数不够

	COMPRESS_ERROR, //压缩数据错误
	DB_ERROR,		//数据库操作错误

	DB_VERSION_ERROR, //数据库版本错误

	TEAM_SWITCH_DISABLE,   //关闭组队开关
	TEAM_PLAYER_ERROR,	 //组队时玩家ID错误
	TEAM_SELF_ERROR,	   //不能对自己组队
	TEAM_SELF_HAVE_TEAM,   //你已经有队伍了
	TEAM_OTHER_HAVE_TEAM,  //对方已经有队伍了
	TEAM_NOTIFY_LIST_FULL, //队伍消息列表满
	TEAM_ERROR,			   //未知错误
	TEAM_TEAMMATE_FULL,	//队伍已满

	ERROR_BAN,	//被禁止
	ERROR_FREEZE, //被冻结

	UNKNOW_ERROR, //未知错误
};

enum ePlayerDataTransferType
{
	PLAYER_TRANS_TYPE_BASE = 0,
	PLAYER_TRANS_TYPE_NORMAL,
	PLAYER_TRANS_TYPE_ADV,

	PLAYER_TRANS_TYPE_END,
};

#endif