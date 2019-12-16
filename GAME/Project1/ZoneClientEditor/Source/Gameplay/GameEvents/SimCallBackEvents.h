//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _SIMCALLBACKEVENTS_H_
#define _SIMCALLBACKEVENTS_H_
#include "Gameplay/GameplayCommon.h"

class simCallBackEvents : public SimEvent
{
	typedef SimEvent Parent;

	enum TransferBits
	{
		InfoTypeBits		= 8,

		IntArgCountBits		= 6,
		IntArgValueBits		= 16,

		Int32ArgCountBits	= 6,
		Int32ArgValueBits	= 32,

		StringArgCountBits	= 3,
		StringArgValueBits	= 255,		//工会名称长度可能有50,留言可能有250字节	//32,
	};

	enum Constants
	{
		MAX_INT_ARGS		= 1 << IntArgCountBits,
		MAX_INT32_ARGS		= 1 << Int32ArgCountBits,
		MAX_STRING_ARGS		= 1 << StringArgCountBits,
	};

public:
	simCallBackEvents(U32 info_type = 0);

	void				SetIntArgValues					(S32 argc, ...);
	void				SetInt32ArgValues				(S32 argc, ...);
	void				SetStringArgValues				(S32 argc, ...);

	S32				    GetIntArgCount					(void)			{ return m_IntArgCount;}
	S32				    GetIntArg						(S32 index)	{ return m_IntArgValue[index];}
	S32				    GetInt32ArgCount				(void)			{ return m_Int32ArgCount;}
	S32				    GetInt32Arg						(S32 index)	{ return m_Int32ArgValue[index];}
	S32				    GetStringArgCount				(void)			{ return m_StringArgCount;}
	const char*			GetStringArg					(S32 index)	{ return m_StringArgValue[index];}

	virtual void        process(SimObject *object);
protected:
	U32			    m_InfoType;

	U32				m_IntArgCount;
	S32				m_IntArgValue[MAX_INT_ARGS];

	U32				m_Int32ArgCount;
	S32				m_Int32ArgValue[MAX_INT32_ARGS];

	U32				m_StringArgCount;
	char			m_StringArgValue[MAX_STRING_ARGS][StringArgValueBits];
};
#endif