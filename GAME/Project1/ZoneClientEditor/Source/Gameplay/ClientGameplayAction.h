//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#pragma once
#include "Gameplay/ClientGameplayParam.h"
#include "platform/types.h"

// ----------------------------------------------------------------------------
// ��Ϸ�߼�������Ϊ��
// ˵��:
//      ������Ҫ�����˷���������Ϣ��ĳ��������Ϊ�����������Ϊ��ͬ��GameNetEvent����
//һ��ʱ������޷��ռ���������ݣ�Ҳ����˵�ڷ�������ǰ����Ҫͨ����ν���������������
//�������ݣ�һ���������setParam���̣�����ȫ���ռ����ʱ����ͨ��sendGameplayEvent�������.
class ClientGameplayAction
{
public:
	ClientGameplayAction							(U32 paramCount, U32 type);
	virtual ~ClientGameplayAction					();

	void setCursorName								(StringTableEntry cursorName) { mCursorName = cursorName; }
	inline U32 getActionType						() { return mActionType; }
	inline U32 getParamCount						() { return mParamCount; }
	inline U32 getCurrentParamCount					() { return mCurrentParamCount; }
	bool isParamValid								(ClientGameplayParam* param);
	bool setParam									(ClientGameplayParam* param);
	ClientGameplayParam* getParam					(U32 index);
	void activeCursor								(StringTableEntry cursorName);

	virtual void sendGameplayEvent					();
	virtual void onActivated						();
	virtual void onDone								();
	virtual	void onCancle							();
	virtual void onSetParam							();
protected:
	U32	mActionType;						// ������Ϊ����
	U32	mParamCount;						// ���ݲ�������
	U32	mCurrentParamCount;					// ��ǰ���ݲ�������
	ClientGameplayParam** mParam;			// ���ݲ����б�
	StringTableEntry mCursorName;			// �������ָ��ͼ����
};