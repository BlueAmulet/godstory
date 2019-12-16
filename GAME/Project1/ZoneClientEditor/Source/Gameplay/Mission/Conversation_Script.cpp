#include "Gameplay/Mission/Conversation.h"
#include "GamePlay/GameObjects/gameobject.h"

// ----------------------------------------------------------------------------
// ���öԻ���ʽ
ConsoleMethod( Conversation, setType, void, 3, 3, "%conv.setType(%type)")
{
	((Conversation*)object)->setType(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// �������񴥷�����
ConsoleMethod( Conversation, setTriggerType, void, 3, 3, "%conv.setTriggerType(%mode)")
{
	((Conversation*)object)->setTriggerMode(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// ���ô���������
ConsoleMethod( Conversation, setTriggerMission, void, 3, 3, "%conv.setTriggerMission(%missionid)")
{
	((Conversation*)object)->setBindMission(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// ���öԻ������ı����
ConsoleMethod( Conversation, setText, void, 3, 4, "%conv.setText(%textid, %textparam)")
{
	if(argc >3)
		((Conversation*)object)->setTextParam(dAtol(argv[3]));
	((Conversation*)object)->setText(dAtol(argv[2]));
}

// ----------------------------------------------------------------------------
// ��ӶԻ�ѡ��
ConsoleMethod( Conversation, addOption, void, 4, 5, "%conv.addOption(%textid, %state, %param)")
{
	S32 param = 0;
	if(argc > 4)
		param = dAtol(argv[4]);
	((Conversation*)object)->addOption(dAtol(argv[2]), dAtol(argv[3]), param);
}

// ----------------------------------------------------------------------------
// ���ͶԻ����ݸ��ͻ���
ConsoleMethod( Conversation, send, void, 3, 3, "%conv.send(%player)")
{
#ifdef NTJ_SERVER
	GameObject* game_object = (GameObject*)Sim::findObject(dAtol(argv[2]));
	if(!game_object)
		return;

	GameConnection* conn = game_object->getControllingClient();
	((Conversation*)object)->send((NetConnection*)conn);
#endif
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ի���ʽ
ConsoleMethod( Conversation, getType, S32, 2, 2, "%conv.getType()")
{
	return ((Conversation*)object)->getType();
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ի��Ĵ�������
ConsoleMethod( Conversation, getTriggerType, S32, 2, 2, "%conv.getTriggerType()")
{
	return ((Conversation*)object)->getTriggerMode();
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ի�������������
ConsoleMethod( Conversation, getTriggerMission, S32, 2, 2, "%conv.getTriggerMission()")
{
	return ((Conversation*)object)->getBindMission();
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ի������ı�
ConsoleMethod(Conversation, getText, const char*, 2, 2, "%conv.getText()")
{
	return ((Conversation*)object)->getText();
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ի������ı�����
ConsoleMethod(Conversation, getTextParam, S32, 2, 2, "%conv.getTextParam()")
{
	return ((Conversation*)object)->getTextParam();
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ի������ı����
ConsoleMethod( Conversation, getTextID, S32, 2, 2, "%conv.getTextID()")
{
	return ((Conversation*)object)->getTextID();
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ի��Ի�ѡ������
ConsoleMethod( Conversation, getOptionTextID, S32, 3, 3, "%conv.getOptionTextID(%index)")
{
	return ((Conversation*)object)->getOptionTextID(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ի�ѡ������
ConsoleMethod( Conversation, getOptionCount, S32, 2, 2, "%conv.getOptionCount()")
{
	return ((Conversation*)object)->getOptionsCount();
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ի��Ի�ѡ������
ConsoleMethod( Conversation, getOptionState, S32, 3, 3, "%conv.getOptionState(%index)")
{
	return ((Conversation*)object)->getOptionReturnState(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ի��Ի�ѡ������
ConsoleMethod( Conversation, getOptionText, const char*, 3, 3, "%conv.getOptionText(%index)")
{
	return ((Conversation*)object)->getOptionText(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ի��Ի�ѡ������
ConsoleMethod( Conversation, getOptionParam, S32, 3, 3, "%conv.getOptionParam(%index)")
{
	return ((Conversation*)object)->getOptionParam(dAtoi(argv[2]));
}

#ifdef NTJ_CLIENT
// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ự����
ConsoleFunction(getConversation, S32, 1, 1, "getConversation()")
{
	Conversation* conv = Conversation::getCurrentConversation();
	return conv ? conv->getId() : 0;
}

// ----------------------------------------------------------------------------
// ��ȡ��ǰ�Ự״̬
ConsoleFunction(getConversationState, S32, 1, 1, "getConversationState()")
{
	return Conversation::getConversationState();
}

// ----------------------------------------------------------------------------
// ���õ�ǰ�Ự״̬
ConsoleFunction(setConversationState, void, 2, 2, "setConversationState(%state)")
{
	Conversation::setConversationState(dAtoi(argv[1]));
}

// ----------------------------------------------------------------------------
// ���õ�ǰ�Ự״̬
ConsoleFunction(getTextByID, const char*, 2, 3, "getTextByID(%id, %param)")
{
	S32 param = 0;
	if(argc > 2)
		param = dAtoi(argv[2]);
	return Conversation::getTextByID(dAtoi(argv[1]), param);
}

// ----------------------------------------------------------------------------
// ���õ�ǰ�Ự״̬
ConsoleFunction(setConversationParam, void, 2, 2, "setConversationParam(%param)")
{
	Conversation::setConversationParam(dAtoi(argv[1]));
}

// ----------------------------------------------------------------------------
// ����ѡ��ĶԻ�ѡ��
ConsoleFunction(setSelectConversationOption, void, 2, 2, "setSelectConversationOption(%index)")
{
	Conversation::setSelectConversationOption(dAtoi(argv[1]));
}

// ----------------------------------------------------------------------------
// ����ѡ��ĶԻ�ѡ��
ConsoleFunction(exitConversation, void, 1, 1, "exitConversation()")
{
	Conversation::setConversationState(-1);
	Conversation::sendConversationReply();
}

// ----------------------------------------------------------------------------
// ��Ӧ�Ի������ύ�����
ConsoleFunction(sendConversationReply, void, 1, 1, "sendConversationReply()")
{
	Conversation::sendConversationReply();
}
#endif