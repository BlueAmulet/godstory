#define BASE_BITSTREAM
#include "GamePlay/ClientGamePlayState.h"
#include "base/md5.h"
#include "zlib/zlib.h"
#include "core/frameAllocator.h"
#include "Common/PlayerStruct.h"
#include "Common/PacketType.h"
#include "Common/CommonClient.h"
#include "UserPacketProcess.h"
#include "Common/ChatBase.h"
#include "Gameplay/Team/ClientTeam.h"
#include "Gameplay/ClientPlayerManager.h"
#include "UI/dGuiObjectView.h"
#include "Common/TeamBase.h"

#include "NetWork/EventCommon.h"
#include "Gameplay/Social/ClientSocial.h"
#include "Gameplay/Chat/ClientChat.h"
#include "Gameplay/Rank/ClientRank.h"


using namespace std;

ClientGameplayState::stMsgParam g_errorMsgParam;

struct errorGen
{
	errorGen()
	{
		g_errorMsgParam.fontcolor = 0xff0000;
		g_errorMsgParam.fontsize = 20;
		g_errorMsgParam.fonttype = 3;
		g_errorMsgParam.limit = 0;
		g_errorMsgParam.time = 10;
		g_errorMsgParam.posx = 0;
		g_errorMsgParam.posy = 0;
		g_errorMsgParam.mode = 0;
	}
};

errorGen eg;

void UserPacketProcess::HandleClientLogin()
{
	char Status = m_pClientGameplayState->getLoginStatus();
	switch(Status)
	{
	case SERVICE_ACCOUNTSERVER:
		{
			if( true )
			{
				const char *pPWD = m_pClientGameplayState->getAccountPWD();
				Send_Account_LoginRequest(m_pClientGameplayState->getAccountName(),GetMD5(pPWD,dStrlen(pPWD)));
			}
			else
			{
				const char *pPWD = m_pClientGameplayState->getAccountPWD();
				Send_Account_CheckRequest(m_pClientGameplayState->getAccountName(),GetMD5(pPWD,dStrlen(pPWD)));
			}

			Con::printf("连接过程：连接至帐号服务器，登录中...");
		}
		break;
	case SERVICE_GATESERVER:
		{
			int AccountId	= m_pClientGameplayState->getAccountId();
			T_UID UID		= m_pClientGameplayState->getAccountUID();
			Send_Gate_LoginRequest(UID,AccountId);
			Con::printf( "连接过程：连接至网关，登录中..." );
		}
		break;
	case SERVICE_GAMESERVER:
		{
			int AccountId	= m_pClientGameplayState->getAccountId();
			int PlayerId	= m_pClientGameplayState->getSelectedPlayerId();
			T_UID UID		= m_pClientGameplayState->getAccountUID();
			char Status		= m_pClientGameplayState->getLoginStatus();
			Send_Game_LoginRequest(UID,AccountId,PlayerId,Status);
		}
		break;
	}
}

void UserPacketProcess::HandleClientLogout()
{
	//连接断开了，是否重新连接
	char Status = m_pClientGameplayState->getLoginStatus();
	switch(Status)
	{
	case SERVICE_ACCOUNTSERVER:
		//帐号服务器断开连接了，这个时候应该停留在登录界面
		{
			Con::executef("BackToMainMenu");
		}
		break;
	case SERVICE_GATESERVER:
		//网管服务器断开了，这个时候如果是在角色选择界面就返回到帐号登录界面

		{
			//m_pClientGameplayState->AddScreenMessage( "和网关服务器断开连接" );

			Con::executef("BackToMainMenu");
		}
		break;
	}
}

bool UserPacketProcess::HandleGamePacket(stPacketHead *pHead,int iSize)
{
	char *pData	= (char *)(pHead) + GetHeadSize();
	Base::BitStream RecvPacket(pData,iSize-GetHeadSize());

	switch(pHead->Message)
	{
	case CLIENT_ACCOUNT_LoginResponse:
		Handle_Account_LoginResponse(RecvPacket);
		return true;
	case CLIENT_ACCOUNT_CheckResponse:
		Handle_Account_CheckResponse(RecvPacket);
		return true;
	case CLIENT_GATE_LoginResponse:
		Handle_Gate_LoginResponse(RecvPacket);
		return true;
	case CLIENT_GAME_LoginResponse:
		Handle_Game_LoginResponse(RecvPacket);
		return true;
	case CLIENT_GAME_LogoutResponse:
		Handle_Game_LogoutResponse(pHead,RecvPacket);
		return true;
	case CLIENT_WORLD_SelectPlayerResponse:
		Handle_World_SelectPlayerResponse(pHead,RecvPacket);
		return true;
	case CLIENT_WORLD_DeletePlayerResponse:
		Handle_World_DeletePlayerResponse(pHead,RecvPacket);
		return true;
	case CLIENT_WORLD_CreatePlayerResponse:
		Handle_World_CreatePlayerResponse(pHead,RecvPacket);
		return true;

	case CLIENT_WORLD_ItemRequest:
		Handle_Client_ItemRequest( pHead, RecvPacket );
		return true;

		// 处理角色传输消息
	case CLIENT_WORLD_PlayerDataTransRequest:
		Handle_World_Client_PlayerDataTransResopnse( pHead, RecvPacket );
		return true;

	// 处理服务端消息
	case SERVER_CHAT_SendMessage:
		Handle_Server_SendMessage( pHead, RecvPacket );
		return true;
	case CLIENT_CHAT_SendMessageResponse:
		Handle_Chat_SendMessageResponse( pHead, RecvPacket );
		return true;

	// 处理组队相关
	case CLIENT_TEAM_BuildRequest:
		Handle_Client_Team_BuildRequest( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_InvationCopymap:
		Handle_Client_Team_InvationCopymap( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_JoinRequest:
		Handle_Client_Team_JoinRequest( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_AddRequest:
		Handle_Client_Team_AddRequest( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_PlayerJoined:
		Handle_Client_Team_PlayerJoined( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_PlayerLeave:
		Handle_Client_Team_PlayerLeave( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_Error:
		Handle_Client_Team_Error( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_TeamInfo:
		Handle_Client_Team_Info( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_MapMark:
		Handle_Client_Team_MapMark( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_TargetMark:
		Handle_Client_Team_TargetMark( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_Refused:
		Handle_Client_Team_Refused( pHead, RecvPacket );
		return true;
	case ClIENT_TEAM_Cancel:
		Handle_Client_Team_Cancel( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_BaseInfo:
		Handle_Client_Team_BaseInfo( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_ShowTeamInfo:
		Handle_Client_Team_ShowInfo( pHead, RecvPacket );
		return true;
	case CLIENT_TEAM_Follow:
		Hanlde_Client_Team_Follow( pHead, RecvPacket );
		return true;

	// mail notify
	case WORLD_CLIENT_MailNotify:
		Hanlde_World_Client_MailNotify( pHead, RecvPacket );
		return true;

		// copy map related
	case CLIENT_COPYMAP_EnterResponse:
		Handle_Client_CopyMap_EnterResponse( pHead, RecvPacket );
		return true;
	case CLIENT_COPYMAP_OpenResponse:
		Handle_Client_CopyMap_OpenResponse( pHead, RecvPacket );
		return true;
	case CLIENT_COPYMAP_LeaveResponse:
		Handle_Client_CopyMap_LeaveResponse( pHead, RecvPacket );
		return true;
    case GM_MESSAGE_RESP:
        HandleGMCommandResp(pHead,RecvPacket);
        return true;
	case WORLD_CLIENT_NOTIFY_KICKED:
		HandleTick(pHead,RecvPacket);
		return true;
	}


	if( pHead->Message > MSG_SOCIAL_BEGIN && pHead->Message < MSG_SOCIAL_END )
		gEventMethod[pHead->Message]( &g_clientSocial, pHead, RecvPacket );

	if( pHead->Message > MSG_TOP_BEGIN && pHead->Message < MSG_TOP_END )
		gEventMethod[pHead->Message]( &g_clientRank, pHead, RecvPacket );

	return false;
}

void UserPacketProcess::SendSimplePacket( int nPlayerId, char cMessageType, int nDestZoneId /* = 0 */, int nSrcZoneId /* = 0 */ )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream SendPacket( buf, MAX_PACKET_SIZE );

	stPacketHead* pHead = BuildPacketHead( SendPacket, cMessageType, nPlayerId, SERVICE_WORLDSERVER, nDestZoneId, nSrcZoneId );
	pHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	Send( SendPacket );
}
void UserPacketProcess::HandleTick(stPacketHead *pHead,Base::BitStream &RecvPacket)
{
	Con::executef("OnTick");
	Con::executef( "此帐号已在其他位置登陆，您已下线！" );
//	Con::evaluatef("CloseAllGui();ShowBackBitmap();disconnect();OpenSystemSettingWnd();OpenSystemMenuWnd();"); // 强制切换到人物选择界面
	g_ClientGameplayState->isTick = true;
}

void UserPacketProcess::Handle_Account_LoginResponse(Base::BitStream &RecvPacket)
{
	m_pClientGameplayState->m_ServerList.clear();

	bool choosed =false;
	ERROR_CODE error = (ERROR_CODE)RecvPacket.readInt(Base::Bit16);
	if(error == NONE_ERROR)
	{
		int AccountId= RecvPacket.readInt(Base::Bit32);
		m_pClientGameplayState->setAccountId(AccountId);
		T_UID UID = RecvPacket.readInt(UID_BITS);
		m_pClientGameplayState->setAccountUID(UID);

		int chooseGateNo=-1;
		int chooseLineId = Con::getIntVariable("$Pref::Net::Line");
		int chooseLineNo;

		stServerList serverList;
		stServerInfo serverInfo;
		int LineNum= RecvPacket.readInt(Base::Bit8);

		if(chooseLineId==0 && LineNum > 0)
			chooseLineNo = Platform::getRandomI(0,LineNum-1);

		if( LineNum == 0 )
		{
			dStrcpy( g_errorMsgParam.message, sizeof(g_errorMsgParam.message), "没有可选的游戏服务器");
			m_pClientGameplayState->OutputScreenMessage( g_errorMsgParam );
		}

		for(int i=0;i<LineNum;i++)
		{
			serverList.LineId = RecvPacket.readInt(Base::Bit8);
			serverList.GateList.clear();
			if(!choosed && chooseLineId==0 && i==chooseLineNo)
			{
				chooseLineId = serverList.LineId;
				Con::setIntVariable("$Pref::Net::Line",chooseLineId);
			}

			int GateNum = RecvPacket.readInt(Base::Bit8);
			
			if(chooseLineId == serverList.LineId)
			{
				chooseGateNo = Platform::getRandomI(0,GateNum-1);
			}

			for(int j=0;j<GateNum;j++)
			{
				serverInfo.GateId = RecvPacket.readInt(Base::Bit16);
				serverInfo.Ip	= RecvPacket.readInt(Base::Bit32);
				serverInfo.Port= RecvPacket.readInt(Base::Bit16);

				if(!choosed && chooseGateNo == j)
				{
					//获得上次玩家登录过的线随机一个网关登录，如果没有需要随机分配某个线的某个网关登录
					m_pClientGameplayState->setGateIP(serverInfo.Ip);					
					m_pClientGameplayState->setGatePort(serverInfo.Port);	
					m_pClientGameplayState->setCurrentLineId(chooseLineId);
					choosed = true;
				}

				serverList.GateList.push_back(serverInfo);
			}

			m_pClientGameplayState->m_ServerList.push_back(serverList);		//客户端线程服务器列表
		}

		if(!choosed)	//线路撤销？ 重新随机吧
		{
			if(m_pClientGameplayState->m_ServerList.size())
			{
				chooseLineNo = Platform::getRandomI(0,m_pClientGameplayState->m_ServerList.size()-1);
				m_pClientGameplayState->RandomGate(m_pClientGameplayState->m_ServerList[chooseLineNo].LineId);
			}
		}

		for( int i = 0; i < m_pClientGameplayState->m_ServerList.size(); i++ )
		{
			for( int j= 0; j < m_pClientGameplayState->m_ServerList[i].GateList.size(); j++ )
			{
				int IP = m_pClientGameplayState->m_ServerList[i].GateList[j].Ip;
				char sIP[COMMON_STRING_LENGTH];
				dSprintf(sIP,COMMON_STRING_LENGTH,"%s:%d",inet_ntoa(*(in_addr*)&IP),htons(m_pClientGameplayState->m_ServerList[i].GateList[j].Port));
				Con::printf( "[%d]-[%d]: %s", m_pClientGameplayState->m_ServerList[i].LineId, j, sIP );
			}
		}

#ifndef GM_CLIENT
		Con::executef("SptConnectGateServer");		//连接到网关服务器
		Con::printf("连接过程：等待网关响应...");
#else
        //貌似在GM客户端调用脚本存在些问题，先这样解决
        int IP = m_pClientGameplayState->getGateIP();
        char sIP[COMMON_STRING_LENGTH];
        dSprintf(sIP,COMMON_STRING_LENGTH,"%s:%d",inet_ntoa(*(in_addr*)&IP),htons(m_pClientGameplayState->getGatePort()));
        m_pClientGameplayState->ConnectServer(sIP,SERVICE_GATESERVER);
#endif
	}
	else
	{
		if( error == VERSION_ERROR )
			dStrcpy( g_errorMsgParam.message, sizeof(g_errorMsgParam.message), "登陆失败, 您的游戏客户端版本已过期，请更新新的客户端程序或使用修复程序修复！" );
		else if( error == DB_VERSION_ERROR )
			dStrcpy( g_errorMsgParam.message, sizeof(g_errorMsgParam.message), "登陆失败, 您的帐户角色已过期，请联系客服寻求解决方案！" );
		else
			dStrcpy( g_errorMsgParam.message, sizeof(g_errorMsgParam.message), "游戏登陆失败！" );

		m_pClientGameplayState->OutputScreenMessage( g_errorMsgParam );

		std::string str = g_errorMsgParam.message;
		char buf[128];
		sprintf_s( buf, 128, "%d", error );
		str += " 错误代码: ";
		str += buf;
		MessageBoxA( 0,  str.c_str(), "登陆失败", MB_OK | MB_ICONERROR );

		HACK("显示帐号验证失败的对话框");
		m_pClientGameplayState->DisconnectServer();
	}
}

void UserPacketProcess::Handle_World_SelectPlayerResponse(stPacketHead *pHead,Base::BitStream &RecvPacket)
{
	Con::printf( "连接过程：收到人物列表数据" );
	int AccountId = pHead->Id;
	T_UID UID = RecvPacket.readInt(UID_BITS);

	m_pClientGameplayState->m_PlayerList.clear();

	if(RecvPacket.readFlag())
	{
		int Size = RecvPacket.readInt(Base::Bit32);
		if(Size)
		{
			U32 waterMark = FrameAllocator::getWaterMark ();
			U32 maxSize = FrameAllocator::getHighWaterMark () - FrameAllocator::getWaterMark ();
			AssertFatal(maxSize>(Size+MAX_PACKET_SIZE*MAX_ACTOR_COUNT),"Handle_World_SelectPlayerResponse Buffer Full!");

			U8 *pDataBuf = (U8 *) FrameAllocator::alloc(Size);
			RecvPacket.readBits(Size*Base::Bit8,pDataBuf);

			unsigned long nLen = MAX_PACKET_SIZE * MAX_ACTOR_COUNT;
			U8 *pUnZipBuf = (U8 *) FrameAllocator::alloc(nLen);

			int iret = uncompress((unsigned char*)pUnZipBuf,&nLen,(const Bytef *)pDataBuf,Size);
			if (!iret)
			{
				Base::BitStream pack(pUnZipBuf,MAX_PACKET_SIZE * MAX_ACTOR_COUNT);
				stPlayerSelect tempData;
				int ActorCount = pack.readInt(Base::Bit8);
				for(int i=0;i<ActorCount;i++)
				{
					tempData.ReadData(&pack);

					//第一个玩家角色为默认选中角色
					if (i == 0)
					{
						m_pClientGameplayState->setSelectedPlayerId(tempData.BaseData.PlayerId);
						m_pClientGameplayState->setCurrentZoneId(tempData.DispData.ZoneId);
					}
					
					m_pClientGameplayState->m_PlayerList.push_back(tempData);		//玩家角色列表
				}
			}

			FrameAllocator::setWaterMark(waterMark);
		}
	}

	//显示服务器选择界面  [05/03/2009 soar]
	Con::executef("ShowServerSelectWnd");
}

void UserPacketProcess::Handle_World_CreatePlayerResponse(stPacketHead *pHead,Base::BitStream &RecvPacket)
{
	int AccountId = pHead->Id;
	T_UID UID = RecvPacket.readInt(UID_BITS);
	int Error = RecvPacket.readInt(Base::Bit16);
	if(Error==NONE_ERROR)
	{
		if(RecvPacket.readFlag())
		{
			int Size = RecvPacket.readInt(Base::Bit32);
			if(Size)
			{ 
				U32 waterMark = FrameAllocator::getWaterMark ();
				U32 maxSize = FrameAllocator::getHighWaterMark () - FrameAllocator::getWaterMark ();
				AssertFatal(maxSize>(Size+MAX_PACKET_SIZE),"Handle_World_CreatePlayerResponse Buffer Full!");

				U8 *pDataBuf = (U8 *) FrameAllocator::alloc(Size);
				RecvPacket.readBits(Size*Base::Bit8,pDataBuf);

				unsigned long nLen = MAX_PACKET_SIZE;
				U8 *pUnZipBuf = (U8 *) FrameAllocator::alloc(nLen);

				int iret = uncompress((unsigned char*)pUnZipBuf,&nLen,(const Bytef *)pDataBuf,Size);
				if (!iret)
				{
					Base::BitStream pack(pUnZipBuf,MAX_PACKET_SIZE);
					stPlayerSelect tempData;
					int ActorCount = pack.readInt(Base::Bit8);			//目前ActorCount必定为1
					for(int i=0;i<ActorCount;i++)
					{
						tempData.ReadData(&pack);

						m_pClientGameplayState->m_PlayerList.push_back(tempData);		//玩家角色列表
					}
				}

				FrameAllocator::setWaterMark(waterMark);

				if(!iret)
				{
					Con::executef("ShowPlayerList");
					Con::executef("SptSetDefaultPlayer");
					Con::executef("AutoConnect");
				}
			}	
		}
	}
	else
	{
		HACK("显示创建角色出错");
		dStrcpy( g_errorMsgParam.message, sizeof(g_errorMsgParam.message), "请选择其他的角色名称重新创建" );
		m_pClientGameplayState->OutputScreenMessage( g_errorMsgParam );
	}
}

void UserPacketProcess::Handle_World_DeletePlayerResponse(stPacketHead *pHead,Base::BitStream &RecvPacket)
{
	int AccountId = pHead->Id;
	T_UID UID = RecvPacket.readInt(UID_BITS);
	int PlayerId  = RecvPacket.readInt(Base::Bit32);
	int Error = RecvPacket.readInt(Base::Bit16);
	if(Error==NONE_ERROR)
	{
		dGuiObjectView* pObjView = dynamic_cast<dGuiObjectView*>(Sim::findObject("SelectObjectView"));
		pObjView->clear();
		std::vector<stPlayerSelect>::iterator itr = m_pClientGameplayState->m_PlayerList.begin();
		for (; itr != m_pClientGameplayState->m_PlayerList.end();itr++)
		{
			if ((*itr).BaseData.PlayerId == PlayerId)
			{	
				m_pClientGameplayState->m_PlayerList.erase(itr);
				break;
			}
		}
		
		Con::executef("ShowPlayerList");
		if (m_pClientGameplayState->m_PlayerList.size() != 0)
		{
			//Con::executef("SptSetDefaultPlayer");
		}
	}
	else
	{
		HACK("显示删除角色出错");

		dStrcpy( g_errorMsgParam.message, sizeof(g_errorMsgParam.message), "游戏过程中不能删除角色,请重新登陆后再继续删除" );
		m_pClientGameplayState->OutputScreenMessage( g_errorMsgParam );
	}
}

void UserPacketProcess::Handle_Gate_LoginResponse(Base::BitStream &RecvPacket)
{
	Con::printf("连接过程：网关已响应");
	int Error = RecvPacket.readInt(Base::Bit16);
	if(Error==NONE_ERROR)
	{
		int AccountId	= m_pClientGameplayState->getAccountId();
		T_UID UID		= m_pClientGameplayState->getAccountUID();

		if(!m_pClientGameplayState->isLineSwitching())
		{
			char Status = m_pClientGameplayState->getLoginStatus();
			if(Status == SERVICE_GATESERVER)
			{
				Con::printf( "连接过程：收到网关登陆响应，请求人物列表..." );
				Send_SelectPlayerRequest(UID,AccountId);
			}

#ifdef GM_CLIENT
            Con::executef("OnGateConnected");
#endif 
		}
		else
		{
			Con::executef("SptConnectGameServer");
		}
	}
	else
	{
		if( Error != GAME_UID_ERROR )
		{
			dStrcpy( g_errorMsgParam.message, sizeof(g_errorMsgParam.message), "登陆网关失败");
			m_pClientGameplayState->OutputScreenMessage( g_errorMsgParam );

			HACK("显示登录网关失败的对话框");
			m_pClientGameplayState->DisconnectServer();
		}
	}
}

void UserPacketProcess::Handle_Game_LogoutResponse(stPacketHead *pHead,Base::BitStream &RecvPacket)
{
	int AccountId	= pHead->Id;
	T_UID	UID		= RecvPacket.readInt(UID_BITS);
	int PlayerId	= RecvPacket.readInt(Base::Bit32);
	//服务器已经知道你断线了
	
}

void UserPacketProcess::Handle_Game_LoginResponse(Base::BitStream &RecvPacket)
{

	g_ClientGameplayState->Cleanup();

	int Error = RecvPacket.readInt(Base::Bit16);
	if(Error==NONE_ERROR)
	{
		if(RecvPacket.readFlag())
		{
			if(RecvPacket.readFlag())
			{
				int IP = RecvPacket.readInt(Base::Bit32);
				m_pClientGameplayState->setZoneIP(IP);
				int Port = RecvPacket.readInt(Base::Bit16);
				m_pClientGameplayState->setZonePort(Port);

				if(m_pClientGameplayState->isLineSwitching())
					Con::executef("disconnect");
				else
				{
					g_ClientGameplayState->clearResource();
				}

				//登录ZoneServer
				char sIP[COMMON_STRING_LENGTH];
				dSprintf(sIP,COMMON_STRING_LENGTH,"%s:%d",inet_ntoa(*(in_addr*)&IP),Port);
				Con::printf( "Connect to server: [%s]", sIP );
				Con::executef("connect",sIP, "1" );
				m_pClientGameplayState->setLoginStatus(SERVICE_GAMESERVER);


				//??
				for (int i=0;i<m_pClientGameplayState->m_PlayerList.size();i++)
				{
					if (m_pClientGameplayState->m_PlayerList[i].BaseData.PlayerId == m_pClientGameplayState->getSelectedPlayerId())
					{
						Con::executef("ShowPlayerInfoToGame",m_pClientGameplayState->m_PlayerList[i].BaseData.PlayerName);
						break;
					}
				}
				
			}
		}
	}
	else
	{
		Con::executef("ShowServerSelectWnd");
		dStrcpy( g_errorMsgParam.message, sizeof(g_errorMsgParam.message), "登陆游戏失败");
		m_pClientGameplayState->OutputScreenMessage( g_errorMsgParam );        
	}
}

bool UserPacketProcess::Send_Account_LoginRequest(const char *AccountName,const unsigned char *Password)
{
	char Buffer[128];
	Base::BitStream SendPacket(Buffer,128);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_ACCOUNT_LoginRequest);
	SendPacket.writeInt(END_NET_MESSAGE,Base::Bit16);
	SendPacket.writeString(AccountName);
	SendPacket.writeBits(MD5_STRING_LENGTH*Base::Bit8,Password);
	pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
	return Send(SendPacket);
}

bool UserPacketProcess::Send_Gate_LoginRequest(T_UID UID,int AccountId)
{
	Con::printf( "连接过程：发送网关连接请求！" );
	char Buffer[64];
	Base::BitStream SendPacket(Buffer,64);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_GATE_LoginRequest,AccountId);
	SendPacket.writeInt(NETWORK_PROTOCOL_VERSION,Base::Bit32);
	SendPacket.writeInt(UID						,UID_BITS);
	pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
	return Send(SendPacket);
}

bool UserPacketProcess::Send_Game_LoginRequest(T_UID UID,int AccountId,int PlayerId,char Status)
{
	char Buffer[64];
	Base::BitStream SendPacket(Buffer,64);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_GAME_LoginRequest,AccountId,SERVICE_WORLDSERVER);
	SendPacket.writeInt(UID						,UID_BITS);
	SendPacket.writeInt(PlayerId				,Base::Bit32);
	SendPacket.writeFlag(Status==SERVICE_GATESERVER);
	SendPacket.writeFlag(m_pClientGameplayState->isLineSwitching());
	pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
	return Send(SendPacket);
}

bool UserPacketProcess::Send_Game_LogoutRequest(T_UID UID,int AccountId,int PlayerId)
{
	char Buffer[64];
	Base::BitStream SendPacket(Buffer,64);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_GAME_LogoutRequest,AccountId,SERVICE_WORLDSERVER);
	SendPacket.writeInt(UID						,UID_BITS);
	SendPacket.writeInt(PlayerId				,Base::Bit32);
	pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
	return Send(SendPacket);
}

bool UserPacketProcess::Send_SelectPlayerRequest(T_UID UID,int AccountId)
{
	char Buffer[64];
	Base::BitStream SendPacket(Buffer,64);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_WORLD_SelectPlayerRequest,AccountId,SERVICE_WORLDSERVER);
	SendPacket.writeInt(UID						,UID_BITS);
	pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
	return Send(SendPacket);
}

bool UserPacketProcess::Send_DeletePlayerRequest(T_UID UID,int AccountId,int PlayerId)
{
	char Buffer[64];
	Base::BitStream SendPacket(Buffer,64);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_WORLD_DeletePlayerRequest,AccountId,SERVICE_WORLDSERVER);
	SendPacket.writeInt(UID						,UID_BITS);
	SendPacket.writeInt(PlayerId				,Base::Bit32);
	pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
	return Send(SendPacket);
}


bool UserPacketProcess::Send_CreatePlayerRequest(T_UID UID,int AccountId,const char *PlayerName,int Sex,int Body,int Face,int Hair,int HairCol,U32 ItemId)
{
	char Buffer[64];
	Base::BitStream SendPacket(Buffer,64);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_WORLD_CreatePlayerRequest,AccountId,SERVICE_WORLDSERVER);
	SendPacket.writeInt(UID				,UID_BITS);
	SendPacket.writeString(PlayerName);
	SendPacket.writeInt(Sex				,Base::Bit8);
	SendPacket.writeInt(Body			,Base::Bit16);
	SendPacket.writeInt(Face			,Base::Bit16);
	SendPacket.writeInt(Hair			,Base::Bit16);
	SendPacket.writeInt(HairCol			,Base::Bit8);
	SendPacket.writeInt(ItemId			,Base::Bit32);
	//SendPacket.writeInt(1301			,Base::Bit16);
	pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
	return Send(SendPacket);
}

// 处理发送消息响应
void UserPacketProcess::Handle_Chat_SendMessageResponse( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	char cType = (char)pHead->SrcZoneId;

	std::string str;

	switch( cType )
	{
	case SEND_CHAT_ACK_REFUSED:
		str = "<t c='0xff0000ff' o='0x2b2b2bff'>您被禁言了，请不要发送消息了！</t>";
		break;
	case SEND_CHAT_ACK_TOOFAST:
		str = "<t c='0xff0000ff' o='0x2b2b2bff'>您发送消息的速度太快了，慢一点吧。</t>";
		break;
	case SEND_CHAT_ACK_ERROR:
		str = "<t c='0xff0000ff' o='0x2b2b2bff'>消息发送错误！</t>";
		break;
	}

	HACK("处理发送消息的服务端响应");

	char type[10];
	dSprintf( type, sizeof(type), "0",cType );
	char sender[10] = {0};
	Con::executef( "AddChatMessage", type, str.c_str(), sender,sender );
}

void UserPacketProcess::Handle_Server_SendMessage( stPacketHead *pHead,Base::BitStream &RecvPacket )
{  
	stChatMessage chatMessage;
	stChatMessage::UnpackMessage( chatMessage, RecvPacket );
	std::string strChannel;
	switch( chatMessage.btMessageType )
	{
	case CHAT_MSG_TYPE_SYSTEM:
		strChannel = "系统";
		break;
	case CHAT_MSG_TYPE_WORLD:
		strChannel = "跨服";
		break;
	case CHAT_MSG_TYPE_WORLDPLUS:
		strChannel = "跑马";
		break;
	case CHAT_MSG_TYPE_RACE:
		strChannel = "门宗";
		break;
	case CHAT_MSG_TYPE_ORG:
		strChannel = "帮会";
		break;
	case CHAT_MSG_TYPE_SQUAD:
		strChannel = "团队";
		break;
	case CHAT_MSG_TYPE_TEAM:
		strChannel = "团队";
		break;
	case CHAT_MSG_TYPE_PRIVATE:
		strChannel = "密语";
		break;
	case CHAT_MSG_TYPE_REALCITY:
		strChannel = "同城";
		break;
	case CHAT_MSG_TYPE_NEARBY:
		strChannel = "附近";
		break;
	}
	Con::printf("SERVER MSG: 【%s】%s", strChannel.c_str(), chatMessage.szMessage );
	char type[10];
	char sender[10];
	dSprintf( type, sizeof(type), "%d", chatMessage.btMessageType);
	dSprintf( sender, sizeof(sender), "%d",chatMessage.nSender );

    bool BanorNot = 0;
    for (std::list<std::string>::iterator it = g_ClientGameplayState->m_BanNamelist.begin(); it != g_ClientGameplayState->m_BanNamelist.end(); ++it)
    {
        std::string temp = sender;
        if (temp == (*it))
        {
            BanorNot = 1;
            break;
        }
    }
    if (BanorNot)
        return;

	//这条消息是否属于仙友聊天对话框
	if (chatMessage.btMessageType ==CHAT_MSG_TYPE_P2P)
        g_ClientChatManager.onNewMsg(chatMessage.szMessage,chatMessage.nSender,chatMessage.szSenderName);
	else
	  Con::executef( "AddChatMessage", type, chatMessage.szMessage, sender, chatMessage.szSenderName );
	HACK("处理服务端消息");
}

bool UserPacketProcess::Send_Chat_ChangeType( int nPlayerId, int nType )
{
	char Buffer[MAX_PACKET_SIZE];
	Base::BitStream SendPacket(Buffer,MAX_PACKET_SIZE);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_CHAT_ChangeMessageTypeStatus, nPlayerId, SERVICE_WORLDSERVER );
	pSendHead->SrcZoneId = nType;
	pSendHead->PacketSize = SendPacket.getPosition() - sizeof( stPacketHead );

	return Send( SendPacket );
}

bool UserPacketProcess::Send_Chat_Message( int nPlayerId, char cMessageType, const char* pMessage, int nRecver )
{
	//  如果发送群消息，这里的nRecver则为群id
	//
	//

	char Buffer[MAX_PACKET_SIZE];
	Base::BitStream SendPacket(Buffer,MAX_PACKET_SIZE);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_CHAT_SendMessageRequest, nPlayerId, SERVICE_WORLDSERVER );
	stChatMessage chatMessage;
	chatMessage.btMessageType = cMessageType;
	chatMessage.nRecver = nRecver;
	dStrcpy( chatMessage.szMessage, sizeof(chatMessage.szMessage), pMessage );
	stChatMessage::PackMessage( chatMessage, SendPacket );
	pSendHead->PacketSize = SendPacket.getPosition() - GetHeadSize();

	return Send( SendPacket );

}

bool UserPacketProcess::Send_Chat_MessageByName( const char* playerName, const char* pMessage )
{
	char Buffer[MAX_PACKET_SIZE];
	Base::BitStream SendPacket(Buffer,MAX_PACKET_SIZE);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_CHAT_SendMessageRequest, g_ClientGameplayState->getSelectedPlayerId(), SERVICE_WORLDSERVER, 1 );
	stChatMessage chatMessage;
	chatMessage.btMessageType = CHAT_MSG_TYPE_PRIVATE;
	chatMessage.nRecver = 0;
	dStrcpy( chatMessage.szMessage, sizeof(chatMessage.szMessage), pMessage );
	stChatMessage::PackMessage( chatMessage, SendPacket );
	pSendHead->PacketSize = SendPacket.getPosition() - GetHeadSize();

	return Send( SendPacket );
}

bool UserPacketProcess::Send_Chat_MessageNearby(const char* pMessage)
{
	char Buffer[MAX_PACKET_SIZE];
	Base::BitStream SendPacket(Buffer,MAX_PACKET_SIZE);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_CHAT_SendMessageRequest, g_ClientGameplayState->getSelectedPlayerId(), SERVICE_WORLDSERVER, 2 );
	stChatMessage chatMessage;
	chatMessage.btMessageType = CHAT_MSG_TYPE_NEARBY;
	chatMessage.nRecver = 0;
	
	int count = PLAYER_MGR->m_localPlayerMap.size();

	SendPacket.writeInt( count, 16 );
	stdext::hash_map<int,Player*>::iterator it;
	for( it = PLAYER_MGR->m_localPlayerMap.begin(); it != PLAYER_MGR->m_localPlayerMap.end(); it++ )
	{
		SendPacket.writeInt( it->first, 32 );
	}

	dStrcpy( chatMessage.szMessage, sizeof(chatMessage.szMessage), pMessage );
	stChatMessage::PackMessage( chatMessage, SendPacket );
	pSendHead->PacketSize = SendPacket.getPosition() - GetHeadSize();

	return Send( SendPacket );

}



//////////////////////////////////////////////////////////////////////////

/*
	队伍相关代码(尚未测试)
*/

// 队长加人的请求
bool UserPacketProcess::Handle_Client_Team_AddRequest( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	PLAYER_MGR->HandleInfoPacket( &RecvPacket );
	
	Con::printf("玩家[%d]邀请您加入队伍", pHead->DestZoneId );
	char ID[10];
	char count[10];
	dSprintf( count, sizeof(count), "%d", pHead->SrcZoneId );
	dSprintf(ID, sizeof(ID), "%d", pHead->DestZoneId);
	Player* pPlayer = PLAYER_MGR->GetLocalPlayer( pHead->DestZoneId );
	char szName[COMMON_STRING_LENGTH] = { 0 };
	if( pPlayer )
		dStrcpy( szName, sizeof(szName), pPlayer->getPlayerName());
		
	if( CLIENT_TEAM->AddInvation( pHead->DestZoneId, 2 ) )
		Con::executef("RequestToTeam",ID, szName, "2", count );
	return false;
}

bool UserPacketProcess::Handle_Client_Team_InvationCopymap( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nPlayerId = pHead->SrcZoneId;

	int nCopymap = pHead->DestZoneId;

	//Con::printf( "玩家[%d]邀请您进入副本[%d]" );

	return true;
}

// 创建队伍请求
bool UserPacketProcess::Handle_Client_Team_BuildRequest( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	PLAYER_MGR->HandleInfoPacket( &RecvPacket );

	Con::printf("玩家[%d]请求一起组队", pHead->DestZoneId );
	char ID[10];
	dSprintf(ID, sizeof(ID), "%d", pHead->DestZoneId);
	Player* pPlayer = PLAYER_MGR->GetLocalPlayer( pHead->DestZoneId );
	char szName[COMMON_STRING_LENGTH] = { 0 };
	if( pPlayer )
		dStrcpy( szName, sizeof(szName), pPlayer->getPlayerName());

	if( CLIENT_TEAM->AddInvation( pHead->DestZoneId, 0 ) )
		Con::executef("RequestToTeam",ID, szName, "0","0" );
	return false;
}

// 有玩家加入队伍
bool UserPacketProcess::Handle_Client_Team_PlayerJoined( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	PLAYER_MGR->HandleInfoPacket( &RecvPacket );
	int nPlayerId = pHead->DestZoneId;
	int nTeamId = pHead->SrcZoneId;

	CLIENT_TEAM->OnAddPlayer( nPlayerId );

	Con::printf("玩家[%d]加入队伍", nPlayerId );
	char str[10];
	dSprintf( str, sizeof(str), "%d", nPlayerId );
	Con::executef("PlayerJoinTeam", str );

	return false;
}

// 有玩家离开队伍
bool UserPacketProcess::Handle_Client_Team_PlayerLeave( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nPlayerId = pHead->DestZoneId;

	//bool isSquad = RecvPacket.readFlag();

	/*char szName[COMMON_STRING_LENGTH];
	RecvPacket.readString( szName );*/

	if( m_pClientGameplayState->getSelectedPlayerId() == nPlayerId )
	{
		Con::executef("ShowLeaveTeamNotify",CLIENT_TEAM->m_strName.c_str());
		std::string str = "<t c='0xff1b1bff' o='0x2b2b2bff' >您已离开队伍";
		str += CLIENT_TEAM->m_strName;
		str += "</t>";
		Con::executef("AddChatMessage","1",str.c_str(),"0","0");
		CLIENT_TEAM->OnDisband(); 
		Con::printf( "您已离开队伍" );
	}
	else
	{		
		Con::printf( "玩家[%d]已经离开队伍", nPlayerId );
		Con::executef("ChangeCaptain");
		char sId[64];
		dSprintf( sId, sizeof(sId), "%d", nPlayerId );
		CLIENT_TEAM->OnRemovePlayer( nPlayerId );
		Con::executef( "TeammateLeaveTeam", sId );
	}

	return false;
}

// 接受到队伍列表
bool UserPacketProcess::Handle_Client_Team_Info( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	// 清空本地队伍
	CLIENT_TEAM->OnDisband();

	CLIENT_TEAM->m_nTeamId = pHead->DestZoneId;

	bool isSquad = RecvPacket.readFlag();

	char szName[COMMON_STRING_LENGTH] = { 0 };
	RecvPacket.readString( szName,COMMON_STRING_LENGTH );
	//CLIENT_TEAM->m_strName = szName;

	int nCount = RecvPacket.readInt( 32 );

	char playerName[COMMON_STRING_LENGTH];
	int playerId;
	for( int i = 0; i < nCount; i++ )
	{
		playerId = RecvPacket.readInt( 32 );
		CLIENT_TEAM->OnAddPlayer( playerId );
		if( RecvPacket.readFlag() )
		{
			RecvPacket.readString( playerName,COMMON_STRING_LENGTH );
			PLAYER_MGR->SetPlayerIdByName( playerName, playerId );
			stPlayerDisp& pDisp = PLAYER_MGR->GetPlayerDisp( playerId );
			pDisp.Level = RecvPacket.readInt( 8 );
			pDisp.Sex = RecvPacket.readInt( 2 );
			pDisp.Family = RecvPacket.readInt( 8 );
		}
	}

	Con::printf( "您已加入队伍[%d]", pHead->DestZoneId );
	Con::executef("ShowTeamBuildOK",szName);

	return false;
}

// 组队相关错误
bool UserPacketProcess::Handle_Client_Team_Error( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	Con::printf("TEAM ERROR: %d", pHead->DestZoneId );
	char* pText;
	int errorId = pHead->DestZoneId;
	switch( errorId )
	{
	case TEAM_PLAYER_ERROR:
		pText = "组队玩家错误！";
		break;
	case TEAM_SELF_ERROR:
		pText = "不能向自身发起组队！";
		break;
	case TEAM_SELF_HAVE_TEAM:
		pText = "自身已经有队伍！";
		break;
	case TEAM_OTHER_HAVE_TEAM:
		pText = "对方已有队伍！";
		break;
	case TEAM_TEAMMATE_FULL:
		pText = "队伍已满！";
		break;
	default:
		pText = "未知的错误！";
	}

	Con::evaluatef( "ShowTeamOperationTxt(%s);", pText );
	return false;
}

// 接收组队基本信息
void UserPacketProcess::Handle_Client_Team_BaseInfo( stPacketHead* pHead, Base::BitStream& RecvPacket )
{
	int cU = pHead->SrcZoneId;
	int cV = pHead->LineServerId;

	char szName[255] = { 0 };
	if( RecvPacket.readFlag() )
	{
		RecvPacket.readString( szName,255 );
		CLIENT_TEAM->m_strName = szName;
		Con::executef( "TeamHavaChangeName", szName );
	}

	if( RecvPacket.readFlag() )
	{
		int nLeaderId;
		nLeaderId = RecvPacket.readInt( 32 );
		dSprintf( szName, sizeof(szName), "%d", nLeaderId );
		CLIENT_TEAM->SetCaption( nLeaderId );
		Con::executef( "TeamHavaChangeHeader", szName );		
	}

	if( cU != 0 )
		CLIENT_TEAM->m_cU = cU;

	if( cV != 0 )
		CLIENT_TEAM->m_cV = cV;
}

bool UserPacketProcess::Handle_Client_Team_JoinRequest( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	PLAYER_MGR->HandleInfoPacket( &RecvPacket );
	
	Con::printf( "玩家[%d]请求加入队伍", pHead->DestZoneId );
	char ID[10];
	dSprintf(ID, sizeof(ID), "%d", pHead->DestZoneId);
	Player* pPlayer = PLAYER_MGR->GetLocalPlayer( pHead->DestZoneId );
	char szName[COMMON_STRING_LENGTH] = { 0 };
	if( pPlayer )
		dStrcpy( szName, sizeof(szName), pPlayer->getPlayerName());

	if( CLIENT_TEAM->AddInvation( pHead->DestZoneId, 1 ) )
		Con::executef("RequestToTeam",ID, szName, "1" ,"0");
	return false;
}

void UserPacketProcess::Send_Client_Team_BuildReuest( int nPlayerId, int nPlayerIdTo )
{
	SendSimplePacket( nPlayerId, CLIENT_TEAM_BuildRequest, nPlayerIdTo );
}

void UserPacketProcess::Send_Client_Team_AcceptBuildRequest( int nPlayerId, int nPlayerIdTo )
{
	SendSimplePacket( nPlayerId, CLIENT_TEAM_AcceptBuildRequest, nPlayerIdTo );
}

void UserPacketProcess::Send_Client_Team_DropPlayer( int nPlayerId, int nPlayerIdTo )
{
	SendSimplePacket( nPlayerId, CLIENT_TEAM_DropRequest, nPlayerIdTo );
}

void UserPacketProcess::Send_Client_Team_AddRequest( int nPlayerId, int nPlayerIdTo )
{
	SendSimplePacket( nPlayerId, CLIENT_TEAM_AddRequest, nPlayerIdTo );
}

void UserPacketProcess::Send_Client_Team_AcceptAddRequest( int nPlayerId, int nPlayerIdTo )
{
	SendSimplePacket( nPlayerId, CLIENT_TEAM_AcceptAddRequest, nPlayerIdTo );
}

void UserPacketProcess::Send_Client_Team_JoinRequest( int nPlayerId, int nPlayerIdTo )
{
	SendSimplePacket( nPlayerId, CLIENT_TEAM_JoinRequest, nPlayerIdTo );
}

void UserPacketProcess::Send_Client_Team_AcceptJoinRequest( int nPlayerId, int nPlayerIdTo )
{
	SendSimplePacket( nPlayerId, CLIENT_TEAM_AcceptdJoinRequest, nPlayerIdTo );
}

void UserPacketProcess::Send_Client_Team_Leave( int nPlayerId )
{
	SendSimplePacket( nPlayerId, CLIENT_TEAM_LeaveRequest );
}

void UserPacketProcess::Send_Client_Team_BuildAlongRequest( int nPlayerId )
{
	SendSimplePacket( nPlayerId, CLIENT_TEAM_BuildAlongRequest );
}

void UserPacketProcess::Send_Client_Team_Disband( int nPlayerId )
{
	SendSimplePacket( nPlayerId, CLIENT_TEAM_Disband );
}

//////////////////////////////////////////////////////////////////////////


void UserPacketProcess::Handle_World_Client_PlayerDataTransResopnse(stPacketHead *pHead,Base::BitStream &RecvPacket)
{
	PLAYER_MGR->HandlePlayerDataTransResponse( pHead, RecvPacket );
}

bool UserPacketProcess::Handle_Client_Team_MapMark( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	CLIENT_TEAM->mapMarks[pHead->DestZoneId].ReadPacket( &RecvPacket );
	Con::printf( "MAP MARK:id[%d], x[%d], y[%d]", pHead->DestZoneId, CLIENT_TEAM->mapMarks[pHead->DestZoneId].sX, CLIENT_TEAM->mapMarks[pHead->DestZoneId].sY );
	return true;
}

bool UserPacketProcess::Handle_Client_Team_TargetMark( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	CLIENT_TEAM->targetMarks[pHead->DestZoneId].ReadPacket( &RecvPacket );	
	return true;
}

void UserPacketProcess::Send_Client_Team_MapMark( int id, int x, int y )
{
	stTeamMapMark mapMark;
	mapMark.sMapId = m_pClientGameplayState->getCurrentZoneId();
	mapMark.sX = x;
	mapMark.sY = y;

	char buf[MAX_PACKET_SIZE];
	Base::BitStream packet( buf, MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( packet, CLIENT_TEAM_MapMark, m_pClientGameplayState->getSelectedPlayerId(), SERVICE_WORLDSERVER, id );
	mapMark.WritePacket( &packet );
	pHead->PacketSize = packet.getPosition() - sizeof( stPacketHead );

	Send( packet );
}

void UserPacketProcess::Send_Client_Team_TargetMark( int id, int targetId )
{
	stTeamTargetMark targetMark;
	targetMark.sMapId = m_pClientGameplayState->getCurrentZoneId();
	targetMark.sTargetId = targetId;

	char buf[MAX_PACKET_SIZE];
	Base::BitStream packet( buf, MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( packet, CLIENT_TEAM_TargetMark, m_pClientGameplayState->getSelectedPlayerId(), SERVICE_WORLDSERVER, id );
	targetMark.WritePacket( &packet );
	pHead->PacketSize = packet.getPosition() - sizeof( stPacketHead );

	Send( packet );
}

void UserPacketProcess::Send_Client_Team_Refused( int nPlayerId, int nType )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream packet( buf, MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( packet, CLIENT_TEAM_Refused, m_pClientGameplayState->getSelectedPlayerId(), SERVICE_WORLDSERVER, nType, nPlayerId );
	pHead->PacketSize = packet.getPosition() - sizeof( stPacketHead );

	Send( packet );
}

void UserPacketProcess::Handle_Client_Team_Refused( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nType = pHead->DestZoneId;
	int nPlayerId = pHead->SrcZoneId;
    
	// [LivenHotch]: here we need to handle refused
	char m[10];
	char m1[10];
	dSprintf( m, sizeof(m), "%d", nType );
	dSprintf( m1,sizeof(m1), "%d", nPlayerId );
	Con::executef( "RefusedApplyRequest", m1, m );
}

void UserPacketProcess::Handle_Client_Team_Cancel( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nType = pHead->DestZoneId;
	int nPlayerId = pHead->SrcZoneId;

	// [LivenHotch]: here we need to handle cancel
	Con::printf( "玩家[%d]拒绝了您的请求[%d]", nPlayerId, nType );
}

void UserPacketProcess::Send_Client_Team_ChangeName( int nPlayerId, char* szName )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream packet( buf, MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( packet, CLIENT_TEAM_BaseInfo, nPlayerId, SERVICE_WORLDSERVER );

	packet.writeFlag( true );
	packet.writeString( szName );
	packet.writeFlag( false );

	pHead->PacketSize = packet.getPosition() - sizeof( stPacketHead );

	Send( packet );
}

void UserPacketProcess::Send_Client_Team_ChangeLeader( int nPlayerId, int nNewLeaderId )
{
	char buf[MAX_PACKET_SIZE];
	Base::BitStream packet( buf, MAX_PACKET_SIZE );

	stPacketHead* pHead = IPacket::BuildPacketHead( packet, CLIENT_TEAM_BaseInfo, nPlayerId, SERVICE_WORLDSERVER );

	packet.writeFlag( false );
	packet.writeFlag( true );
	packet.writeInt( nNewLeaderId, 32 );
	pHead->PacketSize = packet.getPosition() - sizeof( stPacketHead );
	Send( packet );	
}

bool UserPacketProcess::Handle_Client_CopyMap_OpenResponse( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nCopyMapInstId = pHead->DestZoneId;
	int nErr = pHead->SrcZoneId;

	Con::printf("您已开启副本[%d],{%d}", nCopyMapInstId, nErr );

	return true;
}

bool UserPacketProcess::Handle_Client_CopyMap_EnterResponse( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int nCopyMapInstId = pHead->DestZoneId;
	int nErr = pHead->SrcZoneId;

	// if there is something wrong
	if( nErr )
	{
		Con::printf("进入副本错误,代码:%d",nErr);
		return true;
	}
	int nIp = RecvPacket.readInt( 32 );
	int nPort = RecvPacket.readInt( 16 );

	g_ClientGameplayState->setCopymapLayer( nCopyMapInstId );
	g_ClientGameplayState->setGoingCopyMap(true);
	g_ClientGameplayState->setCopymapZoneIp( nIp );
	g_ClientGameplayState->setCopymapZonePort( nPort );
	g_ClientGameplayState->setDisconnectZone(true);
	Con::printf("You are ready to enter copy map [%d], IP: %d:%d", nCopyMapInstId, nIp, nPort );
	
	return true;
}

bool UserPacketProcess::Handle_Client_CopyMap_LeaveResponse( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	Con::printf("您已离开副本[%d]", pHead->DestZoneId );
	g_ClientGameplayState->setInCopymap( false );
	if( m_pClientGameplayState->GetControlPlayer()->getLayerID() == 1 )
		return true;
	g_ClientGameplayState->setTransporting( true );
	g_ClientGameplayState->setDisconnectZone(true);
	return true;
}


/////////////////////////////////////测试代码


bool UserPacketProcess::Send_Chat_TestMessage( int AccountId, const char* pMessage )
{
	char Buffer[256];
	Base::BitStream SendPacket(Buffer,256);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_CHAT_SendMessageRequest, AccountId, SERVICE_WORLDSERVER );
	stChatMessage chatMessage;
	chatMessage.btMessageType = CHAT_MSG_TYPE_TEAM;
	dStrcpy( chatMessage.szMessage, sizeof(chatMessage.szMessage), pMessage );
	stChatMessage::PackMessage( chatMessage, SendPacket );
	pSendHead->PacketSize = SendPacket.getPosition() - GetHeadSize();
	
	return Send( SendPacket );

}

void UserPacketProcess::Handle_Client_Team_ShowInfo( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	char szTeamName[COMMON_STRING_LENGTH];
	RecvPacket.readString( szTeamName,COMMON_STRING_LENGTH );
	Con::executef( "RecvTeamInfoName", szTeamName );
	int nCount = RecvPacket.readInt( 32 );

	char szName[COMMON_STRING_LENGTH];
	char szLevel[COMMON_STRING_LENGTH];
	char szFamily[COMMON_STRING_LENGTH];
	char szIndex[COMMON_STRING_LENGTH];
	
	for( int i = 0; i < nCount; i++ )
	{
		RecvPacket.readString( szName,COMMON_STRING_LENGTH );
		dSprintf( szFamily, sizeof(szFamily), "%d", RecvPacket.readInt( 8 ));
		dSprintf( szLevel, sizeof(szLevel), "%d", RecvPacket.readInt( 8 ));
		dSprintf( szIndex,  sizeof(szIndex), "%d", i );

		Con::executef("RecvTeamInfo", szName, szFamily, szLevel, szIndex );
	}
}

bool UserPacketProcess::SendGMCommand(int sessionId,Base::BitStream* pack,int AccountId,const char* fun)
{
    static char buffer[4096];

    Base::BitStream pkt(buffer,sizeof(buffer));
    stPacketHead *pSendHead = BuildPacketHead(pkt,GM_MESSAGE,AccountId,SERVICE_WORLDSERVER,sessionId);
    pkt.writeLongString(strlen(fun),fun);

    if (0 != pack)
    {
        pkt.writeFlag(true);
        pkt.writeBits(pack->getCurPos(),pack->getBuffer());
    }
    else
    {
        pkt.writeFlag(false);
    }

    pSendHead->PacketSize = pkt.getPosition() - IPacket::GetHeadSize();
    return Send(pkt);
}

bool UserPacketProcess::HandleGMCommandResp( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
    return m_pClientGameplayState->HandleGMCommandResp(pHead->DestZoneId,RecvPacket);
}

void UserPacketProcess::Hanlde_Client_Team_Follow( stPacketHead * pHead, Base::BitStream RecvPacket )
{
	bool isEnabled = pHead->DestZoneId;

	CLIENT_TEAM->OnEnableFollow( isEnabled );

	if( isEnabled )
	{
		Con::printf( "组队跟随开启!" );

		// TODO: update the UI
		if( CLIENT_TEAM->GetCaption() != g_ClientGameplayState->getSelectedPlayerId() )
			Con::executef("RecvTeamFollowRequest","1");
	}
	else
	{
		Con::printf( "组队跟随关闭" );
		
		// TODO： update the UI		
		if( CLIENT_TEAM->GetCaption() != g_ClientGameplayState->getSelectedPlayerId() )
			Con::executef("RecvTeamFollowRequest","0");
	}
}

void UserPacketProcess::Hanlde_World_Client_MailNotify( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	// 创建本地逻辑延迟事件
	Player::stLogicEvent* logicEvent = new Player::stLogicEvent;
	logicEvent->type		= Player::LOGICEVENT_NOTIFYNEWMAIL;
	//logicEvent->sender		= nPlayerId;
	logicEvent->senderName	= StringTable->insert("");
	logicEvent->data		= NULL;
	logicEvent->datalen		= 0;

	Player* pPlayer = g_ClientGameplayState->GetControlPlayer();
	if( pPlayer )
	{	
		U32 eid = pPlayer->insertLogicEvent(logicEvent);

		// 弹出通用邀请计时对话框
		Con::executef("CommonInviteDialog", Con::getIntArg(eid));
	}
}

void UserPacketProcess::Handle_Account_CheckResponse( Base::BitStream &RecvPacket )
{
	int error = RecvPacket.readInt( 16 );

	// 没有错误
	if( error ==  0 )
	{
		m_pClientGameplayState->m_isPwdChecked = true;
	}

	Con::executef( "OpenServerNotice", Con::getIntArg( error ) );
}

bool UserPacketProcess::Send_Account_CheckRequest( const char *AccountName,const unsigned char *Password )
{
	char Buffer[128];
	Base::BitStream SendPacket(Buffer,128);
	stPacketHead *pSendHead = BuildPacketHead(SendPacket,CLIENT_ACCOUNT_CheckRequest);
	SendPacket.writeInt(END_NET_MESSAGE,Base::Bit16);
	SendPacket.writeString(AccountName);
	SendPacket.writeBits(MD5_STRING_LENGTH*Base::Bit8,Password);
	pSendHead->PacketSize = SendPacket.getPosition()-GetHeadSize();
	return Send(SendPacket);
}

void UserPacketProcess::Handle_Client_ItemRequest( stPacketHead *pHead,Base::BitStream &RecvPacket )
{
	int playerId = pHead->SrcZoneId;

	char playerName[COMMON_STRING_LENGTH];

	int itemId = RecvPacket.readInt( 32 );
	int itemCount = RecvPacket.readInt( 32 );

	RecvPacket.readString( playerName ,COMMON_STRING_LENGTH);

	Con::executef("showAskForInfor", playerName, Con::getIntArg(itemId), Con::getIntArg(itemCount));
}