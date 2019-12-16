//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/ClientGameplayAction.h"
#include "Gameplay/GameEvents/ClientGameNetEvents.h"
#include "T3D/gameConnection.h"

#ifdef NTJ_EDITOR
#include "Gameplay/EditorGameplayState.h"
#endif

#ifdef NTJ_CLIENT
#include "Gameplay/ClientGameplayState.h"
#endif

ClientGameplayAction::ClientGameplayAction(U32 paramCount, U32 type)
{
	mActionType	= type;
	mParamCount	= paramCount;
	mCurrentParamCount	= 0;
	mCursorName = NULL;
	mParam = new ClientGameplayParam*[mParamCount];
	for(U32 i = 0; i < mParamCount; i++)
		mParam[i] = NULL;
}

ClientGameplayAction::~ClientGameplayAction()
{
	for( int i = 0; i < mParamCount; i++)
	{
		if(mParam[i])
		{
			delete mParam[i];
			mParam[i] = NULL;
		}
	}
	delete[] mParam;
}

//-----------------------------------------------------------------------------
// 判断是否参数有效
bool ClientGameplayAction::isParamValid(ClientGameplayParam* param)
{
	if(!param || mCurrentParamCount >= mParamCount)
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// 设置参数
bool ClientGameplayAction::setParam(ClientGameplayParam* param)
{
	if(!isParamValid(param))
		return false;

	mParam[mCurrentParamCount] = param;
	mCurrentParamCount++;
	onSetParam();
	return true;
}

//-----------------------------------------------------------------------------
// 获取参数
ClientGameplayParam* ClientGameplayAction::getParam(U32 index)
{
	return index < mParamCount ? mParam[index] : NULL;
}

//-----------------------------------------------------------------------------
// Action创建时的处理
void ClientGameplayAction::onActivated()
{
	if(mCursorName)
		g_ClientGameplayState->setCurrentCursor(mCursorName);
}

//-----------------------------------------------------------------------------
// Action激活鼠标样式
void ClientGameplayAction::activeCursor(StringTableEntry cursorName)
{
	if(!mCursorName && cursorName && cursorName[0])
	{
		mCursorName = cursorName;
		g_ClientGameplayState->setCurrentCursor(mCursorName);
	}
}

//-----------------------------------------------------------------------------
// Action完成结束时的处理
void ClientGameplayAction::onDone()
{
	if(mCursorName)
		g_ClientGameplayState->popCursor();
}

//-----------------------------------------------------------------------------
// Action被终止时的处理
void ClientGameplayAction::onCancle()
{
	if(mCursorName)
		g_ClientGameplayState->popCursor();
}

//-----------------------------------------------------------------------------
// Action设置某次数据参数后的处理
void ClientGameplayAction::onSetParam()
{
}

//-----------------------------------------------------------------------------
// Action完成后发送数据
void ClientGameplayAction::sendGameplayEvent()
{
	GameConnection* conn = GameConnection::getConnectionToServer();
	if(!conn)
		return;
	
	ClientGameNetEvent* ev = new ClientGameNetEvent(mActionType);
	for(int i = 0; i < mParamCount; ++i)
	{
		if(mParam[i]->sendParam(ev) == false)
		{
			AssertWarn(0, "sendGameplayEvent failed!");
			return;
		}
	}
	conn->postNetEvent(ev);
}