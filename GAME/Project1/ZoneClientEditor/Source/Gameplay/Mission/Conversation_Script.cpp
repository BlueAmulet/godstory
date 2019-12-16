#include "Gameplay/Mission/Conversation.h"
#include "GamePlay/GameObjects/gameobject.h"

// ----------------------------------------------------------------------------
// 设置对话方式
ConsoleMethod( Conversation, setType, void, 3, 3, "%conv.setType(%type)")
{
	((Conversation*)object)->setType(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 设置任务触发类型
ConsoleMethod( Conversation, setTriggerType, void, 3, 3, "%conv.setTriggerType(%mode)")
{
	((Conversation*)object)->setTriggerMode(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 设置触发任务编号
ConsoleMethod( Conversation, setTriggerMission, void, 3, 3, "%conv.setTriggerMission(%missionid)")
{
	((Conversation*)object)->setBindMission(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 设置对话内容文本编号
ConsoleMethod( Conversation, setText, void, 3, 4, "%conv.setText(%textid, %textparam)")
{
	if(argc >3)
		((Conversation*)object)->setTextParam(dAtol(argv[3]));
	((Conversation*)object)->setText(dAtol(argv[2]));
}

// ----------------------------------------------------------------------------
// 添加对话选项
ConsoleMethod( Conversation, addOption, void, 4, 5, "%conv.addOption(%textid, %state, %param)")
{
	S32 param = 0;
	if(argc > 4)
		param = dAtol(argv[4]);
	((Conversation*)object)->addOption(dAtol(argv[2]), dAtol(argv[3]), param);
}

// ----------------------------------------------------------------------------
// 发送对话数据给客户端
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
// 获取当前对话方式
ConsoleMethod( Conversation, getType, S32, 2, 2, "%conv.getType()")
{
	return ((Conversation*)object)->getType();
}

// ----------------------------------------------------------------------------
// 获取当前对话的触发类型
ConsoleMethod( Conversation, getTriggerType, S32, 2, 2, "%conv.getTriggerType()")
{
	return ((Conversation*)object)->getTriggerMode();
}

// ----------------------------------------------------------------------------
// 获取当前对话触发的任务编号
ConsoleMethod( Conversation, getTriggerMission, S32, 2, 2, "%conv.getTriggerMission()")
{
	return ((Conversation*)object)->getBindMission();
}

// ----------------------------------------------------------------------------
// 获取当前对话内容文本
ConsoleMethod(Conversation, getText, const char*, 2, 2, "%conv.getText()")
{
	return ((Conversation*)object)->getText();
}

// ----------------------------------------------------------------------------
// 获取当前对话内容文本参数
ConsoleMethod(Conversation, getTextParam, S32, 2, 2, "%conv.getTextParam()")
{
	return ((Conversation*)object)->getTextParam();
}

// ----------------------------------------------------------------------------
// 获取当前对话内容文本编号
ConsoleMethod( Conversation, getTextID, S32, 2, 2, "%conv.getTextID()")
{
	return ((Conversation*)object)->getTextID();
}

// ----------------------------------------------------------------------------
// 获取当前对话对话选项总数
ConsoleMethod( Conversation, getOptionTextID, S32, 3, 3, "%conv.getOptionTextID(%index)")
{
	return ((Conversation*)object)->getOptionTextID(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 获取当前对话选项总数
ConsoleMethod( Conversation, getOptionCount, S32, 2, 2, "%conv.getOptionCount()")
{
	return ((Conversation*)object)->getOptionsCount();
}

// ----------------------------------------------------------------------------
// 获取当前对话对话选项总数
ConsoleMethod( Conversation, getOptionState, S32, 3, 3, "%conv.getOptionState(%index)")
{
	return ((Conversation*)object)->getOptionReturnState(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 获取当前对话对话选项总数
ConsoleMethod( Conversation, getOptionText, const char*, 3, 3, "%conv.getOptionText(%index)")
{
	return ((Conversation*)object)->getOptionText(dAtoi(argv[2]));
}

// ----------------------------------------------------------------------------
// 获取当前对话对话选项总数
ConsoleMethod( Conversation, getOptionParam, S32, 3, 3, "%conv.getOptionParam(%index)")
{
	return ((Conversation*)object)->getOptionParam(dAtoi(argv[2]));
}

#ifdef NTJ_CLIENT
// ----------------------------------------------------------------------------
// 获取当前会话对象
ConsoleFunction(getConversation, S32, 1, 1, "getConversation()")
{
	Conversation* conv = Conversation::getCurrentConversation();
	return conv ? conv->getId() : 0;
}

// ----------------------------------------------------------------------------
// 获取当前会话状态
ConsoleFunction(getConversationState, S32, 1, 1, "getConversationState()")
{
	return Conversation::getConversationState();
}

// ----------------------------------------------------------------------------
// 设置当前会话状态
ConsoleFunction(setConversationState, void, 2, 2, "setConversationState(%state)")
{
	Conversation::setConversationState(dAtoi(argv[1]));
}

// ----------------------------------------------------------------------------
// 设置当前会话状态
ConsoleFunction(getTextByID, const char*, 2, 3, "getTextByID(%id, %param)")
{
	S32 param = 0;
	if(argc > 2)
		param = dAtoi(argv[2]);
	return Conversation::getTextByID(dAtoi(argv[1]), param);
}

// ----------------------------------------------------------------------------
// 设置当前会话状态
ConsoleFunction(setConversationParam, void, 2, 2, "setConversationParam(%param)")
{
	Conversation::setConversationParam(dAtoi(argv[1]));
}

// ----------------------------------------------------------------------------
// 设置选择的对话选项
ConsoleFunction(setSelectConversationOption, void, 2, 2, "setSelectConversationOption(%index)")
{
	Conversation::setSelectConversationOption(dAtoi(argv[1]));
}

// ----------------------------------------------------------------------------
// 设置选择的对话选项
ConsoleFunction(exitConversation, void, 1, 1, "exitConversation()")
{
	Conversation::setConversationState(-1);
	Conversation::sendConversationReply();
}

// ----------------------------------------------------------------------------
// 响应对话交互提交服务端
ConsoleFunction(sendConversationReply, void, 1, 1, "sendConversationReply()")
{
	Conversation::sendConversationReply();
}
#endif