#ifndef __CONVERSATION_H__
#define __CONVERSATION_H__
#pragma once
#include "console/console.h"
#include "console/consoleTypes.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "core/tVector.h"
// ============================================================================
//	Conversation NPC�Ự��
// ============================================================================
class Player;

class Conversation : public SimObject
{
public:
	enum Constants
	{
		TYPE_OPTIONS	= 0,		// �ı���������ѡ��ĶԻ�
		TYPE_TEXT		= 1,		// ���ı��Ի�
		TYPE_MISSION	= 2,		// ��������/�������Ի���ʽ
		TYPE_CLOSE		= 4,		// �رնԻ���

		NPC_TRIGGER     = 0,		// NPC�����Ի�
		ITEM_TRIGGER    = 1,		// ��Ʒ�����Ի�
	};

	// �Ի�ѡ��ṹ
	struct ConversationOption
	{
		U32	mTextID;				// ѡ���ı����
		S32	mReturnState;			// ѡ���״̬
		S32 mParam;					// ѡ�����
		char* mText;				// ѡ���ı�����
		ConversationOption() { memset(this, 0, sizeof(ConversationOption));}
	};

	Conversation										();
	~Conversation										();

	DECLARE_CONOBJECT									(Conversation);

	// ------------------------------------------------------------------------
	//	���ݴ���
	inline void setType									(U32 type)	{ mType = type;};
	inline U32 getType									()	{ return mType;}
	inline void setTriggerMode							(U32 mode)	{ mTriggerMode = mode;}
	inline U32 getTriggerMode							() { return mTriggerMode;}
	inline void setBindMission							(S32 missionid)	{ mMissionID = missionid;}
	inline S32 getBindMission							() { return mMissionID;}
    void setText										(U32 textid);
	void setTextParam									(U32 textparam) { mTextParam = textparam;}
	inline U32 getTextID								() { return mTextID;}
	inline U32 getTextParam								() { return mTextParam;}
	const char* getText									();
	void addOption										(U32 textid, S32 state, S32 param = 0);
	void clearOption									();
	const char*  getOptionText							(S32 index);
	U32 getOptionTextID									(S32 index);
	S32 getOptionParam									(S32 index);
	inline S32 getOptionsCount							() 	{ return mOptions.size();}
	S32	getOptionReturnState							(S32 index);
	
	void send											(NetConnection* conn);
private:
	S32													mType;				// �Ի���������
	U32													mTextID;			// �Ի��ı����
	U32													mTextParam;			// �Ի��ı����Ӳ���
	char*												mText;				// �Ի��ı�����
	Vector<ConversationOption>							mOptions;			// �Ի�ѡ��
	U32													mTriggerMode;		// �����Ի�ģʽ
	S32													mMissionID;			// ������������

#ifdef NTJ_CLIENT
	// ========================================================================
	// ��̬ȫ�ַ��� Client �� ZoneServer �Ľ���,��NPC�ĶԻ�
public:
	static const char*	getTextByID						(U32 textid, U32 textparam);
	static void	setConversationState					(S32 state) { mConversationState = state;}
	static S32	getConversationState					() { return mConversationState;}
	static void setConversationParam					(S32 param) { mConversationParam = param;}
	static S32  getConversationParam					() { return mConversationParam;}
	static Conversation*	getCurrentConversation		() { return mCurrentConversation;}
	static void				setCurrentConversation		(Conversation* conv);
	static void				setSelectConversationOption	(S32 index);
	static void				sendConversationReply		();
private:
	static Conversation*								mCurrentConversation;	// ��ǰ�Ի�
	static S32											mConversationState;		// ��ǰ�Ի�����״̬
	static S32											mConversationParam;		// ��ǰ�Ի�����״̬���Ӳ���

#endif//NTJ_CLIENT
public:
	static void exitConversation						(Player* player);
};

// ============================================================================
//	ConversationEvent �Ի��¼�
// ============================================================================
class ConversationEvent : public GameDataNetEvent
{
	typedef GameDataNetEvent Parent;
public:
	enum Constants
	{
		BITS_4			= 4,
		BITS_6			= 6,
		BITS_32			= 32,
	};

	ConversationEvent();
	~ConversationEvent();
	void packData(NetConnection *_conn, BitStream * stream);
	void unpackData(NetConnection *_conn, BitStream * stream);

	DECLARE_CONOBJECT(ConversationEvent);
	inline void setConversation(Conversation* conv) { mConversation = conv;};
protected:
	SimObjectPtr<Conversation>	mConversation;
};

#endif//__CONVERSATION_H__