#ifndef CHAT_BASE_H
#define CHAT_BASE_H

#include "Base/Log.h"
#include "Base/bitStream.h"

const int CHAT_MESSAGE_MAX_LENGTH = 512;
const int CHAT_MESSAGE_MAX_NAME_LENGTH = 32;

//123
using namespace std;

enum eSendChatResponseType
{
	SEND_CHAT_ACK_UNKNOWN = 0,
	SEND_CHAT_ACK_SUCCESSED,
	SEND_CHAT_ACK_TOOFAST,
	SEND_CHAT_ACK_REFUSED,
	SEND_CHAT_ACK_ERROR,
};

enum eChatMessageType
{
	CHAT_MSG_TYPE_SYSTEM,		// 系统
	CHAT_MSG_TYPE_WORLDPLUS,	// 跑马
	CHAT_MSG_TYPE_WORLD,		// 跨服
	CHAT_MSG_TYPE_LINE,			// 全服
	//----------------------------------------
	CHAT_MSG_TYPE_RACE,			// 门宗
	CHAT_MSG_TYPE_ORG,			// 帮会
	CHAT_MSG_TYPE_SQUAD,		// 团
	CHAT_MSG_TYPE_TEAM,			// 队伍
	CHAT_MSG_TYPE_GROUP,		// 群消息
	CHAT_MSG_TYPE_REALCITY,		// 同城
	CHAT_MSG_TYPE_NEARBY,		// 附近
	//----------------------------------------
	CHAT_MSG_TYPE_PRIVATE,		// 私聊
	CHAT_MSG_TYPE_P2P,			// 私聊窗口
	CHAT_MSG_TYPE_GM,			// GM消息
	CHAT_MSG_TYPE_PERSON,		// 个人
	CHAT_MSG_TYPE_UNKNOWN,		// 预留
};

struct stChatMessage
{
	int	nSender;	// 发送者
	int nRecver;	// 接收者 

	BYTE btMessageType;
	char szMessage[CHAT_MESSAGE_MAX_LENGTH];
	char szSenderName[CHAT_MESSAGE_MAX_NAME_LENGTH];

	stChatMessage()
	{
		memset( this, 0, sizeof( stChatMessage ) );
	}

	static void PackMessage( stChatMessage& chatMessage, Base::BitStream& stream )
	{
		stream.writeInt( chatMessage.nSender, 32 );
		stream.writeInt( chatMessage.nRecver, Base::Bit32 );
		stream.writeInt( chatMessage.btMessageType, Base::Bit8 );
		stream.writeString( chatMessage.szSenderName, CHAT_MESSAGE_MAX_NAME_LENGTH );
		stream.writeString( chatMessage.szMessage, CHAT_MESSAGE_MAX_LENGTH );
	}

	static void UnpackMessage( stChatMessage& chatMessage, Base::BitStream& stream )
	{
		chatMessage.nSender = (int)stream.readInt( 32 );
		chatMessage.nRecver = (int)stream.readInt( Base::Bit32 );
		chatMessage.btMessageType = (BYTE)stream.readInt( Base::Bit8 );
		stream.readString( chatMessage.szSenderName ,CHAT_MESSAGE_MAX_NAME_LENGTH);
		stream.readString( chatMessage.szMessage ,CHAT_MESSAGE_MAX_LENGTH);
	}
};

#endif