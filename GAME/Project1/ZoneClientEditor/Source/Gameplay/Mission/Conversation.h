#ifndef __CONVERSATION_H__
#define __CONVERSATION_H__
#pragma once
#include "console/console.h"
#include "console/consoleTypes.h"
#include "Gameplay/GameEvents/GameNetEvents.h"
#include "core/tVector.h"
// ============================================================================
//	Conversation NPC会话类
// ============================================================================
class Player;

class Conversation : public SimObject
{
public:
	enum Constants
	{
		TYPE_OPTIONS	= 0,		// 文本描述附带选项的对话
		TYPE_TEXT		= 1,		// 仅文本对话
		TYPE_MISSION	= 2,		// 任务描述/任务奖励对话方式
		TYPE_CLOSE		= 4,		// 关闭对话框

		NPC_TRIGGER     = 0,		// NPC触发对话
		ITEM_TRIGGER    = 1,		// 物品触发对话
	};

	// 对话选项结构
	struct ConversationOption
	{
		U32	mTextID;				// 选项文本编号
		S32	mReturnState;			// 选项返回状态
		S32 mParam;					// 选项参数
		char* mText;				// 选项文本内容
		ConversationOption() { memset(this, 0, sizeof(ConversationOption));}
	};

	Conversation										();
	~Conversation										();

	DECLARE_CONOBJECT									(Conversation);

	// ------------------------------------------------------------------------
	//	数据处理
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
	S32													mType;				// 对话操作类型
	U32													mTextID;			// 对话文本编号
	U32													mTextParam;			// 对话文本附加参数
	char*												mText;				// 对话文本内容
	Vector<ConversationOption>							mOptions;			// 对话选项
	U32													mTriggerMode;		// 触发对话模式
	S32													mMissionID;			// 触发的任务编号

#ifdef NTJ_CLIENT
	// ========================================================================
	// 静态全局方法 Client 与 ZoneServer 的交互,与NPC的对话
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
	static Conversation*								mCurrentConversation;	// 当前对话
	static S32											mConversationState;		// 当前对话操作状态
	static S32											mConversationParam;		// 当前对话操作状态附加参数

#endif//NTJ_CLIENT
public:
	static void exitConversation						(Player* player);
};

// ============================================================================
//	ConversationEvent 对话事件
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