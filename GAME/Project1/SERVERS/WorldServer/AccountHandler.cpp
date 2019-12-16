#include "BanList.h"
#include "DBLib/dbLib.h"
#include "wintcp/dtServerSocket.h"
#include "AccountHandler.h"
#include "WorldServer.h"
#include "ServerMgr.h"
#include "PlayerMgr.h"
#include "ChatMgr.h"
#include "zlib/zlib.h"
#include "ExportScript.h"
#include "MailManager.h"
#include "CopymapManager.h"
#include "SocialManager.h"
#include "LockCommon.h"
#include "AccountServer/PlayerMgr.h"
#include "Common/Log/LogTypes.h"
#include "TopManager.h"
#include <mmsystem.h>

#pragma comment( lib, "Winmm.lib" )


const char* stAccountInfo::GetCurPlayerName(void)
{
    if (-1 == CurPlayerPos)
        return "";

    return SERVER->GetPlayerManager()->loadPlayer( PlayerIdList[CurPlayerPos] )->BaseData.PlayerName;
}

void stAccountInfo::__InitStatus(void)
{
    m_status[STATUS_IDEL].onExec            = &stAccountInfo::__OnIdleStatusHandler;
    m_status[STATUS_LOGIN_ACCOUNT].onExec   = &stAccountInfo::__OnLoginAccountStatusHandler;
    m_status[STATUS_IN_SELECT].onExec       = &stAccountInfo::__OnInSelectStatusHandler;
    m_status[STATUS_LOGIN_GAME].onExec      = &stAccountInfo::__OnLoginGameStatusHandler;
    m_status[STATUS_IN_GAME].onExec         = &stAccountInfo::__OnInGameStatusHandler;
    m_status[STATUS_MAP_SWITCH].onExec      = &stAccountInfo::__OnSwitchMapStatusHandler;
    m_status[STATUS_DELAY_DEL].onExec       = &stAccountInfo::__OnDelayStatusHandler;

    m_status[STATUS_LOGIN_GAME].onEnter     = &stAccountInfo::__OnEnterLoginGameHandler;
    m_status[STATUS_DELAY_DEL].onEnter      = &stAccountInfo::__OnEnterDelayDelHandler;
    m_status[STATUS_IN_GAME].onEnter        = &stAccountInfo::__OnEnterGameHandler;

    m_status[STATUS_LOGIN_ACCOUNT].onExpire = &stAccountInfo::__OnLoginAccountExpire;
    m_status[STATUS_LOGIN_GAME].onExpire    = &stAccountInfo::__OnLoginAccountExpire;
    m_status[STATUS_MAP_SWITCH].onExpire    = &stAccountInfo::__OnLoginAccountExpire;
    m_status[STATUS_DELAY_DEL].onExpire     = &stAccountInfo::__OnDelayStatusExpire;

}

bool stAccountInfo::Kick(const char* reason,int& error, bool isModifyState)
{
	DO_LOCK( Lock::Player | Lock::Server );

    if (0 == reason)
        reason = "";

    //通知客户端被踢线
    CMemGuard buf( MAX_PACKET_SIZE );
    Base::BitStream SendPacket( buf.get(),MAX_PACKET_SIZE);

    stPacketHead* pHead = IPacket::BuildPacketHead( SendPacket,WORLD_CLIENT_NOTIFY_KICKED,AccountId,SERVICE_CLIENT);
    SendPacket.writeString(reason);

    pHead->PacketSize = SendPacket.getPosition() - sizeof(stPacketHead);
    SERVER->GetServerSocket()->Send(socketId,SendPacket);

    // 踢掉当前游戏中的用户
    if(this->CurPlayerPos != -1)
    {
        int playerId = this->PlayerIdList[this->CurPlayerPos];
        PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( playerId );
        
        if (!pPlayerData.isNull())
        {
            if (0 == SERVER->GetServerManager()->GetZoneInfo(socketId,pPlayerData->DispData.ZoneId))
                return false;

			// 修改为通过当前玩家登陆的线程去踢出玩家
            if (!__SendWorldZoneTickPlayerRequest(LineId,pPlayerData,playerId,true))
                return false;

            SERVER->GetChatManager()->GetChannelManager()->ClearAccount(pPlayerData->BaseData.AccountId);
        }
        else
        {
			//error = UNKNOW_ERROR;
            return false;
        }
    }
    
    if (isModifyState)
        m_status.SetState(STATUS_DELAY_DEL,PLAYER_TIMEOUT);
    
    return true;
}

int stAccountInfo::TransportPlayer(int playerId,int mapId,float x,float y,float z)
{
    if (m_status.GetState() != STATUS_IN_GAME)
        return 1;

    //向zone发送传送消息
	DO_LOCK( Lock::Server | Lock::Player );

    PlayerDataRef pPlayerData;

    if(CurPlayerPos != -1)
        pPlayerData = SERVER->GetPlayerManager()->loadPlayer( PlayerIdList[CurPlayerPos] );

    if (pPlayerData.isNull() || pPlayerData->BaseData.PlayerId != playerId)
        return 1;

    stServerInfo* pServerInfo = SERVER->GetServerManager()->GetGateServerByZoneId(LineId,pPlayerData->DispData.ZoneId);

    if (0 == pServerInfo)
        return false;

    CMemGuard buf(60);
    Base::BitStream sendPacket( buf.get(),60);

    stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WORLD_ZONE_TransportPlayerRequest,playerId,SERVICE_ZONESERVER);
    pSendHead->DestZoneId   = pPlayerData->DispData.ZoneId;
    pSendHead->SrcZoneId    = pPlayerData->BaseData.AccountId;

    sendPacket.writeInt(mapId,Base::Bit32);
    sendPacket.writeBits(Base::Bit32,&x);
    sendPacket.writeBits(Base::Bit32,&y);
    sendPacket.writeBits(Base::Bit32,&z);

    pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

    SERVER->GetServerSocket()->Send( pServerInfo->SocketId, sendPacket);
    return 0;
}

/************************************************************************/
/* 响应玩家的登入请求
/************************************************************************/
void stAccountInfo::__OnIdleStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet)
{
    switch(opcode)
    {
    case ACCOUNT_WORLD_ClientLoginRequest:
        {
            int  Error  = 0;
            bool kicked = false;

            if(this->CurPlayerPos != -1)
            {
                kicked = Kick("帐号在另外一个地方登录", Error );
            }

            //判断是否临时冻结
            if (CBanlist::Instance()->IsAccLoginBan(AccountName))
                Error = ERROR_FREEZE;

            //判断是否是IP冻结
            if (Error == NONE_ERROR)
            {
                if (CBanlist::Instance()->IsIpLoginBan(loginIP.c_str()))
                    Error = ERROR_FREEZE;
            }

            if (0 != Error)
            {
                __SendClientLoginAccountResponse(Error,SocketHandle);
                return;
            }

            this->accountSocketId = SocketHandle;

            if(!kicked)
            {
                __SendClientLoginAccountResponse(Error,SocketHandle);
            }

			// 设置防沉迷状态
			
            
            //切换到登录状态
            m_status.SetState(STATUS_LOGIN_ACCOUNT,60);
        }
        break;
    }
}

/************************************************************************/
/* 等待玩家连入gate,并处理从zone玩家踢线的返回
/************************************************************************/
void stAccountInfo::__OnLoginAccountStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet)
{
    switch(opcode)
    {
    case GATE_WORLD_ClientLoginRequest:
        {
            this->socketId = SocketHandle;
			LineId = SERVER->GetServerManager()->GetLineId( socketId );

            T_UID UID = Packet->readInt(UID_BITS);

            int Error = SERVER->GetPlayerManager()->VerifyUID(UID,AccountId);

			g_Log.WriteLog( "客户端请求登陆网关【%d】", AccountId );

            CMemGuard Buffer(64);
            Base::BitStream SendPacket(Buffer.get(),64);
            stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,GATE_WORLD_ClientLoginResponse,AccountId);
            SendPacket.writeInt(Error,Base::Bit16);
            pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
            SERVER->GetServerSocket()->Send(SocketHandle,SendPacket);

			//如果验证成功，则切换到人物选择状态
			m_status.SetState(STATUS_IN_SELECT);
        }
        break;
    case WORLD_ZONE_TickPlayerResponse:
        {
            __SendClientLoginAccountResponse(0,accountSocketId);
        }
        break;
    case CLIENT_GAME_LogoutRequest:
    case GATE_WORLD_ClientLost:
        {
            //30秒后帐号将被删除
            m_status.SetState(STATUS_DELAY_DEL,PLAYER_TIMEOUT);
        }
        break;
    case ACCOUNT_WORLD_ClientLoginRequest:
        {
            //客户端从选择界面推出后，是没有任何消息的，直接发送登录的消息
            m_status.SetRawState(STATUS_IDEL);
            m_status.Trigger(opcode,SocketHandle,pHead,Packet);
        }
        break;
    }
}

/************************************************************************/
/* 此时玩家已经在角色选择界面，相应角色相关的操作和客户端断线等操作
/************************************************************************/
void stAccountInfo::__OnInSelectStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet)
{
    switch(opcode)
    {
	case GATE_WORLD_ClientLoginRequest:
		{
			socketId = SocketHandle;
			m_status.SetRawState(STATUS_LOGIN_ACCOUNT,60);
			m_status.Trigger(opcode,SocketHandle,pHead,Packet);
		}
		break;
    case ACCOUNT_WORLD_ClientLoginRequest:
        {
            //客户端从选择界面推出后，是没有任何消息的，直接发送登录的消息
            m_status.SetRawState(STATUS_IDEL);
            m_status.Trigger(opcode,SocketHandle,pHead,Packet);
        }
        break;
    case CLIENT_WORLD_SelectPlayerRequest:
        {
            T_UID UID = Packet->readInt(UID_BITS);

            CMemGuard Buffer(MAX_PACKET_SIZE);
            Base::BitStream SendPacket(Buffer.get(),MAX_PACKET_SIZE);
            stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,CLIENT_WORLD_SelectPlayerResponse,AccountId,SERVICE_CLIENT);

            SendPacket.writeInt(UID,UID_BITS);
            SERVER->GetPlayerManager()->PackPlayerSelectData(AccountId,SendPacket);
            pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
            SERVER->GetServerSocket()->Send(SocketHandle,SendPacket);
        }
        break;
    case CLIENT_WORLD_DeletePlayerRequest:
        {
            T_UID UID = Packet->readInt(UID_BITS);
            int PlayerId = Packet->readInt(Base::Bit32);

            //log it
            PlayerDataRef pPlayer = SERVER->GetPlayerManager()->loadPlayer(PlayerId);

            if (!pPlayer.isNull() )
            {
                lg_char_delete lg;

                lg.playerName  = pPlayer->BaseData.PlayerName;
                lg.accountName = AccountName;
                lg.playerId    = PlayerId;
                lg.job         = pPlayer->DispData.Classes[0];
                lg.level       = pPlayer->DispData.Level;
                lg.sex         = pPlayer->DispData.Sex;
                lg.race        = pPlayer->DispData.Race;

                SERVER->GetLog()->writeLog(&lg);
            }

			// 增加在删除角色的时候清理
			ClearPlayer( PlayerId );
			SERVER->GetPlayerManager()->RemovePlayer( PlayerId );

            int Error = SERVER->GetPlayerManager()->DeletePlayer(UID,AccountId,PlayerId);

            CMemGuard Buffer(64);
            Base::BitStream SendPacket(Buffer.get(),Buffer.size());
            stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,CLIENT_WORLD_DeletePlayerResponse,AccountId,SERVICE_CLIENT);
            SendPacket.writeInt(UID,UID_BITS);
            SendPacket.writeInt(PlayerId,Base::Bit32);
            SendPacket.writeInt(Error,Base::Bit16);
            pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
            SERVER->GetServerSocket()->Send(SocketHandle,SendPacket);
        }
        break;
    case CLIENT_WORLD_CreatePlayerRequest:
        {
            T_UID UID = Packet->readInt(UID_BITS);
            char PlayerName[COMMON_STRING_LENGTH];
            Packet->readString(PlayerName,COMMON_STRING_LENGTH);
            int Sex		= Packet->readInt(Base::Bit8);
            int Body	= Packet->readInt(Base::Bit16);
            int Face	= Packet->readInt(Base::Bit16);
            int Hair	= Packet->readInt(Base::Bit16);
            int HairCol = Packet->readInt(Base::Bit8);
            int ItemId	= Packet->readInt(Base::Bit32);
            //int ZoneId	= Packet->readInt(Base::Bit16);
			int ZoneId	= 1127;

            int PlayerId = 0;
            int Error = SERVER->GetPlayerManager()->CreatePlayer(UID,AccountId,PlayerName,Sex,Body,Face,Hair,HairCol,ItemId,ZoneId,PlayerId);

            CMemGuard Buffer(MAX_PACKET_SIZE);
            Base::BitStream SendPacket(Buffer.get(),Buffer.size());
            stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,CLIENT_WORLD_CreatePlayerResponse,AccountId,SERVICE_CLIENT);
            SendPacket.writeInt(UID,UID_BITS);
            SendPacket.writeInt(Error,Base::Bit16);

            if(Error == NONE_ERROR)
            {
                SERVER->GetPlayerManager()->PackPlayerSelectData(AccountId,PlayerId,SendPacket);
    
                //log it
                lg_char_create lg;
                
                lg.playerName  = PlayerName;
                lg.accountName = AccountName;
                lg.playerId    = PlayerId;
                lg.job         = 0;
                lg.level       = 0;
                lg.sex         = Sex;
                lg.race        = 0;
                
                SERVER->GetLog()->writeLog(&lg);
            }
            else
                SendPacket.writeString(PlayerName);

            pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
            SERVER->GetServerSocket()->Send(SocketHandle,SendPacket);
        }
        break;
    case CLIENT_GAME_LoginRequest:
        {
            m_status.SetState(STATUS_LOGIN_GAME,60);
        }
        break;
    case CLIENT_GAME_LogoutRequest:
    case GATE_WORLD_ClientLost:
        {
            if (opcode == CLIENT_GAME_LogoutRequest)
                __SendClientLogout(SocketHandle,pHead,Packet);

            //30秒后帐号将被删除
            m_status.SetState(STATUS_DELAY_DEL,PLAYER_TIMEOUT);
        }
        break;
    }
}

/************************************************************************/
/* 玩家正在进入游戏
/************************************************************************/
void stAccountInfo::__OnLoginGameStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet)
{
    switch(opcode)
    {
    case WORLD_ZONE_TickPlayerResponse:
        {
            //对于地图切换，只需直接发送给客户端resp
            if (m_t_isSwitch)
            {
                __SendClientGameLoginResponse(0,socketId, UID, LineId, AccountId, m_t_PlayerId);
            }
            else
			{
                if (!__SendZoneGameLogin(socketId, UID, LineId, AccountId, m_t_PlayerId,false))
                {
                    m_status.SetState(STATUS_IN_SELECT);
                }
            }
        }
        break;
	case GATE_WORLD_ClientLoginRequest:
		{
			socketId = SocketHandle;
			m_status.SetRawState(STATUS_LOGIN_ACCOUNT,60);
			m_status.Trigger(opcode,SocketHandle,pHead,Packet);
		}
		break; 
	case ZONE_WORLD_PlayerLogin:
		{
			int PlayerId = pHead->Id;

			// 传送地图信息
			{
				PlayerDataRef pPlayer       = SERVER->GetPlayerManager()->GetPlayerData( PlayerId );
				stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( PlayerId );

				// 修正为玩家登陆到新的地图后,由地图主动保存玩家数据.
				/*if( pPlayer )
				pPlayer->DispData.ZoneId = ZoneId;*/

				CTeam* pTeam = SERVER->GetTeamManager()->GetTeamByPlayer( PlayerId );

				// 向新的地图服务器发送队伍信息
				if( pAccountInfo && pTeam)
					SERVER->GetTeamManager()->SendTeamInfoToZone( pAccountInfo->LineId, pTeam->GetId() );

				//CSCRIPT->run( "onPlayerSwitchZone", 2, PlayerId, ZoneId );
			}

			// 传送社会关系信息
			{
				SERVER->GetSocialManager()->notifyZonePlayerSocialInfo( PlayerId );
			}

			m_status.SetState( STATUS_IN_GAME );
		}
		break;
    case WORLD_ZONE_PlayerDataResponse:
        {
			T_UID zoneUid = Packet->readInt( UID_BITS );
			
			// 判断UID，如果不符，切换到人物选择状态
			if( zoneUid != UID )
			{
				m_status.SetState( STATUS_IN_SELECT );

				__SendClientGameLoginResponse( GAME_UID_ERROR, socketId, UID, LineId, AccountId,m_t_PlayerId);
				
				break;
			}

            __SendClientGameLoginResponse(0,socketId, UID, LineId, AccountId,m_t_PlayerId);

			// 玩家数据已经传送到Zone，清理玩家帐户的其他无用数据
			for( int i = 0; i < this->PlayerNum; i++ )
			{
				int playerId = this->PlayerIdList[i];
				if( playerId != m_t_PlayerId )
				{
					SERVER->GetPlayerManager()->RemovePlayer( playerId );
				}
			}
        }
        break;
    case CLIENT_GAME_LogoutRequest:
    case GATE_WORLD_ClientLost:
        {
            //30秒后帐号将被删除
            m_status.SetState(STATUS_DELAY_DEL,PLAYER_TIMEOUT);
        }
        break;
    case ACCOUNT_WORLD_ClientLoginRequest:
        {
            //客户端从选择界面推出后，是没有任何消息的，直接发送登录的消息
            m_status.SetRawState(STATUS_IDEL);
            m_status.Trigger(opcode,SocketHandle,pHead,Packet);
        }
        break;
	case CLIENT_GAME_LoginRequest:
		{
			m_status.SetState(STATUS_LOGIN_GAME,120);
		}break;

    }
}

/************************************************************************/
/* 处理玩家在游戏世界里的逻辑
/************************************************************************/
void stAccountInfo::__OnInGameStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet)
{
    switch(opcode)
    {
    case GATE_WORLD_ClientLost:
    case CLIENT_GAME_LogoutRequest:
        {
            if (opcode == CLIENT_GAME_LogoutRequest)
                __SendClientLogout(SocketHandle,pHead,Packet);
            
            //30秒后帐号将被删除
            m_status.SetState(STATUS_DELAY_DEL,PLAYER_TIMEOUT);
        }
        break;
	case CLIENT_GAME_LoginRequest:
		{
			m_status.SetState(STATUS_LOGIN_GAME,60);
		}
		break;
	case CLIENT_WORLD_SelectPlayerRequest:
		{
			__OnPlayerLeaveGame();
		
			m_status.SetRawState( STATUS_IN_SELECT );
			m_status.Trigger( opcode, SocketHandle, pHead, Packet );
		}
		break;
    case GATE_WORLD_ClientLoginRequest:
        {
			this->socketId = SocketHandle;

			T_UID UID = Packet->readInt(UID_BITS);

			int Error = SERVER->GetPlayerManager()->VerifyUID(UID,AccountId);
			
			int CurLineId = SERVER->GetServerManager()->GetLineId( SocketHandle );
			this->LineId = CurLineId;

			g_Log.WriteLog( "客户端请求登陆网关【%d】,[%d]", AccountId, LineId );

			CMemGuard Buffer(64);
			Base::BitStream SendPacket(Buffer.get(),64);
			stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,GATE_WORLD_ClientLoginResponse,AccountId);
			SendPacket.writeInt(Error,Base::Bit16);
			pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(SocketHandle,SendPacket);
        }
        break;
    case ACCOUNT_WORLD_ClientLoginRequest:
        {
            __OnPlayerLeaveGame();

            //客户端在其他地方重新登陆
            m_status.SetRawState(STATUS_IDEL);
            m_status.Trigger(opcode,SocketHandle,pHead,Packet);
        }
        break;
	case ZONE_WORLD_PlayerLogin:
		{
			int PlayerId = pHead->Id;

			// 传送地图信息
			{
				PlayerDataRef pPlayer       = SERVER->GetPlayerManager()->GetPlayerData( PlayerId );
				stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountInfo( PlayerId );

				// 修正为玩家登陆到新的地图后,由地图主动保存玩家数据.
				/*if( pPlayer )
				pPlayer->DispData.ZoneId = ZoneId;*/

				CTeam* pTeam = SERVER->GetTeamManager()->GetTeamByPlayer( PlayerId );

				// 向新的地图服务器发送队伍信息
				if( pAccountInfo && pTeam)
					SERVER->GetTeamManager()->SendTeamInfoToZone( pAccountInfo->LineId, pTeam->GetId() );

				//CSCRIPT->run( "onPlayerSwitchZone", 2, PlayerId, ZoneId );
			}

			// 传送社会关系信息
			{
				SERVER->GetSocialManager()->notifyZonePlayerSocialInfo( PlayerId );
			}
		}
		break;
    case ZONE_WORLD_SwitchMapRequest:
        {
            //处理地图切换
            T_UID UID		= Packet->readInt(UID_BITS);
            int LineId		= Packet->readInt(16);
            int TriggerId	= Packet->readInt(32);
            int ZoneId	    = Packet->readInt(16);
            int x		    = Packet->readSignedInt(32);
            int y		    = Packet->readSignedInt(32);
            int z		    = Packet->readSignedInt(32);

            int Error = SERVER->GetPlayerManager()->VerifyUID(UID,pHead->Id);

            if(Error != NONE_ERROR)
                return;

            int ZipBufSize	= Packet->readInt(16);
            U8 *pZipBuf		= MEMPOOL->Alloc(ZipBufSize);
            Packet->readBits(ZipBufSize*Base::Bit8,pZipBuf);

            int Size		= MAX_PACKET_SIZE;
            U8 *pUnZipBuf	= MEMPOOL->Alloc(Size);
            int iret        = uncompress((Bytef*)pUnZipBuf,(uLongf*)&Size,(const Bytef *)pZipBuf,ZipBufSize);

            if (!iret)
            {
                Base::BitStream dataPacket(pUnZipBuf,Size);
                stPlayerStruct PlayerObject;
                PlayerObject.ReadData(&dataPacket);

                SERVER->GetPlayerManager()->SavePlayerData( &PlayerObject ); // 保存由ZoneServer 发来的数据

                int SrcLineId = 0;
                int DestZoneSocketHandle = 0;
                {
					g_Log.WriteLog( "玩家切换地图请求[%d]-[%d] PlayerId:%d", LineId, ZoneId, PlayerObject.BaseData.PlayerId );

                    stServerInfo *pServer = SERVER->GetServerManager()->GetGateServerByZoneId(LineId,ZoneId);

                    if(pServer)
                        DestZoneSocketHandle = pServer->SocketId;

                    pServer = SERVER->GetServerManager()->GetServerInfoBySocket(SocketHandle);

                    if(pServer)
                        SrcLineId = pServer->LineId;
                }

                //这里不再重组包了，但是包头必须修改
                pHead->DestServerType = SERVICE_ZONESERVER;
                pHead->DestZoneId	  = ZoneId;
                pHead->LineServerId	  = SrcLineId;

                int PacketSize = IPacket::GetTotalSize((const char *)pHead);
                Base::BitStream switchPacket((char *)pHead,PacketSize);
                switchPacket.setPosition(PacketSize);

                if( DestZoneSocketHandle )
                {

                    SERVER->GetServerSocket()->Send(DestZoneSocketHandle,switchPacket);

                    //切换地图ing
                    m_status.SetState(STATUS_MAP_SWITCH,300);
                }
                else
                {
                    CMemGuard b( MAX_PACKET_SIZE );
                    Base::BitStream errorPacket( b.get(), MAX_PACKET_SIZE );
                    stPacketHead* pErrorHead = IPacket::BuildPacketHead( errorPacket, ZONE_WORLD_SwitchMapResponse, 0, SERVICE_ZONESERVER, PlayerObject.DispData.ZoneId );
                    /*
                    int PlayerId = Packet->readInt(32);
                    int LineId	 = Packet->readInt(16);
                    int ZoneId	 = Packet->readInt(16);
                    int ErrorCode= Packet->readInt(16);
                    */
                    errorPacket.writeInt( UID, 32 );
                    errorPacket.writeInt( PlayerObject.BaseData.PlayerId, 32 );
                    errorPacket.writeInt( 0, 16 );
                    errorPacket.writeInt( 0, 16 );
                    errorPacket.writeInt( UNKNOW_ERROR, 16 );

                    pErrorHead->PacketSize = errorPacket.getPosition() - sizeof( stPacketHead );
                    SERVER->GetServerSocket()->Send( SocketHandle, errorPacket );

                }
            }

            MEMPOOL->Free(pZipBuf);
            MEMPOOL->Free(pUnZipBuf);
        }
        break;
    }

    //处理聊天事件
    if (opcode > MSG_CHAT_BEGIN && opcode < MSG_CHAT_END)
    {
        SERVER->GetChatManager()->HandleEvent(this,opcode,SocketHandle,pHead,Packet);
    }
    
    //处理队伍事件
    if (opcode > MSG_TEAM_BEGIN && opcode < MSG_TEAM_END)
    {
        SERVER->GetTeamManager()->HandleEvent(this,opcode,SocketHandle, pHead,Packet);
    }

    //处理副本
    if (opcode > MSG_COPYMAP_BEGIN && opcode < MSG_COPYMAP_END)
    {
        SERVER->GetCopyMapManager()->HandleEvent(this,opcode,SocketHandle, pHead,Packet);
    }

    //处理邮件
    if (opcode > MSG_MAIL_BEGIN && opcode < MSG_MAIL_END)
    {
        SERVER->GetMailManager()->HandleEvent(this,opcode,SocketHandle, pHead,Packet);
    }

	//处理好友
	if( opcode > MSG_SOCIAL_BEGIN && opcode < MSG_SOCIAL_END )
	{
		SERVER->GetSocialManager()->HandleEvent( this, opcode, SocketHandle, pHead, Packet );
	}

	//处理排行榜
	if( opcode > MSG_TOP_BEGIN && opcode < MSG_TOP_END )
	{
		SERVER->GetTopManager()->HandleEvent( this, opcode, SocketHandle, pHead, Packet );
	}
}

/************************************************************************/
/* 玩家正在切换地图，特殊处理一些消息
/************************************************************************/
void stAccountInfo::__OnSwitchMapStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet)
{
    switch(opcode)
    {
    case GATE_WORLD_ClientLost:
        {
            int nPlayerId = SERVER->GetPlayerManager()->GetSelectedPlayer(AccountId);

            if( nPlayerId == 0 )
                return;

            stEventArg arg;
            arg.pParam = (void*)nPlayerId;
            arg.nType  = EVENT_PLAYER_OFFLINE;

            // 通知所有监听此事件的对象
            SERVER->GetPlayerEventSource()->RaiseEvent( arg );

            //30秒后帐号将被删除
            m_status.SetState(STATUS_DELAY_DEL,PLAYER_TIMEOUT);
        }
        break;

	
    case ZONE_WORLD_SwitchMapResponse:
        {
            T_UID UID    = Packet->readInt(UID_BITS);
            int PlayerId = Packet->readInt(32);
            int LineId	 = Packet->readInt(16);
            int ZoneId	 = Packet->readInt(16);
            int ErrorCode= Packet->readInt(16);

            int ZoneSocketHandle = 0;

			DO_LOCK( Lock::Player | Lock::Team );

            {
				g_Log.WriteLog( "切换地图响应[%d]-[%d]", pHead->LineServerId, pHead->DestZoneId );

                stServerInfo *pServer = SERVER->GetServerManager()->GetGateServerByZoneId(pHead->LineServerId,pHead->DestZoneId);
                if(pServer)
                    ZoneSocketHandle = pServer->SocketId;
            }



            char Buffer[128];
            Base::BitStream SendPacket(Buffer,128);
            stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,ZONE_WORLD_SwitchMapResponse,pHead->Id,SERVICE_ZONESERVER,pHead->DestZoneId);
            SendPacket.writeInt(UID,UID_BITS);
            SendPacket.writeInt(PlayerId,32);
            SendPacket.writeInt(LineId,16);
            SendPacket.writeInt(ZoneId,16);
            SendPacket.writeInt(ErrorCode,16);

            if(ErrorCode == NONE_ERROR)
            {
                int Ip,Port;
                if(SendPacket.writeFlag(SERVER->GetServerManager()->GetZoneIpPort(LineId,ZoneId,Ip,Port)))
                {
                    SendPacket.writeInt(Ip,Base::Bit32);
                    SendPacket.writeInt(Port,Base::Bit16);
                }
            }

            pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
            SERVER->GetServerSocket()->Send(ZoneSocketHandle,SendPacket);

			m_status.SetState(STATUS_IN_GAME);
        }
        break;
    case CLIENT_GAME_LoginRequest:
        {
            m_status.SetState(STATUS_LOGIN_GAME,60);
        }
        break;
    case GATE_WORLD_ClientLoginRequest:
        { 
            this->socketId = SocketHandle;

            m_status.SetRawState(STATUS_LOGIN_ACCOUNT);
            m_status.Trigger(opcode,SocketHandle,pHead,Packet);
        }
        break;
    case ACCOUNT_WORLD_ClientLoginRequest:
        {
            //客户端从选择界面推出后，是没有任何消息的，直接发送登录的消息
            m_status.SetRawState(STATUS_IDEL);
            m_status.Trigger(opcode,SocketHandle,pHead,Packet);
        }
        break;
    }
}

void stAccountInfo::__OnDelayStatusHandler(int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet)
{
    switch(opcode)
    {
    case ACCOUNT_WORLD_ClientLoginRequest:
        {
			accountSocketId = SocketHandle;
            m_status.SetRawState(STATUS_IDEL);
            m_status.Trigger(opcode,SocketHandle,pHead,Packet);
        }
        break;
	case GATE_WORLD_ClientLoginRequest:
		{
			// 切换到上一个状态
			socketId = SocketHandle;
			m_status.SetRawState( m_status.GetPreState() );
			m_status.Trigger( opcode, SocketHandle, pHead, Packet );
		}
		break;
    }
}

void stAccountInfo::__OnEnterLoginGameHandler(void)
{
    T_UID UID     = m_curPacket->readInt(UID_BITS);
    int PlayerId  = m_curPacket->readInt(Base::Bit32);
    bool isFirst  = m_curPacket->readFlag();
    bool isSwitch = m_curPacket->readFlag();

    m_t_PlayerId = PlayerId;
    m_t_isSwitch = isSwitch;

	DO_LOCK( Lock::Player );

    // 判断玩家是否已经在线
    stAccountInfo* pAccountInfo = SERVER->GetPlayerManager()->GetAccountMap( AccountId );
    PlayerDataRef pPlayerData   = SERVER->GetPlayerManager()->GetPlayerData( PlayerId );

    // 通知聊天管理器,从此玩家订阅的所有频道中,删除掉这个玩家
    SERVER->GetChatManager()->GetChannelManager()->ClearAccount( AccountId );

    // 测试用---------------------------------
    // 有玩家加入就把他放到默认的那个测试频道中
    SERVER->GetChatManager()->GetChannelManager()->AddPlayer( PlayerId, SERVER->nTestChannel );

	// 增加清除挂起列表
	SERVER->GetTeamManager()->ClearPendingList( PlayerId );
    SERVER->GetTeamManager()->addPlayerToTeamChannel(PlayerId);
    SERVER->GetTeamManager()->SendTeamInfo(PlayerId);

    if( !pPlayerData.isNull() )
    {
		// 判断是否之前在副本中
		int nCopyMapInstId = SERVER->GetCopyMapManager()->GetPlayerCopyMapId( PlayerId );

		if( nCopyMapInstId == 0 )
			pPlayerData->DispData.CopyMapInstId = 1;

        if( !__SendWorldZoneTickPlayerRequest(LineId, pPlayerData, PlayerId) )
		{
			m_status.SetState( STATUS_IN_SELECT );
		}
    }
    else
    {
        //对于地图切换，只需直接发送给客户端resp
        if (isSwitch)
        {
            __SendClientGameLoginResponse(0,socketId, UID, LineId, AccountId, PlayerId);
            m_status.SetState(STATUS_IN_GAME);
        }
        else
        {
            if (!__SendZoneGameLogin(m_curSocketHandle, UID, LineId, AccountId, PlayerId,true))
            {
                m_status.SetState(STATUS_IN_SELECT);
            }
        }
    }
}

void stAccountInfo::__OnEnterDelayDelHandler(void)
{
	DO_LOCK( Lock::Player );

    CDBOperator db(DATABASE_ACCOUNT);
    
	int lastTime = (int)time(0) - LastTime;

	// 增加记录防沉迷系统的累计在线时间
    db->SQL("UPDATE baseinfo SET logoutTime = getdate(),TotalOnlineTime=TotalOnlineTime + %d, dTotalOnlineTime = %d WHERE AccountId = %d",lastTime, dTotalOnlineTime + lastTime, AccountId);
    db->Exec();

    //记录玩家退出游戏的时间
    if (m_status.GetPreState() != STATUS_MAP_SWITCH && CurPlayerPos >= 0 && CurPlayerPos < MAX_ACTOR_COUNT)
    {
		PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->loadPlayer( PlayerIdList[CurPlayerPos] );
		if( !pPlayerData.isNull() )
		{
			int playerId = pPlayerData->BaseData.PlayerId;

			pPlayerData->BaseData.lastLogoutTime = (int)timeGetTime();

			CDBOperator dbHandler(DATABASE);
			dbHandler->SQL("UPDATE PlayerInfo SET lastLogout=getdate() WHERE PlayerId=%d AND AccountId=%d",playerId,AccountId);
			dbHandler->Exec();
		}
    }

    //TODO log
    /*if (0 != pAccountInfo)
    {
    lg_account_logout log;
    log.accountName = pAccountInfo->AccountName;
    log.time        = time(0);
    LOG(log);
    }*/

    // 清理玩家帐号
    //SERVER->GetPlayerManager()->RemoveAccount( AccountId );
	//	m_status.SetState(STATUS_IDEL);
}

void stAccountInfo::__OnEnterGameHandler(void)
{
	DO_LOCK( Lock::Player );

    //记录玩家进游戏的时间
    if (m_status.GetPreState() != STATUS_MAP_SWITCH && CurPlayerPos >= 0 && CurPlayerPos < MAX_ACTOR_COUNT)
    {
        PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->loadPlayer( PlayerIdList[CurPlayerPos] );
        int playerId = pPlayerData->BaseData.PlayerId;

		CDBOperator db(DATABASE_ACCOUNT);
		db->SQL("SELECT PointNum FROM BaseInfo WHERE AccountId = %d", pPlayerData->BaseData.AccountId );
		if( db->More() )
		{
			pPlayerData->BaseData.PointNum = db->GetInt();
		}

		pPlayerData->BaseData.lastLoginTime = (int)timeGetTime();
   
        CDBOperator dbHandler(DATABASE);
        dbHandler->SQL("UPDATE PlayerInfo SET lastLogin=getdate() WHERE PlayerId=%d AND AccountId=%d",playerId,AccountId);
        dbHandler->Exec();

        //记录角色登录
        lg_char_login lg;

        lg.playerName  = pPlayerData->BaseData.PlayerName;
        lg.accountName = AccountName;
        lg.ip          = loginIP;
        lg.mapId       = pPlayerData->DispData.ZoneId;

        SERVER->GetLog()->writeLog(&lg);
    } 
} 

void stAccountInfo::__OnDelayStatusExpire(void)
{
    m_status.SetState(STATUS_IDEL);
} 

void stAccountInfo::__OnLoginAccountExpire(void)
{
    m_status.SetState(STATUS_DELAY_DEL,PLAYER_TIMEOUT);
}

/************************************************************************/
/* 发送登录返回消息给客户端
/************************************************************************/
void stAccountInfo::__SendClientLoginAccountResponse(int Error, int SocketHandle)
{
    CMemGuard Buffer(MAX_PACKET_SIZE);
    Base::BitStream SendPacket(Buffer.get(),MAX_PACKET_SIZE);
    stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,ACCOUNT_WORLD_ClientLoginResponse );

    SendPacket.writeInt(UID,UID_BITS);
    SendPacket.writeInt(AccountId,Base::Bit32);
    SendPacket.writeInt(Error,Base::Bit16);

    if(Error == NONE_ERROR)
    {
        //将服务器分线列表发给玩家
        SERVER->GetServerManager()->PackGateData(SendPacket);
    }

    pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
    SERVER->GetServerSocket()->Send(SocketHandle,SendPacket);

	g_Log.WriteLog( "向客户端发送帐号登陆响应[%d]", AccountId );
}

/************************************************************************/
/* 从zone服务器踢除玩家
/************************************************************************/
bool stAccountInfo::__SendWorldZoneTickPlayerRequest(int LineId, PlayerDataRef pPlayerData, int PlayerId, bool bTickNow)
{
	DO_LOCK( Lock::Server );

    if (pPlayerData.isNull())
        return false;

	int ZoneId = pPlayerData->DispData.ZoneId;

	if( pPlayerData->DispData.CopyMapInstId != 1 )
		ZoneId = pPlayerData->DispData.fZoneId;

    stServerInfo* pServerInfo = SERVER->GetServerManager()->GetGateServerByZoneId(this->LineId,ZoneId);

    if (0 == pServerInfo)
        return false;

    CMemGuard buf(40);
    Base::BitStream sendPacket( buf.get(),40);

    stPacketHead* pSendHead = IPacket::BuildPacketHead( sendPacket, WORLD_ZONE_TickPlayerRequest, PlayerId, SERVICE_ZONESERVER );
    pSendHead->DestZoneId = pPlayerData->DispData.ZoneId;
    pSendHead->SrcZoneId  = pPlayerData->BaseData.AccountId;
    sendPacket.writeFlag( bTickNow );
    pSendHead->PacketSize = sendPacket.getPosition() - sizeof( stPacketHead );

    SERVER->GetServerSocket()->Send( pServerInfo->SocketId, sendPacket);

	g_Log.WriteLog( "向发送发送踢人请求[%d],[%d],[%d]", pPlayerData->BaseData.PlayerId, pPlayerData->DispData.ZoneId, this->LineId );
    return true;
}

/************************************************************************/
/* 向zone先传递数据在登录前
/************************************************************************/
bool stAccountInfo::__SendZoneGameLogin( int SocketHandle, T_UID UID, int LineId, int AccountId, int PlayerId, bool isFirst)
{
	DO_LOCK( Lock::Player | Lock::Server );

    int Error  = SERVER->GetPlayerManager()->AddPlayerMap(SocketHandle,UID,LineId,AccountId,PlayerId,isFirst);

	if( Error != NONE_ERROR )
	{
		__SendClientGameLoginResponse(Error,socketId,UID,LineId,AccountId,PlayerId);

		return false;
	}

    //判断角色是否被冻结
    PlayerDataRef pPlayerData = SERVER->GetPlayerManager()->GetPlayerData( PlayerId );

	if( pPlayerData.isNull())
	{
		Error = UNKNOW_ERROR;

		__SendClientGameLoginResponse(Error,SocketHandle,UID,LineId,AccountId,PlayerId);
		
		return false;
	}

	int ZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);

    if (!pPlayerData.isNull())
    {
        if (CBanlist::Instance()->IsActorLoginBan(pPlayerData->BaseData.PlayerName))
		{
            Error = ERROR_FREEZE;
			
			__SendClientGameLoginResponse(Error,SocketHandle,UID,LineId,AccountId,PlayerId);

			return false;
		}
    }


	int SocketId = SERVER->GetServerManager()->GetZoneSocket( this->LineId, ZoneId );

	if( SocketId == 0 )
	{
		Error = UNKNOW_ERROR;

		__SendClientGameLoginResponse(Error,SocketHandle,UID,LineId,AccountId,PlayerId);

		return false;
	}
	
    if(Error == NONE_ERROR)
    {
        // 设置玩家上线状态
        SERVER->GetSocialManager()->onPlayerEnter( PlayerId );

		pPlayerData->BaseData.lastLoginTime = _time32( NULL );
		pPlayerData->BaseData.currentTime = 0;  // 清空角色本次在线时间

        CMemGuard Buffer(MAX_PACKET_SIZE);
        Base::BitStream SendPacket(Buffer.get(),MAX_PACKET_SIZE);
        stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,WORLD_ZONE_PlayerData,PlayerId,SERVICE_ZONESERVER,ZoneId);
        SendPacket.writeInt(UID,UID_BITS);
        pSendHead->DestZoneId = SERVER->GetPlayerManager()->PackPlayerData(PlayerId,SendPacket);

        // 传送其他数据
        stTeamInfo teamInfo;
        SERVER->GetTeamManager()->GetPlayerTeamInfo( PlayerId, teamInfo );
        teamInfo.WritePacket( &SendPacket );

        pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();

        if(SocketId)
            SERVER->GetServerSocket()->Send(SocketId,SendPacket);

		g_Log.WriteLog( "向地图传送玩家数据[%d],[%d],[%d]", ZoneId, PlayerId, LineId );
    }
    else
    {
        __SendClientGameLoginResponse(Error,SocketHandle,UID,LineId,AccountId,PlayerId);
    }

    return Error == NONE_ERROR;
}

void stAccountInfo::__SendClientGameLoginResponse(int Error,int SocketHandle, T_UID UID, int LineId, int AccountId, int PlayerId)
{
    int ZoneId = 0;

	SERVER->GetTeamManager()->SendTeamInfo( PlayerId );

    CMemGuard Buffer(64); 
    Base::BitStream SendPacket(Buffer.get(),64);
    stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,CLIENT_GAME_LoginResponse,AccountId,SERVICE_CLIENT);
    SendPacket.writeInt(Error,Base::Bit16);

    if(Error == NONE_ERROR)
    {
        ZoneId = SERVER->GetPlayerManager()->GetPlayerZoneId(PlayerId);

        if(SendPacket.writeFlag(ZoneId!=0))
        {
            int Ip,Port;

            if(SendPacket.writeFlag(SERVER->GetServerManager()->GetZoneIpPort(LineId,ZoneId,Ip,Port)))
            {
                SendPacket.writeInt(Ip,Base::Bit32);
                SendPacket.writeInt(Port,Base::Bit16);
            }
            else
            {
                SERVER->GetPlayerManager()->ResetPlayerStatus(UID,AccountId);
            }
        }
    }

    pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
    SERVER->GetServerSocket()->Send(this->socketId,SendPacket);

	g_Log.WriteLog( "向玩家发送游戏登陆登陆响应[%d],[%d]", PlayerId, LineId );
}

void stAccountInfo::__SendClientLogout(int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet)
{
    T_UID UID    = Packet->readInt(UID_BITS);
    int PlayerId = Packet->readInt(Base::Bit32);

    CMemGuard Buffer(64);
    Base::BitStream SendPacket(Buffer.get(),64);
    stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,CLIENT_GAME_LogoutResponse,AccountId,SERVICE_CLIENT);
    SendPacket.writeInt(UID		,UID_BITS);
    SendPacket.writeInt(PlayerId, Base::Bit32);
    pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
    SERVER->GetServerSocket()->Send(socketId,SendPacket);
}

void stAccountInfo::__OnPlayerLeaveGame(void)
{
    int PlayerId = SERVER->GetPlayerManager()->GetSelectedPlayer(AccountId);

    if( PlayerId == 0 )
        return;

    PlayerDataRef pPlayer = SERVER->GetPlayerManager()->loadPlayer(PlayerId);

    if (!pPlayer.isNull())
    {
        //记录角色登录
        lg_char_logout lg;

        lg.playerName  = pPlayer->BaseData.PlayerName;
        lg.accountName = AccountName;
        lg.ip          = loginIP;
        lg.mapId       = pPlayer->DispData.ZoneId;

        SERVER->GetLog()->writeLog(&lg);

		pPlayer->BaseData.lastLogoutTime = _time32( NULL );
    }

    ClearPlayer(PlayerId);
}

void stAccountInfo::ClearPlayer( int PlayerId )
{
	// 设置玩家离线状态
	SERVER->GetSocialManager()->onPlayerQuit( PlayerId );

	// 通知聊天管理器,从此玩家订阅的所有频道中,删除掉这个玩家
	SERVER->GetChatManager()->GetChannelManager()->ClearAccount( AccountId );

	// 通知副本管理器，把此玩家清除掉
	SERVER->GetCopyMapManager()->LeaveCopyMap( PlayerId );

	DO_LOCK( Lock::Team );

	// 通知队伍管理器
	CTeam* pTeam;
	SERVER->GetTeamManager()->ClearPendingList( PlayerId );
	pTeam = SERVER->GetTeamManager()->GetTeamByPlayer( PlayerId );

	if( pTeam)
	{
		if( pTeam->GetTeammate()->size() == 1 )
		{	 
			SERVER->GetTeamManager()->DisbandTeamByPlayer( PlayerId );
		}
		else
		{
			std::list<int>::iterator it;
			for( it = pTeam->m_teammate.begin(); it != pTeam->m_teammate.end(); it++ )
			{
				SERVER->GetTeamManager()->NotifyPlayerLeave( *it, PlayerId );
			}

			SERVER->GetTeamManager()->NotifyZonePlayerLeave( LineId, PlayerId, pTeam->GetId() );
			SERVER->GetTeamManager()->LeaveTeam( PlayerId, pTeam->GetId() );
		}
	}
}

void stAccountInfo::__OnIdCodeExpire( void )
{

}

void stAccountInfo::__OnIdCodeHandler( int opcode,int SocketHandle,stPacketHead* pHead,Base::BitStream* Packet )
{
	// 如果遇到其他消息，马上删除这个玩家，玩家在CHEAT
	if( pHead->Message != CLIENT_WORLD_IdCodeRequest )
		 m_status.SetState( STATUS_IDEL );

	char text[COMMON_STRING_LENGTH];

	Packet->readString( text ,COMMON_STRING_LENGTH);
	
	if( strcmp( text, mIdCode ) != 0 )
	{
		// 验证码失败
	}

	
}

//---------------------------------------------------------------
// 生成验证码
//---------------------------------------------------------------
void stAccountInfo::genIdCode()
{
	
}