#include <WinSock2.h>
#include "windows.h"
#include "platform/platform.h"
#include "core/bitStream.h"
#include "core/dnet.h"
#include "console/simBase.h"
#include "sim/netConnection.h"
#include "GamePlay/GameEvents/GameNetEvents.h"
#include "Gameplay/ServerGameplayState.h"
#include "T3D/gameConnection.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "GamePlay/Mission/PlayerMission.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/item/Res.h"
#include "Gameplay/Item/DropItem.h"
#include "Gameplay/item/PrizeBox.h"
#include "Gameplay/GameObjects/CollectionObject.h"
#include "Gameplay/item/NpcShopData.h"
#include "Gameplay/GameObjects/LivingSkillShortcut.h"
#include "Gameplay/Item/PetShortcut.h"
#include "Gameplay/item/Prescription.h"
#include "Gameplay/GameObjects/MPManager.h"
#include "Gameplay/Data/SuperMarketData.h"

#ifdef NTJ_CLIENT
#include "ui/dGuiMouseGamePlay.h"
#include "Gameplay/ClientGameplayState.h"
#include "Gameplay/loadmgr/ObjectLoadManager.h"
#include "gameplay/gameevents/ClientGameNetEvents.h"
#include "Util/LocalString.h"
#include "Effects/ScreenFX.h"
#endif

GameplayEvent::GameplayEvent(U32 info_type):
m_InfoType(info_type),m_IntArgCount(0),m_Int32ArgCount(0),
m_StringArgCount(0), m_nBlockSize(0)
{
	ZeroMemory(m_IntArgValue,sizeof(m_IntArgValue));

	ZeroMemory(m_Int32ArgValue,sizeof(m_Int32ArgValue));

	ZeroMemory(m_StringArgValue,sizeof(m_StringArgValue));

	ZeroMemory(m_Block, sizeof(m_Block));
}

//设置连续的内存块
void GameplayEvent::AddMemoryBlock(char *block, int nBlockSize)
{
	if (!block || nBlockSize == 0)
		return;

	AssertFatal( (m_nBlockSize + nBlockSize) <= MemoryBlockSize, "GameplayEvent::SetMemoryBlock: nBlockSize > MemoryBlockSize");

	dMemcpy(m_Block + m_nBlockSize, block, nBlockSize);
	m_nBlockSize += nBlockSize;
}

//设置16位的整数参数
void GameplayEvent::SetIntArgValues(S32 argc, ...)
{
	AssertRelease(argc<= MAX_INT_ARGS,"错误! 16位整数参数太多，超过64个!" );

	va_list args;

	va_start(args,argc);

	for(S32 i =0  ; i < argc ; ++ i )
	{
		m_IntArgValue[i] = 	va_arg(args, S32);
	}

	va_end(args);

	m_IntArgCount = argc;
}
// 设置32位的整数参数
void GameplayEvent::SetInt32ArgValues(S32 argc, ...)
{
	AssertRelease(argc<= MAX_INT32_ARGS,"错误! 32位整数参数太多，超过64个!" );

	va_list args;

	va_start(args,argc);

	for(S32 i =0 ; i < argc ; ++ i )
	{
		m_Int32ArgValue[i] = 	va_arg(args, S32);
	}

	va_end(args);

	m_Int32ArgCount = argc;
}
// 设置字符串参数
void GameplayEvent::SetStringArgValues(S32 argc, ...)
{
	AssertRelease(argc<= MAX_STRING_ARGS,"错误! 错误! 字符串参数太多，超过8个!" );

	va_list args;
	va_start(args, argc);

	for(S32 i = 0; i < argc; i++)
	{
		ZeroMemory(m_StringArgValue[i],sizeof(m_StringArgValue[i]));
		dSprintf(m_StringArgValue[i], sizeof(m_StringArgValue[i]), va_arg(args, char*), StringArgValueBits);
	}

	va_end(args);

	m_StringArgCount = argc;

}
//
void GameplayEvent::unpack(NetConnection *pCon, BitStream *bstream) 
{
//#ifdef  NTJ_SERVER
	m_InfoType			= bstream->readInt(InfoTypeBits);
	//
	m_IntArgCount		= bstream->readInt(IntArgCountBits);

	for(S32 i = 0; i < m_IntArgCount; i++)
	{
		m_IntArgValue[i] = bstream->readSignedInt(IntArgValueBits);
	}

	m_Int32ArgCount		= bstream->readInt(Int32ArgCountBits);
	for(S32 i = 0; i < m_Int32ArgCount; i++)
	{
		m_Int32ArgValue[i] = bstream->readSignedInt(Int32ArgValueBits);
	}

	m_StringArgCount	= bstream->readInt(StringArgCountBits);
	for(S32 i = 0; i < m_StringArgCount; i++)
	{
		ZeroMemory(m_StringArgValue[i],sizeof(m_StringArgValue[i]));
		bstream->readString(m_StringArgValue[i],StringArgValueBits);
	}

	m_nBlockSize = bstream->readInt(32);
	if (m_nBlockSize > 0)
	{
		bstream->read(m_nBlockSize, m_Block);
	}
//#endif
}
//
void GameplayEvent::pack(NetConnection *pCon, BitStream *bstream)
{
//#ifdef NTJ_CLIENT
	bstream->writeInt(m_InfoType,InfoTypeBits);
	//
	bstream->writeInt(m_IntArgCount,IntArgCountBits);

	for(S32 i= 0 ; i < m_IntArgCount ; ++ i)
		bstream->writeSignedInt(m_IntArgValue[i],IntArgValueBits);

	bstream->writeInt(m_Int32ArgCount,Int32ArgCountBits);

	for(S32 i = 0 ; i < m_Int32ArgCount; ++ i)
		bstream->writeSignedInt(m_Int32ArgValue[i],Int32ArgValueBits);

	bstream->writeInt(m_StringArgCount,StringArgCountBits);

	for(S32 i = 0 ; i < m_StringArgCount; ++ i )
		bstream->writeString(m_StringArgValue[i],StringArgValueBits);

	bstream->writeInt(m_nBlockSize, 32);
	if (m_nBlockSize > 0)
		bstream->write(m_nBlockSize, m_Block);
//#endif
}

void GameDataNetEvent::pack(NetConnection *pCon, BitStream *bstream)		
{
	if(!mBuffer || mBufferSize==0)
		bstream->writeFlag(false);
	else
	{
		bstream->writeFlag(true);
		bstream->writeInt(mBufferSize,12);
		bstream->writeBits(mBufferSize<<3,mBuffer); 
	}
}

void GameDataNetEvent::prepare(NetConnection *pCon) //不允许改写
{
	BitStream *bstream = BitStream::getPacketStream();
	packData(pCon,bstream);
	mBufferSize = bstream->getPosition();
	mBuffer = new char[mBufferSize];
	memcpy(mBuffer,bstream->getBuffer(),mBufferSize);
}

void GameDataNetEvent::unpack(NetConnection *pCon, BitStream *bstream)	//不允许改写
{
	if(bstream->readFlag())
	{
		mBufferSize = bstream->readInt(12);
		mBuffer = new char[mBufferSize];
		bstream->readBits(mBufferSize<<3,mBuffer);
	}
}

void GameDataNetEvent::process(NetConnection *pCon)	//不允许改写
{
	if(mBufferSize && mBuffer)
	{
		BitStream temp(mBuffer, mBufferSize);
		temp.setPosition(0);
		unpackData(pCon,&temp);
	}
}


/////////////////////////////服务端发送给客户端的消息//////////////////////////////////////////////////////服务端发送给客户端的消息////////////////////////////////////////////////
/////////////////////////////服务端发送给客户端的消息//////////////////////////////////////////////////////服务端发送给客户端的消息////////////////////////////////////////////////
/////////////////////////////服务端发送给客户端的消息//////////////////////////////////////////////////////服务端发送给客户端的消息////////////////////////////////////////////////
/////////////////////////////服务端发送给客户端的消息//////////////////////////////////////////////////////服务端发送给客户端的消息////////////////////////////////////////////////

///////////////////////// 字符串消息 ///////////////////////// 
IMPLEMENT_CO_CLIENTEVENT_V1(MessageEvent);

MessageEvent::MessageEvent()
{
	mMsgType	= SHOWTYPE_ERROR;
	mMsgID		= MSG_NONE;
	mMsgStr[0]	= '\0';
	mShowPos	= SHOWPOS_SCREEN;
}

// ----------------------------------------------------------------------------
// 设置屏显内容和显示模式
void MessageEvent::setMessage(U32 MsgType, U32 MsgID, U32 ShowPos)
{
	mMsgType	= MsgType;
	mMsgID		= MsgID;
	mMsgStr[0]	= '\0';
	mShowPos	= ShowPos;
}

// ----------------------------------------------------------------------------
// 设置屏显内容和显示模式
void MessageEvent::setMessage(U32 MsgType, const char* MsgStr, U32 ShowPos)
{
	AssertFatal(dStrlen(MsgStr) < 128, "用户自定义消息的字符串太长,无法发送!");
	dStrcpy(mMsgStr, sizeof(mMsgStr), MsgStr);
	mMsgID		= MSG_NONE;
	mMsgType	= MsgType;
	mShowPos	= ShowPos;
}

#ifdef NTJ_SERVER
void MessageEvent::send(NetConnection* conn, U32 MsgType, const char* MsgStr, U32 ShowPos /* = SHOWPOS_SCREEN */)
{
	MessageEvent *event = new MessageEvent();
	if (!event)
		return;

	event->setMessage(MsgType, MsgStr, ShowPos);
	if (conn)
		conn->postNetEvent(event);
}

void MessageEvent::send(NetConnection* conn, U32 MsgType, U32 MsgID, U32 ShowPos /* = SHOWPOS_SCREEN */)
{
	MessageEvent *event = new MessageEvent();
	if (!event)
		return;

	event->setMessage(MsgType, MsgID, ShowPos);
	if (conn)
		conn->postNetEvent(event);
}
#endif//NTJ_SERVER

#ifdef NTJ_CLIENT
void MessageEvent::show(U32 MsgType, const char* MsgStr, U32 ShowPos /* = SHOWPOS_SCREEN */)
{
	U32 color = g_Color[COLOR_NOTIFY];
	if(MsgType == SHOWTYPE_WARN)
		color = g_Color[COLOR_WARN];
	else if(MsgType == SHOWTYPE_ERROR)
		color = g_Color[COLOR_DANGER];

	if(ShowPos == SHOWPOS_SCREEN || ShowPos == SHOWPOS_SCRANDCHAT)
	{
		Con::executef("SetScreenMessage", MsgStr, Con::getIntArg(color));
	}

	if(ShowPos == SHOWPOS_CHAT || ShowPos == SHOWPOS_SCRANDCHAT)
	{
		Con::executef("SetChatMessage", MsgStr, Con::getIntArg(color));
	}
}

void MessageEvent::show(U32 MsgType, U32 MsgID, U32 ShowPos /* = SHOWPOS_SCREEN */)
{
	show(MsgType, GetLocalStr(MsgID), ShowPos);
}
#endif//NTJ_CLIENT

void MessageEvent::pack(NetConnection* conn, BitStream* stream)
{
#ifdef NTJ_SERVER
	// 屏显字符串
	stream->writeInt(mMsgID,	MSG_BIT16);
	if(mMsgID == MSG_NONE)
		stream->writeString(mMsgStr);

	// 显示消息类型
	stream->writeInt(mMsgType,	MSG_BIT4);
	
	// 显示屏幕位置
	stream->writeInt(mShowPos,	MSG_BIT4);
#endif
}

void MessageEvent::unpack(NetConnection* Conn, BitStream* stream) 
{
#ifdef  NTJ_CLIENT
	// 屏显字符串
	mMsgID		= stream->readInt(MSG_BIT16);
	if(mMsgID == MSG_NONE)
		stream->readString(mMsgStr,MSG_STRING_MAXLEN);

	// 显示消息类型
	mMsgType	= stream->readInt(MSG_BIT4);

	// 显示屏幕位置
	mShowPos	= stream->readInt(MSG_BIT4);
#endif
}

void MessageEvent::process(NetConnection *pCon)
{
#ifdef NTJ_CLIENT
	if(mMsgID != MSG_NONE)
		MessageEvent::show(mMsgType, mMsgID, mShowPos);
	else
		MessageEvent::show(mMsgType, mMsgStr, mShowPos);
#endif	
}

///////////////////////// 传送消息准备 //////////////////////////////
IMPLEMENT_CO_CLIENTEVENT_V1(TransportBeginEvent);

void TransportBeginEvent::unpack(NetConnection *pCon, BitStream *bstream) 
{
#ifdef  NTJ_CLIENT
	mIsSameMap = bstream->readFlag();
#endif
}

//
void TransportBeginEvent::pack(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_SERVER
	bstream->writeFlag(mIsSameMap);
#endif
}

//
void TransportBeginEvent::process(NetConnection *pCon)
{
#ifdef NTJ_CLIENT
	if(mIsSameMap)		//是内传送
	{
		//调出切换界面
	}
	else
	{
		//外传送，调取切换界面连接服务器
	}
#endif	
}

///////////////////////// 传送消息准备 //////////////////////////////
IMPLEMENT_CO_CLIENTEVENT_V1(TransportEvent);

void TransportEvent::unpack(NetConnection *pCon, BitStream *bstream) 
{
#ifdef  NTJ_CLIENT
	mErrorCode = bstream->readInt(16);
	if(mErrorCode==NONE_ERROR)
	{
		mIsInternal = bstream->readFlag();
		if(!mIsInternal)
		{
			mUID = bstream->readInt(UID_BITS);
			mPlayerId = bstream->readInt(32);
			mLineId = bstream->readInt(16);
			mZoneId = bstream->readInt(16);
			if(bstream->readFlag())
			{
				mIP = bstream->readInt(32);
				mPort = bstream->readInt(16);
			}
		}
	}
#endif
}

//
void TransportEvent::pack(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_SERVER
	bstream->writeInt(mErrorCode,16);
	if(mErrorCode==NONE_ERROR)
	{
		if(!bstream->writeFlag(mIsInternal))
		{
			bstream->writeInt(mUID,UID_BITS);
			bstream->writeInt(mPlayerId,32);
			bstream->writeInt(mLineId,16);
			bstream->writeInt(mZoneId,16);
			if(bstream->writeFlag(mIP && mPort))
			{
				bstream->writeInt(mIP,32);
				bstream->writeInt(mPort,16);
			}
		}
	}
#endif
}

//
void TransportEvent::process(NetConnection *pCon)
{
#ifdef NTJ_CLIENT
	bool isSwitchLine = false;

	if(!mIsInternal && g_ClientGameplayState->getCurrentLineId() != mLineId)
		isSwitchLine = true;

#ifdef SWITCH_LINE_DEBUG
	isSwitchLine = true;
#endif

	if(!isSwitchLine)
	{
		if(mErrorCode==NONE_ERROR)
		{
			g_ClientGameplayState->setTransporting();

			if(mIsInternal)
			{
				//内传送(固定快速显示进度条后自己取消传送标志)
				//schedule(g_ClientGameplayState->setTransporting());
				g_ClientGameplayState->setTransporting(false);
			}
			else
			{
				//外传送
				gClientObjectLoadMgr.clearResList();
				g_ClientGameplayState->setZoneIP(mIP);
				g_ClientGameplayState->setZonePort(mPort);
				g_ClientGameplayState->setCurrentZoneId(mZoneId);
				g_ClientGameplayState->setDisconnectZone(true);
			}
		}
		else
		{
			//传送失败
		}
	}
	else
	{
		if(mErrorCode==NONE_ERROR)
		{
			//切换线路

			g_ClientGameplayState->setTransporting();
			g_ClientGameplayState->setLineSwitching();
			g_ClientGameplayState->RandomGate(mLineId);
			Con::executef("SptConnectGateServer");
		}
		else
		{
			//线路切换失败
		}
	}
#endif	
}

///////////////////////// 服务器通知客户端的传送消息 ////////////////////////////////////////////////// 服务器通知客户端的传送消息 ///////////////////////// 
///////////////////////// 服务器通知客户端的传送消息 ////////////////////////////////////////////////// 服务器通知客户端的传送消息 ///////////////////////// 
///////////////////////// 服务器通知客户端的传送消息 ////////////////////////////////////////////////// 服务器通知客户端的传送消息 ///////////////////////// 
///////////////////////// 服务器通知客户端的传送消息 ////////////////////////////////////////////////// 服务器通知客户端的传送消息 ///////////////////////// 
///////////////////////// 服务器通知客户端的传送消息 ////////////////////////////////////////////////// 服务器通知客户端的传送消息 ///////////////////////// 

///////////////////////// 客户端切线请求 ///////////////////////// 
IMPLEMENT_CO_SERVEREVENT_V1(SwitchLineEvent);

void SwitchLineEvent::unpack(NetConnection *pCon, BitStream *bstream) 
{
#ifdef  NTJ_SERVER
	mLineId = bstream->readInt(16);
#endif
}

//
void SwitchLineEvent::pack(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	bstream->writeInt(mLineId,16);
#endif
}

//
void SwitchLineEvent::process(NetConnection *pCon)
{
#ifdef NTJ_SERVER

#ifndef SWITCH_LINE_DEBUG
	if(mLineId == g_ServerGameplayState->getCurrentLineId())
		return;
#endif

	ServerPacketProcess *pPacket = g_ServerGameplayState->GetPacketProcess();
	if(!pPacket)
		return;

	GameConnection *pGameCon = dynamic_cast<GameConnection *>(pCon);
	if(pGameCon)
	{
		Player *pPlayer = dynamic_cast<Player *>(pGameCon->getControlObject());
		Point3F pos = pPlayer->getPosition();
		pPlayer->transportObject(mLineId,pPlayer->getTriggerId(),g_ServerGameplayState->getZoneId(),pos.x,pos.y,pos.z);
	}
#endif	
}

// ========================================================================================================================================
//	PlayerBaseProEvent
// ========================================================================================================================================
IMPLEMENT_CO_CLIENTEVENT_V1(PlayerBaseProEvent);

PlayerBaseProEvent::PlayerBaseProEvent(U32 mask, bool isFirstUpdate)
{
	mMask = mask;
	mIsFirstUpdate = isFirstUpdate;
	
}

void PlayerBaseProEvent::packData(NetConnection* _conn, BitStream* stream)
{
#ifdef NTJ_SERVER
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if (!stream->writeFlag(valid))
		return;

	// 更新初始化标志
	stream->writeFlag(mIsFirstUpdate);
	// 更新标志
	stream->writeInt(mMask, Base::Bit32);

	// 更新玩家金钱
	if(mMask & Player::EventMoneyMask)
	{
		stream->writeSignedInt(player->getMoney(), BITS_UNSIGNLONG);
	}

	// 更新玩家绑定金钱
	if(mMask & Player::EventBindMoneyMask)
	{
		stream->writeSignedInt(player->getMoney(Player::Currentcy_BindMoney), BITS_UNSIGNLONG);
	}

	// 更新银行仓库金钱
	if(mMask & Player::EventBankMoneyMask)
	{
		stream->writeSignedInt(player->getMoney(Player::Currentcy_BankMoney), BITS_UNSIGNLONG);
	}

	// 更新玩家经验
	if(mMask & Player::EventExperienceMask)
	{
		stream->writeSignedInt(player->getExp(), BITS_UNSIGNLONG);
	}

	// 判断是否应该获取交互对象和状态
	if(mMask & Player::EventInteractionMask)
	{
		SceneObject* obj = player->getInteraction();
		U32 objID = obj ? conn->getGhostIndex(obj) : 0;
		stream->writeInt(objID, NetConnection::GhostIdBitSize);
		stream->writeInt(player->getInteractionState(), Base::Bit32);
	}

	// 更新玩家银行锁定标志
	if(mMask & Player::EventBankLockMask)
	{
		stream->writeFlag(player->getBankFlag());
	}

	// 更新玩家交易锁定
	if(mMask & Player::EventTradeLockMask)
	{
		stream->writeSignedInt(player->getTradeFlag(), BITS_UNSIGNLONG);
	}

	//　更新玩家修练专精技能
	if (mMask & Player::EventSkillMask)
	{
		stream->writeSignedInt(player->getLivingSkillPro(), BITS_UNSIGNLONG);
	}
	
	// 更新其它功能标志
	if(mMask & Player::EventOtherFlagMask)
	{
		stream->writeInt(player->getOtherFlag(), BITS_UNSIGNLONG);
	}

	// 更新活力值
	if(mMask & Player::EventVigorMask)
	{
		stream->writeInt(player->getMaxVigor(), BITS_UNSIGNLONG);
		stream->writeInt(player->getVigor(),    BITS_UNSIGNLONG);
	}
#endif
}

void PlayerBaseProEvent::unpackData(NetConnection* _conn, BitStream* stream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	if (!conn || !(player = dynamic_cast<Player*>(conn->getControlObject())) ||
		!stream->readFlag())
		return;

	mIsFirstUpdate = stream->readFlag();
	mMask = stream->readInt(Base::Bit32);

	// 判断是否应该获取玩家金钱
	if (mMask & Player::EventMoneyMask)
	{
		S32 iMoney = stream->readSignedInt(BITS_UNSIGNLONG);
		S32 iCurrentMoney = player->getMoney();
		// 若非第一次更新，则显示获取/失去金元的聊天系统消息
		if(!mIsFirstUpdate)
			Con::executef("ShowMoneyMessage", Con::getIntArg(0), Con::getIntArg(iCurrentMoney), Con::getIntArg(iMoney));

		if(iMoney < iCurrentMoney)
			player->reduceMoney(iCurrentMoney- iMoney, Player::Currentcy_Money);
		else if(iMoney > iCurrentMoney)
			player->addMoney(iMoney - iCurrentMoney, Player::Currentcy_Money);

		S32 iNewMoney = player->getMoney(Player::Currentcy_Money);
		S32 iGold	= mFloor(iNewMoney / 10000);
		S32 iSiller = mFloor((iNewMoney - iGold * 10000) / 100);
		S32 iCopper = iNewMoney - (iGold * 10000 + iSiller * 100);
		// 脚本UI刷新金元显示
		Con::executef("ShowPlayerMoney",Con::getIntArg(iGold),Con::getIntArg(iSiller),Con::getIntArg(iCopper));
	}

	// 判断是否应该获取绑定金钱
	if (mMask & Player::EventBindMoneyMask)
	{
		S32 iMoney = stream->readSignedInt(BITS_UNSIGNLONG);
		S32 iCurrentMoney = player->getMoney(Player::Currentcy_BindMoney);
		// 若非第一次更新，则显示获取/失去灵元的聊天系统消息
		if(!mIsFirstUpdate)
			Con::executef("ShowMoneyMessage", Con::getIntArg(1), Con::getIntArg(iCurrentMoney), Con::getIntArg(iMoney));

		if(iMoney < iCurrentMoney)
			player->reduceMoney(iCurrentMoney- iMoney, Player::Currentcy_BindMoney);
		else if(iMoney > iCurrentMoney)
			player->addMoney(iMoney - iCurrentMoney, Player::Currentcy_BindMoney);

		S32 iNewMoney = player->getMoney(Player::Currentcy_BindMoney);
		S32 iGold	= mFloor(iNewMoney / 10000);
		S32 iSiller = mFloor((iNewMoney - iGold * 10000) / 100);
		S32 iCopper = iNewMoney - (iGold * 10000 + iSiller * 100);
		// 脚本UI刷新灵元显示
		Con::executef("ShowPlayerBindMoney",Con::getIntArg(iGold),Con::getIntArg(iSiller),Con::getIntArg(iCopper));
	}

	// 判断是否应该获取银行仓库金钱
	if (mMask & Player::EventBankMoneyMask)
	{
		S32 iMoney = stream->readSignedInt(BITS_UNSIGNLONG);
		S32 iCurrentMoney = player->getMoney(Player::Currentcy_BankMoney);
		// 若非第一次更新，则显示获取/失去灵元的聊天系统消息
		if(!mIsFirstUpdate)
			Con::executef("ShowMoneyMessage", Con::getIntArg(2), Con::getIntArg(iCurrentMoney), Con::getIntArg(iMoney));
		if(iMoney < iCurrentMoney)
			player->reduceMoney(iCurrentMoney- iMoney, Player::Currentcy_BankMoney);
		else if(iMoney > iCurrentMoney)
		{
			if(mIsFirstUpdate)
				player->setBankMoney(iMoney - iCurrentMoney);
			else
				player->addMoney(iMoney - iCurrentMoney, Player::Currentcy_BankMoney);
		}

		S32 iNewMoney = player->getMoney(Player::Currentcy_BankMoney);
		S32 iGold	= mFloor(iNewMoney / 10000);
		S32 iSiller = mFloor((iNewMoney - iGold * 10000) / 100);
		S32 iCopper = iNewMoney - (iGold * 10000 + iSiller * 100);
		// 脚本UI刷新银行仓库金元显示
		Con::executef("ShowBankMoney",Con::getIntArg(iGold),Con::getIntArg(iSiller),Con::getIntArg(iCopper));
	}

	// 判断是否应该获取玩家经验
	if (mMask & Player::EventExperienceMask)
	{
		S32 iExp = stream->readSignedInt(BITS_UNSIGNLONG);
		S32 iCurrentExp = player->getExp();
        player->addExp(iExp - iCurrentExp);
        if (mIsFirstUpdate == false)        
        g_UIMouseGamePlay->addExpText((GameObject*)player, 0 ,iExp - iCurrentExp);
		// 脚本UI刷新角色经验显示
		Con::executef("ShowPlayerExp",
				Con::getIntArg(player->getExp()),
				Con::getIntArg(g_LevelExp[player->getLevel()]),
				Con::getIntArg(mIsFirstUpdate),
				Con::getIntArg(iExp),
				Con::getIntArg(iCurrentExp));
	}


	// 判断是否应该获取交互对象和状态
	if(mMask & Player::EventInteractionMask)
	{
		 S32 objID = stream->readInt(NetConnection::GhostIdBitSize);
		 SceneObject* obj = NULL;
		 if(objID != 0)
			obj = static_cast<GameObject*>(conn->resolveGhost(objID));
		 U32 state = stream->readInt(Base::Bit32);
		 player->setInteraction(obj, state);
	}

	// 更新玩家银行锁定标志
	if(mMask & Player::EventBankLockMask)
	{
		bool bBankFlag = stream->readFlag();
		player->setBankFlag(bBankFlag);
	}

	// 更新玩家交易锁定
	if(mMask & Player::EventTradeLockMask)
	{
		U32 iTradeFlag = stream->readSignedInt(BITS_UNSIGNLONG);
		player->setTradeFlag(iTradeFlag);
	}

	//　更新玩家修练专精技能
	if(mMask & Player::EventSkillMask)
	{
		U32 skillID = stream->readSignedInt(BITS_UNSIGNLONG);
		player->setLivingSkillPro(skillID);
	}

	// 更新其它功能标志
	if(mMask & Player::EventOtherFlagMask)
	{
		U32 otherFlag = stream->readInt(BITS_UNSIGNLONG);
		player->setOtherFlagValue(otherFlag);
		if(player->IsOtherFlag(Player::OTHERFLAG_NEWBIE))
		{
			Con::executef("ShowNewPlayerWalcomeWnd");			
			GameConnection* conn = GameConnection::getConnectionToServer();
			if(conn)
			{
				ClientGameNetEvent* ev = new ClientGameNetEvent(INFO_UPDATEOTHERFLAG);
				conn->postNetEvent(ev);
			}
		}

		if(player->IsOtherFlag(Player::OTHERFLAG_USEFASHION))
		{
			Con::executef("setFaction", Con::getIntArg(1));
			if(player->IsOtherFlag(Player::OTHERFLAG_FAMILYFASHION))
			{
				Con::executef("setFamilyFaction", Con::getIntArg(1));
			}
			else
			{
				Con::executef("setFamilyFaction", Con::getIntArg(0));
			}
		}
		else
		{
			Con::executef("setFaction", Con::getIntArg(0));
		}		
	}

	// 更新活力值
	if (mMask & Player::EventVigorMask)
	{
		U32 iMaxVigor  = stream->readInt(BITS_UNSIGNLONG);
		S32 deltaVigor = stream->readInt(BITS_UNSIGNLONG) - player->getVigor();
		S32 deltaMaxVigor = iMaxVigor - player->getMaxVigor();

		if(!mIsFirstUpdate)
		{
			if(deltaVigor > 0)
				MessageEvent::show(SHOWTYPE_ERROR,avar(GetLocalStr(MSG_PLAYER_ADDVIGOR),deltaVigor),SHOWPOS_CHAT);
			else if(deltaVigor < 0)
				MessageEvent::show(SHOWTYPE_ERROR,avar(GetLocalStr(MSG_PLAYER_REDUCEVIGOR),-deltaVigor),SHOWPOS_CHAT);

			if (deltaMaxVigor>0)
				MessageEvent::show(SHOWTYPE_ERROR,avar(GetLocalStr(MSG_PLAYER_ADDMAXVIGOR),iMaxVigor),SHOWPOS_CHAT);
		}

		player->setMaxVigor(iMaxVigor);
		player->addVigor(deltaVigor);
		player->SetRefreshGui(true);
	}
#endif
}

//--------------------------------------------------------------------------------
//快捷栏
//--------------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(PanelEvent);

PanelEvent::PanelEvent(S32 index)
{
	mIndex = index;
}

void PanelEvent::setParam(S32 index)
{
	mIndex = index;
}

void PanelEvent::packData(NetConnection* pCon, BitStream* bstream)
{
#ifdef NTJ_SERVER
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));

	if(!bstream->writeFlag(valid))
		return;
	bstream->writeInt(mIndex, Base::Bit8);

	ShortcutObject* pSlot = pPlayer->panelList.GetSlot(mIndex);
	if(bstream->writeFlag(pSlot != NULL))
	{
		bstream->writeFlag(pSlot->isSkillObject());
		bstream->writeFlag(pSlot->isLivingSkillObject());
		bstream->writeFlag(pSlot->isPetObject());
		bstream->writeFlag(pSlot->isMountObject());
		pSlot->packUpdate(bstream);
		
	}
#endif
}

void PanelEvent::unpackData(NetConnection* pCon, BitStream* bstream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !bstream->readFlag())
		return;
	mIndex = bstream->readInt(Base::Bit8);
	ShortcutObject* pSlot = player->panelList.GetSlot(mIndex);
	if(bstream->readFlag())
	{
		bool IsSkillObject = bstream->readFlag();	
		bool isLivingSkillObject = bstream->readFlag();
		bool isPetObject = bstream->readFlag();
		bool isMountObject = bstream->readFlag();
		if(IsSkillObject)
		{
			if(pSlot && pSlot->isSkillObject())
			{
				pSlot->unpackUpdate(bstream);				
			}
			else
			{
				SkillShortcut* pSkill = new SkillShortcut;
				pSkill->unpackUpdate(bstream);
				player->panelList.SetSlot(mIndex, pSkill);
			}				
		}
		else if (isLivingSkillObject)
		{
			if (pSlot && pSlot->isLivingSkillObject())
			{
				pSlot->unpackUpdate(bstream);
			}
			else
			{
				LivingSkillShortcut* pSkill = new LivingSkillShortcut;
				pSkill->unpackUpdate(bstream);
				player->panelList.SetSlot(mIndex,pSkill);
			}
		}
		else if (isPetObject)
		{
			PetShortcut *pPet = PetShortcut::CreateEmptyPetItem();
			pPet->unpackUpdate(bstream);
			player->panelList.SetSlot(mIndex, pPet);
		}
		else if (isMountObject)
		{
			MountShortcut *pMount = MountShortcut::CreateEmptyMountItem();
			pMount->unpackUpdate(bstream);
			player->panelList.SetSlot(mIndex, pMount);
		}
		else
		{
			if(pSlot && pSlot->isItemObject())
			{
				pSlot->unpackUpdate(bstream);
			}
			else
			{
				ItemShortcut* pItem = ItemShortcut::CreateEmptyItem();
				pItem->unpackUpdate(bstream);
				player->panelList.SetSlot(mIndex, pItem);
			}
		}
	}
	else
	{
		player->panelList.SetSlot(mIndex, NULL);
	}
#endif
}


//-----------------------------------------------------------------------------
// 物品栏事件
//-----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(InventoryEvent);
InventoryEvent::InventoryEvent()
{
	mIndex = 0;
	mFlag = 0;
}

InventoryEvent::InventoryEvent(S32 index, S32 flag)
{
	mIndex = index;
	mFlag = flag;
}

void InventoryEvent::packData(NetConnection* _conn, BitStream* stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
    if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	stream->writeInt(mFlag, Base::Bit8);

	// 判定槽位对象是否存在
	ShortcutObject* pSlot = player->inventoryList.GetSlot(mIndex);
	ItemShortcut* pItem = NULL;
	valid = pSlot && (pItem = dynamic_cast<ItemShortcut*>(pSlot));
	if(!stream->writeFlag(valid))
		return;
		
	pItem->packUpdate(stream);
#endif
}

#ifdef NTJ_CLIENT
void ItemNotifyInfo(U32 flag, ItemShortcut* pOldItem, ItemShortcut* pNewItem)
{
	if(flag == ITEM_NOSHOW || flag == ITEM_LOAD)
		return;
	U32  oldnums  = pOldItem ? pOldItem->getRes()->getQuantity() : 0;
	U32  newnums  = pNewItem ? pNewItem->getRes()->getQuantity() : 0;
	bool isSame = g_ItemManager->isSameItem(pOldItem, pNewItem);
	if(oldnums == newnums && isSame)
		return;
	if(!isSame)
	{
		if(pOldItem)
		{
			Con::executef("HandleItemSomething", 
							pOldItem->getRes()->getItemName(),
							Con::getIntArg(ITEM_LOSE),
							Con::getIntArg(oldnums),
							Con::getIntArg(0));
		}
		if(pNewItem)
		{
			Con::executef("HandleItemSomething",
							pNewItem->getRes()->getItemName(),
							Con::getIntArg(flag),
							Con::getIntArg(0),
							Con::getIntArg(newnums));
		}
	}
	else
	{
		char* itemname = Con::getArgBuffer(32);
		if(pNewItem)
			dStrcpy(itemname, 32, pNewItem->getRes()->getItemName());
		else if(pOldItem)
			dStrcpy(itemname, 32, pOldItem->getRes()->getItemName());
		else
			itemname[0] = '\0';
		Con::executef("HandleItemSomething",
						itemname,
						Con::getIntArg(flag),
						Con::getIntArg(oldnums),
						Con::getIntArg(newnums));		
	}

	//获取物品时界面给予ICON表现
	if(newnums > oldnums && pNewItem)
	{
		StringTableEntry iconName = pNewItem->getIconName();
		if(iconName && *iconName)
		{
			char iconFile[128];
			dSprintf(iconFile, 128, "gameres/data/icon/item/%s", iconName);
			g_UIMouseGamePlay->setItemNotify(iconFile);
		}		
	}
}
#endif//NTJ_CLIENT

void InventoryEvent::unpackData(NetConnection* _conn, BitStream* stream)
{
#ifdef NTJ_CLIENT
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;
	
	mIndex = stream->readInt(Base::Bit8);	//槽位索引位置	
	mFlag  = stream->readInt(Base::Bit8);	//更新行为标志
	
	ItemShortcut  *pOldItem, *pNewItem;
	if(stream->readFlag())
	{
		ItemShortcut* pItem = (ItemShortcut*)player->inventoryList.GetSlot(mIndex);		
		if (pItem)
		{
			pOldItem = g_ItemManager->cloneItem(pItem);		
			pItem->unpackUpdate(stream);
			if(player->getInteractionState() != Player::INTERACTION_NPCTRADE)
				Con::executef("updateItemHot", Con::getIntArg(mIndex));
		}
		else
		{
			//确认客户端不存在槽位对象
			pOldItem = NULL;
			pItem = ItemShortcut::CreateEmptyItem();
			pItem->unpackUpdate(stream);
			player->inventoryList.SetSlot(mIndex, pItem);
		}
		pNewItem = pItem;
	}
	else
	{
		// 确认服务端不存在槽位对象
		pOldItem = (ItemShortcut*)player->inventoryList.GetSlot(mIndex);
		player->inventoryList.SetSlot(mIndex, NULL, false);
		pNewItem = NULL;
	}

	ItemNotifyInfo(mFlag, pOldItem, pNewItem);
	if(pOldItem)
		delete pOldItem;	
#endif
}

//-----------------------------------------------------------------------------
// 回购栏事件
//-----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(ReBuyEvent);
ReBuyEvent::ReBuyEvent()
{
	mIndex = 0;
	mFlag = 0;
}

ReBuyEvent::ReBuyEvent(S32 index, S32 flag)
{
	mIndex = index;
	mFlag = flag;
}

void ReBuyEvent::packData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_SERVER
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));

	if(!bstream->writeFlag(valid))
		return;

	bstream->writeInt(mIndex, Base::Bit8);
	bstream->writeInt(mFlag, Base::Bit8);


	// 判定槽位对象是否存在
	ShortcutObject* pSlot = pPlayer->dumpList.GetSlot(mIndex);
	ItemShortcut* pItem = NULL;
	valid = pSlot && (pItem = dynamic_cast<ItemShortcut*>(pSlot));
	if(!bstream->writeFlag(valid))
		return;

	pItem->packUpdate(bstream);
#endif
}

void ReBuyEvent::unpackData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !bstream->readFlag())
		return;
	mIndex = bstream->readInt(Base::Bit8);
	mFlag = bstream->readInt(Base::Bit8);
	
	if(bstream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pPlayer->dumpList.GetSlot(mIndex);
		if (pSlot)
		{
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(bstream);
		}
		else
		{
			//确认客户端不存在槽位对象
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(bstream);
			pPlayer->dumpList.SetSlot(mIndex, pShortcut);
		}
	/*	ItemShortcut* pItem = dynamic_cast<ItemShortcut*>(pPlayer->dumpList.GetSlot(mIndex));
		if(pItem)
			Con::executef("ShowReturnItemInfo",pItem->getRes()->getItemName(), Con::getIntArg(pItem->getRes()->getSalePrice()), Con::getIntArg(mIndex));*/
	}
	else
	{
		// 确认服务端不存在槽位对象
		pPlayer->dumpList.SetSlot(mIndex, NULL);
		Con::executef("clearItemInfor", Con::getIntArg(mIndex));
	}
#endif
}

//-----------------------------------------------------------------------------
// 装备栏事件
//-----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(EquipEvent);
EquipEvent::EquipEvent()
{
	mIndex = 0;
	mFlag = 0;
}

EquipEvent::EquipEvent(S32 index, S32 flag)
{
	mIndex = index;
	mFlag = flag;
}
void EquipEvent::packData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (pCon && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!bstream->writeFlag(valid))
		return;

	// 槽位索引位置
	bstream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	bstream->writeInt(mFlag, Base::Bit8);

	// 判定槽位对象是否存在
	ShortcutObject* pSlot = player->equipList.GetSlot(mIndex);
	ItemShortcut* pItem = NULL;
	valid = pSlot && (pItem = dynamic_cast<ItemShortcut*>(pSlot));
	if(!bstream->writeFlag(valid))
		return;

	pItem->packUpdate(bstream);
#endif
}

void EquipEvent::unpackData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !bstream->readFlag())
		return;
	mIndex = bstream->readInt(Base::Bit8);
	mFlag = bstream->readInt(Base::Bit8);

	if(bstream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pPlayer->equipList.GetSlot(mIndex);
		if (pSlot)
		{
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(bstream);
		}
		else
		{
			//确认客户端不存在槽位对象
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(bstream);
			pPlayer->equipList.SetSlot(mIndex, pShortcut);
		}
	}
	else
	{
		// 确认服务端不存在槽位对象
		pPlayer->equipList.SetSlot(mIndex, NULL);
	}
#endif
}

//----------------------------------------------------------------------------
// 仓库栏事件
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(BankEvent);
BankEvent::BankEvent()
{
	mIndex = 0;
	mFlag  = 0;
}

BankEvent::BankEvent(S32 index, S32 flag)
{
	mIndex = index;
	mFlag  = flag;
}

void BankEvent::packData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (pCon && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!bstream->writeFlag(valid))
		return;

	// 槽位索引位置
	bstream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	bstream->writeInt(mFlag, Base::Bit8);
	//

	ShortcutObject* pSlot = player->bankList.GetSlot(mIndex);
	ItemShortcut* pItem = NULL;
	valid = pSlot && (pItem = dynamic_cast<ItemShortcut*>(pSlot));
	if(!bstream->writeFlag(valid))
		return;

	pItem->packUpdate(bstream);

#endif
}

void BankEvent::unpackData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !bstream->readFlag())
		return;

	mIndex = bstream->readInt(Base::Bit8);
	mFlag = bstream->readInt(Base::Bit8);
	
	if(bstream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pPlayer->bankList.GetSlot(mIndex);
		if (pSlot)
		{
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(bstream);
		}
		else
		{
			//确认客户端不存在槽位对象
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(bstream);
			pPlayer->bankList.SetSlot(mIndex, pShortcut);
		}
	}
	else
	{
		// 确认服务端不存在槽位对象
		pPlayer->bankList.SetSlot(mIndex, NULL);
	}
#endif
}

//----------------------------------------------------------------------------
// 交易栏事件
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(TradeEvent);
TradeEvent::TradeEvent()
{
	mIndex	= 0;
	mFlag	= 0;
}
TradeEvent::TradeEvent(S32 index, S32 flag)
{
	mIndex	= index;
	mFlag	= flag;
}
void TradeEvent::packData(NetConnection *_conn, BitStream *stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	stream->writeInt(mFlag, Base::Bit8);

	// 判定槽位对象是否存在
	ShortcutObject* pSlot = player->tradeList.GetSlot(mIndex);
	ItemShortcut* pItem = NULL;
	valid = pSlot && (pItem = dynamic_cast<ItemShortcut*>(pSlot));
	if(!stream->writeFlag(valid))
		return;

	pItem->packUpdate(stream);

#endif
}

void TradeEvent::unpackData(NetConnection *_conn, BitStream *stream)
{
#ifdef NTJ_CLIENT
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;

	// 槽位索引位置
	mIndex = stream->readInt(Base::Bit8);
	// 更新行为标志
	mFlag  = stream->readInt(Base::Bit8);
	if(stream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = player->tradeList.GetSlot(mIndex);
		if (pSlot)
		{
			ItemShortcut* pItem = (ItemShortcut*)pSlot;
			U32 iQuantity = pItem->getRes()->getQuantity();
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(stream);
		}
		else
		{
			//确认客户端不存在槽位对象
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(stream);
			player->tradeList.SetSlot(mIndex, pShortcut);
		}
	}
	else
	{
		// 确认服务端不存在槽位对象
		player->tradeList.SetSlot(mIndex, NULL);
	}
#endif
}

//----------------------------------------------------------------------------
// 对方交易栏事件
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(TradeTargetEvent);
TradeTargetEvent::TradeTargetEvent()
{
	mIndex	= 0;
	mTargetPlayerID	= 0;
}
TradeTargetEvent::TradeTargetEvent(U32 targetPlayerID, S32 index)
{
	mIndex	= index;
	mTargetPlayerID	= targetPlayerID;
}
void TradeTargetEvent::packData(NetConnection *_conn, BitStream *stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新对方玩家ID
	stream->writeInt(mTargetPlayerID, Base::Bit32);

	Player* pTarget = g_ServerGameplayState->GetPlayer(mTargetPlayerID);
	stream->writeFlag(pTarget != NULL);

	// 判定槽位对象是否存在
	ShortcutObject* pSlot = pTarget->tradeList.GetSlot(mIndex);
	ItemShortcut* pItem = NULL;
	valid = pSlot && (pItem = dynamic_cast<ItemShortcut*>(pSlot));
	if(!stream->writeFlag(valid))
		return;

	pItem->packUpdate(stream);

#endif
}

void TradeTargetEvent::unpackData(NetConnection *_conn, BitStream *stream)
{
#ifdef NTJ_CLIENT
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;

	// 槽位索引位置
	mIndex = stream->readInt(Base::Bit8);
	// 更新对方玩家ID
	mTargetPlayerID  = stream->readInt(Base::Bit32);
	if(!stream->readFlag())
		return;
	Player* pTarget = g_ClientGameplayState->findPlayer(mTargetPlayerID);
	if(!pTarget)
		return;

	if(stream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pTarget->tradeList.GetSlot(mIndex);
		if (pSlot)
		{
			ItemShortcut* pItem = (ItemShortcut*)pSlot;
			U32 iQuantity = pItem->getRes()->getQuantity();
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(stream);
		}
		else
		{
			//确认客户端不存在槽位对象
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(stream);
			pTarget->tradeList.SetSlot(mIndex, pShortcut);
		}
	}
	else
	{
		// 确认服务端不存在槽位对象
		pTarget->tradeList.SetSlot(mIndex, NULL);
	}
#endif
}

//-----------------------------------------------------------------------------
// NPC系统商店事件
//-----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(NpcShopEvent);
NpcShopEvent::NpcShopEvent():
		mIndex(0),
		mShopID(0),
		mCurrentPage(0),
		mTotalPage(0),
		mShopType(0),
		mCurrentServerTime(0)
{
}

NpcShopEvent::NpcShopEvent(S32 index, S32 shopid, U32 shoptype, S32 page, S32 totalPages)
{
	mIndex = index;
	mShopID = shopid;
	mCurrentPage = page;
	mTotalPage = totalPages;
	mShopType = shoptype;
	mCurrentServerTime = 0;
}

void NpcShopEvent::packData(NetConnection* _conn, BitStream* stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	if(mIndex == 0)
	{
		stream->writeInt(mShopID, Base::Bit16);
		stream->writeInt(mShopType, Base::Bit8);
		stream->writeInt(mCurrentPage, Base::Bit8);
		stream->writeInt(mTotalPage, Base::Bit8);		
	}
	// 判定槽位对象是否存在
	NpcShopItem* pItem = player->npcShopList.mSlots[mIndex];
	if(stream->writeFlag(pItem != NULL))
	{
		stream->writeBits(sizeof(NpcShopItem) * 8, pItem);
		mCurrentServerTime = Platform::getTime();
		stream->writeInt(mCurrentServerTime, Base::Bit32);
	}
#endif
}

void NpcShopEvent::unpackData(NetConnection* _conn, BitStream* stream)
{
#ifdef NTJ_CLIENT
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;

	// 槽位索引位置
	mIndex = stream->readInt(Base::Bit8);
	if(mIndex == 0)
	{
		mShopID = stream->readInt(Base::Bit16);
		mShopType = stream->readInt(Base::Bit8);
		mCurrentPage = stream->readInt(Base::Bit8);
		mTotalPage = stream->readInt(Base::Bit8);		
		g_ItemManager->openNpcShop(player, mShopID);
		player->npcShopList.SetNpcShopPage(mCurrentPage, mTotalPage);
		Con::executef("ShowShopItemNum",  Con::getIntArg(mCurrentPage + 1), Con::getIntArg(mTotalPage));
	}
	if(stream->readFlag())
	{
		NpcShopItem* pItem = new NpcShopItem;		
		if(player->npcShopList.mSlots[mIndex])
		{
			if(player->npcShopList.mSlots[mIndex]->pItem)
			{
				delete player->npcShopList.mSlots[mIndex]->pItem;
				player->npcShopList.mSlots[mIndex]->pItem = NULL;
			}
			delete player->npcShopList.mSlots[mIndex];
			player->npcShopList.mSlots[mIndex] = NULL;
		}
		stream->readBits(sizeof(NpcShopItem) * 8, pItem);
		pItem->pItem = ItemShortcut::CreateItem(pItem->ItemID, 1);
		player->npcShopList.mSlots[mIndex] = pItem;

		mCurrentServerTime = stream->readInt(Base::Bit32);
		pItem->LastUpdateTime = Platform::getTime() - (mCurrentServerTime - pItem->LastUpdateTime);
		if(pItem->StockInterval && pItem->CurrentStockNum == 0)
		{
			player->npcShopList.GetSlotTrue(mIndex)->setSlotState(ShortcutObject::SLOT_LOCK);
		}
		else
		{
			player->npcShopList.GetSlotTrue(mIndex)->setSlotState(ShortcutObject::SLOT_COMMON);
		}	

		Con::executef("ShowGoodInfor", pItem->pItem->getRes()->getItemName(), Con::getIntArg(pItem->CurrencyID), Con::getIntArg(pItem->CurrencyValue), Con::getIntArg(pItem->StockNum), 
			Con::getIntArg(pItem->CurrentStockNum), Con::getIntArg(mIndex));

	}
	else
	{
		if(player->npcShopList.mSlots[mIndex])
		{
			if(player->npcShopList.mSlots[mIndex]->pItem)
			{
				delete player->npcShopList.mSlots[mIndex]->pItem;
				player->npcShopList.mSlots[mIndex]->pItem = NULL;
			}

			delete player->npcShopList.mSlots[mIndex];
		}
		player->npcShopList.mSlots[mIndex] = NULL;
		Con::executef("clearNPCShopText", Con::getIntArg(mIndex));
	}
	
#endif
}

//----------------------------------------------------------------------------
// 怪物掉物事件
//----------------------------------------------------------------------------
IMPLEMENT_CO_NETEVENT_V1(DropItemEvent);

bool DropItemEvent::m_bPickupAllItem = false;

#ifdef NTJ_SERVER
bool DropItemEvent::ServerSendDropList(Player *pPicker, S32 nCount)
{
	GameConnection *clientConn = pPicker->getControllingClient();
	SceneObject *pInteractionObj = pPicker->getInteraction();

	if (clientConn && pInteractionObj)
	{	
		InteractionType type = TYPE_PRIZE_BOX;
		if (dynamic_cast<PrizeBox *>(pInteractionObj))
		{
			type = TYPE_PRIZE_BOX;
		}
		else if (dynamic_cast<CollectionObject *>(pInteractionObj))
		{
			type = TYPE_COLLECTION_OBJECT;
		}
		else
			return false;

		for (S32 nIndex = 0; nIndex < nCount; nIndex++)
		{
			DropItemEvent *event = new DropItemEvent(SERVER_DROPLIST);
			event->setInterationType(type);			
			event->setData(pInteractionObj->getId(), nIndex);
			clientConn->postNetEvent(event);
		}		
	}
	return true;
}

bool DropItemEvent::ServerSendEraseDropItem(Player *pPicker, S32 nItemIndex)
{
	GameConnection *clientConn = pPicker->getControllingClient();

	if (clientConn)
	{
		DropItemEvent *event = new DropItemEvent(SERVER_ERASE_ITEM);
		event->setData(nItemIndex);
		if (clientConn->postNetEvent(event))
			return true;
	}

	return true;
}

bool DropItemEvent::ServerSendClearAllDropItems(Player *pPicker)
{
	GameConnection *clientConn = pPicker->getControllingClient();

	if (clientConn)
	{
		DropItemEvent *event = new DropItemEvent(SERVER_CLEAR_ALL_ITEMS);
		if (clientConn->postNetEvent(event))
			return true;
	}

	return true;
}

bool DropItemEvent::ServerSendEraseItemsInList(Player *pPicker, S32 count, U32 mask)
{
	GameConnection *clientConn = pPicker->getControllingClient();

	if (clientConn)
	{
		DropItemEvent *event = new DropItemEvent(SERVER_ERASE_ITEM_IN_LIST);
		event->setData(count, mask);
		if (clientConn->postNetEvent(event))
			return true;
	}

	return true;
}
#endif

bool DropItemEvent::setInterationType(SceneObject *pTargetObject)
{
	if (!pTargetObject)
		return false;

	if ( dynamic_cast<PrizeBox *>(pTargetObject) != NULL )
	{
		m_InteractionType = TYPE_PRIZE_BOX;
		return true;
	}
	else if ( dynamic_cast<CollectionObject *>(pTargetObject) != NULL )
	{
		m_InteractionType = TYPE_COLLECTION_OBJECT;
		return true;
	}
	
	return false;
}

#ifdef NTJ_CLIENT
bool DropItemEvent::ClientSendAutoPickup(PrizeBox *pPrizeBox)
{
	GameConnection* serverConn = GameConnection::getConnectionToServer();
	if (serverConn && pPrizeBox)
	{
		Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());
		if (!player)
			return false;

		DropItemEvent *event = new DropItemEvent(CLIENT_AUTO_PICKUP_ITEMS);
		event->setData(pPrizeBox->getServerId());
		if (serverConn->postNetEvent(event))
			return true;
	}

	return false;
}

bool DropItemEvent::ClientSendPickupPrizeItem(PrizeBox *pPrizeBox)
{
	GameConnection* serverConn = GameConnection::getConnectionToServer();
	if (serverConn && pPrizeBox)
	{
		Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());
		if (!player)
			return false;

		DropItemEvent *event = new DropItemEvent(CLIENT_PICKUP_PRIZE_ITEM);
		event->setData(pPrizeBox->getServerId());
		if (serverConn->postNetEvent(event))
			return true;
	}

	return false;
}

bool DropItemEvent::ClientSendOpenDropItemList(SceneObject *pTargetObject)
{
	GameConnection* serverConn = GameConnection::getConnectionToServer();
	if (serverConn && pTargetObject)
	{
		DropItemEvent *event = new DropItemEvent(CLIENT_OPEN_DROPLIST);
		if (!event->setInterationType(pTargetObject))
		{
			delete event;
			return false;
		}
		event->setData(pTargetObject->getServerId());
		if (serverConn->postNetEvent(event))
			return true;
	}

	return false;
}

bool DropItemEvent::ClientSendCloseDropItemList()
{
	GameConnection* serverConn = GameConnection::getConnectionToServer();
	Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());
	if (serverConn && player)
	{
		SceneObject *pTargetObject = player->getInteraction();
		if (!pTargetObject)
		{
			//WarnMessageEvent::AddScreenMessage(player, "掉物的物品已经消失");
			Con::executef("ClosePickupItemWnd");
			return false;
		}
		
		DropItemEvent *event = new DropItemEvent(CLIENT_CLOSE_DROPLIST);
		if (!event->setInterationType(pTargetObject))
		{
			delete event;
			return false;
		}
		event->setData(player->getInteraction()->getServerId());
		if (serverConn->postNetEvent(event))
			return true;	
	}

	return false;
}

bool DropItemEvent::ClientSendClearItem(S32 nItemIndex)
{
	GameConnection* serverConn = GameConnection::getConnectionToServer();
	Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());
	if (serverConn && player)
	{
		SceneObject *pTargetObject = player->getInteraction();
		if (pTargetObject)
		{
			//WarnMessageEvent::AddScreenMessage(player, "掉物的物品已经消失");
			Con::executef("ClosePickupItemWnd");
			return false;
		}
		DropItemEvent *event = new DropItemEvent(CLIENT_CLEAR_ITEM);
		if (!event->setInterationType(pTargetObject))
		{
			delete event;
			return false;
		}
		event->setData(player->getInteraction()->getServerId());
		if (serverConn->postNetEvent(event))
			return true;
	}

	return true;
}

bool DropItemEvent::ClientSendPickupItem(S32 nItemIndex, bool bPickupAll)
{
	GameConnection* serverConn = GameConnection::getConnectionToServer();
	Player *player = dynamic_cast<Player *>(g_ClientGameplayState->GetControlObject());
	if (serverConn && player)
	{
		SceneObject *pTargetObject = player->getInteraction();
		if (!pTargetObject)
		{
			//WarnMessageEvent::AddScreenMessage(player, "掉物的物品已经消失");
			Con::executef("ClosePickupItemWnd");
			return false;
		}

		DropItemEvent *event = NULL;
		if (bPickupAll)
		{
			event = new DropItemEvent(CLIENT_PICKUP_ITEM_ALL);
		}
		else
		{
			event = new DropItemEvent(CLIENT_PICKUP_ITEM);
		}
		if (!event->setInterationType(pTargetObject))
		{
			delete event;
			return false;
		}
		event->setData(player->getInteraction()->getServerId(), nItemIndex);

		if (serverConn->postNetEvent(event))
			return true;
	}

	return true;
}

void DropItemEvent::ClientSetPickupAllItem(bool bAllItem)
{
	m_bPickupAllItem = bAllItem;
}
#endif


void DropItemEvent::packData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	bstream->writeInt(m_Flag, BITS_MASK);
	bstream->writeInt(m_Data, 32);
	bstream->writeInt(m_Index, 32);
#endif

#ifdef NTJ_SERVER
	GameConnection *clientConn = dynamic_cast<GameConnection *>(pCon);
	Player *player = NULL;
	bool valid =  ( clientConn && (player = dynamic_cast<Player *>(clientConn->getControlObject())) );
	if (!bstream->writeFlag(valid))
		return;

	bstream->writeInt(m_Flag, BITS_MASK);
	switch(m_Flag)
	{
	case SERVER_OPEN_ERROR:
	case SERVER_ERASE_ITEM:
		bstream->writeInt(m_Data, 32);
		break;

	case SERVER_DROPLIST:
		{
			DropItemList *pDropItemList = NULL;
			if (m_InteractionType == TYPE_PRIZE_BOX)
			{
				PrizeBox *pPrizeBox = (PrizeBox *)(Sim::findObject(m_Data));
				if (pPrizeBox == NULL || pPrizeBox->m_DropItemList == NULL)
				{
					bstream->writeFlag(false);
					return;
				}
				pDropItemList = pPrizeBox->m_DropItemList;
			}
			else if (m_InteractionType == TYPE_COLLECTION_OBJECT)
			{
				CollectionObject *pCollectionObj = (CollectionObject *)(Sim::findObject(m_Data));
				if (pCollectionObj == NULL || pCollectionObj->m_DropItemList == NULL)
				{
					bstream->writeFlag(false);
					return;
				}
				pDropItemList = pCollectionObj->m_DropItemList;
			}
			else
			{
				bstream->writeFlag(false);
				return;
			}

			if ( !bstream->writeFlag(pDropItemList->OpenPickup(player)) )
				return;
			
			U32 nSendCount = 1;
			U32 nCount = 0;
			bstream->writeInt(m_InteractionType, 8);
			bstream->writeInt(nSendCount, 32);
			bstream->writeInt(m_Index, 32);
			PickupList *pItemList = pDropItemList->m_ItemsMap[player->getPlayerID()];
			U32 size = pItemList->Size();
			
			for(S32 index = m_Index; index < size; index++)
			{
				nCount++;
				if (nCount > nSendCount)
					break;

				ItemShortcut *pItemShortcut = dynamic_cast<ItemShortcut *>(pItemList->GetSlot(index));
				if (pItemShortcut)
				{
					pItemShortcut->packUpdate(bstream);
				}				
			}

			if (m_Index < size - 1)
			{
				// 未完成发送
				bstream->writeInt(0, BITS_INT_8);
			}
			else
			{
				//完成发送
				bstream->writeInt(1, BITS_INT_8);
			}
		}
		break;

	case SERVER_ERASE_ITEM_IN_LIST:
		{
			bstream->writeInt(m_Data, 32);
			bstream->writeInt(m_Index, 32);
			break;
		}
	}
#endif
}

void DropItemEvent::unpackData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	if (!bstream->readFlag())
		return;

	Player *player = NULL;
	GameConnection *serverConn = dynamic_cast<GameConnection *>(pCon); 
	if (!serverConn || !(player = dynamic_cast<Player*>(serverConn->getControlObject())) )
		return;

	m_Flag = bstream->readInt(BITS_MASK);
	switch (m_Flag)
	{
	case SERVER_DROPLIST:
		{
			bool readFlag = bstream->readFlag();
			if (!readFlag)
				return;
			
			m_InteractionType = bstream->readInt(8);
			S32 nReceiveCount = bstream->readInt(32);
			m_Index = bstream->readInt(32);
			if(m_Index == 0)
				player->pickupList.Clear();

			for(S32 i = 0; i < nReceiveCount; i++)
			{
				ItemShortcut *pItemShortcut = ItemShortcut::CreateEmptyItem();
				pItemShortcut->unpackUpdate(bstream);
				player->pickupList.AddItem(pItemShortcut);
			}

			if (bstream->readInt(BITS_INT_8) == 1)
			{
				//显示拾取列表窗口
				Con::executef("ShowPickupList", "0");
			}

			break;
		}

	case SERVER_ERASE_ITEM:
		{
			m_Data = bstream->readInt(32);
			player->pickupList.EraseSlot(m_Data);

			if (player->pickupList.Empty())
			{
				Con::executef("PickupList_CloseDropList");
			}
			else
			{
				if (!player->pickupList.CanGetSlotInCurrentPage())  //当前页没有了物品，向上翻页
				{
					player->pickupList.DecCurrentPage();
				}
				Con::executef("ShowPickupList");
			}
			
			break;
		}
	
	case SERVER_CLEAR_ALL_ITEMS:
		{
			player->pickupList.Clear();
			Con::executef("ClearPickupListText");
			break;
		}

	case SERVER_ERASE_ITEM_IN_LIST:
		{
			m_Data = bstream->readInt(32);  //count
			m_Index = bstream->readInt(32);	//mask
			player->pickupList.ClearList(m_Data, m_Index);
			Con::executef("ShowPickupList", "0");
			break;
		}
	
	default:
		break;
	}
#endif

#ifdef NTJ_SERVER
	PrizeBox *pPrizeBox = NULL;
	CollectionObject *pCollectionObj = NULL;
	SceneObject *pTargetObject = NULL;
	GameConnection *clientConn = dynamic_cast<GameConnection *>(pCon);
	Player *player = NULL;
	if (!clientConn || !(player = dynamic_cast<Player *>(clientConn->getControlObject())) )
		return;

	m_Flag = bstream->readInt(BITS_MASK);
	m_Data = bstream->readInt(32);
	m_Index = bstream->readInt(32);

	DropItemList* pDropItemList = NULL;
	if ( (pPrizeBox = dynamic_cast<PrizeBox *>(Sim::findObject(m_Data))) != NULL)
	{
		pDropItemList = pPrizeBox->m_DropItemList;
		pTargetObject = pPrizeBox;
		m_InteractionType = TYPE_PRIZE_BOX;
	}
	else if ( (pCollectionObj = dynamic_cast<CollectionObject *>(Sim::findObject(m_Data))) != NULL)
	{
		pDropItemList = pCollectionObj->m_DropItemList;
		pTargetObject = pCollectionObj;
		m_InteractionType = TYPE_COLLECTION_OBJECT;
	}
	else
		return;

	if (pDropItemList == NULL)
		return;

	switch(m_Flag)
	{
	case CLIENT_AUTO_PICKUP_ITEMS:
		{
			if (player->getDistance(pTargetObject) > INTERACTIONDISTANCE)
				return;

			S32 count = 0;
			U32 mask = 0;
			bool bPickup = pDropItemList->PickupItemAll(player, count, mask);
			break;
		}
	case CLIENT_PICKUP_PRIZE_ITEM:
		{
			if (player->getDistance(pTargetObject) > INTERACTIONDISTANCE)
				return;
			
			enWarnMessage msg = MSG_NONE;
			bool bPickup = pDropItemList->PickupItem(player, 0, msg);
			if (bPickup)
			{
				if (m_InteractionType == TYPE_PRIZE_BOX)
				{
					//pPrizeBox->setMaskBits(PrizeBox::SendMask);
					pTargetObject->setMaskBits(PrizeBox::SendMask);
				}
				//DropItemEvent::ServerSendErasePrizeItem(player);
			}
			else
			{
				//WarnMessageEvent::AddScreenMessage(player, msg);
			}
		}

	case CLIENT_OPEN_DROPLIST:
		{
			if (player->getDistance(pTargetObject) > INTERACTIONDISTANCE)
				return;

			if (pDropItemList->IsViewItems(player))
			{				
				if (player->isBusy(Player::INTERACTION_PICKUP))
				{
					//WarnMessageEvent::AddScreenMessage(player, MSG_PLAYER_BUSY);
					return;
				}

				player->setInteraction(pTargetObject, Player::INTERACTION_PICKUP);
				ServerSendDropList(player, pDropItemList->GetItemCount(player));
			}
			else
			{
				//WarnMessageEvent::AddScreenMessage(player, MSG_PICKUP_OPEN_FAILED);
			}
		}
		break;
	
	case CLIENT_CLOSE_DROPLIST:
		{
			pDropItemList->ClosePickup(player);
			player->setInteraction(NULL, Player::INTERACTION_NONE);
		}
		break;

	case CLIENT_PICKUP_ITEM:
		{
			enWarnMessage msg = MSG_NONE;
			bool bPickup = pDropItemList->PickupItem(player, m_Index, msg);
			
			if (pDropItemList->IsEmpty(player->getPlayerID()))
			{
				//pDeadObject->OnDropItemListEmpty();
			}

			if (!bPickup)
			{
				// 通知客户端拾取错误
				if (msg != MSG_NONE)
				{
					//WarnMessageEvent::AddScreenMessage(player, msg);
				}				
				return;
			}
			DropItemEvent::ServerSendEraseDropItem(player, m_Index);
		}
		break;

	case CLIENT_PICKUP_ITEM_ALL:
		{
			S32 count = 0;
			U32 mask = 0;
			bool bPickup = pDropItemList->PickupItemAll(player, count, mask);
			bool bEmpty = pDropItemList->IsEmpty(player->getPlayerID());
			if (bPickup)
			{
				if (bEmpty)
				{
					DropItemEvent::ServerSendClearAllDropItems(player);
					player->setInteraction(NULL, Player::INTERACTION_NONE);
				}
				else
				{					
					// 发送所删除物品的索引列表
					DropItemEvent::ServerSendEraseItemsInList(player, count, mask);
				}
			}
		}
		break;
	}

#endif
}

//-----------------------------------------------------------------------------
// 任务事件
//-----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(MissionEvent);
MissionEvent::MissionEvent()
{
	mOperate	= MissionManager::DONOTHING;
	mArg1 =	mArg2 = 0;
	mNotify		= true;
}

void MissionEvent::setParam(U32 opt, int arg1 /*= -1 */, int arg2 /*= -1*/, bool notify /*= true*/)
{
	mOperate = opt;
	mArg1 = arg1;
	mArg2 = arg2;
	mNotify = notify;
}

void MissionEvent::packData(NetConnection *_conn, BitStream *stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	stream->writeFlag(mNotify);
	stream->writeInt(mOperate,	BIT_4);
	switch(mOperate)
	{
	case MissionManager::UPDATEONE:				// 更新一个任务
		{
			S32 idPos = -1;
			g_MissionManager->HasMission(player, mArg1, idPos);
			stream->write(sizeof(stMissionFlag), &player->missionInfo.Flag[idPos]);
		}
		break;
	case MissionManager::UPDATEONEANDONE:
		{
			stream->writeSignedInt(player->missionInfo.Flag[mArg1].MissionID, BIT_16);
			stream->writeSignedInt(player->missionInfo.Flag[mArg1].Name[mArg2], BIT_16);
			stream->writeSignedInt(player->missionInfo.Flag[mArg1].Value[mArg2], BIT_16);
		}
		break;
	case MissionManager::DELETEONE:				// 删除一个任务
		{
			stream->writeSignedInt(mArg1, BIT_16);
		}
		break;
	case MissionManager::DELETEONEANDONE:		// 删除一个任务的某个任务旗标
		{
			stream->writeSignedInt(player->missionInfo.Flag[mArg1].MissionID, BIT_16);
			stream->writeSignedInt(player->missionInfo.Flag[mArg1].Name[mArg2], BIT_16);
		}
		break;
	case MissionManager::UPDATEONESTATE:		// 设置一个任务的完成
		{
			stream->writeSignedInt(mArg1, BIT_16);
			stream->writeFlag(mArg2);
		}
		break;
	case MissionManager::UPDATEALLSTATE:		// 更新所有任务完成状态
		{
			// 所有任务完成状态需要分STATE_BLOCK次发送,然后拼装
			stream->writeInt(mArg1, BIT_4);
			U8* buffer = (U8*)player->missionInfo.State;
			buffer = buffer + mArg1 * BIT_STATE_BLOCK;
			stream->write(BIT_STATE_BLOCK, buffer);
		}
		break;
	case MissionManager::UPDATEONECYCLE:		// 更新某个循环任务
		{
			stream->writeInt(player->missionInfo.Cycle[mArg1].MissionID, BIT_16);
			stream->writeInt(player->missionInfo.Cycle[mArg1].EndTimes, BIT_16);
			stream->writeInt(player->missionInfo.Cycle[mArg1].EndTime, BIT_32);
		}
		break;
	case MissionManager::UPDATEALLCYCLE:		// 更新所有循环任务
		{
			// 所有循环任务数据需要分CYCLE_BLOCK次发送,然后拼装
			stream->writeInt(mArg1, BIT_4);
			U8* buffer = (U8*)player->missionInfo.Cycle;
			buffer = buffer + mArg1 * BIT_CYCLE_BLOCK;
			stream->write(BIT_CYCLE_BLOCK, buffer);
		}
		break;
	case MissionManager::DELONECYCLE:			// 删除一个循环任务
		{
			stream->writeInt(mArg1, BIT_16);
		}
		break;
	case MissionManager::UPDATEONESHAREFROM:	// 更新一个共享任务
		{
			stream->writeInt(mArg1, BIT_16);
		}
		break;
	case MissionManager::UPDATEALLSHAREFROM:	// 更新所有共享任务
		{
			S32 iSize = player->missionShareList.size();
			if(iSize > 400)
				iSize = 400;
			stream->writeInt(iSize, 9);
			for(S32 i = 0; i < iSize ; i++)
				stream->writeInt(player->missionShareList[i], BIT_16);
		}
		break;
	case MissionManager::STARTTIME:				// 开始一个限时任务
		{
			stream->writeSignedInt(mArg1, BIT_16);
			//stream->write(sizeof(stMissionFlag), &player->missionInfo.Flag[mArg1]);
		}
		break;
	case MissionManager::CONTINUETIME:			// 继续所有限时任务
		{
			//
		}
		break;
	case MissionManager::ENDTIME:				// 结束某限时任务
		{
			stream->writeSignedInt(mArg1, BIT_16);
		}
		break;
	case MissionManager::UPDATENEWBIEMISSION:	// 新手任务
		{
			//......
		}
		break;
	case MissionManager::UPDATENEWBIEHELP:		// 新手帮助
		{
			//......
		}
		break;
	default:
		break;
	}
#endif
}

void MissionEvent::unpackData(NetConnection *_conn, BitStream *stream)
{
#ifdef NTJ_CLIENT
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;

	mNotify  = stream->readFlag();
	mOperate = stream->readInt(BIT_4);
	switch(mOperate)
	{
	case MissionManager::UPDATEONE:			// 更新一个任务
		{
			U32 op = 0;
			stMissionFlag flag;
			stream->read(sizeof(stMissionFlag), &flag);
			int pos = -1;
			for(S32 i = MISSION_ACCEPT_MAX - 1; i >= 0; --i)
			{
				if(player->missionInfo.Flag[i].MissionID == flag.MissionID)
				{
					op = MissionManager::UPDATEONE;
					pos = i;
					break;
				}
				else if(player->missionInfo.Flag[i].MissionID == -1)
				{
					op = MissionManager::ADDONE;
					pos = i;
				}
			}
			if(pos != -1)
			{
				dMemcpy(&player->missionInfo.Flag[pos], &flag, sizeof(stMissionFlag));
				if(mNotify)
				{
					// 根据当前操作，执行刷新任务GUI显示
					Con::executef("RefreshMission",
									Con::getIntArg(player->getId()),
									Con::getIntArg(op),
									Con::getIntArg(flag.MissionID));
				}
			}
		}
		break;
	case MissionManager::UPDATEONEANDONE:	// 更新一个任务的某个任务旗标
		{
			S32 MissionID= stream->readSignedInt(BIT_16);
			S32 FlagName = stream->readSignedInt(BIT_16);
			S32 FlagValue = stream->readSignedInt(BIT_16);			
			S32 idPos, flagPos, oldFlagValue;
			bool ret = g_MissionManager->GetMissionFlag(player, MissionID, FlagName, oldFlagValue, idPos, flagPos);
			if(ret)
			{
				g_MissionManager->SetMissionFlag(player, MissionID, FlagName, FlagValue, idPos, flagPos);
				// 判断是否更新的是当前任务物品数量旗标
				if(FlagName >= Mission_ItemCurNum_Begin && FlagName <= Mission_ItemCurNum_End)
				{
					if(FlagValue > oldFlagValue)
					{
						U32 flag_itemid = FlagName - (Mission_ItemCurNum_Begin - Mission_ItemID_Begin);
						U32 flag_neednum = FlagName - (Mission_ItemCurNum_Begin - Mission_ItemNeedNum_Begin);
						S32 ItemID = 0;
						S32 neednum = 0;
						bool ret1 = g_MissionManager->GetMissionFlag(player, MissionID, flag_itemid, ItemID, idPos, flagPos);
						bool ret2 = g_MissionManager->GetMissionFlag(player, MissionID, flag_neednum, neednum, idPos, flagPos);
						if(ret1 && ret2)
						{
							ItemBaseData* pData = g_ItemRepository->getItemData((U32)ItemID);
							if(pData)
							{
								char* buff = Con::getArgBuffer(128);
								dSprintf(buff, 128, "获得任务物品【%s】 %d / %d", pData->getItemName(), FlagValue, neednum);
								Con::executef("SetScreenMessage", buff, Con::getIntArg( g_Color[COLOR_WARN]));
							}					
						}
					}
				}
				// 判断是否更新是任务怪当前数量旗标
				else if(FlagName >= Mission_MonsterCurNum_Begin && FlagName <= Mission_MonsterCurNum_End)
				{					
					if(FlagValue > oldFlagValue)
					{
						U32 flag_itemid = FlagName - (Mission_MonsterCurNum_Begin - Mission_MonsterID_Begin);
						U32 flag_neednum = FlagName - (Mission_MonsterCurNum_Begin - Mission_MonsterNeedNum_Begin);
						S32 NpcID = 0;
						S32 neednum = 0;
						bool ret1 = g_MissionManager->GetMissionFlag(player, MissionID, flag_itemid, NpcID, idPos, flagPos);
						bool ret2 = g_MissionManager->GetMissionFlag(player, MissionID, flag_neednum, neednum, idPos, flagPos);
						if(ret1 && ret2)
						{
							NpcObjectData* pData = g_NpcRepository.GetNpcData((U32)NpcID);
							if(pData)
							{
								char* buff = Con::getArgBuffer(128);
								dSprintf(buff, 128, "杀死怪物【%s】 %d / %d", pData->npcName, FlagValue, neednum);
								Con::executef("SetScreenMessage", buff, Con::getIntArg( g_Color[COLOR_WARN]));
							}					
						}
					}
				}
				
				// 根据当前操作，执行刷新任务GUI显示
				Con::executef("RefreshMission",
								Con::getIntArg(player->getId()),
								Con::getIntArg(MissionManager::UPDATEONE),
								Con::getIntArg(MissionID));								
			}
		}
		break;
	case MissionManager::DELETEONE:			// 删除一个任务
		{
			S32 MissionID = stream->readSignedInt(BIT_16);
			S32 idPos;
			g_MissionManager->DelMission(player, MissionID, idPos);
			if(mNotify)
			{
				// 根据当前操作，执行刷新任务GUI显示
				Con::executef("RefreshMission",
								Con::getIntArg(player->getId()),
								Con::getIntArg(MissionManager::DELETEONE),
								Con::getIntArg(MissionID));
			}
		}
		break;
	case MissionManager::DELETEONEANDONE:	// 删除一个任务的某个任务旗标
		{
			S32 MissionID= stream->readSignedInt(BIT_16);
			S32 FlagName = stream->readSignedInt(BIT_16);
			S32 idPos, flagPos;
			g_MissionManager->DelMissionFlag(player, MissionID, FlagName, idPos, flagPos);
		}
		break;
	case MissionManager::UPDATEONESTATE:	// 设置一个任务的完成
		{
			S32 MissionID = stream->readSignedInt(BIT_16);
			bool Complete = stream->readFlag();
			g_MissionManager->SetMissionState(player, MissionID, Complete);
			if(mNotify)
			{
				Con::executef("ShowCompleteMission", Con::getIntArg(MissionID));
				// 根据当前操作，执行刷新任务GUI显示
				Con::executef("RefreshMission",
								Con::getIntArg(player->getId()),
								Con::getIntArg(MissionManager::UPDATEONESTATE),
								Con::getIntArg(MissionID));
			}
		}
		break;
	case MissionManager::UPDATEALLSTATE:	// 更新所有任务完成状态
		{
			mArg1 = stream->readInt(BIT_4);
			U8* buffer = (U8*)player->missionInfo.State;
			buffer = buffer + mArg1 * BIT_STATE_BLOCK;
			stream->read(BIT_STATE_BLOCK, buffer);
			if(mNotify && mArg1 >= MissionEvent::STATE_BLOCK - 1)
			{
				// 根据当前操作，执行刷新任务GUI显示
				Con::executef("RefreshMission",
								Con::getIntArg(player->getId()),
								Con::getIntArg(MissionManager::UPDATEALLSTATE),
								Con::getIntArg(0));
			}
		}
		break;
	case MissionManager::UPDATEONECYCLE:		// 更新某个循环任务
		{
			S32 MissionID = stream->readInt(BIT_16);
			S32 EndTimes = stream->readInt(BIT_16);
			U32 EndTime = stream->readInt(BIT_32);
			g_MissionManager->SetCycleMission(player, MissionID, EndTimes, EndTime);
		}
		break;
	case MissionManager::UPDATEALLCYCLE:		// 更新所有循环任务
		{
			mArg1 = stream->readInt(BIT_4);
			U8* buffer = (U8*)player->missionInfo.Cycle;
			buffer = buffer + mArg1 * BIT_CYCLE_BLOCK;
			stream->read(BIT_CYCLE_BLOCK, buffer);
		}
		break;
	case MissionManager::DELONECYCLE:			// 删除一个循环任务
		{
			S32 MissionID = stream->readInt(BIT_16);
			g_MissionManager->DelCycleMission(player, MissionID);
		}
		break;
	case MissionManager::UPDATEONESHAREFROM:	// 更新一个共享任务
		{
			S32 iMissionID = stream->readInt(BIT_16);
		}
		break;
	case MissionManager::UPDATEALLSHAREFROM:	// 更新所有共享任务
		{
			S32 iSize = stream->readInt(9);
			player->missionShareList.clear();
			for(S32 i = 0; i < iSize ; i++)
			{
				S32 iMissionID = stream->readInt(BIT_16);
				player->missionShareList.push_back(iMissionID);
			}

			// 根据当前操作，执行刷新任务GUI显示
			Con::executef("RefreshMission", 
							Con::getIntArg(player->getId()),
							Con::getIntArg(MissionManager::UPDATEALLSHAREFROM),
							Con::getIntArg(0));
		}
		break;
	case MissionManager::STARTTIME:			// 开始限时任务
		{
			S32 MissionID = stream->readSignedInt(BIT_16);
			g_MissionManager->StartTime(player, MissionID);
		}
		break;
	case MissionManager::CONTINUETIME:		// 继续开始所有限时任务
		{
			g_MissionManager->ContinueTime(player);			
		}
		break;		
	case MissionManager::ENDTIME:			// 结束限时任务
		{
			S32 MissionID = stream->readSignedInt(BIT_16);
		}
		break;
	case MissionManager::UPDATENEWBIEMISSION:// 新手任务
		{
			
		}
		break;
	case MissionManager::UPDATENEWBIEHELP:	// 新手帮助
		{
			
		}
		break;
	default:
		break;
	}	
#endif
}

//----------------------------------------------------------------------------
// 施放技能消息
//----------------------------------------------------------------------------
IMPLEMENT_CO_SERVEREVENT_V1(CastSpellEvent);

CastSpellEvent::CastSpellEvent(U32 SkillId, U32 serverId, U32 itemType /* = 0 */, U32 itemIdx /* = 0 */, U64 UID /* = 0 */)
{
	mTarget = true;
	mSkillId = SkillId;
	mTargetServerId = serverId;
	mItemType = itemType;
	mItemIdx = itemIdx;
	mItemUID = UID;
}

CastSpellEvent::CastSpellEvent(U32 SkillId, Point3F& pos, U32 itemType /* = 0 */, U32 itemIdx /* = 0 */, U64 UID /* = 0 */)
{
	mTarget = false;
	mSkillId = SkillId;
	mPos = pos;
	mItemType = itemType;
	mItemIdx = itemIdx;
	mItemUID = UID;
}

void CastSpellEvent::pack(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_CLIENT
	stream->writeInt(mSkillId, Base::Bit32);
	if(Macro_IsDefaultSkill(mSkillId))
		return;
	if(stream->writeFlag(mTarget))
	{
		stream->writeInt(mTargetServerId, Base::Bit32);
	}
	else
	{
		stream->write(mPos.x);
		stream->write(mPos.y);
		stream->write(mPos.z);
	}
	if(stream->writeFlag(mItemUID))
	{
		stream->write(mItemType);
		stream->write(mItemIdx);
		stream->writeBits(64, &mItemUID);
	}
#endif
}

void CastSpellEvent::unpack(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_SERVER
	mSkillId = stream->readInt(Base::Bit32);
	if(Macro_IsDefaultSkill(mSkillId))
		return;
	mTarget = stream->readFlag();
	if(mTarget)
	{
		mTargetServerId = stream->readInt(Base::Bit32);
	}
	else
	{
		stream->read(&mPos.x);
		stream->read(&mPos.y);
		stream->read(&mPos.z);
	}
	if(stream->readFlag())
	{
		stream->read(&mItemType);
		stream->read(&mItemIdx);
		stream->readBits(64, &mItemUID);
	}
#endif
}

void CastSpellEvent::process(NetConnection *_conn)
{
#ifdef NTJ_SERVER
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player;
	if(!conn || !(player = dynamic_cast<Player*>(conn->getControlObject())))
		return;

	if(Macro_IsDefaultSkill(mSkillId))
		return;

	if(mTarget)
	{
		GameObject* obj = dynamic_cast<GameObject*>(Sim::findObject(mTargetServerId));
		if(obj)
			player->CastSpell(mSkillId, *obj, mItemType, mItemIdx, mItemUID);
	}
	else
		player->CastSpell(mSkillId, mPos, mItemType, mItemIdx, mItemUID);
#endif
}

//----------------------------------------------------------------------------
// 施放技能消息
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(CombatLogEvent);

CombatLogEvent::CombatLogEvent()
:	mCombatInfo(0),
mSource(0),
mTarget(0),
mGeneralId(0),
mValHP(0),
mValMP(0),
mValPP(0)
{
}

void CombatLogEvent::setParam(CombatInfo info, S32 src, S32 tgt, U32 id, S32 hp, S32 mp, S32 pp, U32 mask)
{
	mCombatInfo = info;
	mCombatMask = mask;
	mSource = src;
	mTarget = tgt;
	mGeneralId = id;
	mValHP = hp;
	mValMP = mp;
	mValPP = pp;
}

void CombatLogEvent::pack(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_SERVER
	stream->writeInt(mCombatInfo, CombatInfoBits);
	stream->writeInt(mCombatMask, Base::Bit8);

	if(stream->writeFlag(mSource != 0))
		stream->writeInt(mSource, NetConnection::GhostIdBitSize);
	if(stream->writeFlag(mTarget != 0))
		stream->writeInt(mTarget, NetConnection::GhostIdBitSize);

	stream->writeInt(mGeneralId, GeneralIdBits);

	if(stream->writeFlag(mValHP != 0))
		stream->writeSignedInt(mValHP, ValBits);
	if(stream->writeFlag(mValMP != 0))
		stream->writeSignedInt(mValMP, ValBits);
	if(stream->writeFlag(mValPP != 0))
		stream->writeSignedInt(mValPP, ValBits);
#endif
}

void CombatLogEvent::unpack(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_CLIENT
	mCombatInfo = stream->readInt(CombatInfoBits);
	mCombatMask = stream->readInt(Base::Bit8);

	if(stream->readFlag())
		mSource = stream->readInt(NetConnection::GhostIdBitSize);
	if(stream->readFlag())
		mTarget = stream->readInt(NetConnection::GhostIdBitSize);

	mGeneralId = stream->readInt(GeneralIdBits);

	if(stream->readFlag())
		mValHP = stream->readSignedInt(ValBits);
	if(stream->readFlag())
		mValMP = stream->readSignedInt(ValBits);
	if(stream->readFlag())
		mValPP = stream->readSignedInt(ValBits);
#endif
}

void CombatLogEvent::show(U32 info, GameObject* src, GameObject* tgt, U32 id, S32 hp, S32 mp, S32 pp, U32 mask)
{
	if(info >= CombatLogEvent::CombatInfo_Max)
		return;

#ifdef NTJ_SERVER
	GameConnection* conn = NULL;
	if(src && (src->getGameObjectMask() & PetObjectType))
	{
		PetObject* pet = (PetObject*)src;
		conn = pet->getMaster() ? pet->getMaster()->getControllingClient() : NULL;
	}
	else
		conn = src ? src->getControllingClient() : NULL;
	if(conn)
	{
		CombatLogEvent* pEvent = new CombatLogEvent();
		pEvent->setParam((CombatLogEvent::CombatInfo)info, src ? conn->getGhostIndex(src) : 0, tgt ? conn->getGhostIndex(tgt) : 0, id, hp, mp, pp, mask);
		conn->postNetEvent(pEvent);
	}
	if(tgt == src)
		return;

	if(tgt && (tgt->getGameObjectMask() & PetObjectType))
	{
		PetObject* pet = (PetObject*)tgt;
		conn = pet->getMaster() ? pet->getMaster()->getControllingClient() : NULL;
	}
	else
		conn = tgt ? tgt->getControllingClient() : NULL;
	if(conn)
	{
		CombatLogEvent* pEvent = new CombatLogEvent();
		pEvent->setParam((CombatLogEvent::CombatInfo)info, src ? conn->getGhostIndex(src) : 0, tgt ? conn->getGhostIndex(tgt) : 0, id, hp, mp, pp, mask);
		conn->postNetEvent(pEvent);
	}
#endif
#ifdef NTJ_CLIENT
	StringTableEntry sName = src ? (src->getControllingClient() ? GetLocalStr(MSG_COMBAT_YOU) : src->getObjectName()) : GetLocalStr(MSG_COMBAT_UNKNOWN_TARGET);
	StringTableEntry tName = tgt ? (tgt->getControllingClient() ? GetLocalStr(MSG_COMBAT_YOU) : tgt->getObjectName()) : GetLocalStr(MSG_COMBAT_UNKNOWN_TARGET);

	switch(info)
	{
	case CombatInfo_Spell:
		{
			SkillData* pSkill = g_SkillRepository.GetSkill(id);
			if(pSkill)
			{
				//MessageEvent::show(SHOWTYPE_NOTIFY, avar(GetLocalStr(MSG_COMBATLOG_SPELL), pSkill->GetName()), SHOWPOS_CHAT);
				//g_UIMouseGamePlay->addDamageText(src,pSkill->GetName(),g_Color[COLOR_YELLOW],0);
			}
		}
		break;
	case CombatInfo_Buff:
		break;
	case CombatInfo_Item:
		break;
	case CombatInfo_Damage:
		{
			//if(mask & CombatMask_Critical)
			//	Con::printf("critical !!!");
			if(hp < 0)
			{
				//MessageEvent::show(SHOWTYPE_NOTIFY, avar(GetLocalStr(MSG_COMBATLOG_DAMAGE), sName, tName, -hp), SHOWPOS_CHAT);
                if (src->getGameObjectMask() & PetObjectType)
                {
                    g_UIMouseGamePlay->addDamageNumber(tgt,hp, GameObject::GREEN_P,0, mask & CombatMask_Critical);
                }
                else if (tgt->getGameObjectMask() & PetObjectType)
                {
                    g_UIMouseGamePlay->addDamageNumber(tgt,hp, GameObject::PINK_P ,0, mask & CombatMask_Critical);
                }
                else
                {
                    g_UIMouseGamePlay->addDamageNumber(tgt,hp, (tgt && tgt->getControllingClient()) ? GameObject::RED : \
                        ((mask & CombatMask_Critical)?GameObject::ORANGE :GameObject::YELLOW),0, mask & CombatMask_Critical);
                }
                
                // 客户端被爆击
				if (tgt && tgt->getControllingClient() && (mask & CombatMask_Critical))
					g_ScreenFXMgr.Add(new CScreenFX("~/data/mask/blood", 1000.0f, 1.0f, 0.0f));
			}
			else if(hp > 0)
			{
				//MessageEvent::show(SHOWTYPE_NOTIFY, avar(GetLocalStr(MSG_COMBATLOG_HEAL), sName, tName, hp), SHOWPOS_CHAT);				
                g_UIMouseGamePlay->addDamageNumber(tgt,hp,GameObject::GREEN,1,mask & CombatMask_Critical);
			}
			if(mp < 0)
			{				
                g_UIMouseGamePlay->addDamageNumber(tgt,mp,GameObject::BLUE,0,mask & CombatMask_Critical);
			}
			else if(mp > 0)
			{				
                g_UIMouseGamePlay->addDamageNumber(tgt,mp,GameObject::BLUE,1,mask & CombatMask_Critical);
			}
		}
		break;
	case CombatInfo_Miss:
		{
			//MessageEvent::show(SHOWTYPE_NOTIFY, avar(GetLocalStr(MSG_COMBATLOG_MISS), sName, tName), SHOWPOS_CHAT);
            g_UIMouseGamePlay->addDamageText(src,&GameObject::mTextTex[GameObject::MISS]);
		}
		break;
	case CombatInfo_Dodge:
		{
			//MessageEvent::show(SHOWTYPE_NOTIFY, avar(GetLocalStr(MSG_COMBATLOG_DODGE), tName, sName), SHOWPOS_CHAT);
			g_UIMouseGamePlay->addDamageText(tgt,&GameObject::mTextTex[GameObject::DODGE]);
		}
		break;
	case CombatInfo_Immunity:
		{
			//MessageEvent::show(SHOWTYPE_NOTIFY, avar(GetLocalStr(MSG_COMBATLOG_IMMUNITY), tName, sName), SHOWPOS_CHAT);
			g_UIMouseGamePlay->addDamageText(tgt,&GameObject::mTextTex[GameObject::IMMUNE]);
		}
		break;
	case CombatInfo_Absorb:
		{
			g_UIMouseGamePlay->addDamageText(tgt,&GameObject::mTextTex[GameObject::ABSORB]);
		}
		break;
	case CombatInfo_Kill:
		{
			//MessageEvent::show(SHOWTYPE_NOTIFY, avar(GetLocalStr(MSG_COMBATLOG_KILL), sName, tName), SHOWPOS_CHAT);
		}
		break;
	}
#endif
}

void CombatLogEvent::process(NetConnection *_conn)
{
#ifdef NTJ_CLIENT
	GameObject* pSrc = dynamic_cast<GameObject*>(_conn->resolveGhost(mSource));
	GameObject* pTgt = dynamic_cast<GameObject*>(_conn->resolveGhost(mTarget));

	show(mCombatInfo, pSrc, pTgt, mGeneralId, mValHP, mValMP, mValPP, mCombatMask);
#endif
}

//----------------------------------------------------------------------------
// 技能消息
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(SkillEvent);

void SkillEvent::packData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_SERVER
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;

	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));

	if(!stream->writeFlag(valid))
		return;

	if(stream->writeFlag(mSkillId))
	{
		// 有非零的技能ID，添加或删除技能
		stream->writeInt(mSkillId, Base::Bit32);
		if(stream->writeFlag(mAdd))
			stream->writeFlag(mSkillId == player->GetSkillTable().GetBaseSkillId(Macro_GetSkillSeriesId(mSkillId)));
	}
	else
	{
		// 技能ID为0，发送所有技能信息
		// 自动攻击技能
		stream->writeInt(player->GetSkillTable().GetDefaultSkill().GetBaseSkillId(), Base::Bit32);

		SkillTable::SkillTableType& table = player->GetSkillTable().GetSkill();
		if(stream->writeFlag(table.empty()))
			return;
		stream->writeInt((S32)table.size(), Base::Bit8);
		SkillTable::SkillTableType::iterator it = table.begin();
		for (;it != table.end(); it++)
		{
			Skill* skill = it->second;
			if(stream->writeFlag(skill))
				stream->writeInt(skill->GetBaseSkillId(), Base::Bit32);
		}
	}
#endif
}

void SkillEvent::unpackData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_CLIENT
	if(!stream->readFlag())
		return;

	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;

	if(!conn || !conn->getControlObject() || (player = dynamic_cast<Player*>(conn->getControlObject())) == NULL)
		return;

	// 单个技能
	if(stream->readFlag())
	{
		U32 skillId = stream->readInt(Base::Bit32);
		// 新添加技能
		if(stream->readFlag())
		{
			// 服务器确有此技能
			if(stream->readFlag())
			{
				bool isAdded = player->GetSkillTable().GetSkill(Macro_GetSkillSeriesId(skillId));
				Skill* pSkill = player->AddSkill(skillId);

				if(!pSkill)
				{
					AssertFatal(false, avar("SkillEvent::unpack - Can't add skill <%d> in client", skillId));
					return;
				}

				// 加入客户端的技能提示slot
                if (!isAdded && !pSkill->GetData()->IsFlags(SkillData::Flags_AutoCast))
                {					
					player->skillList_Hint.AddSkillHintShortcut(player, Macro_GetSkillSeriesId(skillId));
					Con::evaluatef("OpenSkillHintGui(1);");
                }
				
				//更新技能学习列表
				player->skillList_Study.UpdataSkillStudyUI(skillId);

				//更新技能预览表
				Con::executef("UpdateSkillSeriesView",Con::getIntArg(pSkill->GetData()->GetSchool()));
			}
		}
		else
			player->RemoveSkill(skillId);
	}
	// 发送所有技能
	else
	{
		// 清除客户端的技能slot
		player->skillList_Common.Reset();

		// 普通攻击技能
		U32 skillId = stream->readInt(Base::Bit32);
		player->GetSkillTable().AddDefaultSkill(skillId);

		// 加入客户端的技能slot
		player->skillList_Common.AddSkillShortcut(player, Macro_GetSkillSeriesId(skillId));

		if(stream->readFlag())
			return;
		S32 skillCount = stream->readInt(Base::Bit8);
		for (S32 i=0; i<skillCount; i++)
		{
			if(stream->readFlag())
			{
				skillId = stream->readInt(Base::Bit32);
				Skill* pSkill = player->AddSkill(skillId);
				AssertFatal(pSkill, avar("SkillEvent::unpack - Can't add skill <%d> in client", skillId));
				// 加入客户端的技能slot
				//player->skillList_Common.AddSkillShortcut(player, Macro_GetSkillSeriesId(skillId));
			}
		}
	}
#endif
}

//----------------------------------------------------------------------------
// 技能提示栏消息
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(SkillHintEvent);
SkillHintEvent::SkillHintEvent()
{
	mIndex = 0;
	mFlag = 0;
}

SkillHintEvent::SkillHintEvent(S32 index, S32 flag)
{
	mIndex = index;
	mFlag = flag;
}

void SkillHintEvent::packData(NetConnection* _conn, BitStream* stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	stream->writeInt(mFlag, Base::Bit8);
#endif
}


void SkillHintEvent::unpackData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_CLIENT
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;

	mIndex = stream->readInt(Base::Bit8);
	mFlag =  stream->readInt(Base::Bit8);

	player->skillList_Hint.SetSlot(mIndex,NULL,true);
	player->skillList_Hint.CheckCurrentSlot();
#endif
}

//----------------------------------------------------------------------------
// 宠物信息更新消息
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(PetEvent);
void PetEvent::pack(NetConnection *_conn, BitStream * stream)
{
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	PetTable& petTable = (PetTable&)player->getPetTable();
	petTable.writePetInfo(player, stream, mSlot, mFlag);
}

void PetEvent::unpack(NetConnection *_conn, BitStream * stream)
{
	if(!stream->readFlag())
		return;
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	if(!conn || !conn->getControlObject() || (player = dynamic_cast<Player*>(conn->getControlObject())) == NULL)
		return;

	PetTable& petTable = (PetTable&)player->getPetTable();
	petTable.readPetInfo(player, stream);
}

//----------------------------------------------------------------------------
// 骑宠信息更新消息
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(MountPetEvent);
void MountPetEvent::pack(NetConnection *_conn, BitStream * stream)
{
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	PetTable& petTable = (PetTable&)player->getPetTable();
	petTable.writeMountInfo(player, stream, mSlot);
}

void MountPetEvent::unpack(NetConnection *_conn, BitStream * stream)
{
	if(!stream->readFlag())
		return;
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	if(!conn || !conn->getControlObject() || (player = dynamic_cast<Player*>(conn->getControlObject())) == NULL)
		return;

	PetTable& petTable = (PetTable&)player->getPetTable();
	petTable.readMountInfo(player, stream);
}

//----------------------------------------------------------------------------
// 元神信息更新消息
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(SpiritEvent);
void SpiritEvent::pack(NetConnection *_conn, BitStream * stream)
{
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;
	SpiritTable& spiritTable = player->getSpiritTable();
	spiritTable.PackData(player, stream, mSlot, mFlag);
}

void SpiritEvent::unpack(NetConnection *_conn, BitStream * stream)
{
	if(!stream->readFlag())
		return;
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	if(!conn || !conn->getControlObject() || (player = dynamic_cast<Player*>(conn->getControlObject())) == NULL)
		return;

	SpiritTable& spiritTable = player->getSpiritTable();
	spiritTable.UnpackData(player, stream);
}
//-----------------------------------------------------------------------------
// 装备鉴定栏事件
//-----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(IdentifyEvent);
IdentifyEvent::IdentifyEvent()
{
	mIndex = 0;
	mFlag = 0;
}

IdentifyEvent::IdentifyEvent(U32 index, U32 flag)
{
	mIndex = index;
	mFlag = flag;
}
void IdentifyEvent::packData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (pCon && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!bstream->writeFlag(valid))
		return;

	// 槽位索引位置
	bstream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	bstream->writeInt(mFlag, Base::Bit8);

	// 判定槽位对象是否存在
	ShortcutObject* pSlot = player->identifylist.GetSlot(mIndex);
	ItemShortcut* pItem = NULL;
	valid = pSlot && (pItem = dynamic_cast<ItemShortcut*>(pSlot));
	if(!bstream->writeFlag(valid))
		return;

	pItem->packUpdate(bstream);
#endif
}

void IdentifyEvent::unpackData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !bstream->readFlag())
		return;
	mIndex = bstream->readInt(Base::Bit8);
	mFlag = bstream->readInt(Base::Bit8);

	if(bstream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pPlayer->identifylist.GetSlot(mIndex);
		if (pSlot)
		{
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(bstream);
		}
		else
		{
			//确认客户端不存在槽位对象
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(bstream);
			pPlayer->identifylist.SetSlot(mIndex, pShortcut);
		}
	}
	else
	{
		// 确认服务端不存在槽位对象
		pPlayer->identifylist.SetSlot(mIndex, NULL);
	}
	
	if(mIndex == 0)
	{
		Con::executef("showIdentifyState");
	}
#endif
}

//-----------------------------------------------------------------------------
// 装备强化栏事件
//-----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(EquipStrengthenEvent);
EquipStrengthenEvent::EquipStrengthenEvent()
{
	mIndex = 0;
	mFlag = 0;
}

EquipStrengthenEvent::EquipStrengthenEvent(U32 index, U32 flag)
{
	mIndex = index;
	mFlag = flag;
}

void EquipStrengthenEvent::packData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_SERVER
	GameConnection *con = dynamic_cast<GameConnection *>(_conn);
	Player *pPlayer = NULL;
	bool valid = (con && con->getControlObject() && (pPlayer = dynamic_cast<Player*>(con->getControlObject())));
	if (!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	stream->writeInt(mFlag, Base::Bit8);

	ShortcutObject *pShortcut = pPlayer->strengthenlist.GetSlot(mIndex);
	ItemShortcut *pItemShortcut = NULL;
	valid = (pShortcut && (pItemShortcut = dynamic_cast<ItemShortcut *>(pShortcut)));
	if (!stream->writeFlag(valid))
		return;
	pItemShortcut->packUpdate(stream);
#endif
}

void EquipStrengthenEvent::unpackData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;
	
	mIndex = stream->readInt(Base::Bit8);
	mFlag = stream->readInt(Base::Bit8);

	if(stream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pPlayer->strengthenlist.GetSlot(mIndex);
		if (pSlot)
		{
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(stream);
		}
		else
		{
			//确认客户端不存在槽位对象
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(stream);
			pPlayer->strengthenlist.SetSlot(mIndex, pShortcut);
		}
	}
	else
	{
		// 确认服务端不存在槽位对象
		pPlayer->strengthenlist.SetSlot(mIndex, NULL);
	}

	if (mIndex == 0)  //客户端得到需要强化的装备，需要检查是否使能"强化"按钮
	{	
		Con::executef("EquipStrengthenWnd_RefreshDialog");
	}
#endif
}

//-----------------------------------------------------------------------------
// 任务奖励物品事件
//-----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(MissionItemEvent);
MissionItemEvent::MissionItemEvent()
{
	mIndex = 0;
	mFlag = 0;
}

MissionItemEvent::MissionItemEvent(U32 index, U32 flag)
{
	mIndex = index;
	mFlag = flag;
}
void MissionItemEvent::packData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (pCon && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!bstream->writeFlag(valid))
		return;

	// 槽位索引位置
	bstream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	bstream->writeInt(mFlag, Base::Bit8);

	// 判定槽位对象是否存在
	ShortcutObject* pSlot = player->identifylist.GetSlot(mIndex);
	ItemShortcut* pItem = NULL;
	valid = pSlot && (pItem = dynamic_cast<ItemShortcut*>(pSlot));
	if(!bstream->writeFlag(valid))
		return;

	pItem->packUpdate(bstream);
#endif
}

void MissionItemEvent::unpackData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !bstream->readFlag())
		return;
	mIndex = bstream->readInt(Base::Bit8);
	mFlag = bstream->readInt(Base::Bit8);

	if(bstream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pPlayer->identifylist.GetSlot(mIndex);
		if (pSlot)
		{
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(bstream);
		}
		else
		{
			//确认客户端不存在槽位对象
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(bstream);
			pPlayer->identifylist.SetSlot(mIndex, pShortcut);
		}
	}
	else
	{
		// 确认服务端不存在槽位对象
		pPlayer->identifylist.SetSlot(mIndex, NULL);
	}
#endif
}

//-----------------------------------------------------------------------------
// 装备镶嵌栏事件
//-----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(MountGemEvent);
MountGemEvent::MountGemEvent()
{
	mIndex = 0;
	mFlag = 0;
}

MountGemEvent::MountGemEvent(U32 index, U32 flag)
{
	mIndex = index;
	mFlag = flag;
}

void MountGemEvent::packData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (_conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	stream->writeInt(mFlag, Base::Bit8);

	ShortcutObject *pShortcut = pPlayer->mountGemList.GetSlot(mIndex);
	ItemShortcut *pItemShortcut = NULL;
	valid = (pShortcut && (pItemShortcut = dynamic_cast<ItemShortcut *>(pShortcut)));
	if (!stream->writeFlag(valid))
		return;
	pItemShortcut->packUpdate(stream);
#endif
}

void MountGemEvent::unpackData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;
	mIndex = stream->readInt(Base::Bit8);
	mFlag = stream->readInt(Base::Bit8);
	bool bFlag = stream->readFlag();

	if(bFlag)
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pPlayer->mountGemList.GetSlot(mIndex);
		if (pSlot)
		{
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(stream);
		}
		else
		{
			//确认客户端不存在槽位对象
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(stream);
			pPlayer->mountGemList.SetSlot(mIndex, pShortcut);
		}
	}
	else
	{
		pPlayer->mountGemList.SetSlot(mIndex, NULL);
	}

	if (mIndex != 0)		//更新此镶嵌槽对应的部分
	{
		Con::executef("EquipEmbedGemWnd_SetSelectCol", Con::getIntArg(mIndex));
		Con::executef("EquipEmbedGemWnd_OnRefreshPart", Con::getIntArg(mIndex));
	}
	else
	{
		if (bFlag)
			Con::executef("EquipEmbedGemWnd_OpenWnd");
	}
#endif
}

//-----------------------------------------------------------------------------
// 装备打孔栏事件
//-----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(PunchHoleEvent);
PunchHoleEvent::PunchHoleEvent()
{
	mIndex = 0;
	mFlag = 0;
}

PunchHoleEvent::PunchHoleEvent(U32 index, U32 flag)
{
	mIndex = index;
	mFlag = flag;
}

void PunchHoleEvent::packData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (_conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	stream->writeInt(mFlag, Base::Bit8);

	ShortcutObject *pShortcut = pPlayer->punchHoleList.GetSlot(mIndex);
	ItemShortcut *pItemShortcut = NULL;
	valid = (pShortcut && (pItemShortcut = dynamic_cast<ItemShortcut *>(pShortcut)));
	if (!stream->writeFlag(valid))
		return;
	pItemShortcut->packUpdate(stream);
#endif
}

void PunchHoleEvent::unpackData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;
	mIndex = stream->readInt(Base::Bit8);
	mFlag = stream->readInt(Base::Bit8);

	if(stream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pPlayer->punchHoleList.GetSlot(mIndex);
		if (pSlot)
		{
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(stream);
		}
		else
		{
			//确认客户端不存在槽位对象
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(stream);
			pPlayer->punchHoleList.SetSlot(mIndex, pShortcut);
		}
	}
	else
	{
		pPlayer->punchHoleList.SetSlot(mIndex, NULL);
	}

	if (mIndex == 0)	//刷新界面
	{
		Con::executef("PunchHole_RefreshDialog");
	}
#endif
}
//----------------------------------------------------------------------------
// 生活技能事件
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(LivingSkillEvent);
void LivingSkillEvent::packData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_SERVER
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));

	if (!stream->writeFlag(valid))
		return;

	if (stream->writeFlag(mSkillID))
	{
		stream->writeInt(mSkillID,Base::Bit32);
		stream->writeFlag(mflag);
	}
	else
	{
		LivingSkillTable::LivingSkillTableMap& tabMap = pPlayer->pLivingSkill->getLivingSkillTable();
		if (stream->writeFlag(tabMap.empty()))
			return;

		stream->writeInt((S32)tabMap.size(),Base::Bit8);
		LivingSkillTable::LivingSkillTableMap::iterator it = tabMap.begin();
		for (; it != tabMap.end(); ++it)
		{
			LivingSkill* pSkill = it->second;
			if(stream->writeFlag(pSkill))
			{
				stream->writeInt(pSkill->getData()->getID(),Base::Bit32);
				stream->writeInt(pSkill->getRipe(),Base::Bit32);
			}
		}
	}
#endif
}

void LivingSkillEvent::unpackData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_CLIENT
	 if (!stream->readFlag())
		 return;

	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	if(!conn || !conn->getControlObject() || !(pPlayer = dynamic_cast<Player*>(conn->getControlObject())))
		return;

	if(!pPlayer->pLivingSkill)
		return;

	if (stream->readFlag())
	{
		U32 skillID = stream->readInt(Base::Bit32);
		if(stream->readFlag())
		{
			LivingSkill* pSkill = pPlayer->pLivingSkill->addLivingSkill(skillID);
			AssertFatal(pSkill, avar("LivingSkillEvent::unpack - Can't add skill <%d> in client", skillID));
			if(!pSkill)
				return;

			pPlayer->pLivingSkill->updateLiveingSkillView(skillID);
			if(pPlayer->mLivingSkillList)
				pPlayer->mLivingSkillList->AddLivingSkillShortcut(skillID,pSkill->getRipe());
		}
		else
			pPlayer->pLivingSkill->removeLivingSkill(skillID);
	}
	else
	{
		if(!stream->readFlag())
		{
			U32 count = stream->readInt(Base::Bit8);
			for (int i=0; i<count; i++)
			{
				if (stream->readFlag())
				{
					U32 skillID = stream->readInt(Base::Bit32);
					U32 ripe    = stream->readInt(Base::Bit32);
					LivingSkill* pSkill = pPlayer->pLivingSkill->addLivingSkill(skillID,ripe);
					AssertFatal(pSkill, avar("LivingSkillEvent::unpack - Can't add skill <%d> in client", skillID));
				}
			}
		}
		pPlayer->pLivingSkill->initLivingSkillPreview();
	}
#endif
}
//------------------------------------------------------------------------------
// 生活技能熟练度事件
//------------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(LivingSkillGuerdonEvent);
void LivingSkillGuerdonEvent::packData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_SERVER
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));

	if (!stream->writeFlag(valid))
		return;

	if (stream->writeFlag(mSkillID))
	{
		stream->writeInt(mSkillID,Base::Bit32);
		stream->writeInt(mExtGuerdon,Base::Bit32);
	}
#endif
}
void LivingSkillGuerdonEvent::unpackData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_CLIENT
	if (!stream->readFlag())
		return;

	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	if(!conn || !conn->getControlObject() || !(pPlayer = dynamic_cast<Player*>(conn->getControlObject())))
		return;

	if(stream->readFlag())
	{
		U32 skillID = stream->readInt(Base::Bit32);
		U32 extGuredon = stream->readInt(Base::Bit32);
		pPlayer->setLivingSkillGuerdon(skillID,extGuredon);
	}
#endif
}
//------------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(SocialEvent);

void SocialEvent::packData( NetConnection* pCon, BitStream* stream )
{
#ifdef NTJ_SERVER
	stream->writeInt( eventType, 16 );
	stream->writeInt( srcPlayerId, 32 );
	stream->writeInt( reverseId, 32 );
#endif
}

void SocialEvent::unpackData( NetConnection* pCon, BitStream* stream )
{
#ifdef NTJ_CLIENT
	eventType = stream->readInt( 16 );
	srcPlayerId = stream->readInt( 32 );
	reverseId = stream->readInt( 32 );

	// execute the script
	Con::evaluatef("OnSocialResponse(%d,%d,%d);", eventType, srcPlayerId,eventType, reverseId  );
#endif
}

//----------------------------------------------------------------------------
// 配方事件
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(PrescriptionEvent);
void PrescriptionEvent::AddIDToList(U32 id)
{
	mIDList.push_back(id);
}

void PrescriptionEvent::packData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_SERVER
	Player* player = NULL;
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));

	if(!stream->writeFlag(valid))
		return;

	stream->writeInt((U32)mIDList.size(), Base::Bit8);
	for( S32 i = 0; i < mIDList.size(); i++)
	{
		stream->writeInt(mIDList[i], Base::Bit32);
	}
#endif
}
void PrescriptionEvent::unpackData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_CLIENT
	if(!stream->readFlag())
		return;

	Player* pPlayer = NULL;
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	if (!conn || !(pPlayer = dynamic_cast<Player*>(conn->getControlObject())))
		return;
	if(!pPlayer->pPrescription)
		return;

	U16 iSize = stream->readInt(Base::Bit8);
	for (int i=0; i<iSize; i++)
	{
		U32 iSerialID = stream->readInt(Base::Bit32);
		pPlayer->pPrescription->addPrescription(iSerialID);
	}
#endif
}
//------------------------------------------------------------------------------
// 摆摊栏事件
//------------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(StallEvent);
StallEvent::StallEvent()
{
	mIndex = 0;
}

StallEvent::StallEvent(U32 index)
{
	mIndex = index;
}

void StallEvent::packData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);

	// 判定槽位对象是否存在
	StallItem* pSlot= player->individualStallList.mSlots[mIndex];
	if(!stream->writeFlag(pSlot != NULL))
		return;
	stream->writeInt(pSlot->money, Base::Bit32);
	if(stream->writeFlag(pSlot->pItem != NULL))
	{
		pSlot->pItem->packUpdate(stream);
	}
#endif
}

void StallEvent::unpackData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !bstream->readFlag())
		return;

	// 槽位索引位置
	mIndex = bstream->readInt(Base::Bit8);
	if(!bstream->readFlag())
	{
		player->individualStallList.SetSlot(mIndex, NULL);
	}
	else
	{
		StallItem* item = new StallItem;
		item->money = bstream->readInt(Base::Bit32);
		
		if(player->individualStallList.mSlots[mIndex])
		{
			if(player->individualStallList.mSlots[mIndex]->pItem)
				delete player->individualStallList.mSlots[mIndex]->pItem;
			delete player->individualStallList.mSlots[mIndex];
		}
		if(!bstream->readFlag())
		{
			item->pItem = NULL;
		}
		else
		{
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(bstream);
			item->pItem = pShortcut;
		}
		player->individualStallList.mSlots[mIndex] = item;
	}
#endif
}
//------------------------------------------------------------------------------
// 对方摆摊栏事件
//------------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(StallTargetEvent);
StallTargetEvent::StallTargetEvent()
{
	mIndex	= 0;
	mTargetPlayerID	= 0;
}

StallTargetEvent::StallTargetEvent(U32 targetPlayerID, S32 index)
{
	mIndex	= index;
	mTargetPlayerID	= targetPlayerID;
}
void StallTargetEvent::packData(NetConnection *_conn, BitStream *stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新对方玩家ID
	stream->writeInt(mTargetPlayerID, Base::Bit32);

	Player* pTarget = g_ServerGameplayState->GetPlayer(mTargetPlayerID);
	if(!stream->writeFlag(pTarget != NULL))
		return;

	// 判定槽位对象是否存在
	StallItem* pSlot = pTarget->individualStallList.mSlots[mIndex];
	if(!stream->writeFlag(pSlot != NULL))
		return;

	stream->writeInt(pSlot->money, Base::Bit32);
	if(!stream->writeFlag(pSlot->pItem != NULL))
		return;
	pSlot->pItem->packUpdate(stream);
#endif
}

void StallTargetEvent::unpackData(NetConnection *_conn, BitStream *stream)
{
#ifdef NTJ_CLIENT
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;

	// 槽位索引位置
	mIndex = stream->readInt(Base::Bit8);
	// 更新对方玩家ID
	mTargetPlayerID  = stream->readInt(Base::Bit32);
	if(!stream->readFlag())
		return;

	Player* pTarget = g_ClientGameplayState->findPlayer(mTargetPlayerID);
	if(!stream->readFlag())
	{
		if(pTarget)
			pTarget->individualStallList.SetSlot(mIndex, NULL);
		return;
	}
	else
	{
		StallItem* item = new StallItem;
		if(pTarget && pTarget->individualStallList.mSlots[mIndex])
		{
			pTarget->individualStallList.SetSlot(mIndex, NULL);
		}
		item->money = stream->readInt(Base::Bit32);
		if(!stream->readFlag())
			item->pItem = NULL;
		else
		{
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(stream);
			item->pItem = pShortcut;
		}
		if(pTarget)
			pTarget->individualStallList.mSlots[mIndex] = item;
	}
#endif
}

//-----------------------------------------------------------------------------
// 临时栏事件
//-----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(TempListEvent);
TempListEvent::TempListEvent()
{
	mIndex = 0;
	mFlag = 0;
}

TempListEvent::TempListEvent(U32 index, U32 flag)
{
	mIndex = index;
	mFlag = flag;
}

void TempListEvent::packData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (_conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志
	stream->writeInt(mFlag, Base::Bit8);

	ShortcutObject *pShortcut = pPlayer->tempList.GetSlot(mIndex);
	if (!pShortcut)
	{
		stream->writeFlag(false);
		return;
	}
	stream->writeFlag(true);
	stream->writeInt(pShortcut->getType(), Base::Bit8);

	switch(pShortcut->getType())
	{
	case ShortcutObject::SHORTCUT_ITEM:
		{
			ItemShortcut *pItem = (ItemShortcut*)pShortcut;
			pItem->packUpdate(stream);
		}
		break;
	case ShortcutObject::SHORTCUT_SKILL:
		{
			SkillShortcut *pSkill = (SkillShortcut*)pShortcut;
			pSkill->packUpdate(stream);
		}
		break;
	case ShortcutObject::SHORTCUT_LIVINGSKILL:
		{
			LivingSkillShortcut *pLivingSkill = (LivingSkillShortcut*)pShortcut;
			pLivingSkill->packUpdate(stream);
		}
		break;
	case ShortcutObject::SHORTCUT_PET:
		{
			PetShortcut *pPet = (PetShortcut*)pShortcut;
			pPet->packUpdate(stream);
		}
		break;
	case ShortcutObject::SHORTCUT_MOUNT_PET:
		{
			MountShortcut *pMount = (MountShortcut*)pShortcut;
			pMount->packUpdate(stream);
		}
		break;
	}
#endif
}

void TempListEvent::unpackData(NetConnection *_conn, BitStream * stream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;
	mIndex = stream->readInt(Base::Bit8);
	mFlag = stream->readInt(Base::Bit8);	

	if(stream->readFlag())
	{
		S32 nType = stream->readInt(Base::Bit8);
		ShortcutObject *pShortcut = pPlayer->tempList.GetSlot(mIndex);
		switch(nType)
		{
		case ShortcutObject::SHORTCUT_ITEM:
			{
				if (pShortcut && pShortcut->getType() == nType)
				{
					pShortcut->unpackUpdate(stream);
				}
				else
				{
					ItemShortcut *pItem = ItemShortcut::CreateEmptyItem();
					pItem->unpackUpdate(stream);
					pPlayer->tempList.SetSlot(mIndex, pItem);
				}
			}
			break;
		case ShortcutObject::SHORTCUT_SKILL:
			{
				if (pShortcut && pShortcut->getType() == nType)
				{
					pShortcut->unpackUpdate(stream);
				}
				else
				{
					SkillShortcut *pSkill = new SkillShortcut();
					pSkill->unpackUpdate(stream);
					pPlayer->tempList.SetSlot(mIndex, pSkill);
				}
			}
			break;
		case ShortcutObject::SHORTCUT_LIVINGSKILL:
			{
				if (pShortcut && pShortcut->getType() == nType)
				{
					pShortcut->unpackUpdate(stream);
				}
				else
				{
					LivingSkillShortcut *pLivingSkill = new LivingSkillShortcut();
					pLivingSkill->unpackUpdate(stream);
					pPlayer->tempList.SetSlot(mIndex, pLivingSkill);
				}
			}
			break;
		case ShortcutObject::SHORTCUT_PET:
			{
				if (pShortcut && pShortcut->getType() == nType)
				{
					pShortcut->unpackUpdate(stream);
				}
				else
				{
					PetShortcut *pPet = PetShortcut::CreateEmptyPetItem();
					pPet->unpackUpdate(stream);
					pPlayer->tempList.SetSlot(mIndex, pPet);
				}
			}
			break;
		case ShortcutObject::SHORTCUT_MOUNT_PET:
			{
				if (pShortcut && pShortcut->getType() == nType)
				{
					pShortcut->unpackUpdate(stream);
				}
				else
				{
					MountShortcut *pMount = MountShortcut::CreateEmptyMountItem();
					pMount->unpackUpdate(stream);
					pPlayer->tempList.SetSlot(mIndex, pMount);
				}
			}
			break;
		default:
			break;
		}
	}
	else
	{
		pPlayer->tempList.SetSlot(mIndex, NULL);
	}

	switch(pPlayer->tempList.GetOptType())
	{
	case TempList::OptType_PetIdentify:
		{
			Con::executef("PetCheckupWnd_OnRefreshWnd");
		}
		break;
	case TempList::OptType_PetInsight:
		{
			Con::executef("PetStrengthenWnd_OnRefreshWnd");
		}
		break;
	case TempList::OptType_PetLianHua:
		{
			Con::executef("PetDecompoundWnd_OnRefreshWnd");
		}
		break;
	case TempList::OptType_PetHuanTong:
		{
			Con::executef("PetHuanTong_OnRefreshWnd");
		}
		break;
	case TempList::OptType_SpiritSkill:
		{
			Con::executef("SpiritJueZhaoWnd_RefreshWnd");
		}
		break;
	}
#endif
}



//------------------------------------------------------------------------------
// 摆摊宠物栏事件
//------------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(StallPetEvent);
StallPetEvent::StallPetEvent()
{
	mIndex = 0;
}

StallPetEvent::StallPetEvent(U32 index)
{
	mIndex = index;
}

void StallPetEvent::packData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!bstream->writeFlag(valid))
		return;

	// 槽位索引位置
	bstream->writeInt(mIndex, Base::Bit8);

	stStallPet* pSlot = player->stallPetList.mSlots[mIndex];
	if(!bstream->writeFlag(pSlot != NULL))
		return;
	bstream->writeInt(pSlot->money, Base::Bit32);
	if(bstream->writeFlag(pSlot->petSlot != NULL))
	{
		pSlot->petSlot->packUpdate(bstream);
	}

#endif
}

void StallPetEvent::unpackData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !bstream->readFlag())
		return;

	// 槽位索引位置
	mIndex = bstream->readInt(Base::Bit8);
	if(!bstream->readFlag())
	{
		player->stallPetList.SetSlot(mIndex, NULL, 0);
	}
	else
	{
		stStallPet* item = new stStallPet;
		item->money = bstream->readInt(Base::Bit32);

		if(player->stallPetList.mSlots[mIndex])
		{
			if(player->stallPetList.mSlots[mIndex]->petSlot)
				delete player->stallPetList.mSlots[mIndex]->petSlot;
			delete player->stallPetList.mSlots[mIndex];
		}
		if(!bstream->readFlag())
		{
			item->petSlot = NULL;
		}
		else
		{
			PetShortcut* pShortcut = PetShortcut::CreateEmptyPetItem();
			pShortcut->unpackUpdate(bstream);
			item->petSlot = pShortcut;
		}
		player->stallPetList.mSlots[mIndex] = item;
	}
#endif
}

//------------------------------------------------------------------------------
// 察看摆摊宠物栏事件
//------------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(StallPetTargetEvent);
StallPetTargetEvent::StallPetTargetEvent()
{
	mIndex	= 0;
	mTargetPlayerID	= 0;
}

StallPetTargetEvent::StallPetTargetEvent(U32 targetPlayerID, S32 index)
{
	mIndex = index;
	mTargetPlayerID = targetPlayerID;
}

void StallPetTargetEvent::packData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!bstream->writeFlag(valid))
		return;

	// 槽位索引位置
	bstream->writeInt(mIndex, Base::Bit8);
	// 更新对方玩家ID
	bstream->writeInt(mTargetPlayerID, Base::Bit32);

	Player* pTarget = g_ServerGameplayState->GetPlayer(mTargetPlayerID);
	if(!bstream->writeFlag(pTarget != NULL))
		return;

	// 判定槽位对象是否存在
	stStallPet* pSlot = pTarget->stallPetList.mSlots[mIndex];
	if(!bstream->writeFlag(pSlot != NULL))
		return;

	bstream->writeInt(pSlot->money, Base::Bit32);
	if(!bstream->writeFlag(pSlot->petSlot != NULL))
		return;
	pSlot->petSlot->packUpdate(bstream);
	PetTable& petTable = (PetTable&)pTarget->getPetTable();
	petTable.writePetInfo(pTarget, bstream, pSlot->petSlot->getSlotIndex());
#endif
}

void StallPetTargetEvent::unpackData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !bstream->readFlag())
		return;

	// 槽位索引位置
	mIndex = bstream->readInt(Base::Bit8);
	// 更新对方玩家ID
	mTargetPlayerID  = bstream->readInt(Base::Bit32);
	if(!bstream->readFlag())
		return;

	Player* pTarget = g_ClientGameplayState->findPlayer(mTargetPlayerID);
	if(!bstream->readFlag())
	{
		if(pTarget)
			pTarget->stallPetList.SetSlot(mIndex, NULL, 0);
		return;
	}
	else
	{
		stStallPet* item = new stStallPet;
		item->money = bstream->readInt(Base::Bit32);
		if(pTarget && pTarget->stallPetList.mSlots[mIndex])
		{
			pTarget->stallPetList.SetSlot(mIndex, NULL, item->money);
		}

		if(!bstream->readFlag())
			item->petSlot = NULL;
		else
		{
			PetShortcut* pShortcut = PetShortcut::CreateEmptyPetItem();
			pShortcut->unpackUpdate(bstream);

			pShortcut->SetTargetType(PetShortcut::TARGET_OTHER);
			PetTable& petTable = (PetTable&)pTarget->getPetTable();
			petTable.readPetInfo(pTarget, bstream);
			item->petSlot = pShortcut;
		}
		if(pTarget)
			pTarget->stallPetList.mSlots[mIndex] = item;
	}
#endif
}

//------------------------------------------------------------------------------
// 摆摊查找栏事件
//------------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(StallLookUpEvent);
StallLookUpEvent::StallLookUpEvent()
{
	mIndex = 0;
}
StallLookUpEvent::StallLookUpEvent(U32 index)
{
	mIndex = index;
}
void StallLookUpEvent::packData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);

	// 判定槽位对象是否存在
	stStallBuyItem* pSlot= player->stallBuyItemList.mSlots[mIndex];
	if(!stream->writeFlag(pSlot != NULL))
		return;
	stream->writeInt(pSlot->money, Base::Bit32);
	stream->writeInt(pSlot->num, Base::Bit16);
	if(stream->writeFlag(pSlot->buyItem != NULL))
	{
		pSlot->buyItem->packUpdate(stream);
	}
#endif
}

void StallLookUpEvent::unpackData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !bstream->readFlag())
		return;

	// 槽位索引位置
	mIndex = bstream->readInt(Base::Bit8);
	if(!bstream->readFlag())
	{
		player->stallBuyItemList.SetSlot(mIndex, NULL, 0, 0);
	}
	else
	{
		stStallBuyItem* item = new stStallBuyItem;
		item->money = bstream->readInt(Base::Bit32);
		item->num = bstream->readInt(Base::Bit16);

		if(player->stallBuyItemList.mSlots[mIndex])
		{
			if(player->stallBuyItemList.mSlots[mIndex]->buyItem)
				delete player->stallBuyItemList.mSlots[mIndex]->buyItem;
			delete player->stallBuyItemList.mSlots[mIndex];
		}
		if(!bstream->readFlag())
		{
			item->buyItem = NULL;
		}
		else
		{
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(bstream);
			item->buyItem = pShortcut;
		}
		player->stallBuyItemList.mSlots[mIndex] = item;
	}
#endif
}

//------------------------------------------------------------------------------
// 对方摆摊收购栏事件
//------------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(StallLookUpTargetEvent);
StallLookUpTargetEvent::StallLookUpTargetEvent()
{
	mIndex	= 0;
	mTargetPlayerID	= 0;
}

StallLookUpTargetEvent::StallLookUpTargetEvent(U32 targetPlayerID, S32 index)
{
	mIndex	= index;
	mTargetPlayerID	= targetPlayerID;
}
void StallLookUpTargetEvent::packData(NetConnection *_conn, BitStream *stream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新对方玩家ID
	stream->writeInt(mTargetPlayerID, Base::Bit32);

	Player* pTarget = g_ServerGameplayState->GetPlayer(mTargetPlayerID);
	if(!stream->writeFlag(pTarget != NULL))
		return;

	// 判定槽位对象是否存在
	stStallBuyItem* pSlot = pTarget->stallBuyItemList.mSlots[mIndex];
	if(!stream->writeFlag(pSlot != NULL))
		return;

	stream->writeInt(pSlot->money, Base::Bit32);
	stream->writeInt(pSlot->num, Base::Bit16);
	if(!stream->writeFlag(pSlot->buyItem != NULL))
		return;
	pSlot->buyItem->packUpdate(stream);
#endif
}

void StallLookUpTargetEvent::unpackData(NetConnection *_conn, BitStream *stream)
{
#ifdef NTJ_CLIENT
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;

	// 槽位索引位置
	mIndex = stream->readInt(Base::Bit8);
	// 更新对方玩家ID
	mTargetPlayerID  = stream->readInt(Base::Bit32);
	if(!stream->readFlag())
		return;

	Player* pTarget = g_ClientGameplayState->findPlayer(mTargetPlayerID);
	if(!stream->readFlag())
	{
		if(pTarget)
			pTarget->stallBuyItemList.SetSlot(mIndex, NULL, 0, 0);
		return;
	}
	else
	{
		stStallBuyItem* item = new stStallBuyItem;
		item->money = stream->readInt(Base::Bit32);
		item->num = stream->readInt(Base::Bit16);
		if(pTarget && pTarget->stallBuyItemList.mSlots[mIndex])
		{
			pTarget->stallBuyItemList.SetSlot(mIndex, NULL, item->money, item->num);
		}
		
		if(!stream->readFlag())
			item->buyItem = NULL;
		else
		{
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(stream);
			item->buyItem = pShortcut;
		}
		if(pTarget)
			pTarget->stallBuyItemList.mSlots[mIndex] = item;
	}
#endif
}
//----------------------------------------------------------------------------
// 师徒事件
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(MPManagerEvent);
void MPManagerEvent::packData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_SERVER
	Player* player = NULL;
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())) 
		&& player->getMPInfo());

	if(!stream->writeFlag(valid))
		return;

	// 更新初始化标志
	stream->writeFlag(mIsFirstUpdate);
	// 更新标志
	stream->writeInt(mMask, Base::Bit16);

	// 等级
	if(mMask & MPManager::MASTERLEVEL)
	{
		stream->writeInt(player->getMPInfo()->getMasterLevel(), 3);
	}
	//徒弟数量
	if(mMask & MPManager::PRENTICENUM)
	{
		stream->writeInt(player->getMPInfo()->getCurrPrenticeNum(), 3);
	}
	// 师德
	if(mMask & MPManager::MASTEREXPLOIT)
	{
		stream->writeInt(player->getMPInfo()->getCurrMasterExp(), Base::Bit32);
	}
	// 经验
	if(mMask & MPManager::EXPPOOL)
	{
		stream->writeInt(player->getMPInfo()->getExpPool(), Base::Bit32);
	}
	// 徒状态
	if(mMask & MPManager::PRENTICEFLAGE)
	{
		stream->writeFlag(player->getMPInfo()->getPrenticeState());
	}
	// 出师
	if(mMask & MPManager::LEAVEMASTER)
	{
		stream->writeFlag(player->getMPInfo()->getFarPrentice());
	}
	//师傅公告信息
	if(mMask & MPManager::CALLBOARDTEXT)
	{
		if(stream->writeFlag(player->getMPInfo()->getMasterLevel()>0))
			stream->writeString(player->getMPInfo()->getCallboardString(),MPM_CALLBOARD_MAXSTRING);

	}
	//拜师等级
	if(mMask & MPManager::STUDYLEVEL)
	{
		stream->writeInt(player->getMPInfo()->getStudyLevel(),Base::Bit16);
	}

#endif
}

void MPManagerEvent::unpackData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_CLIENT
	if(!stream->readFlag())
		return;

	Player* pPlayer = NULL;
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	if (!conn || !(pPlayer = dynamic_cast<Player*>(conn->getControlObject()))
		|| !pPlayer->getMPInfo())
		return;

	mIsFirstUpdate = stream->readFlag();
	mMask = stream->readInt(Base::Bit16);

	// 等级
	if(mMask & MPManager::MASTERLEVEL)
	{
		U8 level = stream->readInt(3);
		if(mIsFirstUpdate)
		{
			pPlayer->getMPInfo()->setMasterLevel(level);
		}
		else
		{
			U8 oldLevel = pPlayer->getMPInfo()->getMasterLevel();
			pPlayer->getMPInfo()->addMasterLevel(pPlayer,level-oldLevel);
		}
	}
	//徒弟数量
	if(mMask & MPManager::PRENTICENUM)
	{
		pPlayer->getMPInfo()->setPrenticeNum(pPlayer,stream->readInt(3));
	}
	// 师德
	if(mMask & MPManager::MASTEREXPLOIT)
	{
		S32 exp = stream->readInt(Base::Bit32);
		if(mIsFirstUpdate)
		{
			pPlayer->getMPInfo()->setCurrMasterExp(exp);
		}
		else
		{
			S32 oldExp = pPlayer->getMPInfo()->getCurrMasterExp();
			pPlayer->getMPInfo()->addMasterExploit(pPlayer,exp-oldExp);
		}
	}
	// 经验
	if(mMask & MPManager::EXPPOOL)
	{
		S32 expPool = stream->readInt(Base::Bit32);
		if(mIsFirstUpdate)
		{
			pPlayer->getMPInfo()->setExpPool(expPool);
		}
		else
		{
			S32 oldExp = pPlayer->getMPInfo()->getExpPool();
			pPlayer->getMPInfo()->addExpPool(pPlayer,expPool-oldExp);
		}

	}
	// 徒状态
	if(mMask & MPManager::PRENTICEFLAGE)
	{
		bool state = stream->readFlag();
		pPlayer->getMPInfo()->setPrenticeState(pPlayer,state);
	}
	// 出师
	if(mMask & MPManager::LEAVEMASTER)
	{
		bool state = stream->readFlag();
		pPlayer->getMPInfo()->setFarPrentice(pPlayer,state);
	}
	//师傅公告信息
	if(mMask & MPManager::CALLBOARDTEXT)
	{
		bool flage = stream->readFlag();
		if(flage)
		{
			char buff[MPM_CALLBOARD_MAXSTRING] = {0,};
			stream->readString(buff,MPM_CALLBOARD_MAXSTRING);
			pPlayer->getMPInfo()->setCallboardString(buff);
		}
	}
	//拜师等级
	if(mMask & MPManager::STUDYLEVEL)
	{
		U16 iLevel = stream->readInt(Base::Bit16);
		pPlayer->getMPInfo()->setStudyLevel(pPlayer,iLevel);
	}

#endif
}
//摆摊交易聊天信息
IMPLEMENT_CO_CLIENTEVENT_V1(StallRecordEvent);
StallRecordEvent::StallRecordEvent()
{
	mFlag		= false;
	mIndex		= 0;
	mPlayerID	= 0;
}
StallRecordEvent::StallRecordEvent(U32 playerID, bool flag, U32 index)
{
	mFlag		= flag;
	mIndex		= index;
	mPlayerID	= playerID;
}

void StallRecordEvent::packData(NetConnection* pCon, BitStream* bStream)
{
#ifdef NTJ_SERVER
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!bStream->writeFlag(valid))
		return;

	Player* stallPlayer = g_ServerGameplayState->GetPlayer(mPlayerID);
	if(!bStream->writeFlag(stallPlayer != NULL))
		return;
	bStream->writeInt(mPlayerID, Base::Bit32);
	if(bStream->writeFlag(mFlag))
	{
		bool bIsExits = mIndex <= stallPlayer->individualStallList.mChatInfoList.size();
		if(bIsExits && stallPlayer->individualStallList.mChatInfoList[mIndex])
			bIsExits = true;
		else
			bIsExits = false;
		if(bStream->writeFlag(bIsExits))
		{
			stChatRecord* pRecord = stallPlayer->individualStallList.mChatInfoList[mIndex];
			bStream->writeInt(pRecord->time, Base::Bit32);
			bStream->writeInt(pRecord->senderID, Base::Bit32);
			bStream->writeInt(pRecord->receiverID, Base::Bit32);
			bStream->writeString(pRecord->senderName);
			bStream->writeString(pRecord->receiverName);
			bStream->writeInt(pRecord->contentLen, Base::Bit16);
			if(pRecord->contentLen > 0)
				bStream->writeBits(pRecord->contentLen * 8, pRecord->chatContent);
		}			
	}
	else
	{
		bool bIsExits = mIndex <= stallPlayer->individualStallList.mTradeInfoList.size();
		if(bIsExits && stallPlayer->individualStallList.mTradeInfoList[mIndex])
			bIsExits = true;
		else
			bIsExits = false;
		if(bStream->writeFlag(bIsExits))
		{
			stTradeRecord* pRecord = stallPlayer->individualStallList.mTradeInfoList[mIndex];
			bStream->writeInt(pRecord->time, Base::Bit32);
			bStream->writeInt(pRecord->senderID, Base::Bit32);
			bStream->writeInt(pRecord->receiverID, Base::Bit32);
			bStream->writeString(pRecord->senderName);
			bStream->writeString(pRecord->receiverName);
			bStream->writeInt(pRecord->type, Base::Bit8);
			bStream->writeInt(pRecord->money, Base::Bit32);
			if(pRecord->type == stTradeRecord::TRADE_ITEM)
			{
				if(bStream->writeFlag(pRecord->trade.item != NULL))
					pRecord->trade.item->WriteData(bStream);
			}
			else if(pRecord->type == stTradeRecord::TRADE_PET)
			{
				if(bStream->writeFlag(pRecord->trade.pet != NULL))
					pRecord->trade.pet->WriteData(bStream);
			}	
		}
	}	
#endif
}

void StallRecordEvent::unpackData(NetConnection* pCon, BitStream* bStream)
{
#ifdef NTJ_CLIENT
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !bStream->readFlag())
		return;
	
	if(!bStream->readFlag())
		return;
	mPlayerID = bStream->readInt(Base::Bit32);
	Player* stallPlayer = g_ClientGameplayState->findPlayer(mPlayerID);
	char szSenderName[COMMON_STRING_LENGTH];
	char szReceiverName[COMMON_STRING_LENGTH];
	if(bStream->readFlag())
	{
		if(bStream->readFlag())
		{
			stChatRecord* pRecord = new stChatRecord;
			pRecord->time = bStream->readInt(Base::Bit32);
			pRecord->senderID = bStream->readInt(Base::Bit32);
			pRecord->receiverID = bStream->readInt(Base::Bit32);
			bStream->readString(szSenderName,COMMON_STRING_LENGTH);
			bStream->readString(szReceiverName,COMMON_STRING_LENGTH);
			pRecord->senderName = StringTable->insert(szSenderName);
			pRecord->receiverName = StringTable->insert(szReceiverName);
			pRecord->contentLen = bStream->readInt(Base::Bit16);
			pRecord->chatContent = NULL;
			if(pRecord->contentLen > 1)
			{
				pRecord->chatContent = new char[pRecord->contentLen];
				bStream->readBits(pRecord->contentLen * 8, pRecord->chatContent);
			}
			if(stallPlayer)
				stallPlayer->individualStallList.mChatInfoList.push_back(pRecord);
		}			
	}
	else
	{
		if(bStream->readFlag())
		{
			stTradeRecord* pRecord = new stTradeRecord;
			pRecord->time = bStream->readInt(Base::Bit32);
			pRecord->senderID = bStream->readInt(Base::Bit32);
			pRecord->receiverID = bStream->readInt(Base::Bit32);
			bStream->readString(szSenderName,COMMON_STRING_LENGTH);
			bStream->readString(szReceiverName,COMMON_STRING_LENGTH);
			pRecord->senderName = StringTable->insert(szSenderName);
			pRecord->receiverName = StringTable->insert(szReceiverName);
			pRecord->type = bStream->readInt(Base::Bit8);
			pRecord->money = bStream->readInt(Base::Bit32);
			if(pRecord->type == stTradeRecord::TRADE_ITEM)
			{
				pRecord->trade.item = new stItemInfo;
				if(bStream->readFlag())
					pRecord->trade.item->ReadData(bStream);
			}
			else if(pRecord->type == stTradeRecord::TRADE_PET)
			{
				pRecord->trade.pet = new stPetInfo;
				if(bStream->readFlag())
					pRecord->trade.pet->ReadData(bStream);
			}
			if(stallPlayer)
				stallPlayer->individualStallList.mTradeInfoList.push_back(pRecord);
		}
		
	}	
#endif
}

IMPLEMENT_CO_CLIENTEVENT_V1(MailEvent);

MailEvent::MailEvent()
{
	mIndex = 0;
	mPageCount = 6;
	mPlayerId = 0;
	
}

MailEvent::MailEvent( int index, int pageCount, int playerId )
{
	mIndex = index;
	mPageCount = pageCount;
	mPlayerId = playerId;
}

void MailEvent::pack(NetConnection* pCon, BitStream* pStream )
{
#ifdef NTJ_SERVER
	pStream->writeInt( mPlayerId, 32 );
	pStream->writeInt( mIndex, 16 );
	pStream->writeInt( mPageCount, 16 );

	mHeader.PackData( *pStream );
	pStream->writeInt( mMailItemList.size(), 16 );

	std::list< stMailItem >::iterator it;
	for( it = mMailItemList.begin(); it != mMailItemList.end(); it++ )
	{
		it->PackData( *pStream );
	}
#endif
}

void MailEvent::unpack(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	mPlayerId = bstream->readInt( 32 );
	mIndex = bstream->readInt( 16 );
	mPageCount = bstream->readInt( 16 );

	mHeader.UnpackData( *bstream );
	int count = bstream->readInt( 16 );

	for( int i = 0; i < count; i++ )
	{
		stMailItem item;
		item.UnpackData( *bstream );

		mMailItemList.push_back( item );
	}
#endif
}

void MailEvent::process( NetConnection *pCon )
{
#ifdef NTJ_CLIENT
	// here we update the UI

	char buf[1024];
	char key[16];

	if( mIndex == 0 )
		Con::evaluatef( "setMailArgs(%d,%d,%d);", mHeader.nRecvCount, mHeader.nLetterCount, mHeader.nPackageCount );

	std::list< stMailItem >::iterator it;
	for( it = mMailItemList.begin(); it != mMailItemList.end(); it++ )
	{
		stMailItem& item = *it;
		sprintf_s( buf, sizeof(buf), "<c cid='mail_%d' cf = 'createMailItem' cmd='\"%d %d\",\"%s\", \"%s\", \"%s\", \"2012-12-21\"' />", item.id, item.nSender, item.id, item.title, item.szSenderName, item.buf);	
		sprintf_s( key, sizeof(key), "mail_%d", item.id );
		Con::executef( "addMailItem", buf, key );
	}
#endif
}

#ifdef NTJ_SERVER
void MailEvent::setMailHeader( stMailHeader& mailHeader )
{
	memcpy( &mHeader, &mailHeader, sizeof( stMailHeader ) );
}

void MailEvent::addMailItem( stMailItem& item )
{
	mMailItemList.push_back( item );
}
#endif

IMPLEMENT_CO_CLIENTEVENT_V1(ZoneChatEvent);

ZoneChatEvent::ZoneChatEvent()
{
	mChannelId = 0;
	name[0] = 0;
	msg[0] = 0;
}

ZoneChatEvent::ZoneChatEvent( int channelId, const char* name, const char* msg )
{
	mChannelId = channelId;

	strcpy_s( this->name, sizeof( this->name ), name );
	strcpy_s( this->msg, sizeof( this->msg ), msg );
}

void ZoneChatEvent::pack( NetConnection* pCon, BitStream* pStream )
{
#ifdef NTJ_SERVER
	pStream->writeInt( mChannelId, 8 );
	pStream->writeString( name, COMMON_STRING_LENGTH );
	pStream->writeBits(MAX_MSG_LENGTH * 8, msg );
#endif
}

void ZoneChatEvent::unpack( NetConnection* pCon, BitStream* pStream )
{
#ifdef NTJ_CLIENT
	mChannelId = pStream->readInt( 8 );
	pStream->readString( name,COMMON_STRING_LENGTH );
	pStream->readBits(MAX_MSG_LENGTH * 8, msg );
#endif
}

void ZoneChatEvent::process( NetConnection *pCon )
{
#ifdef NTJ_CLIENT
	Con::evaluatef( "AddChatMessage( %d, \"%s\", 0, \"%s\");", mChannelId, msg, name );
#endif
}

//-----------------------------------------------------------------------------
// 装备修理栏事件
//-----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(RepairEvent);
RepairEvent::RepairEvent()
{
	mIndex = 0;
}

RepairEvent::RepairEvent(U32 index)
{
	mIndex = index;
}

void RepairEvent::packData(NetConnection* _conn, BitStream* stream)
{
#ifdef NTJ_SERVER
	GameConnection *con = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (con && con->getControlObject() && (pPlayer = dynamic_cast<Player*>(con->getControlObject())));
	if (!stream->writeFlag(valid))
		return;

	// 槽位索引位置
	stream->writeInt(mIndex, Base::Bit8);
	// 更新行为标志

	ShortcutObject* pShortcut = pPlayer->mRepairList.GetSlot(mIndex);
	ItemShortcut* pItemShortcut = NULL;
	valid = (pShortcut && (pItemShortcut = dynamic_cast<ItemShortcut*>(pShortcut)));
	if (!stream->writeFlag(valid))
		return;
	pItemShortcut->packUpdate(stream);
#endif
}

void RepairEvent::unpackData(NetConnection* _conn, BitStream* stream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !stream->readFlag())
		return;

	mIndex = stream->readInt(Base::Bit8);

	if(stream->readFlag())
	{
		// 确认服务端存在槽位对象
		ShortcutObject* pSlot = pPlayer->mRepairList.GetSlot(mIndex);
		if (pSlot)
		{
			//确认客户端存在槽位对象
			pSlot->unpackUpdate(stream);
		}
		else
		{
			//确认客户端不存在槽位对象
			ItemShortcut* pShortcut = ItemShortcut::CreateEmptyItem();
			pShortcut->unpackUpdate(stream);
			pPlayer->mRepairList.SetSlot(mIndex, pShortcut);
		}
		if(mIndex == 0)
		{
			ShortcutObject* obj = pPlayer->mRepairList.GetSlot(0);
			if(obj)
			{
				ItemShortcut* pItem = (ItemShortcut*)obj;
				Res* pRes = NULL;
				if(!pItem || !(pRes = pItem->getRes()))
					return;
				U32 iNeedMoney = pRes->getRepairNeedMoney(1);
				U32 iNeedMoney1 = pRes->getRepairNeedMoney1();
				Con::executef("showRepairNeedMoney", Con::getIntArg(iNeedMoney), Con::getIntArg(iNeedMoney1));
			}
		}
	}
	else
	{
		// 确认服务端不存在槽位对象
		pPlayer->mRepairList.SetSlot(mIndex, NULL);
	}
#endif
}


//----------------------------------------------------------------------------
// 物品分解事件
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(ItemSplitEvent);
void ItemSplitEvent::packData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_SERVER
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid)
		return;

	stream->writeInt(mIndex,Base::Bit8);

	ShortcutObject* pShortcut = pPlayer->mItemSplitList.GetSlot(mIndex);
	ItemShortcut* pItem = NULL;
	valid = (pShortcut && (pItem = dynamic_cast<ItemShortcut*>(pShortcut)));

	if(!stream->writeFlag(valid))
		return;

	pItem->packUpdate(stream);

#endif
}

void ItemSplitEvent::unpackData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid)
		return;

	mIndex = stream->readInt(Base::Bit8);
	if(stream->readFlag())
	{
		ShortcutObject* pShortcut = pPlayer->mItemSplitList.GetSlot(mIndex);
		if(pShortcut)
		{
			pShortcut->unpackUpdate(stream);
		}
		else
		{
			ItemShortcut* pNewItem = ItemShortcut::CreateEmptyItem();
			pNewItem->unpackUpdate(stream);
			pPlayer->mItemSplitList.SetSlot(mIndex,pNewItem);
			return;
		}
	}
	else
	{
		pPlayer->mItemSplitList.SetSlot(mIndex,NULL);
	}
#endif
}
//----------------------------------------------------------------------------
// 物品合成事件
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(ItemComposeEvent);
void ItemComposeEvent::packData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_SERVER
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid)
		return;

	stream->writeInt(mIndex,Base::Bit8);

	ShortcutObject* pShortcut = pPlayer->mItemComposeList.GetSlot(mIndex);
	ItemShortcut* pItem = NULL;
	valid = (pShortcut && (pItem = dynamic_cast<ItemShortcut*>(pShortcut)));

	if(!stream->writeFlag(valid))
		return;

	pItem->packUpdate(stream);

	//清除特殊合成信息
	if(mIndex == 0 && pPlayer->mItemComposeList.getItemIdTag() != 0)
		pPlayer->mItemComposeList.clearComposeInfo();
#endif
}

void ItemComposeEvent::unpackData(NetConnection *pCon, BitStream *stream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid)
		return;

	mIndex = stream->readInt(Base::Bit8);
	if(stream->readFlag())
	{
		ShortcutObject* pShortcut = pPlayer->mItemComposeList.GetSlot(mIndex);
		if(pShortcut)
		{
			pShortcut->unpackUpdate(stream);
		}
		else
		{
			ItemShortcut* pNewItem = ItemShortcut::CreateEmptyItem();
			pNewItem->unpackUpdate(stream);
			pPlayer->mItemComposeList.SetSlot(mIndex,pNewItem);
		}

		//清除特殊合成信息
		if(mIndex == 0 && pPlayer->mItemComposeList.getItemIdTag() != 0)
			pPlayer->mItemComposeList.clearComposeInfo();
	}
	else
	{
		pPlayer->mItemComposeList.SetSlot(mIndex,NULL);
	}
#endif
}

//----------------------------------------------------------------------------
// 更新玩家摆摊收购栏事件
//----------------------------------------------------------------------------
IMPLEMENT_CO_CLIENTEVENT_V1(StallerMoneyEvent);
StallerMoneyEvent::StallerMoneyEvent()
{
	mMoney = 0;
	mStallerId = 0;
}

StallerMoneyEvent::StallerMoneyEvent(U32 playerID, U32 money)
{
	mStallerId = playerID;
	mMoney = mMoney;
}

void StallerMoneyEvent::packData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_SERVER
	// 判断玩家对象是否有效
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* player = NULL;
	bool valid = (conn && conn->getControlObject() && (player = dynamic_cast<Player*>(conn->getControlObject())));
	if(!bstream->writeFlag(valid))
		return;
	Player* pTarget = g_ServerGameplayState->GetPlayer(mStallerId);
	if(!bstream->writeFlag(pTarget != NULL))
		return;
	bstream->writeInt(mStallerId, Base::Bit32);
	bstream->writeInt(mMoney, Base::Bit32);
	
#endif
}

void StallerMoneyEvent::unpackData(NetConnection *pCon, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	GameConnection* conn = dynamic_cast<GameConnection*>(pCon);
	Player* pPlayer = NULL;
	bool valid = (conn && conn->getControlObject() && (pPlayer = dynamic_cast<Player*>(conn->getControlObject())));
	if(!valid || !bstream->readFlag())
		return;
	if(!bstream->readFlag())
		return;
	mStallerId = bstream->readInt(Base::Bit32);
	mMoney = bstream->readInt(Base::Bit32);
	Player* pStaller = g_ClientGameplayState->findPlayer(mStallerId);
	if(!pStaller)
		return;
	pStaller->setMoney(mMoney);
#endif
}

//----------------------------------------------------------------------------
// 更新商城购物栏事件到服务端
//----------------------------------------------------------------------------
IMPLEMENT_CO_SERVEREVENT_V1(BasketItemEvent);
BasketItemEvent::BasketItemEvent()
{
	mType = 1;
	mBasketSize = 0;
	mBasketVector.clear();
}

BasketItemEvent::BasketItemEvent(U32 type)
{
	mType = type;
	mBasketSize = 0;
	mBasketVector.clear();
}

BasketItemEvent::~BasketItemEvent()
{
	for(int i=0; i<mBasketVector.size(); ++i)
	{
		if(mBasketVector[i])
			delete mBasketVector[i];
		mBasketVector[i] = NULL;
	}
	mBasketVector.clear();
	mBasketSize = 0;
}
void BasketItemEvent::pack(NetConnection *ps, BitStream *bstream)
{
#ifdef NTJ_CLIENT
	S32 iSize = mBasketVector.size();
	bstream->writeInt(iSize, Base::Bit8);
	bstream->writeInt(mType, Base::Bit8);
	for(int i=0; i<iSize; i++)
	{
		bstream->writeInt(mBasketVector[i]->goodID, Base::Bit16);
		bstream->writeInt(mBasketVector[i]->num, Base::Bit8);
	}
#endif
}

void BasketItemEvent::unpack(NetConnection *ps, BitStream *bstream)
{
#ifdef NTJ_SERVER
	mBasketSize = bstream->readInt(Base::Bit8);
	mType = bstream->readInt(Base::Bit8);
	for(int i=0; i<mBasketSize; ++i)
	{
		stBasketItem* pBasketItem = new stBasketItem;
		pBasketItem->goodID = bstream->readInt(Base::Bit16);
		pBasketItem->num	= bstream->readInt(Base::Bit8);
		mBasketVector.push_back(pBasketItem);
	}
#endif
}

void BasketItemEvent::addBasketItem(U32 id, U32 num)
{
	stBasketItem* pBasketItem = new stBasketItem;
	pBasketItem->goodID = id;
	pBasketItem->num = num;
	mBasketVector.push_back(pBasketItem);
}

void BasketItemEvent::process(NetConnection *ps)
{
#ifdef NTJ_SERVER
	GameConnection* conn = dynamic_cast<GameConnection*>(ps);

	Player* player = NULL;
	if(!conn || !(player = dynamic_cast<Player*>(conn->getControlObject())))
		return;

	U32 iAllMoney = 0;
	stBasketItem* pBasketItem = NULL;
	for(int i=0; i<mBasketVector.size(); ++i)
	{
		pBasketItem = mBasketVector[i];
		if(pBasketItem)
		{
			SuperMarketRes* pMarketRes = g_MarketItemReposity->getSuperMarketRes(pBasketItem->goodID);
			if(!pMarketRes)
				continue;
			iAllMoney += (pMarketRes->mMarketItemPrice)*(pBasketItem->num);
		}
	}
	if(!player->canReduceMoney(iAllMoney, 1))
	{
		MessageEvent::send(ps, SHOWTYPE_NOTIFY, MSG_PLAYER_MONEYNOTENOUGH);
		return;
	}

	pBasketItem = NULL;

	for(int i=0; i<mBasketVector.size(); ++i)
	{
		pBasketItem = mBasketVector[i];
		if(pBasketItem)
		{
			SuperMarketRes* pMarketRes = g_MarketItemReposity->getSuperMarketRes(pBasketItem->goodID);
			if(!pMarketRes)
				continue;
			g_ItemManager->putItem(player->getPlayerID(), pMarketRes->mMarketItemID, pBasketItem->num);
		}
	}

	enWarnMessage msg = MSG_NONE;
	msg = g_ItemManager->batchItem(player->getPlayerID());

	if(msg != MSG_NONE)
	{
		MessageEvent::send(ps,SHOWTYPE_NOTIFY,msg);
		return ;
	}
	
	ServerGameNetEvent* evt = new ServerGameNetEvent(INFO_SUPERMARKET);
	ps->postNetEvent(evt);

	player->reduceMoney(iAllMoney, 1);
#endif
}