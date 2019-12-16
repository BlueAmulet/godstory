#include "Gameplay/Mission/Conversation.h"
#include "GamePlay/GameObjects/PlayerObject.h"
#include "Gameplay/GameObjects/NpcObject.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#endif

// ============================================================================
//	��������������
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
//	�����������ݴ���������ķ���
// ============================================================================
// ----------------------------------------------------------------------------
// ���ͶԻ��¼�
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
// ���öԻ��ı�
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
// ��ȡ�Ի��ı�����
const char* Conversation::getText()
{
	return (mText && mText[0]) ? mText: "";
}

// ----------------------------------------------------------------------------
// ��ӶԻ�ѡ��
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
// ������жԻ�ѡ��
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
// ��ȡ�Ի�ѡ���ı����
U32 Conversation::getOptionTextID(S32 index)
{
	if(index < 0 || index >= mOptions.size())
		return 0;
	return mOptions[index].mTextID;
}
// ----------------------------------------------------------------------------
// ��ȡ�Ի�ѡ���ı����
S32 Conversation::getOptionParam(S32 index)
{
	if(index < 0 || index >= mOptions.size())
		return 0;
	return mOptions[index].mParam;
}

// ----------------------------------------------------------------------------
// ��ȡ�Ի�ѡ���ı�
const char* Conversation::getOptionText(S32 index)
{
	if(index < 0 || index >= mOptions.size() || mOptions[index].mText == 0)
		return "";
	return mOptions[index].mText;
}

// ----------------------------------------------------------------------------
// ��ȡѡ��ĶԻ�״ֵ̬
S32 Conversation::getOptionReturnState(S32 index)
{
	if(index < 0 || index >= getOptionsCount())
		return -1;
	return mOptions[index].mReturnState;
}


// ============================================================================
// ��̬ȫ�ַ��� Client �� ZoneServer �Ľ���,��NPC�ĶԻ�
// ============================================================================
#ifdef NTJ_CLIENT
Conversation* Conversation::mCurrentConversation	= NULL;
S32 Conversation::mConversationState				= -1;
S32 Conversation::mConversationParam				= 0;

// ----------------------------------------------------------------------------
// �ű��ӿڣ���ȡָ���ı���ŵĶԻ�����
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
// ���õ�ǰ�Ի�����
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
// ����ѡ����ĵ�ǰ״ֵ̬
void Conversation::setSelectConversationOption(S32 index)
{
	if(!mCurrentConversation || index < 0 || index >= mCurrentConversation->getOptionsCount())
		return;
	setConversationState(mCurrentConversation->getOptionReturnState(index));
}

// ----------------------------------------------------------------------------
// �ύ�Ի�������Ϣ�������
void Conversation::sendConversationReply()
{
	if(!mCurrentConversation)
		return;
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(!conn)
		return;

	// �ж��¼�����״̬��ʽ
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
// �˳��Ի�����
void Conversation::exitConversation(Player* player)
{
#ifdef NTJ_SERVER

#endif
#ifdef NTJ_CLIENT
	Con::executef("CloseNpcDialog");
#endif
}

// ============================================================================
//	ConversationEvent �Ի��¼�
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
// ���ͶԻ����ݷ���˴��
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
	case Conversation::TYPE_OPTIONS: 	// ��ѡ����
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
	case Conversation::TYPE_TEXT:		// ���ı���ʾ
		{	
			stream->writeInt(mConversation->getTextID(),		BITS_32);
			stream->writeInt(mConversation->getTextParam(),		BITS_32);
		}
		break;
	case Conversation::TYPE_MISSION:	// ��������/�������ض��ı���ʾ
		{	
			stream->writeInt(mConversation->getTextID(),		BITS_32);
			stream->writeInt(mConversation->getTextParam(),		BITS_32);
		}
		break;
	case Conversation::TYPE_CLOSE:		// �رնԻ���
		{
		}
		break;
	}
#endif
}

// ----------------------------------------------------------------------------
// �������紫��Ի����ݿͻ��˽��
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
	case Conversation::TYPE_OPTIONS:	// ��ѡ������ı�
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
	case Conversation::TYPE_TEXT:		// ���ı�
		{	
			U32 textid = stream->readInt(BITS_32);
			U32 textparam = stream->readInt(BITS_32);
			mConversation->setTextParam(textparam);
			mConversation->setText(textid);
		}		
		break;
	case Conversation::TYPE_MISSION:	// ��������/�������ض��Ի�����
		{
			U32 textid = stream->readInt(BITS_32);
			U32 textparam = stream->readInt(BITS_32);
			mConversation->setTextParam(textparam);
			mConversation->setText(textid);
		}
		break;
	case Conversation::TYPE_CLOSE:		// �رնԻ���
		{
		}
		break;
	}

	Conversation::setCurrentConversation(mConversation);
#endif
}