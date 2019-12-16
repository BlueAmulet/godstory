



//********************
//create by zl 09.2.27
//********************

#include "DatBuilder.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include "Shlwapi.h"

using namespace std;

COperateData::COperateData()
{
	setlocale(LC_ALL, ""); 
	m_hWnd = NULL;
	m_ConfigList.clear()   ;
	m_IexFlieList.clear()  ;    //iex�б�
	m_IniFlieList.clear()  ;    //int�б�
	m_DataFlieList.clear() ;   //Data�б�
	//
	ZeroMemory(m_Path, sizeof(m_Path));             //�����ļ���·��
	ZeroMemory(m_FileName, sizeof(m_FileName));     //Ҫ���ɵ��ļ���
	ZeroMemory(m_FileName2, sizeof(m_FileName2));   //
	//
	ReadDataInit();
}
//
COperateData::~COperateData()
{
	ReadDataClose();
}
//
string   COperateData::GetFileTitleName(const char* name)
{
  const	char* pos = (char*)strrchr(name,'.');

	char  tchar[MAX_FILE_PATH];
	ZeroMemory(tchar, sizeof(tchar));
	if(!pos)
		return 0;
	//

	strncpy(tchar, name, strlen(name)-strlen(pos));
	string  result;
	//

	if( strlen(pos) == 0)
		return result;
	//
	if(pos)
	{
		result = tchar;
	}
	return result;

}
//�õ��ļ�����չ��
string  COperateData::GetFileExtenName(const char* name) 
{
	const char* pos = (char*)strrchr(name, '.');
	if(!pos)
		return 0;
	//
	string  result;
	//
	++pos;

	if( strlen(pos) == 0)
		return result;
	//
	if(pos)
	{
		result = pos;
	}
	return result;
}
//�������е��ļ�
void  COperateData::RansackFile(const char* lpPath)   
{
	//
	string  tStr;
	FCheck  tchk;
	char    tchar[max_strLen];
	char    szFind[MAX_FILE_PATH];
	WIN32_FIND_DATA FindFileData;

	ZeroMemory(szFind, sizeof(szFind));
	strcpy(szFind, lpPath);
	strcat(szFind, "\\*.*");

	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	//
	if (INVALID_HANDLE_VALUE == hFind)
		return;

	m_IexFlieList.clear();
	m_IniFlieList.clear();
	m_DataFlieList.clear();

	//��ǰĿ¼�µ������ļ�
	while(TRUE)
	{

		//��������ļ�Ŀ¼
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{

			if((strlen(FindFileData.cFileName) == 0) || (FindFileData.cFileName[0] == '.'))
			{
				if(!FindNextFile(hFind, &FindFileData))     
					break;
				continue;
			}
			//
			tStr = GetFileExtenName(FindFileData.cFileName);
			//iex�ļ�
			if( _stricmp(tStr.c_str(),"iex") ==0 )
			{
				ZeroMemory(tchar,sizeof(tchar));
				sprintf(tchar, "%s\\%s", lpPath, FindFileData.cFileName);
				tchk.path = tchar;
				tchk.title = GetFileTitleName(FindFileData.cFileName);	
				m_IexFlieList.push_back(tchk);
			}
			//ini�ļ�
			if( _stricmp(tStr.c_str(),"ini") ==0 )
			{
				ZeroMemory(tchar,sizeof(tchar));
				sprintf(tchar, "%s\\%s", lpPath, FindFileData.cFileName);
				tchk.path = tchar;
				tchk.title = GetFileTitleName(FindFileData.cFileName);		
				m_IniFlieList.push_back(tchk);
			}
			//data�ļ�
			if( _stricmp(tStr.c_str(),"dat") ==0 )
			{
				ZeroMemory(tchar,sizeof(tchar));
				sprintf(tchar, "%s\\%s", lpPath, FindFileData.cFileName);
				tchk.path = tchar;
				tchk.title = GetFileTitleName(FindFileData.cFileName);		
				m_DataFlieList.push_back(tchk);
			}
		}

		if(!FindNextFile(hFind, &FindFileData))     
			break;
	}
	::FindClose(hFind);

}
//���Ddata�ļ�
void  COperateData::BuildData(const char* fiex)     
{
	//*****************************
	//��ȡ�������ļ�ʱһ��Ҫ�ϸ������ݵĴ�С���ж�ȡ
	//��ȡ��д��һ��Ҫһһ��Ӧ
	//******************************
	char tfilepath[MAX_FILE_PATH];
	Cconfig tcfg;
	char tchr[max_strLen];
	char tchr2[256];
	U16  strSize = 0;
	int  tIndex  = 0;
	S8 tempS8 = 0;
	S16 tempS16 = 0;
	S32 tempS32 = 0;
	S8 tempU8 = 0;
	S16 tempU16 = 0;
	S32 tempU32 = 0;
	F32 tempF32 = 0.0;
	F32 tempF64 = 0.0;

	ZeroMemory(tfilepath, sizeof(tfilepath));
	strcpy(tfilepath, m_Path);
	strcat(tfilepath, m_FileName);
	bool benum = false;  //���enum�ֶ�ֵ�Ƿ���Ч

	iMainKeyVt.clear();
	sMainKeyVt.clear();
	//
	FILE* fp = fopen(tfilepath, "w+b");  //������data��2�����ļ�

	if(!fp)
	{
		ShowErrorMessage("����data�ļ�ʧ�ܣ�");
		return;
	}	

	//�Ȱ�ini����data�ļ�,��β��@���(��û���ִ�ǰ�����ִ��ĳ���
	vector<Cconfig>::iterator pvt  = m_ConfigList.begin();
	vector<elementData>::iterator pvt2 ;
	if( int( m_ConfigList.size() ) == 0 )
	{
		ShowErrorMessage("û�������ļ������ݣ�");
		return;
	}
	//
	for(;pvt != m_ConfigList.end(); ++ pvt,++tIndex)
	{
		tcfg.Init();
		ZeroMemory(tchr, sizeof(tchr));
		tcfg = *pvt;
		sprintf(tchr, "[%s]\n", tcfg.m_ColName);
		strSize = strlen(tchr);
		//д����
		fwrite(tchr, sizeof(char), strSize, fp);
		//д��������
		switch(tcfg.m_DType)
		{
		case  DType_string: 
			{
				sprintf(tchr, "%s\n","type = string");
			};break;
		case  DType_S8:
			{
				sprintf(tchr, "%s\n","type = S8");
			};break;
		case  DType_S16:
			{
				sprintf(tchr, "%s\n","type = S16");
			};break;
		case  DType_S32:
			{
				sprintf(tchr, "%s\n","type = S32");
			};break;
		case  DType_U8:
			{
				sprintf(tchr, "%s\n","type = U8");
			};break;
		case  DType_U16:
			{
				sprintf(tchr, "%s\n","type = U16");
			};break;
		case  DType_U32:
			{
				sprintf(tchr, "%s\n","type = U32");
			};break;
		case  DType_enum8:
			{
				sprintf(tchr, "%s\n","type = enum8");
			};break;
		case  DType_enum16:
			{
				sprintf(tchr, "%s\n","type = enum16");
			};break;
		case DType_F32:
			{	
				sprintf(tchr, "%s\n","type = F32");
			};break;
		case DType_F64:
			{	
				sprintf(tchr, "%s\n","type = F64");
			};break;
		default:
			break;
		}
		//
		strSize = strlen(tchr);
		fwrite(tchr, sizeof(char), strSize, fp);
		//���������
		if(tIndex == MainKeyIndex)
		{
			sprintf(tchr, "%s\n","mKey = true");
			strSize = strlen(tchr);
			fwrite(tchr, sizeof(char), strSize, fp);
		}
		//дenum��
		if(tcfg.m_DType == DType_enum8 || tcfg.m_DType == DType_enum16)
		{
			ZeroMemory(tchr,sizeof(tchr));
			//
			sprintf(tchr, "body = %d\n", tcfg.elem_num);

			for( pvt2 = tcfg.m_enumVT.begin(); pvt2 != tcfg.m_enumVT.end(); ++ pvt2 )
			{
				ZeroMemory(tchr2, sizeof(tchr2));
				sprintf(tchr2, "%s %d\n", pvt2->elem_name.c_str(), pvt2->elem_value);
				strcat(tchr, tchr2);
			}
			//strcat(tchr,"\n");
			strSize = strlen(tchr);
			fwrite(tchr, sizeof(char), strSize, fp);
		}
	}
	//���д�ϱ��
	ZeroMemory(tchr, sizeof(tchr));
	sprintf(tchr, "%s", "@\n");
	strSize = strlen(tchr);
	fwrite(tchr, sizeof(char), strSize, fp);


	/*************************
	д�е�����
	��ʽ:

	�ļ��ײ�:
	1���ܼ�¼��(int)
	2�����ֶ���(int)
	3�����ֶ����ͣ�ÿ��������һ���ֽ�(sizeof(U8))��ʾ

	�ļ�����:
	1���ֶγ���(U16)��ֻ���ַ�����Ҫ���ȣ�
	2���ֶ�����
	*************************/
	int RecordCount = 0;
	int ColumCount  = 0;
	ifstream fstm(fiex);
	char* bpos; 
	char* epos;
	elementData tedata;
	//
	while(!fstm.eof())
	{
		ZeroMemory(tchr, sizeof(tchr));
		fstm.getline(tchr, sizeof(tchr));
		StrTrim(tchr, " ");
		if(tchr[0] == 9 || strlen(tchr) == 0 )
			break;
		++ RecordCount;
	}
	-- RecordCount;
	fstm.close();

	ColumCount = (int) m_ConfigList.size();
	//
	fwrite(&RecordCount, sizeof(U32), 1, fp);
	fwrite(&ColumCount, sizeof(U32), 1, fp);
	for (pvt = m_ConfigList.begin(); pvt != m_ConfigList.end(); ++pvt)
	{
		tcfg = *pvt;
		fwrite((U8 *)&tcfg.m_DType, sizeof(U8), 1, fp);
	}
	//
	ifstream fstm2(fiex);

	fstm2.getline(tchr,sizeof(tchr));  //��һ�в���д��
	//
	for(int i = 0 ; i < RecordCount ; ++ i )
	{
		ZeroMemory(tchr,sizeof(tchr));
		fstm2.getline(tchr,sizeof(tchr));  
		bpos = tchr;
		//
		pvt = m_ConfigList.begin();
		//
		tIndex = 0;
		bool bUseDefaultValues = false;
		for(;pvt != m_ConfigList.end(); ++ pvt,++tIndex)
		{
			tcfg = *pvt;
			//�õ��ֶ�����
			ZeroMemory(tchr2, sizeof(tchr2));
			epos = strchr(bpos,9);
			//
			if( bpos && epos )
				strncpy(tchr2, bpos, strlen(bpos)-strlen(epos));
			else 
				strcpy(tchr2, bpos);

			switch(tcfg.m_DType)
			{
			case DType_string:
				{
					//checkMainKey
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ����Ϊ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(0,tchr2))
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ������Ψһ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
					
					if (bUseDefaultValues)
					{
						strSize = 0;
					}
					else
					{						
						strSize = strlen(tchr2);
					}
					//д�ֶγ���(ֻ���ַ�������������Ҫд����)
					fwrite(&strSize, sizeof(U16), 1, fp);
					//д�ֶ�����
					fwrite(tchr2, sizeof(char), strSize, fp);
				}
				break;

			case DType_U8:
				{
					if (bUseDefaultValues)
					{
						tempU8 = 0;
					}
					else
					{
						tempS32 = atoi(tchr2);
						if(tempS32 < 0 || tempS32 > 0xFF)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: <%s>�ֶε�ֵԽ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						tempU8 = tempS32;
					}				
					fwrite(&tempU8, sizeof(U8), 1, fp);
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ����Ϊ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(tempU8, ""))
						{
							ZeroMemory(ErrorChr, sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ������Ψһ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
				}
				break;

			case DType_S8:
				{
					if (bUseDefaultValues)
					{
						tempS8 = 0;
					}
					else
					{
						tempS32 = atoi(tchr2);
						if(tempS32 < -0xFF/2 || tempS32 >= 0xFF/2)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: <%s>�ֶε�ֵԽ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						tempS8 = tempS32;
					}		
					fwrite(&tempS8, sizeof(S8), 1, fp);
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ����Ϊ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(tempS8, ""))
						{
							ZeroMemory(ErrorChr, sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ������Ψһ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
				}
				break;

			case DType_U16:
				{
					if (bUseDefaultValues)
					{
						tempU16 = 0;
					}
					else
					{
						tempS32 = atoi(tchr2);
						if(tempS32 < 0 || tempS32 >= 0xFFFF)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: <%s>�ֶε�ֵԽ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						tempU16 = tempS32;
					}
					fwrite(&tempU16, sizeof(U16), 1, fp);
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ����Ϊ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(tempU16, ""))
						{
							ZeroMemory(ErrorChr, sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ������Ψһ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
				}
				break;

			case DType_S16:
				{
					if (bUseDefaultValues)
					{
						tempS16 = 0;
					}
					else
					{
						tempS32 = atoi(tchr2);
						if(tempS32 < -0xFFFF/2 || tempS32 >= 0xFFFF/2)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: <%s>�ֶε�ֵԽ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						tempS16 = tempS32;
					}
					fwrite(&tempS16, sizeof(S16), 1, fp);
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ����Ϊ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(tempS16, ""))
						{
							ZeroMemory(ErrorChr, sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ������Ψһ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
				}
				break;

			case DType_U32:
				{
					if (bUseDefaultValues)
					{
						tempU32 = 0;
					}
					else
					{
						tempU32 = atoi(tchr2);
					}
					fwrite(&tempU32, sizeof(U32), 1, fp);
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ����Ϊ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(tempU32, ""))
						{							
							ZeroMemory(ErrorChr, sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ������Ψһ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
				}
				break;

			case DType_S32:
				{
					if (bUseDefaultValues)
					{
						tempS32 = 0;
					}
					else
					{
						tempS32 = atoi(tchr2);
					}
					fwrite(&tempS32, sizeof(S32), 1, fp);
					if(tIndex == MainKeyIndex)
					{
						if (bUseDefaultValues)
						{
							ZeroMemory(ErrorChr,sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ����Ϊ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							return;
						}
						if(CheckmKeyExclusive(tempS32, ""))
						{
							ZeroMemory(ErrorChr, sizeof(ErrorChr));
							sprintf(ErrorChr,"%s�ļ�:\n%d��: ����<%s>�ֶε�ֵ������Ψһ��!", fiex, i+1, tcfg.m_ColName);
							ShowErrorMessage(ErrorChr);
							fclose(fp);
							fstm.close();
							return;
						}
					}
				}
				break;

			case DType_enum8:
				{
					if (bUseDefaultValues)
					{
						ZeroMemory(ErrorChr,sizeof(ErrorChr));
						sprintf(ErrorChr,"%s�ļ�:\n%d��: <%s>�ֶ�ֵΪ��!", fiex, i+1, tcfg.m_ColName);
						ShowErrorMessage(ErrorChr);
						return;
					}
					tempS32 = 0;
					benum = false;
					for(pvt2 = tcfg.m_enumVT.begin(); pvt2 != tcfg.m_enumVT.end(); ++ pvt2,++tempS32)
					{
						tedata = *pvt2;
						if(_stricmp(tedata.elem_name.c_str(), tchr2) == 0 )
						{
							benum = true;
							break;
						}
					}
					//
					if(!benum)
					{
						ZeroMemory(ErrorChr, sizeof(ErrorChr));
						sprintf(ErrorChr,"%s�ļ�:\n%d��: <%s>�ֶ���������Ч��enumֵ!", fiex, i+1, tcfg.m_ColName);
						ShowErrorMessage(ErrorChr);
						fclose(fp);
						fstm.close();
						return;
					}
					//
					fwrite(&tcfg.m_enumVT[tempS32].elem_value, sizeof(U8), 1, fp);
				}
				break;

			case DType_enum16:
				{
					if (bUseDefaultValues)
					{
						ZeroMemory(ErrorChr,sizeof(ErrorChr));
						sprintf(ErrorChr,"%s�ļ�:\n%d��: <%s>�ֶ�ֵΪ��!", fiex, i+1, tcfg.m_ColName);
						ShowErrorMessage(ErrorChr);
						return;
					}
					tempS32 = 0;
					benum = false;
					for(pvt2 = tcfg.m_enumVT.begin(); pvt2 != tcfg.m_enumVT.end(); ++ pvt2,++tempS32)
					{
						tedata = *pvt2;
						if(_stricmp(tedata.elem_name.c_str(), tchr2) == 0 )
						{
							benum = true;
							break;
						}
					}
					//
					if(!benum)
					{
						ZeroMemory(ErrorChr, sizeof(ErrorChr));
						sprintf(ErrorChr,"%s�ļ�:\n%d��: <%s>�ֶ���������Ч��enumֵ!", fiex, i+1, tcfg.m_ColName);
						ShowErrorMessage(ErrorChr);
						fclose(fp);
						fstm.close();
						return;
					}
					//
					fwrite(&tcfg.m_enumVT[tempS32].elem_value, sizeof(U16), 1, fp);
				}
				break;

			case DType_F32:
				{
					if (bUseDefaultValues)
					{
						tempF32 = 0.0;
					}
					else
					{
						tempF32 = (float)atof(tchr2);
					}
					fwrite(&tempF32, sizeof(F32), 1, fp);
				}
				break;

			case DType_F64:
				{
					if (bUseDefaultValues)
					{
						tempF64 = 0.0;
					}
					else
					{
						tempF64 = (float)atof(tchr2);
					}
					fwrite(&tempF64, sizeof(F64), 1, fp);
				}
				break;
			}

			if (bUseDefaultValues)
				continue;

			bpos += strlen(tchr2);
			if(strlen(bpos) == 0 )
			{
				bUseDefaultValues = true;
			}
			else
				++bpos;

			fflush(fp);
		}
	}

	//
	fclose(fp);
	fstm.close();
} 

//����EXL��INI
void  COperateData::OutExlINi()                  
{
	char FilePath[MAX_FILE_PATH]; //��ǰ����·��
	char temp[256];
	char FiexPath[MAX_FILE_PATH], FiniPath[MAX_FILE_PATH];//FileExName[MAX_FILE_PATH],; //�ļ�����չ��,iex�ļ���·��,ini�ļ���·��
	char* pos ;
	bool  biex = false, bini=false;
	vector<FCheck>::iterator pvt,pvt2;
	FCheck tfchk;

	//
	ZeroMemory(FilePath, MAX_FILE_PATH);
	ZeroMemory(temp, MAX_FILE_PATH);
	ZeroMemory(FiexPath, MAX_FILE_PATH);
	ZeroMemory(FiniPath, MAX_FILE_PATH);
	ZeroMemory(m_Path, sizeof(m_Path));
	ZeroMemory(m_FileName, sizeof(m_FileName));
	ZeroMemory(m_FileName2, sizeof(m_FileName2));

	//�����ļ���(����·��)
	sprintf(temp, "%s", __argv[0]);
	pos = strrchr(temp, '\\');
	strncpy(FilePath, temp, strlen(temp)-strlen(pos));
	strcpy(m_Path, FilePath); //���ù��õ�·��
	//GetCurrentDirectory(fsize,curPath);


	//��������ļ�
	RansackFile(FilePath);
	//

	//�ж��Ƿ�ȱ���ļ�
	if( m_DataFlieList.size() == 0 )
	{
		ShowErrorMessage("ȱ��dat��ʽ�ļ�!");
		return;
	}

	//
	pvt = m_DataFlieList.begin();
	for(U32 i = 0; i < m_DataFlieList.size(); ++ i )
	{
		try
		{
			sprintf(m_FileName, "\\%s_x.ini", pvt->title.c_str());
			sprintf(m_FileName2, "\\%s_x.iex", pvt->title.c_str());
			ParseData(m_DataFlieList[i].path.c_str());
		}
		catch (...)
		{
			ZeroMemory(ErrorChr, sizeof(ErrorChr));
			sprintf(ErrorChr, "����%s.datʧ�ܣ�", pvt->title.c_str());
			ShowErrorMessage(ErrorChr);
			return;
		}
		++pvt;
	}
}

//����Data�ļ�
void  COperateData::OutData()						
{
	char FilePath[256]; //��ǰ����·��
	char temp[256];
	char FiexPath[256], FiniPath[256];//FileExName[256],; //�ļ�����չ��,iex�ļ���·��,ini�ļ���·��
	char* pos ;
	bool  biex = false, bini=false;
	vector<FCheck>::iterator pvt, pvt2;
	FCheck tfchk;
	//
	ZeroMemory(ErrorChr, sizeof(ErrorChr));
	ZeroMemory(FilePath, 256);
	ZeroMemory(temp, 256);
	ZeroMemory(FiexPath, 256);
	ZeroMemory(FiniPath, 256);
	ZeroMemory(m_Path, sizeof(m_Path));
	ZeroMemory(m_FileName, sizeof(m_FileName));
	ZeroMemory(m_FileName2, sizeof(m_FileName2));

	//�����ļ���(����·��)
	sprintf(temp, "%s", __argv[0]);
	pos = strrchr(temp, '\\');
	strncpy(FilePath, temp, strlen(temp)-strlen(pos));
	strcpy(m_Path, FilePath); //���ù��õ�·��

	//��������ļ�
	RansackFile(FilePath);

	//�ж��Ƿ�ȱ���ļ�
	if((m_IexFlieList.size() == 0) || (m_IniFlieList.size() == 0 )||  (m_IniFlieList.size() != m_IexFlieList.size()) )
	{
		ShowErrorMessage("û���ҵ�����ļ�");
		return;
	}

	pvt = m_IexFlieList.begin();
	//
	for(;pvt != m_IexFlieList.end(); ++ pvt)
	{
		pvt2 = m_IniFlieList.begin();
		biex = FALSE;
		for(;pvt2 != m_IniFlieList.end(); ++ pvt2)
		{

			//
			tfchk = (*pvt2); 
			if(_stricmp(tfchk.title.c_str(), pvt->title.c_str()) ==0)
			{
				biex = TRUE;
				break;
			}
		}
		//
		if(!biex)
		{
			sprintf(ErrorChr, "ȱ��%s.iex�ļ�", pvt->title.c_str());
			ShowErrorMessage(ErrorChr);
			return;
		}
	}
	//
	pvt = m_IniFlieList.begin();
	for(;pvt != m_IniFlieList.end(); ++ pvt)
	{
		pvt2 = m_IexFlieList.begin();
		biex = FALSE;
		for(;pvt2 != m_IexFlieList.end(); ++ pvt2)
		{
			//
			tfchk = (*pvt2); 
			if(_stricmp(tfchk.title.c_str(), pvt->title.c_str())== 0)
			{
				biex = TRUE;
				break;
			}
		}
		//
		if(!biex)
		{
			sprintf(ErrorChr, "ȱ��%s.ini�ļ�", pvt->title.c_str());
			ShowErrorMessage(ErrorChr);
			return;
		}
	}
	//

	//������Ҫ�Ľ�����Ϣ
	for(size_t i = 0 ; i < m_IexFlieList.size() ; ++ i )
	{
		try
		{
			sprintf(m_FileName, "\\%s.dat", m_IniFlieList[i].title.c_str());
			BulidConfigVector(m_IniFlieList[i].path.c_str());
			BuildData(m_IexFlieList[i].path.c_str());
		}
		catch (...)
		{
			//Cstr.Format("����%s.dat�ļ�ʧ�ܣ�",m_IniFlieList[i].title);
			sprintf(ErrorChr, "����%s.dat�ļ�ʧ�ܣ�", m_IniFlieList[i].title.c_str());
			ShowErrorMessage(ErrorChr);
			return;
		}
	}
}
//������Ҫ�Ľ�����Ϣ
void  COperateData::BulidConfigVector(const char* fini) 
{
	ifstream fstm(fini);
	char temp[max_strLen];
	char tstr[256];
	int  e_val = 0, e_num = 0;

	string  tstring;
	Cconfig  tconfig;
	//
	elementData tedata; 
	//
	m_ConfigList.clear();
	MainKeyType  = DType_none;
	MainKeyIndex = -1;
	//
	while(!fstm.eof())
	{
		ZeroMemory(temp, sizeof(temp));
		tconfig.Init();
		//
		fstm.getline(temp, max_strLen);

		if(strcmp(temp,"@") == 0 || strlen(temp) == 0 )  //����Ǵ�data�Ƕ�,����ǽ������
			break;
		//
		char* mpos = strstr(temp,"mKey");
		//
		if(mpos)
		{
			mpos = NULL;
			if(MainKeyIndex >= 0)
			{
				ZeroMemory(ErrorChr, sizeof(ErrorChr));
				sprintf(ErrorChr, "%s�ļ���ֻ����һ������", fini);
				ShowErrorMessage(ErrorChr);
				throw;
				return;
			}
			else
			{
				MainKeyIndex = m_ConfigList.size() - 1 ;  
				if( MainKeyIndex >= 0)
					MainKeyType = m_ConfigList[MainKeyIndex].m_DType;   //�õ�����������
				continue;
			}

		}

		//���н���
		char* bpos = strchr(temp, '[');
		char* epos = strchr(temp, ']');

		if(bpos && epos) //����
		{
			++ bpos;
			strncpy(tconfig.m_ColName, bpos, strlen(bpos)-strlen(epos));
		} 

		//���е���������
		fstm.getline(temp, max_strLen);

		//
		bpos = strstr(temp, "type");    //����


		if(bpos)
		{
			bpos = strchr(temp, '=');
			++ bpos;
			StrTrim(bpos, " ");

			ZeroMemory(tstr, sizeof(tstr));
			strcpy(tstr, bpos);
			if(_stricmp(tstr, "string")==0)
			{
				tconfig.m_DType = DType_string;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"S8")==0)
			{
				tconfig.m_DType = DType_S8;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"S16")==0)
			{
				tconfig.m_DType = DType_S16;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"S32")==0)
			{
				tconfig.m_DType = DType_S32;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"U8") == 0 )
			{
				tconfig.m_DType = DType_U8;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"U16") == 0 )
			{
				tconfig.m_DType = DType_U16;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"U32") == 0 )
			{
				tconfig.m_DType = DType_U32;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"F32") == 0 )
			{
				tconfig.m_DType = DType_F32;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if(_stricmp(tstr,"F64") == 0 )
			{
				tconfig.m_DType = DType_F64;
				m_ConfigList.push_back(tconfig);
				continue;
			}
			else if (_stricmp(tstr,"enum8") == 0)
				tconfig.m_DType = DType_enum8;
			else if (_stricmp(tstr,"enum16") == 0)
				tconfig.m_DType = DType_enum16;
		}

		//�����enum���͵Ļ�,�ͻ���enum��
		ZeroMemory(temp, sizeof(temp));
		fstm.getline(temp, max_strLen);
		epos = strstr(temp, "body");
		//
		if(epos)   
		{
			// �õ�ö������Ԫ�ص�����
			epos = strchr(temp, '=');
			++ epos;            
			StrTrim(epos, " ");
			e_num = atoi(epos);
			//
			tconfig.elem_num = e_num;
			for(int i = 0 ; i< e_num ; ++ i )
			{
				ZeroMemory(temp, sizeof(temp));
				fstm.getline(temp, max_strLen); 
				bpos = temp;
				epos = strchr(temp,' ');
				//�ַ���ֵ
				ZeroMemory(tstr, sizeof(tstr));
				strncpy(tstr, temp, strlen(bpos)-strlen(epos));
				tedata.elem_name  = tstr;
				//�߼�ֵ
				e_val = atoi(epos);
				tedata.elem_value = e_val; 
				tconfig.m_enumVT.push_back(tedata);
			}
			m_ConfigList.push_back(tconfig);
		}
		//

	}
	//
	fstm.close();
}
//
void  COperateData::ParseData(const char*  filename)
{
	BulidConfigVector(filename);

	vector<Cconfig>::iterator pvt  = m_ConfigList.begin();
	vector<elementData>::iterator  pvt2 ;
	char tfilepath[256];
	Cconfig tcfg;
	char tchr[max_strLen];
	char tchr2[256];
	int  strSize = 0;
	int  tIndex  = 0;

	//�����������ļ�
	ZeroMemory(tfilepath, sizeof(tfilepath));
	strcpy(tfilepath, m_Path);
	strcat(tfilepath, m_FileName);

	FILE* fp = fopen(tfilepath, "w+t"); 
	if(!fp)
	{
		ShowErrorMessage("���ļ�ʧ�ܣ�");
		return;
	}


	if( int( m_ConfigList.size() ) == 0 )
	{
		ShowErrorMessage("û�������ļ������ݣ�");
		return;
	}

	//
	for(;pvt != m_ConfigList.end(); ++ pvt,++ tIndex)
	{
		tcfg.Init();
		ZeroMemory(tchr, sizeof(tchr));
		tcfg = *pvt;
		sprintf(tchr,"[%s]\n", tcfg.m_ColName);
		strSize = strlen(tchr);
		//д����
		fwrite(tchr, sizeof(char), strSize, fp);
		//д��������
		switch(tcfg.m_DType)
		{
		case  DType_string: 
			{
				sprintf(tchr,"%s\n","type = string");
			};break;
		case  DType_S8:
			{
				sprintf(tchr,"%s\n","type = S8");
			};break;
		case  DType_S16:
			{
				sprintf(tchr,"%s\n","type = S16");
			};break;
		case  DType_S32:
			{
				sprintf(tchr,"%s\n","type = S32");
			};break;
		case  DType_U8:
			{
				sprintf(tchr,"%s\n","type = U8");
			};break;
		case  DType_U16:
			{
				sprintf(tchr,"%s\n","type = U16");
			};break;
		case  DType_U32:
			{
				sprintf(tchr,"%s\n","type = U32");
			};break;
		case  DType_enum8:
			{
				sprintf(tchr,"%s\n","type = enum8");
			};break;
		case  DType_enum16:
			{
				sprintf(tchr,"%s\n","type = enum16");
			};break;
		case DType_F32:
			{
				sprintf(tchr,"%s\n","type = F32");
			};break;
		case DType_F64:
			{
				sprintf(tchr,"%s\n","type = F64");
			};break;
		default:
			break;
		}
		//
		strSize = strlen(tchr);
		fwrite(tchr, sizeof(char), strSize, fp);
		//���������
		if(tIndex == MainKeyIndex)
		{
			sprintf(tchr, "%s\n", "mKey = true");
			strSize = strlen(tchr);
			fwrite(tchr, sizeof(char), strSize, fp);
		}

		//дenum��
		if(tcfg.m_DType == DType_enum8 || tcfg.m_DType == DType_enum16)
		{
			ZeroMemory(tchr, sizeof(tchr));
			//
			sprintf(tchr, "body = %d\n", tcfg.elem_num);

			for( pvt2 = tcfg.m_enumVT.begin(); pvt2 != tcfg.m_enumVT.end(); ++ pvt2 )
			{
				ZeroMemory(tchr2, sizeof(tchr2));
				sprintf(tchr2, "%s %d\n", pvt2->elem_name.c_str(), pvt2->elem_value);
				strcat(tchr, tchr2);
			}
			//strcat(tchr,"\n");
			strSize = strlen(tchr);
			fwrite(tchr, sizeof(char), strSize, fp);
		}
	}
	//
	fclose(fp);

	//������iex�ļ�
	ZeroMemory(tfilepath, sizeof(tfilepath));
	strcpy(tfilepath, m_Path);
	strcat(tfilepath, m_FileName2);

	fp = fopen(tfilepath, "w+t");
	//�ȶ�λ����
	FILE* fp2 = fopen(filename, "rb");

	if(!fp || !fp2)
	{
		ShowErrorMessage("���ļ�ʧ�ܣ�");
		return;
	}

	int RecordCount = 0;
	int ColumCount  = 0;
	int temp        = 0;
	S8 fieldS8      = 0;
	S16 fieldS16    = 0;
	S32 fieldS32    = 0;
	U8 fieldU8      = 0;
	U16 fieldU16    = 0;
	U32 fieldU32    = 0;
	F32 fieldF32	= 0.0f;
	F64 fieldF64	= 0.0f;

	if(fp2)
	{
		while(1)
		{
			ZeroMemory(tchr, sizeof(tchr));
			fread(tchr, sizeof(char), 1, fp2);
			if(strcmp(tchr, "@")==0)  //�ҵ������ļ��Ŀ�ͷ
			{
				fread(tchr, sizeof(char), 1, fp2);  //����ǻ����ַ�
				break;
			}
		}
		fread(&RecordCount, sizeof(int), 1, fp2);
		fread(&ColumCount, sizeof(int), 1, fp2);
	}

	fseek(fp2, ColumCount, SEEK_CUR);

	//д��ͷ
	pvt = m_ConfigList.begin();
	ZeroMemory(tchr, sizeof(tchr));
	//
	temp = 1;
	for(;pvt != m_ConfigList.end(); ++ pvt)
	{
		tcfg = *pvt;
		strcat(tchr, tcfg.m_ColName);
		if(temp < ColumCount)
			tchr[strlen(tchr)] = 9;
		++ temp;
	}
	strcat(tchr, "\n");
	fwrite(tchr, sizeof(char), strlen(tchr), fp);
	//д��������
	/*************************
	д�е�����
	��ʽ:
	ͷ�ļ�:
	1���ܼ�¼��(int)
	2�����ֶ���(int)
	

	�ֶθ�ʽ:
	1���ֶγ���(int)
	2���ֶ���������(int->2,string->1,enum->3)
	3���ֶ�����(int,string)
	*************************/
	for(int i =0 ; i < RecordCount; ++ i )
	{
		pvt = m_ConfigList.begin();
		ZeroMemory(tchr,sizeof(tchr));
		temp = 1;
		for(;pvt != m_ConfigList.end(); ++pvt, ++temp)
		{
			ZeroMemory(tchr2, sizeof(tchr2));
			tcfg =  (*pvt);

			switch (tcfg.m_DType)
			{
			case DType_string: 
				{
					fread(&strSize, sizeof(U16), 1, fp2);
					fread(tchr2, sizeof(char), strSize, fp2);
				};break;
			case DType_S8:
				{
					fread(&fieldS8, sizeof(S8), 1, fp2);
					sprintf(tchr2, "%d", fieldS8);
				};break;
			case DType_S16:
				{
					fread(&fieldS16, sizeof(S16), 1, fp2);
					sprintf(tchr2, "%d", fieldS16);
				};break;
			case DType_S32:
				{
					fread(&fieldS32, sizeof(S32), 1, fp2);
					sprintf(tchr2, "%d", fieldS32);
				};break;
			case DType_U8:
				{
					fread(&fieldU8, sizeof(U8), 1, fp2);
					sprintf(tchr2, "%u", fieldU8);
				};break;
			case DType_U16:
				{
					fread(&fieldU16, sizeof(U16), 1, fp2);
					sprintf(tchr2, "%u", fieldU16);
				};break;
			case DType_U32:
				{
					fread(&fieldU32, sizeof(U32), 1, fp2);
					sprintf(tchr2, "%u", fieldU32);
				};break;
			case DType_enum8:
				{
					fread(&fieldU8, sizeof(U8), 1, fp2);
					pvt2 = tcfg.m_enumVT.begin();
	
					for(; pvt2 != tcfg.m_enumVT.end(); ++ pvt2)
					{
						
						if(fieldU8 == pvt2->elem_value)
						{
							sprintf(tchr2,"%s",pvt2->elem_name.c_str());
							break;
						}
					}
				};break;
			case DType_enum16:
				{
					fread(&fieldU16, sizeof(U16), 1, fp2);
					pvt2 = tcfg.m_enumVT.begin();

					for(; pvt2 != tcfg.m_enumVT.end(); ++ pvt2)
					{
						if(fieldU16 == pvt2->elem_value)
						{
							sprintf(tchr2, "%s", pvt2->elem_name.c_str());
							break;
						}
					}
				};break;
			case DType_F32:
				{
					fread(&fieldF32, sizeof(F32), 1, fp2);
					sprintf(tchr2, "%f", fieldF32);
				};break;
			case DType_F64:
				{
					fread(&fieldF64, sizeof(F64), 1, fp2);
					sprintf(tchr2, "%lf", fieldF64);
				};break;
			}
			strcat(tchr, tchr2);
			if(temp < ColumCount)
				tchr[strlen(tchr)] = 9;	
		}
		strcat(tchr, "\n");
		fwrite(tchr, sizeof(char), strlen(tchr), fp);
	}
	//
	fclose(fp);
	fclose(fp2);
}
//**************************************************************
//��ȡdata��
//������
//**************************************************************
void  COperateData::ReadDataInit()
{
	RecordNum = 0 ;
	ColumNum  = 0 ;
	readstep  = 0 ;
	pfdata = NULL;
}

void  COperateData::ReadDataClose()
{

	if(pfdata)
	{
		fclose(pfdata);
		pfdata = NULL;
	}
}

//��ȡdata�ļ�
bool  COperateData::readDataFile(const char* name)
{
	//�ȶ�λ����
	pfdata = fopen(name, "rb");
	if(!pfdata)
		return FALSE;

	char tchr[max_strLen];

	if(pfdata)
	{
		while(1)
		{
			ZeroMemory(tchr, sizeof(tchr));
			fread(tchr, sizeof(char), 1, pfdata);
			if(strcmp(tchr, "@")==0)  //�ҵ������ļ��Ŀ�ͷ
			{
				fread(tchr, sizeof(char), 1, pfdata);  //����ǻ����ַ�
				break;
			}
		}

		fread(&RecordNum, sizeof(int), 1, pfdata);   //�õ���¼����
		fread(&ColumNum, sizeof(int), 1, pfdata);    //�õ��е�����
	}
	//
	readstep = RecordNum * ColumNum;  
	return TRUE;
}

//�õ����������
bool  COperateData::GetData(RData& readsct)
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

	int iSize = 0 ; //�ַ����������ݵĳ���

	ZeroMemory(&readsct, sizeof(readsct));
	fread(&readsct.m_Type, sizeof(U8), 1, pfdata);  //�ֶ���������
	//
	switch (readsct.m_Type)
	{
	case DType_string: 
		{
			fread(&iSize, sizeof(U16), 1, pfdata); 
			fread(readsct.m_string, sizeof(char), iSize, pfdata);
		};break;

	case DType_S8:
	case DType_U8:
	case DType_enum8:
		{
			fread(&readsct.m_S8, sizeof(S8), 1, pfdata);
		};break;

	case DType_S16:
	case DType_U16:
	case DType_enum16:
		{
			fread(&readsct.m_S16, sizeof(S16), 1, pfdata);
		};break;

	case DType_S32:
	case DType_U32:
		{
			fread(&readsct.m_S32, sizeof(S32), 1, pfdata);
		};break;

	case DType_F32:
		{
			fread(&readsct.m_F32, sizeof(F32), 1, pfdata);			
		};break;

	case DType_F64:
		{
			fread(&readsct.m_F64, sizeof(F64), 1, pfdata);			
		};break;
	}	
	//	
	readstep --;
	//
	return TRUE;
}
//
void  COperateData::ShowErrorMessage(const char* chr)  //��ʾ������Ϣ
{
	if(m_hWnd)
	{
		MessageBox(m_hWnd, chr, "DatBuilder������Ϣ", MB_OK);
	}
}

//���������Ψһ��
bool  COperateData::CheckmKeyExclusive(S32 itemp, string stemp)           
{	
	//
	if(MainKeyIndex == -1 || MainKeyType == DType_none) //û���������ļ�
		return false;

	vector<int>::iterator pint;
	vector<string>::iterator pstr;
	bool result = false;

	switch(MainKeyType)
	{
	case DType_S8:
	case DType_S16:
	case DType_U32:
	case DType_U8:
	case DType_U16:
	case DType_S32:
		{
			pint = iMainKeyVt.begin();
			for(; pint != iMainKeyVt.end(); ++pint)
			{
				if((*pint) == itemp)
				{
					result = true;
					break;
				}
			}
			if(!result)
				iMainKeyVt.push_back(itemp);
		}
		break;

	case DType_string:
		{
			//pstr = find(sMainKeyVt.end(),sMainKeyVt.end(),itemp);
			pstr = sMainKeyVt.begin();
			for(; pstr != sMainKeyVt.end(); ++ pstr)
			{
				if(_stricmp(pstr->c_str(), stemp.c_str()) == 0)
				{
					result = true;
					break;
				}
			}
			if(!result)
				sMainKeyVt.push_back(stemp);
		}
		break;
	default:
		break;
	}

	return result;
}
