//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#include "Gameplay/ClientGameplayParam.h"
#include "Gameplay/GameObjects/PlayerObject.h"
#include "GamePlay/GameEvents/GameNetEvents.h"
#include "Gameplay/GameplayCommon.h"

ClientGameplayParam::ClientGameplayParam()
{
	mArgCount = 0;
	mArg = NULL;
	mArg32Count = 0;
	mArg32 = NULL;
}

ClientGameplayParam::~ClientGameplayParam()
{
	if(mArg)
		delete[] mArg;
	if(mArg32)
		delete[] mArg32;
}

// ----------------------------------------------------------------------------
// ���ò�������ĸ����Ӳ�����ֵ
void ClientGameplayParam::setIntArgValues(S32 argc, ...)
{
	AssertRelease(argc <= GameplayEvent::MAX_INT_ARGS,"����! 16λ��������̫�࣬����64��!" );

	mArg = new S16[argc];
	dMemset(mArg, 0, sizeof(S16) * mArgCount);

	va_list args;
	va_start(args,argc);

	for(S32 i = 0 ; i < argc ; ++ i )
	{
		mArg[i] = va_arg(args, S16);
	}

	va_end(args);

	mArgCount = argc;
}

// ----------------------------------------------------------------------------
// ��ȡ���������ĳ�Ӳ�����ֵ
S32 ClientGameplayParam::getIntArg(S32 index)
{
	return index < mArgCount ? mArg[index] : 0;
}

// ----------------------------------------------------------------------------
// ���ò�������ĸ����Ӳ�����ֵ
void ClientGameplayParam::setInt32ArgValues(S32 argc, ...)
{
	AssertRelease(argc <= GameplayEvent::MAX_INT32_ARGS,"����! 32λ��������̫�࣬����64��!" );

	mArg32 = new S32[argc];
	dMemset(mArg32, 0, sizeof(S32) * mArg32Count);

	va_list args;
	va_start(args,argc);

	for(S32 i = 0 ; i < argc ; ++ i )
	{
		mArg32[i] = va_arg(args, S32);
	}

	va_end(args);

	mArg32Count = argc;
}

// ----------------------------------------------------------------------------
// ��ȡ���������ĳ�Ӳ�����ֵ
S32 ClientGameplayParam::getInt32Arg(S32 index)
{
	return index < mArg32Count ? mArg32[index] : 0;
}

// ----------------------------------------------------------------------------
// Ϊ�������ݷ��Ͷ����ø�����
bool ClientGameplayParam::sendParam(GameplayEvent* ev)
{
	U32& mCount = ev->m_IntArgCount;
	U32& mCount32 = ev->m_Int32ArgCount;
	if(mCount + mArgCount >= GameplayEvent::MAX_INT_ARGS || mCount32 + mArg32Count >= GameplayEvent::MAX_INT32_ARGS)
		return false;

	for(int i = 0; i < mArgCount; ++i)
	{
		ev->m_IntArgValue[mCount++] = mArg[i];
	}

	for(int i = 0; i < mArg32Count; ++i)
	{
		ev->m_Int32ArgValue[mCount32++] = mArg32[i];
	}
	return true;
}