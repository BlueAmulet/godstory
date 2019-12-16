#include "Gameplay/Mission/Conversation.h"
#include "GamePlay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#endif

// ============================================================================
//	构造与析构方法
// ============================================================================
IMPLEMENT_CONOBJECT(Conversation);
Conversation::Conversation():
	mType(0),
	mTextID(0),
	mMissionID(-1),
	mTriggerMode(NPC_TRIGGER),
	mText(NULL),
	mTextParam(0)
{
}

Conversation::~Conversation()
{
	clearOption();
	if(mText)
	{
		dFree(mText);
		mText = NULL;
	}
}

// ============================================================================
//	用于网络数据传输解包打包的方法
// ============================================================================
// ----------------------------------------------------------------------------
// 发送对话事件
void Conversation::send(NetConnection* _conn)
{
#ifdef NTJ_SERVER
	GameConnection* conn = dynamic_cast<GameConnection*>(_conn);
	if(conn == NULL)
		return;
	GameBase* obj = conn->getControlObject();
	if(obj == NULL)
		return;
	Player* player = dynamic_cast<Player*>(obj);
	if(this->getTriggerMode() == NPC_TRIGGER && player->getInteractionState() != Player::INTERACTION_NPCTALK)
		return;
	if(conn)
	{
		ConversationEvent* event = new ConversationEvent;
		event->setConversation(this);
		conn->postNetEvent(event);
	}
#endif
}

// ----------------------------------------------------------------------------
// 设置对话文本
void Conversation::setText(U32 textid)
{
	mTextID = textid;
#ifdef NTJ_CLIENT
	if(mText)
	{
		dFree(mText);
		mText = NULL;
	}
	const char* szText = Conversation::getTextByID(mTextID, mTextParam);
	if(szText)
		mText = dStrdup(szText);	
#endif
}

// ----------------------------------------------------------------------------
// 获取对话文本内容
const char* Conversation::getText()
{
	return (mText && mText[0]) ? mText: "";
}

// ----------------------------------------------------------------------------
// 添加对话选项
void Conversation::addOption(U32 textid, S32 state, S32 param /*=0*/)
{
	setType(TYPE_OPTIONS);
	ConversationOption option;
	option.mTextID = textid;
	option.mReturnState	= state;
	option.mParam = param;
#ifdef NTJ_CLIENT
	const char* szText = Conversation::getTextByID(option.mTextID, mTextParam);
	if(szText)
		option.mText = dStrdup(szText);
#endif
	mOptions.push_back(option);
}

// ----------------------------------------------------------------------------
// 清除所有对话选项
void Conversation::clearOption()
{
#ifdef NTJ_CLIENT
	for(U32 i = 0; i < mOptions.size(); i++)
	{	
		if(mOptions[i].mText)
		{
			dFree(mOptions[i].mText);
			mOptions[i].mText = NULL;
		}
	}
#endif
	mOptions.clear();
	setType(TYPE_TEXT);
}

// ----------------------------------------------------------------------------
// 获取对话选项文本编号
U32 Conversation::getOptionTextID(S32 index)
{
	if(index < 0 || index >= mOptions.size())
		return 0;
	return mOptions[index].mTextID;
}
// ----------------------------------------------------------------------------
// 获取对话选项文本编号
S32 Conversation::getOptionParam(S32 index)
{
	if(index < 0 || index >= mOptions.size())
		return 0;
	return mOptions[index].mParam;
}

// ----------------------------------------------------------------------------
// 获取对话选项文本
const char* Conversation::getOptionText(S32 index)
{
	if(index < 0 || index >= mOptions.size() || mOptions[index].mText == 0)
		return "";
	return mOptions[index].mText;
}

// ----------------------------------------------------------------------------
// 获取选项的对话状态值
S32 Conversation::getOptionReturnState(S32 index)
{
	if(index < 0 || index >= getOptionsCount())
		return -1;
	return mOptions[index].mReturnState;
}


// ============================================================================
// 静态全局方法 Client 与 ZoneServer 的交互,与NPC的对话
// ============================================================================
#ifdef NTJ_CLIENT
Conversation* Conversation::mCurrentConversation	= NULL;
S32 Conversation::mConversationState				= -1;
S32 Conversation::mConversationParam				= 0;

// ----------------------------------------------------------------------------
// 脚本接口：获取指定文本编号的对话内容
const char* Conversation::getTextByID(U32 textid, U32 textparam)
{
	U32 npcid = 0, playerid = 0;
	SceneObject* obj = g_ClientGameplayState->GetInteraction();
	NpcObject* npc = obj ? dynamic_cast<NpcObject*>(obj) : NULL;
	if(npc)
		npcid = npc->getId();

	Player* player = g_ClientGameplayState->GetControlPlayer();
	if(player)
		playerid = player->getId();		

	return Con::executef("GetDialogText", Con::getIntArg(npcid), 
		Con::getIntArg(playerid), Con::getIntArg(textid), Con::getIntArg(textparam));
}

// ----------------------------------------------------------------------------
// 设置当前对话对象
void Conversation::setCurrentConversation(Conversation* conv)
{
	if(mCurrentConversation)
		mCurrentConversation->deleteObject();
	mCurrentConversation = conv;
	if(conv == NULL)
		return;	
	if(conv->getTriggerMode() == NPC_TRIGGER)
	{
		SceneObject* obj = g_ClientGameplayState->GetInteraction();
		NpcObject* npc = obj ? dynamic_cast<NpcObject*>(obj) : NULL;
		if(npc)
		{
			Con::executef("OnConversationEventReceived",npc->getObjectName());
		}
	}
	else
		Con::executef("OnConversationEventReceived","");
}

// ----------------------------------------------------------------------------
// 设置选择项的当前状态值
void Conversation::setSelectConversationOption(S32 index)
{
	if(!mCurrentConversation || index < 0 || index >= mCurrentConversation->getOptionsCount())
		return;
	setConversationState(mCurrentConversation->getOptionReturnState(index));
}

// ----------------------------------------------------------------------------
// 提交对话交互信息给服务端
void Conversation::sendConversationReply()
{
	if(!mCurrentConversation)
		return;
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(!conn)
		return;

	// 判断事件触发状态方式
	if(mCurrentConversation->getTriggerMode() == NPC_TRIGGER)
	{
		SceneObject* obj = g_ClientGameplayState->GetInteraction();
		NpcObject* npc = obj ? dynamic_cast<NpcObject*>(obj) : NULL;
		if(!npc)
			return;

		ClientGameNetEvent* ev = new ClientGameNetEvent(INFO_QUEST_TRIGGER_NPC);
		ev->SetInt32ArgValues(3, npc->getServerId(), getConversationState(), getConversationParam());
		conn->postNetEvent(ev);
	}
	else
	{
		ClientGameNetEvent* ev = new ClientGameNetEvent(INFO_QUEST_TRIGGER_OTHER);
		ev->SetInt32ArgValues(5, mCurrentConversation->getBindMission(), getConversationState(), 0, 0, getConversationParam());
		conn->postNetEvent(ev);
	}
}

#endif //NTJ_CLIENT

// ----------------------------------------------------------------------------
// 退出对话交互
void Conversation::exitConversation(Player* player)
{
#ifdef NTJ_SERVER

#endif
#ifdef NTJ_CLIENT
	Con::executef("CloseNpcDialog");
#endif
}

// ============================================================================
//	ConversationEvent 对话事件
// ============================================================================
IMPLEMENT_CO_CLIENTEVENT_V1(ConversationEvent);
ConversationEvent::ConversationEvent()
{
	mConversation = NULL;
}

ConversationEvent::~ConversationEvent()
{
}

// ----------------------------------------------------------------------------
// 发送对话数据服务端打包
void ConversationEvent::packData(NetConnection* conn, BitStream* stream)
{
#ifdef NTJ_SERVER
	if(!stream->writeFlag(!mConversation.isNull()))
		return;
	stream->writeInt(mConversation->getType(),			BITS_4);
	stream->writeInt(mConversation->getTriggerMode(),	BITS_4);
	stream->writeSignedInt(mConversation->getBindMission(), BITS_32);

	switch(mConversation->getType())
	{
	case Conversation::TYPE_OPTIONS: 	// 带选择项
		{
			stream->writeInt(mConversation->getTextID(),		BITS_32);
			stream->writeInt(mConversation->getTextParam(),		BITS_32);

			U32 Count = mConversation->getOptionsCount();
			stream->writeInt(Count,			BITS_6);
			for(U32 i = 0; i < Count; i++)
			{
				stream->writeInt(mConversation->getOptionParam(i), BITS_32);
				stream->writeInt(mConversation->getOptionTextID(i),	BITS_32);
				stream->writeSignedInt(mConversation->getOptionReturnState(i), BITS_32);
			}
		}	
		break;
	case Conversation::TYPE_TEXT:		// 仅文本显示
		{	
			stream->writeInt(mConversation->getTextID(),		BITS_32);
			stream->writeInt(mConversation->getTextParam(),		BITS_32);
		}
		break;
	case Conversation::TYPE_MISSION:	// 任务描述/任务奖励特定文本显示
		{	
			stream->writeInt(mConversation->getTextID(),		BITS_32);
			stream->writeInt(mConversation->getTextParam(),		BITS_32);
		}
		break;
	case Conversation::TYPE_CLOSE:		// 关闭对话框
		{
		}
		break;
	}
#endif
}

// ----------------------------------------------------------------------------
// 接收网络传输对话数据客户端解包
void ConversationEvent::unpackData(NetConnection* conn, BitStream* stream)
{
#ifdef NTJ_CLIENT
	if(!stream->readFlag())
		return;
	mConversation = new Conversation;
	mConversation->registerObject();
	mConversation->setType(stream->readInt(BITS_4));
	mConversation->setTriggerMode(stream->readInt(BITS_4));
	mConversation->setBindMission(stream->readSignedInt(BITS_32));
	switch(mConversation->getType())
	{
	case Conversation::TYPE_OPTIONS:	// 带选择项的文本
		{
			U32 textid = stream->readInt(BITS_32);
			U32 textparam = stream->readInt(BITS_32);
			mConversation->setTextParam(textparam);
			mConversation->setText(textid);
			U32 count = stream->readInt(BITS_6);
			mConversation->clearOption();
			for(U32 i = 0; i < count; i++)
			{
				S32 param = stream->readInt(BITS_32);
				U32 textid = stream->readInt(BITS_32);
				S32 state  = stream->readSignedInt(BITS_32);
				mConversation->addOption(textid, state, param);
			}
		}
		break;
	case Conversation::TYPE_TEXT:		// 仅文本
		{	
			U32 textid = stream->readInt(BITS_32);
			U32 textparam = stream->readInt(BITS_32);
			mConversation->setTextParam(textparam);
			mConversation->setText(textid);
		}		
		break;
	case Conversation::TYPE_MISSION:	// 任务描述/任务奖励特定对话窗口
		{
			U32 textid = stream->readInt(BITS_32);
			U32 textparam = stream->readInt(BITS_32);
			mConversation->setTextParam(textparam);
			mConversation->setText(textid);
		}
		break;
	case Conversation::TYPE_CLOSE:		// 关闭对话框
		{
		}
		break;
	}

	Conversation::setCurrentConversation(mConversation);
#endif
}