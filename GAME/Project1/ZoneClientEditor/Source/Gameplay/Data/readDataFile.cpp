
#include "readDataFile.h"
#include "core/resManager.h"

U8 CDataFile::m_CurrentColumnIndex = 0;
Vector<U8> CDataFile::m_DataTypes;

CDataFile::CDataFile()
{
	ColumNum	= 0 ;
	RecordNum	= 0 ;
	readstep	= 0 ;
	pfdata		= NULL;
}

//
CDataFile::~CDataFile()
{
	ReadDataClose();
}
//
void  CDataFile::ReadDataInit()
{
	ColumNum	= 0 ;
	RecordNum	= 0 ;
	readstep	= 0 ;
	ReadDataClose();
}

//
void CDataFile::ReadDataClose()
{
	if(pfdata)
	{
		pfdata->close();
		delete pfdata;
		pfdata = NULL;
	}

}
//
bool  CDataFile::readDataFile(const char* name)
{
	m_DataTypes.clear();

	//�ȶ�λ����
	pfdata = ResourceManager->openStream(name);

	if(!pfdata)
		return FALSE;

	char tchr[1024];

	if(pfdata)
	{
		while(1)
		{
			ZeroMemory(tchr,sizeof(tchr));
			//fread(tchr,sizeof(char),1,pfdata);
			pfdata->read(sizeof(char),tchr);
			//
			if(strcmp(tchr,"@")==0)  //�ҵ������ļ��Ŀ�ͷ
			{
				//fread(tchr,sizeof(char),1,pfdata);  //����ǻ����ַ�
				pfdata->read(sizeof(char),tchr);
				break;
			}
		}
		//fread(&ColumNum,sizeof(int),1,pfdata);   //�õ���¼����
		//fread(&ColumNum,sizeof(int),1,pfdata);    //�õ��е�����
		pfdata->read(sizeof(int), &RecordNum);
		pfdata->read(sizeof(int), &ColumNum);
	}
	//
	readstep = RecordNum * ColumNum;
	U8 nDataType = 0;

	for(S32 nColumnIndex = 0; nColumnIndex < ColumNum; nColumnIndex++)
	{
		pfdata->read(sizeof(U8), &nDataType);
		m_DataTypes.push_back(nDataType);
	}

	return TRUE;
}

//�õ����������
bool  CDataFile::GetData(RData& readsct)
{
	/****************************
	��ʽ:
	ͷ�ļ�:
	1���ܼ�¼��(int)
	2�����ֶ���(int)
	�ֶθ�ʽ:
	1���ֶγ���(int)
	2���ֶ���������(int->2,string->1,enum->3,float->4)
	3���ֶ�����(int,string)
	*************************/
	if( (readstep == 0) || (!pfdata))
		return FALSE;
	int isize = 0 ; //���ݵĴ�С
	ZeroMemory(&readsct, sizeof(readsct));
	//
	switch (m_DataTypes[m_CurrentColumnIndex])
	{
	case DType_string: 
		{
			pfdata->read(sizeof(U16), &isize);
			pfdata->read(sizeof(char)*isize, readsct.m_string);
		};break;
	case DType_U8:
		{
			pfdata->read(sizeof(U8), &readsct.m_U8);
		};break;
	case DType_S8:
		{
			pfdata->read(sizeof(S8), &readsct.m_S8);
		};break;
	case DType_U16:
		{
			pfdata->read(sizeof(U16), &readsct.m_U16);
		};break;
	case DType_S16:
		{
			pfdata->read(sizeof(S16), &readsct.m_S16);
		};break;
	case DType_U32:
		{
			pfdata->read(sizeof(U32), &readsct.m_U32);
		};break;
	case DType_S32:
		{
			pfdata->read(sizeof(S32), &readsct.m_S32);
		};break;
	case DType_enum8:
		{
			pfdata->read(sizeof(U8), &readsct.m_Enum8);
		};break;
	case DType_enum16:
		{
			pfdata->read(sizeof(U16), &readsct.m_Enum16);
		};break;
	case DType_F32:
		{
			pfdata->read(sizeof(F32), &readsct.m_F32);
		};break;
	case DType_F64:
		{
			pfdata->read(sizeof(F32), &readsct.m_F64);
		};break;
	}

	readsct.m_Type = m_DataTypes[m_CurrentColumnIndex];
	m_CurrentColumnIndex = (m_CurrentColumnIndex + 1) % ColumNum;
	readstep--;
	return TRUE;
}
