#include "ConfigStruct.h"
#include "windows.h"

Cconfig::Cconfig()
{
	Init();
}

Cconfig::~Cconfig()
{
	Init();
}

void Cconfig::Init()
{
	ZeroMemory(m_ColName,MAXStrLen);
	m_DType = DType_none;
	m_enumVT.clear();
	elem_num = 0;
}
