#pragma once
#include "core/fileStream.h"
#include "core/tVector.h"
#include "DatBuilder/DatBuilder.h"

class CDataFile
{
public:
	int  RecordNum;  //记录数量
	int  ColumNum;   //列数量

	CDataFile();
	~CDataFile();
	//
	bool readDataFile   (const char* name);    //读取data文件
	bool GetData        (RData&);              //得到具体的内容
	//
	void ReadDataInit   ();
	void ReadDataClose  ();
private:
	Stream*   pfdata;
	int       readstep;   //控制读的总的数量
	//

public:
	static Vector<U8> m_DataTypes;
	static U8	m_CurrentColumnIndex;
};