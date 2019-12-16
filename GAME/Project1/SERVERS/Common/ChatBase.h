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
	CHAT_MSG_TYPE_SYSTEM,		// ϵͳ
	CHAT_MSG_TYPE_WORLDPLUS,	// ����
	CHAT_MSG_TYPE_WORLD,		// ���
	CHAT_MSG_TYPE_LINE,			// ȫ��
	//----------------------------------------
	CHAT_MSG_TYPE_RACE,			// ����
	CHAT_MSG_TYPE_ORG,			// ���
	CHAT_MSG_TYPE_SQUAD,		// ��
	CHAT_MSG_TYPE_TEAM,			// ����
	CHAT_MSG_TYPE_GROUP,		// Ⱥ��Ϣ
	CHAT_MSG_TYPE_REALCITY,		// ͬ��
	CHAT_MSG_TYPE_NEARBY,		// ����
	//----------------------------------------
	CHAT_MSG_TYPE_PRIVATE,		// ˽��
	CHAT_MSG_TYPE_P2P,			// ˽�Ĵ���
	CHAT_MSG_TYPE_GM,			// GM��Ϣ
	CHAT_MSG_TYPE_PERSON,		// ����
	CHAT_MSG_TYPE_UNKNOWN,		// Ԥ��
};

struct stChatMessage
{
	int	nSender;	// ������
	int nRecver;	// ������ 

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