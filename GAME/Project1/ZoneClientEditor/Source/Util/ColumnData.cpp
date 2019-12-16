#include "Util/ColumnData.h"
#include <sstream>

IColumnData::IColumnData(U32 maxColumn, const char* dataname)
{
	count = maxColumn;
	entry = new Entry[count];
	dMemset(entry, 0, sizeof(Entry) * count);
	dStrcpy(dataName, sizeof(dataName), dataname);
	current = 0;
}
IColumnData::~IColumnData()
{
	if(entry)
		delete[] entry;
	entry = NULL;
}

// ----------------------------------------------------------------------------
// 设置字段的属性（偏移地址，类型，字段名）
void IColumnData::setField(U32 offset, U32 type, const char* name)
{
	if(current >= count)
		return;
	entry[current].offset = offset;
	entry[current].type = type;
	dStrcpy(entry[current].name, sizeof(entry[current].name), name);
	current++;
}

// ----------------------------------------------------------------------------
// 类变量赋值
bool IColumnData::setData(void* classObj, U32 col, RData& data)
{
	if(col >= count)
	{
		char szMsg[128];
		dSprintf(szMsg, 128, "%s read out of range, col=%d, count=%d", dataName, col, count);
		AssertRelease(0, szMsg);
		return false;
	}
	if(data.m_Type != entry[col].type)
	{
		char szMsg[128];
		dSprintf(szMsg,128, "%s read error, field name: %s", dataName, entry[col].name);
		AssertRelease(0, szMsg);
		return false;
	}
	setValue((void*)((int)classObj + entry[col].offset), data);
	return true;
}

// ----------------------------------------------------------------------------
// 获取字段数据数值
void IColumnData::getData(void* classObj, U32 col, std::string& to)
{
	if(col >= count)
		return;
	getValue(entry[col].type, (void*)((int)classObj + entry[col].offset), to);
}

// ----------------------------------------------------------------------------
// 根据类型给类变量字段赋值
void IColumnData::getValue(U32 type, void* from, std::string& to)
{
	switch(type)
	{
	case DType_string:
		to = *(StringTableEntry*)from;
		break;
	case DType_enum8:
		{
			std::stringstream ss; 
			ss << (int)(*(U8*)from); 
			to = ss.str();
		}
		break;
	case DType_enum16:
		{
			std::stringstream ss; 
			ss << *(U16*)from; 
			to = ss.str();
		}
		break;
	case DType_S8:
		{
			std::stringstream ss; 
			ss << (int)(*(S8*)from); 
			to = ss.str();
		}
		break;
	case DType_S16:
		{
			std::stringstream ss; 
			ss << *(S16*)from; 
			to = ss.str();
		}
		break;
	case DType_S32:
		{
			std::stringstream ss; 
			ss << *(S32*)from; 
			to = ss.str();
		}
		break;
	case DType_U8:
		{
			std::stringstream ss; 
			ss << (int)(*(U8*)from); 
			to = ss.str();
		}
		break;
	case DType_U16:
		{
			std::stringstream ss; 
			ss << *(U16*)from; 
			to = ss.str();
		}
		break;
	case DType_U32:
		{
			std::stringstream ss; 
			ss << *(U32*)from; 
			to = ss.str();
		}
		break;
	case DType_F32:
		{
			std::stringstream ss; 
			ss << *(F32*)from; 
			to = ss.str();
		}
		break;
	case DType_F64:
		{
			std::stringstream ss; 
			ss << *(F64*)from; 
			to = ss.str();
		}
		break;
	default:
		break;
	}
}

// ----------------------------------------------------------------------------
// 从类变量获取数据
void IColumnData::setValue(void* to, RData& from)
{
	switch(from.m_Type)
	{
	case DType_string:
		*(StringTableEntry*)to =  StringTable->insert(from.m_string);
		break;
	case DType_enum8:
		*(U8*)to = from.m_Enum8;
		break;
	case DType_enum16:
		*(U16*)to = from.m_Enum16;
		break;
	case DType_S8:
		*(S8*)to = from.m_S8;
		break;
	case DType_S16:
		*(S16*)to = from.m_S16;
		break;
	case DType_S32:
		*(S32*)to = from.m_S32;
		break;
	case DType_U8:
		*(U8*)to = from.m_U8;
		break;
	case DType_U16:
		*(U16*)to = from.m_U16;
		break;
	case DType_U32:
		*(U32*)to = from.m_U32;
		break;
	case DType_F32:
		*(F32*)to = from.m_F32;
		break;
	case DType_F64:
		*(F64*)to = from.m_F64;
		break;
	default:
		*(U8*)to = 0;
		break;
	}
}