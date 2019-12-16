#include "base/SafeString.h"
#include "DBLib/dbLib.h"
#include "Common/Script.h"
#include "Common/MemGuard.h"
#include "Common/WorkQueue.h"
#include "Common/PacketType.h"
#include "WinTCP/IPacket.h"
#include "WinTCP/dtServerSocket.h"
#include "Common/CommonClient.h"
#include "AccountServer.h"
#include "EventProcess.h"
#include "PlayerMgr.h"
#include "Common/PlayerStruct.h"

using namespace std;

typedef bool (*EventFunction)(int ,stPacketHead *,Base::BitStream *);
static EventFunction fnList[END_NET_MESSAGE];

int SERVER_CLASS_NAME::EventProcess(LPVOID Param)
{
	WorkQueueItemStruct *pItem = (WorkQueueItemStruct*)Param;

	switch(pItem->opCode)
	{
	case WQ_CONNECT:
		break;
	case WQ_DISCONNECT:
		SERVER->GetPlayerManager()->ReleaseSocketMap(pItem->Id, true);
		break;
	case WQ_PACKET:
		{
			stPacketHead* pHead = (stPacketHead*)pItem->Buffer;
			char *pData	= (char *)(pHead) + IPacket::GetHeadSize();
			Base::BitStream RecvPacket(pData,pItem->size-IPacket::GetHeadSize());

			if(!IsValidMessage(pHead->Message))
				return false;

			if(fnList[pHead->Message])
				return fnList[pHead->Message](pItem->Id,pHead,&RecvPacket);
		}
		break;
	}

	return false;
}

namespace EventFn
{
	void Initialize()
	{
		memset(fnList,NULL,sizeof(fnList));
		fnList[COMMON_RegisterResponse]					= EventFn::HandleRegisterResponse;
		fnList[CLIENT_ACCOUNT_LoginRequest]				= EventFn::HandleClientLoginRequest;
		fnList[ACCOUNT_WORLD_ClientLoginResponse]		= EventFn::HandleWorldClientLoginResponse;
		fnList[CLIENT_ACCOUNT_CheckRequest]				= EventFn::HandleClientCheckRequest;
	}
	
	bool SwitchSendToWorld(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		Base::BitStream retPacket(pHead,IPacket::GetTotalSize((const char *)pHead));
		retPacket.setPosition(IPacket::GetTotalSize((const char *)pHead));
		SERVER->GetWorldSocket()->Send(retPacket);
		return true;
	}

	bool HandleRegisterResponse(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		//来自WorldServer对自己注册的反馈
		SERVER->OnServerStart();
		return true;
	}

	bool HandleWorldClientLoginResponse(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		int UID = Packet->readInt(Base::Bit32);
		int AccountId = Packet->readInt(Base::Bit32);
		int Error = Packet->readInt(Base::Bit16);

		{
			CLocker lock(SERVER->GetPlayerManager()->m_cs);

			stAccountInfo *ppInfo = SERVER->GetPlayerManager()->GetAccountMap(AccountId);
			if(ppInfo && ppInfo->UID == UID)
			{
				CMemGuard Buffer(MAX_PACKET_SIZE);
				Base::BitStream SendPacket(Buffer.get(),MAX_PACKET_SIZE);                         
				stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,CLIENT_ACCOUNT_LoginResponse);
				SendPacket.writeInt(Error,Base::Bit16);
				if(Error == NONE_ERROR)
				{
					SendPacket.writeInt(AccountId,Base::Bit32);
					SendPacket.writeInt(UID,UID_BITS);
					SendPacket.copyFrom(Packet);
				}
				pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
				SERVER->GetServerSocket()->Send(ppInfo->socketId,SendPacket);

                //TODO log
                //lg_account_login log;
                //log.accountName = ppInfo->AccountName;
                //stSocketInfo* pSocketInfo = SERVER->GetPlayerManager()->GetSocketMap(ppInfo->socketId);
                //log.ip          = pSocketInfo ? inet_ntoa((in_addr)pSocketInfo->LoginIP) : "";
                //log.time        = time(0);
                //LOG(log);

                //设置玩家的登入时间信息
                CDBOperator db(DATABASE);
                stSocketInfo* pSocketInfo = SERVER->GetPlayerManager()->GetSocketMap(ppInfo->socketId);
                db->SQL("UPDATE baseinfo SET loginTime = getdate(),loginIP='%s',loginError=%d WHERE AccountId = %d",pSocketInfo ? inet_ntoa(*((in_addr*)&pSocketInfo->LoginIP)) : "",Error,AccountId);
                db->Exec();
			}
		}

		return true;
	}

	bool HandleClientLoginRequest(int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet)
	{
		short Error = NONE_ERROR;
		char AccountName[COMMON_STRING_LENGTH],Password[MD5_STRING_LENGTH],PasswordHex[128]="";

		int NetVersion = Packet->readInt(Base::Bit16);
		if(NetVersion != END_NET_MESSAGE)
		{
			Error = VERSION_ERROR;
		}
		else
		{
			Packet->readString(AccountName,COMMON_STRING_LENGTH);
			strupr(AccountName);
			Packet->readBits(MD5_STRING_LENGTH*Base::Bit8 , Password);
			ConvertHex(PasswordHex,(const unsigned char*)Password,MD5_STRING_LENGTH);

			int AccountId = 2;
			char Password2[MD5_STRING_LENGTH],Password3[MD5_STRING_LENGTH];

			//先去登录平台验证并获取回信息，包括AccountId,Password1-3等
			//如果成功验证了，再到本地数据库获取相关信息
			int bIsGM  = 0;
            int status = 0;
            std::string ip;
			int isAdult = 0;
			int dTotalOnlineTime = 0;
			int dTotalOfflineTime = 0;
			int dLastLoginTime = 0;
			int dLastLogoutTime = 0;
			int pointNum = 0;

			if(Error == NONE_ERROR)
			{
				CDBOperator dbHandle(DATABASE);
				dbHandle->SQL("select AccountId,GMFlag,Status, dTotalOnlineTime, dTotalOfflineTime, LoginTime, LogoutTime, isAdult, PointNum from baseinfo where AccountName='%s'",AccountName);		//查询激活区的帐号相关信息
				try
				{
					if(dbHandle->More())
					{
						int LocalAccountId = dbHandle->GetInt();
						bIsGM              = dbHandle->GetInt();
                        status             = dbHandle->GetInt();
						
						dTotalOnlineTime = dbHandle->GetInt();
						dTotalOfflineTime = dbHandle->GetInt();
						dLastLoginTime = dbHandle->GetTime();
						dLastLogoutTime = dbHandle->GetTime();

						isAdult = dbHandle->GetInt();
						pointNum = dbHandle->GetInt();
						
						//if(LocalAccountId!=AccountId)
						//	Error = UNKNOW_ERROR;		//怎么会本地ID和全局ID不同呢？

						AccountId = LocalAccountId;		//调试版本将本地ID作为全局ID
					}
					else
						Error = ACCOUNT_UNACTIVE;
				}
				DB_CATCH_LOG(HandleClientLoginRequest);

				if(Error == NONE_ERROR)
				{
					CLocker lock(SERVER->GetPlayerManager()->m_cs);

					stAccountInfo *ppInfo = SERVER->GetPlayerManager()->GetAccountMap(AccountId);
					if(ppInfo)
					{
						SERVER->GetPlayerManager()->ReleaseAccountMap(AccountId);
					}

					ppInfo = SERVER->GetPlayerManager()->AddAccountMap(SocketHandle,AccountId);

                    stSocketInfo* pSocketInfo = SERVER->GetPlayerManager()->GetSocketMap(SocketHandle);

                    if (0 != pSocketInfo)
                    {
                        ip = inet_ntoa(*((in_addr*)&pSocketInfo->LoginIP));
                    }

					assert(ppInfo);
					sStrcpy(ppInfo->AccountName,COMMON_STRING_LENGTH,AccountName,COMMON_STRING_LENGTH);
					memcpy(ppInfo->Password1,Password ,MD5_STRING_LENGTH);
					memcpy(ppInfo->Password2,Password2,MD5_STRING_LENGTH);
					memcpy(ppInfo->Password3,Password3,MD5_STRING_LENGTH);

					CMemGuard Buffer(256);
					Base::BitStream SendPacket(Buffer.get(),256);
					stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,ACCOUNT_WORLD_ClientLoginRequest);
					SendPacket.writeInt(ppInfo->UID,UID_BITS);
					SendPacket.writeInt(ppInfo->AccountId,Base::Bit32);
					SendPacket.writeInt(bIsGM,Base::Bit32);
                    SendPacket.writeInt(status,Base::Bit32);

					// 增加读取防沉迷数据
					SendPacket.writeFlag( isAdult );
					SendPacket.writeInt( dTotalOnlineTime, 32 );
					SendPacket.writeInt( dTotalOfflineTime, 32 );
					SendPacket.writeInt( dLastLoginTime, 32 );
					SendPacket.writeInt( dLastLogoutTime, 32 );
					SendPacket.writeInt( pointNum, 32 );
                    
					SendPacket.writeString(ip.c_str());
                    SendPacket.writeString(ppInfo->AccountName);
					SendPacket.writeBits(MD5_STRING_LENGTH*Base::Bit8,ppInfo->Password1);
					SendPacket.writeBits(MD5_STRING_LENGTH*Base::Bit8,ppInfo->Password2);
					SendPacket.writeBits(MD5_STRING_LENGTH*Base::Bit8,ppInfo->Password3);
					pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
					SERVER->GetWorldSocket()->Send(SendPacket);
				}
			}
		}

		if(Error!=NONE_ERROR)
		{
			CMemGuard Buffer(64);
			Base::BitStream SendPacket(Buffer.get(),64);
			stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,CLIENT_ACCOUNT_LoginResponse);
			SendPacket.writeInt(Error,Base::Bit16);
			pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
			SERVER->GetServerSocket()->Send(SocketHandle,SendPacket);
		}

		return true;
	}

	bool HandleClientCheckRequest( int SocketHandle,stPacketHead *pHead,Base::BitStream *Packet )
	{
		CMemGuard Buffer(64);
		Base::BitStream SendPacket(Buffer.get(),64);
		stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,CLIENT_ACCOUNT_CheckResponse);
		SendPacket.writeInt(0,Base::Bit16);
		pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
		SERVER->GetServerSocket()->Send(SocketHandle,SendPacket);

		return true;
	}
}


//反馈消息给客户端，让其接入指定的网管
//CMemGuard Buffer(50);
//Base::BitStream SendPacket(Buffer.get(),50);
//stPacketHead *pSendHead = IPacket::BuildPacketHead(SendPacket,CLIENT_ACCOUNT_LoginResponse);
//SendPacket.writeInt((short)Error,Base::Bit16);
//pSendHead->PacketSize = SendPacket.getPosition()-IPacket::GetHeadSize();
//SERVER->GetServerSocket()->Send(SocketHandle,SendPacket);




