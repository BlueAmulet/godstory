#ifndef __COLUMNDATA_H__
#define __COLUMNDATA_H__

#pragma once
#include "platform/platform.h"
#include "core/stringTable.h"
#include <string>
#include "Gameplay/Data/readDataFile.h"

#define CalcOffset(x, cls) (int)&(((cls *)0)->x)
class IColumnData
{
public:
	struct Entry
	{
		char type;			// 数据类型（见ConfigStruct.h)
		U32  offset;		// 字段偏移地址
		char name[32];		// 字段名
	};

	IColumnData(U32 maxColumn, const char* dataname);
	~IColumnData();

	void setField(U32 offset, U32 type, const char* name);
	bool setData(void* classObj, U32 col, RData& data);
	void getData(void* classObj, U32 col, std::string& to);

private:
	void getValue(U32 type, void* from, std::string& to);
	void setValue(void* to, RData& from);

private:
	char dataName[32];
	U32	 count;
	U32  current;
	Entry* entry;
};

#endif//__COLUMNDATA_H__