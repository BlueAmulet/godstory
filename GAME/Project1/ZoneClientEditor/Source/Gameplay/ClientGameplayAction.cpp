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
// �ж��Ƿ������Ч
bool ClientGameplayAction::isParamValid(ClientGameplayParam* param)
{
	if(!param || mCurrentParamCount >= mParamCount)
		return false;
	return true;
}

//-----------------------------------------------------------------------------
// ���ò���
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
// ��ȡ����
ClientGameplayParam* ClientGameplayAction::getParam(U32 index)
{
	return index < mParamCount ? mParam[index] : NULL;
}

//-----------------------------------------------------------------------------
// Action����ʱ�Ĵ���
void ClientGameplayAction::onActivated()
{
	if(mCursorName)
		g_ClientGameplayState->setCurrentCursor(mCursorName);
}

//-----------------------------------------------------------------------------
// Action���������ʽ
void ClientGameplayAction::activeCursor(StringTableEntry cursorName)
{
	if(!mCursorName && cursorName && cursorName[0])
	{
		mCursorName = cursorName;
		g_ClientGameplayState->setCurrentCursor(mCursorName);
	}
}

//-----------------------------------------------------------------------------
// Action��ɽ���ʱ�Ĵ���
void ClientGameplayAction::onDone()
{
	if(mCursorName)
		g_ClientGameplayState->popCursor();
}

//-----------------------------------------------------------------------------
// Action����ֹʱ�Ĵ���
void ClientGameplayAction::onCancle()
{
	if(mCursorName)
		g_ClientGameplayState->popCursor();
}

//-----------------------------------------------------------------------------
// Action����ĳ�����ݲ�����Ĵ���
void ClientGameplayAction::onSetParam()
{
}

//-----------------------------------------------------------------------------
// Action��ɺ�������
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