#pragma once
#include "core/fileStream.h"
#include "core/tVector.h"
#include "DatBuilder/DatBuilder.h"

class CDataFile
{
public:
	int  RecordNum;  //��¼����
	int  ColumNum;   //������

	CDataFile();
	~CDataFile();
	//
	bool readDataFile   (const char* name);    //��ȡdata�ļ�
	bool GetData        (RData&);              //�õ����������
	//
	void ReadDataInit   ();
	void ReadDataClose  ();
private:
	Stream*   pfdata;
	int       readstep;   //���ƶ����ܵ�����
	//

public:
	static Vector<U8> m_DataTypes;
	static U8	m_CurrentColumnIndex;
};